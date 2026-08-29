#include <linux/module.h>

#include <tx_isp/tx_isp_sinfo.h>
#include "../../../external/ingenic-sdk/3.10.14/isp/t20/tx-isp-videobuf.h"

/*
 * The vendor T20 source only registers the platform driver.  The recovered
 * T21/T23 drivers and the open T31 driver also initialize the shared sensor
 * registry before a sensor module can call the exported tx_isp_sinfo_* ABI.
 * Suppress the vendor module aliases and give T20 the same lifecycle here.
 */
#undef module_init
#undef module_exit
#define module_init(fn)
#define module_exit(fn)

#include "../../../external/ingenic-sdk/3.10.14/isp/t20/tx-isp-device.c"

int __init init_module(void)
{
	int ret;

	ret = tx_isp_init();
	if (ret)
		return ret;

	ret = tx_isp_sinfo_init();
	if (ret) {
		tx_isp_exit();
		return ret;
	}

	ret = frame_channel_dmabuf_resolver_register();
	if (ret) {
		tx_isp_sinfo_exit();
		tx_isp_exit();
	}

	return ret;
}

void __exit cleanup_module(void)
{
	frame_channel_dmabuf_resolver_unregister();
	tx_isp_sinfo_exit();
	tx_isp_exit();
}
