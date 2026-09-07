/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_awb_gain.h"
static unsigned char p[T41_AWB_GAIN_PARAM_BYTES], s[T41_AWB_GAIN_STATE_BYTES];
static unsigned char report[T41_AWB_GAIN_REPORT_BYTES], saved[sizeof(s)];
int main(void)
{
	unsigned int gains[2] = {1234, 0xffffffffU}, words[2], enable;
	t41_awb_gain_pack(gains, words);
	assert(words[0] == 0x040004d2 && words[1] == 0x04003fff);
	gains[0] = 0x4000; gains[1] = 5432;
	t41_awb_gain_pack(gains, words);
	assert(words[0] == 0x04003fff && words[1] == (0x04000000U | 5432));
	t41_awb_gain_put16(p+0xcd2,16); t41_awb_gain_put16(p+0xcd4,8);
	t41_ae_put32(p+0x14,300); t41_ae_put32(p+0x18,500);
	t41_ae_put32(p+0x30,1024); t41_ae_put32(p+0x34,1024);
	t41_ae_put32(p+0x28,5000);
	t41_ae_put32(p+0xc0c,4000); t41_ae_put32(p+0xc10,6500);
	t41_ae_put32(p+0xbf4,1000); t41_ae_put32(p+0xbf8,1100);
	t41_ae_put32(p+0xbfc,1200); t41_ae_put32(p+0xc00,1300);
	t41_ae_put32(p+0xc04,1400); t41_ae_put32(p+0xc08,1500);
	assert(!t41_awb_gain_prepare(p,sizeof(p),s,sizeof(s),report,sizeof(report),words,&enable));
	assert(enable == 1 && words[0] == (0x04000000U | 1376) && words[1] == (0x04000000U | 2276));
	assert(t41_tmo_le32(s+0x351c)==5000 && t41_tmo_le32(s+0x3924)==1200);
	assert(t41_tmo_le16(s+0xeab8)==218 && t41_tmo_le16(s+0xeaba)==131);
	t41_ae_put32(p+0xbfc,1600); t41_ae_put32(p+0x28,5199);
	assert(!t41_awb_gain_prepare(p,sizeof(p),s,sizeof(s),report,sizeof(report),words,&enable));
	assert(t41_tmo_le32(s+0x3924)==1200);
	t41_ae_put32(p+0x28,5200); s[0xeaa1]=1;
	assert(!t41_awb_gain_prepare(p,sizeof(p),s,sizeof(s),report,sizeof(report),words,&enable));
	assert(!enable && t41_tmo_le32(s+0x3924)==1600);
	memcpy(saved,s,sizeof(s)); words[0]=words[1]=99; enable=99;
	t41_ae_put32(p+0x14,0);
	assert(t41_awb_gain_prepare(p,sizeof(p),s,sizeof(s),report,sizeof(report),words,&enable));
	assert(!memcmp(s,saved,sizeof(s)) && words[0]==99 && enable==99);
	t41_ae_put32(p+0x14,300);
	assert(t41_awb_gain_prepare(p,sizeof(p)-1,s,sizeof(s),report,sizeof(report),words,&enable));
	s[0xeaa2]=10; memcpy(saved,s,sizeof(s));
	assert(t41_awb_gain_prepare(p,sizeof(p),s,sizeof(s),report,sizeof(report),words,&enable));
	assert(!memcmp(s,saved,sizeof(s)) && words[0]==99 && enable==99);
	puts("T41 calibrated AWB gain conversion and history: passed");
	return 0;
}
