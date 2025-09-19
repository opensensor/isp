#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/uaccess.h>
#include "../include/tx_isp.h"
#include "../include/tx_isp_core.h"
#include "../include/tx-isp-debug.h"

/* AE Zone Constants - Binary Ninja Reference */
#define AE_ZONE_DATA_SIZE 0x384  /* 900 bytes - matches Binary Ninja */
#define ISP_AE_ZONE_BASE 0x1000  /* AE zone register base */

/* AE Zone Data Structures - Binary Ninja Reference: 0x384 bytes (900 bytes exactly) */
struct ae_zone_info {
    uint32_t zone_metrics[225];              /* 15x15 AE zones = 225 zones (900 bytes) */
};

/* AE Zone Global Data - Binary Ninja Reference (0xd3b24 equivalent) */
static struct {
    uint32_t zone_data[225];                 /* Zone luminance data (exactly 900 bytes) */
    uint32_t zone_status;                    /* Processing status */
    spinlock_t lock;                         /* Protection spinlock */
    bool initialized;                        /* Initialization flag */
} ae_zone_data = {
    .initialized = false,
};

/* MCP Logging Helper */
static void mcp_log_info(const char *method_name, const char *description, void *data)
{
    pr_debug("MCP_LOG: %s: %s, data=%p\n", method_name, description, data);
}

/* tisp_ae_get_y_zone - Binary Ninja EXACT Implementation */
int tisp_ae_get_y_zone(void *arg1)
{
    unsigned long flags;
    
    mcp_log_info("tisp_ae_get_y_zone", "entry with argument", arg1);
    
    if (!arg1) {
        pr_err("tisp_ae_get_y_zone: Invalid argument\n");
        mcp_log_info("tisp_ae_get_y_zone", "error - null argument", NULL);
        return -EINVAL;
    }
    
    /* Initialize AE zone data if not already done */
    if (!ae_zone_data.initialized) {
        spin_lock_init(&ae_zone_data.lock);
        memset(ae_zone_data.zone_data, 0, sizeof(ae_zone_data.zone_data));
        ae_zone_data.zone_status = 1; /* Default active status */
        ae_zone_data.initialized = true;
        mcp_log_info("tisp_ae_get_y_zone", "initialized AE zone data", &ae_zone_data);
    }
    
    /* Binary Ninja: __private_spin_lock_irqsave(0, &var_18) */
    spin_lock_irqsave(&ae_zone_data.lock, flags);
    mcp_log_info("tisp_ae_get_y_zone", "acquired spinlock", &ae_zone_data.lock);
    
    /* Binary Ninja: memcpy(arg1, 0xd3b24, 0x384) - copy ONLY zone_data array */
    memcpy(arg1, ae_zone_data.zone_data, AE_ZONE_DATA_SIZE);
    mcp_log_info("tisp_ae_get_y_zone", "copied AE zone data", &ae_zone_data);
    
    /* Binary Ninja: private_spin_unlock_irqrestore(0, var_18) */
    spin_unlock_irqrestore(&ae_zone_data.lock, flags);
    mcp_log_info("tisp_ae_get_y_zone", "released spinlock", &ae_zone_data.lock);
    
    mcp_log_info("tisp_ae_get_y_zone", "exit success", NULL);
    /* Binary Ninja: return 0 */
    return 0;
}
EXPORT_SYMBOL(tisp_ae_get_y_zone);

/* tisp_g_ae_zone - Binary Ninja EXACT Implementation */
int tisp_g_ae_zone(struct tx_isp_dev *dev, struct isp_core_ctrl *ctrl)
{
    struct ae_zone_info zones;
    int ret;

    mcp_log_info("tisp_g_ae_zone", "entry with device and control", dev);

    if (!dev || !ctrl) {
        pr_err("tisp_g_ae_zone: Invalid device or control pointer\n");
        mcp_log_info("tisp_g_ae_zone", "error - invalid parameters", NULL);
        return -EINVAL;
    }

    if (!ctrl->value) {
        pr_err("tisp_g_ae_zone: No data pointer for AE zone\n");
        mcp_log_info("tisp_g_ae_zone", "error - no data pointer", NULL);
        return -EINVAL;
    }

    /* Clear structure first */
    memset(&zones, 0, sizeof(zones));
    mcp_log_info("tisp_g_ae_zone", "cleared zones structure", &zones);

    /* Get latest zone data using reference method */
    ret = tisp_ae_get_y_zone(&zones);
    if (ret) {
        pr_err("tisp_g_ae_zone: Failed to get AE zone data: %d\n", ret);
        mcp_log_info("tisp_g_ae_zone", "error getting zone data", &ret);
        return ret;
    }

    mcp_log_info("tisp_g_ae_zone", "got AE zone data successfully", &zones);

    /* Copy zone data to user-provided buffer - Fixed: correct size */
    if (copy_to_user((void __user *)ctrl->value, &zones, sizeof(zones))) {
        pr_err("tisp_g_ae_zone: Failed to copy data to user\n");
        mcp_log_info("tisp_g_ae_zone", "error copying to user", NULL);
        return -EFAULT;
    }

    /* Binary Ninja: just return 0, don't modify ctrl->value */
    mcp_log_info("tisp_g_ae_zone", "exit success", NULL);
    /* Binary Ninja reference: return 0 */
    return 0;
}
EXPORT_SYMBOL(tisp_g_ae_zone);

/* Update AE zone data - called by ISP hardware interrupt */
int tisp_ae_update_zone_data(uint32_t *new_zone_data, size_t data_size)
{
    unsigned long flags;
    
    mcp_log_info("tisp_ae_update_zone_data", "entry with new data", new_zone_data);
    
    if (!new_zone_data || data_size != sizeof(ae_zone_data.zone_data)) {
        pr_err("tisp_ae_update_zone_data: Invalid parameters\n");
        mcp_log_info("tisp_ae_update_zone_data", "error - invalid parameters", NULL);
        return -EINVAL;
    }
    
    /* Initialize if needed */
    if (!ae_zone_data.initialized) {
        spin_lock_init(&ae_zone_data.lock);
        ae_zone_data.zone_status = 1;
        ae_zone_data.initialized = true;
        mcp_log_info("tisp_ae_update_zone_data", "initialized on first update", &ae_zone_data);
    }
    
    spin_lock_irqsave(&ae_zone_data.lock, flags);
    memcpy(ae_zone_data.zone_data, new_zone_data, data_size);
    ae_zone_data.zone_status = 1; /* Mark as updated */
    spin_unlock_irqrestore(&ae_zone_data.lock, flags);
    
    mcp_log_info("tisp_ae_update_zone_data", "updated zone data successfully", &ae_zone_data);
    return 0;
}
EXPORT_SYMBOL(tisp_ae_update_zone_data);
