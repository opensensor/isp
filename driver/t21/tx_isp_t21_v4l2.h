#ifndef TX_ISP_T21_V4L2_H
#define TX_ISP_T21_V4L2_H

#include <linux/fs.h>
#include <linux/poll.h>
#include <linux/types.h>

#ifdef TX_ISP_T21_V4L2
struct device;

int tx_isp_t21_v4l2_init(void);
void tx_isp_t21_v4l2_cleanup(void);

struct device *tx_isp_t21_capture_device(void);
int tx_isp_t21_capture_open(unsigned int channel);
void tx_isp_t21_capture_release(unsigned int channel);
long tx_isp_t21_capture_ioctl(unsigned int channel, unsigned int command,
			      void *argument);
unsigned int tx_isp_t21_capture_poll(unsigned int channel, struct file *file,
				     poll_table *wait);
int tx_isp_t21_capture_stream(int enable);
#else
static inline int tx_isp_t21_v4l2_init(void)
{
	return 0;
}

static inline void tx_isp_t21_v4l2_cleanup(void)
{
}
#endif

#endif /* TX_ISP_T21_V4L2_H */
