/* T20 adapter for the shared Thingino sensor registry. */

#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/errno.h>

#include "../include/tx_isp/tx_isp_sinfo.h"

/* Offsets are from the T20 3.10.14 tx-isp-sinfo.o instruction stream. */
static const struct tx_isp_sinfo_config tx_isp_sinfo_config = {
	.client_offset = 0x094,
	.attr_offset = 0x1c0,
	.width_offset = 0x190,
	.height_offset = 0x194,
	.fps_offset = 0x1cc,
	.adapter_nr_offset = 0x0e0,
	.attr_name_offset = 0,
	.attr_chip_id_offset = 4,
};

#include "../common/tx_isp_sinfo.c"
