#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include "../include/tx_isp.h"

#define TX_ISP_PROC_JZ_DIR "jz"
#define TX_ISP_PROC_ISP_DIR "isp"
#define TX_ISP_PROC_ISP_W00_FILE "isp-w00"
#define TX_ISP_PROC_ISP_W01_FILE "isp-w01"
#define TX_ISP_PROC_ISP_W02_FILE "isp-w02"
#define TX_ISP_PROC_ISP_FS_FILE "isp-fs"
#define TX_ISP_PROC_ISP_M0_FILE "isp-m0"
#define TX_ISP_PROC_CSI_FILE "csi"
#define TX_ISP_PROC_VIC_FILE "vic"

struct proc_context {
    struct proc_dir_entry *jz_dir;
    struct proc_dir_entry *isp_dir;
    struct proc_dir_entry *isp_w00_entry;
    struct proc_dir_entry *isp_w01_entry;
    struct proc_dir_entry *isp_w02_entry;
    struct proc_dir_entry *isp_fs_entry;
    struct proc_dir_entry *isp_m0_entry;
    struct proc_dir_entry *csi_entry;
    struct proc_dir_entry *vic_entry;
    struct tx_isp_dev *isp;
    bool jz_dir_created;  /* Track if we created jz_dir or just got reference */
};

/* ISP-W00 file operations - matches reference driver behavior */
static int tx_isp_proc_w00_show(struct seq_file *m, void *v)
{
    struct tx_isp_dev *isp = m->private;

    if (!isp) {
        seq_printf(m, "0");
        return 0;
    }

    /* Reference driver outputs single integer for vic frame counter */
    seq_printf(m, "%u", isp->frame_count);
    
    return 0;
}

static int tx_isp_proc_w00_open(struct inode *inode, struct file *file)
{
    return single_open(file, tx_isp_proc_w00_show, PDE_DATA(inode));
}

static const struct file_operations tx_isp_proc_w00_fops = {
    .owner = THIS_MODULE,
    .open = tx_isp_proc_w00_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};

/* ISP-W01 file operations - matches reference driver behavior */
static int tx_isp_proc_w01_show(struct seq_file *m, void *v)
{
    struct tx_isp_dev *isp = m->private;

    if (!isp) {
        seq_printf(m, "0");
        return 0;
    }

    /* Reference driver outputs single integer for vic frame counter */
    seq_printf(m, "%u", isp->frame_count);
    
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

/* ISP-W02 file operations - CRITICAL: Match reference driver output format */
static int tx_isp_proc_w02_show(struct seq_file *m, void *v)
{
    struct tx_isp_dev *isp = m->private;

    if (!isp) {
        /* Output expected format: frame_counter, 0 on first line, then 13 zeros */
        seq_printf(m, " 0, 0\n");
        seq_printf(m, "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0\n");
        return 0;
    }

    /* CRITICAL: Output exact format as reference driver:
     * Line 1: " frame_count, 0"
     * Line 2: "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0"
     * The frame_count increments to show ISP is processing frames
     */
    seq_printf(m, " %u, 0\n", isp->frame_count);
    seq_printf(m, "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0\n");
    
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

/* ISP-FS file operations - CRITICAL MISSING PIECE */
static int tx_isp_proc_fs_show(struct seq_file *m, void *v)
{
    struct tx_isp_dev *isp = m->private;

    if (!isp) {
        seq_printf(m, "0");
        return 0;
    }

    /* FS proc entry for Frame Source status */
    seq_printf(m, "%u", isp->frame_count);
    
    return 0;
}

static ssize_t tx_isp_proc_fs_write(struct file *file, const char __user *buffer,
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
    
    pr_info("ISP FS proc command: %s\n", cmd);
    
    /* Handle FS-specific commands */
    if (strncmp(cmd, "enable", 6) == 0) {
        pr_info("ISP FS enable command received\n");
        if (isp)
            isp->streaming_enabled = true;
    } else if (strncmp(cmd, "disable", 7) == 0) {
        pr_info("ISP FS disable command received\n");
        if (isp)
            isp->streaming_enabled = false;
    }
    
    return count;
}

static int tx_isp_proc_fs_open(struct inode *inode, struct file *file)
{
    return single_open(file, tx_isp_proc_fs_show, PDE_DATA(inode));
}

static const struct file_operations tx_isp_proc_fs_fops = {
    .owner = THIS_MODULE,
    .open = tx_isp_proc_fs_open,
    .read = seq_read,
    .write = tx_isp_proc_fs_write,
    .llseek = seq_lseek,
    .release = single_release,
};

/* ISP-M0 file operations - CRITICAL MISSING PIECE */
static int tx_isp_proc_m0_show(struct seq_file *m, void *v)
{
    struct tx_isp_dev *isp = m->private;

    if (!isp) {
        seq_printf(m, "0");
        return 0;
    }

    /* M0 proc entry for main ISP core status */
    seq_printf(m, "%u", isp->frame_count);
    
    return 0;
}

static ssize_t tx_isp_proc_m0_write(struct file *file, const char __user *buffer,
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
    
    pr_info("ISP M0 proc command: %s\n", cmd);
    
    /* Handle M0-specific commands */
    if (strncmp(cmd, "enable", 6) == 0) {
        pr_info("ISP M0 enable command received\n");
        if (isp)
            isp->streaming_enabled = true;
    } else if (strncmp(cmd, "disable", 7) == 0) {
        pr_info("ISP M0 disable command received\n");
        if (isp)
            isp->streaming_enabled = false;
    }
    
    return count;
}

static int tx_isp_proc_m0_open(struct inode *inode, struct file *file)
{
    return single_open(file, tx_isp_proc_m0_show, PDE_DATA(inode));
}

static const struct file_operations tx_isp_proc_m0_fops = {
    .owner = THIS_MODULE,
    .open = tx_isp_proc_m0_open,
    .read = seq_read,
    .write = tx_isp_proc_m0_write,
    .llseek = seq_lseek,
    .release = single_release,
};

/* CSI file operations */
static int tx_isp_proc_csi_show(struct seq_file *m, void *v)
{
    seq_printf(m, "csi\n");
    return 0;
}

static int tx_isp_proc_csi_open(struct inode *inode, struct file *file)
{
    return single_open(file, tx_isp_proc_csi_show, PDE_DATA(inode));
}

static const struct file_operations tx_isp_proc_csi_fops = {
    .owner = THIS_MODULE,
    .open = tx_isp_proc_csi_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};

/* VIC file operations */
static int tx_isp_proc_vic_show(struct seq_file *m, void *v)
{
    seq_printf(m, "vic\n");
    return 0;
}

static int tx_isp_proc_vic_open(struct inode *inode, struct file *file)
{
    return single_open(file, tx_isp_proc_vic_show, PDE_DATA(inode));
}

static const struct file_operations tx_isp_proc_vic_fops = {
    .owner = THIS_MODULE,
    .open = tx_isp_proc_vic_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};

/* Global proc context */
static struct proc_context *tx_isp_proc_ctx = NULL;

/* Helper function to safely get or create proc directory on Linux 3.10 with overlays */
static struct proc_dir_entry *get_or_create_proc_dir(const char *name, struct proc_dir_entry *parent, bool *created)
{
    struct proc_dir_entry *dir = NULL;
    
    *created = false;
    
    /* On Linux 3.10, try proc_mkdir which should handle existing directories gracefully */
    dir = proc_mkdir(name, parent);
    if (dir) {
        /* Success - assume we created it for cleanup purposes */
        *created = true;
        pr_info("Created or accessed proc directory: %s\n", name);
        return dir;
    }
    
    /* 
     * If proc_mkdir failed, it could be because:
     * 1. Directory exists but proc_mkdir doesn't return it (overlay fs issue)
     * 2. Insufficient permissions
     * 3. Out of memory
     * 
     * For Linux 3.10 with overlays, we try a different approach:
     * Create a dummy file to test if the parent directory is writable
     */
    if (parent) {
        struct proc_dir_entry *test_entry;
        test_entry = proc_create("__tx_isp_test__", 0644, parent, &tx_isp_proc_csi_fops);
        if (test_entry) {
            /* Parent is writable, remove test entry */
            proc_remove(test_entry);
            
            /* The directory might exist but be inaccessible via proc_mkdir
             * In this case, we'll proceed without the jz directory reference
             * and create our subdirectory directly under proc root if needed
             */
            pr_warn("Directory %s may exist but is not accessible via proc_mkdir\n", name);
            return NULL;
        }
    }
    
    pr_err("Failed to create or access proc directory: %s\n", name);
    return NULL;
}
/* Create all proc entries - matches reference driver layout */
int tx_isp_create_proc_entries(struct tx_isp_dev *isp)
{
    struct proc_context *ctx;

    pr_info("*** tx_isp_create_proc_entries: Creating proc entries to match reference driver ***\n");

    ctx = kzalloc(sizeof(struct proc_context), GFP_KERNEL);
    if (!ctx) {
        pr_err("Failed to allocate proc context\n");
        return -ENOMEM;
    }

    ctx->isp = isp;
    tx_isp_proc_ctx = ctx;

    /* Safely handle existing /proc/jz directory or create if needed */
    ctx->jz_dir_created = false;
    
    /* Try to create directory - proc_mkdir returns existing dir if it exists */
    ctx->jz_dir = proc_mkdir(TX_ISP_PROC_JZ_DIR, NULL);
    if (ctx->jz_dir) {
        /* Success - either created new or got reference to existing */
        pr_info("Using /proc/%s directory\n", TX_ISP_PROC_JZ_DIR);
        ctx->jz_dir_created = true;  /* We'll manage this reference */
    } else {
        /* Failed completely - this is an actual error */
        pr_err("Failed to access or create /proc/%s\n", TX_ISP_PROC_JZ_DIR);
        goto error_free_ctx;
    }

    /* Create /proc/jz/isp directory */
    ctx->isp_dir = proc_mkdir(TX_ISP_PROC_ISP_DIR, ctx->jz_dir);
    if (!ctx->isp_dir) {
        pr_err("Failed to create /proc/%s/%s\n", TX_ISP_PROC_JZ_DIR, TX_ISP_PROC_ISP_DIR);
        goto error_free_ctx;
    }

    /* Create /proc/jz/isp/isp-w00 */
    ctx->isp_w00_entry = proc_create_data(TX_ISP_PROC_ISP_W00_FILE, 0644, ctx->isp_dir,
                                         &tx_isp_proc_w00_fops, isp);
    if (!ctx->isp_w00_entry) {
        pr_err("Failed to create isp-w00 proc entry\n");
        goto error_remove_isp_dir;
    }
    pr_info("Created proc entry: /proc/jz/isp/isp-w00\n");

    /* Create /proc/jz/isp/isp-w01 */
    ctx->isp_w01_entry = proc_create_data(TX_ISP_PROC_ISP_W01_FILE, 0644, ctx->isp_dir,
                                         &tx_isp_proc_w01_fops, isp);
    if (!ctx->isp_w01_entry) {
        pr_err("Failed to create isp-w01 proc entry\n");
        goto error_remove_w00;
    }
    pr_info("Created proc entry: /proc/jz/isp/isp-w01\n");

    /* Create /proc/jz/isp/isp-w02 */
    ctx->isp_w02_entry = proc_create_data(TX_ISP_PROC_ISP_W02_FILE, 0644, ctx->isp_dir,
                                         &tx_isp_proc_w02_fops, isp);
    if (!ctx->isp_w02_entry) {
        pr_err("Failed to create isp-w02 proc entry\n");
        goto error_remove_w01;
    }
    pr_info("Created proc entry: /proc/jz/isp/isp-w02\n");

    /* Create /proc/jz/isp/isp-fs - CRITICAL FOR REFERENCE DRIVER COMPATIBILITY */
    ctx->isp_fs_entry = proc_create_data(TX_ISP_PROC_ISP_FS_FILE, 0644, ctx->isp_dir,
                                        &tx_isp_proc_fs_fops, isp);
    if (!ctx->isp_fs_entry) {
        pr_err("Failed to create isp-fs proc entry\n");
        goto error_remove_w02;
    }
    pr_info("*** CREATED PROC ENTRY: /proc/jz/isp/isp-fs (CRITICAL FOR FS FUNCTIONALITY) ***\n");

    /* Create /proc/jz/isp/isp-m0 - CRITICAL FOR REFERENCE DRIVER COMPATIBILITY */
    ctx->isp_m0_entry = proc_create_data(TX_ISP_PROC_ISP_M0_FILE, 0644, ctx->isp_dir,
                                        &tx_isp_proc_m0_fops, isp);
    if (!ctx->isp_m0_entry) {
        pr_err("Failed to create isp-m0 proc entry\n");
        goto error_remove_fs;
    }
    pr_info("*** CREATED PROC ENTRY: /proc/jz/isp/isp-m0 (CRITICAL FOR M0 FUNCTIONALITY) ***\n");

    /* Create /proc/jz/isp/csi */
    ctx->csi_entry = proc_create_data(TX_ISP_PROC_CSI_FILE, 0644, ctx->isp_dir,
                                     &tx_isp_proc_csi_fops, isp);
    if (!ctx->csi_entry) {
        pr_err("Failed to create csi proc entry\n");
        goto error_remove_m0;
    }
    pr_info("Created proc entry: /proc/jz/isp/csi\n");

    /* Create /proc/jz/isp/vic */
    ctx->vic_entry = proc_create_data(TX_ISP_PROC_VIC_FILE, 0644, ctx->isp_dir,
                                     &tx_isp_proc_vic_fops, isp);
    if (!ctx->vic_entry) {
        pr_err("Failed to create vic proc entry\n");
        goto error_remove_csi;
    }
    pr_info("Created proc entry: /proc/jz/isp/vic\n");

    pr_info("*** ALL PROC ENTRIES CREATED SUCCESSFULLY - MATCHES REFERENCE DRIVER LAYOUT ***\n");
    pr_info("*** /proc/jz/isp/ now contains: isp-w00, isp-w01, isp-w02, isp-fs, isp-m0, csi, vic ***\n");

    return 0;

error_remove_csi:
    proc_remove(ctx->csi_entry);
error_remove_m0:
    proc_remove(ctx->isp_m0_entry);
error_remove_fs:
    proc_remove(ctx->isp_fs_entry);
error_remove_w02:
    proc_remove(ctx->isp_w02_entry);
error_remove_w01:
    proc_remove(ctx->isp_w01_entry);
error_remove_w00:
    proc_remove(ctx->isp_w00_entry);
error_remove_isp_dir:
    proc_remove(ctx->isp_dir);
    /* Don't remove jz_dir as it may be used by other modules */
error_free_ctx:
    kfree(ctx);
    tx_isp_proc_ctx = NULL;
    return -1;
}

/* Remove all proc entries */
void tx_isp_remove_proc_entries(void)
{
    struct proc_context *ctx = tx_isp_proc_ctx;
    
    if (!ctx) {
        return;
    }
    
    pr_info("*** tx_isp_remove_proc_entries: Cleaning up proc entries ***\n");
    
    if (ctx->vic_entry) {
        proc_remove(ctx->vic_entry);
    }
    if (ctx->csi_entry) {
        proc_remove(ctx->csi_entry);
    }
    if (ctx->isp_m0_entry) {
        proc_remove(ctx->isp_m0_entry);
    }
    if (ctx->isp_fs_entry) {
        proc_remove(ctx->isp_fs_entry);
    }
    if (ctx->isp_w02_entry) {
        proc_remove(ctx->isp_w02_entry);
    }
    if (ctx->isp_w01_entry) {
        proc_remove(ctx->isp_w01_entry);
    }
    if (ctx->isp_w00_entry) {
        proc_remove(ctx->isp_w00_entry);
    }
    if (ctx->isp_dir) {
        proc_remove(ctx->isp_dir);
    }
    if (ctx->jz_dir) {
        proc_remove(ctx->jz_dir);
    }
    
    kfree(ctx);
    tx_isp_proc_ctx = NULL;
    
    pr_info("All proc entries removed\n");
}

/* Export symbols */
EXPORT_SYMBOL(tx_isp_create_proc_entries);
EXPORT_SYMBOL(tx_isp_remove_proc_entries);
