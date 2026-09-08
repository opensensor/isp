/* Raw, pre-encoder temporal diagnostic on the third scaler output.
 * Reuse the checked capture lifetime. Main/sub encoders may keep running.
 * 640x360 and 16x9 zones are sampling geometry, not tuning coefficients. */
#define main lifecycle_test_main
#include "t41_v4l2_multi_test.c"
#undef main

#define GRID_X 16u
#define GRID_Y 9u
#define SAMPLE_STEP 4u

int main(int argc, char **argv)
{
    struct output o;
    unsigned int frames = 500;
    uint8_t previous[160u * 90u] = {0};
    uint64_t last_us = 0;
    unsigned int round;

    assert(argc <= 2);
    if (argc > 1) {
        char *end;
        unsigned long n = strtoul(argv[1], &end, 10);
        assert(*argv[1] && !*end && n && n <= 10000);
        frames = n;
    }
    widths[2] = 640;
    heights[2] = 360;
    start(&o, 2);
    printf("frame,sequence,buffer,timestamp_us,delta_us,yavg,ydif");
    for (round = 0; round < GRID_X * GRID_Y; ++round)
        printf(",zone_%u_%u", round % GRID_X, round / GRID_X);
    putchar('\n');
    for (round = 0; round < frames; ++round) {
        struct v4l2_buffer b = {
            .type = V4L2_BUF_TYPE_VIDEO_CAPTURE,
            .memory = V4L2_MEMORY_MMAP,
        };
        struct pollfd p = { .fd = o.fd, .events = POLLIN };
        uint32_t sums[GRID_X * GRID_Y] = {0};
        uint32_t total = 0, difference = 0;
        uint64_t us;
        unsigned int x, y, index = 0;
        const uint8_t *pixels;

        assert(poll(&p, 1, 3000) == 1 && (p.revents & POLLIN));
        checked(o.fd, VIDIOC_DQBUF, &b);
        assert(b.index < 2 && b.bytesused >= 640u * 360u);
        assert(!(b.flags & V4L2_BUF_FLAG_ERROR));
        pixels = o.map[b.index];
        us = (uint64_t)b.timestamp.tv_sec * 1000000u + b.timestamp.tv_usec;
        for (y = 0; y < 360u; y += SAMPLE_STEP) {
            for (x = 0; x < 640u; x += SAMPLE_STEP) {
                uint8_t value = pixels[y * 640u + x];
                total += value;
                sums[(y / 40u) * GRID_X + x / 40u] += value;
                difference += abs((int)value - previous[index]);
                previous[index++] = value;
            }
        }
        printf("%u,%u,%u,%llu,%llu,%.4f,%.4f", round, b.sequence,
               b.index, (unsigned long long)us,
               (unsigned long long)(last_us ? us - last_us : 0),
               total / 14400.0, round ? difference / 14400.0 : 0);
        for (index = 0; index < GRID_X * GRID_Y; ++index)
            printf(",%.2f", sums[index] / 100.0);
        putchar('\n');
        last_us = us;
        checked(o.fd, VIDIOC_QBUF, &b);
    }
    stop(&o);
    return 0;
}
