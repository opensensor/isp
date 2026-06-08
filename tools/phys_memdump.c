// SPDX-License-Identifier: MIT
/*
 * Small device-side probe for ISP buffer experiments.
 *
 * Usage:
 *   phys_memdump <phys> <len> [outfile] [chunk_len]
 *
 * Maps /dev/mem at the requested physical range and copies bytes to stdout or
 * outfile. Intended for dumping one live NV12 qbuf so host-side tools can test
 * chroma ordering and plane assumptions without relying on the RTSP encoder.
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
	unsigned long v;

	errno = 0;
	v = strtoul(s, &end, 0);
	if (errno || end == s || *end != '\0') {
		fprintf(stderr, "invalid %s: %s\n", name, s);
		exit(2);
	}
	return v;
}

static int write_all(int fd, const unsigned char *buf, unsigned long len)
{
	while (len) {
		ssize_t n = write(fd, buf, len);

		if (n < 0) {
			if (errno == EINTR)
				continue;
			perror("write");
			return -1;
		}
		if (!n) {
			fprintf(stderr, "short write\n");
			return -1;
		}
		buf += n;
		len -= (unsigned long)n;
	}
	return 0;
}

int main(int argc, char **argv)
{
	unsigned long phys, len, chunk_len = 0x100000;
	unsigned long done = 0;
	long page;
	unsigned long page_mask;
	int mem_fd;
	int out_fd = STDOUT_FILENO;

	if (argc < 3 || argc > 5) {
		fprintf(stderr,
			"usage: %s <phys> <len> [outfile] [chunk_len]\n",
			argv[0]);
		return 2;
	}

	phys = parse_ulong(argv[1], "phys");
	len = parse_ulong(argv[2], "len");
	if (argc >= 5)
		chunk_len = parse_ulong(argv[4], "chunk_len");
	if (!len || !chunk_len) {
		fprintf(stderr, "invalid len/chunk_len\n");
		return 2;
	}

	page = sysconf(_SC_PAGESIZE);
	if (page <= 0) {
		perror("sysconf");
		return 1;
	}
	page_mask = (unsigned long)page - 1;

	mem_fd = open("/dev/mem", O_RDONLY | O_SYNC);
	if (mem_fd < 0) {
		perror("open /dev/mem");
		return 1;
	}

	if (argc >= 4 && argv[3][0] && argv[3][0] != '-') {
		out_fd = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (out_fd < 0) {
			perror("open outfile");
			close(mem_fd);
			return 1;
		}
	}

	while (done < len) {
		unsigned long cur_phys = phys + done;
		unsigned long want = len - done;
		unsigned long map_base;
		unsigned long page_off;
		unsigned long map_len;
		void *map;
		unsigned char *p;

		if (want > chunk_len)
			want = chunk_len;
		map_base = cur_phys & ~page_mask;
		page_off = cur_phys - map_base;
		map_len = page_off + want;

		map = mmap(NULL, map_len, PROT_READ, MAP_SHARED, mem_fd,
			   (off_t)map_base);
		if (map == MAP_FAILED) {
			perror("mmap");
			if (out_fd != STDOUT_FILENO)
				close(out_fd);
			close(mem_fd);
			return 1;
		}

		p = (unsigned char *)map + page_off;
		if (write_all(out_fd, p, want)) {
			munmap(map, map_len);
			if (out_fd != STDOUT_FILENO)
				close(out_fd);
			close(mem_fd);
			return 1;
		}

		munmap(map, map_len);
		done += want;
	}

	if (out_fd != STDOUT_FILENO)
		close(out_fd);
	close(mem_fd);
	return 0;
}
