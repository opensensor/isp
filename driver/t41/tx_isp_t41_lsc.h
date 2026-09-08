/* SPDX-License-Identifier: MIT */
#ifndef TX_ISP_T41_LSC_H
#define TX_ISP_T41_LSC_H
#include "tx_isp_t41_dpc.h"
#define T41_LSC_PARAM_BYTES 0xd880U
#define T41_LSC_STATE_BYTES 0x6c38U
static inline void t41_lsc_put32(unsigned char *p, unsigned int v)
{ p[0]=v; p[1]=v>>8; p[2]=v>>16; p[3]=v>>24; }
static inline unsigned int t41_lsc_blend(unsigned int a, unsigned int as,
        unsigned int b, unsigned int bs, unsigned int fraction)
{
    unsigned int common=as>bs?as:bs;
    a >>= (common-as)&31; b >>= (common-bs)&31;
    return (a+(((b-a)*fraction)>>12))&65535;
}
static inline void t41_lsc_pack6(unsigned char *out, const unsigned int v[6])
{
    t41_lsc_put32(out,(v[0]&4095)|((v[1]&4095)<<12)|(v[2]<<24));
    t41_lsc_put32(out+4,((v[2]>>8)&15)|((v[3]<<4)&65535)|((v[4]&4095)<<16)|(v[5]<<28));
    t41_lsc_put32(out+8,(v[5]>>4)&255);
}
/* The OEM packs the complete hardware bank, even when the mesh interpolation
 * count is smaller. Unused scratch therefore retains its previous contents. */
static inline int t41_lsc_ct(const unsigned char *p, unsigned int bytes,
        unsigned char *s, unsigned int state_bytes, unsigned int ct, unsigned int force)
{
    unsigned int zone,lo,hi,fraction=0,i,k,count,ring,previous,delta;
    const unsigned char *plane[4];
    if (!p || !s || bytes<T41_LSC_PARAM_BYTES || state_bytes<T41_LSC_STATE_BYTES) return -1;
    count=t41_tmo_le16(p+24); ring=p[26]!=0;
    if (!ring && count>1152) return -1;
    zone=ct<=t41_tmo_le16(p+16)?0:ct<t41_tmo_le16(p+18)?1:
        ct<=t41_tmo_le16(p+20)?2:ct<t41_tmo_le16(p+22)?3:4;
    previous=t41_tmo_le32(s+12); delta=ct>previous?ct-previous:previous-ct;
    if (!force && ((s[16]==zone && !(zone&1)) || delta<=t41_tmo_le32(s+20))) return 1;
    lo=zone/2; hi=lo+(zone&1);
    if (zone&1) {
        unsigned int first=t41_tmo_le16(p+16+(zone-1)*2),last=t41_tmo_le16(p+18+(zone-1)*2);
        if (last<=first || ct<=first || ct>=last) return -1;
        fraction=((ct-first)<<12)/(last-first);
    }
    t41_lsc_put32(s+12,ct); t41_lsc_put32(s+16,zone);
    for (k=0;k<7;++k) {
        unsigned int base=k<4?0x53+k:0x5f+k-4,stride=k<4?4:3;
        unsigned int a=p[base+lo*stride],b=p[base+hi*stride];
        s[0x6c2c+k]=a>b?a:b;
    }
    for (k=0;k<(ring?4U:3U);++k) {
        unsigned int base=ring?0x68+lo*0x1208+k*0x482:0x3680+lo*0x3600+k*0x1200;
        plane[k]=p+base;
        if (zone&1) {
            unsigned int target=ring?0x3628+k*0x800:0x3628+k*0x1200;
            unsigned int step=ring?0x1208:0x3600;
            unsigned int flags=ring?0x53+k:0x5f+k,flagstep=ring?4:3;
            /* RGB rings have 577 samples; the optional IR ring is separate. */
            if (!ring || k!=3 || s[0x6c28])
                for (i=0;i<(ring?577:count*2);++i)
                    t41_dpc_put16(s+target+i*2,t41_lsc_blend(
                        t41_tmo_le16(p+base+i*2),p[flags+lo*flagstep],
                        t41_tmo_le16(p+base+step+i*2),p[flags+hi*flagstep],fraction));
            plane[k]=s+target;
        }
    }
    for (i=0;i<(ring?576U:1152U);++i) {
        unsigned int v[6],offset=i*(ring?2:4);
        for(k=0;k<6;++k) v[k]=t41_tmo_le16(plane[k/2]+offset+(k&1)*2);
        t41_lsc_pack6(s+32+i*12,v);
        if(ring) {
            v[0]=t41_tmo_le16(plane[3]+offset); v[1]=t41_tmo_le16(plane[3]+offset+2);
            v[4]=0; v[5]=0;
            t41_lsc_pack6(s+32+(i+576)*12,v);
        }
    }
    return 0;
}
static inline int t41_lsc_gain(const unsigned char *p, unsigned int bytes,
        unsigned char *s, unsigned int state_bytes, unsigned int gain, unsigned int force,
        unsigned int wdr)
{
    unsigned int previous,delta;
    if(!p || !s || bytes<T41_LSC_PARAM_BYTES || state_bytes<T41_LSC_STATE_BYTES || wdr>1) return -1;
    previous=t41_tmo_le32(s+24); delta=gain>previous?gain-previous:previous-gain;
    if(!force && delta<t41_tmo_le32(s+28)) return 1;
    t41_lsc_put32(s+24,gain);
    s[0x6c29]=t41_dpc_interpolate(p+(wdr?49:27),gain,1);
    s[0x6c2a]=t41_dpc_interpolate(p+(wdr?60:38),gain,1);
    return 0;
}
static inline int t41_lsc_registers(const unsigned char *p, unsigned int bytes,
        const unsigned char *s, unsigned int state_bytes, unsigned int mode,
        struct t41_dpc_word *out,unsigned int capacity)
{
    unsigned int n=0;
    if(!p || !s || !out || bytes<T41_LSC_PARAM_BYTES || state_bytes<T41_LSC_STATE_BYTES || capacity<10) return -1;
#define LSC_EMIT(a,v) do { out[n].address=(a); out[n++].value=(v); } while(0)
    if(mode==0) {
        LSC_EMIT(0x3000,((unsigned int)p[78]<<16&0x7f0000)|(p[79]&127));
        LSC_EMIT(0x3008,((unsigned int)p[76]<<8)|((unsigned int)p[72]<<31)|p[74]|(t41_tmo_le16(p+12)<<16&0x3ff0000));
        LSC_EMIT(0x300c,((unsigned int)p[77]<<8)|((unsigned int)p[73]<<31)|p[75]|(t41_tmo_le16(p+14)<<16&0x3ff0000));
        LSC_EMIT(0x3014,t41_tmo_le16(p+8)|(t41_tmo_le32(p+4)<<16));
        LSC_EMIT(0x3018,t41_tmo_le32(p)&0x7fffff);
        LSC_EMIT(0x3020,((unsigned int)p[71]<<16&0x10000)|(p[26]&1));
    }
    if(mode<2) LSC_EMIT(0x3010,((unsigned int)s[0x6c2a]<<16)|s[0x6c29]);
    if(mode==0 || mode==2) {
        LSC_EMIT(0x3004,((unsigned int)s[0x6c31]<<4&0x30)|((unsigned int)s[0x6c32]<<8&0x300)|
            (s[0x6c30]&3)|((unsigned int)p[80]<<16)|((unsigned int)p[81]<<12&0x3000));
        LSC_EMIT(0x301c,((unsigned int)s[0x6c2d]<<4&0x30)|((unsigned int)s[0x6c2e]<<8&0x300)|
            (s[0x6c2c]&3)|((unsigned int)s[0x6c2f]<<12&0x3000)|((unsigned int)p[82]<<16&0x70000));
    }
    LSC_EMIT(0x3100,1);
#undef LSC_EMIT
    return n;
}
/* Geometry is shared by mesh and ring controls. The OEM accepts a recognized
 * step on either axis; zero divisors are rejected even if the other is valid. */
static inline int t41_lsc_geometry(unsigned char *s,unsigned int state_bytes,
        unsigned int height,unsigned int width,unsigned int sy,unsigned int sx,unsigned int pairs)
{
    unsigned int valid=0,i,rows,columns,mask;
    if(!s || state_bytes<T41_LSC_STATE_BYTES || !sx || !sy || width>8192 || height>8192) return -2;
    for(i=0;i<10;++i) {
        unsigned int step=i<7?16+i*4:i==7?48:i==8?64:80;
        if(sx==step || sy==step) valid=1;
    }
    if(!valid) mask=1;
    else {
        rows=(height+sy-1)/sy+1;
        columns=((width+sx-1)/sx+2)&~1U;
        if(columns!=pairs*2) mask=2;
        else if(rows*pairs>65535) return -2; /* OEM narrows this product and can accept an oversized grid. */
        else if(rows*pairs>1152) mask=4;
        else return 1;
    }
    s[0x6c35]&=mask;
    return -1;
}
/* arg3 flips rows and arg4 columns in the OEM four-argument ABI. The padded
 * last column is not part of the visible grid and must not be mirrored. */
static inline int t41_lsc_flip(unsigned char *p,unsigned int bytes,unsigned char *s,
        unsigned int state_bytes,unsigned int width,unsigned int height,
        unsigned int vertical,unsigned int horizontal,unsigned int bypass)
{
    unsigned int rows,columns,stride,v,h,i,j,k,cy,cx;
    int ret;
    if(!p || !s || bytes<T41_LSC_PARAM_BYTES || state_bytes<T41_LSC_STATE_BYTES ||
        vertical>1 || horizontal>1) return -2;
    ret=t41_lsc_geometry(s,state_bytes,height,width,p[78],p[79],p[80]);
    if(ret<0) { if(ret==-1) s[0x6c35]&=bypass?32:64; return ret; }
    rows=(height+p[78]-1)/p[78]+1; columns=(width+p[79]-1)/p[79]+1;
    stride=(columns+1)&~1U;
    if(rows*stride>2304) return -2;
    v=s[0x6c34]!=vertical; h=s[0x6c33]!=horizontal;
    if(v || h) {
        cy=t41_tmo_le32(p+4); cx=t41_tmo_le32(p+8);
        if(v) cy=0U-cy-height;
        if(h) cx=0U-cx-width;
        t41_lsc_put32(p+4,cy); t41_lsc_put32(p+8,cx);
        t41_lsc_put32(p,cy*cy+cx*cx);
        for(i=0;i<rows;++i) for(j=0;j<columns;++j) {
            unsigned int a=i*stride+j,b=(v?rows-1-i:i)*stride+(h?columns-1-j:j);
            if(a>=b) continue;
            for(k=0;k<9;++k) {
                unsigned char *plane=p+0x3680+k*0x1200;
                unsigned int av=t41_tmo_le16(plane+a*2),bv=t41_tmo_le16(plane+b*2);
                t41_dpc_put16(plane+a*2,bv); t41_dpc_put16(plane+b*2,av);
            }
        }
    }
    t41_lsc_put32(s+16,5);
    ret=t41_lsc_ct(p,bytes,s,state_bytes,t41_tmo_le32(s+12),1);
    if(ret<0) return ret;
    s[0x6c33]=horizontal; s[0x6c34]=vertical;
    return 0;
}
static inline int t41_lsc_initialize(const unsigned char *p,unsigned int bytes,unsigned char *s,
        unsigned int state_bytes,unsigned int width,unsigned int height,unsigned int bayer,unsigned int wdr)
{
    unsigned int i;
    if(!p || !s || bytes<T41_LSC_PARAM_BYTES || state_bytes<T41_LSC_STATE_BYTES ||
        !width || !height || width>8192 || height>8192 || wdr>1 ||
        (!p[26] && (!t41_tmo_le16(p+24) || t41_tmo_le16(p+24)>1152))) return -1;
    for(i=0;i<T41_LSC_STATE_BYTES;++i) s[i]=0;
    t41_lsc_put32(s+4,height); t41_lsc_put32(s+8,width);
    t41_lsc_put32(s+12,5000); t41_lsc_put32(s+16,5); t41_lsc_put32(s+20,16);
    t41_lsc_put32(s+28,256); s[0x6c28]=(bayer&31)>=4;
    s[0x6c29]=255; s[0x6c2a]=255;
    if(t41_lsc_geometry(s,state_bytes,height,width,p[78],p[79],p[80])<0) return -1;
    if(t41_lsc_gain(p,bytes,s,state_bytes,0,1,wdr)<0 || t41_lsc_ct(p,bytes,s,state_bytes,5000,1)<0) return -1;
    return 0;
}
static inline int t41_lsc_refresh(unsigned char *p,unsigned int bytes,unsigned char *s,
        unsigned int state_bytes,unsigned int wdr,unsigned int bypass)
{
    unsigned int v,h;
    if(!p || !s || bytes<T41_LSC_PARAM_BYTES || state_bytes<T41_LSC_STATE_BYTES || wdr>1) return -1;
    v=s[0x6c34]; h=s[0x6c33]; s[0x6c34]=0; s[0x6c33]=0;
    if(t41_lsc_flip(p,bytes,s,state_bytes,t41_tmo_le32(s+8),t41_tmo_le32(s+4),v,h,bypass)<0) return -1;
    if(t41_lsc_gain(p,bytes,s,state_bytes,t41_tmo_le32(s+24),1,wdr)<0 ||
       t41_lsc_ct(p,bytes,s,state_bytes,t41_tmo_le32(s+12),1)<0) return -1;
    return 0;
}
#endif
