// SPDX-License-Identifier: GPL-2.0
/* Target-side proof that T41 capture MMAP buffers export as DMA-BUF fds. */

#define _GNU_SOURCE

#include <errno.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <poll.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#define TEST_BUFFER_COUNT 2U
#define TEST_FRAME_COUNT 10U

struct exported_buffer {
	void *address;
	size_t length;
	int fd;
};

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

static uint32_t sparse_checksum(const unsigned char *data, size_t length)
{
	uint32_t hash = 2166136261U;
	size_t offset;
	size_t step = length / 4096U;

	if (!step)
		step = 1;
	for (offset = 0; offset < length; offset += step) {
		hash ^= data[offset];
		hash *= 16777619U;
	}
	return hash;
}

int main(int argc, char **argv)
{
	const char *path = argc > 1 ? argv[1] : "/dev/video0";
	struct exported_buffer buffers[TEST_BUFFER_COUNT];
	struct v4l2_requestbuffers request;
	struct v4l2_exportbuffer export;
	struct v4l2_format format;
	struct v4l2_buffer buffer;
	struct pollfd pollfd;
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	unsigned int frame;
	unsigned int index;
	int video_fd = -1;
	int status = 1;

	memset(buffers, 0, sizeof(buffers));
	for (index = 0; index < TEST_BUFFER_COUNT; index++)
		buffers[index].fd = -1;
	video_fd = open(path, O_RDWR | O_NONBLOCK);
	if (video_fd < 0) {
		fprintf(stderr, "open %s: %s\n", path, strerror(errno));
		return 1;
	}

	memset(&format, 0, sizeof(format));
	format.type = type;
	format.fmt.pix.pixelformat = V4L2_PIX_FMT_NV12;
	format.fmt.pix.width = 2560;
	format.fmt.pix.height = 1440;
	if (checked_ioctl(video_fd, VIDIOC_S_FMT, &format, "VIDIOC_S_FMT"))
		goto out;

	memset(&request, 0, sizeof(request));
	request.count = TEST_BUFFER_COUNT;
	request.type = type;
	request.memory = V4L2_MEMORY_MMAP;
	if (checked_ioctl(video_fd, VIDIOC_REQBUFS, &request,
			  "VIDIOC_REQBUFS") || request.count != TEST_BUFFER_COUNT)
		goto out;

	for (index = 0; index < request.count; index++) {
		memset(&buffer, 0, sizeof(buffer));
		buffer.type = type;
		buffer.memory = V4L2_MEMORY_MMAP;
		buffer.index = index;
		if (checked_ioctl(video_fd, VIDIOC_QUERYBUF, &buffer,
				  "VIDIOC_QUERYBUF"))
			goto out;
		memset(&export, 0, sizeof(export));
		export.type = type;
		export.index = index;
		export.flags = O_CLOEXEC;
		if (checked_ioctl(video_fd, VIDIOC_EXPBUF, &export,
				  "VIDIOC_EXPBUF"))
			goto out;
		buffers[index].fd = export.fd;
		buffers[index].length = buffer.length;
		buffers[index].address = mmap(NULL, buffer.length, PROT_READ,
			MAP_SHARED, export.fd, 0);
		if (buffers[index].address == MAP_FAILED) {
			buffers[index].address = NULL;
			fprintf(stderr, "mmap DMA-BUF %u: %s\n", index,
				strerror(errno));
			goto out;
		}
		if (checked_ioctl(video_fd, VIDIOC_QBUF, &buffer,
				  "VIDIOC_QBUF"))
			goto out;
	}

	if (checked_ioctl(video_fd, VIDIOC_STREAMON, &type, "VIDIOC_STREAMON"))
		goto out;
	for (frame = 0; frame < TEST_FRAME_COUNT; frame++) {
		pollfd.fd = video_fd;
		pollfd.events = POLLIN;
		if (poll(&pollfd, 1, 3000) <= 0) {
			fprintf(stderr, "frame %u poll timeout/error\n", frame);
			goto streamoff;
		}
		memset(&buffer, 0, sizeof(buffer));
		buffer.type = type;
		buffer.memory = V4L2_MEMORY_MMAP;
		if (checked_ioctl(video_fd, VIDIOC_DQBUF, &buffer,
				  "VIDIOC_DQBUF"))
			goto streamoff;
		printf("frame=%u index=%u sequence=%u dmafd=%d bytes=%u checksum=%08x\n",
			frame, buffer.index, buffer.sequence,
			buffers[buffer.index].fd, buffer.bytesused,
			sparse_checksum(buffers[buffer.index].address,
				buffer.bytesused));
		if (frame + 1 < TEST_FRAME_COUNT &&
		    checked_ioctl(video_fd, VIDIOC_QBUF, &buffer, "VIDIOC_QBUF"))
			goto streamoff;
	}
	status = 0;

streamoff:
	checked_ioctl(video_fd, VIDIOC_STREAMOFF, &type, "VIDIOC_STREAMOFF");
out:
	for (index = 0; index < TEST_BUFFER_COUNT; index++) {
		if (buffers[index].address)
			munmap(buffers[index].address, buffers[index].length);
		if (buffers[index].fd >= 0)
			close(buffers[index].fd);
	}
	memset(&request, 0, sizeof(request));
	request.type = type;
	request.memory = V4L2_MEMORY_MMAP;
	checked_ioctl(video_fd, VIDIOC_REQBUFS, &request, "VIDIOC_REQBUFS(0)");
	close(video_fd);
	return status;
}
