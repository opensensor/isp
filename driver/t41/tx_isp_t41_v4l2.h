#ifndef TX_ISP_T41_V4L2_H
#define TX_ISP_T41_V4L2_H

struct device;

int tx_isp_t41_v4l2_init(struct device *parent);
void tx_isp_t41_v4l2_exit(void);
void tx_isp_t41_v4l2_bind_channel(void *channel, unsigned int index);
void tx_isp_t41_v4l2_unbind_channel(void *channel);

int tx_isp_t41_frame_channel_claim(void *channel);
int tx_isp_t41_frame_channel_release(void *channel);
int tx_isp_t41_frame_channel_streamoff(void *channel);
int t41_frame_channel_reqbufs_clean(void *channel, void __user *user_req);
int t41_frame_channel_qbuf_clean(void *channel, void __user *user_buf);
int t41_frame_channel_dqbuf_clean(void *channel, void __user *user_buf);
int t41_frame_channel_streamon_clean(void *channel, void __user *user_type);

#endif /* TX_ISP_T41_V4L2_H */
