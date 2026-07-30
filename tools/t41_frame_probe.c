#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define T41_SET_FRAME_FORMAT 0xc0745451UL
#define T41_GET_FRAME_FORMAT 0xc0745452UL
#define T41_V4L2_BUF_TYPE_VIDEO_CAPTURE 1U
#define T41_V4L2_FIELD_NONE 4U
#define T41_V4L2_COLORSPACE_REC709 8U
#define T41_V4L2_PIX_FMT_NV12 0x3231564eU

int main(int argc, char **argv)
{
	const char *path = argc > 1 ? argv[1] : "/dev/framechan0";
	uint32_t format[0x74 / sizeof(uint32_t)];
	int saved_errno;
	int result;
	int fd;
	int do_get;
	int do_set;

	fd = open(path, O_RDWR | O_CLOEXEC);
	saved_errno = errno;
	printf("open path=%s ret=%d errno=%d (%s)\n", path, fd,
	       saved_errno, strerror(saved_errno));
	if (fd < 0)
		return 1;

	do_get = argc > 2 && (!strcmp(argv[2], "getfmt") ||
			      !strcmp(argv[2], "getset"));
	do_set = argc > 2 && (!strcmp(argv[2], "setfmt") ||
			      !strcmp(argv[2], "getset"));

	memset(format, 0, sizeof(format));
	format[0] = T41_V4L2_BUF_TYPE_VIDEO_CAPTURE;
	format[1] = 1920;
	format[2] = 1080;
	format[3] = T41_V4L2_PIX_FMT_NV12;
	format[4] = T41_V4L2_FIELD_NONE;
	format[7] = T41_V4L2_COLORSPACE_REC709;

	if (do_get) {
		errno = 0;
		result = ioctl(fd, T41_GET_FRAME_FORMAT, format);
		saved_errno = errno;
		printf("ioctl cmd=0x%08lx ret=%d errno=%d (%s) "
		       "size=%ux%u pixfmt=0x%08x field=%u "
		       "sizeimage=%u colorspace=%u\n",
		       T41_GET_FRAME_FORMAT, result, saved_errno,
		       strerror(saved_errno), format[1], format[2], format[3],
		       format[4], format[6], format[7]);
	}

	if (do_set) {
		memset(format, 0, sizeof(format));
		format[0] = T41_V4L2_BUF_TYPE_VIDEO_CAPTURE;
		format[1] = 1920;
		format[2] = 1080;
		format[3] = T41_V4L2_PIX_FMT_NV12;
		format[4] = T41_V4L2_FIELD_NONE;
		format[7] = T41_V4L2_COLORSPACE_REC709;
		errno = 0;
		result = ioctl(fd, T41_SET_FRAME_FORMAT, format);
		saved_errno = errno;
		printf("ioctl cmd=0x%08lx ret=%d errno=%d (%s) "
		       "size=%ux%u pixfmt=0x%08x field=%u "
		       "sizeimage=%u colorspace=%u\n",
		       T41_SET_FRAME_FORMAT, result, saved_errno,
		       strerror(saved_errno), format[1], format[2], format[3],
		       format[4], format[6], format[7]);
	}

	errno = 0;
	result = close(fd);
	saved_errno = errno;
	printf("close ret=%d errno=%d (%s)\n", result, saved_errno,
	       strerror(saved_errno));
	return result < 0 ? 1 : 0;
}
