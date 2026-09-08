/* Target-only public control test; an active T41 capture owner is required.
 * No arguments: GET. MODE FREQ: SET/GET. --check: round trips and rejected
 * requests, restoring the original policy. Does not change saved config. */
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "tx_isp/tx_isp_tuning_abi.h"

_Static_assert(sizeof(void *) == 4, "T41 userspace pointer width");
_Static_assert(sizeof(struct tx_isp_tuning_t41_antiflicker) == 8, "Gen3 ABI");

static int control(int fd, unsigned int channel, unsigned int get,
                   struct tx_isp_tuning_t41_antiflicker *attr)
{
    struct tx_isp_tuning_t41_control req = {
        channel, get, TX_ISP_TUNING_CMD_T41_ANTIFLICKER, (uintptr_t)attr
    };
    return ioctl(fd, 0xc0105435U, &req);
}

static struct tx_isp_tuning_t41_antiflicker get_attr(int fd)
{
    struct tx_isp_tuning_t41_antiflicker attr;
    memset(&attr, 0xa5, sizeof(attr));
    assert(control(fd, 0, 1, &attr) == 0);
    assert(attr.mode <= 2);
    assert(attr.mode ? attr.frequency == 50 || attr.frequency == 60 :
                       attr.frequency == 0);
    assert(!attr.reserved[0] && !attr.reserved[1] && !attr.reserved[2]);
    return attr;
}

static void unchanged(int fd, struct tx_isp_tuning_t41_antiflicker expected)
{
    struct tx_isp_tuning_t41_antiflicker got = get_attr(fd);
    assert(got.mode == expected.mode && got.frequency == expected.frequency);
}

static void check(int fd)
{
    struct tx_isp_tuning_t41_antiflicker saved = get_attr(fd), attr;
    unsigned int mode, freq;

    for (mode = 0; mode < 3; ++mode) {
        for (freq = 50; freq <= 60; freq += 10) {
            attr = (struct tx_isp_tuning_t41_antiflicker){ mode, freq, {0} };
            assert(control(fd, 0, 0, &attr) == 0);
            if (!mode) attr.frequency = 0;
            unchanged(fd, attr);
        }
    }
    assert(control(fd, 0, 0, &saved) == 0);
    attr = (struct tx_isp_tuning_t41_antiflicker){ 3, 60, {0} };
    assert(control(fd, 0, 0, &attr) == -1 && errno == EINVAL);
    unchanged(fd, saved);
    attr.mode = 1;
    attr.frequency = 59;
    assert(control(fd, 0, 0, &attr) == -1 && errno == EINVAL);
    unchanged(fd, saved);
    assert(control(fd, 1, 0, &saved) == -1 && errno == EINVAL);
    assert(control(fd, 0, 2, &saved) == -1 && errno == EINVAL);
    assert(control(fd, 0, 0, NULL) == -1 && errno == EFAULT);
    assert(control(fd, 0, 1, NULL) == -1 && errno == EFAULT);
    unchanged(fd, saved);
    puts("T41 anti-flicker ioctl round trips and rejection tests passed");
}

int main(int argc, char **argv)
{
    struct tx_isp_tuning_t41_antiflicker attr;
    int fd;

    if (argc != 1 && !(argc == 2 && !strcmp(argv[1], "--check")) && argc != 3) {
        fprintf(stderr, "usage: %s [MODE FREQ | --check]\n", argv[0]);
        return 2;
    }
    fd = open("/dev/isp-m0", O_RDWR | O_CLOEXEC);
    assert(fd >= 0);
    if (argc == 3) {
        unsigned long values[2];
        unsigned int i;
        for (i = 0; i < 2; ++i) {
            char *end;
            errno = 0;
            values[i] = strtoul(argv[i + 1], &end, 0);
            assert(!errno && *argv[i + 1] && !*end && values[i] <= 255);
        }
        attr = (struct tx_isp_tuning_t41_antiflicker){values[0], values[1], {0}};
        assert(control(fd, 0, 0, &attr) == 0);
    } else if (argc == 2) {
        check(fd);
    }
    attr = get_attr(fd);
    printf("mode=%u frequency=%u\n", attr.mode, attr.frequency);
    assert(close(fd) == 0);
    return 0;
}
