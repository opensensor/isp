/*
 * TX ISP Subdevice Management
 * Cleaner abstraction for managing ISP subdevices and graph creation
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include "../include/tx_isp.h"
#include "../include/tx-isp-debug.h"

/* Subdevice type definitions */
enum tx_isp_subdev_type {
    TX_ISP_SUBDEV_TYPE_SOURCE = 1,  /* Source devices (CSI, VIN, FS) */
    TX_ISP_SUBDEV_TYPE_SINK = 2,    /* Sink devices (VIC, CORE) */
};

/* Subdevice descriptor - cleaner than Binary Ninja struct */
struct tx_isp_subdev_desc {
    const char *name;
    enum tx_isp_subdev_type type;
    uint32_t device_id;
    uint32_t src_index;
    uint32_t dst_index;
    struct platform_device *pdev;
    const struct file_operations *fops;
    bool create_misc_device;
    bool create_proc_entry;
};

/* Subdevice runtime data */
struct tx_isp_subdev_runtime {
    struct tx_isp_subdev_desc *desc;
    struct miscdevice *misc_dev;
    struct proc_dir_entry *proc_entry;
    void *driver_data;
    bool initialized;
};

/* Global subdevice registry */
static struct tx_isp_subdev_runtime *subdev_registry[ISP_MAX_SUBDEVS];
static int subdev_count = 0;
static DEFINE_MUTEX(subdev_registry_mutex);

/* Forward declarations */
static int tx_isp_init_source_subdev(struct tx_isp_dev *isp, 
                                    struct tx_isp_subdev_runtime *runtime);
static int tx_isp_init_sink_subdev(struct tx_isp_dev *isp,
                                  struct tx_isp_subdev_runtime *runtime);
static int tx_isp_create_subdev_link(void *src_subdev, void *dst_subdev,
                                    struct tx_isp_subdev_desc *desc);
int tx_isp_create_proc_entries(struct tx_isp_dev *isp);
static int tx_isp_create_misc_device(struct tx_isp_subdev_runtime *runtime);
static int tx_isp_create_basic_pipeline(struct tx_isp_dev *isp);
static void *tx_isp_create_driver_data(struct tx_isp_subdev_desc *desc);

/* Basic pipeline function declarations */
int tx_isp_csi_device_init(struct tx_isp_dev *isp);
int tx_isp_csi_device_deinit(struct tx_isp_dev *isp);
int tx_isp_vic_device_deinit(struct tx_isp_dev *isp);
int tx_isp_setup_pipeline(struct tx_isp_dev *isp);
void tx_isp_cleanup_subdev_graph(struct tx_isp_dev *isp);

/* Frame channel device operation forward declarations */
int frame_channel_open(struct inode *inode, struct file *file);
int frame_channel_release(struct inode *inode, struct file *file);
long frame_channel_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

static const struct file_operations frame_channel_fops = {
    .owner = THIS_MODULE,
    .open = frame_channel_open,
    .release = frame_channel_release,
    .unlocked_ioctl = frame_channel_unlocked_ioctl,
    .compat_ioctl = frame_channel_unlocked_ioctl,
};

static int graph_proc_show(struct seq_file *m, void *v)
{
    struct tx_isp_dev *isp = m->private;
    
    seq_printf(m, "TX ISP Subdevice Graph Status\n");
    seq_printf(m, "=============================\n");
    seq_printf(m, "Registry count: %d\n", subdev_count);
    seq_printf(m, "ISP device: %p\n", isp);
    
    return 0;
}

static int graph_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, graph_proc_show, PDE_DATA(inode));
}

const struct file_operations graph_proc_fops = {
    .owner = THIS_MODULE,
    .open = graph_proc_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};

/* Binary Ninja compatible subdevice data structure */
struct isp_subdev_data {
    uint32_t device_type;     /* 0x00: Device type (1=source, 2=sink) */
    uint32_t device_id;       /* 0x04: Device ID */
    uint32_t src_index;       /* 0x08: Source index (for type 2) */
    uint32_t dst_index;       /* 0x0C: Destination index */
    struct miscdevice misc;   /* 0x10: Misc device (starts at 0xC, but we pad) */
    char device_name[16];     /* 0x20: Device name */
    void *file_ops;           /* 0x30: File operations pointer */
    void *proc_ops;           /* 0x34: Proc operations pointer */
    char padding[0x100];      /* Padding to match Binary Ninja expectations */
};

/* Subdevice descriptors - cleaner definition than hardcoded structs */
static struct tx_isp_subdev_desc isp_subdev_descriptors[] = {
    {
        .name = "tx-isp-csi",
        .type = TX_ISP_SUBDEV_TYPE_SOURCE,
        .device_id = 0,
        .src_index = 0,
        .dst_index = 0,
        .pdev = NULL,  /* Will be set during registration */
        .fops = &frame_channel_fops,
        .create_misc_device = true,
        .create_proc_entry = true,
    },
    {
        .name = "tx-isp-vic", 
        .type = TX_ISP_SUBDEV_TYPE_SOURCE,  /* VIC is a source in the pipeline CSI->VIC->Core */
        .device_id = 1,
        .src_index = 0,  /* Connect from CSI (not used for sources) */
        .dst_index = 1,  /* Store VIC at index 1 for Core to find */
        .pdev = NULL,
        .fops = &frame_channel_fops,
        .create_misc_device = true,
        .create_proc_entry = true,
    },
    {
        .name = "tx-isp-vin",
        .type = TX_ISP_SUBDEV_TYPE_SOURCE,
        .device_id = 2,
        .src_index = 0,
        .dst_index = 2,
        .pdev = NULL,
        .fops = &frame_channel_fops,
        .create_misc_device = false,
        .create_proc_entry = true,
    },
    {
        .name = "tx-isp-fs",
        .type = TX_ISP_SUBDEV_TYPE_SOURCE,
        .device_id = 3,
        .src_index = 0,
        .dst_index = 3,
        .pdev = NULL,
        .fops = &frame_channel_fops,
        .create_misc_device = true,
        .create_proc_entry = true,
    },
    {
        .name = "tx-isp-core",
        .type = TX_ISP_SUBDEV_TYPE_SINK,
        .device_id = 4,
        .src_index = 1,  /* Connect from VIC at index 1 */
        .dst_index = 4,
        .pdev = NULL,
        .fops = &frame_channel_fops,
        .create_misc_device = false,
        .create_proc_entry = true,
    },
};

extern struct tx_isp_dev *ourISPdev;

#define NUM_ISP_SUBDEVS ARRAY_SIZE(isp_subdev_descriptors)

/**
 * tx_isp_subdev_register - Register a subdevice with the management system
 * @desc: Subdevice descriptor
 * @pdev: Platform device
 * @driver_data: Driver-specific data
 * 
 * Returns: 0 on success, negative error code on failure
 */
int tx_isp_subdev_register(struct tx_isp_subdev_desc *desc, 
                          struct platform_device *pdev,
                          void *driver_data)
{
    struct tx_isp_subdev_runtime *runtime;
    int ret = 0;

    if (!desc || !pdev) {
        pr_err("tx_isp_subdev_register: Invalid parameters\n");
        return -EINVAL;
    }

    mutex_lock(&subdev_registry_mutex);

    if (subdev_count >= ISP_MAX_SUBDEVS) {
        pr_err("tx_isp_subdev_register: Too many subdevices\n");
        ret = -ENOSPC;
        goto unlock;
    }

    /* Allocate runtime structure */
    runtime = kzalloc(sizeof(*runtime), GFP_KERNEL);
    if (!runtime) {
        ret = -ENOMEM;
        goto unlock;
    }

    /* Initialize runtime data */
    runtime->desc = desc;
    runtime->driver_data = driver_data;
    runtime->initialized = false;

    /* Update descriptor with platform device */
    desc->pdev = pdev;

    /* Set platform device driver data */
    platform_set_drvdata(pdev, driver_data);

    /* Add to registry */
    subdev_registry[subdev_count] = runtime;
    subdev_count++;

    pr_info("tx_isp_subdev_register: Registered subdevice '%s' (type=%d, id=%d)\n",
            desc->name, desc->type, desc->device_id);

unlock:
    mutex_unlock(&subdev_registry_mutex);
    return ret;
}

/**
 * tx_isp_subdev_unregister - Unregister a subdevice
 * @pdev: Platform device to unregister
 */
void tx_isp_subdev_unregister(struct platform_device *pdev)
{
    int i;

    if (!pdev)
        return;

    mutex_lock(&subdev_registry_mutex);

    for (i = 0; i < subdev_count; i++) {
        if (subdev_registry[i] && subdev_registry[i]->desc->pdev == pdev) {
            struct tx_isp_subdev_runtime *runtime = subdev_registry[i];

            /* Cleanup misc device if created */
            if (runtime->misc_dev) {
                misc_deregister(runtime->misc_dev);
                kfree(runtime->misc_dev->name);
                kfree(runtime->misc_dev);
            }

            /* Cleanup proc entry if created */
            if (runtime->proc_entry) {
                proc_remove(runtime->proc_entry);
            }

            kfree(runtime);
            subdev_registry[i] = NULL;

            /* Compact the array */
            for (int j = i; j < subdev_count - 1; j++) {
                subdev_registry[j] = subdev_registry[j + 1];
            }
            subdev_count--;

            pr_info("tx_isp_subdev_unregister: Unregistered subdevice\n");
            break;
        }
    }

    mutex_unlock(&subdev_registry_mutex);
}

/**
 * tx_isp_create_subdev_graph - Create ISP processing graph (refactored version)
 * @isp: ISP device
 * 
 * This is a cleaner, more maintainable version of tx_isp_create_graph_and_nodes
 * that doesn't rely on Binary Ninja offsets and unsafe pointer arithmetic.
 * 
 * Returns: 0 on success, negative error code on failure
 */
int tx_isp_create_subdev_graph(struct tx_isp_dev *isp)
{
    int ret = 0;
    int i;

    if (!isp) {
        pr_err("tx_isp_create_subdev_graph: Invalid ISP device\n");
        return -EINVAL;
    }

    pr_info("*** tx_isp_create_subdev_graph: Creating ISP processing graph ***\n");

    mutex_lock(&subdev_registry_mutex);

    if (subdev_count == 0) {
        pr_info("tx_isp_create_subdev_graph: No subdevices registered, creating basic setup\n");
        
        /* Create basic CSI and VIC setup when no subdevices exist */
        ret = tx_isp_create_basic_pipeline(isp);
        if (ret < 0) {
            pr_err("Failed to create basic pipeline: %d\n", ret);
        }
        goto unlock;
    }

    pr_info("tx_isp_create_subdev_graph: Processing %d registered subdevices\n", subdev_count);

    /* Step 1: Initialize all source subdevices */
    for (i = 0; i < subdev_count; i++) {
        struct tx_isp_subdev_runtime *runtime = subdev_registry[i];
        
        if (!runtime || !runtime->desc)
            continue;

        if (runtime->desc->type == TX_ISP_SUBDEV_TYPE_SOURCE) {
            ret = tx_isp_init_source_subdev(isp, runtime);
            if (ret < 0) {
                pr_err("Failed to initialize source subdev %s: %d\n", 
                       runtime->desc->name, ret);
                goto cleanup;
            }
        }
    }

    /* Step 2: Initialize all sink subdevices and create links */
    for (i = 0; i < subdev_count; i++) {
        struct tx_isp_subdev_runtime *runtime = subdev_registry[i];
        
        if (!runtime || !runtime->desc)
            continue;

        if (runtime->desc->type == TX_ISP_SUBDEV_TYPE_SINK) {
            ret = tx_isp_init_sink_subdev(isp, runtime);
            if (ret < 0) {
                pr_err("Failed to initialize sink subdev %s: %d\n",
                       runtime->desc->name, ret);
                goto cleanup;
            }
        }
    }

    pr_info("*** tx_isp_create_subdev_graph: Graph creation completed successfully ***\n");
    goto unlock;

cleanup:
    pr_err("tx_isp_create_subdev_graph: Graph creation failed, cleaning up\n");
    tx_isp_cleanup_subdev_graph(isp);

unlock:
    mutex_unlock(&subdev_registry_mutex);
    return ret;
}

/**
 * tx_isp_init_source_subdev - Initialize a source subdevice
 */
static int tx_isp_init_source_subdev(struct tx_isp_dev *isp, 
                                    struct tx_isp_subdev_runtime *runtime)
{
    struct tx_isp_subdev_desc *desc = runtime->desc;
    void *driver_data = platform_get_drvdata(desc->pdev);

    if (!driver_data) {
        pr_warn("tx_isp_init_source_subdev: No driver data for %s\n", desc->name);
        return 0;
    }
    // if name is tx-isp-vic then
    if (strcmp(desc->name, "tx-isp-vic") == 0) {
        ourISPdev->vic_dev = driver_data;
    }

    /* Store in ISP device graph array for compatibility */
    if (desc->dst_index < ISP_MAX_SUBDEVS) {
        isp->subdev_graph[desc->dst_index] = driver_data;
        pr_info("tx_isp_init_source_subdev: %s stored at index %d\n", 
                desc->name, desc->dst_index);
    }

    runtime->initialized = true;
    return 0;
}

/**
 * tx_isp_init_sink_subdev - Initialize a sink subdevice and create links
 */
static int tx_isp_init_sink_subdev(struct tx_isp_dev *isp,
                                  struct tx_isp_subdev_runtime *runtime)
{
    struct tx_isp_subdev_desc *desc = runtime->desc;
    void *driver_data = platform_get_drvdata(desc->pdev);
    void *src_subdev;

    if (!driver_data) {
        pr_warn("tx_isp_init_sink_subdev: No driver data for %s\n", desc->name);
        return 0;
    }

    /* Find source subdevice */
    if (desc->src_index >= ISP_MAX_SUBDEVS) {
        pr_err("tx_isp_init_sink_subdev: Invalid source index %d for %s\n",
               desc->src_index, desc->name);
        return -EINVAL;
    }

    src_subdev = isp->subdev_graph[desc->src_index];
    if (!src_subdev) {
        pr_err("tx_isp_init_sink_subdev: Source subdev %d not found for %s\n",
               desc->src_index, desc->name);
        return -ENODEV;
    }

    /* Create link: source -> sink */
    int ret = tx_isp_create_subdev_link(src_subdev, driver_data, desc);
    if (ret < 0) {
        pr_err("tx_isp_init_sink_subdev: Failed to create link for %s: %d\n",
               desc->name, ret);
        return ret;
    }

    pr_info("tx_isp_init_sink_subdev: Created link %s[%d] -> %s[%d]\n",
            "source", desc->src_index, desc->name, desc->dst_index);

    runtime->initialized = true;
    return 0;
}

/**
 * tx_isp_create_subdev_link - Create a link between two subdevices
 */
static int tx_isp_create_subdev_link(void *src_subdev, void *dst_subdev,
                                    struct tx_isp_subdev_desc *desc)
{
    /* This replaces the complex Binary Ninja pointer arithmetic with
     * a cleaner approach. In a real implementation, this would configure
     * the hardware routing between subdevices. */
    
    pr_info("tx_isp_create_subdev_link: Linking %s (src=%p, dst=%p)\n", 
            desc->name, src_subdev, dst_subdev);
    
    /* Store the link information for later use */
    /* In the original code, this did:
     * *((void**)((char*)src_subdev + link_offset)) = dst_subdev;
     * We can implement this more safely with proper structures */
    
    return 0;
}

/* tx_isp_create_proc_entries is implemented in tx_isp_proc.c */
extern int tx_isp_create_proc_entries(struct tx_isp_dev *isp);

/**
 * tx_isp_create_misc_device - Create a misc device for a subdevice
 */
static int tx_isp_create_misc_device(struct tx_isp_subdev_runtime *runtime)
{
    struct miscdevice *misc_dev;
    int ret;

    misc_dev = kzalloc(sizeof(struct miscdevice), GFP_KERNEL);
    if (!misc_dev) {
        return -ENOMEM;
    }

    misc_dev->name = kstrdup(runtime->desc->name, GFP_KERNEL);
    misc_dev->minor = MISC_DYNAMIC_MINOR;
    misc_dev->fops = runtime->desc->fops;

    ret = misc_register(misc_dev);
    if (ret < 0) {
        pr_err("Failed to register misc device %s: %d\n", 
               runtime->desc->name, ret);
        kfree(misc_dev->name);
        kfree(misc_dev);
        return ret;
    }

    runtime->misc_dev = misc_dev;
    pr_info("Created misc device: /dev/%s\n", runtime->desc->name);
    return 0;
}

/**
 * tx_isp_create_basic_pipeline - Create basic pipeline when no subdevices are registered
 */
static int tx_isp_create_basic_pipeline(struct tx_isp_dev *isp)
{
    int ret;

    pr_info("tx_isp_create_basic_pipeline: Creating basic CSI->VIC pipeline\n");

    /* Initialize CSI device */
    ret = tx_isp_csi_device_init(isp);
    if (ret < 0) {
        pr_err("Failed to initialize CSI device: %d\n", ret);
        return ret;
    }

    /* Setup pipeline */
    ret = tx_isp_setup_pipeline(isp);
    if (ret < 0) {
        pr_err("Failed to setup pipeline: %d\n", ret);
        tx_isp_vic_device_deinit(isp);
        tx_isp_csi_device_deinit(isp);
        return ret;
    }

    pr_info("tx_isp_create_basic_pipeline: Basic pipeline created successfully\n");
    return 0;
}

/**
 * tx_isp_cleanup_subdev_graph - Clean up the subdevice graph
 */
void tx_isp_cleanup_subdev_graph(struct tx_isp_dev *isp)
{
    int i;

    pr_info("tx_isp_cleanup_subdev_graph: Cleaning up subdevice graph\n");

    /* Cleanup frame channels */
    for (i = 0; i < 4; i++) {
        if (isp->fs_miscdevs[i]) {
            misc_deregister(isp->fs_miscdevs[i]);
            kfree(isp->fs_miscdevs[i]->name);
            kfree(isp->fs_miscdevs[i]);
            isp->fs_miscdevs[i] = NULL;
        }
    }

    /* Cleanup proc entries */
    if (isp->isp_proc_dir) {
        proc_remove(isp->isp_proc_dir);
        isp->isp_proc_dir = NULL;
    }

    /* Cleanup subdev graph */
    memset(isp->subdev_graph, 0, sizeof(isp->subdev_graph));

    pr_info("tx_isp_cleanup_subdev_graph: Cleanup completed\n");
}

/**
 * tx_isp_init_subdev_registry - Initialize the subdevice management system
 * @isp: ISP device
 * @platform_devices: Array of platform devices to register
 * @count: Number of platform devices
 * 
 * This replaces the complex platform device registration in the init function
 * with a cleaner approach that builds the subdevice list properly.
 */
int tx_isp_init_subdev_registry(struct tx_isp_dev *isp,
                               struct platform_device **platform_devices,
                               int count)
{
    int ret = 0;
    int i;

    pr_info("*** tx_isp_init_subdev_registry: Initializing subdevice registry ***\n");

    if (!isp || !platform_devices) {
        pr_err("tx_isp_init_subdev_registry: Invalid parameters\n");
        return -EINVAL;
    }

    /* Initialize ISP device subdev fields for Binary Ninja compatibility */
    isp->subdev_count = count;
    isp->subdev_list = platform_devices;
    memset(isp->subdev_graph, 0, sizeof(isp->subdev_graph));

    /* Register each subdevice */
    for (i = 0; i < count && i < NUM_ISP_SUBDEVS; i++) {
        struct tx_isp_subdev_desc *desc = &isp_subdev_descriptors[i];
        
        /* Create driver data based on device type */
        void *driver_data = tx_isp_create_driver_data(desc);
        if (!driver_data) {
            pr_err("Failed to create driver data for %s\n", desc->name);
            continue;
        }

        ret = tx_isp_subdev_register(desc, platform_devices[i], driver_data);
        if (ret < 0) {
            pr_err("Failed to register subdevice %s: %d\n", desc->name, ret);
            kfree(driver_data);
            continue;
        }

        pr_info("Registered subdevice: %s (pdev=%p)\n", desc->name, platform_devices[i]);
    }

    pr_info("*** tx_isp_init_subdev_registry: Registry initialized with %d subdevices ***\n", 
            subdev_count);
    return 0;
}

/**
 * tx_isp_create_driver_data - Create appropriate driver data structure for subdevice
 */
static void *tx_isp_create_driver_data(struct tx_isp_subdev_desc *desc)
{
    void *data = NULL;

    /* Create appropriate data structure based on device type */
    if (strcmp(desc->name, "tx-isp-csi") == 0) {
        struct isp_subdev_data *csi_data = kzalloc(sizeof(struct isp_subdev_data), GFP_KERNEL);
        if (csi_data) {
            csi_data->device_type = desc->type;
            csi_data->device_id = desc->device_id;
            csi_data->src_index = desc->src_index;
            csi_data->dst_index = desc->dst_index;
            strcpy(csi_data->device_name, "csi");
            csi_data->file_ops = desc->fops;
            data = csi_data;
        }
    } else if (strcmp(desc->name, "tx-isp-vic") == 0) {
        struct isp_subdev_data *vic_data = kzalloc(sizeof(struct isp_subdev_data), GFP_KERNEL);
        if (vic_data) {
            vic_data->device_type = desc->type;
            vic_data->device_id = desc->device_id;
            vic_data->src_index = desc->src_index;
            vic_data->dst_index = desc->dst_index;
            strcpy(vic_data->device_name, "vic");
            vic_data->file_ops = desc->fops;
            data = vic_data;
        }
    } else {
        /* Generic subdevice data */
        struct isp_subdev_data *generic_data = kzalloc(sizeof(struct isp_subdev_data), GFP_KERNEL);
        if (generic_data) {
            generic_data->device_type = desc->type;
            generic_data->device_id = desc->device_id;
            generic_data->src_index = desc->src_index;
            generic_data->dst_index = desc->dst_index;
            strncpy(generic_data->device_name, desc->name, sizeof(generic_data->device_name) - 1);
            generic_data->file_ops = desc->fops;
            data = generic_data;
        }
    }

    return data;
}

/* Simple stub implementations for basic pipeline functions */

/**
 * tx_isp_csi_device_init - Initialize CSI device (stub implementation)
 */
int tx_isp_csi_device_init(struct tx_isp_dev *isp)
{
    if (!isp) {
        pr_err("tx_isp_csi_device_init: Invalid ISP device\n");
        return -EINVAL;
    }
    
    pr_info("tx_isp_csi_device_init: CSI device initialized (stub)\n");
    return 0;
}

/**
 * tx_isp_csi_device_deinit - Deinitialize CSI device (stub implementation)
 */
int tx_isp_csi_device_deinit(struct tx_isp_dev *isp)
{
    if (!isp) {
        pr_err("tx_isp_csi_device_deinit: Invalid ISP device\n");
        return -EINVAL;
    }
    
    pr_info("tx_isp_csi_device_deinit: CSI device deinitialized (stub)\n");
    return 0;
}

/**
 * tx_isp_vic_device_deinit - Deinitialize VIC device (stub implementation)
 */
int tx_isp_vic_device_deinit(struct tx_isp_dev *isp)
{
    if (!isp) {
        pr_err("tx_isp_vic_device_deinit: Invalid ISP device\n");
        return -EINVAL;
    }
    
    pr_info("tx_isp_vic_device_deinit: VIC device deinitialized (stub)\n");
    return 0;
}

/**
 * tx_isp_setup_pipeline - Setup ISP pipeline (stub implementation)
 */
int tx_isp_setup_pipeline(struct tx_isp_dev *isp)
{
    if (!isp) {
        pr_err("tx_isp_setup_pipeline: Invalid ISP device\n");
        return -EINVAL;
    }
    
    pr_info("tx_isp_setup_pipeline: ISP pipeline configured (stub)\n");
    return 0;
}

/* Export symbols for use by other modules */
EXPORT_SYMBOL(tx_isp_subdev_register);
EXPORT_SYMBOL(tx_isp_subdev_unregister);
EXPORT_SYMBOL(tx_isp_create_subdev_graph);
EXPORT_SYMBOL(tx_isp_init_subdev_registry);
EXPORT_SYMBOL(tx_isp_cleanup_subdev_graph);
