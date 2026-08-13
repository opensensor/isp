// SPDX-License-Identifier: MIT
/* Dump a bounded physical MMIO range through /dev/mem.
 *
 * Usage: phys_memdump <phys> <output> <len>
 */
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

static unsigned long parse_ulong(const char *s, const char *name)
{
	char *end = NULL;
	unsigned long value;

	errno = 0;
	value = strtoul(s, &end, 0);
	if (errno || end == s || *end != '\0') {
		fprintf(stderr, "invalid %s: %s\n", name, s);
		exit(2);
	}
	return value;
}

static int write_all(int fd, const unsigned char *buf, unsigned long len)
{
	while (len) {
		ssize_t written = write(fd, buf, len);

		if (written < 0) {
			if (errno == EINTR)
				continue;
			perror("write");
			return -1;
		}
		buf += written;
		len -= (unsigned long)written;
	}
	return 0;
}

int main(int argc, char **argv)
{
	unsigned long phys, len, page_mask, map_base, page_offset, map_len;
	volatile uint32_t *src;
	unsigned char *buf;
	void *map;
	long page;
	int mem_fd, out_fd;
	unsigned long i;

	if (argc != 4) {
		fprintf(stderr, "usage: %s <phys> <output> <len>\n", argv[0]);
		return 2;
	}
	phys = parse_ulong(argv[1], "phys");
	len = parse_ulong(argv[3], "len");
	if (!len || (phys & 3) || (len & 3)) {
		fprintf(stderr, "phys and len must be 32-bit aligned\n");
		return 2;
	}

	page = sysconf(_SC_PAGESIZE);
	if (page <= 0) {
		perror("sysconf");
		return 1;
	}
	page_mask = (unsigned long)page - 1;
	map_base = phys & ~page_mask;
	page_offset = phys - map_base;
	map_len = page_offset + len;

	mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (mem_fd < 0) {
		perror("open /dev/mem");
		return 1;
	}
	map = mmap(NULL, map_len, PROT_READ | PROT_WRITE, MAP_SHARED,
		   mem_fd, (off_t)map_base);
	if (map == MAP_FAILED) {
		perror("mmap");
		close(mem_fd);
		return 1;
	}

	buf = malloc(len);
	if (!buf) {
		perror("malloc");
		munmap(map, map_len);
		close(mem_fd);
		return 1;
	}
	src = (volatile uint32_t *)((unsigned char *)map + page_offset);
	for (i = 0; i < len / sizeof(uint32_t); i++) {
		uint32_t value = src[i];

		buf[i * 4] = value;
		buf[i * 4 + 1] = value >> 8;
		buf[i * 4 + 2] = value >> 16;
		buf[i * 4 + 3] = value >> 24;
	}

	out_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0600);
	if (out_fd < 0) {
		perror("open output");
		free(buf);
		munmap(map, map_len);
		close(mem_fd);
		return 1;
	}
	if (write_all(out_fd, buf, len)) {
		close(out_fd);
		free(buf);
		munmap(map, map_len);
		close(mem_fd);
		return 1;
	}

	close(out_fd);
	free(buf);
	munmap(map, map_len);
	close(mem_fd);
	printf("read %lu bytes from physical 0x%lx\n", len, phys);
	return 0;
}
