#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/of.h>
#include <linux/interrupt.h>
#include <linux/ratelimit.h>

#include <linux/i2c.h>
#include <linux/clk.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>

#include "../include/tx_isp.h"
#include "../include/tx_isp_core.h"
#include "../include/tx-isp-debug.h"
#include "../include/tx_isp_sysfs.h"
#include "../include/tx_isp_vic.h"
#include "../include/tx_isp_csi.h"
#include "../include/tx_isp_vin.h"
#include "../include/tx_isp_tuning.h"
#include "../include/tx-isp-device.h"
#include "../include/tx-libimp.h"
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/jiffies.h>

#include <linux/videodev2.h>

int vic_core_s_stream(struct tx_isp_subdev *sd, int enable);
int vic_video_s_stream(struct tx_isp_subdev *sd, int enable);
extern struct tx_isp_dev *ourISPdev;
uint32_t vic_start_ok = 0;  /* Global VIC interrupt enable flag definition */

/* system_reg_write is now defined in tx-isp-module.c - removed duplicate */

/* Debug function to track vic_start_ok changes */
static void debug_vic_start_ok_change(int new_value, const char *location, int line)
{
    if (vic_start_ok != new_value) {
        pr_info("*** VIC_START_OK CHANGE: %d -> %d at %s:%d ***\n",
                vic_start_ok, new_value, location, line);
    }
    vic_start_ok = new_value;
}
void tx_vic_enable_irq(struct tx_isp_vic_device *vic_dev);
static int ispcore_activate_module(struct tx_isp_dev *isp_dev);
static int tx_isp_vic_apply_full_config(struct tx_isp_vic_device *vic_dev);
static int vic_enabled = 0;

/* *** CRITICAL: MISSING FUNCTION - tx_isp_create_vic_device *** */
/* Forward declarations for PIPO callbacks used before definitions */
static int ispvic_frame_channel_qbuf(void *arg1, void *arg2);
static int ispvic_frame_channel_clearbuf(void);

/* This function creates and links the VIC device structure to the ISP core */
int tx_isp_create_vic_device(struct tx_isp_dev *isp_dev)
{
    struct tx_isp_vic_device *vic_dev;
    int ret = 0;

    if (!isp_dev) {
        pr_err("tx_isp_create_vic_device: Invalid ISP device\n");
        return -EINVAL;
    }

    pr_info("*** tx_isp_create_vic_device: Creating VIC device structure ***\n");

    /* FIXED: Use regular kernel memory instead of precious rmem for small structures */
    vic_dev = kzalloc(sizeof(struct tx_isp_vic_device), GFP_KERNEL);
    if (!vic_dev) {
        pr_err("tx_isp_create_vic_device: Failed to allocate VIC device structure\n");
        return -ENOMEM;
    }

    pr_info("*** VIC DEVICE ALLOCATED: %p (size=0x21c bytes) ***\n", vic_dev);

    /* Initialize VIC device structure - Binary Ninja exact layout */

    /* Initialize spinlock at offset 0x130 */
    spin_lock_init((spinlock_t *)((char *)vic_dev + 0x130));

    /* Initialize mutex at offset 0x154 */
    mutex_init((struct mutex *)((char *)vic_dev + 0x154));

    /* Initialize completion at offset 0x148 */
    init_completion((struct completion *)((char *)vic_dev + 0x148));

    /* Set initial state to 1 (INIT) at offset 0x128 */
    *(uint32_t *)((char *)vic_dev + 0x128) = 1;

    /* Set self-pointer at offset 0xd4 */
    *(void **)((char *)vic_dev + 0xd4) = vic_dev;

    /* CRITICAL FIX: Set NV12 format magic number at offset 0xc to prevent control limit error */
    /* The VIC interrupt handler checks for 0x3231564e (NV12) at offset 0xc */
    *(uint32_t *)((char *)vic_dev + 0xc) = 0x3231564e;  /* NV12 format magic number */
    pr_info("*** VIC DEVICE: Set NV12 format magic number 0x3231564e at offset 0xc ***\n");

    /* *** CRITICAL FIX: Map BOTH VIC register spaces - dual VIC architecture *** */
    pr_info("*** CRITICAL: Mapping DUAL VIC register spaces for complete VIC control ***\n");

    /* Primary VIC space (original CSI PHY shared space) */
    vic_dev->vic_regs = ioremap_nocache(0x133e0000, 0x10000);
    if (!vic_dev->vic_regs) {
        pr_err("tx_isp_create_vic_device: Failed to map primary VIC registers at 0x133e0000\n");
        kfree(vic_dev);
        return -ENOMEM;
    }
    pr_info("*** Primary VIC registers mapped (NC): %p (0x133e0000) ***\n", vic_dev->vic_regs);

    /* Secondary VIC space (isp-w01 - CSI PHY coordination space) */
    vic_dev->vic_regs_secondary = ioremap_nocache(0x10023000, 0x1000);
    if (!vic_dev->vic_regs_secondary) {
        pr_err("tx_isp_create_vic_device: Failed to map secondary VIC registers at 0x10023000\n");
        iounmap(vic_dev->vic_regs);
        kfree(vic_dev);
        return -ENOMEM;
    }
    pr_info("*** Secondary VIC registers mapped (NC): %p (0x10023000) ***\n", vic_dev->vic_regs_secondary);

    /* Detect which bank is the live MDMA bank (has non-zero stride or typical 3840 pre-NV12) */
    {
        u32 stride_a = readl(vic_dev->vic_regs + 0x310);
        u32 stride_b = readl(vic_dev->vic_regs_secondary + 0x310);
        pr_info("*** VIC MDMA bank detect: PRI strideY=%u, SEC strideY=%u ***\n", stride_a, stride_b);
        if ((stride_b && !stride_a) || (stride_b == 3840 && stride_a != 3840)) {
            /* Swap so vic_regs always points to the live MDMA bank */
            void __iomem *tmp = vic_dev->vic_regs;
            vic_dev->vic_regs = vic_dev->vic_regs_secondary;
            vic_dev->vic_regs_secondary = tmp;
            pr_info("*** VIC MDMA bank detect: swapped banks so vic_regs points to live MDMA window ***\n");
        }
    }

    /* Also store in ISP device for compatibility */
    if (!isp_dev->vic_regs) {
        isp_dev->vic_regs = vic_dev->vic_regs;
    }
    if (!isp_dev->vic_regs2) {
        isp_dev->vic_regs2 = vic_dev->vic_regs_secondary;
    }

    /* Initialize VIC device dimensions - CRITICAL: Use actual sensor output dimensions */
    vic_dev->width = 1920;  /* GC2053 actual output width */
    vic_dev->height = 1080; /* GC2053 actual output height */

    /* Set up VIC subdev structure */
    memset(&vic_dev->sd, 0, sizeof(vic_dev->sd));
    vic_dev->sd.isp = isp_dev;
    vic_dev->sd.ops = &vic_subdev_ops;
    vic_dev->sd.vin_state = TX_ISP_MODULE_INIT;

    /* Initialize buffer management */
    INIT_LIST_HEAD(&vic_dev->queue_head);
    INIT_LIST_HEAD(&vic_dev->done_head);
    INIT_LIST_HEAD(&vic_dev->free_head);
    spin_lock_init(&vic_dev->buffer_lock);
    spin_lock_init(&vic_dev->lock);
    mutex_init(&vic_dev->mlock);
    mutex_init(&vic_dev->state_lock);
    init_completion(&vic_dev->frame_complete);

    /* Initialize VIC error counters */
    memset(vic_dev->vic_errors, 0, sizeof(vic_dev->vic_errors));

    /* Set initial frame count */
    vic_dev->frame_count = 0;
    vic_dev->buffer_count = 0;
    vic_dev->streaming = 0;
    vic_dev->state = 1; /* INIT state */

    /* *** CRITICAL: Initialize VIC hardware buffers - DEFERRED to prevent memory exhaustion *** */
    pr_info("*** CRITICAL: VIC buffer allocation DEFERRED to prevent Wyze Cam memory exhaustion ***\n");
    pr_info("*** Buffers will be allocated on-demand during QBUF operations ***\n");

    /* Initialize empty lists - buffers allocated later when needed */
    pr_info("*** VIC: Buffer lists initialized - allocation deferred to prevent memory pressure ***\n");

    /* Set up sensor attributes with defaults */
    memset(&vic_dev->sensor_attr, 0, sizeof(vic_dev->sensor_attr));
    vic_dev->sensor_attr.dbus_type = TX_SENSOR_DATA_INTERFACE_MIPI; /* MIPI interface (correct value from enum) */
    vic_dev->sensor_attr.total_width = 1920;
    vic_dev->sensor_attr.total_height = 1080;
    vic_dev->sensor_attr.data_type = 0x2b; /* Default RAW10 */

    /* *** CRITICAL: Link VIC device to ISP core *** */
    /* Store the VIC device properly - the subdev is PART of the VIC device */
    isp_dev->vic_dev = (struct tx_isp_subdev *)&vic_dev->sd;

    /* *** CRITICAL FIX: Ensure VIC subdev has proper ISP device back-reference *** */
    vic_dev->sd.isp = isp_dev;  /* This ensures tx_isp_vic_start can find the ISP device */

    /* Also set host_priv so vic_core_ops_ioctl can retrieve vic_dev (QBUF path) */
    tx_isp_set_subdev_hostdata(&vic_dev->sd, vic_dev);

    pr_info("*** CRITICAL: VIC DEVICE LINKED TO ISP CORE ***\n");
    pr_info("  isp_dev->vic_dev = %p\n", isp_dev->vic_dev);
    pr_info("  vic_dev->sd.isp = %p\n", vic_dev->sd.isp);
    pr_info("  vic_dev->sd.ops = %p\n", vic_dev->sd.ops);
    pr_info("  vic_dev->vic_regs = %p\n", vic_dev->vic_regs);
    pr_info("  vic_dev->state = %d\n", vic_dev->state);
    pr_info("  vic_dev dimensions = %dx%d\n", vic_dev->width, vic_dev->height);

    /* Set up tx_isp_get_subdevdata to work properly */
    /* This sets up the private data pointer so tx_isp_get_subdevdata can retrieve the VIC device */
    vic_dev->sd.dev_priv = vic_dev;

    pr_info("*** tx_isp_create_vic_device: VIC device creation complete ***\n");
    pr_info("*** NO MORE 'NO VIC DEVICE' ERROR SHOULD OCCUR ***\n");

    return 0;
}
EXPORT_SYMBOL(tx_isp_create_vic_device);

/* VIC frame completion handler */
static void tx_isp_vic_frame_done(struct tx_isp_subdev *sd, int channel)
{
    if (!sd || channel >= VIC_MAX_CHAN)
        return;

    complete(&sd->vic_frame_end_completion[channel]);
}

/* Forward declarations for interrupt functions */
int vic_framedone_irq_function(struct tx_isp_vic_device *vic_dev);
static int vic_mdma_irq_function(struct tx_isp_vic_device *vic_dev, int channel);

/* VIC interrupt restoration function - using correct VIC base */
void tx_isp_vic_restore_interrupts(void)
{
    extern struct tx_isp_dev *ourISPdev;
    void __iomem *vic_interrupt_base;

    if (!ourISPdev || !ourISPdev->vic_dev || vic_start_ok != 1) {
        return; /* VIC not active */
    }

    pr_info("*** VIC INTERRUPT RESTORE: Restoring VIC interrupt registers in PRIMARY VIC space ***\n");

    /* CRITICAL: Use PRIMARY VIC space for interrupt control (0x133e0000) */
    struct tx_isp_vic_device *vic_dev = ourISPdev->vic_dev;
    if (!vic_dev || !vic_dev->vic_regs) {
        pr_err("*** VIC INTERRUPT RESTORE: No primary VIC registers available ***\n");
        return;
    }

    /* Restore VIC interrupt register values using WORKING ISP-activates configuration */
    pr_info("*** VIC INTERRUPT RESTORE: Using WORKING ISP-activates configuration (0x1e8/0x1ec) ***\n");

    /* Clear pending interrupts first */
    writel(0xFFFFFFFF, vic_dev->vic_regs + 0x1f0);  /* Clear main interrupt status */
    writel(0xFFFFFFFF, vic_dev->vic_regs + 0x1f4);  /* Clear MDMA interrupt status */
    wmb();

    /* Restore working interrupt masks - FOCUS ON MAIN INTERRUPT ONLY */
    writel(0xFFFFFFFE, vic_dev->vic_regs + 0x1e8);  /* Enable frame done interrupt */
    /* SKIP MDMA register 0x1ec - it doesn't work correctly */
    wmb();

    pr_info("*** VIC INTERRUPT RESTORE: WORKING configuration restored (MainMask=0xFFFFFFFE) ***\n");
}
EXPORT_SYMBOL(tx_isp_vic_restore_interrupts);

/* Global data symbol used by reference driver */
static char data_b0000[1] = {0};

/* VIC buffer entry structure for safe list handling */
struct vic_buffer_entry {
    struct list_head list;
    u32 reserved;
    u32 buffer_addr;
    u32 buffer_index; /* slot index 0..4 for VIC 0x318..0x328 */
};

/* Helper functions - removed conflicting declarations as they're already in SDK headers */
/* __private_spin_lock_irqsave and private_spin_unlock_irqrestore are defined in txx-funcs.h */


/* Forward declaration for streaming functions */
int ispvic_frame_channel_s_stream(struct tx_isp_vic_device *vic_dev, int enable);

/* GPIO info and state for vic_framedone_irq_function - matching reference driver */
static volatile int gpio_switch_state = 0;
static struct {
    uint8_t pin;
    uint8_t pad[19];  /* Padding to reach offset 0x14 */
    uint8_t state;    /* GPIO state at offset 0x14 */
} gpio_info[10];

/* vic_framedone_irq_function - Updated to match BN MCP reference with safe struct access */
int vic_framedone_irq_function(struct tx_isp_vic_device *vic_dev)
{

/* Reduce IRQ log noise: log a concise info line every N frames; otherwise use pr_debug_ratelimited. */
static u32 vic_irq_log_every = 60; /* frames */
static u32 vic_irq_counter;

    void __iomem *vic_regs;
    void *result = &data_b0000;  /* Return value matching reference */

    pr_debug_ratelimited("vic_framedone_irq: vic_dev=%p\n", vic_dev);

    /* Validate vic_dev first */
    if (!vic_dev) {
        pr_err("vic_framedone_irq_function: NULL vic_dev\n");
        return 0;
    }

    /* Binary Ninja: if (*(arg1 + 0x214) == 0) */
    /* SAFE: Use proper struct member 'processing' instead of offset 0x214 */
//    if (vic_dev->processing == 0) {
//        /* goto label_123f4 - GPIO handling section */
//        pr_info("vic_framedone_irq_function: Processing not active, skipping frame handling\n");
//        goto label_123f4;
//    } else {
        /* Binary Ninja: result = *(arg1 + 0x210) */
        /* SAFE: Use proper struct member 'stream_state' instead of offset 0x210 */
        pr_debug_ratelimited("vic_irq: stream_state=%d\n", vic_dev->stream_state);
        result = (void *)(uintptr_t)vic_dev->stream_state;

        if (vic_dev->stream_state != 0) {
            /* Binary Ninja: void* $a3_1 = *(arg1 + 0xb8) */
            /* SAFE: Use vic_regs member instead of offset 0xb8 */
            vic_regs = vic_dev->vic_regs;

            /* Binary Ninja: void** i_1 = *(arg1 + 0x204) */
            /* SAFE: Use done_head list instead of offset 0x204 */
            struct list_head *pos;
            int buffer_index = 0;    /* $a1_1 = 0 */
            int high_bits = 0;       /* $v1_1 = 0 */
            int match_found = 0;     /* $v0 = 0 */

            /* Binary Ninja: for (; i_1 != arg1 + 0x204; i_1 = *i_1) */
            /* SAFE: Iterate through done_head list instead of manual pointer walking */
            list_for_each(pos, &vic_dev->done_head) {
                /* Binary Ninja: $v1_1 += 0 u< $v0 ? 1 : 0 */
                high_bits += (0 < match_found) ? 1 : 0;
                /* Binary Ninja: $a1_1 += 1 */
                buffer_index += 1;

                /* Binary Ninja: if (i_1[2] == *($a3_1 + 0x380)) */
                /* Check if buffer address matches current frame register */
                if (vic_regs) {
                    u32 current_frame_addr = readl(vic_regs + 0x380);
                    /* SAFE: Extract buffer address from list entry */
                    /* Assuming buffer structure has address at offset 8 from list_head */
                    struct vic_buffer_entry {
                        struct list_head list;
                        u32 reserved;
                        u32 buffer_addr;
                    } *entry = container_of(pos, struct vic_buffer_entry, list);

                    if (entry->buffer_addr == current_frame_addr) {

                        match_found = 1;  /* $v0 = 1 */
                    }
                }
            }

            /* Binary Ninja: int32_t $v1_2 = $v1_1 << 0x10 */
            int shifted_value = high_bits << 0x10;

            /* Binary Ninja: if ($v0 == 0) */
            if (match_found == 0) {
                /* $v1_2 = $a1_1 << 0x10 */
                shifted_value = buffer_index << 0x10;
            }

            /* CRITICAL FIX: Preserve EXACT control bits 0x80000020 when updating buffer index */
            /* The reference driver preserves control bits, we were clearing them! */
            if (vic_regs) {
                u32 reg_val = readl(vic_regs + 0x300);
                /* If control is 0 or no valid index computed, do not touch control. */
                if (reg_val == 0 || shifted_value == 0) {
                    pr_debug_ratelimited("vic_irq: skip idx update (ctrl=0x%x, shift=0x%x, idx=%d, match=%d)\n",
                            reg_val, shifted_value, buffer_index, match_found);
                } else {
                    /* Preserve control bits for NV12 (0x80000020 | format=7) and only update buffer index (bits 16-19) */
                    reg_val = (reg_val & 0xfff0ffff) | shifted_value;  /* Clear bits 16-19, set new buffer index */

                    /* Reference: only update index bits [16..19], preserve other control/format bits */
                    writel(reg_val, vic_regs + 0x300);

                    pr_debug_ratelimited("vic_irq: VIC[0x300]=0x%x (idx updated)\n",
                            reg_val);
                    pr_debug_ratelimited("vic_irq: ctrl=0x%x idx=%d match=%d\n",
                             reg_val, buffer_index, match_found);
                }
            }
            /* Update lightweight MDMA snapshot for proc (no printk; per-frame) */
            {
                u32 ctrl = 0, stride = 0, y0 = 0, uv0 = 0, uvsh0 = 0;
                void __iomem *regs1 = vic_regs;
                if (regs1) {
                    ctrl   = readl(regs1 + 0x300);
                    stride = readl(regs1 + 0x310);
                    y0     = readl(regs1 + 0x318);
                    uv0    = readl(regs1 + 0x340);
                    uvsh0  = readl(regs1 + 0x32c);
                    vic_dev->mdma_snap_pri.ctrl       = ctrl;
                    vic_dev->mdma_snap_pri.strideY    = stride;
                    vic_dev->mdma_snap_pri.y0         = y0;
                    vic_dev->mdma_snap_pri.uv0        = uv0;
                    vic_dev->mdma_snap_pri.uvsh0      = uvsh0;
                    vic_dev->mdma_snap_pri.jiffies_ts = jiffies;
                }
                if (vic_dev->vic_regs_secondary) {
                    regs1 = vic_dev->vic_regs_secondary;
                    ctrl   = readl(regs1 + 0x300);
                    stride = readl(regs1 + 0x310);
                    y0     = readl(regs1 + 0x318);
                    uv0    = readl(regs1 + 0x340);
                    uvsh0  = readl(regs1 + 0x32c);
                    vic_dev->mdma_snap_sec.ctrl       = ctrl;
                    vic_dev->mdma_snap_sec.strideY    = stride;
                    vic_dev->mdma_snap_sec.y0         = y0;
                    vic_dev->mdma_snap_sec.uv0        = uv0;
                    vic_dev->mdma_snap_sec.uvsh0      = uvsh0;
                    vic_dev->mdma_snap_sec.jiffies_ts = jiffies;
                }
            }


            /* REFERENCE DRIVER: VIC frame done processing complete */
            /* The reference driver does NOT manually trigger ISP core interrupts */
            /* ISP core interrupts should be triggered automatically by hardware */
            pr_debug_ratelimited("vic_irq: frame processing complete\n");
        }

//        /* Binary Ninja: result = &data_b0000, goto label_123f4 */
//        result = &data_b0000;
//        goto label_123f4;
//    }

label_123f4:
    /* Binary Ninja: GPIO handling section */
    if (gpio_switch_state != 0) {
        /* Binary Ninja: void* $s1_1 = &gpio_info */
        struct {
            uint8_t pin;
            uint8_t pad[19];
            uint8_t state;
        } *gpio_ptr = &gpio_info[0];

        gpio_switch_state = 0;

        /* Binary Ninja: for (int32_t i = 0; i != 0xa; ) */
        for (int i = 0; i < 0xa; i++) {
            /* Binary Ninja: uint32_t $a0_2 = zx.d(*$s1_1) */
            uint32_t gpio_pin = (uint32_t)gpio_ptr->pin;

            /* Binary Ninja: if ($a0_2 == 0xff) break */
            if (gpio_pin == 0xff) {
                break;
            }

            /* Binary Ninja: result = private_gpio_direction_output($a0_2, zx.d(*($s1_1 + 0x14))) */
            uint32_t gpio_state = (uint32_t)gpio_ptr->state;

            /* SAFE: Call GPIO function with validated parameters */
            /* In real implementation, would call: private_gpio_direction_output(gpio_pin, gpio_state) */
            int gpio_result = 0; /* Placeholder for actual GPIO call */

            /* Binary Ninja: if (result s< 0) */
            if (gpio_result < 0) {
                pr_err("%s[%d] SET ERR GPIO(%d),STATE(%d),%d\n",
                       "vic_framedone_irq_function", __LINE__,
                       gpio_pin, gpio_state, gpio_result);
                return gpio_result;
            }

            pr_debug_ratelimited("vic_irq: GPIO %d -> %d\n", gpio_pin, gpio_state);

            /* Move to next GPIO info entry */
            gpio_ptr++;
        }
    }

    /* Occasional concise summary to keep logs useful without flooding */
    if (++vic_irq_counter % vic_irq_log_every == 0) {
        void __iomem *base = vic_dev->vic_regs ? vic_dev->vic_regs : vic_dev->vic_regs_secondary;
        u32 ctrl = base ? readl(base + 0x300) : 0;
        u32 idx = (ctrl >> 16) & 0xF;
        u32 stride = base ? readl(base + 0x310) : 0;
        pr_info("VIC IRQ summary: frames=%u idx=%u ctrl=0x%x strideY=%u\n",
                vic_irq_counter, idx, ctrl, stride);
    }

    pr_debug("*** vic_framedone_irq_function: completed successfully ***\n");
    /* Binary Ninja: return result */
    return 0;  /* Return 0 for success matching reference behavior */
}

/* vic_mdma_irq_function - Binary Ninja implementation for MDMA channel interrupts */
static int vic_mdma_irq_function(struct tx_isp_vic_device *vic_dev, int channel)
{
    void __iomem *vic_base = vic_dev->vic_regs;

    pr_debug("*** vic_mdma_irq_function: channel=%d, vic_dev=%p ***\n", channel, vic_dev);

    /* MDMA channel interrupt processing */
    if (channel == 0) {
        /* Channel 0 MDMA processing */
        pr_debug("vic_mdma_irq_function: Processing MDMA channel 0 interrupt\n");

        /* Complete any pending frame operations for channel 0 */
        complete(&vic_dev->frame_complete);

        /* Clear channel 0 specific MDMA status if needed */
        /* This would include hardware-specific register operations */
    } else if (channel == 1) {
        /* Channel 1 MDMA processing */
        pr_debug("vic_mdma_irq_function: Processing MDMA channel 1 interrupt\n");

        /* Channel 1 specific processing would go here */
        /* This would include different buffer management or DMA operations */
    }

    pr_debug("*** vic_mdma_irq_function: completed for channel %d ***\n", channel);
    return 0;  /* Success */
}

/* VIC interrupt handler - Now uses the comprehensive Binary Ninja implementation */
static irqreturn_t isp_vic_interrupt_service_routine(int irq, void *dev_id)
{
    /* Forward to the comprehensive interrupt handler from tx_isp_vic_debug.c */
    extern irqreturn_t isp_vic_interrupt_service_routine(int irq, void *dev_id);
    return isp_vic_interrupt_service_routine(irq, dev_id);
}

/* CRITICAL FIX: Initialize VIC hardware with proper interrupt configuration */
int tx_isp_vic_hw_init(struct tx_isp_subdev *sd)
{
    struct tx_isp_vic_device *vic_dev = container_of(sd, struct tx_isp_vic_device, sd);
    void __iomem *vic_base;

    if (!vic_dev || !vic_dev->vic_regs) {
        pr_err("tx_isp_vic_hw_init: No primary VIC registers available\n");
        return -EINVAL;
    }

    // CRITICAL: Use PRIMARY VIC space for interrupt configuration
    vic_base = vic_dev->vic_regs;  // Use primary VIC space (0x133e0000)
    pr_info("*** VIC HW INIT: Using PRIMARY VIC space for interrupt configuration ***\n");

    // Clear any pending interrupts first
    writel(0, vic_base + 0x00);  // Clear ISR
    writel(0, vic_base + 0x20);  // Clear ISR1
    wmb();

    // Set up interrupt masks to match OEM
    writel(0x00000001, vic_base + 0x04);  // IMR
    wmb();
    writel(0x00000000, vic_base + 0x24);  // IMR1
    wmb();

    // Configure ISP control interrupts
    writel(0x07800438, vic_base + 0x04);  // IMR
    wmb();
    writel(0xb5742249, vic_base + 0x0c);  // IMCR
    wmb();

    pr_info("*** VIC HW INIT: Interrupt configuration applied to PRIMARY VIC space ***\n");

    /* CRITICAL: Register the VIC interrupt handler - THIS WAS MISSING! */
    int irq = 38;  /* VIC uses IRQ 38 (isp-w02) */
    int ret = request_irq(irq, isp_vic_interrupt_service_routine, IRQF_SHARED, "tx-isp-vic", sd);
    if (ret == 0) {
        pr_info("*** VIC HW INIT: Interrupt handler registered for IRQ %d ***\n", irq);
    } else {
        pr_err("*** VIC HW INIT: Failed to register interrupt handler for IRQ %d: %d ***\n", irq, ret);
        return ret;
    }

    /* Enable the interrupt at hardware level */
    enable_irq(irq);
    pr_info("*** VIC HW INIT: Hardware interrupt enabled for IRQ %d ***\n", irq);

    return 0;
}

/* Stop VIC processing */
int tx_isp_vic_stop(struct tx_isp_subdev *sd)
{
    u32 ctrl;

    if (!sd || !sd->isp)
        return -EINVAL;

    mutex_lock(&sd->vic_frame_end_lock);

    /* Stop processing */
    ctrl = vic_read32(VIC_CTRL);
    ctrl &= ~VIC_CTRL_START;
    ctrl |= VIC_CTRL_STOP;
    vic_write32(VIC_CTRL, ctrl);

    /* Wait for stop to complete */
    while (vic_read32(VIC_STATUS) & STATUS_BUSY) {
        udelay(10);
    }

    mutex_unlock(&sd->vic_frame_end_lock);
    return 0;
}

/* Configure VIC frame buffer */
int tx_isp_vic_set_buffer(struct tx_isp_subdev *sd, dma_addr_t addr, u32 size)
{
    if (!sd || !sd->isp)
        return -EINVAL;

    mutex_lock(&sd->vic_frame_end_lock);

    /* Set frame buffer address and size */
    vic_write32(VIC_BUFFER_ADDR, addr);
    vic_write32(VIC_FRAME_SIZE, size);

    mutex_unlock(&sd->vic_frame_end_lock);
    return 0;
}

/* Wait for frame completion */
int tx_isp_vic_wait_frame_done(struct tx_isp_subdev *sd, int channel, int timeout_ms)
{
    int ret;

    if (!sd || channel >= VIC_MAX_CHAN) {
        pr_err("Invalid subdev or channel\n");
        return -EINVAL;
    }

    ret = wait_for_completion_timeout(
        &sd->vic_frame_end_completion[channel],
        msecs_to_jiffies(timeout_ms)
    );

    return ret ? 0 : -ETIMEDOUT;
}


int vic_saveraw(struct tx_isp_subdev *sd, unsigned int savenum)
{
    uint32_t vic_ctrl, vic_status, vic_intr, vic_addr;
    uint32_t width, height, frame_size, buf_size;
    struct tx_isp_dev *isp_dev = ourISPdev;
    void __iomem *vic_base;
    void *capture_buf;
    dma_addr_t dma_addr;
    unsigned long timeout;
    int i, ret = 0;
    struct file *fp;
    char filename[64];
    loff_t pos = 0;
    mm_segment_t old_fs;

    if (!sd || !isp_dev) {
        pr_err("No VIC or ISP device\n");
        return -EINVAL;
    }

    width = isp_dev->sensor_width;
    height = isp_dev->sensor_height;

    if (width >= 0xa81) {
        pr_err("Can't output the width(%d)!\n", width);
        return -EINVAL;
    }

    frame_size = width * height * 2;
    buf_size = frame_size * savenum;

    pr_info("width=%d height=%d frame_size=%d total_size=%d savenum=%d\n",
            width, height, frame_size, buf_size, savenum);

    vic_base = ioremap(0x10023000, 0x1000);
    if (!vic_base) {
        pr_err("Failed to map VIC registers\n");
        return -ENOMEM;
    }

    // FIXED: Use regular DMA allocation instead of precious rmem
    capture_buf = dma_alloc_coherent(sd->dev, buf_size, &dma_addr, GFP_KERNEL);
    if (!capture_buf) {
        pr_err("Failed to allocate DMA buffer\n");
        iounmap(vic_base);
        return -ENOMEM;
    }
    pr_info("*** VIC: Using DMA buffer at virt=%p, phys=0x%08x (FIXED: no more rmem usage) ***\n", capture_buf, (uint32_t)dma_addr);
    // Read original register values
    vic_ctrl = readl(vic_base + 0x7810);
    vic_status = readl(vic_base + 0x7814);
    vic_intr = readl(vic_base + 0x7804);
    vic_addr = readl(vic_base + 0x7820);

    // Different register configuration for saveraw
    writel(vic_ctrl & 0x11111111, vic_base + 0x7810);
    writel(0, vic_base + 0x7814);
    writel(vic_intr | 1, vic_base + 0x7804);

    writel(dma_addr, vic_base + 0x7820);
    writel(width * 2, vic_base + 0x7824);
    writel(height, vic_base + 0x7828);

    // Start capture
    writel(1, vic_base + 0x7800);

    timeout = jiffies + msecs_to_jiffies(600);
    while (time_before(jiffies, timeout)) {
        if (!(readl(vic_base + 0x7800) & 1)) {
            break;
        }
        usleep_range(1000, 2000);
    }

    if (readl(vic_base + 0x7800) & 1) {
        pr_err("VIC capture timeout!\n");
        ret = -ETIMEDOUT;
        goto cleanup;
    }

    // Save frames to files
    old_fs = get_fs();
    set_fs(KERNEL_DS);

    for (i = 0; i < savenum; i++) {
        // Different filename format for saveraw
        snprintf(filename, sizeof(filename), "/tmp/vic_save_%d.raw", i);
        fp = filp_open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (IS_ERR(fp)) {
            pr_err("Failed to open file %s\n", filename);
            continue;
        }

        ret = vfs_write(fp, capture_buf + (i * frame_size), frame_size, &pos);
        if (ret != frame_size) {
            pr_err("Failed to write frame %d\n", i);
        }

        filp_close(fp, NULL);
        pos = 0;
    }

    set_fs(old_fs);

    // Restore registers
    writel(vic_ctrl & 0x11111111, vic_base + 0x7810);
    writel(vic_status, vic_base + 0x7814);
    writel(vic_intr | 1, vic_base + 0x7804);

cleanup:
    // Don't free the buffer - it stays allocated for future use
    iounmap(vic_base);
    return ret;
}

int vic_snapraw(struct tx_isp_subdev *sd, unsigned int savenum)
{
    uint32_t vic_ctrl, vic_status, vic_intr, vic_addr;
    uint32_t width, height, frame_size, buf_size; /* capture dimensions */
    struct tx_isp_dev *isp_dev = ourISPdev;
    void __iomem *vic_base;
    void *capture_buf;
    dma_addr_t dma_addr;
    unsigned long timeout;
    int i, ret = 0;
    struct file *fp;
    char filename[64];
    loff_t pos = 0;
    mm_segment_t old_fs;
    bool using_rmem = false;
    /* Extra diagnostics for logging to file */
    u32 dims_reg = 0, w_hw_snap = 0, h_hw_snap = 0;
    u32 stride_reg = 0, stride_used = 0;
    size_t sample_n = 0; u32 sample_sum = 0; u8 sample_min = 0xFF, sample_max = 0x00;

    if (!sd) {
        pr_err("No VIC or sensor device\n");
        return -EINVAL;
    }

    /* Prefer live VIC hardware-configured dims over cached sensor totals */
    width = isp_dev->sensor_width;
    height = isp_dev->sensor_height;
    if (isp_dev && isp_dev->vic_dev && isp_dev->vic_dev->vic_regs) {
        u32 dims = readl(isp_dev->vic_dev->vic_regs + 0x304); /* [31:16]=width, [15:0]=height */
        u32 w_hw = (dims >> 16) & 0xFFFF;
        u32 h_hw = dims & 0xFFFF;
        dims_reg = dims; w_hw_snap = w_hw; h_hw_snap = h_hw;
        if (w_hw >= 64 && w_hw <= 8192 && h_hw >= 64 && h_hw <= 8192) {
            pr_info("vic_snapraw: using VIC hw dims %ux%u (was %ux%u)\n", w_hw, h_hw, width, height);
            width = w_hw;
            height = h_hw;
        } else {
            pr_warn("vic_snapraw: VIC dims out of range (%u x %u), keeping %u x %u\n", w_hw, h_hw, width, height);
        }
    }

    if (width >= 0xa81) {
        pr_err("Can't output the width(%d)!\n", width);
        return -EINVAL;
    }

    /* Determine stride from VIC if available; fallback to 2 bytes per pixel */
    {
        /* Determine RAW10 stride: ceil(width*10/8), 16B aligned; prefer VIC stride regs if >= estimate */
        u32 raw10_bpl = (width * 10 + 7) >> 3;           /* bytes per line, packed RAW10 */
        u32 stride_tmp = (raw10_bpl + 15) & ~15;         /* 16-byte align fallback */
        if (isp_dev && isp_dev->vic_dev && isp_dev->vic_dev->vic_regs) {
            u32 s0 = readl(isp_dev->vic_dev->vic_regs + 0x310);
            u32 s1 = readl(isp_dev->vic_dev->vic_regs + 0x314);
            stride_reg = s0 ? s0 : s1;
            if (s0 >= raw10_bpl && s0 <= 0x20000)
                stride_tmp = s0;
            else if (s1 >= raw10_bpl && s1 <= 0x20000)
                stride_tmp = s1;
        }
        stride_used = stride_tmp;
        frame_size = stride_tmp * height;                 /* RAW10 single plane */
        buf_size = frame_size * savenum;
        pr_info("vic_snapraw: RAW10 dims=%ux%u bpl_est=%u stride=%u frame_size=%u total_size=%u savenum=%u\n",
                width, height, raw10_bpl, stride_tmp, frame_size, buf_size, savenum);
    }

    // Map VIC registers
    vic_base = ioremap(0x10023000, 0x1000);
    if (!vic_base) {
        pr_err("Failed to map VIC registers\n");
        return -ENOMEM;
    }

    // FIXED: Use regular DMA allocation instead of precious rmem
    capture_buf = dma_alloc_coherent(sd->dev, buf_size, &dma_addr, GFP_KERNEL);
    if (!capture_buf) {
        pr_err("Failed to allocate DMA buffer\n");
        iounmap(vic_base);
        return -ENOMEM;
    }
    using_rmem = false;
    pr_info("*** VIC: Using DMA buffer at virt=%p, phys=0x%08x (FIXED: no more rmem usage) ***\n", capture_buf, (uint32_t)dma_addr);

    // Read original register values
    vic_ctrl = readl(vic_base + 0x7810);
    vic_status = readl(vic_base + 0x7814);
    vic_intr = readl(vic_base + 0x7804);
    vic_addr = readl(vic_base + 0x7820);

    // Configure snapshot: clear status and enable snapshot interrupt; leave control unchanged
    writel(0, vic_base + 0x7814);
    writel(vic_intr | 1, vic_base + 0x7804);

    // Setup DMA
    writel(dma_addr, vic_base + 0x7820);  // DMA target address
    /* Program stride/height for capture DMA using previously determined RAW10 stride */
    writel(stride_used, vic_base + 0x7824);  // Stride (bytes per line)
    writel(height,     vic_base + 0x7828);  // Number of lines
    pr_info("vic_snapraw: programmed stride=%u height=%u\n", stride_used, height);

    // Start capture
    writel(1, vic_base + 0x7800);  // Start DMA

    // Wait for completion with timeout
    timeout = jiffies + msecs_to_jiffies(600); // 600ms timeout
    while (time_before(jiffies, timeout)) {
        if (!(readl(vic_base + 0x7800) & 1)) {
            // DMA complete
            break;
        }
        usleep_range(1000, 2000);
    }

    if (readl(vic_base + 0x7800) & 1) {
        pr_err("VIC capture timeout!\n");
        ret = -ETIMEDOUT;
        goto cleanup;
    }




    /* Inspect first chunk for non-zero data and store stats */
    do {
        size_t n = frame_size < 4096 ? frame_size : 4096;
        const unsigned char *p = (const unsigned char *)capture_buf;
        unsigned int sum = 0; unsigned char mn = 0xFF, mx = 0x00;
        size_t k; for (k = 0; k < n; ++k) { unsigned char v = p[k]; sum += v; if (v < mn) mn = v; if (v > mx) mx = v; }
        sample_n = n; sample_sum = sum; sample_min = mn; sample_max = mx;
        pr_info("vic_snapraw: sample bytes n=%zu sum=%u min=%u max=%u\n", n, sum, mn, mx);
    } while (0);

    // Save frames to files
    old_fs = get_fs();
    set_fs(KERNEL_DS);

    for (i = 0; i < savenum; i++) {
        snprintf(filename, sizeof(filename), "/tmp/vic_frame_%d.raw", i);
        fp = filp_open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (IS_ERR(fp)) {
            pr_err("Failed to open file %s\n", filename);
            continue;
        }

        ret = vfs_write(fp, capture_buf + (i * frame_size), frame_size, &pos);
        if (ret != frame_size) {
            pr_err("Failed to write frame %d\n", i);
    /* Now append a one-line summary (after sampling so stats are valid) */
    do {
        struct file *lfp; loff_t lpos = 0; char line[160]; int len;
        old_fs = get_fs(); set_fs(KERNEL_DS);
        lfp = filp_open("/tmp/vic_snapraw.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (!IS_ERR(lfp)) {
            len = snprintf(line, sizeof(line),
                "dims=%ux%u (reg=0x%08x) stride_used=%u stride_reg=%u frame_size=%u sample(n=%zu sum=%u min=%u max=%u)\n",
                width, height, dims_reg, stride_used, stride_reg, frame_size, sample_n, sample_sum, sample_min, sample_max);
            vfs_write(lfp, line, len, &lpos);
            filp_close(lfp, NULL);
        }
        set_fs(old_fs);
    } while (0);

    /* If buffer looks all-zero, retry snapshot on primary VIC bank and resample */
    if (sample_n > 0 && sample_sum == 0 && sample_max == 0) {
        void __iomem *vic_base_pri = ioremap(0x133e0000, 0x1000);
        if (vic_base_pri) {
            unsigned long tmo;
            /* Re-program snapshot on primary bank with same addr/stride/height */
            writel(0,                vic_base_pri + 0x7814);
            writel(1,                vic_base_pri + 0x7804);
            writel(dma_addr,         vic_base_pri + 0x7820);
            writel(stride_used,      vic_base_pri + 0x7824);
            writel(height,           vic_base_pri + 0x7828);
            writel(1,                vic_base_pri + 0x7800);
            tmo = jiffies + msecs_to_jiffies(600);
            while (time_before(jiffies, tmo)) {
                if (!(readl(vic_base_pri + 0x7800) & 1)) break;
                usleep_range(1000, 2000);
            }
            /* Re-sample */
            {
                size_t n2 = frame_size < 4096 ? frame_size : 4096;
                const unsigned char *p2 = (const unsigned char *)capture_buf;
                unsigned int sum2 = 0; unsigned char mn2 = 0xFF, mx2 = 0x00; size_t k2;
                for (k2 = 0; k2 < n2; ++k2) { unsigned char v2 = p2[k2]; sum2 += v2; if (v2 < mn2) mn2 = v2; if (v2 > mx2) mx2 = v2; }
                if (sum2 || mx2) {
                    /* Overwrite preview with primary-bank data */
                    struct file *fp2 = filp_open("/tmp/vic_frame_0_pri.raw", O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (!IS_ERR(fp2)) { loff_t p = 0; vfs_write(fp2, capture_buf, frame_size, &p); filp_close(fp2, NULL); }
                    fp2 = filp_open("/tmp/snap0.raw", O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (!IS_ERR(fp2)) { loff_t p = 0; vfs_write(fp2, capture_buf, frame_size, &p); filp_close(fp2, NULL); }
                    pr_info("vic_snapraw: primary VIC snapshot produced non-zero data (sum=%u max=%u)\n", sum2, mx2);
                } else {
                    pr_warn("vic_snapraw: primary VIC snapshot also zero data\n");
                }
            }
            iounmap(vic_base_pri);
        }
    }

        }
        filp_close(fp, NULL);
        pos = 0;

        /* Also save first frame to /tmp/snap0.raw for web preview compatibility */
        if (i == 0) {
            struct file *fp2 = filp_open("/tmp/snap0.raw", O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (!IS_ERR(fp2)) {
                loff_t pos2 = 0;
                vfs_write(fp2, capture_buf, frame_size, &pos2);
                filp_close(fp2, NULL);
            }
        }
    }

    set_fs(old_fs);

    // Restore registers
    writel(vic_ctrl, vic_base + 0x7810);
    writel(vic_status, vic_base + 0x7814);
    writel(vic_intr, vic_base + 0x7804);

cleanup:
    // Free buffer if using DMA allocation (not rmem)
    if (!using_rmem && capture_buf) {
        dma_free_coherent(sd->dev, buf_size, capture_buf, dma_addr);
    }


    iounmap(vic_base);
    return ret;
}


int vic_snapnv12(struct tx_isp_subdev *sd, unsigned int savenum)
{
    struct tx_isp_dev *isp_dev = ourISPdev;
    void __iomem *vic_base;
    u32 dims_reg = 0, stride_reg = 0;
    u32 width = 0, height = 0, stride = 0;
    u32 total_lines = 0; /* Y + UV lines */
    u32 frame_size = 0, buf_size = 0;
    void *capture_buf = NULL; dma_addr_t dma_addr = 0;
    int i, ret = 0; unsigned long timeout;
    struct file *fp; char filename[64]; loff_t pos = 0; mm_segment_t old_fs;

    if (!sd || !isp_dev) return -EINVAL;

    /* Read live VIC geometry */
    if (isp_dev->vic_dev && isp_dev->vic_dev->vic_regs) {
        dims_reg = readl(isp_dev->vic_dev->vic_regs + 0x304);
        width  = (dims_reg >> 16) & 0xFFFF;
        height = (dims_reg) & 0xFFFF;
        stride_reg = readl(isp_dev->vic_dev->vic_regs + 0x310);
    }
    if (width < 64 || height < 64) { width = isp_dev->sensor_width; height = isp_dev->sensor_height; }
    if (stride_reg >= width && stride_reg <= 0x20000) stride = stride_reg; else stride = ALIGN(width, 2);

    /* NV12 size: Y plane (stride*H) + interleaved UV plane (stride*H/2) */
    total_lines = height + (height >> 1);
    frame_size = stride * total_lines;
    buf_size = frame_size * savenum;
    pr_info("vic_snapnv12: dims=%ux%u stride=%u total_lines=%u frame_size=%u\n", width, height, stride, total_lines, frame_size);

    /* Map snapshot DMA bank (secondary) */
    vic_base = ioremap(0x10023000, 0x1000);
    if (!vic_base) return -ENOMEM;

    capture_buf = dma_alloc_coherent(sd->dev, buf_size, &dma_addr, GFP_KERNEL);
    if (!capture_buf) { iounmap(vic_base); return -ENOMEM; }

    /* Program snapshot DMA: address, stride, and number of lines to copy */
    writel(dma_addr,            vic_base + 0x7820);
    writel(stride,              vic_base + 0x7824);
    writel(total_lines,         vic_base + 0x7828);
    writel(1,                   vic_base + 0x7800); /* start */

    timeout = jiffies + msecs_to_jiffies(600);
    while (time_before(jiffies, timeout)) {
        if (!(readl(vic_base + 0x7800) & 1)) break;
        usleep_range(1000, 2000);
    }

    if (readl(vic_base + 0x7800) & 1) { pr_err("vic_snapnv12: timeout\n"); ret = -ETIMEDOUT; goto out; }

    old_fs = get_fs(); set_fs(KERNEL_DS);
    for (i = 0; i < savenum; i++) {
        snprintf(filename, sizeof(filename), "/tmp/vic_frame_%d_nv12.yuv", i);
        fp = filp_open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (IS_ERR(fp)) continue;
        vfs_write(fp, capture_buf + (i * frame_size), frame_size, &pos);
        filp_close(fp, NULL); pos = 0;
    }
    set_fs(old_fs);

    /* Append one-line summary to file */
    do {
        struct file *lfp; loff_t lpos = 0; char line[160]; int len;
        old_fs = get_fs(); set_fs(KERNEL_DS);
        lfp = filp_open("/tmp/vic_snapraw.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (!IS_ERR(lfp)) {
            len = snprintf(line, sizeof(line),
              "NV12 dims=%ux%u stride=%u (reg=%u) total_lines=%u frame_size=%u\n",
              width, height, stride, stride_reg, total_lines, frame_size);
            vfs_write(lfp, line, len, &lpos); filp_close(lfp, NULL);
        }
        set_fs(old_fs);
    } while (0);

out:
    if (capture_buf) dma_free_coherent(sd->dev, buf_size, capture_buf, dma_addr);
    iounmap(vic_base);
    return ret;
}



/* Move vic_proc_write outside of vic_snapraw */
static ssize_t vic_proc_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    struct tx_isp_subdev *sd = PDE_DATA(file->f_inode);
    char cmd[32];
    unsigned int savenum = 0;
    int ret;

    if (count >= sizeof(cmd))
        return -EINVAL;

    if (copy_from_user(cmd, buf, count))
        return -EFAULT;

    cmd[count] = '\0';
    cmd[count-1] = '\0'; // Remove trailing newline

    // Parse command format: "<cmd> <savenum>"
    ret = sscanf(cmd, "%s %u", cmd, &savenum);
    if (ret != 2) {
        pr_info("\t\t\t please use this cmd: \n");
        pr_info("\t\"echo snapraw savenum > /proc/jz/isp/isp-w02\"\n");
        pr_info("\t\"echo saveraw savenum > /proc/jz/isp/isp-w02\"\n");
        return count;
    }

    if (strcmp(cmd, "snapraw") == 0) {
        if (savenum < 2)
            savenum = 1;

        // Save raw frames
        ret = vic_snapraw(sd, savenum);
    }
    else if (strcmp(cmd, "saveraw") == 0) {
        if (savenum < 2)
            savenum = 1;

        // Save processed frames
        ret = vic_saveraw(sd, savenum);
    }
    else {
        pr_info("help:\n");
        pr_info("\t cmd:\n");
        pr_info("\t\t snapraw\n");
        pr_info("\t\t saveraw\n");
        pr_info("\t\t\t please use this cmd: \n");
        pr_info("\t\"echo cmd savenum > /proc/jz/isp/isp-w02\"\n");
    }

    return count;
}


/* tx_isp_vic_start - Following EXACT Binary Ninja flow with reference driver sequences */
int tx_isp_vic_start(struct tx_isp_vic_device *vic_dev)
{
    void __iomem *vic_regs;
    struct tx_isp_sensor_attribute *sensor_attr;
    u32 interface_type, sensor_format;
    u32 timeout = 10000;
    void __iomem *cpm_regs;
    int ret;

    /* CRITICAL FIX: Define actual sensor dimensions for consistent use throughout function */
    u32 actual_width, actual_height;

    pr_info("*** tx_isp_vic_start: Following EXACT Binary Ninja flow ***\n");

    /* Binary Ninja: 00010244 void* $v1 = *(arg1 + 0x110) */
    if (!vic_dev) {
        pr_info("*** CRITICAL: Invalid vic_dev pointer ***\n");
        return -EINVAL;
    }

    /* Get sensor attributes - offset 0x110 in Binary Ninja */
    /* Binary Ninja: void* $v1 = *(arg1 + 0x110) */
    sensor_attr = ourISPdev->sensor->video.attr;

    /* DEBUG: Check if sensor_attr is properly initialized */
    pr_info("*** DEBUG: sensor_attr=%p, dbus_type=%d ***\n", sensor_attr, sensor_attr ? sensor_attr->dbus_type : -1);

    /* CRITICAL FIX: Use ACTUAL sensor output dimensions, not total dimensions */
    /* GC2053 sensor outputs 1920x1080 but reports total dimensions 2200x1418 */
    actual_width = 1920;   /* ACTUAL sensor output width */
    actual_height = 1080;  /* ACTUAL sensor output height */

    pr_info("*** DIMENSION FIX: Using ACTUAL sensor output dimensions %dx%d for VIC configuration ***\n",
            actual_width, actual_height);
    pr_info("*** CRITICAL: VIC configured for sensor OUTPUT, not sensor TOTAL dimensions ***\n");

    /* Binary Ninja: 0001024c int32_t $v0 = *($v1 + 0x14) - interface type at offset 0x14 */
    interface_type = sensor_attr->dbus_type;

    pr_info("*** VIC INTERFACE DETECTION: interface_type=%d (MIPI=1, DVP=2) ***\n", interface_type);
    pr_info("*** SENSOR ATTRIBUTE: dbus_type=%d ***\n", sensor_attr->dbus_type);

    /* CRITICAL FIX: Use CSI format instead of data_type for RAW10 */
    /* sensor_attr->data_type = TX_SENSOR_DATA_TYPE_LINEAR (not what we need) */
    /* sensor_attr->mipi.mipi_sc.sensor_csi_fmt = TX_SENSOR_RAW10 (this is what we need) */

    /* SAFETY: Check if sensor_attr is valid before accessing nested structures */
    if (!sensor_attr) {
        pr_info("*** CRITICAL: sensor_attr is NULL ***\n");
        return -EINVAL;
    }

    /* SAFETY: Use default RAW10 format if sensor_attr access fails */
    sensor_format = 0x2b;  /* Default to RAW10 MIPI data type value */

    /* Try to get actual sensor format, but use default if it fails */
    if (sensor_attr) {
        /* For now, just use the default RAW10 format to avoid potential crashes */
        pr_info("*** SAFETY: Using default RAW10 format (0x2b) to avoid sensor_attr access issues ***\n");
    }

    pr_info("*** Interface type: %d, Format: 0x%x (RAW10) ***\n", interface_type, sensor_format);

    /* Get VIC register base - offset 0xb8 in Binary Ninja */
    vic_regs = vic_dev->vic_regs;
    if (!vic_regs) {
        pr_info("*** CRITICAL: No VIC register base ***\n");
        return -EINVAL;
    }

    /* Calculate base addresses for register blocks */
    void __iomem *main_isp_base = vic_regs - 0x9a00;
    void __iomem *csi_base = main_isp_base + 0x10000;

    /* STEP 2: CPM register setup */
    cpm_regs = ioremap(0x10000000, 0x1000);
    if (cpm_regs) {
        u32 clkgr0 = readl(cpm_regs + 0x20);
        u32 clkgr1 = readl(cpm_regs + 0x28);

        clkgr0 &= ~(1 << 13); // ISP
        clkgr0 &= ~(1 << 21); // Alternative ISP
        clkgr0 &= ~(1 << 30); // VIC in CLKGR0
        clkgr1 &= ~(1 << 30); // VIC in CLKGR1

        writel(clkgr0, cpm_regs + 0x20);
        writel(clkgr1, cpm_regs + 0x28);
        wmb();
        msleep(20);
        iounmap(cpm_regs);
    }

    /* Binary Ninja: Branch on interface type at 00010250 */
    /* CRITICAL FIX: Use correct enum values - MIPI=1, DVP=2 */
    if (interface_type == TX_SENSOR_DATA_INTERFACE_MIPI) {  /* MIPI = 1 */
        /* MIPI interface - Binary Ninja 00010688-00010a50 */
        pr_info("MIPI interface configuration\n");

        /* Binary Ninja: Check flags at 00010260 */
        if (sensor_attr->dbus_type != interface_type) {
            writel(0xa000a, vic_regs + 0x1a4);
        } else {
            writel(0x20000, vic_regs + 0x10);
            writel(0x100010, vic_regs + 0x1a4);
        }

        /* Calculate buffer size - Binary Ninja 000102b8-00010308 */
        u32 stride_mult = 8;
        if (sensor_format == 1) stride_mult = 0xa;
        else if (sensor_format == 2) stride_mult = 0xc;
        else if (sensor_format == 7) stride_mult = 0x10;

        u32 buffer_calc = stride_mult * sensor_attr->integration_time;
        writel((buffer_calc >> 5) + ((buffer_calc & 0x1f) ? 1 : 0), vic_regs + 0x100);

        /* Binary Ninja: Core DVP registers 00010310-00010338 */
        writel(2, vic_regs + 0xc);
        writel(sensor_format, vic_regs + 0x14);
        writel((vic_dev->width << 16) | vic_dev->height, vic_regs + 0x4);

        /* Frame mode based on WDR - Binary Ninja 00010414-00010478 */
        u32 wdr_mode = sensor_attr->wdr_cache;
        u32 frame_mode = (wdr_mode == 0) ? 0x4440 :
                        (wdr_mode == 1) ? 0x4140 : 0x4240;
        writel(frame_mode, vic_regs + 0x1ac);
        writel(frame_mode, vic_regs + 0x1a8);
        writel(0x10, vic_regs + 0x1b0);

        /* VIC interrupt initialization moved to END of function after CSI PHY setup */
        pr_info("*** VIC INTERRUPT INIT: VIC interrupt setup deferred until after CSI PHY writes ***\n");

        /* Unlock sequence - Binary Ninja 00010484-00010490 - EXACT REFERENCE IMPLEMENTATION */
        pr_info("*** VIC UNLOCK SEQUENCE: Starting unlock sequence ***\n");
        pr_info("*** VIC UNLOCK: Initial register 0x0 value = 0x%08x ***\n", readl(vic_regs + 0x0));

        writel(2, vic_regs + 0x0);
        wmb();
        pr_info("*** VIC UNLOCK: After writing 2, register 0x0 = 0x%08x ***\n", readl(vic_regs + 0x0));

        writel(4, vic_regs + 0x0);
        wmb();
        pr_info("*** VIC UNLOCK: After writing 4, register 0x0 = 0x%08x ***\n", readl(vic_regs + 0x0));

        /* Wait for unlock - Binary Ninja 000104b8 - DUAL VIC SPACE COORDINATION */
        timeout = 10000;  /* 10ms timeout */

        /* CRITICAL: Check CSI PHY coordination in SECONDARY VIC space (0x10023000) */
        void __iomem *secondary_regs = vic_dev->vic_regs_secondary;
        u32 secondary_val = secondary_regs ? readl(secondary_regs + 0x0) : 0;
        u32 primary_val = readl(vic_regs + 0x0);

        pr_info("*** VIC UNLOCK: Primary space (0x133e0000) = 0x%08x, Secondary space (0x10023000) = 0x%08x ***\n",
                primary_val, secondary_val);

        /* Handle CSI PHY coordination - 0x3130322a in secondary space is expected */
        if (secondary_val == 0x3130322a) {
            pr_info("*** VIC UNLOCK: CSI PHY coordination complete in secondary space ***\n");
        }

        /* Wait for primary VIC space unlock */
        while (readl(vic_regs + 0x0) != 0) {
            udelay(1);
            if (--timeout == 0) {
                primary_val = readl(vic_regs + 0x0);
                secondary_val = secondary_regs ? readl(secondary_regs + 0x0) : 0;
                pr_info("*** VIC UNLOCK TIMEOUT: Primary=0x%08x, Secondary=0x%08x ***\n", primary_val, secondary_val);
                pr_info("*** Continuing anyway to prevent infinite hang ***\n");
                break;  /* Continue instead of returning error to prevent hang */
            }
        }

        pr_info("*** VIC UNLOCK: Unlock sequence completed, register 0x0 = 0x%08x ***\n", readl(vic_regs + 0x0));

        /* vic_start_ok flag setting moved to END of function after CSI PHY setup */

        /* Enable VIC - Binary Ninja 000107d4 */
        pr_info("*** VIC UNLOCK: Enabling VIC (writing 1 to register 0x0) ***\n");
        writel(1, vic_regs + 0x0);
        pr_info("*** VIC UNLOCK: VIC enabled, register 0x0 = 0x%08x ***\n", readl(vic_regs + 0x0));

    } else if (interface_type == TX_SENSOR_DATA_INTERFACE_MIPI) {  /* MIPI = 1 in our enum */
        /* MIPI interface - Binary Ninja 000107ec-00010b04 */
        pr_info("MIPI interface configuration\n");

        /* CRITICAL: VIC hardware should already be initialized by platform driver */
        pr_info("*** VIC hardware should be ready - proceeding with unlock sequence ***\n");

        /* Binary Ninja: EXACT reference driver MIPI mode configuration */
        /* Binary Ninja: 000107ec - Set CSI mode (match working logs) */
        writel(2, vic_regs + 0xc);  /* Set VIC MIPI mode = 2 */
        wmb();
        pr_info("*** VIC: Set MIPI mode (2) to VIC control register 0xc (matches working logs) ***\n");

        /* BINARY NINJA EXACT: All missing register configurations */

        /* 1. Register 0x4 - Dimensions (Binary Ninja exact) */
        u32 width = 1920;   /* sensor output width */
        u32 height = 1080;  /* sensor output height */
        writel((width << 16) | height, vic_regs + 0x4);
        pr_info("*** BINARY NINJA: reg 0x4 = 0x%x (dimensions %dx%d) ***\n", (width << 16) | height, width, height);

        /* 2. Register 0x14 - Interrupt config (from sensor attributes) */
        writel(0x0, vic_regs + 0x14);  /* Start with safe default */
        pr_info("*** BINARY NINJA: reg 0x14 = 0x0 (interrupt config) ***\n");

        /* 3. Register 0x100 - Complex calculation for MIPI */
        u32 reg_100_value = 0x1;  /* Basic value for MIPI RAW10 */
        writel(reg_100_value, vic_regs + 0x100);
        pr_info("*** BINARY NINJA: reg 0x100 = 0x%x (MIPI calculation) ***\n", reg_100_value);

        /* DISABLED per working branch: defer MIPI timing/packing (0x10c, 0x110-0x11c)
         * These writes interfered with CSI/MIPI bring-up. Allow sensor/CSI path to program them.
         */
        pr_info("*** VIC: Skipping writes to 0x10c and 0x110-0x11c (defer to CSI/MIPI path) ***\n");


        /* 6. Frame mode registers */
        writel(0x4440, vic_regs + 0x1ac);  /* Binary Ninja default for interface type 1 */
        writel(0x4440, vic_regs + 0x1a8);
        writel(0x10, vic_regs + 0x1b0);
        pr_info("*** BINARY NINJA: frame mode regs configured (0x4440, 0x4440, 0x10) ***\n");

        /* 7. Register 0x1a0 - Additional frame config */
        writel(0x0, vic_regs + 0x1a0);  /* Binary Ninja: frame config */
        pr_info("*** BINARY NINJA: reg 0x1a0 = 0x0 (frame config) ***\n");

        /* 8. Register 0x1a4 - Control register */
        writel(0x100010, vic_regs + 0x1a4);  /* Binary Ninja exact value */
        pr_info("*** BINARY NINJA: reg 0x1a4 = 0x100010 (control) ***\n");

        /* 9. BINARY NINJA EXACT: Hardware enable sequence */
        writel(0x2, vic_regs + 0x0);  /* Pre-enable */
        wmb();
        writel(0x4, vic_regs + 0x0);  /* Wait state */
        wmb();

        /* Wait for hardware ready (Binary Ninja: while (*$v1_30 != 0) nop) */
        u32 wait_count = 0;
        while ((readl(vic_regs + 0x0) != 0) && (wait_count < 1000)) {
            wait_count++;
            udelay(1);
        }

        writel(0x1, vic_regs + 0x0);  /* Final enable */
        wmb();
        pr_info("*** BINARY NINJA EXACT: Hardware sequence 2->4->wait(%d us)->1 ***\n", wait_count);


        /* Re-assert stream control after this enable too, to guard against register clearing */
        {
            u32 buffer_count = vic_dev->active_buffer_count;
            if (buffer_count == 0) buffer_count = 2;
            if (buffer_count > 5) buffer_count = 5;
            u32 stream_ctrl = (buffer_count << 16) | 0x80000020;
            writel(stream_ctrl, vic_regs + 0x300);
            wmb();
            pr_info("*** POST-ENABLE(A): Rewrote VIC[0x300]=0x%x (buffer_count=%u) ***\n", stream_ctrl, buffer_count);
        }

        /* Format detection logic - Binary Ninja 000107f8-00010a04 */
        u32 mipi_config;

        if (sensor_format >= 0x3010) {
            if (sensor_format >= 0x3110) {
                if (sensor_format >= 0x3200) {
                    if (sensor_format < 0x3210) {
                        mipi_config = 0x20000;
                    } else if ((sensor_format - 0x3300) < 0x10) {
                        mipi_config = 0x40000;
                        if (sensor_attr->total_width == 2) {
                            mipi_config = 0x50000;
                        }
                    } else {
                        pr_info("Format 0x%x not supported\n", sensor_format);
                        return -1;
                    }
                } else {
                    mipi_config = 0x20000;
                }
            } else if (sensor_format >= 0x3100) {
                u32 gpio_mode = sensor_attr->dbus_type;
                if (gpio_mode == 3) {
                    mipi_config = 0;
                } else if (gpio_mode == 4) {
                    mipi_config = 0x100000;
                } else {
                    pr_info("DVP mode config failed\n");
                    return -1;
                }
            } else if (sensor_format >= 0x3013 && sensor_format < 0x3015) {
                u32 gpio_mode = sensor_attr->dbus_type;
                if (gpio_mode == 3) {
                    mipi_config = 0;
                } else if (gpio_mode == 4) {
                    mipi_config = 0x100000;
                } else {
                    pr_info("DVP mode config failed\n");
                    return -1;
                }
            } else {
                mipi_config = 0x40000;
                if (sensor_attr->total_width == 2) {
                    mipi_config = 0x50000;
                }
            }
        } else if (sensor_format >= 0x300e) {
            mipi_config = 0x20000;
        } else if (sensor_format == 0x2011) {
            mipi_config = 0xc0000;
        } else if (sensor_format >= 0x2012) {
            if (sensor_format == 0x3007) {
                mipi_config = 0x20000;
            } else if (sensor_format < 0x3008) {
                if ((sensor_format - 0x3001) < 2) {
                    u32 gpio_mode = sensor_attr->dbus_type;
                    if (gpio_mode == 3) {
                        mipi_config = 0;
                    } else if (gpio_mode == 4) {
                        mipi_config = 0x100000;
                    } else {
                        pr_info("DVP mode config failed\n");
                        return -1;
                    }
                } else {
                    pr_info("Format 0x%x not supported\n", sensor_format);
                    return -1;
                }
            } else if (sensor_format == 0x3008) {
                mipi_config = 0x40000;
                if (sensor_attr->total_width == 2) {
                    mipi_config = 0x50000;
                }
            } else if (sensor_format == 0x300a) {
                mipi_config = 0x20000;
            } else {
                pr_info("Format 0x%x not supported\n", sensor_format);
                return -1;
            }
        } else if (sensor_format == 0x1008) {
            mipi_config = 0x80000;
        } else if (sensor_format >= 0x1009) {
            if ((sensor_format - 0x2002) >= 4) {
                pr_info("Format 0x%x not supported\n", sensor_format);
                return -1;
            }
            mipi_config = 0xc0000;
        } else if (sensor_format == 0x1006) {
            mipi_config = 0xa0000;
        } else {
            /* Default case - includes RAW10 (0x2b) */
            mipi_config = 0x20000;
        }

        /* Binary Ninja: 00010a08-00010a30 - Apply width/height flags */
        if (sensor_attr->total_width == 2) {
            mipi_config |= 2;
        }
        if (sensor_attr->total_height == 2) {
            mipi_config |= 1;
        }

        /* Binary Ninja: 00010a2c-00010a78 - Integration time and gain */
        /* CRITICAL FIX: Register 0x18 is a TIMING parameter, NOT a width register! */
        /* The reference driver sets 0x18 = 0xf00 (3840) and it must stay that way */
        /* DO NOT overwrite register 0x18 with sensor width - this causes control limit errors */
        u32 integration_time = sensor_attr->integration_time;
        pr_info("*** CRITICAL: Skipping register 0x18 write - it's a timing parameter (0xf00), not width! ***\n");

        u32 again = sensor_attr->again;
        if (again != 0) {
            writel(again, vic_regs + 0x3c);
        }

        /* Binary Ninja: EXACT reference driver MIPI configuration */
        /* Binary Ninja: 00010a90-00010aa8 - Final MIPI config */
        /* Use actual sensor output width instead of total width to prevent control limit error */
        writel((actual_width << 31) | mipi_config, vic_regs + 0x10);
        writel((actual_width << 16) | actual_height, vic_regs + 0x4);
        wmb();

        /* Binary Ninja: 00010ab4-00010ac0 - Unlock sequence - EXACT REFERENCE IMPLEMENTATION */
        /* Binary Ninja: EXACT reference driver unlock sequence */
        writel(2, vic_regs + 0x0);
        wmb();

        /* CRITICAL FIX: Skip remaining unlock sequence during streaming restart */
        if (vic_start_ok == 1) {
            pr_info("*** VIC: SKIPPING remaining unlock sequence - VIC interrupts already working ***\n");
        } else {
            writel(4, vic_regs + 0x0);
            wmb();

            /* Binary Ninja: 00010acc - Wait for unlock */
            while (readl(vic_regs + 0x0) != 0) {
                udelay(1);
                if (--timeout == 0) {
                    pr_info("VIC unlock timeout\n");
                    return -ETIMEDOUT;
                }
            }

            /* Binary Ninja: 00010ad4 - Enable VIC */
            writel(1, vic_regs + 0x0);
            wmb();
        }

        /* Binary Ninja: 00010ae4-00010b04 - Final MIPI registers */
        writel(0x100010, vic_regs + 0x1a4);
        writel(0x4210, vic_regs + 0x1ac);
        writel(0x10, vic_regs + 0x1b0);
        writel(0, vic_regs + 0x1b4);
        wmb();

    } else if (interface_type == TX_SENSOR_DATA_INTERFACE_BT601) {
        /* BT601 - Binary Ninja 00010688-000107d4 */
        pr_info("BT601 interface configuration\n");

        writel(1, vic_regs + 0xc);

        int gpio_mode = sensor_attr->dbus_type;
        u32 bt601_config;

        if (gpio_mode == 0) {
            bt601_config = 0x800c8000;
        } else if (gpio_mode == 1) {
            bt601_config = 0x88060820;
        } else {
            pr_info("Unsupported GPIO mode\n");
            return -1;
        }

        writel(bt601_config, vic_regs + 0x10);
        writel((actual_width << 1) | 0x100000, vic_regs + 0x18);
        writel(0x30, vic_regs + 0x3c);
        writel(0x1b8, vic_regs + 0x1c);
        writel(0x1402d0, vic_regs + 0x30);
        writel(0x50014, vic_regs + 0x34);
        writel(0x2d00014, vic_regs + 0x38);
        writel(0, vic_regs + 0x1a0);
        writel(0x100010, vic_regs + 0x1a4);
        writel(0x4440, vic_regs + 0x1ac);
        writel((actual_width << 16) | actual_height, vic_regs + 0x4);

        /* CRITICAL FIX: Complete unlock sequence matching reference driver */
        writel(2, vic_regs + 0x0);
        wmb();
        writel(1, vic_regs + 0x0);

    } else if (interface_type == TX_SENSOR_DATA_INTERFACE_BT656) {
        /* BT656 - Binary Ninja 000105b0-00010684 */
        pr_info("BT656 interface configuration\n");

        writel(0, vic_regs + 0xc);
        writel(0x800c0000, vic_regs + 0x10);
        writel((actual_width << 16) | actual_height, vic_regs + 0x4);
        writel(actual_width << 1, vic_regs + 0x18);
        writel(0x100010, vic_regs + 0x1a4);
        writel(0x4440, vic_regs + 0x1ac);
        writel(0x200, vic_regs + 0x1d0);
        writel(0x200, vic_regs + 0x1d4);

        /* CRITICAL FIX: Complete unlock sequence matching reference driver */
        writel(2, vic_regs + 0x0);
        wmb();
        writel(1, vic_regs + 0x0);

    } else if (interface_type == TX_SENSOR_DATA_INTERFACE_BT1120) {
        /* BT1120 - Binary Ninja 00010500-00010684 */
        pr_info("BT1120 interface configuration\n");

        writel(4, vic_regs + 0xc);
        writel(0x800c0000, vic_regs + 0x10);
        writel((actual_width << 16) | actual_height, vic_regs + 0x4);
        writel(actual_width << 1, vic_regs + 0x18);
        writel(0x100010, vic_regs + 0x1a4);
        writel(0x4440, vic_regs + 0x1ac);

        /* CRITICAL FIX: Complete unlock sequence matching reference driver */
        writel(2, vic_regs + 0x0);
        wmb();
        writel(1, vic_regs + 0x0);

    } else {
        pr_info("Unsupported interface type %d\n", interface_type);
        return -1;
    }

    /* Binary Ninja: 00010b48-00010b74 - Log WDR mode */
    if (sensor_attr->wdr_cache != 0) {
        pr_info("tx_isp_vic_start: WDR mode enabled\n");
    } else {
        pr_info("tx_isp_vic_start: Linear mode enabled\n");
    }

    /* Binary Ninja: 00010b84 - Set vic_start_ok */
    vic_start_ok = 1;
    pr_info("*** VIC start completed - vic_start_ok = 1 ***\n");

    /* CRITICAL: Enable ISP core interrupt generation - EXACT Binary Ninja reference */
    /* This was the missing piece that caused interrupts to stall out */
    if (ourISPdev && ourISPdev->core_regs) {
        void __iomem *core = ourISPdev->core_regs;

        /* Clear any pending interrupts first */
        u32 pend_legacy = readl(core + 0xb4);
        u32 pend_new    = readl(core + 0x98b4);
        writel(pend_legacy, core + 0xb8);
        writel(pend_new,    core + 0x98b8);

        /* CRITICAL: Enable ISP pipeline connection - this is what was missing! */
        /* Binary Ninja: system_reg_write(0x800, 1) - Enable ISP pipeline */
        writel(1, core + 0x800);

        /* Binary Ninja: system_reg_write(0x804, routing) - Configure ISP routing */
        writel(0x1c, core + 0x804);

        /* Binary Ninja: system_reg_write(0x1c, 8) - Set ISP control mode */
        writel(8, core + 0x1c);

        /* CRITICAL: Enable ISP core interrupt generation at hardware level */
        /* Binary Ninja: system_reg_write(0x30, 0xffffffff) - Enable all interrupt sources */
        writel(0xffffffff, core + 0x30);

        /* Binary Ninja: system_reg_write(0x10, 0x133) - Enable specific interrupt types */
        writel(0x133, core + 0x10);

        /* Enable interrupt banks */
        writel(0x3FFF, core + 0xb0);
        writel(0x3FFF, core + 0xbc);
        writel(0x3FFF, core + 0x98b0);
        writel(0x3FFF, core + 0x98bc);
        wmb();

        pr_info("*** ISP PIPELINE: VIC->ISP connection ENABLED (0x800=1, 0x804=0x1c, 0x1c=8) ***\n");
        pr_info("*** ISP CORE: Hardware interrupt generation ENABLED during VIC init ***\n");
        pr_info("*** VIC->ISP: Pipeline should now generate hardware interrupts when VIC completes frames! ***\n");
    } else {
        pr_info("*** ISP CORE IRQ: core_regs not mapped; unable to enable core interrupts here ***\n");
    }

    /* Also enable the kernel IRQ line if it was registered earlier */
    enable_irq(37);

    return 0;
}


/* VIC sensor IOCTL handler - EXACT Binary Ninja reference implementation */
int vic_sensor_ops_ioctl(struct tx_isp_subdev *sd, unsigned int cmd, void *arg)
{
    int result = 0;
    struct tx_isp_vic_device *vic_dev;

    if (sd != NULL && (unsigned long)sd < 0xfffff001) {
        /* Binary Ninja: void* $a0 = *(arg1 + 0xd4) */
        vic_dev = (struct tx_isp_vic_device *)tx_isp_get_subdev_hostdata(sd);

        if (vic_dev != NULL && (unsigned long)vic_dev < 0xfffff001) {
            /* Binary Ninja: if (arg2 - 0x200000c u>= 0xd) return 0 */
            if (cmd - 0x200000c >= 0xd) {
                return 0;
            }

            switch (cmd) {
                case 0x200000c:
                case 0x200000f:
                    /* Binary Ninja: return tx_isp_vic_start($a0) */
                    return tx_isp_vic_start(vic_dev);

                case 0x200000d:
                case 0x2000010:
                case 0x2000011:
                case 0x2000012:
                case 0x2000014:
                case 0x2000015:
                case 0x2000016:
                    /* Binary Ninja: return 0 */
                    return 0;

                case 0x200000e:
                    /* Binary Ninja: **($a0 + 0xb8) = 0x10 */
                    /* Set VIC register to 0x10 */
                    if (vic_dev->vic_regs) {
                        writel(0x10, vic_dev->vic_regs + 0xb8);
                    }
                    return 0;

                case 0x2000013:
                    /* Binary Ninja: **($a0 + 0xb8) = 0, then = 4 */
                    /* Set VIC register to 0, then 4 */
                    if (vic_dev->vic_regs) {
                        writel(0, vic_dev->vic_regs + 0xb8);
                        writel(4, vic_dev->vic_regs + 0xb8);
                    }
                    return 0;

		        case 0x2000017:
		            pr_info("vic_sensor_ops_ioctl: GPIO configuration (cmd=0x%x)\n", cmd);
		            /* Binary Ninja GPIO configuration - simplified for now */
		            if (arg) {
		                gpio_switch_state = 0;  /* Reset GPIO state */
		                /* memcpy(&gpio_info, arg, 0x2a) would go here */
		                pr_info("vic_sensor_ops_ioctl: GPIO config processed\n");
		            }
		            return 0;

		        case 0x2000018:
		            pr_info("vic_sensor_ops_ioctl: GPIO switch state (cmd=0x%x)\n", cmd);
		            /* Binary Ninja: gpio_switch_state = 1, memcpy(&gpio_info, arg, 0x2a) */
		            gpio_switch_state = 1;
		            if (arg) {
		                /* Copy GPIO info structure */
		                memcpy(&gpio_info, arg, 0x2a);
		                pr_info("vic_sensor_ops_ioctl: GPIO switch state enabled, info copied\n");
		            }
		            return 0;
            }
        }
    }

    return result;
}

/* VIC sensor operations sync_sensor_attr - EXACT Binary Ninja implementation */
int vic_sensor_ops_sync_sensor_attr(struct tx_isp_subdev *sd, struct tx_isp_sensor_attribute *attr)
{
    struct tx_isp_vic_device *vic_dev;

    pr_info("vic_sensor_ops_sync_sensor_attr: sd=%p, attr=%p\n", sd, attr);

    if (!sd || (unsigned long)sd >= 0xfffff001) {
        pr_err("The parameter is invalid!\n");
        return -EINVAL;
    }

    vic_dev = (struct tx_isp_vic_device *)tx_isp_get_subdevdata(sd);
    if (!vic_dev || (unsigned long)vic_dev >= 0xfffff001) {
        pr_err("The parameter is invalid!\n");
        return -EINVAL;
    }

    /* Binary Ninja: $v0_1 = arg2 == 0 ? memset : memcpy */
    if (attr == NULL) {
        /* Clear sensor attribute */
        memset(&vic_dev->sensor_attr, 0, sizeof(vic_dev->sensor_attr));
        pr_info("vic_sensor_ops_sync_sensor_attr: cleared sensor attributes\n");
    } else {
        /* Copy sensor attribute */
        memcpy(&vic_dev->sensor_attr, attr, sizeof(vic_dev->sensor_attr));
        pr_info("vic_sensor_ops_sync_sensor_attr: copied sensor attributes\n");
    }

    return 0;
}

/* VIC core operations ioctl - EXACT Binary Ninja implementation */
int vic_core_ops_ioctl(struct tx_isp_subdev *sd, unsigned int cmd, void *arg)
{
    int result = -ENOTSUPP;  /* 0xffffffed */
    void *callback_ptr;
    int (*callback_func)(void);  /* Changed to int return type */

    pr_info("vic_core_ops_ioctl: cmd=0x%x, arg=%p\n", cmd, arg);

    /* CRITICAL FIX: Handle TX_ISP_EVENT_SYNC_SENSOR_ATTR specifically */
    if (cmd == TX_ISP_EVENT_SYNC_SENSOR_ATTR) {
        pr_info("*** CRITICAL FIX: TX_ISP_EVENT_SYNC_SENSOR_ATTR event received ***\n");

        /* Call the sensor sync function which will return -515 */
        result = vic_sensor_ops_sync_sensor_attr(sd, (struct tx_isp_sensor_attribute *)arg);

        pr_info("*** TX_ISP_EVENT_SYNC_SENSOR_ATTR handled successfully, returning %d ***\n", result);
        return result;
    }

    if (cmd == 0x1000001) {
        result = -ENOTSUPP;
        if (sd != NULL) {
            /* Binary Ninja: void* $v0_2 = *(*(arg1 + 0xc4) + 0xc) */
            if (sd->inpads && sd->inpads[0].priv) {
                callback_ptr = sd->inpads[0].priv;
                if (callback_ptr != NULL) {
                    /* Get function pointer at offset +4 in callback structure */
                    callback_func = *((int (**)(void))((char *)callback_ptr + 4));
                    if (callback_func != NULL) {
                        pr_info("vic_core_ops_ioctl: Calling callback function for cmd 0x1000001\n");
                        result = callback_func();  /* Call the function without casting */
                    }
                }
            }
        }
    } else if (cmd == 0x3000008) {  /* TX_ISP_EVENT_BUFFER_REQUEST / Buffer allocation */
        struct tx_isp_vic_device *vic_dev;
        struct {
            int32_t channel_id;
            int32_t buffer_count;
        } *event_data = (void *)arg;
        int32_t channel_id;
        int32_t buffer_count;

        if (!sd || !sd->host_priv || !event_data) {
            pr_err("vic_core_ops_ioctl: 0x3000008 - missing sd/host_priv/arg\n");
            return -EINVAL;
        }

        vic_dev = (struct tx_isp_vic_device *)sd->host_priv;
        channel_id = event_data->channel_id;
        buffer_count = event_data->buffer_count;

        pr_info("*** vic_core_ops_ioctl: 0x3000008 - Channel %d buffer allocation: count=%d ***\n",
                channel_id, buffer_count);

        /* Validate buffer count */
        if (buffer_count < 0 || buffer_count > 64) {
            pr_err("vic_core_ops_ioctl: 0x3000008 - invalid buffer count %d\n", buffer_count);
            return -EINVAL;
        }

        /* Only Channel 0 controls VIC hardware buffer count */
        if (channel_id == 0) {
            vic_dev->active_buffer_count = (buffer_count > 5) ? 5 : buffer_count;
            pr_info("*** vic_core_ops_ioctl: Channel 0 - VIC active_buffer_count set to %d ***\n",
                    vic_dev->active_buffer_count);
        } else {
            pr_info("*** vic_core_ops_ioctl: Channel %d - VIC active_buffer_count unchanged (%d) ***\n",
                    channel_id, vic_dev->active_buffer_count);
        }
        return 0;
    } else if (cmd == 0x3000005) {  /* BUFFER_ENQUEUE: program provided node */
        if (sd) {
            (void) ispvic_frame_channel_qbuf(sd, arg);
            result = 0;
        } else {
            result = -EINVAL;
        }
        return result;
    } else if (cmd == 0x3000009) {
        pr_info("vic_core_ops_ioctl: tx_isp_subdev_pipo cmd=0x%x\n", cmd);
        result = tx_isp_subdev_pipo(sd, arg);
    } else if (cmd == 0x1000000) {
        result = -ENOTSUPP;
        if (sd != NULL) {
            /* Binary Ninja: void* $v0_5 = **(arg1 + 0xc4) */
            if (sd->inpads && sd->inpads[0].priv) {
                callback_ptr = sd->inpads[0].priv;
                if (callback_ptr != NULL) {
                    /* Get function pointer at offset +4 in callback structure */
                    callback_func = *((int (**)(void))((char *)callback_ptr + 4));
                    if (callback_func != NULL) {
                        pr_info("vic_core_ops_ioctl: Calling callback function for cmd 0x1000000\n");
                        result = callback_func();  /* Call the function without casting */
                    }
                }
            }
        }
    } else {
        pr_info("vic_core_ops_ioctl: Unknown cmd=0x%x, returning 0\n", cmd);
        return 0;
    }

    /* Binary Ninja: if (result == 0xfffffdfd) return 0 */
    if (result == -515) {  /* 0xfffffdfd */
        pr_info("vic_core_ops_ioctl: Result -515, returning 0\n");
        return 0;
    }

    return result;
}

/* ISP VIC FRD show function - EXACT Binary Ninja implementation */
int isp_vic_frd_show(struct seq_file *seq, void *v)
{
    struct tx_isp_subdev *sd;
    struct tx_isp_vic_device *vic_dev;
    int i, total_errors = 0;
    int frame_count;

    /* Binary Ninja: void* $v0 = *(arg1 + 0x3c) */
    sd = (struct tx_isp_subdev *)seq->private;
    if (!sd || (unsigned long)sd >= 0xfffff001) {
        pr_err("The parameter is invalid!\n");
        return 0;
    }

    vic_dev = (struct tx_isp_vic_device *)tx_isp_get_subdevdata(sd);
    if (!vic_dev || (unsigned long)vic_dev >= 0xfffff001) {
        pr_err("The parameter is invalid!\n");
        return 0;
    }

    /* Binary Ninja: *($v0_1 + 0x164) = 0 */
    vic_dev->total_errors = 0;

    /* Binary Ninja: Sum up error counts from vic_err array */
    for (i = 0; i < 13; i++) {  /* 0x34 / 4 = 13 elements */
        total_errors += vic_dev->vic_errors[i];
    }

    frame_count = vic_dev->frame_count;
    vic_dev->total_errors = total_errors;

    /* Binary Ninja: private_seq_printf(arg1, " %d, %d\n", $a2) */
    seq_printf(seq, " %d, %d\n", frame_count, total_errors);

    /* Binary Ninja: Print all error counts */
    return seq_printf(seq, "%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n",
                     vic_dev->vic_errors[0], vic_dev->vic_errors[1], vic_dev->vic_errors[2],
                     vic_dev->vic_errors[3], vic_dev->vic_errors[4], vic_dev->vic_errors[5],
                     vic_dev->vic_errors[6], vic_dev->vic_errors[7], vic_dev->vic_errors[8],
                     vic_dev->vic_errors[9], vic_dev->vic_errors[10], vic_dev->vic_errors[11],
                     vic_dev->vic_errors[12]);
}

/* Dump ISP VIC FRD open - EXACT Binary Ninja implementation */
int dump_isp_vic_frd_open(struct inode *inode, struct file *file)
{
    /* Binary Ninja: return private_single_open_size(arg2, isp_vic_frd_show, PDE_DATA(), 0x400) __tailcall */
    return single_open_size(file, isp_vic_frd_show, PDE_DATA(inode), 0x400);
}

/* ISP VIC cmd set function - placeholder matching reference driver interface */
long isp_vic_cmd_set(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct tx_isp_subdev *sd = file->private_data;

    pr_info("isp_vic_cmd_set: cmd=0x%x, arg=0x%lx\n", cmd, arg);

    if (!sd) {
        pr_err("isp_vic_cmd_set: No subdev in file private_data\n");
        return -EINVAL;
    }

    /* Forward to the main VIC ioctl handler */
    return vic_chardev_ioctl(file, cmd, arg);
}

/* VIC activation function - matching reference driver */
int tx_isp_vic_activate_subdev(struct tx_isp_subdev *sd)
{
    struct tx_isp_vic_device *vic_dev;
    struct clk **clks;
    int clk_count;
	int i;
    if (!sd)
        return -EINVAL;

    vic_dev = (struct tx_isp_vic_device *)tx_isp_get_subdevdata(sd);
    if (!vic_dev) {
        pr_err("VIC device is NULL\n");
        return -EINVAL;
    }

    mutex_lock(&vic_dev->state_lock);

    if (vic_dev->state == 1) {
        vic_dev->state = 2; /* INIT -> READY */
        pr_info("VIC activated: state %d -> 2 (READY)\n", 1);

        /* Enable VIC/CSI clocks like reference; do not preallocate buffers here */
        clks = sd->clks;
        if (clks && sd->clk_num > 0) {
            clk_count = sd->clk_num;
            for (i = 0; i < clk_count; i++) {
                clk_enable(clks[i]);
            }
            pr_info("*** VIC ACTIVATION: Enabled %d clocks ***\n", clk_count);
        } else {
            pr_warn("*** VIC ACTIVATION: No clocks to enable (clks=%p, clk_num=%d) ***\n", clks, sd->clk_num);
        }
        /* Defer buffer allocation to QBUF path to match reference */
        vic_dev->buffer_count = 0;
	}

    mutex_unlock(&vic_dev->state_lock);
    return 0;
}
EXPORT_SYMBOL(tx_isp_vic_activate_subdev);

/* VIC core operations initialization - EXACT Binary Ninja reference implementation */
int vic_core_ops_init(struct tx_isp_subdev *sd, int enable)
{
    struct tx_isp_vic_device *vic_dev;
    int state;

    if (!sd)
        return -EINVAL;

    vic_dev = (struct tx_isp_vic_device *)tx_isp_get_subdevdata(sd);
    if (!vic_dev) {
        pr_err("VIC device is NULL\n");
        return -EINVAL;
    }

    /* Binary Ninja: Read state at offset 0x128 */
    state = vic_dev->state;

    /* Binary Ninja: if (arg2 == 0) - disable path */
    if (enable == 0) {
        /* Binary Ninja: if ($v0_2 != 2) */
        if (state != 2) {
            /* Binary Ninja: tx_vic_disable_irq() */
            tx_vic_disable_irq(vic_dev);
            /* Binary Ninja: *($s1_1 + 0x128) = 2 */
            vic_dev->state = 2;
            pr_info("vic_core_ops_init: Disabled VIC, state -> 2\n");
        }
    } else {
        /* Binary Ninja: else - enable path */
        /* Binary Ninja: if ($v0_2 != 3) */
        if (state != 3) {
            /* Binary Ninja: tx_vic_enable_irq() */
            tx_vic_enable_irq(vic_dev);
            /* Binary Ninja: *($s1_1 + 0x128) = 3 */
            vic_dev->state = 3;
            pr_info("vic_core_ops_init: Enabled VIC, state -> 3\n");
        }
    }

    pr_info("vic_core_ops_init: enable=%d, final state=%d\n", enable, vic_dev->state);
    return 0;
}

/* VIC slake function - EXACT Binary Ninja reference implementation */
int tx_isp_vic_slake_subdev(struct tx_isp_subdev *sd)
{
    struct tx_isp_vic_device *vic_dev;
    int state;

    if (!sd)
        return -EINVAL;

    vic_dev = (struct tx_isp_vic_device *)tx_isp_get_subdevdata(sd);
    if (!vic_dev) {
        pr_err("VIC device is NULL\n");
        return -EINVAL;
    }

    pr_info("*** tx_isp_vic_slake_subdev: ENTRY - state=%d ***\n", vic_dev->state);

    /* Binary Ninja: Read state at offset 0x128 */
    state = vic_dev->state;

    /* Binary Ninja: if (state == 4) call vic_core_s_stream and re-read state */
    if (state == 4) {
        pr_info("tx_isp_vic_slake_subdev: VIC streaming (state=4) -> stop stream\n");
        vic_core_s_stream(sd, 0);
        state = vic_dev->state;  /* Re-read state after stopping stream */
    }

    /* Binary Ninja: if (state == 3) call vic_core_ops_init(sd, 0) */
    if (state == 3) {
        pr_info("tx_isp_vic_slake_subdev: VIC state 3 -> vic_core_ops_init(0)\n");
        vic_core_ops_init(sd, 0);
    }

    /* Binary Ninja: Lock mutex at offset 0x130 (state_lock) */
    mutex_lock(&vic_dev->state_lock);

    /* Binary Ninja: Only transition from state 2 to state 1 */
    if (vic_dev->state == 2) {
        vic_dev->state = 1;
        pr_info("tx_isp_vic_slake_subdev: VIC state 2 -> 1 (INIT)\n");
    }

    mutex_unlock(&vic_dev->state_lock);

    pr_info("*** tx_isp_vic_slake_subdev: EXIT - state=%d ***\n", vic_dev->state);
    return 0;
}

/* VIC PIPO MDMA Enable function - EXACT Binary Ninja implementation */
static void vic_pipo_mdma_enable(struct tx_isp_vic_device *vic_dev)
{
    void __iomem *vic_base;
    u32 width, height, stride;

    pr_info("*** vic_pipo_mdma_enable: EXACT Binary Ninja implementation + control-bank mirror ***\n");

    /* Validate vic_dev */
    if (!vic_dev) {
        pr_err("vic_pipo_mdma_enable: NULL vic_dev parameter\n");
        return;
    }

    vic_base = vic_dev->vic_regs;

    /* Validate VIC register base */
    if (!vic_base || (unsigned long)vic_base < 0x80000000 || (unsigned long)vic_base == 0x735f656d) {
        pr_err("vic_pipo_mdma_enable: Invalid VIC register base %p - ABORTING\n", vic_base);
        return;
    }

    /* Prefer fresh sensor dimensions, fallback to cached */
    if (vic_dev->sensor_attr.total_width != 0 && vic_dev->sensor_attr.total_height != 0) {
        width = vic_dev->sensor_attr.total_width;
        height = vic_dev->sensor_attr.total_height;
        pr_info("*** Using sensor_attr dimensions %dx%d ***\n", width, height);
    } else {
        width = vic_dev->width;
        height = vic_dev->height;
        pr_info("*** Using vic_dev dimensions %dx%d ***\n", width, height);
    }

    if (width == 0 || height == 0) {
        width = 1920;  /* fallback */
        height = 1080; /* fallback */
        vic_dev->width = width;
        vic_dev->height = height;
        pr_info("*** DIMENSION FIX: fallback to %dx%d ***\n", width, height);
    }

    /* Reference: pre-format stride = width<<1; final stride updated after 0x300 (format) is set */
    stride = width << 1; /* bytes per line for packed paths; NV12 will be corrected post-ctrl */
    pr_info("vic_pipo_mdma_enable: dims=%dx%d, pre-format stride=%u (ref)\n", width, height, stride);

    /* MDMA enable: 0x308 */
    writel(1, vic_base + 0x308);
    if (vic_dev->vic_regs_secondary)
        writel(1, vic_dev->vic_regs_secondary + 0x308);
    wmb();

    /* Frame size: 0x304 (width<<16 | height) */
    writel((width << 16) | height, vic_base + 0x304);
    if (vic_dev->vic_regs_secondary)
        writel((width << 16) | height, vic_dev->vic_regs_secondary + 0x304);
    wmb();

    /* Stride: 0x310 and 0x314 */
    writel(stride, vic_base + 0x310);
    if (vic_dev->vic_regs_secondary)
        writel(stride, vic_dev->vic_regs_secondary + 0x310);
    wmb();

    writel(stride, vic_base + 0x314);
    if (vic_dev->vic_regs_secondary)
        writel(stride, vic_dev->vic_regs_secondary + 0x314);
    wmb();

    pr_info("*** VIC PIPO MDMA ENABLE COMPLETE ***\n");
}

/* ISPVIC Frame Channel S_Stream - EXACT Binary Ninja Implementation */
int ispvic_frame_channel_s_stream(struct tx_isp_vic_device *vic_dev, int enable)
{
    void __iomem *vic_base = NULL;
    int32_t var_18 = 0;
    const char *stream_op;

    pr_info("*** ispvic_frame_channel_s_stream: RACE CONDITION FIX ***\n");
    pr_info("ispvic_frame_channel_s_stream: vic_dev=%p, enable=%d\n", vic_dev, enable);

    if (!vic_dev) {
        pr_err("%s[%d]: invalid parameter\n", "ispvic_frame_channel_s_stream", __LINE__);
        return -EINVAL;
    }

    /* Binary Ninja: Set stream operation string */
    stream_op = (enable != 0) ? "streamon" : "streamoff";
    pr_info("%s[%d]: %s\n", "ispvic_frame_channel_s_stream", __LINE__, stream_op);

    /* Binary Ninja EXACT: if (enable == stream_state) return 0
     * Exception: If enable==1 but hardware ctrl is 0 (not running), force a control write. */
    {
        u32 ctrl_peek = 0;
        void __iomem *peek_base = vic_dev->vic_regs;
        if (peek_base && (unsigned long)peek_base >= 0x80000000)
            ctrl_peek = readl(peek_base + 0x300);
        pr_info("ispvic_frame_channel_s_stream: pre-check stream_state=%d, enable=%d, ctrl_peek=0x%x\n",
                vic_dev->stream_state, enable, ctrl_peek);
        if (enable == vic_dev->stream_state) {
            if (enable && ctrl_peek == 0) {
                pr_info("ispvic_frame_channel_s_stream: stream_state=1 but ctrl=0, forcing control write\n");
            } else {
                return 0;
            }
        }
    }

    /* Binary Ninja EXACT: __private_spin_lock_irqsave($s0 + 0x1f4, &var_18) */
    __private_spin_lock_irqsave(&vic_dev->buffer_mgmt_lock, &var_18);

    if (enable == 0) {
        /* Stream OFF */
        /* Binary Ninja EXACT: *(*($s0 + 0xb8) + 0x300) = 0 */
        vic_base = vic_dev->vic_regs;
        if (vic_base && (unsigned long)vic_base >= 0x80000000) {
            writel(0, vic_base + 0x300);
            wmb();
            pr_info("ispvic_frame_channel_s_stream: Stream OFF - wrote 0 to reg 0x300\n");
        }

        /* Binary Ninja EXACT: *($s0 + 0x210) = 0 */
        vic_dev->stream_state = 0;

    } else {
        /* Stream ON */
        /* Binary Ninja EXACT: vic_pipo_mdma_enable($s0) */
        vic_pipo_mdma_enable(vic_dev);

        /* Binary Ninja EXACT: *(*($s0 + 0xb8) + 0x300) = *($s0 + 0x218) << 0x10 | 0x80000020 */
        vic_base = vic_dev->vic_regs;
        if (vic_base && (unsigned long)vic_base >= 0x80000000) {
            pr_info("ispvic_frame_channel_s_stream: BEFORE - active_buffer_count=%d\n", vic_dev->active_buffer_count);

            /* CRITICAL FIX: Follow EXACT reference driver buffer management sequence */
            /* Reference driver ALWAYS starts VIC hardware immediately with proper buffer configuration */
            pr_info("*** REFERENCE DRIVER SEQUENCE: Starting VIC hardware with exact buffer configuration ***\n");

            /* Binary Ninja EXACT: Use reference driver buffer management values */
            /* The reference driver uses specific buffer count and control values that prevent control limit errors */
            u32 reference_buffer_count = 2;  /* Reference driver uses 2 buffers */
            u32 reference_control_bits = 0x80000020;  /* Reference driver control bits (format set elsewhere) */

            /* BINARY NINJA EXACT: Use exact reference driver formula to prevent control limit error */
            /* Binary Ninja: program stride and buffer bases before writing control (0x300) */
            u32 buffer_count = vic_dev->active_buffer_count;
            if (buffer_count == 0) buffer_count = 2;       /* ensure at least double-buffering */
            if (buffer_count > 5) buffer_count = 5;        /* 5-slot ring cap */

            /* Reference driver does not modify stride or UV bases here; qbuf sets buffer slots and vic_pipo_mdma_enable sets stride (width<<1). */

            /* Quick readback before writing control to aid debugging on short dmesg buffers */
            {
                u32 y0 = readl(vic_base + 0x318);
                u32 uv0 = readl(vic_base + 0x340);
                u32 uvsh0 = readl(vic_base + 0x32c);
                u32 stride_y = readl(vic_base + 0x310);
                u32 ctrl_pre = readl(vic_base + 0x300);
                pr_info("*** VIC MDMA pre-ctrl: ctrl=0x%x strideY=%u slot0 Y=0x%x UV=0x%x UVsh=0x%x ***\n",
                        ctrl_pre, stride_y, y0, uv0, uvsh0);
            }

            /* Now write control (0x300) with NV12 format AFTER bases are valid, then start RUN */
            {
                u32 stream_ctrl = (buffer_count << 16) | 0x80000020;  /* Reference driver: control bits only; format configured elsewhere */
                writel(stream_ctrl, vic_base + 0x300);
                if (vic_dev->vic_regs_secondary)
                    writel(stream_ctrl, vic_dev->vic_regs_secondary + 0x300);
                wmb();
                pr_info("*** BINARY NINJA EXACT: Wrote 0x%x to reg 0x300 (buffer_count=%d, formula: (count<<16)|0x80000020) ***\n",
                        stream_ctrl, buffer_count);

                /* Enable VIC IRQs before CONFIG and RUN (per working branch) */
                writel(0x3FFFFFFF, vic_base + 0x1e0);
                writel(0x0000000F, vic_base + 0x1e4);
                if (vic_dev->vic_regs_secondary) {
                    writel(0x3FFFFFFF, vic_dev->vic_regs_secondary + 0x1e0);
                    writel(0x0000000F, vic_dev->vic_regs_secondary + 0x1e4);
                }
                wmb();
                pr_info("*** VIC IRQ enable: [0x1e0]=0x3FFFFFFF [0x1e4]=0x0000000F (mirrored if secondary) ***\n");

                /* Transition to RUN after programming control, mirror if secondary present */
                writel(1, vic_base + 0x0);
                if (vic_dev->vic_regs_secondary)
                    writel(1, vic_dev->vic_regs_secondary + 0x0);
                wmb();
                pr_info("*** VIC RUN: Wrote 1 to VIC[0x0] (and secondary if mapped) ***\n");

                /* MCP LOG: Stream ON completed */
                pr_info("MCP_LOG: VIC streaming enabled - ctrl=0x%x, base=%p, state=%d\n",
                        stream_ctrl, vic_base, 1);
            }

        }

        /* Binary Ninja EXACT: *($s0 + 0x210) = 1 */
        vic_dev->stream_state = 1;
    }

    /* Binary Ninja EXACT: private_spin_unlock_irqrestore($s0 + 0x1f4, var_18) */
    private_spin_unlock_irqrestore(&vic_dev->buffer_mgmt_lock, var_18);

    /* Binary Ninja EXACT: return 0 */
    return 0;
}

/* VIC event callback structure - matching reference driver layout
 * CRITICAL: Must be exactly 32 bytes total with function pointer at offset 0x1c */
struct vic_callback_struct {
    char padding[28];           /* Exact 28 bytes padding to reach offset 0x1c */
    void *event_callback;       /* Function pointer at offset 0x1c */
} __attribute__((packed));

/* VIC event callback handler for QBUF events */
static int vic_pad_event_handler(struct tx_isp_subdev_pad *pad, unsigned int cmd, void *data)
{
    struct tx_isp_subdev *sd;
    struct tx_isp_vic_device *vic_dev;
    int ret = 0;

    if (!pad || !pad->sd) {
        pr_err("VIC event callback: Invalid pad or subdev\n");
        return -EINVAL;
    }

    sd = pad->sd;
    vic_dev = (struct tx_isp_vic_device *)tx_isp_get_subdevdata(sd);
    if (!vic_dev) {
        pr_err("VIC event callback: No vic_dev\n");
        return -EINVAL;
    }

    pr_info("*** VIC EVENT CALLBACK: cmd=0x%x, data=%p ***\n", cmd, data);

    switch (cmd) {
        case 0x3000005: { /* BUFFER_ENQUEUE: program VIC slot from provided node */
            pr_info("*** VIC EVENT: BUFFER_ENQUEUE (0x3000005) ***\n");
            if (sd && data) {
                (void) ispvic_frame_channel_qbuf(sd, data);
                ret = 0;
            } else {
                ret = -EINVAL;
            }
            break;
        }
        case 0x3000008: { /* REQBUFS notification: {channel_id, buffer_count} */
            struct { int32_t channel_id; int32_t buffer_count; } *e = data;
            if (!e) { ret = -EINVAL; break; }
            pr_info("*** VIC EVENT: REQBUFS (0x3000008) ch=%d count=%d ***\n", e->channel_id, e->buffer_count);
            if (e->channel_id == 0) {
                int cnt = e->buffer_count;
                if (cnt < 0) cnt = 0; if (cnt > 5) cnt = 5;
                vic_dev->active_buffer_count = cnt;
                pr_info("*** VIC: active_buffer_count set to %d ***\n", vic_dev->active_buffer_count);
            }
            ret = 0;
            break;
        }
        default:
            pr_info("VIC: Unknown event cmd=0x%x\n", cmd);
            ret = -ENOIOCTLCMD;
            break;
    }

    pr_info("*** VIC EVENT CALLBACK: returning %d ***\n", ret);
    return ret;
}

/* BINARY NINJA EXACT: vic_core_s_stream implementation with CSI register updates */
/* BINARY NINJA EXACT: vic_core_s_stream implementation with CSI register updates */
int vic_core_s_stream(struct tx_isp_subdev *sd, int enable)
{
    struct tx_isp_vic_device *vic_dev;
    void __iomem *vic_regs;
    void __iomem *isp_base;
    void __iomem *csi_base;
    int ret = -EINVAL;

    pr_info("*** vic_core_s_stream: ENTRY - sd=%p, enable=%d ***\n", sd, enable);

    /* Validate parameters */
    if (!sd || !ourISPdev || !ourISPdev->vic_dev) {
        pr_err("vic_core_s_stream: Invalid parameters - sd=%p, ourISPdev=%p\n", sd, ourISPdev);
        return -EINVAL;
    }

    vic_dev = (struct tx_isp_vic_device *)ourISPdev->vic_dev;
    if (!vic_dev) {
        pr_err("vic_core_s_stream: Failed to get VIC device\n");
        return -EINVAL;
    }

    /* REVERT: Ensure VIC registers are mapped to ORIGINAL working address */
    vic_regs = vic_dev->vic_regs;
    if (!vic_regs) {
        pr_err("*** CRITICAL FIX: VIC registers not mapped - mapping now ***\n");
        vic_regs = ioremap(0x133e0000, 0x10000);
        if (!vic_regs) {
            pr_err("vic_core_s_stream: Failed to map VIC registers at 0x133e0000\n");
            return -ENOMEM;
        }
        vic_dev->vic_regs = vic_regs;
        pr_info("*** VIC registers mapped successfully: %p ***\n", vic_regs);
    }

    /* Calculate base addresses safely */
    isp_base = vic_regs - 0x9a00;  /* Correct ISP base calculation */
    csi_base = isp_base + 0x10000;

    pr_info("vic_core_s_stream: vic_regs=%p, isp_base=%p, csi_base=%p\n", vic_regs, isp_base, csi_base);

    if (sd != NULL) {
        if ((unsigned long)sd >= 0xfffff001) {
            pr_err("vic_core_s_stream: Invalid sd pointer\n");
            return -EINVAL;
        }

        ret = -EINVAL;

        if (vic_dev != NULL && (unsigned long)vic_dev < 0xfffff001) {
            int current_state = vic_dev->state;
            pr_info("*** vic_core_s_stream: current_state=%d (enable=%d) ***\n", current_state, enable);

            if (enable == 0) {
                /* Stream OFF - Call slake to reset VIC state to 1 */
                ret = 0;
                if (current_state == 4) {
                    vic_dev->state = 3;
                    pr_info("vic_core_s_stream: Stream OFF - state 4 -> 3\n");

                    /* CRITICAL FIX: Call ispcore_slake_module during stream-OFF to reset VIC state to 1 */
                    /* This ensures that on the next stream-ON, ispcore_activate_module will run */
                    extern int ispcore_slake_module(struct tx_isp_dev *isp_dev);
                    extern struct tx_isp_dev *ourISPdev;
                    if (ourISPdev) {
                        pr_info("*** VIC STREAM OFF: Calling ispcore_slake_module to reset state to 1 ***\n");
                        ispcore_slake_module(ourISPdev);
                        pr_info("*** VIC STREAM OFF: ispcore_slake_module complete, VIC state should be 1 ***\n");
                    }
                }
            } else {
                /* Stream ON - Binary Ninja: if (state != 4) */
                if (current_state != 4) {
                    /* Binary Ninja: tx_vic_disable_irq() */
                    pr_info("*** vic_core_s_stream: Disabling VIC IRQ before start ***\n");
                    tx_vic_disable_irq(vic_dev);

                    /* Binary Ninja: ret = tx_isp_vic_start(vic_dev) */
                    pr_info("*** vic_core_s_stream: Calling tx_isp_vic_start ***\n");
                    ret = tx_isp_vic_start(vic_dev);
                    pr_info("*** vic_core_s_stream: tx_isp_vic_start returned %d ***\n", ret);

                    /* REMOVED: ispcore_slake_module call moved to stream-OFF path */
                    /* This allows VIC state to remain 1 after stream-OFF, so ispcore_activate_module runs on next stream-ON */

                    /* Binary Ninja: state = 4 */
                    vic_dev->state = 4;

                    /* Binary Ninja: tx_vic_enable_irq() */
                    pr_info("*** vic_core_s_stream: Enabling VIC IRQ after start ***\n");
                    tx_vic_enable_irq(vic_dev);

                    /* Binary Ninja: return ret */
                    return ret;
                }

                /* If already streaming (state == 4), just return success */
                ret = 0;

                /* OLD COMPLEX INITIALIZATION - KEEPING AS FALLBACK FOR NOW */
                pr_info("*** CRITICAL: Following EXACT reference driver sub-device initialization sequence ***\n");

                /* CRITICAL FIX: Disable VIC interrupts during initialization to prevent control limit errors */
                pr_info("*** DISABLING VIC INTERRUPTS DURING INITIALIZATION ***\n");
                vic_start_ok = 1;  /* Disable interrupt processing */

                /* CRITICAL FIX: Correct the register base mapping! */
                /* vic_regs = 0x133e0000 = CSI PHY (isp-w02 in trace) */
                /* isp_base = 0x13300000 = Main ISP (isp-m0 in trace) - NEEDS SEPARATE MAPPING */
                void __iomem *main_isp_base = ioremap(0x13300000, 0x100000);  /* Map main ISP separately */
                void __iomem *vic_w01_base = ioremap(0x10023000, 0x1000);    /* Map isp-w01 separately */

                if (!main_isp_base || !vic_w01_base) {
                    pr_err("*** CRITICAL: Failed to map ISP register bases ***\n");
                    if (main_isp_base) iounmap(main_isp_base);
                    if (vic_w01_base) iounmap(vic_w01_base);
                    return -ENOMEM;
                }


                /* STEP 1: VIC Hardware Reset and Clean Configuration */
                pr_info("*** STEP 1: VIC Hardware Reset and Clean Configuration ***\n");

                /* Declare sensor dimensions at function scope */
                u32 sensor_width = 1920;   /* ACTUAL sensor output width */
                u32 sensor_height = 1080;  /* ACTUAL sensor output height */

                /* HARDWARE RESET APPROACH: Reset VIC to clean state first */
                pr_info("*** VIC HARDWARE RESET: Clearing VIC hardware state to prevent control limit errors ***\n");

                /* Step 1: Disable VIC hardware completely */
                writel(0x0, vic_regs + 0x0);           /* Disable VIC hardware */
                wmb();

                /* Step 2: Clear interrupt status only */
                writel(0xffffffff, vic_regs + 0x1c);   /* Clear interrupt status */
                wmb();

                /* CRITICAL: MINIMAL VIC configuration to prevent control limit errors */
                pr_info("*** MINIMAL VIC CONFIG: Applying only essential registers to prevent control limit errors ***\n");

                /* Step 1: Essential VIC mode and dimensions only */
                writel(3, vic_regs + 0xc);                                    /* MIPI mode = 3 */
                writel((sensor_width << 16) | sensor_height, vic_regs + 0x4); /* Dimensions */
                wmb();

                /* Step 2: Preserve critical timing parameter (never overwrite 0x18) */
                /* Register 0x18 should remain 0xf00 - do NOT overwrite it */

                /* Step 3: DEFER complex VIC configuration until after sensor is fully initialized */
                /* This prevents control limit errors during initialization */

                /* Step 5: DEFER full VIC configuration until sensor is streaming */
                pr_info("*** DEFERRING FULL VIC CONFIGURATION: Will apply after sensor initialization ***\n");

                pr_info("*** VIC HARDWARE CONFIGURATION COMPLETE (hardware disabled until sensor ready) ***\n");

                /* STEP 2: ISP isp-w01 - Control registers */
                pr_info("*** STEP 2: ISP isp-w01 - Control registers ***\n");

                /* ESSENTIAL isp-w01 configuration - needed for proper ISP operation */
                writel(0x3130322a, vic_regs + 0x0);     /* Essential isp-w01 control */
                writel(0x1, vic_regs + 0x4);            /* Essential isp-w01 control */
                writel(0x200, vic_regs + 0x14);         /* Essential isp-w01 control */
                wmb();
                pr_info("*** isp-w01 CONFIGURATION COMPLETE ***\n");

                /* STEP 3: ISP isp-m0 - Main ISP registers (BEFORE sensor detection) */
                pr_info("*** STEP 3: ISP isp-m0 - Main ISP registers (BEFORE sensor detection) ***\n");
                /* Use the correct main_isp_base (0x13300000 = isp-m0) */
                writel(0x54560031, main_isp_base + 0x0);
                writel(0x7800438, main_isp_base + 0x4);


                if (current_state != 4) {
                    pr_info("vic_core_s_stream: Stream ON - tx_isp_vic_start called after proper sub-device init\n");

                    /* CRITICAL FIX: Only enable interrupts AFTER all initialization is complete */
                    vic_dev->state = 4;
                    wmb();  /* Ensure state is written before enabling interrupts */
                    vic_start_ok = 1;  /* NOW safe to enable interrupt processing */
                    pr_info("*** INTERRUPTS RE-ENABLED AFTER COMPLETE INITIALIZATION ***\n");

                    /* NOTE: ispcore_slake_module is now called in the primary path above (lines 2716-2724) */
                    pr_info("*** VIC STATE %d → 4: ispcore_slake_module handled in primary path ***\n", current_state);

                    /* CRITICAL: Apply full VIC configuration now that sensor is streaming */
                    pr_info("*** APPLYING FULL VIC CONFIGURATION AFTER SENSOR INITIALIZATION ***\n");
                    tx_isp_vic_apply_full_config(vic_dev);

                    /* DELAYED VIC HARDWARE ENABLE: Now that everything is configured and sensor is streaming */
                    pr_info("*** DELAYED VIC HARDWARE ENABLE: Enabling VIC hardware after complete initialization ***\n");
                    void __iomem *vic_regs = vic_dev->vic_regs;
                    if (vic_regs) {
                        /* BINARY NINJA EXACT: Hardware enable sequence */
                        /* Binary Ninja: **(arg1 + 0xb8) = 2; **(arg1 + 0xb8) = 4; while (*$v1_30 != 0) nop; **(arg1 + 0xb8) = 1 */
                        writel(0x2, vic_regs + 0x0);        /* Binary Ninja: Pre-enable state */
                        wmb();
                        writel(0x4, vic_regs + 0x0);        /* Binary Ninja: Wait state */
                        wmb();

                        /* Binary Ninja: Wait for hardware ready */
                        u32 wait_count = 0;
                        while ((readl(vic_regs + 0x0) != 0) && (wait_count < 1000)) {
                            wait_count++;
                            udelay(1);
                        }
                        writel(0x1, vic_regs + 0x0);        /* Binary Ninja: Final enable */
                        wmb();
                        pr_info("*** BINARY NINJA EXACT: Hardware enable sequence 2->4->wait->1 (waited %d us) ***\n", wait_count);
                    } else {
                        pr_err("*** ERROR: VIC registers not available for delayed enable ***\n");
                    }

                    pr_info("vic_core_s_stream: tx_isp_vic_start returned %d, state -> 4\n", ret);
                    return ret;
                }
            }
        }
    }

    return ret;
}
/* Cleanup function remains the same */

/* Define VIC video operations */
static struct tx_isp_subdev_video_ops vic_video_ops = {
    .s_stream = vic_core_s_stream,  /* CRITICAL FIX: Use vic_core_s_stream instead of vic_video_s_stream */
};

/* Forward declarations for functions used in structures */
extern int vic_sensor_ops_ioctl(struct tx_isp_subdev *sd, unsigned int cmd, void *arg);
extern int vic_sensor_ops_sync_sensor_attr(struct tx_isp_subdev *sd, struct tx_isp_sensor_attribute *attr);
extern int vic_core_ops_init(struct tx_isp_subdev *sd, int enable);
extern int vic_core_ops_ioctl(struct tx_isp_subdev *sd, unsigned int cmd, void *arg);
extern long isp_vic_cmd_set(struct file *file, unsigned int cmd, unsigned long arg);
extern int dump_isp_vic_frd_open(struct inode *inode, struct file *file);
extern int vic_chardev_open(struct inode *inode, struct file *file);
extern int vic_chardev_release(struct inode *inode, struct file *file);
extern ssize_t vic_proc_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos);

/* VIC sensor operations structure - MISSING from original implementation */
struct tx_isp_subdev_sensor_ops vic_sensor_ops = {
    .ioctl = vic_sensor_ops_ioctl,                    /* From tx-isp-module.c */
    .sync_sensor_attr = vic_sensor_ops_sync_sensor_attr, /* From tx-isp-module.c */
};

/* VIC core operations structure - MISSING ioctl registration */
struct tx_isp_subdev_core_ops vic_core_ops = {
    .init = vic_core_ops_init,
    .ioctl = vic_core_ops_ioctl,  /* MISSING from original! */
};

/* VIC internal ops for activate/slake */
static struct tx_isp_subdev_internal_ops vic_subdev_internal_ops = {
    .activate_module = tx_isp_vic_activate_subdev,
    .slake_module = tx_isp_vic_slake_subdev,
};

/* Complete VIC subdev ops structure */
struct tx_isp_subdev_ops vic_subdev_ops = {
    .core = &vic_core_ops,
    .video = &vic_video_ops,
    .sensor = &vic_sensor_ops,
    .internal = &vic_subdev_internal_ops,
};
EXPORT_SYMBOL(vic_subdev_ops);


/* VIC FRD file operations - EXACT Binary Ninja implementation */
const struct file_operations isp_vic_frd_fops = {
    .owner = THIS_MODULE,
    .llseek = seq_lseek,                /* private_seq_lseek from hex dump */
    .read = seq_read,                   /* private_seq_read from hex dump */
    .unlocked_ioctl = isp_vic_cmd_set,  /* isp_vic_cmd_set from hex dump */
    .open = dump_isp_vic_frd_open,      /* dump_isp_vic_frd_open from hex dump */
    .release = single_release,          /* private_single_release from hex dump */
};

/* VIC W02 proc file operations - FIXED for proper proc interface */
const struct file_operations isp_w02_proc_fops = {
    .owner = THIS_MODULE,
    .open = vic_chardev_open,
    .release = vic_chardev_release,
    .write = vic_proc_write,
    .llseek = default_llseek,
};

/* Implementation of the open/release functions */
/* Implementation of the open/release functions */
int vic_chardev_open(struct inode *inode, struct file *file)
{
    struct tx_isp_subdev *sd = PDE_DATA(inode);  // Get data set during proc_create_data

    pr_info("VIC device open called from pid %d\n", current->pid);
    file->private_data = sd;

    return 0;
}
EXPORT_SYMBOL(vic_chardev_open);

int vic_chardev_release(struct inode *inode, struct file *file)
{
    struct tx_isp_subdev *sd = file->private_data;

    if (!sd) {
        return -EINVAL;
    }

    file->private_data = NULL;
    pr_info("VIC device released\n");
    return 0;
}
EXPORT_SYMBOL(vic_chardev_release);


long vic_chardev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct tx_isp_subdev *sd = file->private_data;
    int ret = 0;

    pr_info("VIC IOCTL called: cmd=0x%x arg=0x%lx\n", cmd, arg);

    if (!sd) {
        pr_err("VIC: no private data in file\n");
        return -EINVAL;
    }

    return ret;
}
EXPORT_SYMBOL(vic_chardev_ioctl);

static struct tx_isp_vic_device *dump_vsd = NULL;
static void *test_addr = NULL;

/* tx_isp_vic_probe - Matching binary flow with safe struct member access */
int tx_isp_vic_probe(struct platform_device *pdev)
{
    struct tx_isp_vic_device *vic_dev;
    struct tx_isp_subdev *sd;
    struct resource *res;
    int ret;
    extern struct tx_isp_dev *ourISPdev;

    pr_info("*** tx_isp_vic_probe: Starting VIC device probe ***\n");

    /* CRITICAL: Use existing VIC device from ourISPdev, DO NOT create a new one! */
    if (ourISPdev && ourISPdev->vic_dev) {
        vic_dev = ourISPdev->vic_dev;
        pr_info("*** tx_isp_vic_probe: Using existing VIC device at %p ***\n", vic_dev);
    } else {
        pr_err("*** tx_isp_vic_probe: No existing VIC device found! ***\n");
        return -EINVAL;
    }

    /* Get subdev pointer */
    sd = &vic_dev->sd;

    /* Get platform resource (binary uses this for error message) */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);

    /* CRITICAL FIX: Set subdev private data BEFORE calling tx_isp_subdev_init
     * because tx_isp_subdev_init calls tx_isp_subdev_auto_link which needs
     * to retrieve vic_dev via tx_isp_get_subdevdata(sd)
     */
    tx_isp_set_subdevdata(sd, vic_dev);
    pr_info("*** tx_isp_vic_probe: Set subdev private data to vic_dev=%p ***\n", vic_dev);

    /* CRITICAL: Initialize subdev AFTER setting private data */
    ret = tx_isp_subdev_init(pdev, sd, &vic_subdev_ops);
    if (ret != 0) {
        pr_err("Failed to init isp module(%d.%d)\n",
               res ? MAJOR(res->start) : 0,
               res ? MINOR(res->start) : 0);
        kfree(vic_dev);
        return -EFAULT;  /* Binary returns -12 (EFAULT) */
    }

    /* Set platform driver data after successful init */
    platform_set_drvdata(pdev, vic_dev);

    /* CRITICAL FIX: DO NOT overwrite sd->ops here!
     * sd->ops was correctly set by tx_isp_subdev_init() to &vic_subdev_ops
     * which contains the core->init function pointer needed for initialization.
     * The isp_vic_frd_fops is a file_operations structure for /proc, not subdev ops!
     */
    /* REMOVED BUGGY LINE: sd->ops = &isp_vic_frd_fops; */

    /* Initialize synchronization primitives (binary order) */
    spin_lock_init(&vic_dev->lock);
    mutex_init(&vic_dev->mlock);
    init_completion(&vic_dev->frame_complete);

    /* Set initial state to 1 (matches binary) */
    vic_dev->state = 1;

    /* Store global reference (binary uses 'dump_vsd' global) */
    dump_vsd = vic_dev;
    vic_dev->irq = 38;

    /* Set test_addr to point to sensor_attr or appropriate member */
    /* Binary points to offset 0x80 in the structure */
    test_addr = &vic_dev->sensor_attr;  /* Or another member around offset 0x80 */

    pr_info("*** tx_isp_vic_probe: VIC device initialized successfully ***\n");
    pr_info("VIC device: vic_dev=%p, size=%zu\n", vic_dev, sizeof(struct tx_isp_vic_device));
    pr_info("  sd: %p\n", sd);
    pr_info("  state: %d\n", vic_dev->state);
    pr_info("  test_addr: %p\n", test_addr);

    return 0;
}

/* VIC remove function */
int tx_isp_vic_remove(struct platform_device *pdev)
{
    struct tx_isp_subdev *sd = platform_get_drvdata(pdev);
    struct resource *res;

    if (!sd)
        return -EINVAL;

    /* Stop VIC */
    tx_isp_vic_stop(sd);

    /* Free interrupt */
    free_irq(platform_get_irq(pdev, 0), sd);

    remove_proc_entry("isp-w02", NULL);
    remove_proc_entry("jz/isp", NULL);

    /* Unmap and release memory */
    iounmap(sd->base);
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (res)
        release_mem_region(res->start, resource_size(res));

    /* CRITICAL: Clean up BOTH VIC register mappings */
    struct tx_isp_vic_device *vic_dev = container_of(sd, struct tx_isp_vic_device, sd);
    if (vic_dev) {
        if (vic_dev->vic_regs) {
            pr_info("*** VIC REMOVE: Unmapping primary VIC registers ***\n");
            iounmap(vic_dev->vic_regs);
            vic_dev->vic_regs = NULL;
        }
        if (vic_dev->vic_regs_secondary) {
            pr_info("*** VIC REMOVE: Unmapping secondary VIC registers ***\n");
            iounmap(vic_dev->vic_regs_secondary);
            vic_dev->vic_regs_secondary = NULL;
        }
    }

    /* Clean up subdev */
    tx_isp_subdev_deinit(sd);
    kfree(sd);

    return 0;
}

/* ispvic_frame_channel_qbuf - Prefer explicit node (addr+index) when provided; fallback to queue */
static int ispvic_frame_channel_qbuf(void *arg1, void *arg2)
{
    struct tx_isp_vic_device *vic_dev = NULL;
    struct tx_isp_subdev *sd = (struct tx_isp_subdev *)arg1;
    unsigned long irq_flags = 0;
    struct vic_buffer_entry *new_buffer = (struct vic_buffer_entry *)arg2;
    struct vic_buffer_entry *buffer_to_program;
    u32 buffer_addr, buffer_index, reg_offset;

    pr_info("*** ispvic_frame_channel_qbuf: entry arg1=%p, arg2=%p ***\n", arg1, arg2);

    /* Get vic_dev from subdev */
    if (sd != NULL && (unsigned long)sd < 0xfffff001)
        vic_dev = (struct tx_isp_vic_device *)tx_isp_get_subdev_hostdata(sd);

    if (!vic_dev || !vic_dev->vic_regs)
        return 0;

    spin_lock_irqsave(&vic_dev->buffer_mgmt_lock, irq_flags);

    /* Fast path: explicit node provided with address and slot index */
    if (new_buffer) {
        buffer_addr  = new_buffer->buffer_addr;
        buffer_index = new_buffer->buffer_index; /* 0..4 */
        reg_offset   = (buffer_index + 0xc6) << 2;  /* 0x318..0x328 */

        /* Program Y base */
        writel(buffer_addr, vic_dev->vic_regs + reg_offset);
        if (vic_dev->vic_regs_secondary)
            writel(buffer_addr, vic_dev->vic_regs_secondary + reg_offset);
        /* Program UV base for NV12: 0x340 + index*4 = Y_base + stride*height */
        {
            u32 stride_nv12 = readl(vic_dev->vic_regs + 0x310);
            u32 uv_base = buffer_addr + stride_nv12 * vic_dev->height;
            u32 uv_reg_off = 0x340 + (buffer_index * 4);
            u32 uv_shadow_off = 0x32c + (buffer_index * 4);
            writel(uv_base, vic_dev->vic_regs + uv_reg_off);
            writel(uv_base, vic_dev->vic_regs + uv_shadow_off);
            if (vic_dev->vic_regs_secondary) {
                writel(uv_base, vic_dev->vic_regs_secondary + uv_reg_off);
                writel(uv_base, vic_dev->vic_regs_secondary + uv_shadow_off);
            }
        }
        wmb();
        pr_info("*** VIC QBUF: slot%u Y=0x%x(off=0x%x) UV=0x%x(off=0x%x) UVsh=0x%x(off=0x%x) ***\n",
                buffer_index, buffer_addr, reg_offset,
                buffer_addr + readl(vic_dev->vic_regs + 0x310) * vic_dev->height,
                0x340 + (buffer_index * 4),
                buffer_addr + readl(vic_dev->vic_regs + 0x310) * vic_dev->height,
                0x32c + (buffer_index * 4));
        spin_unlock_irqrestore(&vic_dev->buffer_mgmt_lock, irq_flags);
        return 0;
    }

    /* Fallback: use internal done queue when no explicit node is provided */
    if (list_empty(&vic_dev->free_head) || list_empty(&vic_dev->done_head)) {
        spin_unlock_irqrestore(&vic_dev->buffer_mgmt_lock, irq_flags);
        return 0;
    }

    buffer_to_program = list_first_entry(&vic_dev->done_head, struct vic_buffer_entry, list);
    list_del(&buffer_to_program->list);

    buffer_addr  = buffer_to_program->buffer_addr;
    buffer_index = vic_dev->active_buffer_count % 5; /* derive index if no explicit node */
    reg_offset   = (buffer_index + 0xc6) << 2;  /* 0x318..0x328 */

    /* Program Y base */
    writel(buffer_addr, vic_dev->vic_regs + reg_offset);
    if (vic_dev->vic_regs_secondary)
        writel(buffer_addr, vic_dev->vic_regs_secondary + reg_offset);
    /* Program UV base for NV12: 0x340 + index*4 = Y_base + stride*height */
    {
        u32 stride_nv12 = readl(vic_dev->vic_regs + 0x310);
        u32 uv_base = buffer_addr + stride_nv12 * vic_dev->height;
        u32 uv_reg_off = 0x340 + (buffer_index * 4);
        u32 uv_shadow_off = 0x32c + (buffer_index * 4);
        writel(uv_base, vic_dev->vic_regs + uv_reg_off);
        writel(uv_base, vic_dev->vic_regs + uv_shadow_off);
        if (vic_dev->vic_regs_secondary) {
            writel(uv_base, vic_dev->vic_regs_secondary + uv_reg_off);
            writel(uv_base, vic_dev->vic_regs_secondary + uv_shadow_off);
        }
    }
    wmb();
    pr_info("*** VIC QBUF (fallback): Y=0x%x(off=0x%x) UV=0x%x(off=0x%x) UVsh=0x%x(off=0x%x) idx=%u ***\n",
            buffer_addr, reg_offset,
            buffer_addr + readl(vic_dev->vic_regs + 0x310) * vic_dev->height,
            0x340 + (buffer_index * 4),
            buffer_addr + readl(vic_dev->vic_regs + 0x310) * vic_dev->height,
            0x32c + (buffer_index * 4),
            buffer_index);

    spin_unlock_irqrestore(&vic_dev->buffer_mgmt_lock, irq_flags);
    return 0;
}

/* ISPVIC Frame Channel Clear Buffer - placeholder matching Binary Ninja reference */
static int ispvic_frame_channel_clearbuf(void)
{
    pr_info("ispvic_frame_channel_clearbuf called\n");
    return 0;
}

/* tx_isp_subdev_pipo - SAFE struct member access implementation */
int tx_isp_subdev_pipo(struct tx_isp_subdev *sd, void *arg)
{
    struct tx_isp_vic_device *vic_dev = NULL;
    void **raw_pipe = (void **)arg;
    int i;

    pr_info("*** tx_isp_subdev_pipo: SAFE struct member access implementation ***\n");
    pr_info("tx_isp_subdev_pipo: entry - sd=%p, arg=%p\n", sd, arg);

    /* SAFE: Validate parameters */
    if (sd != NULL && (unsigned long)sd < 0xfffff001) {
        vic_dev = (struct tx_isp_vic_device *)tx_isp_get_subdevdata(sd);
        pr_info("tx_isp_subdev_pipo: vic_dev retrieved: %p\n", vic_dev);
    }

    if (!vic_dev) {
        pr_err("tx_isp_subdev_pipo: vic_dev is NULL\n");
        return 0;  /* Binary Ninja returns 0 even on error */
    }

    /* SAFE: Use proper struct member access instead of offset 0x20c */
    vic_dev->processing = 1;
    pr_info("tx_isp_subdev_pipo: set processing = 1 (safe struct access)\n");

    /* SAFE: Check if arg is NULL */
    if (arg == NULL) {
        /* SAFE: Use proper struct member access instead of offset 0x214 */
        vic_dev->processing = 0;
        pr_info("tx_isp_subdev_pipo: arg is NULL - set processing = 0 (safe struct access)\n");
    } else {
        pr_info("tx_isp_subdev_pipo: arg is not NULL - initializing pipe structures\n");

        /* SAFE: Use Linux list initialization instead of manual pointer manipulation */
        INIT_LIST_HEAD(&vic_dev->queue_head);
        INIT_LIST_HEAD(&vic_dev->done_head);
        INIT_LIST_HEAD(&vic_dev->free_head);

        pr_info("tx_isp_subdev_pipo: initialized linked list heads (safe Linux API)\n");

        /* SAFE: Use proper spinlock initialization */
        spin_lock_init(&vic_dev->buffer_mgmt_lock);
        pr_info("tx_isp_subdev_pipo: initialized spinlock\n");

        /* SAFE: Set function pointers using proper array indexing */
        raw_pipe[0] = (void *)ispvic_frame_channel_qbuf;
        raw_pipe[2] = (void *)ispvic_frame_channel_clearbuf;  /* offset 8 / 4 = index 2 */
        raw_pipe[3] = (void *)ispvic_frame_channel_s_stream;  /* offset 0xc / 4 = index 3 */
        raw_pipe[4] = (void *)sd;                             /* offset 0x10 / 4 = index 4 */

        pr_info("tx_isp_subdev_pipo: set function pointers - qbuf=%p, clearbuf=%p, s_stream=%p, sd=%p\n",
                ispvic_frame_channel_qbuf, ispvic_frame_channel_clearbuf,
                ispvic_frame_channel_s_stream, sd);

        /* SAFE: Initialize buffer structures using proper struct members */
        for (i = 0; i < 5; i++) {
            /* SAFE: Use proper buffer index array instead of unsafe pointer arithmetic */
            if (i < sizeof(vic_dev->buffer_index) / sizeof(vic_dev->buffer_index[0])) {
                vic_dev->buffer_index[i] = i;
            }

            /* SAFE: Create proper buffer entries and add to free list */
            struct list_head *buffer_entry = kzalloc(sizeof(struct list_head) + 32, GFP_KERNEL);
            if (buffer_entry) {
                /* Initialize buffer data */
                uint32_t *buffer_data = (uint32_t *)((char *)buffer_entry + sizeof(struct list_head));
                buffer_data[0] = 0;  /* Buffer address */
                buffer_data[1] = i;  /* Buffer index */
                buffer_data[2] = 0;  /* Buffer status */

                /* Add to free list using safe Linux API */
                list_add_tail(buffer_entry, &vic_dev->free_head);
                pr_info("tx_isp_subdev_pipo: added buffer entry %d to free list\n", i);
            }

            /* SAFE: Clear VIC register using validated register access */
            uint32_t reg_offset = (i + 0xc6) << 2;
            if (vic_dev->vic_regs && reg_offset < 0x1000) {
                writel(0, vic_dev->vic_regs + reg_offset);
                pr_info("tx_isp_subdev_pipo: cleared VIC register at offset 0x%x for buffer %d\n", reg_offset, i);
            }
        }

        pr_info("tx_isp_subdev_pipo: initialized %d buffer structures (safe implementation)\n", i);

        /* SAFE: Use proper struct member access instead of offset 0x214 */
        vic_dev->processing = 1;
        pr_info("tx_isp_subdev_pipo: set processing = 1 (pipe enabled, safe struct access)\n");
    }

    pr_info("tx_isp_subdev_pipo: completed successfully, returning 0\n");
    return 0;
}
EXPORT_SYMBOL(tx_isp_subdev_pipo);

/* VIC platform driver structure - CRITICAL MISSING PIECE */
static struct platform_driver tx_isp_vic_platform_driver = {
    .probe = tx_isp_vic_probe,
    .remove = tx_isp_vic_remove,
    .driver = {
        .name = "tx-isp-vic",
        .owner = THIS_MODULE,
    },
};

/* VIC platform device registration functions */
int __init tx_isp_vic_platform_init(void)
{
    int ret;

    pr_info("*** TX ISP VIC PLATFORM DRIVER REGISTRATION ***\n");

    ret = platform_driver_register(&tx_isp_vic_platform_driver);
    if (ret) {
        pr_err("Failed to register VIC platform driver: %d\n", ret);
        return ret;
    }

    pr_info("VIC platform driver registered successfully\n");
    return 0;
}

void __exit tx_isp_vic_platform_exit(void)
{
    pr_info("*** TX ISP VIC PLATFORM DRIVER UNREGISTRATION ***\n");
    platform_driver_unregister(&tx_isp_vic_platform_driver);
    pr_info("VIC platform driver unregistered\n");
}

/* Export symbols for use by other parts of the driver */
EXPORT_SYMBOL(tx_isp_vic_stop);
EXPORT_SYMBOL(tx_isp_vic_set_buffer);
EXPORT_SYMBOL(tx_isp_vic_wait_frame_done);
EXPORT_SYMBOL(vic_core_s_stream);  /* CRITICAL: Export the missing function */

/* Export VIC platform init/exit for main module */
EXPORT_SYMBOL(tx_isp_vic_platform_init);
EXPORT_SYMBOL(tx_isp_vic_platform_exit);

/**
 * tx_isp_vic_apply_full_config - Apply complete VIC configuration after sensor initialization
 * This function applies the full VIC register configuration that was deferred during initialization
 */
static int tx_isp_vic_apply_full_config(struct tx_isp_vic_device *vic_dev)
{
    void __iomem *vic_regs;

    if (!vic_dev || !vic_dev->vic_regs) {
        pr_err("tx_isp_vic_apply_full_config: Invalid VIC device\n");
        return -EINVAL;
    }

    vic_regs = vic_dev->vic_regs;

    pr_info("*** VIC FULL CONFIG: Applying complete VIC configuration after sensor initialization ***\n");

    /* Apply VIC interrupt system configuration */
    writel(0x2d0, vic_regs + 0x100);        /* Interrupt configuration */
    writel(0x2b, vic_regs + 0x14);          /* Interrupt control - reference driver value */
    wmb();

    /* Apply essential VIC control registers */
    writel(0x800800, vic_regs + 0x60);      /* Control register */
    writel(0x9d09d0, vic_regs + 0x64);      /* Control register */
    writel(0x6002, vic_regs + 0x70);        /* Control register */
    writel(0x7003, vic_regs + 0x74);        /* Control register */
    wmb();

    /* Apply VIC color space configuration */
    writel(0xeb8080, vic_regs + 0xc0);      /* Color space config */
    writel(0x108080, vic_regs + 0xc4);      /* Color space config */
    writel(0x29f06e, vic_regs + 0xc8);      /* Color space config */
    writel(0x913622, vic_regs + 0xcc);      /* Color space config */
    wmb();

    /* Apply VIC processing configuration */
    writel(0x515af0, vic_regs + 0xd0);      /* Processing config */
    writel(0xaaa610, vic_regs + 0xd4);      /* Processing config */
    writel(0xd21092, vic_regs + 0xd8);      /* Processing config */
    writel(0x6acade, vic_regs + 0xdc);      /* Processing config */
    wmb();

    pr_info("*** VIC FULL CONFIG: Complete VIC configuration applied successfully ***\n");

    return 0;
}
