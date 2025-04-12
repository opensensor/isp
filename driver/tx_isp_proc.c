#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include "../include/tx_isp.h"

#define TX_ISP_PROC_DIR "tx-isp"

struct proc_context {
    struct proc_dir_entry *proc_dir;
    struct proc_dir_entry *info_entry;
    struct proc_dir_entry *status_entry;
    struct proc_dir_entry *stats_entry;
    struct tx_isp_dev *isp;
};

/* Info file operations */
static int tx_isp_proc_info_show(struct seq_file *m, void *v)
{
    struct tx_isp_dev *isp = m->private;

    seq_printf(m, "TX ISP Driver Information:\n");
    seq_printf(m, "Driver Version: %s\n", TX_ISP_DRIVER_VERSION);
    seq_printf(m, "Device Name: %s\n", TX_ISP_DEVICE_NAME);
    
    if (isp) {
        seq_printf(m, "Sensor Name: %s\n", isp->sensor_name);
        seq_printf(m, "Sensor Resolution: %dx%d\n", 
                  isp->sensor_width, isp->sensor_height);
        seq_printf(m, "Frame Count: %u\n", isp->frame_count);
    }

    return 0;
}

static int tx_isp_proc_info_open(struct inode *inode, struct file *file)
{
    return single_open(file, tx_isp_proc_info_show, PDE_DATA(inode));
}

static const struct file_operations tx_isp_proc_info_fops = {
    .open = tx_isp_proc_info_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};

/* Status file operations */
static int tx_isp_proc_status_show(struct seq_file *m, void *v)
{
    struct tx_isp_dev *isp = m->private;
    u32 core_status, vic_status, csi_status;

    if (!isp)
        return -EINVAL;

    core_status = isp_read32(ISP_STATUS);
    vic_status = vic_read32(VIC_STATUS);
    
    seq_printf(m, "ISP Status:\n");
    seq_printf(m, "Core Status: 0x%08x\n", core_status);
    seq_printf(m, "VIC Status: 0x%08x\n", vic_status);
    seq_printf(m, "Streaming: %s\n", isp->streaming_enabled ? "enabled" : "disabled");
    seq_printf(m, "WDR Mode: %d\n", isp->wdr_mode);
    seq_printf(m, "AE Enabled: %d\n", isp->ae_algo_enabled);

    return 0;
}

static int tx_isp_proc_status_open(struct inode *inode, struct file *file)
{
    return single_open(file, tx_isp_proc_status_show, PDE_DATA(inode));
}

static const struct file_operations tx_isp_proc_status_fops = {
    .open = tx_isp_proc_status_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};

/* Statistics file operations */
static int tx_isp_proc_stats_show(struct seq_file *m, void *v)
{
    struct tx_isp_dev *isp = m->private;
    unsigned long flags;

    if (!isp)
        return -EINVAL;

    spin_lock_irqsave(&isp->ae_lock, flags);
    if (isp->ae_valid) {
        seq_printf(m, "AE Statistics:\n");
        seq_printf(m, "Frame Count: %u\n", isp->frame_count);
        /* Add more AE statistics as needed */
    }
    spin_unlock_irqrestore(&isp->ae_lock, flags);

    return 0;
}

static int tx_isp_proc_stats_open(struct inode *inode, struct file *file)
{
    return single_open(file, tx_isp_proc_stats_show, PDE_DATA(inode));
}

static const struct file_operations tx_isp_proc_stats_fops = {
    .open = tx_isp_proc_stats_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};

/* Initialize proc filesystem entries */
int tx_isp_proc_init(struct tx_isp_dev *isp)
{
    struct proc_context *proc;

    if (!isp)
        return -EINVAL;

    proc = kzalloc(sizeof(*proc), GFP_KERNEL);
    if (!proc)
        return -ENOMEM;

    proc->isp = isp;
    isp->proc_context = proc;

    /* Create proc directory */
    proc->proc_dir = proc_mkdir(TX_ISP_PROC_DIR, NULL);
    if (!proc->proc_dir) {
        ISP_ERROR("Failed to create proc directory\n");
        goto err_free_proc;
    }

    /* Create info entry */
    proc->info_entry = proc_create_data("info", 0444, proc->proc_dir,
                                      &tx_isp_proc_info_fops, isp);
    if (!proc->info_entry) {
        ISP_ERROR("Failed to create info entry\n");
        goto err_remove_dir;
    }

    /* Create status entry */
    proc->status_entry = proc_create_data("status", 0444, proc->proc_dir,
                                        &tx_isp_proc_status_fops, isp);
    if (!proc->status_entry) {
        ISP_ERROR("Failed to create status entry\n");
        goto err_remove_info;
    }

    /* Create statistics entry */
    proc->stats_entry = proc_create_data("stats", 0444, proc->proc_dir,
                                       &tx_isp_proc_stats_fops, isp);
    if (!proc->stats_entry) {
        ISP_ERROR("Failed to create stats entry\n");
        goto err_remove_status;
    }

    return 0;

err_remove_status:
    remove_proc_entry("status", proc->proc_dir);
err_remove_info:
    remove_proc_entry("info", proc->proc_dir);
err_remove_dir:
    remove_proc_entry(TX_ISP_PROC_DIR, NULL);
err_free_proc:
    kfree(proc);
    return -ENOMEM;
}

/* Cleanup proc filesystem entries */
void tx_isp_proc_exit(struct tx_isp_dev *isp)
{
    struct proc_context *proc;

    if (!isp || !isp->proc_context)
        return;

    proc = isp->proc_context;

    if (proc->stats_entry)
        remove_proc_entry("stats", proc->proc_dir);
    if (proc->status_entry)
        remove_proc_entry("status", proc->proc_dir);
    if (proc->info_entry)
        remove_proc_entry("info", proc->proc_dir);
    if (proc->proc_dir)
        remove_proc_entry(TX_ISP_PROC_DIR, NULL);

    kfree(proc);
    isp->proc_context = NULL;
}
