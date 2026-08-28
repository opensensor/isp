// SPDX-License-Identifier: GPL-2.0
/* Public V4L2 MMAP/DMA-BUF adapter for the T31 private frame channel. */

#include <linux/dma-buf.h>
#include <linux/dma-mapping.h>
#include <linux/fs.h>
#include <linux/kernel.h>
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

#include "include/tx_isp.h"
#include "include/tx_isp_device.h"
#include "../include/tx_isp/tx_isp_frame_channel.h"

#define TX_ISP_V4L2_CHANNEL 0
#define TX_ISP_V4L2_MIN_WIDTH 16U
#define TX_ISP_V4L2_MIN_HEIGHT 16U
#define TX_ISP_V4L2_MIN_BUFFERS 2U
#define TX_ISP_V4L2_MAX_BUFFERS 4U

struct tx_isp_v4l2_buffer {
	struct device *device;
	void *cpu_address;
	dma_addr_t dma_address;
	u32 size;
	atomic_t references;
};

struct tx_isp_v4l2_attachment {
	struct sg_table table;
	enum dma_data_direction direction;
};

struct tx_isp_v4l2_export {
	struct tx_isp_v4l2_buffer *buffer;
};

struct tx_isp_v4l2_device {
	struct v4l2_device v4l2_device;
	struct video_device *video_device;
	struct device *dma_device;
	struct mutex lock;
	struct v4l2_pix_format format;
	struct tx_isp_v4l2_buffer *buffers[TX_ISP_V4L2_MAX_BUFFERS];
	u32 buffer_count;
	bool opened;
	bool upstream_streaming;
	bool streaming;
	bool registered;
};

static struct tx_isp_v4l2_device tx_isp_video;

extern struct tx_isp_dev *ourISPdev;
extern int tx_isp_video_s_stream(struct tx_isp_dev *dev, int enable);
extern long frame_channel_unlocked_ioctl(struct file *file,
					 unsigned int command,
					 unsigned long argument);

static long tx_isp_v4l2_legacy_ioctl(unsigned int command, void *argument)
{
	struct file legacy_file;
	mm_segment_t old_fs;
	long ret;

	memset(&legacy_file, 0, sizeof(legacy_file));
	legacy_file.private_data = &frame_channels[TX_ISP_V4L2_CHANNEL];
	old_fs = get_fs();
	set_fs(KERNEL_DS);
	ret = frame_channel_unlocked_ioctl(&legacy_file, command,
					   (unsigned long)argument);
	set_fs(old_fs);
	return ret;
}

static void tx_isp_v4l2_buffer_get(struct tx_isp_v4l2_buffer *buffer)
{
	atomic_inc(&buffer->references);
}

static void tx_isp_v4l2_buffer_put(struct tx_isp_v4l2_buffer *buffer)
{
	if (!buffer || !atomic_dec_and_test(&buffer->references))
		return;
	dma_free_coherent(buffer->device, buffer->size, buffer->cpu_address,
			  buffer->dma_address);
	put_device(buffer->device);
	kfree(buffer);
}

static struct tx_isp_v4l2_buffer *
tx_isp_v4l2_buffer_alloc(struct device *device, u32 size)
{
	struct tx_isp_v4l2_buffer *buffer;

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

static int tx_isp_v4l2_dmabuf_attach(struct dma_buf *dma_buffer,
				     struct device *device,
				     struct dma_buf_attachment *attachment)
{
	struct tx_isp_v4l2_export *export = dma_buffer->priv;
	struct tx_isp_v4l2_attachment *private;
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

static void tx_isp_v4l2_dmabuf_detach(struct dma_buf *dma_buffer,
				      struct dma_buf_attachment *attachment)
{
	struct tx_isp_v4l2_attachment *private = attachment->priv;

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
tx_isp_v4l2_dmabuf_map(struct dma_buf_attachment *attachment,
			 enum dma_data_direction direction)
{
	struct tx_isp_v4l2_attachment *private = attachment->priv;

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

static void tx_isp_v4l2_dmabuf_unmap(struct dma_buf_attachment *attachment,
				     struct sg_table *table,
				     enum dma_data_direction direction)
{
}

static int tx_isp_v4l2_dmabuf_mmap(struct dma_buf *dma_buffer,
				   struct vm_area_struct *area)
{
	struct tx_isp_v4l2_export *export = dma_buffer->priv;
	struct tx_isp_v4l2_buffer *buffer = export->buffer;
	u32 length = area->vm_end - area->vm_start;

	if (length > buffer->size)
		return -EINVAL;
	area->vm_pgoff = 0;
	return dma_mmap_coherent(buffer->device, area, buffer->cpu_address,
				 buffer->dma_address, buffer->size);
}

static void *tx_isp_v4l2_dmabuf_kmap(struct dma_buf *dma_buffer,
				     unsigned long page)
{
	struct tx_isp_v4l2_export *export = dma_buffer->priv;

	return export->buffer->cpu_address + page * PAGE_SIZE;
}

static void *tx_isp_v4l2_dmabuf_vmap(struct dma_buf *dma_buffer)
{
	struct tx_isp_v4l2_export *export = dma_buffer->priv;

	return export->buffer->cpu_address;
}

static void tx_isp_v4l2_dmabuf_release(struct dma_buf *dma_buffer)
{
	struct tx_isp_v4l2_export *export = dma_buffer->priv;

	tx_isp_v4l2_buffer_put(export->buffer);
	kfree(export);
}

static const struct dma_buf_ops tx_isp_v4l2_dmabuf_ops = {
	.attach = tx_isp_v4l2_dmabuf_attach,
	.detach = tx_isp_v4l2_dmabuf_detach,
	.map_dma_buf = tx_isp_v4l2_dmabuf_map,
	.unmap_dma_buf = tx_isp_v4l2_dmabuf_unmap,
	.kmap_atomic = tx_isp_v4l2_dmabuf_kmap,
	.kmap = tx_isp_v4l2_dmabuf_kmap,
	.vmap = tx_isp_v4l2_dmabuf_vmap,
	.mmap = tx_isp_v4l2_dmabuf_mmap,
	.release = tx_isp_v4l2_dmabuf_release,
};

static void tx_isp_v4l2_deactivate_buffers(struct tx_isp_v4l2_device *video,
					   bool notify_legacy)
{
	struct tx_isp_frame_request_wire request;

	if (notify_legacy && video->buffer_count) {
		memset(&request, 0, sizeof(request));
		request.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		request.memory = V4L2_MEMORY_USERPTR;
		tx_isp_v4l2_legacy_ioctl(TX_ISP_FRAME_IOCTL_LEGACY_REQBUFS,
					  &request);
	}
	video->buffer_count = 0;
}

static void tx_isp_v4l2_free_buffers(struct tx_isp_v4l2_device *video)
{
	u32 index;

	for (index = 0; index < TX_ISP_V4L2_MAX_BUFFERS; index++) {
		tx_isp_v4l2_buffer_put(video->buffers[index]);
		video->buffers[index] = NULL;
	}
}

static void tx_isp_v4l2_try_format(struct v4l2_format *format)
{
	u32 width = format->fmt.pix.width;
	u32 height = format->fmt.pix.height;
	u32 sensor_width;
	u32 sensor_height;

	if ((!width || !height) && ourISPdev &&
	    !tx_isp_sensor_active_dimensions(ourISPdev->sensor,
					     &sensor_width, &sensor_height)) {
		if (!width)
			width = sensor_width;
		if (!height)
			height = sensor_height;
	}
	/* Registration can precede an external sensor module.  Advertise the
	 * SoC envelope until sensor sync supplies active geometry. */
	if (!width)
		width = TX_ISP_MAX_WIDTH;
	if (!height)
		height = TX_ISP_MAX_HEIGHT;
	width = clamp_t(u32, width, TX_ISP_V4L2_MIN_WIDTH,
			TX_ISP_MAX_WIDTH) & ~1U;
	height = clamp_t(u32, height, TX_ISP_V4L2_MIN_HEIGHT,
			 TX_ISP_MAX_HEIGHT) & ~1U;
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

static int tx_isp_v4l2_querycap(struct file *file, void *private,
				struct v4l2_capability *capability)
{
	strlcpy(capability->driver, "tx-isp-t31", sizeof(capability->driver));
	strlcpy(capability->card, "Ingenic T31 ISP capture",
		sizeof(capability->card));
	strlcpy(capability->bus_info, "platform:tx-isp-t31",
		sizeof(capability->bus_info));
	capability->version = KERNEL_VERSION(0, 1, 0);
	capability->device_caps = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING;
	capability->capabilities = capability->device_caps |
		V4L2_CAP_DEVICE_CAPS;
	return 0;
}

static int tx_isp_v4l2_enum_format(struct file *file, void *private,
				   struct v4l2_fmtdesc *format)
{
	if (format->index)
		return -EINVAL;
	format->pixelformat = V4L2_PIX_FMT_NV12;
	strlcpy(format->description, "NV12", sizeof(format->description));
	return 0;
}

static int tx_isp_v4l2_get_format(struct file *file, void *private,
				  struct v4l2_format *format)
{
	struct tx_isp_v4l2_device *video = video_drvdata(file);

	if (format->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;
	format->fmt.pix = video->format;
	return 0;
}

static int tx_isp_v4l2_try_format_ioctl(struct file *file, void *private,
					struct v4l2_format *format)
{
	if (format->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;
	tx_isp_v4l2_try_format(format);
	return 0;
}

static int tx_isp_v4l2_set_format(struct file *file, void *private,
				  struct v4l2_format *format)
{
	struct tx_isp_v4l2_device *video = video_drvdata(file);
	struct frame_image_format legacy_format;
	long ret;

	if (format->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;
	if (video->streaming || video->buffer_count)
		return -EBUSY;
	tx_isp_v4l2_try_format(format);
	memset(&legacy_format, 0, sizeof(legacy_format));
	legacy_format.type = format->type;
	legacy_format.pix.width = format->fmt.pix.width;
	legacy_format.pix.height = format->fmt.pix.height;
	legacy_format.pix.pixelformat = format->fmt.pix.pixelformat;
	legacy_format.pix.field = format->fmt.pix.field;
	legacy_format.pix.bytesperline = format->fmt.pix.bytesperline;
	legacy_format.pix.sizeimage = format->fmt.pix.sizeimage;
	legacy_format.pix.colorspace = format->fmt.pix.colorspace;
	legacy_format.pix.priv = format->fmt.pix.priv;
	ret = tx_isp_v4l2_legacy_ioctl(TX_ISP_FRAME_IOCTL_LEGACY_SET_FORMAT,
					       &legacy_format);
	if (ret)
		return ret;
	format->fmt.pix.width = legacy_format.pix.width;
	format->fmt.pix.height = legacy_format.pix.height;
	format->fmt.pix.pixelformat = legacy_format.pix.pixelformat;
	format->fmt.pix.field = legacy_format.pix.field;
	format->fmt.pix.bytesperline = legacy_format.pix.bytesperline;
	format->fmt.pix.sizeimage = legacy_format.pix.sizeimage;
	format->fmt.pix.colorspace = legacy_format.pix.colorspace;
	format->fmt.pix.priv = legacy_format.pix.priv;
	video->format = format->fmt.pix;
	return 0;
}

static int tx_isp_v4l2_get_parameters(struct file *file, void *private,
				      struct v4l2_streamparm *parameters)
{
	u32 raw_fps;
	u32 numerator;
	u32 denominator;

	if (parameters->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;
	if (!ourISPdev || !ourISPdev->sensor)
		return -ENODEV;

	raw_fps = ourISPdev->sensor->video.fps;
	numerator = raw_fps >> 16;
	denominator = raw_fps & 0xffffU;
	if (!numerator || !denominator)
		return -EINVAL;

	memset(&parameters->parm.capture, 0,
	       sizeof(parameters->parm.capture));
	parameters->parm.capture.capability = V4L2_CAP_TIMEPERFRAME;
	parameters->parm.capture.timeperframe.numerator = denominator;
	parameters->parm.capture.timeperframe.denominator = numerator;
	parameters->parm.capture.readbuffers = TX_ISP_V4L2_MIN_BUFFERS;
	return 0;
}

static int tx_isp_v4l2_request_buffers(struct file *file, void *private,
				       struct v4l2_requestbuffers *request)
{
	struct tx_isp_v4l2_device *video = video_drvdata(file);
	struct tx_isp_frame_request_wire legacy_request;
	u32 count;
	u32 index;
	long ret;

	if (request->type != V4L2_BUF_TYPE_VIDEO_CAPTURE ||
	    request->memory != V4L2_MEMORY_MMAP)
		return -EINVAL;
	if (video->streaming)
		return -EBUSY;
	tx_isp_v4l2_deactivate_buffers(video, true);
	if (!request->count) {
		request->count = 0;
		return 0;
	}
	count = clamp_t(u32, request->count, TX_ISP_V4L2_MIN_BUFFERS,
			TX_ISP_V4L2_MAX_BUFFERS);
	for (index = 0; index < count; index++) {
		struct tx_isp_v4l2_buffer *buffer = video->buffers[index];
		u32 size = PAGE_ALIGN(video->format.sizeimage);

		/* Keep the physically contiguous capture pool across userspace
		 * restarts.  Reallocating two order-10 1080p buffers after boot is
		 * unreliable on 64 MiB parts once the 42 MiB kernel heap has
		 * fragmented.  The pool owns one reference; exported DMA-BUFs own
		 * additional references and must be gone before a slot is reused. */
		if (buffer && atomic_read(&buffer->references) != 1)
			return -EBUSY;
		if (buffer && buffer->size != size) {
			tx_isp_v4l2_buffer_put(buffer);
			video->buffers[index] = NULL;
		}
		if (!video->buffers[index])
			video->buffers[index] = tx_isp_v4l2_buffer_alloc(
				video->dma_device, video->format.sizeimage);
		if (!video->buffers[index]) {
			return -ENOMEM;
		}
	}
	video->buffer_count = count;
	memset(&legacy_request, 0, sizeof(legacy_request));
	legacy_request.count = count;
	legacy_request.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	legacy_request.memory = V4L2_MEMORY_USERPTR;
	ret = tx_isp_v4l2_legacy_ioctl(TX_ISP_FRAME_IOCTL_LEGACY_REQBUFS,
					       &legacy_request);
	if (ret) {
		tx_isp_v4l2_deactivate_buffers(video, false);
		return ret;
	}
	if (legacy_request.count < count) {
		tx_isp_v4l2_deactivate_buffers(video, true);
		return -ENOMEM;
	}
	request->count = count;
	return 0;
}

static int tx_isp_v4l2_query_buffer(struct file *file, void *private,
				    struct v4l2_buffer *buffer)
{
	struct tx_isp_v4l2_device *video = video_drvdata(file);
	struct tx_isp_v4l2_buffer *capture;

	if (buffer->type != V4L2_BUF_TYPE_VIDEO_CAPTURE ||
	    buffer->memory != V4L2_MEMORY_MMAP ||
	    buffer->index >= video->buffer_count)
		return -EINVAL;
	capture = video->buffers[buffer->index];
	buffer->length = capture->size;
	buffer->m.offset = buffer->index * PAGE_ALIGN(video->format.sizeimage);
	buffer->field = V4L2_FIELD_NONE;
	return 0;
}

static int tx_isp_v4l2_export_buffer(struct file *file, void *private,
				     struct v4l2_exportbuffer *request)
{
	struct tx_isp_v4l2_device *video = video_drvdata(file);
	struct tx_isp_v4l2_export *export;
	struct dma_buf *dma_buffer;
	int fd;

	if (request->type != V4L2_BUF_TYPE_VIDEO_CAPTURE ||
	    request->index >= video->buffer_count || request->plane)
		return -EINVAL;
	export = kzalloc(sizeof(*export), GFP_KERNEL);
	if (!export)
		return -ENOMEM;
	export->buffer = video->buffers[request->index];
	tx_isp_v4l2_buffer_get(export->buffer);
	dma_buffer = dma_buf_export(export, &tx_isp_v4l2_dmabuf_ops,
				    export->buffer->size, O_RDWR);
	if (IS_ERR(dma_buffer)) {
		fd = PTR_ERR(dma_buffer);
		tx_isp_v4l2_buffer_put(export->buffer);
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

static int tx_isp_v4l2_queue_buffer(struct file *file, void *private,
				    struct v4l2_buffer *buffer)
{
	struct tx_isp_v4l2_device *video = video_drvdata(file);
	struct tx_isp_frame_buffer_wire legacy_buffer;
	struct tx_isp_v4l2_buffer *capture;
	long ret;

	if (buffer->type != V4L2_BUF_TYPE_VIDEO_CAPTURE ||
	    buffer->memory != V4L2_MEMORY_MMAP ||
	    buffer->index >= video->buffer_count)
		return -EINVAL;
	capture = video->buffers[buffer->index];
	memset(&legacy_buffer, 0, sizeof(legacy_buffer));
	legacy_buffer.index = buffer->index;
	legacy_buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	legacy_buffer.field = V4L2_FIELD_NONE;
	legacy_buffer.memory = V4L2_MEMORY_USERPTR;
	legacy_buffer.dma = (u32)capture->dma_address;
	legacy_buffer.length = capture->size;
	ret = tx_isp_v4l2_legacy_ioctl(TX_ISP_FRAME_IOCTL_LEGACY_QBUF,
					       &legacy_buffer);
	if (!ret)
		buffer->flags |= V4L2_BUF_FLAG_QUEUED;
	return ret;
}

static int tx_isp_v4l2_dequeue_buffer(struct file *file, void *private,
				      struct v4l2_buffer *buffer)
{
	struct tx_isp_v4l2_device *video = video_drvdata(file);
	struct tx_isp_frame_buffer_wire legacy_buffer;
	long ret;

	if (buffer->type != V4L2_BUF_TYPE_VIDEO_CAPTURE ||
	    buffer->memory != V4L2_MEMORY_MMAP)
		return -EINVAL;
	memset(&legacy_buffer, 0, sizeof(legacy_buffer));
	legacy_buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	legacy_buffer.memory = V4L2_MEMORY_USERPTR;
	ret = tx_isp_v4l2_legacy_ioctl(TX_ISP_FRAME_IOCTL_LEGACY_DQBUF,
					       &legacy_buffer);
	if (ret)
		return ret;
	if (legacy_buffer.index >= video->buffer_count)
		return -EIO;
	buffer->index = legacy_buffer.index;
	buffer->bytesused = legacy_buffer.bytesused;
	buffer->flags = legacy_buffer.flags;
	buffer->field = legacy_buffer.field;
	buffer->timestamp.tv_sec = legacy_buffer.timestamp_sec;
	buffer->timestamp.tv_usec = legacy_buffer.timestamp_usec;
	buffer->sequence = legacy_buffer.sequence;
	buffer->memory = V4L2_MEMORY_MMAP;
	buffer->m.offset = buffer->index * PAGE_ALIGN(video->format.sizeimage);
	buffer->length = video->buffers[buffer->index]->size;
	return 0;
}

static int tx_isp_v4l2_stream_on(struct file *file, void *private,
				 enum v4l2_buf_type type)
{
	struct tx_isp_v4l2_device *video = video_drvdata(file);
	u32 legacy_type = type;
	int rollback_ret;
	long ret;

	if (type != V4L2_BUF_TYPE_VIDEO_CAPTURE || !video->buffer_count)
		return -EINVAL;
	if (video->streaming)
		return -EBUSY;
	if (!ourISPdev)
		return -ENODEV;

	ret = tx_isp_video_s_stream(ourISPdev, 1);
	if (ret)
		return ret;
	video->upstream_streaming = true;

	ret = tx_isp_v4l2_legacy_ioctl(TX_ISP_FRAME_IOCTL_LEGACY_STREAM_ON,
					       &legacy_type);
	if (ret) {
		rollback_ret = tx_isp_video_s_stream(ourISPdev, 0);
		if (!rollback_ret)
			video->upstream_streaming = false;
		return ret;
	}
	video->streaming = true;
	return 0;
}

static int tx_isp_v4l2_stop_streaming(struct tx_isp_v4l2_device *video,
				       u32 type)
{
	long channel_ret = 0;
	int upstream_ret = 0;

	if (video->streaming) {
		channel_ret = tx_isp_v4l2_legacy_ioctl(
			TX_ISP_FRAME_IOCTL_LEGACY_STREAM_OFF, &type);
		if (!channel_ret)
			video->streaming = false;
	}
	if (video->upstream_streaming) {
		upstream_ret = tx_isp_video_s_stream(ourISPdev, 0);
		if (!upstream_ret)
			video->upstream_streaming = false;
	}

	return channel_ret ? channel_ret : upstream_ret;
}

static int tx_isp_v4l2_stream_off(struct file *file, void *private,
				  enum v4l2_buf_type type)
{
	struct tx_isp_v4l2_device *video = video_drvdata(file);

	if (type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;
	if (!video->streaming && !video->upstream_streaming)
		return 0;
	return tx_isp_v4l2_stop_streaming(video, type);
}

static const struct v4l2_ioctl_ops tx_isp_v4l2_ioctl_ops = {
	.vidioc_querycap = tx_isp_v4l2_querycap,
	.vidioc_enum_fmt_vid_cap = tx_isp_v4l2_enum_format,
	.vidioc_g_fmt_vid_cap = tx_isp_v4l2_get_format,
	.vidioc_try_fmt_vid_cap = tx_isp_v4l2_try_format_ioctl,
	.vidioc_s_fmt_vid_cap = tx_isp_v4l2_set_format,
	.vidioc_g_parm = tx_isp_v4l2_get_parameters,
	.vidioc_reqbufs = tx_isp_v4l2_request_buffers,
	.vidioc_querybuf = tx_isp_v4l2_query_buffer,
	.vidioc_expbuf = tx_isp_v4l2_export_buffer,
	.vidioc_qbuf = tx_isp_v4l2_queue_buffer,
	.vidioc_dqbuf = tx_isp_v4l2_dequeue_buffer,
	.vidioc_streamon = tx_isp_v4l2_stream_on,
	.vidioc_streamoff = tx_isp_v4l2_stream_off,
};

static int tx_isp_v4l2_open(struct file *file)
{
	struct tx_isp_v4l2_device *video = video_drvdata(file);
	struct v4l2_format format;
	int ret = 0;

	mutex_lock(&video->lock);
	if (video->opened)
		ret = -EBUSY;
	else {
		memset(&format, 0, sizeof(format));
		tx_isp_v4l2_try_format(&format);
		video->format = format.fmt.pix;
		video->opened = true;
	}
	mutex_unlock(&video->lock);
	return ret;
}

static int tx_isp_v4l2_release(struct file *file)
{
	struct tx_isp_v4l2_device *video = video_drvdata(file);
	u32 type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	mutex_lock(&video->lock);
	if (video->streaming || video->upstream_streaming)
		tx_isp_v4l2_stop_streaming(video, type);
	tx_isp_v4l2_deactivate_buffers(video, true);
	video->opened = false;
	mutex_unlock(&video->lock);
	return 0;
}

static unsigned int tx_isp_v4l2_poll(struct file *file,
				     struct poll_table_struct *wait)
{
	struct tx_isp_channel_state *state =
		&frame_channels[TX_ISP_V4L2_CHANNEL].state;

	poll_wait(file, &state->frame_wait, wait);
	if (!state->streaming)
		return POLLERR;
	if (atomic_read(&state->frame_ready_count) > 0)
		return POLLIN | POLLRDNORM;
	return 0;
}

static int tx_isp_v4l2_mmap(struct file *file, struct vm_area_struct *area)
{
	struct tx_isp_v4l2_device *video = video_drvdata(file);
	struct tx_isp_v4l2_buffer *buffer;
	u32 offset = area->vm_pgoff << PAGE_SHIFT;
	u32 slot_size = PAGE_ALIGN(video->format.sizeimage);
	u32 index;
	int ret;

	if (!slot_size || offset % slot_size)
		return -EINVAL;
	index = offset / slot_size;
	if (index >= video->buffer_count)
		return -EINVAL;
	buffer = video->buffers[index];
	if (area->vm_end - area->vm_start > buffer->size)
		return -EINVAL;
	area->vm_pgoff = 0;
	ret = dma_mmap_coherent(buffer->device, area, buffer->cpu_address,
				buffer->dma_address, buffer->size);
	return ret;
}

static const struct v4l2_file_operations tx_isp_v4l2_file_ops = {
	.owner = THIS_MODULE,
	.open = tx_isp_v4l2_open,
	.release = tx_isp_v4l2_release,
	.unlocked_ioctl = video_ioctl2,
	.poll = tx_isp_v4l2_poll,
	.mmap = tx_isp_v4l2_mmap,
};

int tx_isp_v4l2_init(void)
{
	struct tx_isp_v4l2_device *video = &tx_isp_video;
	struct v4l2_format format;
	int ret;

	if (!ourISPdev || !ourISPdev->dev)
		return -ENODEV;
	memset(video, 0, sizeof(*video));
	video->dma_device = ourISPdev->dev;
	mutex_init(&video->lock);
	memset(&format, 0, sizeof(format));
	tx_isp_v4l2_try_format(&format);
	video->format = format.fmt.pix;
	ret = v4l2_device_register(video->dma_device, &video->v4l2_device);
	if (ret)
		return ret;
	video->video_device = video_device_alloc();
	if (!video->video_device) {
		ret = -ENOMEM;
		goto fail_v4l2;
	}
	strlcpy(video->video_device->name, "tx-isp-t31-capture",
		sizeof(video->video_device->name));
	video->video_device->v4l2_dev = &video->v4l2_device;
	video->video_device->fops = &tx_isp_v4l2_file_ops;
	video->video_device->ioctl_ops = &tx_isp_v4l2_ioctl_ops;
	video->video_device->release = video_device_release;
	video->video_device->lock = &video->lock;
	video->video_device->vfl_dir = VFL_DIR_RX;
	video_set_drvdata(video->video_device, video);
	ret = video_register_device(video->video_device, VFL_TYPE_GRABBER, -1);
	if (ret)
		goto fail_video;
	video->registered = true;
	pr_info("tx-isp-t31: public V4L2 capture registered as /dev/video%d\n",
		video->video_device->num);
	return 0;

fail_video:
	video_device_release(video->video_device);
	video->video_device = NULL;
fail_v4l2:
	v4l2_device_unregister(&video->v4l2_device);
	return ret;
}

void tx_isp_v4l2_cleanup(void)
{
	struct tx_isp_v4l2_device *video = &tx_isp_video;
	u32 type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (!video->registered)
		return;
	if (video->streaming || video->upstream_streaming)
		tx_isp_v4l2_stop_streaming(video, type);
	video->streaming = false;
	video->upstream_streaming = false;
	tx_isp_v4l2_deactivate_buffers(video, true);
	tx_isp_v4l2_free_buffers(video);
	video_unregister_device(video->video_device);
	video->video_device = NULL;
	video->registered = false;
	v4l2_device_unregister(&video->v4l2_device);
}
