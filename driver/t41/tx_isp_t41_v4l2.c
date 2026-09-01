// SPDX-License-Identifier: GPL-2.0
/* Linux-4.4 V4L2 MMAP adapter for one recovered T41 scaler channel. */

#include <linux/kernel.h>
#include <linux/dma-mapping.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <linux/timekeeping.h>
#include <linux/version.h>
#include <linux/videodev2.h>

#include <asm/uaccess.h>
#include <media/v4l2-device.h>
#include <media/v4l2-fh.h>
#include <media/v4l2-ioctl.h>
#include <media/videobuf2-dma-contig.h>
#include <media/videobuf2-v4l2.h>

#include "../include/tx_isp/tx_isp_frame_format.h"
#include "../include/tx_isp/tx_isp_frame_layout.h"
#include "../include/tx_isp/tx_isp_video_queue.h"
#include "tx_isp_t41_v4l2.h"

#ifndef CONFIG_DMA_SHARED_BUFFER
#error "CONFIG_TX_ISP_T41_V4L2 requires CONFIG_DMA_SHARED_BUFFER"
#endif

#if !defined(CONFIG_VIDEOBUF2_DMA_CONTIG) && \
	!defined(CONFIG_VIDEOBUF2_DMA_CONTIG_MODULE)
#error "CONFIG_TX_ISP_T41_V4L2 requires CONFIG_VIDEOBUF2_DMA_CONTIG"
#endif

#define TX_ISP_T41_V4L2_WIDTH_ALIGN	32U
#define TX_ISP_T41_V4L2_HEIGHT_ALIGN	16U
#define TX_ISP_T41_V4L2_NATIVE_WIDTH	2560U
#define TX_ISP_T41_V4L2_NATIVE_HEIGHT	1440U
#define TX_ISP_T41_V4L2_FPS		25U
#define TX_ISP_T41_V4L2_MIN_BUFFERS	2U
#define TX_ISP_T41_V4L2_MAX_BUFFERS	8U

#define TISP_VIDIOC_REGISTER_SENSOR	0x80645405U
#define TISP_VIDIOC_UNREGISTER_SENSOR	0x80645406U
#define TISP_VIDIOC_S_INPUT		0xc0085404U
#define TISP_VIDIOC_G_INPUT		0x80085403U
#define TISP_VIDIOC_PREPARE_SENSOR	0x80085407U
#define TISP_VIDIOC_FINISH_SENSOR	0x80085408U
#define TISP_VIDIOC_START_SENSOR		0x80085409U
#define TISP_VIDIOC_STOP_SENSOR		0x8008540aU
#define TISP_VIDIOC_ENABLE_SENSOR	0xc008540bU
#define TISP_VIDIOC_DISABLE_SENSOR	0xc008540cU
#define TISP_VIDIOC_SET_MDNS_BUF_INFO	0x800c540fU
#define TISP_VIDIOC_GET_MDNS_BUF_INFO	0x800c5410U

struct tx_isp_t41_i2c_info {
	char type[20];
	s32 address;
	s32 adapter;
};

struct tx_isp_t41_spi_info {
	char modalias[32];
	s32 bus;
};

/* Exact T41 userspace sensor-registration wire image (100 bytes). */
struct tx_isp_t41_sensor_info {
	char name[32];
	s32 control_bus;
	union {
		struct tx_isp_t41_i2c_info i2c;
		struct tx_isp_t41_spi_info spi;
	};
	s32 reset_gpio;
	s32 power_down_gpio;
	s32 power_gpio;
	u16 sensor_id;
	s32 video_interface;
	s32 mclk;
	s32 default_boot;
};

struct tx_isp_t41_initarg {
	s32 enable;
	s32 vinum;
};

struct tx_isp_t41_buf_info {
	u32 vinum;
	u32 paddr;
	u32 size;
};

static bool v4l2_autostart = true;
module_param(v4l2_autostart, bool, 0644);
MODULE_PARM_DESC(v4l2_autostart,
	"start the configured sensor when V4L2 is the first ISP consumer");
/* Keep this zero-valued adapter parameter out of the recovered BSS prefix:
 * later recovered objects retain absolute layout contracts.  The .data tail
 * already has alignment space reserved by the other V4L2 parameters. */
static unsigned int v4l2_channel __attribute__((section(".data")));
module_param(v4l2_channel, uint, 0444);
MODULE_PARM_DESC(v4l2_channel,
	"frame-source channel exported through V4L2 (0 is the native main path)");
static char *v4l2_sensor_name = "os04d10";
module_param(v4l2_sensor_name, charp, 0644);
MODULE_PARM_DESC(v4l2_sensor_name, "sensor driver name used by V4L2 autostart");
static unsigned int v4l2_sensor_i2c_addr = 0x3c;
module_param(v4l2_sensor_i2c_addr, uint, 0644);
MODULE_PARM_DESC(v4l2_sensor_i2c_addr, "7-bit sensor I2C address");
static int v4l2_sensor_i2c_adapter;
module_param(v4l2_sensor_i2c_adapter, int, 0644);
MODULE_PARM_DESC(v4l2_sensor_i2c_adapter, "sensor I2C adapter number");
static int v4l2_sensor_reset_gpio = -1;
module_param(v4l2_sensor_reset_gpio, int, 0644);
MODULE_PARM_DESC(v4l2_sensor_reset_gpio, "sensor reset GPIO (-1 disables)");
static int v4l2_sensor_power_down_gpio = -1;
module_param(v4l2_sensor_power_down_gpio, int, 0644);
MODULE_PARM_DESC(v4l2_sensor_power_down_gpio,
	"sensor power-down GPIO (-1 disables)");
static int v4l2_sensor_power_gpio = -1;
module_param(v4l2_sensor_power_gpio, int, 0644);
MODULE_PARM_DESC(v4l2_sensor_power_gpio, "sensor power GPIO (-1 disables)");
static unsigned int v4l2_sensor_id;
module_param(v4l2_sensor_id, uint, 0644);
MODULE_PARM_DESC(v4l2_sensor_id, "optional sensor instance identifier");
static int v4l2_sensor_video_interface;
module_param(v4l2_sensor_video_interface, int, 0644);
MODULE_PARM_DESC(v4l2_sensor_video_interface, "sensor video input interface");
static int v4l2_sensor_mclk = 1;
module_param(v4l2_sensor_mclk, int, 0644);
MODULE_PARM_DESC(v4l2_sensor_mclk, "sensor master-clock selector");
static int v4l2_sensor_default_boot;
module_param(v4l2_sensor_default_boot, int, 0644);
MODULE_PARM_DESC(v4l2_sensor_default_boot, "sensor default boot mode");

extern int frame_channel_vidioc_set_fmt(void *channel,
					struct v4l2_format *format);

struct tx_isp_t41_v4l2 {
	struct v4l2_device v4l2_dev;
	struct video_device video_dev;
	struct vb2_queue vb2_queue;
	struct mutex ioctl_lock;
	struct mutex channel_lock;
	spinlock_t queue_lock;
	struct tx_isp_video_queue capture_queue;
	struct tx_isp_video_slot slots[TX_ISP_T41_V4L2_MAX_BUFFERS];
	struct vb2_buffer *buffers[TX_ISP_T41_V4L2_MAX_BUFFERS];
	struct v4l2_pix_format format;
	struct task_struct *completion_task;
	void *alloc_ctx;
	struct device *parent;
	void *channel;
	void *owned_channel;
	struct file legacy_file;
	struct tx_isp_t41_sensor_info sensor;
	void *mdns_cpu;
	dma_addr_t mdns_dma;
	u32 mdns_size;
	u32 private_count;
	bool lifecycle_acquired;
	bool lifecycle_owned;
	bool stopping;
	bool registered;
};

static struct tx_isp_t41_v4l2 tx_isp_t41_video;

static int tx_isp_t41_v4l2_legacy_ioctl(struct tx_isp_t41_v4l2 *video,
					unsigned int command, void *argument)
{
	mm_segment_t old_fs = get_fs();
	int ret;

	set_fs(KERNEL_DS);
	ret = tx_isp_t41_legacy_ioctl(&video->legacy_file, command, argument);
	set_fs(old_fs);
	return ret;
}

static void tx_isp_t41_v4l2_fill_sensor(
	struct tx_isp_t41_sensor_info *sensor)
{
	memset(sensor, 0, sizeof(*sensor));
	strlcpy(sensor->name, v4l2_sensor_name, sizeof(sensor->name));
	sensor->control_bus = 1;
	strlcpy(sensor->i2c.type, v4l2_sensor_name,
		sizeof(sensor->i2c.type));
	sensor->i2c.address = v4l2_sensor_i2c_addr;
	sensor->i2c.adapter = v4l2_sensor_i2c_adapter;
	sensor->reset_gpio = v4l2_sensor_reset_gpio;
	sensor->power_down_gpio = v4l2_sensor_power_down_gpio;
	sensor->power_gpio = v4l2_sensor_power_gpio;
	sensor->sensor_id = v4l2_sensor_id;
	sensor->video_interface = v4l2_sensor_video_interface;
	sensor->mclk = v4l2_sensor_mclk;
	sensor->default_boot = v4l2_sensor_default_boot;
}

static void tx_isp_t41_v4l2_pipeline_put(struct tx_isp_t41_v4l2 *video)
{
	struct tx_isp_t41_initarg input = { 0, 0 };

	if (!video->lifecycle_acquired)
		return;
	if (video->lifecycle_owned) {
		tx_isp_t41_v4l2_legacy_ioctl(video,
			TISP_VIDIOC_DISABLE_SENSOR, &input);
		tx_isp_t41_v4l2_legacy_ioctl(video,
			TISP_VIDIOC_STOP_SENSOR, &input);
		tx_isp_t41_v4l2_legacy_ioctl(video,
			TISP_VIDIOC_FINISH_SENSOR, &input);
		tx_isp_t41_v4l2_legacy_ioctl(video, TISP_VIDIOC_S_INPUT,
			&input);
		tx_isp_t41_v4l2_legacy_ioctl(video,
			TISP_VIDIOC_UNREGISTER_SENSOR, &video->sensor);
		if (video->mdns_cpu)
			dma_free_coherent(video->parent, video->mdns_size,
				video->mdns_cpu, video->mdns_dma);
		video->mdns_cpu = NULL;
		video->mdns_dma = 0;
		video->mdns_size = 0;
		tx_isp_t41_legacy_release(&video->legacy_file);
		pr_info("tx_isp_t41: V4L2 released standalone ISP lifecycle\n");
	}
	video->lifecycle_owned = false;
	video->lifecycle_acquired = false;
}

static int tx_isp_t41_v4l2_pipeline_get(struct tx_isp_t41_v4l2 *video)
{
	struct tx_isp_t41_initarg input = { 1, 0 };
	struct tx_isp_t41_buf_info mdns = { 0, 0, 0 };
	int ret;

	if (video->lifecycle_acquired)
		return 0;
	if (tx_isp_t41_legacy_sensor_present()) {
		video->lifecycle_acquired = true;
		video->lifecycle_owned = false;
		return 0;
	}
	if (!v4l2_autostart || !v4l2_sensor_name || !*v4l2_sensor_name)
		return -ENODEV;
	if (strlen(v4l2_sensor_name) >= sizeof(video->sensor.i2c.type) ||
	    !v4l2_sensor_i2c_addr || v4l2_sensor_i2c_addr > 0x7fU ||
	    v4l2_sensor_i2c_adapter < 0 || v4l2_sensor_id > U16_MAX ||
	    v4l2_sensor_video_interface < 0 ||
	    v4l2_sensor_video_interface > 2 || v4l2_sensor_mclk < 0 ||
	    v4l2_sensor_mclk > 2)
		return -EINVAL;

	memset(&video->legacy_file, 0, sizeof(video->legacy_file));
	ret = tx_isp_t41_legacy_open(&video->legacy_file);
	if (ret)
		return ret;
	tx_isp_t41_v4l2_fill_sensor(&video->sensor);
	ret = tx_isp_t41_v4l2_legacy_ioctl(video,
		TISP_VIDIOC_REGISTER_SENSOR, &video->sensor);
	if (ret)
		goto fail_open;
	ret = tx_isp_t41_v4l2_legacy_ioctl(video, TISP_VIDIOC_S_INPUT,
		&input);
	if (ret)
		goto fail_sensor;
	ret = tx_isp_t41_v4l2_legacy_ioctl(video,
		TISP_VIDIOC_GET_MDNS_BUF_INFO, &mdns);
	if (ret)
		goto fail_input;
	if (mdns.size) {
		video->mdns_cpu = dma_alloc_coherent(video->parent, mdns.size,
			&video->mdns_dma, GFP_KERNEL);
		if (!video->mdns_cpu) {
			ret = -ENOMEM;
			goto fail_input;
		}
		video->mdns_size = mdns.size;
		if ((u64)video->mdns_dma > U32_MAX) {
			ret = -ERANGE;
			goto fail_dma;
		}
		memset(video->mdns_cpu, 0, mdns.size);
		mdns.paddr = (u32)video->mdns_dma;
		ret = tx_isp_t41_v4l2_legacy_ioctl(video,
			TISP_VIDIOC_SET_MDNS_BUF_INFO, &mdns);
		if (ret)
			goto fail_dma;
	}
	ret = tx_isp_t41_v4l2_legacy_ioctl(video, TISP_VIDIOC_G_INPUT,
		&input);
	if (ret)
		goto fail_dma;
	ret = tx_isp_t41_v4l2_legacy_ioctl(video,
		TISP_VIDIOC_PREPARE_SENSOR, &input);
	if (ret)
		goto fail_dma;
	ret = tx_isp_t41_v4l2_legacy_ioctl(video,
		TISP_VIDIOC_START_SENSOR, &input);
	if (ret)
		goto fail_finish;
	ret = tx_isp_t41_v4l2_legacy_ioctl(video,
		TISP_VIDIOC_ENABLE_SENSOR, &input);
	if (ret)
		goto fail_stop;
	video->lifecycle_owned = true;
	video->lifecycle_acquired = true;
	pr_info("tx_isp_t41: V4L2 started standalone sensor %s at i2c-%d/%#x\n",
		video->sensor.name, video->sensor.i2c.adapter,
		video->sensor.i2c.address);
	return 0;

fail_stop:
	tx_isp_t41_v4l2_legacy_ioctl(video, TISP_VIDIOC_STOP_SENSOR,
		&input);
fail_finish:
	input.enable = 0;
	tx_isp_t41_v4l2_legacy_ioctl(video, TISP_VIDIOC_FINISH_SENSOR,
		&input);
fail_dma:
	if (video->mdns_cpu)
		dma_free_coherent(video->parent, video->mdns_size,
			video->mdns_cpu, video->mdns_dma);
	video->mdns_cpu = NULL;
	video->mdns_dma = 0;
	video->mdns_size = 0;
fail_input:
	input.enable = 0;
	tx_isp_t41_v4l2_legacy_ioctl(video, TISP_VIDIOC_S_INPUT, &input);
fail_sensor:
	tx_isp_t41_v4l2_legacy_ioctl(video, TISP_VIDIOC_UNREGISTER_SENSOR,
		&video->sensor);
fail_open:
	tx_isp_t41_legacy_release(&video->legacy_file);
	return ret;
}

/* The private ABI is a userspace ABI. Linux 4.4 still permits a narrow,
 * synchronous KERNEL_DS bridge; newer-kernel glue will call the same queue
 * core through native kernel operations instead. */
static int tx_isp_t41_v4l2_set_format(void *channel,
				      struct tx_isp_t41_frame_format_wire *format)
{
	mm_segment_t old_fs = get_fs();
	int ret;

	set_fs(KERNEL_DS);
	ret = frame_channel_vidioc_set_fmt(channel,
		(struct v4l2_format __user *)format);
	set_fs(old_fs);
	return ret;
}

static int tx_isp_t41_v4l2_reqbufs(void *channel,
				   struct tx_isp_frame_request_wire *request)
{
	mm_segment_t old_fs = get_fs();
	int ret;

	set_fs(KERNEL_DS);
	ret = t41_frame_channel_reqbufs_clean(channel,
		(void __user *)request);
	set_fs(old_fs);
	return ret;
}

static int tx_isp_t41_v4l2_qbuf(void *channel,
				struct tx_isp_frame_buffer_wire *buffer)
{
	mm_segment_t old_fs = get_fs();
	int ret;

	set_fs(KERNEL_DS);
	ret = t41_frame_channel_qbuf_clean(channel, (void __user *)buffer);
	set_fs(old_fs);
	return ret;
}

static int tx_isp_t41_v4l2_dqbuf(void *channel,
				 struct tx_isp_frame_buffer_wire *buffer)
{
	mm_segment_t old_fs = get_fs();
	int ret;

	set_fs(KERNEL_DS);
	ret = t41_frame_channel_dqbuf_clean(channel, (void __user *)buffer);
	set_fs(old_fs);
	return ret;
}

static int tx_isp_t41_v4l2_streamon(void *channel, u32 type)
{
	mm_segment_t old_fs = get_fs();
	int ret;

	set_fs(KERNEL_DS);
	ret = t41_frame_channel_streamon_clean(channel, (void __user *)&type);
	set_fs(old_fs);
	return ret;
}

static int tx_isp_t41_v4l2_acquire_channel(struct tx_isp_t41_v4l2 *video,
					   unsigned int count)
{
	struct tx_isp_t41_frame_format_wire wire;
	struct tx_isp_frame_request_wire request;
	void *channel;
	int ret;

	mutex_lock(&video->channel_lock);
	if (video->owned_channel) {
		ret = video->private_count == count ? 0 : -EBUSY;
		mutex_unlock(&video->channel_lock);
		return ret;
	}
	mutex_unlock(&video->channel_lock);

	/* A cold module has no frame-channel devices yet.  Sensor selection and
	 * graph activation create them, so lifecycle acquisition must precede the
	 * channel lookup rather than depend on a legacy process doing it first. */
	ret = tx_isp_t41_v4l2_pipeline_get(video);
	if (ret)
		return ret;

	mutex_lock(&video->channel_lock);
	channel = video->channel;
	if (!channel) {
		mutex_unlock(&video->channel_lock);
		tx_isp_t41_v4l2_pipeline_put(video);
		return -ENODEV;
	}
	ret = tx_isp_t41_frame_channel_claim(channel);
	if (ret) {
		mutex_unlock(&video->channel_lock);
		tx_isp_t41_v4l2_pipeline_put(video);
		return ret;
	}
	video->owned_channel = channel;
	mutex_unlock(&video->channel_lock);

	memset(&wire, 0, sizeof(wire));
	wire.base.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	wire.base.pix.width = video->format.width;
	wire.base.pix.height = video->format.height;
	/* The private T41 format uses vendor pixel code zero for NV12. */
	wire.base.pix.pixelformat = 0;
	wire.base.pix.field = 4;
	wire.base.pix.bytesperline = video->format.bytesperline;
	wire.base.pix.sizeimage = video->format.sizeimage;
	wire.base.pix.colorspace = 8;
	ret = tx_isp_t41_v4l2_set_format(channel, &wire);
	if (ret)
		goto fail;

	memset(&request, 0, sizeof(request));
	request.count = count;
	request.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	request.memory = *(u32 *)((char *)channel + 0x48);
	ret = tx_isp_t41_v4l2_reqbufs(channel, &request);
	if (ret)
		goto fail;
	if (request.count != count) {
		ret = -ENOMEM;
		goto fail;
	}
	video->private_count = count;
	return 0;

fail:
	tx_isp_t41_frame_channel_release(channel);
	tx_isp_t41_v4l2_pipeline_put(video);
	mutex_lock(&video->channel_lock);
	video->owned_channel = NULL;
	video->private_count = 0;
	mutex_unlock(&video->channel_lock);
	return ret;
}

static void tx_isp_t41_v4l2_release_channel(struct tx_isp_t41_v4l2 *video)
{
	void *channel;

	mutex_lock(&video->channel_lock);
	channel = video->owned_channel;
	video->owned_channel = NULL;
	video->private_count = 0;
	mutex_unlock(&video->channel_lock);
	if (channel)
		tx_isp_t41_frame_channel_release(channel);
	tx_isp_t41_v4l2_pipeline_put(video);
}

static int tx_isp_t41_v4l2_queue_setup(
	struct vb2_queue *queue, const void *argument, unsigned int *num_buffers,
	unsigned int *num_planes, unsigned int sizes[], void *alloc_ctxs[])
{
	struct tx_isp_t41_v4l2 *video = vb2_get_drv_priv(queue);
	const struct v4l2_format *requested = argument;
	unsigned int count;
	int ret;

	if (*num_planes) {
		if (*num_planes != 1 || sizes[0] < video->format.sizeimage)
			return -EINVAL;
		return 0;
	}
	if (requested && requested->fmt.pix.sizeimage < video->format.sizeimage)
		return -EINVAL;
	count = *num_buffers;
	if (count < TX_ISP_T41_V4L2_MIN_BUFFERS)
		count = TX_ISP_T41_V4L2_MIN_BUFFERS;
	if (count > TX_ISP_T41_V4L2_MAX_BUFFERS)
		count = TX_ISP_T41_V4L2_MAX_BUFFERS;

	ret = tx_isp_t41_v4l2_acquire_channel(video, count);
	if (ret)
		return ret;
	ret = tx_isp_video_queue_configure(&video->capture_queue, count,
		V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_MEMORY_MMAP,
		video->format.sizeimage);
	if (ret) {
		tx_isp_t41_v4l2_release_channel(video);
		return ret;
	}
	*num_buffers = count;
	*num_planes = 1;
	sizes[0] = video->format.sizeimage;
	alloc_ctxs[0] = video->alloc_ctx;
	return 0;
}

static int tx_isp_t41_v4l2_buffer_init(struct vb2_buffer *buffer)
{
	struct tx_isp_t41_v4l2 *video = vb2_get_drv_priv(buffer->vb2_queue);
	struct tx_isp_nv12_buffer checked;
	dma_addr_t dma = vb2_dma_contig_plane_dma_addr(buffer, 0);
	unsigned long flags;
	int ret;

	/* T41's dma_addr_t and private wire address are both 32 bits. */
	if (buffer->index >= video->private_count)
		return -EINVAL;
	ret = tx_isp_nv12_buffer_build(video->format.width, video->format.height,
		TX_ISP_T41_V4L2_WIDTH_ALIGN, TX_ISP_T41_V4L2_HEIGHT_ALIGN,
		(u32)dma, vb2_plane_size(buffer, 0), &checked);
	if (ret)
		return ret;
	spin_lock_irqsave(&video->queue_lock, flags);
	ret = tx_isp_video_queue_prepare(&video->capture_queue, buffer->index,
		(u32)dma, vb2_plane_size(buffer, 0));
	spin_unlock_irqrestore(&video->queue_lock, flags);
	return ret;
}

static int tx_isp_t41_v4l2_buffer_prepare(struct vb2_buffer *buffer)
{
	struct tx_isp_t41_v4l2 *video = vb2_get_drv_priv(buffer->vb2_queue);

	if (vb2_plane_size(buffer, 0) < video->format.sizeimage)
		return -EINVAL;
	vb2_set_plane_payload(buffer, 0, video->format.sizeimage);
	return 0;
}

static int tx_isp_t41_v4l2_submit(struct tx_isp_t41_v4l2 *video, u32 index)
{
	struct tx_isp_frame_buffer_wire wire;
	struct tx_isp_video_slot *slot;

	if (!video->owned_channel || index >= video->private_count)
		return -ENODEV;
	slot = &video->slots[index];
	memset(&wire, 0, sizeof(wire));
	wire.index = index;
	wire.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	wire.field = 4;
	wire.memory = *(u32 *)((char *)video->owned_channel + 0x48);
	wire.dma = slot->dma;
	wire.length = slot->length;
	return tx_isp_t41_v4l2_qbuf(video->owned_channel, &wire);
}

static void tx_isp_t41_v4l2_buffer_queue(struct vb2_buffer *buffer)
{
	struct tx_isp_t41_v4l2 *video = vb2_get_drv_priv(buffer->vb2_queue);
	unsigned long flags;
	u32 index;
	int ret;

	video->buffers[buffer->index] = buffer;
	spin_lock_irqsave(&video->queue_lock, flags);
	ret = tx_isp_video_queue_qbuf(&video->capture_queue, buffer->index);
	if (!ret && video->capture_queue.streaming)
		ret = tx_isp_video_queue_take(&video->capture_queue, &index);
	spin_unlock_irqrestore(&video->queue_lock, flags);
	if (ret) {
		video->buffers[buffer->index] = NULL;
		vb2_buffer_done(buffer, VB2_BUF_STATE_ERROR);
		return;
	}
	if (video->capture_queue.streaming && tx_isp_t41_v4l2_submit(video, index)) {
		video->buffers[buffer->index] = NULL;
		vb2_buffer_done(buffer, VB2_BUF_STATE_ERROR);
	}
}

static void tx_isp_t41_v4l2_return_buffers(struct tx_isp_t41_v4l2 *video,
					   enum vb2_buffer_state state)
{
	unsigned int index;

	for (index = 0; index < TX_ISP_T41_V4L2_MAX_BUFFERS; index++) {
		struct vb2_buffer *buffer = video->buffers[index];

		if (!buffer)
			continue;
		video->buffers[index] = NULL;
		vb2_buffer_done(buffer, state);
	}
}

static int tx_isp_t41_v4l2_completion_thread(void *argument)
{
	struct tx_isp_t41_v4l2 *video = argument;

	while (!kthread_should_stop()) {
		struct tx_isp_frame_buffer_wire hardware;
		struct tx_isp_frame_buffer_wire complete;
		struct vb2_buffer *buffer;
		struct vb2_v4l2_buffer *v4l2_buffer;
		unsigned long flags;
		u64 timestamp_ns;
		u32 bytesused;
		int ret;

		memset(&hardware, 0, sizeof(hardware));
		hardware.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		ret = tx_isp_t41_v4l2_dqbuf(video->owned_channel, &hardware);
		if (ret) {
			if (kthread_should_stop() || video->stopping)
				break;
			schedule_timeout_interruptible(msecs_to_jiffies(10));
			continue;
		}
		bytesused = hardware.bytesused ? hardware.bytesused :
			video->format.sizeimage;
		timestamp_ns = (u64)hardware.timestamp_sec * 1000000000ULL +
			(u64)hardware.timestamp_usec * 1000ULL;
		if (!timestamp_ns)
			timestamp_ns = ktime_get_ns();

		spin_lock_irqsave(&video->queue_lock, flags);
		ret = tx_isp_video_queue_complete(&video->capture_queue,
			hardware.index, bytesused, timestamp_ns,
			hardware.flags & TX_ISP_FRAME_FLAG_ERROR);
		if (!ret)
			ret = tx_isp_video_queue_dqbuf(&video->capture_queue,
				&complete);
		buffer = hardware.index < TX_ISP_T41_V4L2_MAX_BUFFERS ?
			video->buffers[hardware.index] : NULL;
		if (buffer)
			video->buffers[hardware.index] = NULL;
		spin_unlock_irqrestore(&video->queue_lock, flags);
		if (ret || !buffer) {
			vb2_queue_error(&video->vb2_queue);
			continue;
		}

		v4l2_buffer = to_vb2_v4l2_buffer(buffer);
		v4l2_buffer->field = V4L2_FIELD_NONE;
		v4l2_buffer->sequence = complete.sequence;
		v4l2_buffer->timestamp.tv_sec = complete.timestamp_sec;
		v4l2_buffer->timestamp.tv_usec = complete.timestamp_usec;
		vb2_set_plane_payload(buffer, 0, complete.bytesused);
		vb2_buffer_done(buffer,
			complete.flags & TX_ISP_FRAME_FLAG_ERROR ?
			VB2_BUF_STATE_ERROR : VB2_BUF_STATE_DONE);
	}
	return 0;
}

static int tx_isp_t41_v4l2_start_streaming(struct vb2_queue *queue,
					   unsigned int count)
{
	struct tx_isp_t41_v4l2 *video = vb2_get_drv_priv(queue);
	unsigned long flags;
	u32 index;
	int ret;

	(void)count;
	if (!video->owned_channel)
		return -ENODEV;
	spin_lock_irqsave(&video->queue_lock, flags);
	ret = tx_isp_video_queue_stream_on(&video->capture_queue);
	spin_unlock_irqrestore(&video->queue_lock, flags);
	if (ret)
		goto fail;
	for (;;) {
		spin_lock_irqsave(&video->queue_lock, flags);
		ret = tx_isp_video_queue_take(&video->capture_queue, &index);
		spin_unlock_irqrestore(&video->queue_lock, flags);
		if (ret == -EAGAIN)
			break;
		if (ret || tx_isp_t41_v4l2_submit(video, index)) {
			ret = ret ? ret : -EIO;
			goto fail_stream;
		}
	}
	ret = tx_isp_t41_v4l2_streamon(video->owned_channel,
		V4L2_BUF_TYPE_VIDEO_CAPTURE);
	if (ret)
		goto fail_stream;
	video->stopping = false;
	video->completion_task = kthread_run(
		tx_isp_t41_v4l2_completion_thread, video, "tx-isp-v4l2");
	if (IS_ERR(video->completion_task)) {
		ret = PTR_ERR(video->completion_task);
		video->completion_task = NULL;
		tx_isp_t41_frame_channel_streamoff(video->owned_channel);
		goto fail_stream;
	}
	return 0;

fail_stream:
	spin_lock_irqsave(&video->queue_lock, flags);
	tx_isp_video_queue_stream_off(&video->capture_queue);
	spin_unlock_irqrestore(&video->queue_lock, flags);
fail:
	tx_isp_t41_v4l2_return_buffers(video, VB2_BUF_STATE_ERROR);
	return ret;
}

static void tx_isp_t41_v4l2_stop_streaming(struct vb2_queue *queue)
{
	struct tx_isp_t41_v4l2 *video = vb2_get_drv_priv(queue);
	unsigned long flags;

	video->stopping = true;
	if (video->owned_channel)
		tx_isp_t41_frame_channel_streamoff(video->owned_channel);
	if (video->completion_task) {
		kthread_stop(video->completion_task);
		video->completion_task = NULL;
	}
	spin_lock_irqsave(&video->queue_lock, flags);
	tx_isp_video_queue_stream_off(&video->capture_queue);
	spin_unlock_irqrestore(&video->queue_lock, flags);
	tx_isp_t41_v4l2_return_buffers(video, VB2_BUF_STATE_ERROR);
	video->stopping = false;
}

static const struct vb2_ops tx_isp_t41_v4l2_vb2_ops = {
	.queue_setup = tx_isp_t41_v4l2_queue_setup,
	.buf_init = tx_isp_t41_v4l2_buffer_init,
	.buf_prepare = tx_isp_t41_v4l2_buffer_prepare,
	.buf_queue = tx_isp_t41_v4l2_buffer_queue,
	.start_streaming = tx_isp_t41_v4l2_start_streaming,
	.stop_streaming = tx_isp_t41_v4l2_stop_streaming,
	.wait_prepare = vb2_ops_wait_prepare,
	.wait_finish = vb2_ops_wait_finish,
};

static void tx_isp_t41_v4l2_active_format(struct v4l2_pix_format *format)
{
	*format = tx_isp_t41_video.format;
}

static int tx_isp_t41_v4l2_querycap(struct file *file, void *priv,
				    struct v4l2_capability *capability)
{
	(void)file;
	(void)priv;
	strlcpy((char *)capability->driver, "tx-isp-t41",
		sizeof(capability->driver));
	strlcpy((char *)capability->card, "Ingenic T41 ISP capture",
		sizeof(capability->card));
	strlcpy((char *)capability->bus_info, "platform:tx-isp-t41",
		sizeof(capability->bus_info));
	capability->version = KERNEL_VERSION(0, 2, 0);
	capability->device_caps = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING;
	capability->capabilities = capability->device_caps |
		V4L2_CAP_DEVICE_CAPS;
	return 0;
}

static int tx_isp_t41_v4l2_enum_fmt(struct file *file, void *priv,
				    struct v4l2_fmtdesc *format)
{
	(void)file;
	(void)priv;
	if (format->index)
		return -EINVAL;
	format->pixelformat = V4L2_PIX_FMT_NV12;
	strlcpy((char *)format->description, "NV12",
		sizeof(format->description));
	return 0;
}

static int tx_isp_t41_v4l2_get_fmt(struct file *file, void *priv,
				   struct v4l2_format *format)
{
	(void)file;
	(void)priv;
	if (format->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;
	tx_isp_t41_v4l2_active_format(&format->fmt.pix);
	return 0;
}

static int tx_isp_t41_v4l2_try_fmt(struct file *file, void *priv,
				   struct v4l2_format *format)
{
	return tx_isp_t41_v4l2_get_fmt(file, priv, format);
}

static int tx_isp_t41_v4l2_enum_framesizes(struct file *file, void *priv,
					   struct v4l2_frmsizeenum *size)
{
	(void)file;
	(void)priv;
	if (size->index || size->pixel_format != V4L2_PIX_FMT_NV12)
		return -EINVAL;
	size->type = V4L2_FRMSIZE_TYPE_DISCRETE;
	size->discrete.width = tx_isp_t41_video.format.width;
	size->discrete.height = tx_isp_t41_video.format.height;
	return 0;
}

static int tx_isp_t41_v4l2_enum_frameintervals(
	struct file *file, void *priv, struct v4l2_frmivalenum *interval)
{
	(void)file;
	(void)priv;
	if (interval->index || interval->pixel_format != V4L2_PIX_FMT_NV12 ||
	    interval->width != tx_isp_t41_video.format.width ||
	    interval->height != tx_isp_t41_video.format.height)
		return -EINVAL;
	interval->type = V4L2_FRMIVAL_TYPE_DISCRETE;
	interval->discrete.numerator = 1;
	interval->discrete.denominator = TX_ISP_T41_V4L2_FPS;
	return 0;
}

static int tx_isp_t41_v4l2_get_parm(struct file *file, void *priv,
				    struct v4l2_streamparm *parm)
{
	(void)file;
	(void)priv;
	if (parm->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;
	memset(&parm->parm.capture, 0, sizeof(parm->parm.capture));
	parm->parm.capture.timeperframe.numerator = 1;
	parm->parm.capture.timeperframe.denominator = TX_ISP_T41_V4L2_FPS;
	return 0;
}

static int tx_isp_t41_v4l2_enum_input(struct file *file, void *priv,
				      struct v4l2_input *input)
{
	(void)file;
	(void)priv;
	if (input->index)
		return -EINVAL;
	strlcpy((char *)input->name, "T41 ISP sensor", sizeof(input->name));
	input->type = V4L2_INPUT_TYPE_CAMERA;
	return 0;
}

static int tx_isp_t41_v4l2_get_input(struct file *file, void *priv,
				     unsigned int *input)
{
	(void)file;
	(void)priv;
	*input = 0;
	return 0;
}

static int tx_isp_t41_v4l2_request_buffers(
	struct file *file, void *priv, struct v4l2_requestbuffers *request)
{
	struct tx_isp_t41_v4l2 *video = video_drvdata(file);
	int ret = vb2_ioctl_reqbufs(file, priv, request);

	if (!ret && !request->count)
		tx_isp_t41_v4l2_release_channel(video);
	return ret;
}

static const struct v4l2_ioctl_ops tx_isp_t41_v4l2_ioctl_ops = {
	.vidioc_querycap = tx_isp_t41_v4l2_querycap,
	.vidioc_enum_fmt_vid_cap = tx_isp_t41_v4l2_enum_fmt,
	.vidioc_g_fmt_vid_cap = tx_isp_t41_v4l2_get_fmt,
	.vidioc_try_fmt_vid_cap = tx_isp_t41_v4l2_try_fmt,
	.vidioc_s_fmt_vid_cap = tx_isp_t41_v4l2_try_fmt,
	.vidioc_enum_framesizes = tx_isp_t41_v4l2_enum_framesizes,
	.vidioc_enum_frameintervals = tx_isp_t41_v4l2_enum_frameintervals,
	.vidioc_g_parm = tx_isp_t41_v4l2_get_parm,
	.vidioc_enum_input = tx_isp_t41_v4l2_enum_input,
	.vidioc_g_input = tx_isp_t41_v4l2_get_input,
	.vidioc_reqbufs = tx_isp_t41_v4l2_request_buffers,
	.vidioc_querybuf = vb2_ioctl_querybuf,
	.vidioc_expbuf = vb2_ioctl_expbuf,
	.vidioc_qbuf = vb2_ioctl_qbuf,
	.vidioc_dqbuf = vb2_ioctl_dqbuf,
	.vidioc_streamon = vb2_ioctl_streamon,
	.vidioc_streamoff = vb2_ioctl_streamoff,
};

static int tx_isp_t41_v4l2_release(struct file *file)
{
	struct tx_isp_t41_v4l2 *video = video_drvdata(file);
	int ret = vb2_fop_release(file);

	tx_isp_t41_v4l2_release_channel(video);
	return ret;
}

static const struct v4l2_file_operations tx_isp_t41_v4l2_fops = {
	.owner = THIS_MODULE,
	.open = v4l2_fh_open,
	.release = tx_isp_t41_v4l2_release,
	.unlocked_ioctl = video_ioctl2,
	.mmap = vb2_fop_mmap,
	.poll = vb2_fop_poll,
};

int tx_isp_t41_v4l2_init(struct device *parent)
{
	struct tx_isp_t41_v4l2 *video = &tx_isp_t41_video;
	struct tx_isp_nv12_layout layout;
	int ret;

	if (!parent || v4l2_channel > 2U)
		return -EINVAL;
	memset(video, 0, sizeof(*video));
	BUILD_BUG_ON(sizeof(struct tx_isp_t41_sensor_info) != 100);
	video->parent = parent;
	mutex_init(&video->ioctl_lock);
	mutex_init(&video->channel_lock);
	spin_lock_init(&video->queue_lock);
	ret = tx_isp_video_queue_init(&video->capture_queue, video->slots,
		TX_ISP_T41_V4L2_MAX_BUFFERS);
	if (ret)
		return ret;
	ret = tx_isp_nv12_layout_build(TX_ISP_T41_V4L2_NATIVE_WIDTH,
		TX_ISP_T41_V4L2_NATIVE_HEIGHT, TX_ISP_T41_V4L2_WIDTH_ALIGN,
		TX_ISP_T41_V4L2_HEIGHT_ALIGN, &layout);
	if (ret)
		return ret;
	video->format.width = TX_ISP_T41_V4L2_NATIVE_WIDTH;
	video->format.height = TX_ISP_T41_V4L2_NATIVE_HEIGHT;
	video->format.pixelformat = V4L2_PIX_FMT_NV12;
	video->format.field = V4L2_FIELD_NONE;
	video->format.bytesperline = layout.stride;
	video->format.sizeimage = layout.sizeimage;
	video->format.colorspace = V4L2_COLORSPACE_SRGB;

	video->alloc_ctx = vb2_dma_contig_init_ctx(parent);
	if (IS_ERR(video->alloc_ctx))
		return PTR_ERR(video->alloc_ctx);
	video->vb2_queue.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	video->vb2_queue.io_modes = VB2_MMAP;
	video->vb2_queue.drv_priv = video;
	video->vb2_queue.buf_struct_size = sizeof(struct vb2_v4l2_buffer);
	video->vb2_queue.ops = &tx_isp_t41_v4l2_vb2_ops;
	video->vb2_queue.mem_ops = &vb2_dma_contig_memops;
	video->vb2_queue.timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	video->vb2_queue.lock = &video->ioctl_lock;
	ret = vb2_queue_init(&video->vb2_queue);
	if (ret)
		goto fail_allocator;

	ret = v4l2_device_register(parent, &video->v4l2_dev);
	if (ret)
		goto fail_queue;
	strlcpy(video->video_dev.name, "tx-isp-t41-capture",
		sizeof(video->video_dev.name));
	video->video_dev.v4l2_dev = &video->v4l2_dev;
	video->video_dev.fops = &tx_isp_t41_v4l2_fops;
	video->video_dev.ioctl_ops = &tx_isp_t41_v4l2_ioctl_ops;
	video->video_dev.release = video_device_release_empty;
	video->video_dev.lock = &video->ioctl_lock;
	video->video_dev.queue = &video->vb2_queue;
	video->video_dev.vfl_dir = VFL_DIR_RX;
	video_set_drvdata(&video->video_dev, video);
	ret = video_register_device(&video->video_dev, VFL_TYPE_GRABBER, -1);
	if (ret)
		goto fail_v4l2;
	video->registered = true;
	pr_info("tx_isp_t41: V4L2 MMAP node registered as /dev/video%d (channel %u)\n",
		video->video_dev.num, v4l2_channel);
	return 0;

fail_v4l2:
	v4l2_device_unregister(&video->v4l2_dev);
fail_queue:
	vb2_queue_release(&video->vb2_queue);
fail_allocator:
	vb2_dma_contig_cleanup_ctx(video->alloc_ctx);
	return ret;
}

void tx_isp_t41_v4l2_exit(void)
{
	struct tx_isp_t41_v4l2 *video = &tx_isp_t41_video;

	if (!video->registered)
		return;
	tx_isp_t41_v4l2_release_channel(video);
	video_unregister_device(&video->video_dev);
	video->registered = false;
	v4l2_device_unregister(&video->v4l2_dev);
	vb2_queue_release(&video->vb2_queue);
	vb2_dma_contig_cleanup_ctx(video->alloc_ctx);
}

void tx_isp_t41_v4l2_bind_channel(void *channel, unsigned int index)
{
	struct tx_isp_t41_v4l2 *video = &tx_isp_t41_video;

	if (!video->registered || index != v4l2_channel)
		return;
	mutex_lock(&video->channel_lock);
	video->channel = channel;
	mutex_unlock(&video->channel_lock);
}

void tx_isp_t41_v4l2_unbind_channel(void *channel)
{
	struct tx_isp_t41_v4l2 *video = &tx_isp_t41_video;

	if (!video->registered)
		return;
	mutex_lock(&video->channel_lock);
	if (video->channel == channel)
		video->channel = NULL;
	if (video->owned_channel == channel) {
		video->owned_channel = NULL;
		video->private_count = 0;
		vb2_queue_error(&video->vb2_queue);
	}
	mutex_unlock(&video->channel_lock);
}
