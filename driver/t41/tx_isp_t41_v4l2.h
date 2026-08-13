#ifndef TX_ISP_T41_V4L2_H
#define TX_ISP_T41_V4L2_H

struct device;
struct file;

#ifdef CONFIG_TX_ISP_T41_V4L2
int tx_isp_t41_v4l2_init(struct device *parent);
void tx_isp_t41_v4l2_exit(void);
void tx_isp_t41_v4l2_bind_channel(void *channel, unsigned int index);
void tx_isp_t41_v4l2_unbind_channel(void *channel);
#else
static inline int tx_isp_t41_v4l2_init(struct device *parent)
{
	(void)parent;
	return 0;
}

static inline void tx_isp_t41_v4l2_exit(void)
{
}

static inline void tx_isp_t41_v4l2_bind_channel(void *channel,
					 unsigned int index)
{
	(void)channel;
	(void)index;
}

static inline void tx_isp_t41_v4l2_unbind_channel(void *channel)
{
	(void)channel;
}
#endif

int tx_isp_t41_frame_channel_claim(void *channel);
int tx_isp_t41_frame_channel_release(void *channel);
int tx_isp_t41_frame_channel_streamoff(void *channel);
int t41_frame_channel_reqbufs_clean(void *channel, void __user *user_req);
int t41_frame_channel_qbuf_clean(void *channel, void __user *user_buf);
int t41_frame_channel_dqbuf_clean(void *channel, void __user *user_buf);
int t41_frame_channel_streamon_clean(void *channel, void __user *user_type);

/* Narrow bridge from the public V4L2 adapter to the recovered aggregate
 * lifecycle.  The adapter owns policy, sensor metadata, and DMA allocation;
 * the recovered unit only supplies access to its private aggregate object. */
int tx_isp_t41_legacy_sensor_present(void);
int tx_isp_t41_legacy_open(struct file *file);
long tx_isp_t41_legacy_ioctl(struct file *file, unsigned int command,
			     void *argument);
int tx_isp_t41_legacy_release(struct file *file);

#endif /* TX_ISP_T41_V4L2_H */
