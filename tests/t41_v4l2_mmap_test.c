// SPDX-License-Identifier: GPL-2.0
/* Target-side two-buffer MMAP capture smoke test for the T41 adapter. */

#include <errno.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <poll.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#define TEST_BUFFER_COUNT 2U
#define TEST_FRAME_COUNT 10U

struct mapped_buffer {
	void *address;
	size_t length;
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
	const char *output_path = argc > 2 ? argv[2] : NULL;
	struct mapped_buffer mapped[TEST_BUFFER_COUNT];
	struct v4l2_requestbuffers request;
	struct v4l2_format format;
	struct v4l2_buffer buffer;
	struct pollfd pollfd;
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	unsigned int frame;
	unsigned int index;
	int fd = -1;
	int status = 1;

	memset(mapped, 0, sizeof(mapped));
	fd = open(path, O_RDWR | O_NONBLOCK);
	if (fd < 0) {
		fprintf(stderr, "open %s: %s\n", path, strerror(errno));
		return 1;
	}

	memset(&format, 0, sizeof(format));
	format.type = type;
	format.fmt.pix.pixelformat = V4L2_PIX_FMT_NV12;
	format.fmt.pix.width = 2560;
	format.fmt.pix.height = 1440;
	if (checked_ioctl(fd, VIDIOC_S_FMT, &format, "VIDIOC_S_FMT"))
		goto out;

	memset(&request, 0, sizeof(request));
	request.count = TEST_BUFFER_COUNT;
	request.type = type;
	request.memory = V4L2_MEMORY_MMAP;
	if (checked_ioctl(fd, VIDIOC_REQBUFS, &request, "VIDIOC_REQBUFS"))
		goto out;
	if (request.count != TEST_BUFFER_COUNT) {
		fprintf(stderr, "expected %u buffers, received %u\n",
			TEST_BUFFER_COUNT, request.count);
		goto out;
	}

	for (index = 0; index < request.count; index++) {
		memset(&buffer, 0, sizeof(buffer));
		buffer.type = type;
		buffer.memory = V4L2_MEMORY_MMAP;
		buffer.index = index;
		if (checked_ioctl(fd, VIDIOC_QUERYBUF, &buffer, "VIDIOC_QUERYBUF"))
			goto out;
		mapped[index].length = buffer.length;
		mapped[index].address = mmap(NULL, buffer.length,
			PROT_READ | PROT_WRITE, MAP_SHARED, fd, buffer.m.offset);
		if (mapped[index].address == MAP_FAILED) {
			mapped[index].address = NULL;
			fprintf(stderr, "mmap buffer %u: %s\n", index,
				strerror(errno));
			goto out;
		}
		if (checked_ioctl(fd, VIDIOC_QBUF, &buffer, "VIDIOC_QBUF"))
			goto out;
	}

	if (checked_ioctl(fd, VIDIOC_STREAMON, &type, "VIDIOC_STREAMON"))
		goto out;
	for (frame = 0; frame < TEST_FRAME_COUNT; frame++) {
		pollfd.fd = fd;
		pollfd.events = POLLIN;
		if (poll(&pollfd, 1, 3000) <= 0) {
			fprintf(stderr, "frame %u poll timeout/error: %s\n", frame,
				errno ? strerror(errno) : "timeout");
			goto streamoff;
		}
		memset(&buffer, 0, sizeof(buffer));
		buffer.type = type;
		buffer.memory = V4L2_MEMORY_MMAP;
		if (checked_ioctl(fd, VIDIOC_DQBUF, &buffer, "VIDIOC_DQBUF"))
			goto streamoff;
		printf("frame=%u index=%u sequence=%u bytes=%u timestamp=%ld.%06ld checksum=%08x\n",
			frame, buffer.index, buffer.sequence, buffer.bytesused,
			(long)buffer.timestamp.tv_sec, (long)buffer.timestamp.tv_usec,
			sparse_checksum(mapped[buffer.index].address,
				buffer.bytesused));
		if (output_path && frame + 1 == TEST_FRAME_COUNT) {
			FILE *output = fopen(output_path, "wb");

			if (!output || fwrite(mapped[buffer.index].address, 1,
					      buffer.bytesused, output) !=
					      buffer.bytesused) {
				fprintf(stderr, "write %s: %s\n", output_path,
					strerror(errno));
				if (output)
					fclose(output);
				goto streamoff;
			}
			fclose(output);
		}
		if (frame + 1 < TEST_FRAME_COUNT &&
		    checked_ioctl(fd, VIDIOC_QBUF, &buffer, "VIDIOC_QBUF"))
			goto streamoff;
	}
	status = 0;

streamoff:
	checked_ioctl(fd, VIDIOC_STREAMOFF, &type, "VIDIOC_STREAMOFF");
out:
	for (index = 0; index < TEST_BUFFER_COUNT; index++)
		if (mapped[index].address)
			munmap(mapped[index].address, mapped[index].length);
	memset(&request, 0, sizeof(request));
	request.type = type;
	request.memory = V4L2_MEMORY_MMAP;
	checked_ioctl(fd, VIDIOC_REQBUFS, &request, "VIDIOC_REQBUFS(0)");
	close(fd);
	return status;
}
