#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include "../include/tx_isp.h"

#define TX_ISP_PROC_JZ_DIR "jz"
#define TX_ISP_PROC_ISP_DIR "isp"
#define TX_ISP_PROC_ISP_W01_FILE "isp-ws0"
#define TX_ISP_PROC_ISP_W02_FILE "isp-ws1"

struct proc_context {
    struct proc_dir_entry *jz_dir;
    struct proc_dir_entry *isp_dir;
    struct proc_dir_entry *isp_w01_entry;
    struct proc_dir_entry *isp_w02_entry;
    struct tx_isp_dev *isp;
};

/* ISP-W01 file operations - the file userspace expects */
static int tx_isp_proc_w01_show(struct seq_file *m, void *v)
{
    struct tx_isp_dev *isp = m->private;

    if (!isp) {
        /* Output single frame counter value for libimp's fscanf */
        seq_printf(m, "0\n");
        return 0;
    }

    /* CRITICAL: libimp reads this with fscanf(stream, "%d", &var)
     * and expects a single incrementing frame counter value.
     * If this value doesn't change for 2+ reads, it logs "err:video drop"
     */
    seq_printf(m, "%u\n", isp->frame_count);
    
    return 0;
}

static ssize_t tx_isp_proc_w01_write(struct file *file, const char __user *buffer,
                                     size_t count, loff_t *ppos)
{
    struct seq_file *m = file->private_data;
    struct tx_isp_dev *isp = m->private;
    char cmd[64];
    
    if (count >= sizeof(cmd))
        return -EINVAL;
        
    if (copy_from_user(cmd, buffer, count))
        return -EFAULT;
        
    cmd[count] = '\0';
    
    pr_info("ISP W01 proc command: %s\n", cmd);
    
    /* Handle common ISP commands that userspace might send */
    if (strncmp(cmd, "snapraw", 7) == 0) {
        pr_info("ISP W01 snapraw command received\n");
        /* Handle raw snapshot command */
    } else if (strncmp(cmd, "enable", 6) == 0) {
        pr_info("ISP W01 enable command received\n");
        if (isp)
            isp->streaming_enabled = true;
    } else if (strncmp(cmd, "disable", 7) == 0) {
        pr_info("ISP W01 disable command received\n");
        if (isp)
            isp->streaming_enabled = false;
    }
    
    return count;
}

static int tx_isp_proc_w01_open(struct inode *inode, struct file *file)
{
    return single_open(file, tx_isp_proc_w01_show, PDE_DATA(inode));
}

static const struct file_operations tx_isp_proc_w01_fops = {
    .owner = THIS_MODULE,
    .open = tx_isp_proc_w01_open,
    .read = seq_read,
    .write = tx_isp_proc_w01_write,
    .llseek = seq_lseek,
    .release = single_release,
};

/* ISP-W02 file operations - the file userspace expects */
static int tx_isp_proc_w02_show(struct seq_file *m, void *v)
{
    struct tx_isp_dev *isp = m->private;

    if (!isp) {
        /* Output single frame counter value for libimp's fscanf */
        seq_printf(m, "0\n");
        return 0;
    }

    /* CRITICAL: libimp reads this with fscanf(stream, "%d", &var)
     * and expects a single incrementing frame counter value.
     * If this value doesn't change for 2+ reads, it logs "err:video drop"
     */
    seq_printf(m, "%u\n", isp->frame_count);
    
    return 0;
}

static ssize_t tx_isp_proc_w02_write(struct file *file, const char __user *buffer,
                                     size_t count, loff_t *ppos)
{
    struct seq_file *m = file->private_data;
    struct tx_isp_dev *isp = m->private;
    char cmd[64];
    
    if (count >= sizeof(cmd))
        return -EINVAL;
        
    if (copy_from_user(cmd, buffer, count))
        return -EFAULT;
        
    cmd[count] = '\0';
    
    pr_info("ISP W02 proc command: %s\n", cmd);
    
    /* Handle common ISP commands that userspace might send */
    if (strncmp(cmd, "snapraw", 7) == 0) {
        pr_info("ISP W02 snapraw command received\n");
        /* Handle raw snapshot command */
    } else if (strncmp(cmd, "enable", 6) == 0) {
        pr_info("ISP W02 enable command received\n");
        if (isp)
            isp->streaming_enabled = true;
    } else if (strncmp(cmd, "disable", 7) == 0) {
        pr_info("ISP W02 disable command received\n");
        if (isp)
            isp->streaming_enabled = false;
    }
    
    return count;
}

static int tx_isp_proc_w02_open(struct inode *inode, struct file *file)
{
    return single_open(file, tx_isp_proc_w02_show, PDE_DATA(inode));
}

static const struct file_operations tx_isp_proc_w02_fops = {
    .owner = THIS_MODULE,
    .open = tx_isp_proc_w02_open,
    .read = seq_read,
    .write = tx_isp_proc_w02_write,
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

    /* Create /proc/jz directory */
    proc->jz_dir = proc_mkdir(TX_ISP_PROC_JZ_DIR, NULL);
    if (!proc->jz_dir) {
        pr_err("Failed to create /proc/jz directory\n");
        goto err_free_proc;
    }

    /* Create /proc/jz/isp directory */
    proc->isp_dir = proc_mkdir(TX_ISP_PROC_ISP_DIR, proc->jz_dir);
    if (!proc->isp_dir) {
        pr_err("Failed to create /proc/jz/isp directory\n");
        goto err_remove_jz_dir;
    }

    /* Create /proc/jz/isp/isp-w01 file - first expected proc entry */
    proc->isp_w01_entry = proc_create_data(TX_ISP_PROC_ISP_W01_FILE, 0664, proc->isp_dir,
                                          &tx_isp_proc_w01_fops, isp);
    if (!proc->isp_w01_entry) {
        pr_err("Failed to create /proc/jz/isp/isp-w01 entry\n");
        goto err_remove_isp_dir;
    }

    /* Create /proc/jz/isp/isp-w02 file - second expected proc entry */
    proc->isp_w02_entry = proc_create_data(TX_ISP_PROC_ISP_W02_FILE, 0664, proc->isp_dir,
                                          &tx_isp_proc_w02_fops, isp);
    if (!proc->isp_w02_entry) {
        pr_err("Failed to create /proc/jz/isp/isp-w02 entry\n");
        goto err_remove_w01_entry;
    }

    pr_info("Created proc entries: /proc/jz/isp/isp-w01 and /proc/jz/isp/isp-w02\n");
    return 0;

err_remove_w01_entry:
    remove_proc_entry(TX_ISP_PROC_ISP_W01_FILE, proc->isp_dir);

err_remove_isp_dir:
    remove_proc_entry(TX_ISP_PROC_ISP_DIR, proc->jz_dir);
err_remove_jz_dir:
    remove_proc_entry(TX_ISP_PROC_JZ_DIR, NULL);
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

    if (proc->isp_w01_entry)
        remove_proc_entry(TX_ISP_PROC_ISP_W01_FILE, proc->isp_dir);
    if (proc->isp_w02_entry)
        remove_proc_entry(TX_ISP_PROC_ISP_W02_FILE, proc->isp_dir);
    if (proc->isp_dir)
        remove_proc_entry(TX_ISP_PROC_ISP_DIR, proc->jz_dir);
    if (proc->jz_dir)
        remove_proc_entry(TX_ISP_PROC_JZ_DIR, NULL);

    kfree(proc);
    isp->proc_context = NULL;
    pr_info("Removed proc entries: /proc/jz/isp/isp-w01 and /proc/jz/isp/isp-w02\n");
}
