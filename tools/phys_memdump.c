// SPDX-License-Identifier: MIT
/*
 * Small device-side probe for ISP buffer experiments.
 *
 * Usage:
 *   phys_memdump <phys> <len> [outfile] [chunk_len]
 *   phys_memdump --mean8 <phys> <len> [chunk_len]
 *   phys_memdump --uvmean <phys> <len> [chunk_len]
 *
 * Maps /dev/mem at the requested physical range and copies bytes to stdout or
 * outfile. The compact mean modes avoid expanding a live frame into hundreds
 * of kilobytes of od/awk text when the userspace T40 3A loop only needs a
 * luma or interleaved NV12 chroma average.
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

enum output_mode {
	OUTPUT_DUMP,
	OUTPUT_MEAN8,
	OUTPUT_UVMEAN,
};

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
	uint64_t sum0 = 0, sum1 = 0;
	uint64_t count0 = 0, count1 = 0;
	long page;
	unsigned long page_mask;
	int argi = 1;
	int mem_fd;
	int out_fd = STDOUT_FILENO;
	enum output_mode mode = OUTPUT_DUMP;

	if (argc >= 2 && !strcmp(argv[1], "--mean8")) {
		mode = OUTPUT_MEAN8;
		argi++;
	} else if (argc >= 2 && !strcmp(argv[1], "--uvmean")) {
		mode = OUTPUT_UVMEAN;
		argi++;
	}

	if ((mode == OUTPUT_DUMP && (argc - argi < 2 || argc - argi > 4)) ||
	    (mode != OUTPUT_DUMP && (argc - argi < 2 || argc - argi > 3))) {
		fprintf(stderr,
			"usage: %s <phys> <len> [outfile] [chunk_len]\n"
			"       %s --mean8 <phys> <len> [chunk_len]\n"
			"       %s --uvmean <phys> <len> [chunk_len]\n",
			argv[0], argv[0],
			argv[0]);
		return 2;
	}

	phys = parse_ulong(argv[argi], "phys");
	len = parse_ulong(argv[argi + 1], "len");
	if (mode == OUTPUT_DUMP && argc - argi >= 4)
		chunk_len = parse_ulong(argv[argi + 3], "chunk_len");
	else if (mode != OUTPUT_DUMP && argc - argi >= 3)
		chunk_len = parse_ulong(argv[argi + 2], "chunk_len");
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

	if (mode == OUTPUT_DUMP && argc - argi >= 3 &&
	    argv[argi + 2][0] && argv[argi + 2][0] != '-') {
		out_fd = open(argv[argi + 2], O_WRONLY | O_CREAT | O_TRUNC,
			      0644);
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
		if (mode == OUTPUT_DUMP && write_all(out_fd, p, want)) {
			munmap(map, map_len);
			if (out_fd != STDOUT_FILENO)
				close(out_fd);
			close(mem_fd);
			return 1;
		} else if (mode != OUTPUT_DUMP) {
			unsigned long i;

			for (i = 0; i < want; i++) {
				if (mode == OUTPUT_MEAN8 || !((done + i) & 1)) {
					sum0 += p[i];
					count0++;
				} else {
					sum1 += p[i];
					count1++;
				}
			}
		}

		munmap(map, map_len);
		done += want;
	}

	if (out_fd != STDOUT_FILENO)
		close(out_fd);
	close(mem_fd);
	if (mode == OUTPUT_MEAN8) {
		if (!count0)
			return 1;
		printf("%llu\n", (unsigned long long)(sum0 / count0));
	} else if (mode == OUTPUT_UVMEAN) {
		if (!count0 || !count1)
			return 1;
		printf("%llu %llu\n",
		       (unsigned long long)(sum0 / count0),
		       (unsigned long long)(sum1 / count1));
	}
	return 0;
}
