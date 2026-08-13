// SPDX-License-Identifier: MIT
/*
 * Copy a bounded file payload to a physical MMIO range through /dev/mem.
 *
 * Usage:
 *   phys_memload <phys> <input> [len] [input_offset]
 *
 * This is intended for controlled register-oracle experiments.  The mapping
 * is limited to the requested payload and every copied 32-bit word is written
 * through a volatile MMIO pointer in file order.
 */
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static unsigned long parse_ulong(const char *s, const char *name)
{
	char *end = NULL;
	unsigned long v;

	errno = 0;
	v = strtoul(s, &end, 0);
	if (errno || end == s || *end != '\0') {
		fprintf(stderr, "invalid %s: %s\n", name, s);
		exit(2);
	}
	return v;
}

static int read_all(int fd, unsigned char *buf, unsigned long len)
{
	while (len) {
		ssize_t n = read(fd, buf, len);

		if (n < 0) {
			if (errno == EINTR)
				continue;
			perror("read");
			return -1;
		}
		if (!n) {
			fprintf(stderr, "short input file\n");
			return -1;
		}
		buf += n;
		len -= (unsigned long)n;
	}
	return 0;
}

int main(int argc, char **argv)
{
	unsigned long phys, len, input_offset = 0;
	unsigned long page_mask, map_base, page_offset, map_len;
	long page;
	struct stat st;
	unsigned char *buf;
	volatile uint32_t *dst;
	void *map;
	int input_fd, mem_fd;
	unsigned long i;

	if (argc < 3 || argc > 5) {
		fprintf(stderr,
			"usage: %s <phys> <input> [len] [input_offset]\n",
			argv[0]);
		return 2;
	}

	phys = parse_ulong(argv[1], "phys");
	input_fd = open(argv[2], O_RDONLY);
	if (input_fd < 0) {
		perror("open input");
		return 1;
	}
	if (fstat(input_fd, &st)) {
		perror("fstat input");
		close(input_fd);
		return 1;
	}
	if (argc >= 5)
		input_offset = parse_ulong(argv[4], "input_offset");
	if (input_offset > (unsigned long)st.st_size) {
		fprintf(stderr, "input_offset exceeds input size\n");
		close(input_fd);
		return 2;
	}
	len = argc >= 4 ? parse_ulong(argv[3], "len") :
		(unsigned long)st.st_size - input_offset;
	if (!len || len > (unsigned long)st.st_size - input_offset ||
	    (phys & 3) || (len & 3)) {
		fprintf(stderr, "phys and len must be 32-bit aligned and fit input\n");
		close(input_fd);
		return 2;
	}
	if (lseek(input_fd, (off_t)input_offset, SEEK_SET) < 0) {
		perror("lseek input");
		close(input_fd);
		return 1;
	}

	buf = malloc(len);
	if (!buf) {
		perror("malloc");
		close(input_fd);
		return 1;
	}
	if (read_all(input_fd, buf, len)) {
		free(buf);
		close(input_fd);
		return 1;
	}
	close(input_fd);

	page = sysconf(_SC_PAGESIZE);
	if (page <= 0) {
		perror("sysconf");
		free(buf);
		return 1;
	}
	page_mask = (unsigned long)page - 1;
	map_base = phys & ~page_mask;
	page_offset = phys - map_base;
	map_len = page_offset + len;

	mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (mem_fd < 0) {
		perror("open /dev/mem");
		free(buf);
		return 1;
	}
	map = mmap(NULL, map_len, PROT_READ | PROT_WRITE, MAP_SHARED,
		   mem_fd, (off_t)map_base);
	if (map == MAP_FAILED) {
		perror("mmap");
		close(mem_fd);
		free(buf);
		return 1;
	}

	dst = (volatile uint32_t *)((unsigned char *)map + page_offset);
	for (i = 0; i < len / sizeof(uint32_t); i++) {
		uint32_t value;

		value = (uint32_t)buf[i * 4] |
			((uint32_t)buf[i * 4 + 1] << 8) |
			((uint32_t)buf[i * 4 + 2] << 16) |
			((uint32_t)buf[i * 4 + 3] << 24);
		dst[i] = value;
	}

	printf("wrote %lu bytes from offset 0x%lx to physical 0x%lx\n",
	       len, input_offset, phys);
	munmap(map, map_len);
	close(mem_fd);
	free(buf);
	return 0;
}
