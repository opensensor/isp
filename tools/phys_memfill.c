// SPDX-License-Identifier: MIT
/*
 * Small device-side probe for ISP buffer experiments.
 *
 * Usage:
 *   phys_memfill <phys> <len> <byte> [repeat] [delay_us]
 *
 * Maps /dev/mem at the requested physical range and repeatedly fills it with
 * one byte value. Intended for short live tests such as forcing NV12 UV planes
 * to 0x80 while a frame is captured.
 */
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
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

int main(int argc, char **argv)
{
	unsigned long phys, len, byte, repeat = 1, delay_us = 0;
	long page;
	unsigned long page_mask, map_base, page_off, map_len;
	int fd;
	void *map;
	unsigned char *p;
	unsigned long i;

	if (argc < 4 || argc > 6) {
		fprintf(stderr,
			"usage: %s <phys> <len> <byte> [repeat] [delay_us]\n",
			argv[0]);
		return 2;
	}

	phys = parse_ulong(argv[1], "phys");
	len = parse_ulong(argv[2], "len");
	byte = parse_ulong(argv[3], "byte");
	if (argc >= 5)
		repeat = parse_ulong(argv[4], "repeat");
	if (argc >= 6)
		delay_us = parse_ulong(argv[5], "delay_us");
	if (!len || byte > 0xff || !repeat) {
		fprintf(stderr, "invalid len/byte/repeat\n");
		return 2;
	}

	page = sysconf(_SC_PAGESIZE);
	if (page <= 0) {
		perror("sysconf");
		return 1;
	}
	page_mask = (unsigned long)page - 1;
	map_base = phys & ~page_mask;
	page_off = phys - map_base;
	map_len = page_off + len;

	fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd < 0) {
		perror("open /dev/mem");
		return 1;
	}

	map = mmap(NULL, map_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
		   (off_t)map_base);
	if (map == MAP_FAILED) {
		perror("mmap");
		close(fd);
		return 1;
	}

	p = (unsigned char *)map + page_off;
	for (i = 0; i < repeat; i++) {
		memset(p, (int)byte, len);
		if (delay_us)
			usleep(delay_us);
	}
	msync(map, map_len, MS_SYNC);

	munmap(map, map_len);
	close(fd);
	return 0;
}
