// SPDX-License-Identifier: GPL-2.0
/* Minimal target-side ABI smoke test for the T41 V4L2 discovery node. */

#include <errno.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

static int checked_ioctl(int fd, unsigned long request, void *argument,
			 const char *name)
{
	int ret;

	do {
		ret = ioctl(fd, request, argument);
	} while (ret < 0 && errno == EINTR);
	if (ret < 0)
		fprintf(stderr, "%s: %s\n", name, strerror(errno));
	return ret;
}

int main(int argc, char **argv)
{
	const char *path = argc > 1 ? argv[1] : "/dev/video0";
	struct v4l2_capability capability;
	struct v4l2_fmtdesc description;
	struct v4l2_format format;
	struct v4l2_frmsizeenum size;
	struct v4l2_frmivalenum interval;
	const char bus_prefix[] = "platform:tx-isp-t41:ch";
	size_t channel_offset = sizeof(bus_prefix) - 1;
	int fd;

	fd = open(path, O_RDWR | O_NONBLOCK);
	if (fd < 0) {
		fprintf(stderr, "open %s: %s\n", path, strerror(errno));
		return 1;
	}

	memset(&capability, 0, sizeof(capability));
	if (checked_ioctl(fd, VIDIOC_QUERYCAP, &capability, "VIDIOC_QUERYCAP"))
		goto fail;
	if (!(capability.device_caps & V4L2_CAP_VIDEO_CAPTURE)) {
		fprintf(stderr, "node does not advertise VIDEO_CAPTURE\n");
		goto fail;
	}
	if (strncmp((char *)capability.bus_info, bus_prefix, channel_offset) ||
	    capability.bus_info[channel_offset] < '0' ||
	    capability.bus_info[channel_offset] > '2' ||
	    capability.bus_info[channel_offset + 1]) {
		fprintf(stderr, "missing stable scaler-channel identity\n");
		goto fail;
	}

	memset(&description, 0, sizeof(description));
	description.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (checked_ioctl(fd, VIDIOC_ENUM_FMT, &description, "VIDIOC_ENUM_FMT"))
		goto fail;
	if (description.pixelformat != V4L2_PIX_FMT_NV12) {
		fprintf(stderr, "expected NV12, got %#x\n", description.pixelformat);
		goto fail;
	}

	memset(&format, 0, sizeof(format));
	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (checked_ioctl(fd, VIDIOC_G_FMT, &format, "VIDIOC_G_FMT"))
		goto fail;
	if (!format.fmt.pix.width || !format.fmt.pix.height ||
	    !format.fmt.pix.bytesperline || !format.fmt.pix.sizeimage) {
		fprintf(stderr, "active format has an empty layout\n");
		goto fail;
	}

	memset(&size, 0, sizeof(size));
	size.index = 0;
	size.pixel_format = V4L2_PIX_FMT_NV12;
	if (checked_ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &size,
			  "VIDIOC_ENUM_FRAMESIZES"))
		goto fail;
	if (size.type != V4L2_FRMSIZE_TYPE_STEPWISE ||
	    size.stepwise.min_width != 32 || size.stepwise.step_width != 32 ||
	    size.stepwise.min_height != 16 || size.stepwise.step_height != 2 ||
	    size.stepwise.max_width < format.fmt.pix.width ||
	    size.stepwise.max_height < format.fmt.pix.height) {
		fprintf(stderr, "invalid per-channel scaler size range\n");
		goto fail;
	}

	memset(&interval, 0, sizeof(interval));
	interval.index = 0;
	interval.pixel_format = V4L2_PIX_FMT_NV12;
	interval.width = format.fmt.pix.width;
	interval.height = format.fmt.pix.height;
	if (checked_ioctl(fd, VIDIOC_ENUM_FRAMEINTERVALS, &interval,
			  "VIDIOC_ENUM_FRAMEINTERVALS"))
		goto fail;

	printf("driver=%s card=%s bus=%s\n",
	       capability.driver, capability.card, capability.bus_info);
	printf("caps=%#x device_caps=%#x format=NV12 %ux%u stride=%u size=%u fps=%u/%u\n",
	       capability.capabilities, capability.device_caps,
	       format.fmt.pix.width, format.fmt.pix.height,
	       format.fmt.pix.bytesperline, format.fmt.pix.sizeimage,
	       interval.discrete.denominator, interval.discrete.numerator);
	close(fd);
	return 0;

fail:
	close(fd);
	return 1;
}
