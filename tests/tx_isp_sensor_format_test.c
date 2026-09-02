#include <stdio.h>

#include "tx_isp/tx_isp_sensor_format.h"

int main(void)
{
	if (tx_isp_sensor_csi_bits_per_pixel(TX_ISP_SENSOR_CSI_RAW8) != 8U ||
	    tx_isp_sensor_csi_bits_per_pixel(TX_ISP_SENSOR_CSI_RAW10) != 10U ||
	    tx_isp_sensor_csi_bits_per_pixel(TX_ISP_SENSOR_CSI_RAW12) != 12U ||
	    tx_isp_sensor_csi_bits_per_pixel(TX_ISP_SENSOR_CSI_YUV422) != 16U ||
	    tx_isp_sensor_csi_bits_per_pixel(3U) != 0U ||
	    tx_isp_sensor_csi_bits_per_pixel(~0U) != 0U) {
		fputs("tx_isp_sensor_format: format mapping failed\n", stderr);
		return 1;
	}

	puts("tx_isp_sensor_format: all tests passed");
	return 0;
}
