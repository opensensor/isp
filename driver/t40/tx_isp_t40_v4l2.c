// SPDX-License-Identifier: GPL-2.0
/* Linux-4.4 V4L2 MMAP/DMA-BUF adapter for the recovered T40 frame channel. */

#include <linux/dma-mapping.h>
#include <linux/kernel.h>
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

#include "../include/tx_isp/tx_isp_frame_channel.h"
#include "../include/tx_isp/tx_isp_frame_format.h"
#include "../include/tx_isp/tx_isp_frame_layout.h"
#include "../include/tx_isp/tx_isp_video_queue.h"
#include "tx_isp_t40_v4l2.h"

#ifndef CONFIG_DMA_SHARED_BUFFER
#error "CONFIG_TX_ISP_T40_V4L2 requires CONFIG_DMA_SHARED_BUFFER"
#endif

#if !defined(CONFIG_VIDEOBUF2_DMA_CONTIG) && \
	!defined(CONFIG_VIDEOBUF2_DMA_CONTIG_MODULE)
#error "CONFIG_TX_ISP_T40_V4L2 requires CONFIG_VIDEOBUF2_DMA_CONTIG"
#endif

#define TX_ISP_T40_V4L2_WIDTH_ALIGN	32U
#define TX_ISP_T40_V4L2_HEIGHT_ALIGN	16U
#define TX_ISP_T40_V4L2_FALLBACK_WIDTH	1920U
#define TX_ISP_T40_V4L2_FALLBACK_HEIGHT	1080U
#define TX_ISP_T40_V4L2_FALLBACK_FPS	25U
#define TX_ISP_T40_V4L2_MIN_BUFFERS	2U
#define TX_ISP_T40_V4L2_MAX_BUFFERS	8U
#define TX_ISP_T40_DRAM_LIMIT		0x10000000U

/* T40 and T41 use the same private 'T' ioctl numbers except that T40's
 * format image is 112 bytes rather than T41's 116-byte extended image. */
#define TX_ISP_T40_IOCTL_SET_FORMAT	0xc0705451U
#define TX_ISP_T40_IOCTL_REQBUFS		0xc0145453U
#define TX_ISP_T40_IOCTL_QBUF		0xc0445455U
#define TX_ISP_T40_IOCTL_DQBUF		0xc0445456U
#define TX_ISP_T40_IOCTL_STREAM_ON	0xc0045457U
#define TX_ISP_T40_IOCTL_STREAM_OFF	0xc0045458U

struct tx_isp_t40_v4l2 {
	struct v4l2_device v4l2_dev;
	struct video_device video_dev;
	struct vb2_queue vb2_queue;
	struct mutex ioctl_lock;
	spinlock_t queue_lock;
	struct tx_isp_video_queue capture_queue;
	struct tx_isp_video_slot slots[TX_ISP_T40_V4L2_MAX_BUFFERS];
	struct vb2_buffer *buffers[TX_ISP_T40_V4L2_MAX_BUFFERS];
	struct v4l2_pix_format format;
	struct task_struct *completion_task;
	struct file private_file;
	void *alloc_ctx;
	struct device *parent;
	u32 private_count;
	bool private_opened;
	bool stopping;
	bool registered;
};

static struct tx_isp_t40_v4l2 tx_isp_t40_video;

static int tx_isp_t40_v4l2_private_call(struct tx_isp_t40_v4l2 *video,
					 unsigned int command,
					 void *argument)
{
	mm_segment_t old_fs = get_fs();
	long ret;

	set_fs(KERNEL_DS);
	ret = tx_isp_t40_v4l2_private_ioctl(&video->private_file, command,
		argument);
	set_fs(old_fs);
	/* The recovered T40 frame-channel QBUF may return a positive vendor
	 * result after successfully accepting the buffer.  Its native callers
	 * use the normal ioctl convention (only negative values are errors), so
	 * do not turn that successful result into a VB2 buffer failure. */
	return ret < 0 ? (int)ret : 0;
}

static int tx_isp_t40_v4l2_make_format(struct v4l2_pix_format *format)
{
	struct tx_isp_nv12_layout layout;
	unsigned int width = 0;
	unsigned int height = 0;
	int ret;

	ret = tx_isp_t40_v4l2_sensor_dimensions(&width, &height);
	if (ret || !width || !height) {
		width = TX_ISP_T40_V4L2_FALLBACK_WIDTH;
		height = TX_ISP_T40_V4L2_FALLBACK_HEIGHT;
	}
	ret = tx_isp_nv12_layout_build(width, height,
		TX_ISP_T40_V4L2_WIDTH_ALIGN, TX_ISP_T40_V4L2_HEIGHT_ALIGN,
		&layout);
	if (ret)
		return ret;

	memset(format, 0, sizeof(*format));
	format->width = width;
	format->height = height;
	format->pixelformat = V4L2_PIX_FMT_NV12;
	format->field = V4L2_FIELD_NONE;
	format->bytesperline = layout.stride;
	format->sizeimage = layout.sizeimage;
	format->colorspace = V4L2_COLORSPACE_REC709;
	return 0;
}

static void tx_isp_t40_v4l2_refresh_format(struct tx_isp_t40_v4l2 *video)
{
	struct v4l2_pix_format sensed;

	if (video->private_count)
		return;
	if (!tx_isp_t40_v4l2_make_format(&sensed))
		video->format = sensed;
}

static int tx_isp_t40_v4l2_acquire_private(
	struct tx_isp_t40_v4l2 *video, unsigned int count)
{
	struct tx_isp_frame_format_wire format;
	struct tx_isp_frame_request_wire request;
	int ret;

	if (video->private_opened)
		return video->private_count == count ? 0 : -EBUSY;

	memset(&video->private_file, 0, sizeof(video->private_file));
	ret = tx_isp_t40_v4l2_private_open(&video->private_file);
	if (ret)
		return ret;
	video->private_opened = true;

	memset(&format, 0, sizeof(format));
	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	format.pix.width = video->format.width;
	format.pix.height = video->format.height;
	/* The T40 private path normalizes both FOURCC NV12 and vendor code 0. */
	format.pix.pixelformat = V4L2_PIX_FMT_NV12;
	format.pix.field = V4L2_FIELD_NONE;
	format.pix.bytesperline = video->format.bytesperline;
	format.pix.sizeimage = video->format.sizeimage;
	format.pix.colorspace = V4L2_COLORSPACE_REC709;
	ret = tx_isp_t40_v4l2_private_call(video,
		TX_ISP_T40_IOCTL_SET_FORMAT, &format);
	if (ret)
		goto fail;

	memset(&request, 0, sizeof(request));
	request.count = count;
	request.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	request.memory = V4L2_MEMORY_USERPTR;
	ret = tx_isp_t40_v4l2_private_call(video,
		TX_ISP_T40_IOCTL_REQBUFS, &request);
	if (ret)
		goto fail;
	if (request.count != count) {
		ret = -ENOMEM;
		goto fail_buffers;
	}
	video->private_count = count;
	return 0;

fail_buffers:
	request.count = 0;
	tx_isp_t40_v4l2_private_call(video, TX_ISP_T40_IOCTL_REQBUFS,
		&request);
fail:
	tx_isp_t40_v4l2_private_release(&video->private_file);
	video->private_opened = false;
	return ret;
}

static void tx_isp_t40_v4l2_release_private(struct tx_isp_t40_v4l2 *video)
{
	struct tx_isp_frame_request_wire request;

	if (!video->private_opened)
		return;
	memset(&request, 0, sizeof(request));
	request.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	request.memory = V4L2_MEMORY_USERPTR;
	tx_isp_t40_v4l2_private_call(video, TX_ISP_T40_IOCTL_REQBUFS,
		&request);
	tx_isp_t40_v4l2_private_release(&video->private_file);
	memset(&video->private_file, 0, sizeof(video->private_file));
	video->private_count = 0;
	video->private_opened = false;
}

static int tx_isp_t40_v4l2_queue_setup(
	struct vb2_queue *queue, const void *argument, unsigned int *num_buffers,
	unsigned int *num_planes, unsigned int sizes[], void *alloc_ctxs[])
{
	struct tx_isp_t40_v4l2 *video = vb2_get_drv_priv(queue);
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
	if (count < TX_ISP_T40_V4L2_MIN_BUFFERS)
		count = TX_ISP_T40_V4L2_MIN_BUFFERS;
	if (count > TX_ISP_T40_V4L2_MAX_BUFFERS)
		count = TX_ISP_T40_V4L2_MAX_BUFFERS;

	ret = tx_isp_t40_v4l2_acquire_private(video, count);
	if (ret)
		return ret;
	ret = tx_isp_video_queue_configure(&video->capture_queue, count,
		V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_MEMORY_MMAP,
		video->format.sizeimage);
	if (ret) {
		tx_isp_t40_v4l2_release_private(video);
		return ret;
	}
	*num_buffers = count;
	*num_planes = 1;
	sizes[0] = video->format.sizeimage;
	alloc_ctxs[0] = video->alloc_ctx;
	return 0;
}

static int tx_isp_t40_v4l2_buffer_init(struct vb2_buffer *buffer)
{
	struct tx_isp_t40_v4l2 *video = vb2_get_drv_priv(buffer->vb2_queue);
	struct tx_isp_nv12_buffer checked;
	dma_addr_t dma = vb2_dma_contig_plane_dma_addr(buffer, 0);
	unsigned long flags;
	int ret;

	if (buffer->index >= video->private_count || (u64)dma > U32_MAX)
		return -EINVAL;
	ret = tx_isp_nv12_buffer_build(video->format.width, video->format.height,
		TX_ISP_T40_V4L2_WIDTH_ALIGN, TX_ISP_T40_V4L2_HEIGHT_ALIGN,
		(u32)dma, vb2_plane_size(buffer, 0), &checked);
	if (ret)
		return ret;
	ret = tx_isp_dma_range_validate((u32)dma, video->format.sizeimage,
		TX_ISP_T40_DRAM_LIMIT);
	if (ret)
		return ret;
	spin_lock_irqsave(&video->queue_lock, flags);
	ret = tx_isp_video_queue_prepare(&video->capture_queue, buffer->index,
		(u32)dma, vb2_plane_size(buffer, 0));
	spin_unlock_irqrestore(&video->queue_lock, flags);
	return ret;
}

static int tx_isp_t40_v4l2_buffer_prepare(struct vb2_buffer *buffer)
{
	struct tx_isp_t40_v4l2 *video = vb2_get_drv_priv(buffer->vb2_queue);

	if (vb2_plane_size(buffer, 0) < video->format.sizeimage)
		return -EINVAL;
	vb2_set_plane_payload(buffer, 0, video->format.sizeimage);
	return 0;
}

static int tx_isp_t40_v4l2_submit(struct tx_isp_t40_v4l2 *video, u32 index)
{
	struct tx_isp_frame_buffer_wire wire;
	struct tx_isp_video_slot *slot;

	if (!video->private_opened || index >= video->private_count)
		return -ENODEV;
	slot = &video->slots[index];
	memset(&wire, 0, sizeof(wire));
	wire.index = index;
	wire.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	wire.field = V4L2_FIELD_NONE;
	wire.memory = V4L2_MEMORY_USERPTR;
	wire.dma = slot->dma;
	wire.length = slot->length;
	return tx_isp_t40_v4l2_private_call(video, TX_ISP_T40_IOCTL_QBUF,
		&wire);
}

static void tx_isp_t40_v4l2_buffer_queue(struct vb2_buffer *buffer)
{
	struct tx_isp_t40_v4l2 *video = vb2_get_drv_priv(buffer->vb2_queue);
	unsigned long flags;
	u32 index = 0;
	bool streaming;
	int ret;

	video->buffers[buffer->index] = buffer;
	spin_lock_irqsave(&video->queue_lock, flags);
	ret = tx_isp_video_queue_qbuf(&video->capture_queue, buffer->index);
	streaming = video->capture_queue.streaming;
	if (!ret && streaming)
		ret = tx_isp_video_queue_take(&video->capture_queue, &index);
	spin_unlock_irqrestore(&video->queue_lock, flags);
	if (ret || (streaming && tx_isp_t40_v4l2_submit(video, index))) {
		video->buffers[buffer->index] = NULL;
		vb2_buffer_done(buffer, VB2_BUF_STATE_ERROR);
	}
}

static void tx_isp_t40_v4l2_return_buffers(struct tx_isp_t40_v4l2 *video,
					   enum vb2_buffer_state state)
{
	unsigned int index;

	for (index = 0; index < TX_ISP_T40_V4L2_MAX_BUFFERS; index++) {
		struct vb2_buffer *buffer = video->buffers[index];

		if (!buffer)
			continue;
		video->buffers[index] = NULL;
		vb2_buffer_done(buffer, state);
	}
}

static int tx_isp_t40_v4l2_completion_thread(void *argument)
{
	struct tx_isp_t40_v4l2 *video = argument;
	unsigned int empty_dqbuf_count = 0;

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
		ret = tx_isp_t40_v4l2_private_call(video,
			TX_ISP_T40_IOCTL_DQBUF, &hardware);
		if (ret) {
			if (kthread_should_stop() || video->stopping)
				break;
			schedule_timeout_interruptible(msecs_to_jiffies(10));
			continue;
		}
		/* The recovered OEM DQBUF sometimes returns a nonnegative stale
		 * result before it has copied out a completed descriptor.  The zeroed
		 * argument then looks like buffer 0 and would make VB2 complete the
		 * same buffer twice, permanently setting the queue error flag.  A real
		 * completion carries back the exact DMA buffer identity registered by
		 * QBUF, so reject incomplete/stale descriptors here. */
		if (hardware.index >= video->private_count ||
		    hardware.dma != video->slots[hardware.index].dma ||
		    hardware.length < video->format.sizeimage) {
			if (empty_dqbuf_count < 4)
				pr_warn("tx_isp_t40: ignored empty DQBUF index=%u dma=0x%x length=0x%x\n",
					hardware.index, hardware.dma, hardware.length);
			empty_dqbuf_count++;
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
		buffer = hardware.index < TX_ISP_T40_V4L2_MAX_BUFFERS ?
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

static int tx_isp_t40_v4l2_start_streaming(struct vb2_queue *queue,
					   unsigned int count)
{
	struct tx_isp_t40_v4l2 *video = vb2_get_drv_priv(queue);
	unsigned long flags;
	u32 index;
	u32 type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	int ret;

	(void)count;
	if (!video->private_opened)
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
		if (ret || tx_isp_t40_v4l2_submit(video, index)) {
			ret = ret ? ret : -EIO;
			goto fail_stream;
		}
	}
	ret = tx_isp_t40_v4l2_private_call(video,
		TX_ISP_T40_IOCTL_STREAM_ON, &type);
	if (ret)
		goto fail_stream;
	video->stopping = false;
	video->completion_task = kthread_run(
		tx_isp_t40_v4l2_completion_thread, video, "tx-isp-t40-v4l2");
	if (IS_ERR(video->completion_task)) {
		ret = PTR_ERR(video->completion_task);
		video->completion_task = NULL;
		tx_isp_t40_v4l2_private_call(video,
			TX_ISP_T40_IOCTL_STREAM_OFF, &type);
		goto fail_stream;
	}
	return 0;

fail_stream:
	spin_lock_irqsave(&video->queue_lock, flags);
	tx_isp_video_queue_stream_off(&video->capture_queue);
	spin_unlock_irqrestore(&video->queue_lock, flags);
fail:
	tx_isp_t40_v4l2_return_buffers(video, VB2_BUF_STATE_ERROR);
	return ret;
}

static void tx_isp_t40_v4l2_stop_streaming(struct vb2_queue *queue)
{
	struct tx_isp_t40_v4l2 *video = vb2_get_drv_priv(queue);
	unsigned long flags;
	u32 type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	video->stopping = true;
	/* Stop DQBUF before changing the private frame-channel state.  Running
	 * DQBUF concurrently with STREAM_OFF can leave the adapter kthread alive
	 * after its userspace owner has gone away. */
	if (video->completion_task) {
		kthread_stop(video->completion_task);
		video->completion_task = NULL;
	}
	if (video->private_opened)
		tx_isp_t40_v4l2_private_call(video,
			TX_ISP_T40_IOCTL_STREAM_OFF, &type);
	spin_lock_irqsave(&video->queue_lock, flags);
	tx_isp_video_queue_stream_off(&video->capture_queue);
	spin_unlock_irqrestore(&video->queue_lock, flags);
	tx_isp_t40_v4l2_return_buffers(video, VB2_BUF_STATE_ERROR);
	video->stopping = false;
}

static const struct vb2_ops tx_isp_t40_v4l2_vb2_ops = {
	.queue_setup = tx_isp_t40_v4l2_queue_setup,
	.buf_init = tx_isp_t40_v4l2_buffer_init,
	.buf_prepare = tx_isp_t40_v4l2_buffer_prepare,
	.buf_queue = tx_isp_t40_v4l2_buffer_queue,
	.start_streaming = tx_isp_t40_v4l2_start_streaming,
	.stop_streaming = tx_isp_t40_v4l2_stop_streaming,
	.wait_prepare = vb2_ops_wait_prepare,
	.wait_finish = vb2_ops_wait_finish,
};

static int tx_isp_t40_v4l2_querycap(struct file *file, void *priv,
				    struct v4l2_capability *capability)
{
	(void)file;
	(void)priv;
	strlcpy((char *)capability->driver, "tx-isp-t40",
		sizeof(capability->driver));
	strlcpy((char *)capability->card, "Ingenic T40 ISP capture",
		sizeof(capability->card));
	strlcpy((char *)capability->bus_info, "platform:tx-isp-t40",
		sizeof(capability->bus_info));
	capability->version = KERNEL_VERSION(0, 1, 0);
	capability->device_caps = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING;
	capability->capabilities = capability->device_caps |
		V4L2_CAP_DEVICE_CAPS;
	return 0;
}

static int tx_isp_t40_v4l2_enum_fmt(struct file *file, void *priv,
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

static int tx_isp_t40_v4l2_get_fmt(struct file *file, void *priv,
				   struct v4l2_format *format)
{
	struct tx_isp_t40_v4l2 *video = video_drvdata(file);

	(void)priv;
	if (format->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;
	tx_isp_t40_v4l2_refresh_format(video);
	format->fmt.pix = video->format;
	return 0;
}

static int tx_isp_t40_v4l2_set_fmt(struct file *file, void *priv,
				   struct v4l2_format *format)
{
	struct tx_isp_t40_v4l2 *video = video_drvdata(file);

	if (video->private_count)
		return -EBUSY;
	return tx_isp_t40_v4l2_get_fmt(file, priv, format);
}

static int tx_isp_t40_v4l2_enum_framesizes(struct file *file, void *priv,
					   struct v4l2_frmsizeenum *size)
{
	struct tx_isp_t40_v4l2 *video = video_drvdata(file);

	(void)priv;
	if (size->index || size->pixel_format != V4L2_PIX_FMT_NV12)
		return -EINVAL;
	tx_isp_t40_v4l2_refresh_format(video);
	size->type = V4L2_FRMSIZE_TYPE_DISCRETE;
	size->discrete.width = video->format.width;
	size->discrete.height = video->format.height;
	return 0;
}

static int tx_isp_t40_v4l2_enum_frameintervals(
	struct file *file, void *priv, struct v4l2_frmivalenum *interval)
{
	struct tx_isp_t40_v4l2 *video = video_drvdata(file);
	unsigned int numerator = TX_ISP_T40_V4L2_FALLBACK_FPS;
	unsigned int denominator = 1U;

	(void)priv;
	tx_isp_t40_v4l2_refresh_format(video);
	if (interval->index || interval->pixel_format != V4L2_PIX_FMT_NV12 ||
	    interval->width != video->format.width ||
	    interval->height != video->format.height)
		return -EINVAL;
	interval->type = V4L2_FRMIVAL_TYPE_DISCRETE;
	if (tx_isp_t40_v4l2_sensor_fps(&numerator, &denominator)) {
		numerator = TX_ISP_T40_V4L2_FALLBACK_FPS;
		denominator = 1U;
	}
	interval->discrete.numerator = denominator;
	interval->discrete.denominator = numerator;
	return 0;
}

static int tx_isp_t40_v4l2_get_parm(struct file *file, void *priv,
				    struct v4l2_streamparm *parm)
{
	unsigned int numerator = TX_ISP_T40_V4L2_FALLBACK_FPS;
	unsigned int denominator = 1U;

	(void)file;
	(void)priv;
	if (parm->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;
	memset(&parm->parm.capture, 0, sizeof(parm->parm.capture));
	if (tx_isp_t40_v4l2_sensor_fps(&numerator, &denominator)) {
		numerator = TX_ISP_T40_V4L2_FALLBACK_FPS;
		denominator = 1U;
	}
	parm->parm.capture.timeperframe.numerator = denominator;
	parm->parm.capture.timeperframe.denominator = numerator;
	return 0;
}

static int tx_isp_t40_v4l2_enum_input(struct file *file, void *priv,
				      struct v4l2_input *input)
{
	(void)file;
	(void)priv;
	if (input->index)
		return -EINVAL;
	strlcpy((char *)input->name, "T40 ISP sensor", sizeof(input->name));
	input->type = V4L2_INPUT_TYPE_CAMERA;
	return 0;
}

static int tx_isp_t40_v4l2_get_input(struct file *file, void *priv,
				     unsigned int *input)
{
	(void)file;
	(void)priv;
	*input = 0;
	return 0;
}

static int tx_isp_t40_v4l2_request_buffers(
	struct file *file, void *priv, struct v4l2_requestbuffers *request)
{
	struct tx_isp_t40_v4l2 *video = video_drvdata(file);
	int ret = vb2_ioctl_reqbufs(file, priv, request);

	if (!ret && !request->count)
		tx_isp_t40_v4l2_release_private(video);
	return ret;
}

static const struct v4l2_ioctl_ops tx_isp_t40_v4l2_ioctl_ops = {
	.vidioc_querycap = tx_isp_t40_v4l2_querycap,
	.vidioc_enum_fmt_vid_cap = tx_isp_t40_v4l2_enum_fmt,
	.vidioc_g_fmt_vid_cap = tx_isp_t40_v4l2_get_fmt,
	.vidioc_try_fmt_vid_cap = tx_isp_t40_v4l2_get_fmt,
	.vidioc_s_fmt_vid_cap = tx_isp_t40_v4l2_set_fmt,
	.vidioc_enum_framesizes = tx_isp_t40_v4l2_enum_framesizes,
	.vidioc_enum_frameintervals = tx_isp_t40_v4l2_enum_frameintervals,
	.vidioc_g_parm = tx_isp_t40_v4l2_get_parm,
	.vidioc_enum_input = tx_isp_t40_v4l2_enum_input,
	.vidioc_g_input = tx_isp_t40_v4l2_get_input,
	.vidioc_reqbufs = tx_isp_t40_v4l2_request_buffers,
	.vidioc_querybuf = vb2_ioctl_querybuf,
	.vidioc_expbuf = vb2_ioctl_expbuf,
	.vidioc_qbuf = vb2_ioctl_qbuf,
	.vidioc_dqbuf = vb2_ioctl_dqbuf,
	.vidioc_streamon = vb2_ioctl_streamon,
	.vidioc_streamoff = vb2_ioctl_streamoff,
};

static int tx_isp_t40_v4l2_release(struct file *file)
{
	struct tx_isp_t40_v4l2 *video = video_drvdata(file);
	int ret;

	/* Linux 4.4's vb2_fop_release() normally calls stop_streaming(), but make
	 * that ownership transition explicit for abrupt daemon disconnects. */
	if (vb2_is_streaming(&video->vb2_queue))
		vb2_streamoff(&video->vb2_queue, video->vb2_queue.type);
	ret = vb2_fop_release(file);

	tx_isp_t40_v4l2_release_private(video);
	return ret;
}

static const struct v4l2_file_operations tx_isp_t40_v4l2_fops = {
	.owner = THIS_MODULE,
	.open = v4l2_fh_open,
	.release = tx_isp_t40_v4l2_release,
	.unlocked_ioctl = video_ioctl2,
	.mmap = vb2_fop_mmap,
	.poll = vb2_fop_poll,
};

int tx_isp_t40_v4l2_init(struct device *parent)
{
	struct tx_isp_t40_v4l2 *video = &tx_isp_t40_video;
	int ret;

	if (!parent)
		return -EINVAL;
	memset(video, 0, sizeof(*video));
	BUILD_BUG_ON(sizeof(struct tx_isp_frame_format_wire) !=
		TX_ISP_FRAME_FORMAT_BYTES);
	BUILD_BUG_ON(sizeof(struct tx_isp_frame_buffer_wire) !=
		TX_ISP_FRAME_BUFFER_BYTES);
	video->parent = parent;
	mutex_init(&video->ioctl_lock);
	spin_lock_init(&video->queue_lock);
	ret = tx_isp_video_queue_init(&video->capture_queue, video->slots,
		TX_ISP_T40_V4L2_MAX_BUFFERS);
	if (ret)
		return ret;
	ret = tx_isp_t40_v4l2_make_format(&video->format);
	if (ret)
		return ret;

	video->alloc_ctx = vb2_dma_contig_init_ctx(parent);
	if (IS_ERR(video->alloc_ctx))
		return PTR_ERR(video->alloc_ctx);
	video->vb2_queue.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	video->vb2_queue.io_modes = VB2_MMAP;
	video->vb2_queue.drv_priv = video;
	video->vb2_queue.buf_struct_size = sizeof(struct vb2_v4l2_buffer);
	video->vb2_queue.ops = &tx_isp_t40_v4l2_vb2_ops;
	video->vb2_queue.mem_ops = &vb2_dma_contig_memops;
	video->vb2_queue.timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	video->vb2_queue.lock = &video->ioctl_lock;
	ret = vb2_queue_init(&video->vb2_queue);
	if (ret)
		goto fail_allocator;

	ret = v4l2_device_register(parent, &video->v4l2_dev);
	if (ret)
		goto fail_queue;
	strlcpy(video->video_dev.name, "tx-isp-t40-capture",
		sizeof(video->video_dev.name));
	video->video_dev.v4l2_dev = &video->v4l2_dev;
	video->video_dev.fops = &tx_isp_t40_v4l2_fops;
	video->video_dev.ioctl_ops = &tx_isp_t40_v4l2_ioctl_ops;
	video->video_dev.release = video_device_release_empty;
	video->video_dev.lock = &video->ioctl_lock;
	video->video_dev.queue = &video->vb2_queue;
	video->video_dev.vfl_dir = VFL_DIR_RX;
	video_set_drvdata(&video->video_dev, video);
	ret = video_register_device(&video->video_dev, VFL_TYPE_GRABBER, -1);
	if (ret)
		goto fail_v4l2;
	video->registered = true;
	pr_info("tx_isp_t40: V4L2 MMAP/DMA-BUF node registered as /dev/video%d\n",
		video->video_dev.num);
	return 0;

fail_v4l2:
	v4l2_device_unregister(&video->v4l2_dev);
fail_queue:
	vb2_queue_release(&video->vb2_queue);
fail_allocator:
	vb2_dma_contig_cleanup_ctx(video->alloc_ctx);
	return ret;
}

void tx_isp_t40_v4l2_exit(void)
{
	struct tx_isp_t40_v4l2 *video = &tx_isp_t40_video;

	if (!video->registered)
		return;
	tx_isp_t40_v4l2_release_private(video);
	video_unregister_device(&video->video_dev);
	video->registered = false;
	v4l2_device_unregister(&video->v4l2_dev);
	vb2_queue_release(&video->vb2_queue);
	vb2_dma_contig_cleanup_ctx(video->alloc_ctx);
}
