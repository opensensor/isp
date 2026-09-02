#ifndef TX_ISP_SENSOR_FORMAT_H
#define TX_ISP_SENSOR_FORMAT_H

/* Values shared by the Ingenic sensor_csi_fmt ABI. */
#define TX_ISP_SENSOR_CSI_RAW8		0U
#define TX_ISP_SENSOR_CSI_RAW10		1U
#define TX_ISP_SENSOR_CSI_RAW12		2U
#define TX_ISP_SENSOR_CSI_YUV422	7U

static inline unsigned int
tx_isp_sensor_csi_bits_per_pixel(unsigned int format)
{
	switch (format) {
	case TX_ISP_SENSOR_CSI_RAW8:
		return 8U;
	case TX_ISP_SENSOR_CSI_RAW10:
		return 10U;
	case TX_ISP_SENSOR_CSI_RAW12:
		return 12U;
	case TX_ISP_SENSOR_CSI_YUV422:
		return 16U;
	default:
		return 0U;
	}
}

#endif /* TX_ISP_SENSOR_FORMAT_H */
