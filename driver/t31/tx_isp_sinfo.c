#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/errno.h>

#include "../include/tx_isp/tx_isp_sinfo.h"

static const struct tx_isp_sinfo_config tx_isp_sinfo_config = {
	.client_offset = 0x0d4,
	.attr_offset = 0x268,
	.width_offset = 0x234,
	.height_offset = 0x238,
	.fps_offset = 0x274,
	.adapter_nr_offset = 0x0e0,
	.attr_name_offset = 0,
	.attr_chip_id_offset = 4,
};

#include "../common/tx_isp_sinfo.c"
