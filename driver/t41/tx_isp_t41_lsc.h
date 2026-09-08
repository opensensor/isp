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
#endif
