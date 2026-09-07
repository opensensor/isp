/* Read-only offline diagnostic: AE params then AE state on stdin. */
#include <stdio.h>
#include "../driver/t41/tx_isp_t41_ae.h"
int main(void)
{
	unsigned char p[T41_AE_PARAM_BYTES], s[T41_AE_STATE_BYTES];
	unsigned long long knots[15], exposure;
	unsigned short targets[15];
	unsigned int shift, target;
	struct t41_ae_meter meter;
	if (fread(p,1,sizeof(p),stdin) != sizeof(p) ||
	    fread(s,1,sizeof(s),stdin) != sizeof(s)) return 2;
	if (t41_ae_weight_mean(p,sizeof(p),s,sizeof(s),&meter) ||
	    t41_ae_target_tables(p,sizeof(p),knots,targets)) return 3;
	shift = t41_tmo_le16(p+0x6c0);
	exposure = (unsigned long long)t41_tmo_le32(s+0x1dc0) *
		t41_tmo_le32(s+0x1dc4) * t41_tmo_le32(s+0x1dcc) >> shift;
	if (t41_ae_long_target(exposure,knots,targets,shift,&target)) return 4;
	printf("scalar mean=%u foreground=%u background=%u bright=%u dark=%u target=%u\n",
		meter.mean,meter.foreground,meter.background,meter.bright_q,meter.dark_q,target);
	printf("OEM state last_mean=%u target=%u integration=%u again=%u dgain=%u exposure=%llu Q%u\n",
		t41_tmo_le16(s+0x217c),t41_tmo_le16(s+0x217a),t41_tmo_le32(s+0x1dc0),
		t41_tmo_le32(s+0x1dc4),t41_tmo_le32(s+0x1dcc),exposure,shift);
	return 0;
}
