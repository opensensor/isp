/* Read-only preflight for an installed T41 calibration, not production data.
 * No coefficients, MMIO captures or sensor special cases are compiled here. */
#include <stdio.h>
#include <stdlib.h>
#include "../driver/t41/tx_isp_t41_awb_frame.h"
static struct t41_awb_owned o;
static unsigned char p[T41_AWB_PARAM_BYTES],dma[32768];
int main(int argc,char **argv)
{
    FILE *f;
    struct t41_awb_register words[T41_AWB_FRAME_WORDS];
    struct t41_awb_frame_buffers b;
    unsigned int n,m,rgb[3]={0,0,0},width,height;
    if (argc!=4) { fprintf(stderr,"usage: %s T41_SENSOR_BIN WIDTH HEIGHT\n",argv[0]); return 2; }
    width=strtoul(argv[2],NULL,0); height=strtoul(argv[3],NULL,0);
    f=fopen(argv[1],"rb");
    if (!f) { perror("calibration"); return 2; }
    /* The loader's file header is 64 bytes; params_copy maps the first
     * bank's +0xd18 AWB block to active +0x978 (4836 bytes). */
    if (fseek(f,64+0xd18,SEEK_SET) || fread(p,1,sizeof(p),f)!=sizeof(p)) { fclose(f); return 2; }
    fclose(f);
    printf("AWB precision=%u fraction=%u estimator=%u grid=%ux%u\n",
        t41_tmo_le16(p+0xcd2),t41_tmo_le16(p+0xcd4),t41_tmo_le32(p+0x120),
        t41_tmo_le16(p+0xc72),t41_tmo_le16(p+0xc6e));
    if (t41_awb_cold(&o,p,sizeof(p),width,height)) { puts("cold validation failed"); return 1; }
    if (t41_awb_hardware(&o,1,words,&n)) { puts("hardware validation failed"); return 1; }
    b=t41_awb_buffers(&o);
    if (t41_awb_gain_words(&b,words+n,&m)) { puts("gain validation failed"); return 1; }
    printf("Cold hardware words=%u WB=%u/%u CT=%u\n",n+m,
        words[n].value&0x3fff,words[n+1].value&0x3fff,t41_tmo_le32(o.p+0x28));
    if (t41_awb_yweight_owned(&o)) { puts("EV weight validation failed"); return 1; }
    if (t41_awb_frame(&b,o.red,o.blue,dma,sizeof(dma),rgb,words,&n)) { puts("empty-frame validation failed"); return 1; }
    puts("Installed calibration: native cold/hardware/gain/EV/empty-frame preflight PASS");
    return 0;
}
