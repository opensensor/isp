// SPDX-License-Identifier: MIT
/*
 * Stream a constant test LUT into the T40 LSC write port.
 *
 * Usage:
 *   lsc_lut_blast <bank0|1> <nodes> <word_hex>
 *
 * Mirrors OEM tisp_lsc_real_write_lut: ctrl 0x101, nodes*3 data words,
 * ((nodes-1)<<16)|0x102. Ports: bank0 ctrl 0x13350020 data 0x13350024,
 * bank1 ctrl 0x13350030 data 0x13350034.
 */
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	if (argc != 4) {
		fprintf(stderr, "usage: %s <bank> <nodes> <word_hex>\n", argv[0]);
		return 2;
	}
	unsigned long bank = strtoul(argv[1], NULL, 0);
	unsigned long nodes = strtoul(argv[2], NULL, 0);
	uint32_t word = (uint32_t)strtoul(argv[3], NULL, 16);
	unsigned long ctrl_off = bank ? 0x30 : 0x20;

	int fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd < 0) {
		perror("open /dev/mem");
		return 1;
	}
	volatile uint32_t *base = mmap(NULL, 0x1000, PROT_READ | PROT_WRITE,
				       MAP_SHARED, fd, 0x13350000);
	if (base == MAP_FAILED) {
		perror("mmap");
		return 1;
	}
	volatile uint32_t *ctrl = base + ctrl_off / 4;
	volatile uint32_t *data = ctrl + 1;

	*ctrl = 0x101;
	for (unsigned long i = 0; i < nodes * 3; i++)
		*data = word;
	*ctrl = (uint32_t)(((nodes - 1) << 16) | 0x102);
	printf("streamed %lu words of 0x%08x to bank %lu (ctrl rb 0x%08x)\n",
	       nodes * 3, word, bank, *ctrl);
	munmap((void *)base, 0x1000);
	close(fd);
	return 0;
}
