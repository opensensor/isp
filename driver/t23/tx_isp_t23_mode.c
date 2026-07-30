/*
 * T23 mode-profile adapter.
 *
 * The flag merge is common across generations; the bypass masks and exact
 * tuning-block refresh order are T23 behavior and deliberately stay here.
 */

#include "../include/tx_isp/tx_isp_callback_plan.h"
#include "../include/tx_isp/tx_isp_reg_profile.h"
#include "include/tx_isp_t23_mode.h"

extern int system_reg_read(unsigned int reg);
extern int system_reg_write(unsigned int reg, unsigned int value);

extern int tiziano_defog_dn_params_refresh(void);
extern int tiziano_ae_dn_params_refresh(void);
extern int tiziano_awb_dn_params_refresh(void);
extern int tiziano_dmsc_dn_params_refresh(void);
extern int tiziano_sharpen_dn_params_refresh(void);
extern void tiziano_mdns_dn_params_refresh(void);
extern int tiziano_sdns_dn_params_refresh(void);
extern int tiziano_gib_dn_params_refresh(void);
extern int tiziano_lsc_dn_params_refresh(void);
extern int tiziano_ccm_dn_params_refresh(void);
extern int tiziano_clm_dn_params_refresh(void);
extern int tiziano_gamma_dn_params_refresh(void);
extern int tiziano_adr_dn_params_refresh(void);
extern int tiziano_dpc_dn_params_refresh(void);
extern int tiziano_af_dn_params_refresh(void);
extern int tiziano_bcsh_dn_params_refresh(void);
extern int tiziano_ydns_dn_params_refresh(void);

#define T23_REFRESH_ADAPTER(name)					\
	static void t23_refresh_##name(void *opaque)			\
	{								\
		(void)opaque;						\
		name();							\
	}

T23_REFRESH_ADAPTER(tiziano_defog_dn_params_refresh)
T23_REFRESH_ADAPTER(tiziano_ae_dn_params_refresh)
T23_REFRESH_ADAPTER(tiziano_awb_dn_params_refresh)
T23_REFRESH_ADAPTER(tiziano_dmsc_dn_params_refresh)
T23_REFRESH_ADAPTER(tiziano_sharpen_dn_params_refresh)
T23_REFRESH_ADAPTER(tiziano_mdns_dn_params_refresh)
T23_REFRESH_ADAPTER(tiziano_sdns_dn_params_refresh)
T23_REFRESH_ADAPTER(tiziano_gib_dn_params_refresh)
T23_REFRESH_ADAPTER(tiziano_lsc_dn_params_refresh)
T23_REFRESH_ADAPTER(tiziano_ccm_dn_params_refresh)
T23_REFRESH_ADAPTER(tiziano_clm_dn_params_refresh)
T23_REFRESH_ADAPTER(tiziano_gamma_dn_params_refresh)
T23_REFRESH_ADAPTER(tiziano_adr_dn_params_refresh)
T23_REFRESH_ADAPTER(tiziano_dpc_dn_params_refresh)
T23_REFRESH_ADAPTER(tiziano_af_dn_params_refresh)
T23_REFRESH_ADAPTER(tiziano_bcsh_dn_params_refresh)
T23_REFRESH_ADAPTER(tiziano_ydns_dn_params_refresh)

#define T23_REFRESH_STEP(name) { t23_refresh_##name }

static const struct tx_isp_callback_step t23_mode_refresh_steps[] = {
	T23_REFRESH_STEP(tiziano_defog_dn_params_refresh),
	T23_REFRESH_STEP(tiziano_ae_dn_params_refresh),
	T23_REFRESH_STEP(tiziano_awb_dn_params_refresh),
	T23_REFRESH_STEP(tiziano_dmsc_dn_params_refresh),
	T23_REFRESH_STEP(tiziano_sharpen_dn_params_refresh),
	T23_REFRESH_STEP(tiziano_mdns_dn_params_refresh),
	T23_REFRESH_STEP(tiziano_sdns_dn_params_refresh),
	T23_REFRESH_STEP(tiziano_gib_dn_params_refresh),
	T23_REFRESH_STEP(tiziano_lsc_dn_params_refresh),
	T23_REFRESH_STEP(tiziano_ccm_dn_params_refresh),
	T23_REFRESH_STEP(tiziano_clm_dn_params_refresh),
	T23_REFRESH_STEP(tiziano_gamma_dn_params_refresh),
	T23_REFRESH_STEP(tiziano_adr_dn_params_refresh),
	T23_REFRESH_STEP(tiziano_dpc_dn_params_refresh),
	T23_REFRESH_STEP(tiziano_af_dn_params_refresh),
	T23_REFRESH_STEP(tiziano_bcsh_dn_params_refresh),
	T23_REFRESH_STEP(tiziano_ydns_dn_params_refresh),
};

static const struct tx_isp_callback_plan t23_mode_refresh_plan = {
	.steps = t23_mode_refresh_steps,
	.count = sizeof(t23_mode_refresh_steps) /
		 sizeof(t23_mode_refresh_steps[0]),
};

void tx_isp_t23_mode_profile_apply(const unsigned int *flags)
{
	unsigned int bypass;

	bypass = tx_isp_reg_flags_merge(system_reg_read(12), flags, 32U);
	system_reg_write(12, (bypass & 0xb577fffdU) | 0x34000009U);

	(void)tx_isp_callback_plan_run(&t23_mode_refresh_plan, (void *)0);
}
