#ifndef __TX_ISP_T31_V4L2_H__
#define __TX_ISP_T31_V4L2_H__

#ifdef CONFIG_TX_ISP_T31_V4L2
#define TX_ISP_T31_V4L2_ENABLED 1

int tx_isp_v4l2_init(void);
void tx_isp_v4l2_cleanup(void);
#else
#define TX_ISP_T31_V4L2_ENABLED 0

static inline int tx_isp_v4l2_init(void)
{
	return 0;
}

static inline void tx_isp_v4l2_cleanup(void)
{
}
#endif

#endif
