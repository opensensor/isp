/* write a single 32-bit word to a physical address via /dev/mem */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	unsigned long phys, val;
	long page;
	unsigned long page_mask, map_base, page_off;
	int fd;
	volatile unsigned int *p;
	void *map;

	if (argc != 3) {
		fprintf(stderr, "usage: %s <phys> <val32>\n", argv[0]);
		return 2;
	}
	phys = strtoul(argv[1], NULL, 0);
	val = strtoul(argv[2], NULL, 0);
	page = sysconf(_SC_PAGESIZE);
	page_mask = ~(page - 1);
	map_base = phys & page_mask;
	page_off = phys - map_base;
	fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd < 0) { perror("open"); return 1; }
	map = mmap(NULL, page, PROT_READ | PROT_WRITE, MAP_SHARED, fd, map_base);
	if (map == MAP_FAILED) { perror("mmap"); return 1; }
	p = (volatile unsigned int *)((char *)map + page_off);
	printf("before=0x%08x\n", *p);
	*p = (unsigned int)val;
	printf("after=0x%08x\n", *p);
	munmap(map, page);
	close(fd);
	return 0;
}
