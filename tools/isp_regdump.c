/*
 * Read a physical MMIO window through /dev/mem and write it to stdout.
 *
 * This deliberately has no TX-ISP dependencies so it can be cross-compiled
 * as a small device-side diagnostic:
 *
 *   mipsel-linux-gcc -Os -Wall -Wextra -o isp_regdump isp_regdump.c
 *   isp_regdump 0x13300000 0x90000 > isp-registers.bin
 */

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

static int write_all(int fd, const void *buffer, size_t length)
{
	const unsigned char *cursor = buffer;

	while (length) {
		ssize_t written = write(fd, cursor, length);

		if (written < 0) {
			if (errno == EINTR)
				continue;
			return -1;
		}
		if (!written)
			return -1;
		cursor += written;
		length -= written;
	}

	return 0;
}

int main(int argc, char **argv)
{
	long page_size;
	unsigned long long address;
	unsigned long long length;
	unsigned long long page_base;
	size_t page_offset;
	size_t map_length;
	void *mapping;
	char *end;
	int fd;
	int result = EXIT_FAILURE;

	if (argc != 3) {
		fprintf(stderr, "usage: %s ADDRESS LENGTH\n", argv[0]);
		return EXIT_FAILURE;
	}

	errno = 0;
	address = strtoull(argv[1], &end, 0);
	if (errno || !*argv[1] || *end) {
		fprintf(stderr, "invalid address: %s\n", argv[1]);
		return EXIT_FAILURE;
	}

	errno = 0;
	length = strtoull(argv[2], &end, 0);
	if (errno || !*argv[2] || *end || !length || length > SIZE_MAX) {
		fprintf(stderr, "invalid length: %s\n", argv[2]);
		return EXIT_FAILURE;
	}

	page_size = sysconf(_SC_PAGESIZE);
	if (page_size <= 0) {
		perror("sysconf");
		return EXIT_FAILURE;
	}

	page_base = address & ~((unsigned long long)page_size - 1);
	page_offset = (size_t)(address - page_base);
	if (length > SIZE_MAX - page_offset) {
		fprintf(stderr, "mapping length overflow\n");
		return EXIT_FAILURE;
	}
	map_length = page_offset + (size_t)length;

	fd = open("/dev/mem", O_RDONLY | O_SYNC);
	if (fd < 0) {
		perror("/dev/mem");
		return EXIT_FAILURE;
	}

	mapping = mmap(NULL, map_length, PROT_READ, MAP_SHARED, fd,
		       (off_t)page_base);
	if (mapping == MAP_FAILED) {
		perror("mmap");
		goto out_close;
	}

	if (write_all(STDOUT_FILENO,
		      (const unsigned char *)mapping + page_offset,
		      (size_t)length) < 0) {
		perror("write");
		goto out_unmap;
	}

	result = EXIT_SUCCESS;

out_unmap:
	munmap(mapping, map_length);
out_close:
	close(fd);
	return result;
}
