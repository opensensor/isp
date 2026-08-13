#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define ISP_CORE_G_CTRL 0xc008561bUL
#define ISP_CORE_S_CTRL 0xc008561cUL

struct isp_ctrl {
	uint32_t id;
	uint32_t value;
};

static void usage(const char *prog)
{
	fprintf(stderr, "usage: %s [-d device] get ID [VALUE]\n"
			"       %s [-d device] set ID VALUE\n", prog, prog);
}

static int parse_u32(const char *text, uint32_t *value)
{
	char *end;
	unsigned long parsed;

	errno = 0;
	parsed = strtoul(text, &end, 0);
	if (errno || end == text || *end || parsed > UINT32_MAX)
		return -1;
	*value = (uint32_t)parsed;
	return 0;
}

int main(int argc, char **argv)
{
	const char *device = "/dev/isp-m0";
	const char *operation;
	struct isp_ctrl ctrl = { 0, 0 };
	unsigned long request;
	int argi = 1;
	int fd;

	if (argc > 2 && !strcmp(argv[argi], "-d")) {
		device = argv[argi + 1];
		argi += 2;
	}
	if (argc - argi < 2) {
		usage(argv[0]);
		return 2;
	}

	operation = argv[argi++];
	if (parse_u32(argv[argi++], &ctrl.id)) {
		fprintf(stderr, "invalid control ID\n");
		return 2;
	}

	if (!strcmp(operation, "set")) {
		if (argc - argi != 1 || parse_u32(argv[argi], &ctrl.value)) {
			usage(argv[0]);
			return 2;
		}
		request = ISP_CORE_S_CTRL;
	} else if (!strcmp(operation, "get")) {
		if (argc - argi > 1 ||
		    (argc - argi == 1 && parse_u32(argv[argi], &ctrl.value))) {
			usage(argv[0]);
			return 2;
		}
		request = ISP_CORE_G_CTRL;
	} else {
		usage(argv[0]);
		return 2;
	}

	fd = open(device, O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "open %s: %s\n", device, strerror(errno));
		return 1;
	}
	if (ioctl(fd, request, &ctrl) < 0) {
		fprintf(stderr, "ioctl 0x%08lx: %s\n", request, strerror(errno));
		close(fd);
		return 1;
	}
	close(fd);

	printf("id=0x%08x value=%u/0x%08x\n",
	       ctrl.id, ctrl.value, ctrl.value);
	return 0;
}
