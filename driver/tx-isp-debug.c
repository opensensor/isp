#include <linux/debugfs.h>
#include <linux/module.h>
#include <tx-isp-debug.h>
#include <linux/vmalloc.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/version.h>


/* -------------------debugfs interface------------------- */
static int print_level = ISP_WARN_LEVEL;
module_param(print_level, int, S_IRUGO);
MODULE_PARM_DESC(print_level, "isp print level");


int isp_ch0_pre_dequeue_time;
module_param(isp_ch0_pre_dequeue_time, int, S_IRUGO);
MODULE_PARM_DESC(isp_ch0_pre_dequeue_time, "isp pre dequeue time, unit ms");

int isp_ch0_pre_dequeue_interrupt_process;
module_param(isp_ch0_pre_dequeue_interrupt_process, int, S_IRUGO);
MODULE_PARM_DESC(isp_ch0_pre_dequeue_interrupt_process, "isp pre dequeue interrupt process");

int isp_ch0_pre_dequeue_valid_lines;
module_param(isp_ch0_pre_dequeue_valid_lines, int, S_IRUGO);
MODULE_PARM_DESC(isp_ch0_pre_dequeue_valid_lines, "isp pre dequeue valid lines");

int isp_ch1_dequeue_delay_time;
module_param(isp_ch1_dequeue_delay_time, int, S_IRUGO);
MODULE_PARM_DESC(isp_ch1_dequeue_delay_time, "isp pre dequeue time, unit ms");

int isp_day_night_switch_drop_frame_num;
module_param(isp_day_night_switch_drop_frame_num, int, S_IRUGO);
MODULE_PARM_DESC(isp_day_night_switch_drop_frame_num, "isp day night switch drop frame number");

int isp_memopt;
module_param(isp_memopt, int, S_IRUGO);
MODULE_PARM_DESC(isp_memopt, "isp memory optimize");

int isp_printf(unsigned int level, unsigned char *fmt, ...)
{
	struct va_format vaf;
	va_list args;
	int r = 0;

	if(level >= print_level){
		va_start(args, fmt);

		vaf.fmt = fmt;
		vaf.va = &args;

		r = printk("%pV",&vaf);
		va_end(args);
		if(level >= ISP_ERROR_LEVEL)
			dump_stack();
	}
	return r;
}
EXPORT_SYMBOL(isp_printf);

int get_isp_clk(void)
{
	return isp_clk;
}

void *private_vmalloc(unsigned long size)
{
	void *addr = vmalloc(size);
	return addr;
}

void private_vfree(const void *addr)
{
	vfree(addr);
}

ktime_t private_ktime_set(const long secs, const unsigned long nsecs)
{
	return ktime_set(secs, nsecs);
}

void private_set_current_state(unsigned int state)
{
	__set_current_state(state);
	return;
}


int private_schedule_hrtimeout(ktime_t *ex, const enum hrtimer_mode mode)
{
	return schedule_hrtimeout(ex, mode);
}

bool private_schedule_work(struct work_struct *work)
{
	return schedule_work(work);
}


void private_do_gettimeofday(struct timeval *tv)
{
	do_gettimeofday(tv);
	return;
}


void private_dma_sync_single_for_device(struct device *dev,
							      dma_addr_t addr, size_t size, enum dma_data_direction dir)
{
	dma_sync_single_for_device(dev, addr, size, dir);
	return;
}

/* OEM-style wrappers for vfs read/write/llseek and addr_limit helpers */
ssize_t private_vfs_read(struct file *file, char __user *buf, size_t count, loff_t *pos)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,14,0)
	return kernel_read(file, buf, count, pos);
#else
	mm_segment_t oldfs = get_fs();
	set_fs(KERNEL_DS);
	ssize_t ret = vfs_read(file, buf, count, pos);
	set_fs(oldfs);
	return ret;
#endif
}

ssize_t private_vfs_write(struct file *file, const char __user *buf, size_t count, loff_t *pos)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,14,0)
	return kernel_write(file, buf, count, pos);
#else
	mm_segment_t oldfs = get_fs();
	set_fs(KERNEL_DS);
	ssize_t ret = vfs_write(file, buf, count, pos);
	set_fs(oldfs);
	return ret;
#endif
}

loff_t private_vfs_llseek(struct file *file, loff_t offset, int whence)
{
	return vfs_llseek(file, offset, whence);
}

mm_segment_t private_get_fs(void)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,10,0)
	return get_fs();
#else
	/* set_fs/get_fs removed; return KERNEL_DS as a harmless sentinel */
	return KERNEL_DS;
#endif
}

void private_set_fs(mm_segment_t val)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,10,0)
	set_fs(val);
#else
	/* No-op on modern kernels */
	(void)val;
#endif
}




