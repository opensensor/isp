#define _GNU_SOURCE

#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <time.h>
#include <unistd.h>

typedef int (*ioctl_fn)(int, unsigned long, ...);

static ioctl_fn real_ioctl;
static int trace_fd = -2;
static unsigned int sequence;

static int interesting(unsigned long request)
{
	return request == 0xc0145453UL || request == 0xc0445454UL ||
	       request == 0xc0445455UL || request == 0xc0445456UL ||
	       request == 0xc0045457UL || request == 0xc0045458UL;
}

static void trace_words(const char *phase, int fd, unsigned long request,
			void *argument, int result, int saved_errno)
{
	uint32_t words[17] = { 0 };
	char path[64] = { 0 };
	char link[64];
	char line[768];
	struct timespec now;
	unsigned int count;
	int length;

	if (!interesting(request))
		return;
	if (trace_fd == -2)
		trace_fd = open("/tmp/t41-ioctl-trace.txt",
				O_WRONLY | O_CREAT | O_APPEND | O_CLOEXEC, 0644);
	if (trace_fd < 0)
		return;

	length = snprintf(link, sizeof(link), "/proc/self/fd/%d", fd);
	if (length > 0)
		(void)readlink(link, path, sizeof(path) - 1);
	if (argument) {
		count = request == 0xc0145453UL ? 5U :
			(request == 0xc0045457UL || request == 0xc0045458UL ? 1U : 17U);
		memcpy(words, argument, count * sizeof(words[0]));
	} else {
		count = 0;
	}
	clock_gettime(CLOCK_MONOTONIC, &now);
	length = snprintf(line, sizeof(line),
		"%u %ld.%09ld %s fd=%d path=%s req=%08lx ret=%d errno=%d "
		"n=%u words=%08x/%08x/%08x/%08x/%08x/%08x/%08x/%08x/"
		"%08x/%08x/%08x/%08x/%08x/%08x/%08x/%08x/%08x\n",
		__sync_add_and_fetch(&sequence, 1), (long)now.tv_sec, now.tv_nsec,
		phase, fd, path, request, result, saved_errno, count,
		words[0], words[1], words[2], words[3], words[4], words[5],
		words[6], words[7], words[8], words[9], words[10], words[11],
		words[12], words[13], words[14], words[15], words[16]);
	if (length > 0)
		(void)write(trace_fd, line, (size_t)length);
}

int ioctl(int fd, unsigned long request, ...)
{
	va_list arguments;
	void *argument;
	int result;
	int saved_errno;

	va_start(arguments, request);
	argument = va_arg(arguments, void *);
	va_end(arguments);
	if (!real_ioctl)
		real_ioctl = (ioctl_fn)dlsym(RTLD_NEXT, "ioctl");
	if (!real_ioctl) {
		errno = ENOSYS;
		return -1;
	}
	trace_words("before", fd, request, argument, 0, 0);
	result = real_ioctl(fd, request, argument);
	saved_errno = errno;
	trace_words("after", fd, request, argument, result, saved_errno);
	errno = saved_errno;
	return result;
}
