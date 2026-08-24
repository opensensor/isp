#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define TISP_VIDIOC_TUNING 0xc00c56c6UL

struct t31_tuning_control {
	uint32_t direction;
	uint32_t id;
	uint32_t value;
};

int main(int argc, char **argv)
{
	struct t31_tuning_control control = { 0 };
	char *end;
	int fd;
	int ret;

	if (argc != 3) {
		fprintf(stderr, "usage: %s CONTROL_ID VALUE\n", argv[0]);
		return 2;
	}

	errno = 0;
	control.id = (uint32_t)strtoul(argv[1], &end, 0);
	if (errno || *end != '\0') {
		fprintf(stderr, "invalid control id: %s\n", argv[1]);
		return 2;
	}

	errno = 0;
	control.value = (uint32_t)strtoul(argv[2], &end, 0);
	if (errno || *end != '\0') {
		fprintf(stderr, "invalid value: %s\n", argv[2]);
		return 2;
	}

	fd = open("/dev/isp-m0", O_RDWR);
	if (fd < 0) {
		perror("open /dev/isp-m0");
		return 1;
	}

	ret = ioctl(fd, TISP_VIDIOC_TUNING, &control);
	if (ret < 0)
		perror("TISP_VIDIOC_TUNING");

	close(fd);
	return ret < 0 ? 1 : 0;
}
