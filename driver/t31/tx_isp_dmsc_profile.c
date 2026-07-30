/*
 * T31 SC2336 DMSC compatibility profile.
 *
 * The reconstructed generic DMSC decoder is useful across T31 sensors, but a
 * small set of interpolated words differs from the shipping SC2336 pipeline.
 * Those differences activate an edge/noise output instead of a normal Bayer
 * demosaic.  These sparse day/night corrections were measured from the
 * preserved shipping module on the same sensor.  They are applied after the
 * generic decoder so the rest of the tuning block remains data-driven.
 */

#include <linux/kernel.h>

#include "../include/tx_isp/tx_isp_reg_profile.h"
#include "include/tx_isp_dmsc_profile.h"

extern void system_reg_write(u32 reg, u32 value);

static const struct tx_isp_reg_value sc2336_dmsc_day_values[] = {
	{ 0x4800, 0x00000000 },
	{ 0x4808, 0x00000064 },
	{ 0x4810, 0x00000002 },
	{ 0x4814, 0x00000002 },
	{ 0x4824, 0x00000002 },
	{ 0x482c, 0x00000002 },
	{ 0x4848, 0x00640064 },
	{ 0x4860, 0x00590059 },
	{ 0x48b8, 0x00c82414 },
	{ 0x4988, 0x0320000a },
	{ 0x498c, 0x01038000 },
	{ 0x4990, 0x00068000 },
	{ 0x4994, 0x00080000 },
	{ 0x4998, 0x00010008 },
};

static const struct tx_isp_reg_value sc2336_dmsc_night_values[] = {
	{ 0x4800, 0x00000000 },
	{ 0x4808, 0x00060064 },
	{ 0x4848, 0x005a005a },
	{ 0x4860, 0x005a005a },
	{ 0x498c, 0x01378000 },
	{ 0x4990, 0x006e8000 },
	{ 0x4994, 0x00080000 },
	{ 0x4998, 0x00010008 },
};

static const struct tx_isp_reg_profile sc2336_dmsc_profiles[] = {
	{
		.values = sc2336_dmsc_day_values,
		.count = sizeof(sc2336_dmsc_day_values) /
			 sizeof(sc2336_dmsc_day_values[0]),
		.commit_reg = 0x499c,
		.commit_value = 1,
		.has_commit = true,
	},
	{
		.values = sc2336_dmsc_night_values,
		.count = sizeof(sc2336_dmsc_night_values) /
			 sizeof(sc2336_dmsc_night_values[0]),
		.commit_reg = 0x499c,
		.commit_value = 1,
		.has_commit = true,
	},
};

static void t31_dmsc_profile_write(void *opaque, u32 reg, u32 value)
{
	(void)opaque;
	system_reg_write(reg, value);
}

int tx_isp_t31_sc2336_dmsc_profile_apply(bool night_mode)
{
	return tx_isp_reg_profile_apply(
		&sc2336_dmsc_profiles[night_mode ? 1 : 0],
		t31_dmsc_profile_write, NULL);
}
