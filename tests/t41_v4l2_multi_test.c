// SPDX-License-Identifier: GPL-2.0
/* Target test: independent queues, discovery closes, main-first/sub-first
 * lifetime, last-owner teardown and a fresh sensor acquisition. No encoder. */
#define _GNU_SOURCE
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <poll.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

struct output {
    int fd;
    void *map[2];
    size_t size[2];
    uint64_t previous_us, max_delta;
    unsigned int frames;
};

static unsigned widths[] = {2560, 640, 320};
static unsigned heights[] = {1440, 360, 240};

static void checked(int fd, unsigned long cmd, void *arg)
{
    int ret;
    do { ret = ioctl(fd, cmd, arg); } while (ret < 0 && errno == EINTR);
    if (ret) perror("V4L2 ioctl");
    assert(ret == 0);
}

static void start(struct output *o, int channel)
{
    struct v4l2_format fmt = { .type = V4L2_BUF_TYPE_VIDEO_CAPTURE };
    struct v4l2_requestbuffers req = {
        .count = 2, .type = V4L2_BUF_TYPE_VIDEO_CAPTURE, .memory = V4L2_MEMORY_MMAP,
    };
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    char path[32];

    memset(o, 0, sizeof(*o));
    snprintf(path, sizeof(path), "/dev/video%d", channel);
    o->fd = open(path, O_RDWR | O_NONBLOCK);
    assert(o->fd >= 0);
    fmt.fmt.pix.width = widths[channel];
    fmt.fmt.pix.height = heights[channel];
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_NV12;
    printf("start channel=%d size=%ux%u: S_FMT\n", channel, widths[channel], heights[channel]);
    checked(o->fd, VIDIOC_S_FMT, &fmt);
    assert(fmt.fmt.pix.width == widths[channel] && fmt.fmt.pix.height == heights[channel]);
    puts("REQBUFS");
    checked(o->fd, VIDIOC_REQBUFS, &req);
    assert(req.count == 2);
    /* S_FMT must not rewrite the layout of an allocated queue. */
    errno = 0;
    assert(ioctl(o->fd, VIDIOC_S_FMT, &fmt) < 0 && errno == EBUSY);
    for (unsigned i = 0; i < 2; ++i) {
        struct v4l2_buffer b = { .index = i, .type = type, .memory = V4L2_MEMORY_MMAP };
        checked(o->fd, VIDIOC_QUERYBUF, &b);
        o->size[i] = b.length;
        o->map[i] = mmap(NULL, b.length, PROT_READ | PROT_WRITE, MAP_SHARED,
                          o->fd, b.m.offset);
        assert(o->map[i] != MAP_FAILED);
        checked(o->fd, VIDIOC_QBUF, &b);
    }
    puts("STREAMON");
    checked(o->fd, VIDIOC_STREAMON, &type);
    /* HAL discovery opens/closes a second fd on an already streaming node. */
    {
        struct v4l2_capability cap;
        int probe = open(path, O_RDWR | O_NONBLOCK);
        assert(probe >= 0);
        checked(probe, VIDIOC_QUERYCAP, &cap);
        close(probe);
    }
}

static void stop(struct output *o)
{
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    struct v4l2_requestbuffers req = { .type = type, .memory = V4L2_MEMORY_MMAP };
    checked(o->fd, VIDIOC_STREAMOFF, &type);
    for (unsigned i = 0; i < 2; ++i)
        assert(!munmap(o->map[i], o->size[i]));
    checked(o->fd, VIDIOC_REQBUFS, &req);
    close(o->fd);
    o->fd = -1;
}

static void capture(struct output o[3], unsigned rounds)
{
    for (unsigned round = 0; round < rounds; ++round) {
        for (unsigned i = 0; i < 3; ++i) {
            struct v4l2_buffer b = {
                .type = V4L2_BUF_TYPE_VIDEO_CAPTURE, .memory = V4L2_MEMORY_MMAP,
            };
            struct pollfd p = { .fd = o[i].fd, .events = POLLIN };
            uint64_t us;
            if (o[i].fd < 0) continue;
            assert(poll(&p, 1, 3000) == 1 && (p.revents & POLLIN));
            checked(o[i].fd, VIDIOC_DQBUF, &b);
            assert(b.index < 2 && b.bytesused && b.bytesused <= o[i].size[b.index]);
            assert(!(b.flags & V4L2_BUF_FLAG_ERROR));
            us = (uint64_t)b.timestamp.tv_sec * 1000000u + b.timestamp.tv_usec;
            if (o[i].previous_us) {
                assert(us > o[i].previous_us);
                if (us - o[i].previous_us > o[i].max_delta)
                    o[i].max_delta = us - o[i].previous_us;
            }
            o[i].previous_us = us;
            o[i].frames++;
            checked(o[i].fd, VIDIOC_QBUF, &b);
        }
    }
    for (unsigned i = 0; i < 3; ++i)
        if (o[i].fd >= 0)
            printf("channel=%u frames=%u max_delta_us=%llu\n", i, o[i].frames,
                   (unsigned long long)o[i].max_delta);
    fflush(stdout);
}

int main(int argc, char **argv)
{
    struct output o[3] = { { .fd = -1 }, { .fd = -1 }, { .fd = -1 } };
    setbuf(stdout, NULL);
    if (argc > 1 && !strcmp(argv[1], "third")) {
        unsigned delay_us = 0;
        if (argc > 2) {
            char *end;
            unsigned long value = strtoul(argv[2], &end, 10);
            assert(*argv[2] && !*end && value < 40000);
            delay_us = value;
        }
        /* Leave live Raptor encoders on 0/1 alone: isolate capture queue
         * allocation/scaler changes from codec destruction/recreation. */
        for (unsigned round = 0; round < 6; ++round) {
            widths[2] = (round & 1) ? 960 : 320;
            heights[2] = (round & 1) ? 544 : 240;
            start(&o[2], 2);
            capture(o, 50);
            /* Stop during the next sensor frame as well as at DQBUF's
             * frame boundary; late DMA must not outlive STREAMOFF. */
            if (delay_us) usleep(delay_us);
            stop(&o[2]);
        }
        puts("independent third output resize PASS");
        return 0;
    }
    start(&o[0], 0);
    start(&o[1], 1);
    if (argc > 1) {
        capture(o, 50);
        stop(&o[1]);
        widths[1] = 960;
        heights[1] = 544;
        puts("resizing raw sub to 960x544");
        start(&o[1], 1);
        capture(o, 100);
        stop(&o[1]);
        widths[1] = 640;
        heights[1] = 360;
        puts("resizing raw sub to 640x360");
        start(&o[1], 1);
        capture(o, 100);
        stop(&o[1]);
        stop(&o[0]);
        puts("raw output resize PASS");
        return 0;
    }
    start(&o[2], 2);
    capture(o, 100);
    stop(&o[0]); /* the first output cannot tear down two surviving users */
    capture(o, 50);
    start(&o[0], 0);
    capture(o, 50);
    stop(&o[1]);
    capture(o, 50);
    start(&o[1], 1);
    capture(o, 50);
    stop(&o[0]);
    stop(&o[2]);
    stop(&o[1]); /* last user owns the sensor teardown */
    start(&o[1], 1); /* sub must also acquire the sensor on a cold graph */
    capture(o, 50);
    start(&o[0], 0);
    capture(o, 50);
    stop(&o[1]);
    capture(o, 50);
    stop(&o[0]);
    puts("three-output V4L2 lifecycle PASS");
    return 0;
}
