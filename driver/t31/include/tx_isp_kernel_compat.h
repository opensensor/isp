#ifndef __TX_ISP_KERNEL_COMPAT_H__
#define __TX_ISP_KERNEL_COMPAT_H__

#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 0, 0)
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/timekeeping.h>
#include <linux/types.h>

/* Types removed from the in-kernel API but retained in the T31 userspace ABI. */
struct timespec {
	long tv_sec;
	long tv_nsec;
};

struct timeval {
	long tv_sec;
	long tv_usec;
};

typedef phys_addr_t phys_t;

/* set_fs() disappeared once kernel_read()/kernel_write() became universal. */
#ifndef KERNEL_DS
#define KERNEL_DS 0UL
#endif
#define get_fs() 0UL
#define set_fs(segment) do { (void)(segment); } while (0)
#define vfs_read(file, buf, count, pos) kernel_read((file), (buf), (count), (pos))
#define vfs_write(file, buf, count, pos) kernel_write((file), (buf), (count), (pos))
#define ioremap_nocache(offset, size) ioremap((offset), (size))
#define INIT_COMPLETION(completion) reinit_completion(&(completion))

#ifndef PDE_DATA
#define PDE_DATA(inode) pde_data(inode)
#endif

static inline void do_gettimeofday(struct timeval *tv)
{
	struct timespec64 now;

	ktime_get_real_ts64(&now);
	tv->tv_sec = (long)now.tv_sec;
	tv->tv_usec = now.tv_nsec / NSEC_PER_USEC;
}

static inline void getrawmonotonic(struct timespec *ts)
{
	struct timespec64 now;

	ktime_get_raw_ts64(&now);
	ts->tv_sec = (long)now.tv_sec;
	ts->tv_nsec = now.tv_nsec;
}
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define TX_ISP_PROC_OPS struct proc_ops
#define TX_ISP_PROC_OWNER
#define TX_ISP_PROC_OPEN .proc_open
#define TX_ISP_PROC_READ .proc_read
#define TX_ISP_PROC_WRITE .proc_write
#define TX_ISP_PROC_LSEEK .proc_lseek
#define TX_ISP_PROC_RELEASE .proc_release
#else
#define TX_ISP_PROC_OPS struct file_operations
#define TX_ISP_PROC_OWNER .owner = THIS_MODULE,
#define TX_ISP_PROC_OPEN .open
#define TX_ISP_PROC_READ .read
#define TX_ISP_PROC_WRITE .write
#define TX_ISP_PROC_LSEEK .llseek
#define TX_ISP_PROC_RELEASE .release
#endif

#endif
