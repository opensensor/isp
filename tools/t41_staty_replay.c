#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

#define STATY_BYTES 15000U

int main(int argc, char **argv)
{
    unsigned long physical;
    unsigned long page_base;
    unsigned long page_offset;
    size_t map_length;
    volatile uint32_t *destination;
    FILE *trace;
    void *mapping;
    char line[160];
    unsigned int offset;
    unsigned int value;
    unsigned int words = 0;
    int in_stats = 0;
    int memfd;

    if (argc != 3) {
        fprintf(stderr, "usage: %s TRACE PHYSICAL_ADDRESS\n", argv[0]);
        return 2;
    }
    errno = 0;
    physical = strtoul(argv[2], NULL, 0);
    if (errno || !physical) {
        perror("physical address");
        return 2;
    }
    trace = fopen(argv[1], "r");
    if (!trace) {
        perror(argv[1]);
        return 1;
    }
    memfd = open("/dev/mem", O_RDWR | O_SYNC);
    if (memfd < 0) {
        perror("/dev/mem");
        fclose(trace);
        return 1;
    }
    page_base = physical & ~(unsigned long)(getpagesize() - 1);
    page_offset = physical - page_base;
    map_length = page_offset + STATY_BYTES;
    mapping = mmap(NULL, map_length, PROT_READ | PROT_WRITE, MAP_SHARED,
                   memfd, (off_t)page_base);
    if (mapping == MAP_FAILED) {
        perror("mmap");
        close(memfd);
        fclose(trace);
        return 1;
    }
    destination = (volatile uint32_t *)((uint8_t *)mapping + page_offset);
    while (fgets(line, sizeof(line), trace)) {
        if (!strncmp(line, "[TMO_STAT_MEMORY]", 17)) {
            in_stats = 1;
            continue;
        }
        if (in_stats && line[0] == '[')
            break;
        if (!in_stats || sscanf(line, " Y+0x%x = 0x%x", &offset, &value) != 2)
            continue;
        /* Older trace captures accidentally include a second 15,000-byte
         * window.  The first window is the complete stock statYOut bank. */
        if (offset >= STATY_BYTES)
            break;
        if (offset & 3U) {
            fprintf(stderr, "invalid statYOut offset %#x\n", offset);
            munmap(mapping, map_length);
            close(memfd);
            fclose(trace);
            return 1;
        }
        destination[offset / 4U] = value;
        ++words;
    }
    msync(mapping, map_length, MS_SYNC);
    munmap(mapping, map_length);
    close(memfd);
    fclose(trace);
    if (words != STATY_BYTES / sizeof(uint32_t)) {
        fprintf(stderr, "expected %u words, parsed %u\n",
                STATY_BYTES / (unsigned int)sizeof(uint32_t), words);
        return 1;
    }
    printf("replayed %u statYOut words at physical %#lx\n", words, physical);
    return 0;
}
