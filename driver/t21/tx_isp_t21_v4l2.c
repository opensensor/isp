// SPDX-License-Identifier: GPL-2.0
/* Public V4L2 MMAP/DMA-BUF adapter for the T21 private frame channel. */

#include <linux/dma-buf.h>
#include <linux/dma-mapping.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/poll.h>
#include <linux/scatterlist.h>
#include <linux/slab.h>
#include <linux/videodev2.h>

#include <asm/uaccess.h>

#include <media/v4l2-dev.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ioctl.h>

#include "tx_isp_t21_v4l2.h"

#define TX_ISP_T21_CAPTURE_CHANNEL 0U
#define TX_ISP_T21_MIN_WIDTH 16U
#define TX_ISP_T21_MIN_HEIGHT 16U
#define TX_ISP_T21_MAX_WIDTH 2592U
#define TX_ISP_T21_MAX_HEIGHT 2048U
#define TX_ISP_T21_MIN_BUFFERS 2U
#define TX_ISP_T21_MAX_BUFFERS 4U

#define TX_ISP_T21_FRAME_IOCTL_SET_FORMAT 0xc04c56c3U
#define TX_ISP_T21_FRAME_IOCTL_REQBUFS VIDIOC_REQBUFS
#define TX_ISP_T21_FRAME_IOCTL_QBUF VIDIOC_QBUF
#define TX_ISP_T21_FRAME_IOCTL_DQBUF VIDIOC_DQBUF
#define TX_ISP_T21_FRAME_IOCTL_STREAM_ON VIDIOC_STREAMON
#define TX_ISP_T21_FRAME_IOCTL_STREAM_OFF VIDIOC_STREAMOFF

#define TX_ISP_T21_GET_DMA_PHY \
	_IOWR('q', 18, struct tx_isp_t21_dma_info)

struct tx_isp_t21_dma_info {
	u32 fd;
	u32 size;
	u32 physical_address;
};

struct tx_isp_t21_legacy_format {
	u32 type;
	struct v4l2_pix_format pix;
	u8 crop_enable;
	u8 crop_pad[3];
	u32 crop_top;
	u32 crop_left;
	u32 crop_width;
	u32 crop_height;
	u8 scaler_enable;
	u8 scaler_pad[3];
	u32 scaler_out_width;
	u32 scaler_out_height;
	u32 rate_bits;
	u32 rate_mask;
};

struct tx_isp_t21_v4l2_buffer {
	struct device *device;
	void *cpu_address;
	dma_addr_t dma_address;
	u32 size;
	atomic_t references;
};

struct tx_isp_t21_v4l2_attachment {
	struct sg_table table;
	enum dma_data_direction direction;
};

struct tx_isp_t21_v4l2_export {
	struct tx_isp_t21_v4l2_buffer *buffer;
};

struct tx_isp_t21_v4l2_device {
	struct v4l2_device v4l2_device;
	struct video_device *video_device;
	struct device *dma_device;
	struct mutex lock;
	struct v4l2_pix_format format;
	struct tx_isp_t21_v4l2_buffer *buffers[TX_ISP_T21_MAX_BUFFERS];
	struct tx_isp_t21_v4l2_buffer *pool_buffers[TX_ISP_T21_MAX_BUFFERS];
	u32 buffer_count;
	u32 pool_count;
	u32 pool_size;
	bool opened;
	bool upstream_streaming;
	bool streaming;
	bool registered;
	bool resolver_registered;
};

static struct tx_isp_t21_v4l2_device tx_isp_t21_video;

static void tx_isp_t21_buffer_get(struct tx_isp_t21_v4l2_buffer *buffer)
{
	atomic_inc(&buffer->references);
}

static void tx_isp_t21_buffer_put(struct tx_isp_t21_v4l2_buffer *buffer)
{
	if (!buffer || !atomic_dec_and_test(&buffer->references))
		return;
	dma_free_coherent(buffer->device, buffer->size, buffer->cpu_address,
			  buffer->dma_address);
	put_device(buffer->device);
	kfree(buffer);
}

static struct tx_isp_t21_v4l2_buffer *
tx_isp_t21_buffer_alloc(struct device *device, u32 size)
{
	struct tx_isp_t21_v4l2_buffer *buffer;

	buffer = kzalloc(sizeof(*buffer), GFP_KERNEL);
	if (!buffer)
		return NULL;
	buffer->device = get_device(device);
	buffer->size = PAGE_ALIGN(size);
	buffer->cpu_address = dma_alloc_coherent(device, buffer->size,
						 &buffer->dma_address,
						 GFP_KERNEL | GFP_DMA);
	if (!buffer->cpu_address) {
		put_device(buffer->device);
		kfree(buffer);
		return NULL;
	}
	atomic_set(&buffer->references, 1);
	memset(buffer->cpu_address, 0, buffer->size);
	return buffer;
}

static int tx_isp_t21_dmabuf_attach(struct dma_buf *dma_buffer,
				     struct device *device,
				     struct dma_buf_attachment *attachment)
{
	struct tx_isp_t21_v4l2_export *export = dma_buffer->priv;
	struct tx_isp_t21_v4l2_attachment *private;
	int ret;

	private = kzalloc(sizeof(*private), GFP_KERNEL);
	if (!private)
		return -ENOMEM;
	ret = dma_get_sgtable(export->buffer->device, &private->table,
			      export->buffer->cpu_address,
			      export->buffer->dma_address,
			      export->buffer->size);
	if (ret) {
		kfree(private);
		return ret;
	}
	private->direction = DMA_NONE;
	attachment->priv = private;
	return 0;
}

static void tx_isp_t21_dmabuf_detach(struct dma_buf *dma_buffer,
				      struct dma_buf_attachment *attachment)
{
	struct tx_isp_t21_v4l2_attachment *private = attachment->priv;

	if (!private)
		return;
	if (private->direction != DMA_NONE)
		dma_unmap_sg(attachment->dev, private->table.sgl,
			     private->table.orig_nents, private->direction);
	sg_free_table(&private->table);
	kfree(private);
	attachment->priv = NULL;
}

static struct sg_table *
tx_isp_t21_dmabuf_map(struct dma_buf_attachment *attachment,
			 enum dma_data_direction direction)
{
	struct tx_isp_t21_v4l2_attachment *private = attachment->priv;

	if (private->direction == direction)
		return &private->table;
	if (private->direction != DMA_NONE)
		dma_unmap_sg(attachment->dev, private->table.sgl,
			     private->table.orig_nents, private->direction);
	private->table.nents = dma_map_sg(attachment->dev, private->table.sgl,
					  private->table.orig_nents, direction);
	if (!private->table.nents) {
		private->direction = DMA_NONE;
		return ERR_PTR(-EIO);
	}
	private->direction = direction;
	return &private->table;
}

static void tx_isp_t21_dmabuf_unmap(struct dma_buf_attachment *attachment,
				     struct sg_table *table,
				     enum dma_data_direction direction)
{
}

static int tx_isp_t21_dmabuf_mmap(struct dma_buf *dma_buffer,
				   struct vm_area_struct *area)
{
	struct tx_isp_t21_v4l2_export *export = dma_buffer->priv;
	struct tx_isp_t21_v4l2_buffer *buffer = export->buffer;
	u32 length = area->vm_end - area->vm_start;

	if (length > buffer->size)
		return -EINVAL;
	area->vm_pgoff = 0;
	return dma_mmap_coherent(buffer->device, area, buffer->cpu_address,
				 buffer->dma_address, buffer->size);
}

static void *tx_isp_t21_dmabuf_kmap(struct dma_buf *dma_buffer,
				     unsigned long page)
{
	struct tx_isp_t21_v4l2_export *export = dma_buffer->priv;

	return export->buffer->cpu_address + page * PAGE_SIZE;
}

static void *tx_isp_t21_dmabuf_vmap(struct dma_buf *dma_buffer)
{
	struct tx_isp_t21_v4l2_export *export = dma_buffer->priv;

	return export->buffer->cpu_address;
}

static void tx_isp_t21_dmabuf_release(struct dma_buf *dma_buffer)
{
	struct tx_isp_t21_v4l2_export *export = dma_buffer->priv;

	tx_isp_t21_buffer_put(export->buffer);
	kfree(export);
}

static const struct dma_buf_ops tx_isp_t21_dmabuf_ops = {
	.attach = tx_isp_t21_dmabuf_attach,
	.detach = tx_isp_t21_dmabuf_detach,
	.map_dma_buf = tx_isp_t21_dmabuf_map,
	.unmap_dma_buf = tx_isp_t21_dmabuf_unmap,
	.kmap_atomic = tx_isp_t21_dmabuf_kmap,
	.kmap = tx_isp_t21_dmabuf_kmap,
	.vmap = tx_isp_t21_dmabuf_vmap,
	.mmap = tx_isp_t21_dmabuf_mmap,
	.release = tx_isp_t21_dmabuf_release,
};

static void tx_isp_t21_release_buffers(struct tx_isp_t21_v4l2_device *video,
					bool notify_capture)
{
	struct v4l2_requestbuffers request;
	u32 index;

	if (notify_capture && video->buffer_count) {
		memset(&request, 0, sizeof(request));
		request.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		request.memory = V4L2_MEMORY_USERPTR;
		tx_isp_t21_capture_ioctl(TX_ISP_T21_CAPTURE_CHANNEL,
					 TX_ISP_T21_FRAME_IOCTL_REQBUFS,
					 &request);
	}
	for (index = 0; index < TX_ISP_T21_MAX_BUFFERS; index++) {
		tx_isp_t21_buffer_put(video->buffers[index]);
		video->buffers[index] = NULL;
	}
	video->buffer_count = 0;
}

static void tx_isp_t21_release_pool(struct tx_isp_t21_v4l2_device *video)
{
	u32 index;

	for (index = 0; index < TX_ISP_T21_MAX_BUFFERS; index++) {
		tx_isp_t21_buffer_put(video->pool_buffers[index]);
		video->pool_buffers[index] = NULL;
	}
	video->pool_count = 0;
	video->pool_size = 0;
}

static int tx_isp_t21_prepare_pool(struct tx_isp_t21_v4l2_device *video,
				    u32 count, u32 size)
{
	u32 aligned_size = PAGE_ALIGN(size);
	u32 index;

	/*
	 * A 1080p NV12 buffer needs an order-10 coherent allocation on T21.
	 * Keep the pool across userspace closes so a service restart does not
	 * depend on finding two new contiguous 4 MiB regions after boot.
	 */
	if (video->pool_size && video->pool_size < aligned_size) {
		for (index = 0; index < video->pool_count; index++) {
			if (atomic_read(&video->pool_buffers[index]->references) != 1)
				return -EBUSY;
		}
		tx_isp_t21_release_pool(video);
	}
	if (!video->pool_size)
		video->pool_size = aligned_size;
	for (index = video->pool_count; index < count; index++) {
		video->pool_buffers[index] = tx_isp_t21_buffer_alloc(
			video->dma_device, video->pool_size);
		if (!video->pool_buffers[index])
			return -ENOMEM;
		video->pool_count++;
	}
	return 0;
}

static void tx_isp_t21_try_format(struct v4l2_format *format)
{
	u32 width = format->fmt.pix.width;
	u32 height = format->fmt.pix.height;

	if (!width)
		width = TX_ISP_T21_MAX_WIDTH;
	if (!height)
		height = TX_ISP_T21_MAX_HEIGHT;
	width = clamp_t(u32, width, TX_ISP_T21_MIN_WIDTH,
			TX_ISP_T21_MAX_WIDTH) & ~1U;
	height = clamp_t(u32, height, TX_ISP_T21_MIN_HEIGHT,
			 TX_ISP_T21_MAX_HEIGHT) & ~1U;
	memset(&format->fmt.pix, 0, sizeof(format->fmt.pix));
	format->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	format->fmt.pix.width = width;
	format->fmt.pix.height = height;
	format->fmt.pix.pixelformat = V4L2_PIX_FMT_NV12;
	format->fmt.pix.field = V4L2_FIELD_NONE;
	format->fmt.pix.bytesperline = width;
	format->fmt.pix.sizeimage = width * ALIGN(height, 16) * 3 / 2;
	format->fmt.pix.colorspace = V4L2_COLORSPACE_REC709;
}

static int tx_isp_t21_querycap(struct file *file, void *private,
			       struct v4l2_capability *capability)
{
	strlcpy(capability->driver, "tx-isp-t21", sizeof(capability->driver));
	strlcpy(capability->card, "Ingenic T21 ISP capture",
		sizeof(capability->card));
	strlcpy(capability->bus_info, "platform:tx-isp-t21",
		sizeof(capability->bus_info));
	capability->version = KERNEL_VERSION(0, 1, 0);
	capability->device_caps = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING;
	capability->capabilities = capability->device_caps |
		V4L2_CAP_DEVICE_CAPS;
	return 0;
}

static int tx_isp_t21_get_format(struct file *file, void *private,
				 struct v4l2_format *format)
{
	struct tx_isp_t21_v4l2_device *video = video_drvdata(file);

	if (format->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;
	format->fmt.pix = video->format;
	return 0;
}

static int tx_isp_t21_try_format_ioctl(struct file *file, void *private,
				       struct v4l2_format *format)
{
	if (format->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;
	tx_isp_t21_try_format(format);
	return 0;
}

static int tx_isp_t21_set_format(struct file *file, void *private,
				 struct v4l2_format *format)
{
	struct tx_isp_t21_v4l2_device *video = video_drvdata(file);
	struct tx_isp_t21_legacy_format legacy;
	long ret;

	if (format->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;
	if (video->streaming || video->buffer_count)
		return -EBUSY;
	tx_isp_t21_try_format(format);
	memset(&legacy, 0, sizeof(legacy));
	legacy.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	legacy.pix = format->fmt.pix;
	legacy.pix.field = V4L2_FIELD_INTERLACED;
	legacy.pix.colorspace = V4L2_COLORSPACE_SRGB;
	ret = tx_isp_t21_capture_ioctl(TX_ISP_T21_CAPTURE_CHANNEL,
				       TX_ISP_T21_FRAME_IOCTL_SET_FORMAT,
				       &legacy);
	if (ret)
		return ret;
	format->fmt.pix.width = legacy.pix.width;
	format->fmt.pix.height = legacy.pix.height;
	format->fmt.pix.pixelformat = legacy.pix.pixelformat;
	format->fmt.pix.bytesperline = legacy.pix.bytesperline;
	format->fmt.pix.sizeimage = legacy.pix.sizeimage;
	format->fmt.pix.field = V4L2_FIELD_NONE;
	format->fmt.pix.colorspace = V4L2_COLORSPACE_REC709;
	video->format = format->fmt.pix;
	return 0;
}

static int tx_isp_t21_request_buffers(struct file *file, void *private,
				      struct v4l2_requestbuffers *request)
{
	struct tx_isp_t21_v4l2_device *video = video_drvdata(file);
	struct v4l2_requestbuffers legacy;
	u32 count;
	u32 index;
	long ret;

	if (request->type != V4L2_BUF_TYPE_VIDEO_CAPTURE ||
	    request->memory != V4L2_MEMORY_MMAP)
		return -EINVAL;
	if (video->streaming)
		return -EBUSY;
	tx_isp_t21_release_buffers(video, true);
	if (!request->count) {
		request->count = 0;
		return 0;
	}
	count = clamp_t(u32, request->count, TX_ISP_T21_MIN_BUFFERS,
			TX_ISP_T21_MAX_BUFFERS);
	ret = tx_isp_t21_prepare_pool(video, count, video->format.sizeimage);
	if (ret)
		return ret;
	for (index = 0; index < count; index++) {
		video->buffers[index] = video->pool_buffers[index];
		tx_isp_t21_buffer_get(video->buffers[index]);
	}
	video->buffer_count = count;
	memset(&legacy, 0, sizeof(legacy));
	legacy.count = count;
	legacy.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	legacy.memory = V4L2_MEMORY_USERPTR;
	ret = tx_isp_t21_capture_ioctl(TX_ISP_T21_CAPTURE_CHANNEL,
				       TX_ISP_T21_FRAME_IOCTL_REQBUFS,
				       &legacy);
	if (ret || legacy.count < count) {
		tx_isp_t21_release_buffers(video, !ret);
		return ret ? ret : -ENOMEM;
	}
	request->count = count;
	return 0;
}

static int tx_isp_t21_query_buffer(struct file *file, void *private,
				   struct v4l2_buffer *buffer)
{
	struct tx_isp_t21_v4l2_device *video = video_drvdata(file);

	if (buffer->type != V4L2_BUF_TYPE_VIDEO_CAPTURE ||
	    buffer->memory != V4L2_MEMORY_MMAP ||
	    buffer->index >= video->buffer_count)
		return -EINVAL;
	buffer->length = video->buffers[buffer->index]->size;
	buffer->m.offset = buffer->index * PAGE_ALIGN(video->format.sizeimage);
	buffer->field = V4L2_FIELD_NONE;
	return 0;
}

static int tx_isp_t21_export_buffer(struct file *file, void *private,
				    struct v4l2_exportbuffer *request)
{
	struct tx_isp_t21_v4l2_device *video = video_drvdata(file);
	struct tx_isp_t21_v4l2_export *export;
	struct dma_buf *dma_buffer;
	int fd;

	if (request->type != V4L2_BUF_TYPE_VIDEO_CAPTURE ||
	    request->index >= video->buffer_count || request->plane)
		return -EINVAL;
	export = kzalloc(sizeof(*export), GFP_KERNEL);
	if (!export)
		return -ENOMEM;
	export->buffer = video->buffers[request->index];
	tx_isp_t21_buffer_get(export->buffer);
	dma_buffer = dma_buf_export(export, &tx_isp_t21_dmabuf_ops,
				    export->buffer->size, O_RDWR);
	if (IS_ERR(dma_buffer)) {
		fd = PTR_ERR(dma_buffer);
		tx_isp_t21_buffer_put(export->buffer);
		kfree(export);
		return fd;
	}
	fd = dma_buf_fd(dma_buffer, O_RDWR | (request->flags & O_CLOEXEC));
	if (fd < 0) {
		dma_buf_put(dma_buffer);
		return fd;
	}
	request->fd = fd;
	return 0;
}

static int tx_isp_t21_queue_buffer(struct file *file, void *private,
				   struct v4l2_buffer *buffer)
{
	struct tx_isp_t21_v4l2_device *video = video_drvdata(file);
	struct v4l2_buffer legacy;
	long ret;

	if (buffer->type != V4L2_BUF_TYPE_VIDEO_CAPTURE ||
	    buffer->memory != V4L2_MEMORY_MMAP ||
	    buffer->index >= video->buffer_count)
		return -EINVAL;
	memset(&legacy, 0, sizeof(legacy));
	legacy.index = buffer->index;
	legacy.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	legacy.field = V4L2_FIELD_INTERLACED;
	legacy.memory = V4L2_MEMORY_USERPTR;
	legacy.m.userptr = (unsigned long)
		video->buffers[buffer->index]->dma_address;
	legacy.length = video->format.sizeimage;
	ret = tx_isp_t21_capture_ioctl(TX_ISP_T21_CAPTURE_CHANNEL,
				       TX_ISP_T21_FRAME_IOCTL_QBUF, &legacy);
	if (!ret)
		buffer->flags |= V4L2_BUF_FLAG_QUEUED;
	return ret;
}

static int tx_isp_t21_dequeue_buffer(struct file *file, void *private,
				     struct v4l2_buffer *buffer)
{
	struct tx_isp_t21_v4l2_device *video = video_drvdata(file);
	struct v4l2_buffer legacy;
	long ret;

	if (buffer->type != V4L2_BUF_TYPE_VIDEO_CAPTURE ||
	    buffer->memory != V4L2_MEMORY_MMAP)
		return -EINVAL;
	memset(&legacy, 0, sizeof(legacy));
	legacy.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	legacy.memory = V4L2_MEMORY_USERPTR;
	ret = tx_isp_t21_capture_ioctl(TX_ISP_T21_CAPTURE_CHANNEL,
				       TX_ISP_T21_FRAME_IOCTL_DQBUF, &legacy);
	if (ret)
		return ret;
	if (legacy.index >= video->buffer_count)
		return -EIO;
	buffer->index = legacy.index;
	buffer->bytesused = legacy.bytesused;
	buffer->flags = legacy.flags;
	buffer->field = V4L2_FIELD_NONE;
	buffer->timestamp = legacy.timestamp;
	buffer->sequence = legacy.sequence;
	buffer->memory = V4L2_MEMORY_MMAP;
	buffer->m.offset = buffer->index * PAGE_ALIGN(video->format.sizeimage);
	buffer->length = video->buffers[buffer->index]->size;
	return 0;
}

static int tx_isp_t21_stream_on(struct file *file, void *private,
				enum v4l2_buf_type type)
{
	struct tx_isp_t21_v4l2_device *video = video_drvdata(file);
	u32 legacy_type = type;
	long ret;

	if (type != V4L2_BUF_TYPE_VIDEO_CAPTURE || !video->buffer_count)
		return -EINVAL;
	if (video->streaming)
		return -EBUSY;
	ret = tx_isp_t21_capture_stream(1);
	if (ret)
		return ret;
	video->upstream_streaming = true;
	ret = tx_isp_t21_capture_ioctl(TX_ISP_T21_CAPTURE_CHANNEL,
				       TX_ISP_T21_FRAME_IOCTL_STREAM_ON,
				       &legacy_type);
	if (ret) {
		if (!tx_isp_t21_capture_stream(0))
			video->upstream_streaming = false;
		return ret;
	}
	video->streaming = true;
	return 0;
}

static int tx_isp_t21_stop_streaming(struct tx_isp_t21_v4l2_device *video,
				      u32 type)
{
	long channel_ret = 0;
	int upstream_ret = 0;

	if (video->streaming) {
		channel_ret = tx_isp_t21_capture_ioctl(
			TX_ISP_T21_CAPTURE_CHANNEL,
			TX_ISP_T21_FRAME_IOCTL_STREAM_OFF, &type);
		if (!channel_ret)
			video->streaming = false;
	}
	if (video->upstream_streaming) {
		upstream_ret = tx_isp_t21_capture_stream(0);
		if (!upstream_ret)
			video->upstream_streaming = false;
	}
	return channel_ret ? channel_ret : upstream_ret;
}

static int tx_isp_t21_stream_off(struct file *file, void *private,
				 enum v4l2_buf_type type)
{
	struct tx_isp_t21_v4l2_device *video = video_drvdata(file);

	if (type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;
	if (!video->streaming && !video->upstream_streaming)
		return 0;
	return tx_isp_t21_stop_streaming(video, type);
}

static const struct v4l2_ioctl_ops tx_isp_t21_ioctl_ops = {
	.vidioc_querycap = tx_isp_t21_querycap,
	.vidioc_g_fmt_vid_cap = tx_isp_t21_get_format,
	.vidioc_try_fmt_vid_cap = tx_isp_t21_try_format_ioctl,
	.vidioc_s_fmt_vid_cap = tx_isp_t21_set_format,
	.vidioc_reqbufs = tx_isp_t21_request_buffers,
	.vidioc_querybuf = tx_isp_t21_query_buffer,
	.vidioc_expbuf = tx_isp_t21_export_buffer,
	.vidioc_qbuf = tx_isp_t21_queue_buffer,
	.vidioc_dqbuf = tx_isp_t21_dequeue_buffer,
	.vidioc_streamon = tx_isp_t21_stream_on,
	.vidioc_streamoff = tx_isp_t21_stream_off,
};

static int tx_isp_t21_open(struct file *file)
{
	struct tx_isp_t21_v4l2_device *video = video_drvdata(file);
	struct v4l2_format format;
	int ret = 0;

	mutex_lock(&video->lock);
	if (video->opened) {
		ret = -EBUSY;
	} else {
		ret = tx_isp_t21_capture_open(TX_ISP_T21_CAPTURE_CHANNEL);
		if (!ret) {
			memset(&format, 0, sizeof(format));
			tx_isp_t21_try_format(&format);
			video->format = format.fmt.pix;
			video->opened = true;
		}
	}
	mutex_unlock(&video->lock);
	return ret;
}

static int tx_isp_t21_release(struct file *file)
{
	struct tx_isp_t21_v4l2_device *video = video_drvdata(file);
	u32 type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	mutex_lock(&video->lock);
	if (video->streaming || video->upstream_streaming)
		tx_isp_t21_stop_streaming(video, type);
	tx_isp_t21_release_buffers(video, true);
	tx_isp_t21_capture_release(TX_ISP_T21_CAPTURE_CHANNEL);
	video->opened = false;
	mutex_unlock(&video->lock);
	return 0;
}

static unsigned int tx_isp_t21_poll(struct file *file,
				    struct poll_table_struct *wait)
{
	return tx_isp_t21_capture_poll(TX_ISP_T21_CAPTURE_CHANNEL, file, wait);
}

static const struct v4l2_file_operations tx_isp_t21_file_ops = {
	.owner = THIS_MODULE,
	.open = tx_isp_t21_open,
	.release = tx_isp_t21_release,
	.unlocked_ioctl = video_ioctl2,
	.poll = tx_isp_t21_poll,
};

static long tx_isp_t21_resolve_dma(struct file *file, unsigned int command,
				   unsigned long argument)
{
	struct tx_isp_t21_dma_info info;
	struct tx_isp_t21_v4l2_export *export;
	struct dma_buf *dma_buffer;
	long ret = 0;

	if (command != TX_ISP_T21_GET_DMA_PHY || !argument)
		return -ENOIOCTLCMD;
	if (copy_from_user(&info, (void __user *)argument, sizeof(info)))
		return -EFAULT;
	dma_buffer = dma_buf_get((int)info.fd);
	if (IS_ERR(dma_buffer))
		return PTR_ERR(dma_buffer);
	if (dma_buffer->ops != &tx_isp_t21_dmabuf_ops) {
		ret = -EINVAL;
		goto out;
	}
	export = dma_buffer->priv;
	if (!export || !export->buffer || !info.size ||
	    info.size > export->buffer->size) {
		ret = -EINVAL;
		goto out;
	}
	info.physical_address = (u32)export->buffer->dma_address;
	if (copy_to_user((void __user *)argument, &info, sizeof(info)))
		ret = -EFAULT;
out:
	dma_buf_put(dma_buffer);
	return ret;
}

static const struct file_operations tx_isp_t21_resolver_ops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = tx_isp_t21_resolve_dma,
};

static struct miscdevice tx_isp_t21_resolver = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "tx-isp-t21-dmabuf",
	.fops = &tx_isp_t21_resolver_ops,
};

int tx_isp_t21_v4l2_init(void)
{
	struct tx_isp_t21_v4l2_device *video = &tx_isp_t21_video;
	struct v4l2_format format;
	int ret;

	memset(video, 0, sizeof(*video));
	video->dma_device = tx_isp_t21_capture_device();
	if (!video->dma_device)
		return -ENODEV;
	mutex_init(&video->lock);
	memset(&format, 0, sizeof(format));
	tx_isp_t21_try_format(&format);
	video->format = format.fmt.pix;
	ret = v4l2_device_register(video->dma_device, &video->v4l2_device);
	if (ret)
		return ret;
	video->video_device = video_device_alloc();
	if (!video->video_device) {
		ret = -ENOMEM;
		goto fail_v4l2;
	}
	strlcpy(video->video_device->name, "tx-isp-t21-capture",
		sizeof(video->video_device->name));
	video->video_device->v4l2_dev = &video->v4l2_device;
	video->video_device->fops = &tx_isp_t21_file_ops;
	video->video_device->ioctl_ops = &tx_isp_t21_ioctl_ops;
	video->video_device->release = video_device_release;
	video->video_device->lock = &video->lock;
	video->video_device->vfl_dir = VFL_DIR_RX;
	video_set_drvdata(video->video_device, video);
	ret = video_register_device(video->video_device, VFL_TYPE_GRABBER, -1);
	if (ret)
		goto fail_video;
	video->registered = true;
	ret = misc_register(&tx_isp_t21_resolver);
	if (ret)
		goto fail_registered_video;
	video->resolver_registered = true;
	pr_info("tx-isp-t21: public V4L2 capture registered as /dev/video%d\n",
		video->video_device->num);
	return 0;

fail_registered_video:
	video_unregister_device(video->video_device);
	video->video_device = NULL;
	video->registered = false;
	v4l2_device_unregister(&video->v4l2_device);
	return ret;
fail_video:
	video_device_release(video->video_device);
	video->video_device = NULL;
fail_v4l2:
	v4l2_device_unregister(&video->v4l2_device);
	return ret;
}

void tx_isp_t21_v4l2_cleanup(void)
{
	struct tx_isp_t21_v4l2_device *video = &tx_isp_t21_video;
	u32 type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (video->resolver_registered) {
		misc_deregister(&tx_isp_t21_resolver);
		video->resolver_registered = false;
	}
	if (!video->registered)
		return;
	if (video->streaming || video->upstream_streaming)
		tx_isp_t21_stop_streaming(video, type);
	video->streaming = false;
	video->upstream_streaming = false;
	tx_isp_t21_release_buffers(video, true);
	tx_isp_t21_release_pool(video);
	video_unregister_device(video->video_device);
	video->video_device = NULL;
	video->registered = false;
	v4l2_device_unregister(&video->v4l2_device);
}
