// SPDX-License-Identifier: MIT
/*
 * Replay a T31 sensor IQ lens-shading table through the live ISP MMIO window.
 *
 * Usage: t31_lsc_replay IQ_BIN day|night CT GAIN_HI GAIN_LO [DELAY_US]
 *
 * This is a diagnostic companion to tisp_lsc_write_lut_datas().  It consumes
 * the shipping two-bank T31 IQ file directly and deliberately uses the same
 * fixed-point interpolation, strength scaling, write order, and commit write
 * as the kernel driver.
 */
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define T31_IQ_HEADER_SIZE 0x18U
#define T31_IQ_BLOCK_SIZE  0x137f0U
#define T31_ISP_PHYS       0x13300000UL
#define T31_ISP_MAP_SIZE   0x30000UL
#define T31_LSC_PORT       0x28000U
#define T31_LSC_MAX_WORDS  2047U

static uint32_t get_u32(const unsigned char *p)
{
	return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
	       ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

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

static uint32_t lerp_u32(uint32_t base, uint32_t next, uint32_t fraction)
{
	uint32_t difference;
	uint32_t product;
	uint32_t step;

	if (base == next || fraction == 0)
		return base;
	difference = base >= next ? base - next : next - base;
	product = difference * fraction;
	step = (product >> 16) + ((product >> 15) & 1U);
	return base >= next ? base - step : base + step;
}

static uint32_t simple_intp(uint32_t hi, uint32_t lo,
			    const uint32_t values[9])
{
	if (hi >= 8)
		return values[8];
	return lerp_u32(values[hi], values[hi + 1], lo);
}

static uint32_t interpolate_lut_word(uint32_t first, uint32_t second,
				     uint32_t weight)
{
	int32_t first_hi = (int32_t)(first >> 12);
	int32_t first_lo = (int32_t)(first & 0xfff);
	int32_t second_hi = (int32_t)(second >> 12);
	int32_t second_lo = (int32_t)(second & 0xfff);
	int32_t out_hi = first_hi + (((second_hi - first_hi) * (int32_t)weight) >> 12);
	int32_t out_lo = first_lo + (((second_lo - first_lo) * (int32_t)weight) >> 12);

	if (out_hi < 0)
		out_hi = 0;
	else if (out_hi > 0xfff)
		out_hi = 0xfff;
	if (out_lo < 0)
		out_lo = 0;
	else if (out_lo > 0xfff)
		out_lo = 0xfff;
	return ((uint32_t)out_hi << 12) | (uint32_t)out_lo;
}

static uint32_t scale_lut_word(uint32_t value, uint32_t strength,
			       uint32_t base)
{
	int32_t hi = (int32_t)(value >> 12);
	int32_t lo = (int32_t)(value & 0xfff);

	hi = (int32_t)base + (((hi - (int32_t)base) * (int32_t)strength) >> 12);
	lo = (int32_t)base + (((lo - (int32_t)base) * (int32_t)strength) >> 12);
	if (hi < 0)
		hi = 0;
	else if (hi > 0xfff)
		hi = 0xfff;
	if (lo < 0)
		lo = 0;
	else if (lo > 0xfff)
		lo = 0xfff;
	return ((uint32_t)hi << 12) | (uint32_t)lo;
}

int main(int argc, char **argv)
{
	unsigned char *file_data = NULL;
	const unsigned char *block;
	const unsigned char *lut0;
	const unsigned char *lut1 = NULL;
	uint32_t thresholds[4];
	uint32_t strengths[9];
	uint32_t lut_words;
	uint32_t mesh_scale;
	uint32_t strength;
	uint32_t base_strength;
	uint32_t weight = 0;
	uint32_t ct;
	uint32_t gain_hi;
	uint32_t gain_lo;
	unsigned long delay_us = 0;
	unsigned long bank_offset;
	struct stat st;
	volatile uint32_t *regs;
	void *map;
	FILE *fp;
	int mem_fd;
	uint32_t i;

	if (argc < 6 || argc > 7) {
		fprintf(stderr, "usage: %s IQ_BIN day|night CT GAIN_HI GAIN_LO [DELAY_US]\n",
			argv[0]);
		return 2;
	}
	if (!strcmp(argv[2], "day"))
		bank_offset = T31_IQ_HEADER_SIZE;
	else if (!strcmp(argv[2], "night"))
		bank_offset = T31_IQ_HEADER_SIZE + T31_IQ_BLOCK_SIZE;
	else {
		fprintf(stderr, "bank must be day or night\n");
		return 2;
	}
	ct = (uint32_t)parse_ulong(argv[3], "CT");
	gain_hi = (uint32_t)parse_ulong(argv[4], "GAIN_HI");
	gain_lo = (uint32_t)parse_ulong(argv[5], "GAIN_LO");
	if (argc == 7)
		delay_us = parse_ulong(argv[6], "DELAY_US");

	fp = fopen(argv[1], "rb");
	if (!fp) {
		perror("open IQ bin");
		return 1;
	}
	if (fstat(fileno(fp), &st) ||
	    (unsigned long)st.st_size < bank_offset + T31_IQ_BLOCK_SIZE) {
		fprintf(stderr, "IQ bin is too short\n");
		fclose(fp);
		return 1;
	}
	file_data = malloc((size_t)st.st_size);
	if (!file_data || fread(file_data, 1, (size_t)st.st_size, fp) != (size_t)st.st_size) {
		perror("read IQ bin");
		free(file_data);
		fclose(fp);
		return 1;
	}
	fclose(fp);
	block = file_data + bank_offset;
	lut_words = get_u32(block + 0x30e0);
	mesh_scale = get_u32(block + 0x30e4);
	if (!lut_words || lut_words > T31_LSC_MAX_WORDS || lut_words % 3) {
		fprintf(stderr, "invalid LSC word count: %u\n", lut_words);
		free(file_data);
		return 1;
	}
	for (i = 0; i < 4; i++)
		thresholds[i] = get_u32(block + 0x30f4 + i * 4);
	for (i = 0; i < 9; i++)
		strengths[i] = get_u32(block + 0x90f8 + i * 4);
	strength = simple_intp(gain_hi, gain_lo, strengths);
	base_strength = mesh_scale == 0 ? 0x800 :
			mesh_scale == 1 ? 0x400 :
			mesh_scale == 2 ? 0x200 : 0x100;

	if (ct <= thresholds[0]) {
		lut0 = block + 0x3104;
	} else if (ct <= thresholds[1]) {
		lut0 = block + 0x3104;
		lut1 = block + 0x5100;
		weight = ((ct - thresholds[0]) << 12) /
			 (thresholds[1] - thresholds[0]);
	} else if (ct <= thresholds[2]) {
		lut0 = block + 0x5100;
	} else if (ct <= thresholds[3]) {
		lut0 = block + 0x5100;
		lut1 = block + 0x70fc;
		weight = ((ct - thresholds[2]) << 12) /
			 (thresholds[3] - thresholds[2]);
	} else {
		lut0 = block + 0x70fc;
	}

	mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (mem_fd < 0) {
		perror("open /dev/mem");
		free(file_data);
		return 1;
	}
	map = mmap(NULL, T31_ISP_MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
		   mem_fd, T31_ISP_PHYS);
	if (map == MAP_FAILED) {
		perror("mmap ISP");
		close(mem_fd);
		free(file_data);
		return 1;
	}
	regs = map;
	for (i = 0; i < lut_words / 3; i++) {
		uint32_t channel;

		for (channel = 0; channel < 3; channel++) {
			uint32_t index = i * 3 + channel;
			uint32_t value = get_u32(lut0 + index * 4);

			if (lut1)
				value = interpolate_lut_word(value,
					get_u32(lut1 + index * 4), weight);
			value = scale_lut_word(value, strength, base_strength);
			regs[(T31_LSC_PORT + i * 16 + channel * 4) / 4] = value;
			if (delay_us)
				usleep(delay_us);
		}
	}
	regs[(T31_LSC_PORT + 0x0c) / 4] = 0;
	printf("replayed %u LSC entries: bank=%s ct=%u gain=%u:%u strength=%u delay=%luus first=0x%08x\n",
	       lut_words / 3, argv[2], ct, gain_hi, gain_lo, strength, delay_us,
	       regs[T31_LSC_PORT / 4]);

	munmap(map, T31_ISP_MAP_SIZE);
	close(mem_fd);
	free(file_data);
	return 0;
}
