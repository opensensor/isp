#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/uaccess.h>
#include "include/tx_isp.h"
#include "include/tx_isp_core.h"
#include "include/tx_isp_debug.h"

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

/* tisp_ae_get_y_zone - OEM EXACT Implementation (0x54594)
 *
 * Copies 225 uint32_t zone luminance values from the internal buffer
 * to the caller's buffer.  Called per-frame by tisp_ae_mean_update
 * and by libimp via ioctl. */
int tisp_ae_get_y_zone(void *arg1)
{
    unsigned long flags;

    if (!arg1)
        return -EINVAL;

    /* Initialize AE zone data on first access */
    if (!ae_zone_data.initialized) {
        spin_lock_init(&ae_zone_data.lock);
        memset(ae_zone_data.zone_data, 0, sizeof(ae_zone_data.zone_data));
        ae_zone_data.zone_status = 1;
        ae_zone_data.initialized = true;
    }

    spin_lock_irqsave(&ae_zone_data.lock, flags);
    memcpy(arg1, ae_zone_data.zone_data, AE_ZONE_DATA_SIZE);
    spin_unlock_irqrestore(&ae_zone_data.lock, flags);

    return 0;
}
EXPORT_SYMBOL(tisp_ae_get_y_zone);

/* tisp_g_ae_zone - OEM EXACT: ioctl handler to copy AE zone data to userspace */
int tisp_g_ae_zone(struct tx_isp_dev *dev, struct isp_core_ctrl *ctrl)
{
    struct ae_zone_info zones;
    int ret;

    if (!dev || !ctrl || !ctrl->value)
        return -EINVAL;

    memset(&zones, 0, sizeof(zones));

    ret = tisp_ae_get_y_zone(&zones);
    if (ret)
        return ret;

    if (copy_to_user((void __user *)ctrl->value, &zones, sizeof(zones)))
        return -EFAULT;

    return 0;
}
EXPORT_SYMBOL(tisp_g_ae_zone);

/* Update AE zone data - called from ae0_interrupt_static every frame */
int tisp_ae_update_zone_data(uint32_t *new_zone_data, size_t data_size)
{
    unsigned long flags;

    if (!new_zone_data || data_size != sizeof(ae_zone_data.zone_data))
        return -EINVAL;

    /* Initialize if needed */
    if (!ae_zone_data.initialized) {
        spin_lock_init(&ae_zone_data.lock);
        ae_zone_data.zone_status = 1;
        ae_zone_data.initialized = true;
    }

    spin_lock_irqsave(&ae_zone_data.lock, flags);
    memcpy(ae_zone_data.zone_data, new_zone_data, data_size);
    ae_zone_data.zone_status = 1;
    spin_unlock_irqrestore(&ae_zone_data.lock, flags);

    return 0;
}
EXPORT_SYMBOL(tisp_ae_update_zone_data);
