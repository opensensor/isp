#ifndef TX_ISP_T40_V4L2_H
#define TX_ISP_T40_V4L2_H

struct device;
struct file;

#ifdef CONFIG_TX_ISP_T40_V4L2
int tx_isp_t40_v4l2_init(struct device *parent);
void tx_isp_t40_v4l2_exit(void);
#else
static inline int tx_isp_t40_v4l2_init(struct device *parent)
{
	(void)parent;
	return 0;
}

static inline void tx_isp_t40_v4l2_exit(void)
{
}
#endif

/* Narrow bridge into framechan0's recovered private ABI. */
int tx_isp_t40_v4l2_private_open(struct file *file);
long tx_isp_t40_v4l2_private_ioctl(struct file *file,
				    unsigned int command, void *argument);
int tx_isp_t40_v4l2_private_release(struct file *file);
int tx_isp_t40_v4l2_sensor_dimensions(unsigned int *width,
				       unsigned int *height);
int tx_isp_t40_v4l2_sensor_fps(unsigned int *numerator,
			       unsigned int *denominator);

#endif /* TX_ISP_T40_V4L2_H */
