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

#include "include/tx_isp.h"
#include "include/tx_isp_core.h"
#include "include/tx-isp-debug.h"
#include "include/tx_isp_sysfs.h"
#include "include/tx_isp_vic.h"
#include "include/tx_isp_csi.h"
#include "include/tx_isp_vin.h"
#include "include/tx_isp_tuning.h"
#include "include/tx_isp_subdev_helpers.h"
#include "include/tx-isp-device.h"
#include "include/tx-libimp.h"
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/jiffies.h>

#include <linux/videodev2.h>

int vic_core_s_stream(struct tx_isp_subdev *sd, int enable);
int vic_video_s_stream(struct tx_isp_subdev *sd, int enable);
extern struct tx_isp_dev *ourISPdev;
uint32_t vic_start_ok = 0;  /* Global VIC interrupt enable flag definition */

/*
 * REMOVED: All VIC_RAW_*_OFFSET macros.
 * These were hardcoded byte offsets into the embedded tx_isp_subdev struct
 * that broke whenever the subdev layout changed (e.g. adding submods[16]).
 * All accessors now use proper struct member access via vic_dev->field.
 */

static inline bool vic_pixfmt_is_semiplanar_420(u32 pixfmt)
{
    return pixfmt == V4L2_PIX_FMT_NV12 || pixfmt == V4L2_PIX_FMT_NV21;
}

static inline u32 vic_mdma_stride_resolve(struct tx_isp_vic_device *vic_dev,
                                         u32 width, u32 pixfmt)
{
    u32 stride = vic_dev ? vic_dev->stride : 0;
	u32 min_stride = width << 1;

	(void)pixfmt;

    if (stride >= min_stride)
        return stride;

    return min_stride;
}

static inline u32 vic_mdma_uv_offset_resolve(struct tx_isp_vic_device *vic_dev,
					     const struct vic_buffer_entry *buffer,
					     u32 pixfmt)
{
	u32 buffer_length = buffer ? buffer->buffer_length : 0;
	u32 width;
	u32 height;

	if (!(pixfmt == 0 || vic_pixfmt_is_semiplanar_420(pixfmt)))
		return 0;

	if (buffer_length >= 3)
		return (buffer_length * 2) / 3;

	if (!vic_dev)
		return 0;

	width = vic_dev->width;
	height = vic_dev->height;
	if (!width || !height)
		return 0;

	return width * ALIGN(height, 16);
}

static int vic_resolve_irq_number(struct tx_isp_vic_device *vic_dev)
{
    int irq = 0;

    if (!vic_dev)
        return 0;

    if (vic_dev->sd_irq_info.irq > 0 && vic_dev->sd_irq_info.irq < 1024)
        return vic_dev->sd_irq_info.irq;

    if (ourISPdev && ourISPdev->isp_irq2 > 0)
        return ourISPdev->isp_irq2;

    if (vic_dev->irq > 0)
        return vic_dev->irq;
    if (vic_dev->irq_number > 0)
        return vic_dev->irq_number;

    if (vic_dev->sd.module.dev)
        irq = platform_get_irq(to_platform_device(vic_dev->sd.module.dev), 0);

    if (irq > 0)
        return irq;

    /* The VIC shared line is isp-w02 / IRQ 38. */
    return 38;
}

static inline void vic_raw_irq_slot_seed(struct tx_isp_vic_device *vic_dev, int irq)
{
    if (!vic_dev)
        return;

    if (irq <= 0)
        irq = vic_resolve_irq_number(vic_dev);

    /* Do not overwrite the OEM-owned raw +0x80 IRQ slot. Several runtime
     * paths treat that field as a pointer-bearing object, so writing a bare
     * integer IRQ there causes rollback-time crashes.
     */
    vic_dev->irq = irq;
    vic_dev->irq_number = irq;
}

static inline void vic_raw_regs_set(struct tx_isp_vic_device *vic_dev, void __iomem *regs)
{
    if (!vic_dev)
        return;
    vic_dev->sd.base = regs;
}

static inline void __iomem *vic_raw_regs_get(struct tx_isp_vic_device *vic_dev)
{
    if (!vic_dev)
        return NULL;
    return vic_dev->sd.base;
}

static inline int vic_regs_is_secondary(struct tx_isp_vic_device *vic_dev,
                                        void __iomem *regs)
{
    struct tx_isp_dev *isp_dev;

    if (!vic_dev || !regs)
        return 0;

    isp_dev = ourISPdev;

    if (regs == vic_dev->vic_regs_secondary)
        return 1;
    if (isp_dev && regs == isp_dev->vic_regs2)
        return 1;
    if (ourISPdev && regs == ourISPdev->vic_regs2)
        return 1;

    return 0;
}

static inline void __iomem *vic_primary_regs_resolve(struct tx_isp_vic_device *vic_dev)
{
    struct tx_isp_dev *isp_dev;
    void __iomem *regs;

    if (!vic_dev)
        return NULL;

    isp_dev = ourISPdev;

    /* OEM HLIL: ALL VIC register access goes through *(arg1 + 0xb8) which is
     * a single canonical pointer.  The OEM never splits writes across two
     * register banks.  Use vic_dev->vic_regs (the primary ISP VIC subsystem
     * at 0x133e0000) as the single canonical bank for everything: MDMA config,
     * stream control (0x300), IRQ mask/status, and VIC start registers.
     */
    regs = vic_dev->vic_regs;
    if (!regs && isp_dev)
        regs = isp_dev->vic_regs;
    if (!regs && ourISPdev)
        regs = ourISPdev->vic_regs;

    /* Last resort: use whatever is at the raw slot */
    if (!regs)
        regs = vic_raw_regs_get(vic_dev);

    /* Keep the raw +0xb8 slot in sync so OEM-style *(arg1+0xb8) paths work */
    if (regs)
        vic_raw_regs_set(vic_dev, regs);

    /* Lazily populate the secondary/coordination bank pointer */
    if (!vic_dev->vic_regs_secondary) {
        if (isp_dev && isp_dev->vic_regs2)
            vic_dev->vic_regs_secondary = isp_dev->vic_regs2;
        else if (ourISPdev && ourISPdev->vic_regs2)
            vic_dev->vic_regs_secondary = ourISPdev->vic_regs2;
    }

    return regs;
}

static inline void __iomem *vic_config_regs_resolve(struct tx_isp_vic_device *vic_dev)
{
    /* OEM BN: tx_isp_vic_start() uses the same +(0xb8) pointer as QBUF,
     * stream arm, MDMA enable, and framedone.
     */
    return vic_primary_regs_resolve(vic_dev);
}

static inline void __iomem *vic_coord_regs_resolve(struct tx_isp_vic_device *vic_dev)
{
    struct tx_isp_dev *isp_dev;

    if (!vic_dev)
        return NULL;

    isp_dev = ourISPdev;

    /* Keep W01/coordination diagnostics off the OEM raw +0xb8 hot path unless
     * the secondary bank is unavailable.
     */
    if (vic_dev->vic_regs_secondary)
        return vic_dev->vic_regs_secondary;
    if (isp_dev && isp_dev->vic_regs2)
        return isp_dev->vic_regs2;
    if (ourISPdev && ourISPdev->vic_regs2)
        return ourISPdev->vic_regs2;

    return vic_primary_regs_resolve(vic_dev);
}

static void vic_log_timeout_bank(const char *tag, void __iomem *regs)
{
    if (!regs) {
        pr_info("VIC timeout bank %s: unmapped\n", tag);
        return;
    }

    pr_info("VIC timeout bank %s@%p: reg0=0x%x reg1a0=0x%x reg1a8=0x%x reg1ac=0x%x\n",
            tag, regs,
            readl(regs + 0x0), readl(regs + 0x1a0),
            readl(regs + 0x1a8), readl(regs + 0x1ac));
}

static void vic_log_unlock_timeout_state(struct tx_isp_vic_device *vic_dev,
                                         void __iomem *vic_regs,
                                         const char *origin)
{
    struct tx_isp_dev *isp_dev;
    void __iomem *coord_regs;
    void __iomem *csi_regs = NULL;
    void __iomem *core_regs = NULL;

    if (!vic_dev)
        return;

    isp_dev = ourISPdev;
    coord_regs = vic_coord_regs_resolve(vic_dev);

    if (isp_dev) {
        if (isp_dev->csi_dev)
            csi_regs = isp_dev->csi_dev->csi_regs;
        if (!csi_regs)
            csi_regs = isp_dev->csi_regs;
        core_regs = isp_dev->core_regs;
    }

    if (!csi_regs && ourISPdev) {
        if (ourISPdev->csi_dev)
            csi_regs = ourISPdev->csi_dev->csi_regs;
        if (!csi_regs)
            csi_regs = ourISPdev->csi_regs;
    }

    if (!core_regs && ourISPdev)
        core_regs = ourISPdev->core_regs;

    vic_log_timeout_bank("stream", vic_regs);
    if (coord_regs && coord_regs != vic_regs) {
        pr_info("%s: W01 coord@%p reg0=0x%x reg14=0x%x reg40=0x%x\n",
                origin, coord_regs,
                readl(coord_regs + 0x0),
                readl(coord_regs + 0x14),
                readl(coord_regs + 0x40));
    }

    if (csi_regs) {
        pr_info("%s: CSI basic@%p reg00=0x%x reg04=0x%x reg0c=0x%x reg10=0x%x reg128=0x%x reg250=0x%x reg254=0x%x\n",
                origin, csi_regs,
                readl(csi_regs + 0x0),
                readl(csi_regs + 0x4),
                readl(csi_regs + 0xc),
                readl(csi_regs + 0x10),
                readl(csi_regs + 0x128),
                readl(csi_regs + 0x250),
                readl(csi_regs + 0x254));
    } else {
        pr_info("%s: CSI basic regs unmapped\n", origin);
    }

    if (core_regs) {
        pr_info("%s: core route 9a00=0x%x 9a04=0x%x 9a2c=0x%x 9a34=0x%x 9a80=0x%x 9a98=0x%x gate=0x%x/0x%x\n",
                origin,
                readl(core_regs + 0x9a00),
                readl(core_regs + 0x9a04),
                readl(core_regs + 0x9a2c),
                readl(core_regs + 0x9a34),
                readl(core_regs + 0x9a80),
                readl(core_regs + 0x9a98),
                readl(core_regs + 0x9ac0),
                readl(core_regs + 0x9ac8));
    }
}

static int vic_wait_reg0_zero_timeout(struct tx_isp_vic_device *vic_dev,
                                      void __iomem *vic_regs,
                                      unsigned int timeout_ms,
                                      const char *origin)
{
    unsigned int waited = 0;
    u32 reg0 = 0;

    if (!vic_regs)
        return -EINVAL;

    while (waited < timeout_ms) {
        reg0 = readl(vic_regs + 0x0);
        if (reg0 == 0)
            return 0;

        usleep_range(1000, 2000);
        waited += 1;
    }

    reg0 = readl(vic_regs + 0x0);
    pr_err("%s: VIC unlock poll timed out after %u ms (reg0=0x%x)\n",
           origin, waited, reg0);
    vic_log_unlock_timeout_state(vic_dev, vic_regs, origin);
    return -ETIMEDOUT;
}

static int vic_wait_w01_phase(void __iomem *vic_coord_regs, unsigned int timeout_ms)
{
    const u32 t0 = 0x00000230;
    const u32 t1 = 0x00000300;
    const u32 t2 = 0x00000330;
    u32 prev;
    unsigned int stable = 0;
    unsigned int waited = 0;

    if (!vic_coord_regs)
        return 0;

    prev = readl(vic_coord_regs + 0x14);

    while (waited < timeout_ms) {
        u32 cur;
        int match;

        usleep_range(1000, 2000);
        waited += 1;
        cur = readl(vic_coord_regs + 0x14);
        match = (cur == t0) || (cur == t1) || (cur == t2);

        if (match && cur == prev) {
            stable++;
            if (stable >= 2) {
                pr_info("tx_isp_vic_start: W01 phase-ready 0x14=0x%08x after %u ms\n",
                        cur, waited);
                return 1;
            }
        } else {
            stable = match ? 1 : 0;
        }

        prev = cur;
    }

    pr_info("tx_isp_vic_start: W01 phase wait timed out, last 0x14=0x%08x after %u ms\n",
            prev, waited);
    return 0;
}

static int vic_force_channel_start(struct tx_isp_vic_device *vic_dev,
                                   u32 width, u32 height,
                                   const char *origin)
{
    struct tx_isp_channel_attr attr;
    u32 attr_words[0x34 / sizeof(u32)];
    int ret;

    if (!vic_dev)
        return -EINVAL;

    memset(&attr, 0, sizeof(attr));
    memset(attr_words, 0, sizeof(attr_words));
    attr.width = width ? width : 1920;
    attr.height = height ? height : 1080;
    attr.stride = attr.width;

    /* Mirror the OEM set-format path enough to populate ds0_attr and the
     * 0x9860/0x9864 + 0x99xx geometry registers before channel_start.
     */
    attr_words[0] = 0;
    attr_words[1] = attr.width;
    attr_words[2] = attr.height;
    attr_words[3] = 0;
    attr_words[4] = 0;
    attr_words[5] = 0;
    attr_words[6] = attr.width;
    attr_words[7] = attr.height;
    attr_words[8] = 0;

    ret = tisp_channel_attr_set(0, attr_words);
    if (ret < 0) {
        pr_warn("%s: fallback tisp_channel_attr_set(ch0 %ux%u) failed: %d\n",
                origin, attr.width, attr.height, ret);
        return ret;
    }

    pr_info("%s: fallback tisp_channel_attr_set(ch0 %ux%u) complete\n",
            origin, attr.width, attr.height);

    ret = tisp_channel_start(0, &attr);
    if (ret < 0) {
        pr_warn("%s: fallback tisp_channel_start(ch0 %ux%u) failed: %d\n",
                origin, attr.width, attr.height, ret);
        return ret;
    }

    pr_info("%s: fallback tisp_channel_start(ch0 %ux%u) complete\n",
            origin, attr.width, attr.height);
    return 0;
}

static inline void __iomem *vic_stream_regs_resolve(struct tx_isp_vic_device *vic_dev)
{
    struct tx_isp_dev *isp_dev;
    void __iomem *regs;

    if (!vic_dev)
        return NULL;

    isp_dev = ourISPdev;

    /* Keep tx_isp_vic_start(), QBUF, and STREAMON on the stable primary
     * wrapper bank (0x133e0000), matching the last green-stream state.
     */
    regs = vic_dev->vic_regs;
    if (!regs && isp_dev)
        regs = isp_dev->vic_regs;
    if (!regs && ourISPdev)
        regs = ourISPdev->vic_regs;
    if (!regs)
        regs = vic_raw_regs_get(vic_dev);

    if (regs)
        vic_raw_regs_set(vic_dev, regs);

    return regs;
}

static inline void vic_raw_dims_set(struct tx_isp_vic_device *vic_dev, u32 width, u32 height)
{
    if (!vic_dev)
        return;
    vic_dev->width = width;
    vic_dev->height = height;
}

static inline u32 vic_raw_width_get(struct tx_isp_vic_device *vic_dev)
{
    if (!vic_dev)
        return 0;
    return vic_dev->width;
}

static inline u32 vic_raw_height_get(struct tx_isp_vic_device *vic_dev)
{
    if (!vic_dev)
        return 0;
    return vic_dev->height;
}

static inline struct tx_isp_sensor_attribute *vic_raw_sensor_attr_get(struct tx_isp_vic_device *vic_dev)
{
    if (!vic_dev)
        return NULL;

    /* OEM tx_isp_vic_start dereferences a POINTER to the FULL sensor_attr
     * (set during sync_sensor_attr).  Prefer that over the 76-byte cache,
     * which lacks mipi_sc fields beyond hcomp_en.
     */
    if (vic_dev->sensor_attr_ptr)
        return vic_dev->sensor_attr_ptr;

    return &vic_dev->sensor_attr;
}

static inline void vic_raw_sensor_attr_sync(struct tx_isp_vic_device *vic_dev,
                                            const struct tx_isp_sensor_attribute *attr)
{
    struct tx_isp_sensor_attribute *raw_attr;

    if (!vic_dev)
        return;

    raw_attr = vic_raw_sensor_attr_get(vic_dev);
    if (!raw_attr)
        return;

    if (attr)
        memcpy(raw_attr, attr, sizeof(*raw_attr));
    else
        memset(raw_attr, 0, sizeof(*raw_attr));
}

static inline struct tx_isp_vic_device *vic_raw_self_from_sd(struct tx_isp_subdev *sd)
{
    if (!sd)
        return NULL;
    /* OEM stores self-pointer in sd->dev_priv */
    return (struct tx_isp_vic_device *)sd->dev_priv;
}

static inline void vic_raw_self_set(struct tx_isp_vic_device *vic_dev)
{
    if (!vic_dev)
        return;
    /* Store self-pointer in sd->dev_priv for container recovery */
    vic_dev->sd.dev_priv = vic_dev;
}

static inline u32 vic_raw_state_get(struct tx_isp_vic_device *vic_dev)
{
    if (!vic_dev)
        return 0;
    return vic_dev->state;
}

static inline void vic_raw_state_set(struct tx_isp_vic_device *vic_dev, u32 state)
{
    if (!vic_dev)
        return;
    vic_dev->state = state;
}

static inline void vic_raw_irq_flag_set(struct tx_isp_vic_device *vic_dev, u32 enabled)
{
    if (!vic_dev)
        return;
    vic_dev->hw_irq_enabled = enabled;
    vic_dev->irq_enabled = enabled;
}

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

/* NOTE: deferred_vic_run removed — OEM issues VIC RUN immediately in
 * tx_isp_vic_start.  MDMA enable is a separate, independent step. */

/* *** CRITICAL: MISSING FUNCTION - tx_isp_create_vic_device *** */
/* Forward declarations for PIPO callbacks used before definitions */
static int ispvic_frame_channel_qbuf(void *arg1, void *arg2);
static int ispvic_frame_channel_clearbuf(void);
static void vic_pipo_mdma_enable(struct tx_isp_vic_device *vic_dev);
static void vic_bind_event_dispatch_table(struct tx_isp_vic_device *vic_dev);

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

    pr_info("*** VIC DEVICE ALLOCATED: %p (size=%zu bytes) ***\n",
            vic_dev, sizeof(struct tx_isp_vic_device));

    /* Initialize VIC device structure */

    /* Initialize synchronization primitives using struct members */
    spin_lock_init(&vic_dev->lock);
    mutex_init(&vic_dev->mlock);
    mutex_init(&vic_dev->state_lock);
    init_completion(&vic_dev->frame_complete);
    spin_lock_init(&vic_dev->buffer_mgmt_lock);
    spin_lock_init(&vic_dev->buffer_lock);

    /* Set initial state */
    vic_raw_state_set(vic_dev, 1);

    /* Set self-pointer for container recovery */
    vic_raw_self_set(vic_dev);

    /* Set default pixel format */
    vic_dev->pixel_format = V4L2_PIX_FMT_NV12;

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

    /* OEM parity: the VIC datapath uses a single primary register base.
     * Keep the secondary mapping for coordination/status reads only.
     */
    {
        u32 stride_a = readl(vic_dev->vic_regs + 0x310);
        u32 stride_b = readl(vic_dev->vic_regs_secondary + 0x310);
        pr_info("*** VIC MDMA bank observe: PRI strideY=%u, SEC strideY=%u ***\n",
                stride_a, stride_b);
    }

    /* Also store in ISP device for compatibility */
    if (!isp_dev->vic_regs) {
        isp_dev->vic_regs = vic_dev->vic_regs;
    }
    if (!isp_dev->vic_regs2) {
        isp_dev->vic_regs2 = vic_dev->vic_regs_secondary;
    }

    /* OEM: *(arg1 + 0xb8) is the SINGLE canonical VIC register pointer used
     * by ALL functions: vic_pipo_mdma_enable, ispvic_frame_channel_s_stream,
     * isp_vic_interrupt_service_routine, tx_isp_vic_start, etc.
     * Seed it to the primary VIC subsystem bank (0x133e0000).
     */
    vic_raw_regs_set(vic_dev, vic_dev->vic_regs);

    /* Initialize VIC device dimensions - CRITICAL: Use actual sensor output dimensions */
    vic_dev->width = 1920;  /* GC2053 actual output width */
    vic_dev->height = 1080; /* GC2053 actual output height */
    vic_dev->stride = vic_dev->width;
    vic_dev->pixel_format = V4L2_PIX_FMT_NV12;
    vic_raw_dims_set(vic_dev, vic_dev->width, vic_dev->height);

    /* Set up VIC subdev structure.
     * Do NOT memset the embedded subdev here: kzalloc() already zeroed the
     * whole object, and wiping sd at this point clobbers the drifted VIC
     * fields/MMIO cache that live beyond the OEM subdev footprint.
     */
    /* sd.isp removed for ABI - use ourISPdev global */
    vic_dev->sd.ops = &vic_subdev_ops;
    ourISPdev->vin_state = TX_ISP_MODULE_INIT;

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
    vic_raw_state_set(vic_dev, 1);
    vic_dev->irq = vic_resolve_irq_number(vic_dev);
    vic_dev->irq_number = vic_dev->irq;
    vic_dev->irq_enabled = 0;
    vic_raw_irq_flag_set(vic_dev, 0);
    vic_raw_irq_slot_seed(vic_dev, vic_dev->irq);

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
    vic_raw_sensor_attr_sync(vic_dev, &vic_dev->sensor_attr);

    /* *** CRITICAL: Link VIC device to ISP core *** */
    /* Store the VIC device properly - the subdev is PART of the VIC device */
    isp_dev->vic_dev = (struct tx_isp_subdev *)&vic_dev->sd;

    /* *** CRITICAL FIX: Ensure VIC subdev has proper ISP device back-reference *** */
    /* sd.isp removed for ABI - use ourISPdev global */  /* This ensures tx_isp_vic_start can find the ISP device */

    /* Also set host_priv so vic_core_ops_ioctl can retrieve vic_dev (QBUF path) */
    tx_isp_set_subdev_hostdata(&vic_dev->sd, vic_dev);
    vic_bind_event_dispatch_table(vic_dev);

    pr_info("*** CRITICAL: VIC DEVICE LINKED TO ISP CORE ***\n");
    pr_info("  isp_dev->vic_dev = %p\n", isp_dev->vic_dev);
    pr_info("  ourISPdev = %p\n", ourISPdev);
    pr_info("  vic_dev->sd.ops = %p\n", vic_dev->sd.ops);
    pr_info("  vic_dev->vic_regs = %p\n", vic_dev->vic_regs);
    pr_info("  vic_dev->state = %d\n", vic_dev->state);
    pr_info("  vic_dev dimensions = %dx%d\n", vic_dev->width, vic_dev->height);

    /* Set up tx_isp_get_subdevdata to work properly */
    /* This sets up the private data pointer so tx_isp_get_subdevdata can retrieve the VIC device */
    vic_dev->sd.dev_priv = vic_dev;

    /* Final reseed after subdev wiring so both the drifted named members and
     * OEM raw slots are populated from the stable ISP-owned mappings/geometry
     * before probe/stream-on touch the object.
     */
    {
        u32 seed_width = 1920;
        u32 seed_height = 1080;

        if (isp_dev->sensor_width && isp_dev->sensor_height) {
            seed_width = isp_dev->sensor_width;
            seed_height = isp_dev->sensor_height;
        }

        vic_dev->vic_regs = isp_dev->vic_regs;
        if (!vic_dev->vic_regs_secondary)
            vic_dev->vic_regs_secondary = isp_dev->vic_regs2;
        vic_dev->width = seed_width;
        vic_dev->height = seed_height;

        vic_raw_self_set(vic_dev);
        vic_raw_regs_set(vic_dev, vic_primary_regs_resolve(vic_dev));
        vic_raw_dims_set(vic_dev, seed_width, seed_height);
    }
    vic_raw_sensor_attr_sync(vic_dev, &vic_dev->sensor_attr);
    pr_info("*** tx_isp_create_vic_device: Final OEM slot seed stable=%p raw=%p dims=%ux%u raw_attr=%p ***\n",
            isp_dev->vic_regs, vic_raw_regs_get(vic_dev), vic_raw_width_get(vic_dev),
            vic_raw_height_get(vic_dev), vic_raw_sensor_attr_get(vic_dev));

    pr_info("*** tx_isp_create_vic_device: VIC device creation complete ***\n");
    pr_info("*** NO MORE 'NO VIC DEVICE' ERROR SHOULD OCCUR ***\n");

    return 0;
}
EXPORT_SYMBOL(tx_isp_create_vic_device);

/* VIC frame completion handler */
static void tx_isp_vic_frame_done(struct tx_isp_subdev *sd, int channel)
{
    struct tx_isp_vic_device *vic_dev;
    if (!sd || channel >= VIC_MAX_CHAN)
        return;
    vic_dev = container_of(sd, struct tx_isp_vic_device, sd);
    complete(&vic_dev->vic_frame_end_completion[channel]);
}

/* Forward declarations for interrupt functions */
int vic_framedone_irq_function(struct tx_isp_vic_device *vic_dev);
static int vic_mdma_irq_function(struct tx_isp_vic_device *vic_dev, int channel);

static void vic_program_irq_registers(struct tx_isp_vic_device *vic_dev,
                                      const char *origin)
{
    void __iomem *vic_base;

    vic_base = vic_primary_regs_resolve(vic_dev);
    if (!vic_base) {
        pr_err("%s: No primary VIC registers available\n", origin);
        return;
    }

    /* OEM ISR semantics:
     *   0x1e0/0x1e4 = status registers
     *   0x1e8/0x1ec = mask registers (bit=1 → MASKED, bit=0 → unmasked)
     *   0x1f0/0x1f4 = acknowledge/clear registers
     *
     * OEM ISR formula: pending = (~mask) & status
     *
     * Bank 1 (0x1e8): Unmask bit 0 (frame_done) — OEM minimum.
     * Bank 2 (0x1ec): Unmask bits 0,1 (MDMA ch0/ch1 done) — CRITICAL for
     *   frame delivery.  OEM vic_mdma_irq_function handles these.
     *   HW is only 4 bits wide so 0xFFFFFFFC reads back as 0x0000000C.
     */
    writel(0xFFFFFFFF, vic_base + 0x1f0);   /* clear all bank 1 pending */
    writel(0xFFFFFFFF, vic_base + 0x1f4);   /* clear all bank 2 pending */
    wmb();

    writel(0xFFFFFFFE, vic_base + 0x1e8);   /* unmask bit 0 (frame_done) */
    writel(0xFFFFFFFC, vic_base + 0x1ec);   /* unmask bits 0,1 (MDMA ch0/ch1) */
    wmb();

    pr_info("%s: VIC IRQ regs status=0x%08x/0x%08x mask=0x%08x/0x%08x\n",
            origin,
            readl(vic_base + 0x1e0), readl(vic_base + 0x1e4),
            readl(vic_base + 0x1e8), readl(vic_base + 0x1ec));
}

static void vic_reassert_core_irq_route(struct tx_isp_vic_device *vic_dev,
                                        u32 width, u32 height, u32 stride,
                                        const char *origin)
{
    struct tx_isp_dev *isp_dev;
    void __iomem *core = NULL;
    void __iomem *vic_base;
    u32 packed_geom;

    if (!vic_dev)
        return;

    isp_dev = ourISPdev;
    if (isp_dev && isp_dev->core_regs)
        core = isp_dev->core_regs;
    if (!core && ourISPdev && ourISPdev->core_regs)
        core = ourISPdev->core_regs;

    vic_base = vic_primary_regs_resolve(vic_dev);

    if (!core) {
        pr_info("%s: deferring VIC core route/gate reassert until core_regs is mapped\n",
                origin);
        return;
    }

    if (!width) {
        width = vic_raw_width_get(vic_dev);
        if (!width)
            width = vic_dev->width;
        if (!width)
            width = 1920;
    }

    if (!height) {
        height = vic_raw_height_get(vic_dev);
        if (!height)
            height = vic_dev->height;
        if (!height)
            height = 1080;
    }

    packed_geom = (width << 16) | (height & 0xffff);

    writel(0x1, core + 0x9a70);
    writel(0x1, core + 0x9a7c);
    writel(0x1, core + 0x9a34);
    writel(0x1, core + 0x9a88);
    writel(0x1, core + 0x9a94);

    /* The OEM trace reprograms the VIC/core route block with live geometry.
     * Do not preserve stale values from an earlier mode (e.g. 1280-wide route
     * when VIC is now starting with a 1920-wide sensor).
     */
    writel(packed_geom, core + 0x9a00);
    writel(0x3000300, core + 0x9a04);
    writel(packed_geom, core + 0x9a2c);
    writel(width, core + 0x9a80);
    writel(width, core + 0x9a98);
    writel(0x200, core + 0x9ac0);
    writel(0x200, core + 0x9ac8);

    wmb();

    pr_info("%s: core VIC route 9a00=0x%08x 9a04=0x%08x 9a2c=0x%08x 9a34=0x%08x 9a88=0x%08x 9a94=0x%08x 9a80=0x%08x 9a98=0x%08x gate=0x%08x/0x%08x\n",
            origin,
            readl(core + 0x9a00),
            readl(core + 0x9a04),
            readl(core + 0x9a2c),
            readl(core + 0x9a34), readl(core + 0x9a88), readl(core + 0x9a94),
            readl(core + 0x9a80), readl(core + 0x9a98),
            readl(core + 0x9ac0), readl(core + 0x9ac8));
}

/* VIC interrupt restoration function - using correct VIC base */
void tx_isp_vic_restore_interrupts(void)
{
    extern struct tx_isp_dev *ourISPdev;
    struct tx_isp_vic_device *vic_dev;

    if (!ourISPdev || !ourISPdev->vic_dev || vic_start_ok != 1)
        return; /* VIC not active */

    pr_info("*** VIC INTERRUPT RESTORE: Restoring VIC interrupt registers in PRIMARY VIC space ***\n");

    vic_dev = ourISPdev->vic_dev;
    if (!vic_dev) {
        pr_err("*** VIC INTERRUPT RESTORE: No primary VIC registers available ***\n");
        return;
    }

    /* Restore VIC interrupt register values using WORKING ISP-activates configuration */
    pr_info("*** VIC INTERRUPT RESTORE: Using WORKING ISP-activates configuration (0x1e8/0x1ec) ***\n");
    vic_program_irq_registers(vic_dev, "tx_isp_vic_restore_interrupts");
}
EXPORT_SYMBOL(tx_isp_vic_restore_interrupts);

/* Global data symbol used by reference driver */
static char data_b0000[1] = {0};

static void vic_free_buffer_list(struct list_head *head)
{
	while (!list_empty(head)) {
		struct vic_buffer_entry *entry;

		entry = list_first_entry(head, struct vic_buffer_entry, list);
		list_del(&entry->list);
		kfree(entry);
	}
}

/* Helper functions - removed conflicting declarations as they're already in SDK headers */
/* __private_spin_lock_irqsave and private_spin_unlock_irqrestore are defined in txx-funcs.h */


/* Forward declaration for streaming functions */
int ispvic_frame_channel_s_stream(struct tx_isp_vic_device *vic_dev, int enable);

/* OEM global raw_pipe — persists between tx_isp_subdev_pipo and vic_mdma_irq_function.
 * Populated during pipo init; the IRQ handler reads raw_pipe[1] (dqbuf callback)
 * and raw_pipe[4] (sd argument).
 */
static void *raw_pipe_global[6];

/* OEM global DMA buffer index / sub-get counters for non-streaming MDMA cycling */
static u32 vic_mdma_ch0_set_buff_index;
static u32 vic_mdma_ch0_sub_get_num;
static u32 vic_mdma_ch1_set_buff_index;
static u32 vic_mdma_ch1_sub_get_num;

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
            /* Binary Ninja: void* $a3_1 = *(arg1 + 0xb8)
             * Follow the live raw-slot/hot-path resolver here so framedone
             * handling observes the same bank the OEM stream/QBUF path uses.
             */
            vic_regs = vic_stream_regs_resolve(vic_dev);

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
                    struct vic_buffer_entry *entry = container_of(pos, struct vic_buffer_entry, list);

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

            /* OEM framedone always updates only the bank-count/index bits in
             * VIC_ADDR_DMA_CONTROL, even when the computed value is zero.
             * Zero is valid here: it means the current DMA address matched the
             * last active bank in the done list.
             */
            if (vic_regs) {
                u32 reg_val = readl(vic_regs + 0x300);

                reg_val = (reg_val & 0xfff0ffff) | shifted_value;
                writel(reg_val, vic_regs + 0x300);

                pr_debug_ratelimited("vic_irq: VIC[0x300]=0x%x (idx updated)\n",
                        reg_val);
                pr_debug_ratelimited("vic_irq: ctrl=0x%x idx=%d match=%d shift=0x%x\n",
                         reg_val, buffer_index, match_found, shifted_value);
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

/* vic_mdma_irq_function - OEM HLIL: handles MDMA DMA completion per channel.
 *
 * OEM has two paths:
 *   processing == 0 (non-streaming/calibration): cycle DMA buffer indices, complete()
 *   processing != 0 (streaming): pop done buffer, deliver frame via raw_pipe,
 *                                 rotate next queued buffer into VIC DMA slot.
 */
static int vic_mdma_irq_function(struct tx_isp_vic_device *vic_dev, int channel)
{
	void __iomem *vic_base;
	u32 frame_size;

	vic_base = vic_raw_regs_get(vic_dev);

	if (vic_dev->processing == 0) {
		/* --- OEM non-streaming path: calibration buffer cycling --- */
		frame_size = vic_dev->width * vic_dev->height * 2;

		if (channel == 0 && vic_mdma_ch0_sub_get_num > 0) {
			u32 cur_idx = vic_mdma_ch0_set_buff_index;
			u32 next_idx = (cur_idx + 1) % 5;
			u32 next_addr;

			if (vic_base) {
				next_addr = readl(vic_base + ((cur_idx + 0xc6) << 2)) + frame_size;
				vic_mdma_ch0_set_buff_index = next_idx;
				writel(next_addr, vic_base + ((next_idx + 0xc6) << 2));
			}
			vic_mdma_ch0_sub_get_num--;

			/* OEM: when sub_get_num wraps to 7 (underflow), disable MDMA sub-get */
			if (vic_mdma_ch0_sub_get_num == 7 && vic_base)
				writel(readl(vic_base + 0x300) & 0xfff0ffff, vic_base + 0x300);
		} else if (channel == 1 && vic_mdma_ch1_sub_get_num > 0) {
			u32 cur_idx = vic_mdma_ch1_set_buff_index;
			u32 next_idx = (cur_idx + 1) % 5;
			u32 next_addr;

			if (vic_base) {
				next_addr = readl(vic_base + ((cur_idx + 0xc6) << 2)) + frame_size;
				vic_mdma_ch1_set_buff_index = next_idx;
				writel(next_addr, vic_base + ((next_idx + 0xc6) << 2));
			}
			vic_mdma_ch1_sub_get_num--;
		}

		/* OEM: complete() only when BOTH channels' sub_get_nums are 0 */
		if (vic_mdma_ch0_sub_get_num == 0 && vic_mdma_ch1_sub_get_num == 0)
			complete(&vic_dev->frame_complete);

	} else {
		/* --- OEM streaming path: buffer rotation + frame delivery --- */
		u32 current_dma_addr = 0;
		struct vic_buffer_entry *done_buf;
		int (*dqbuf_fn)(void *, void *);
		void *dqbuf_arg;
		int i;

		if (vic_base)
			current_dma_addr = readl(vic_base + 0x380);

		/* Pop completed buffer from done_head */
		if (list_empty(&vic_dev->done_head)) {
			pr_info_ratelimited("busy_buf null; busy_buf_count= %d\n",
					    vic_dev->active_buffer_count);
			goto mdma_irq_refill;
		}

		done_buf = list_first_entry(&vic_dev->done_head,
					    struct vic_buffer_entry, list);
		list_del(&done_buf->list);
		vic_dev->active_buffer_count--;

		/* Deliver frame to downstream via raw_pipe[1](raw_pipe[4], buffer) */
		dqbuf_fn = (int (*)(void *, void *))raw_pipe_global[1];
		dqbuf_arg = raw_pipe_global[4];
		if (dqbuf_fn)
			dqbuf_fn(dqbuf_arg, done_buf);

		/* Move completed buffer to free_head */
		list_add_tail(&done_buf->list, &vic_dev->free_head);

		/* OEM: check if done_buf's DMA addr matches current HW addr */
		if (done_buf->buffer_addr != current_dma_addr) {
			/* DMA address mismatch — drain done_head until match */
			for (i = 0; i < (int)vic_dev->active_buffer_count; i++) {
				struct vic_buffer_entry *extra;

				if (list_empty(&vic_dev->done_head)) {
					pr_info_ratelimited("line = %d, i=%d ;num = %d;busy_buf_count %d\n",
							    __LINE__, i, i,
							    vic_dev->active_buffer_count);
					continue;
				}

				extra = list_first_entry(&vic_dev->done_head,
							 struct vic_buffer_entry, list);
				list_del(&extra->list);
				vic_dev->active_buffer_count--;

				pr_debug_ratelimited("line : %d; bank_addr:0x%x; addr:0x%x\n",
						     __LINE__, extra->buffer_addr,
						     current_dma_addr);

				/* Deliver this buffer too */
				if (dqbuf_fn)
					dqbuf_fn(dqbuf_arg, extra);

				list_add_tail(&extra->list, &vic_dev->free_head);

				if (extra->buffer_addr == current_dma_addr)
					break;
			}

			if (i >= (int)vic_dev->active_buffer_count) {
				pr_info_ratelimited("function: %s ; vic dma addrrss error!!!\n",
						    __func__);
				if (vic_base)
					pr_info_ratelimited("VIC_ADDR_DMA_CONTROL : 0x%x\n",
							    readl(vic_base + 0x300));
			}
		}

mdma_irq_refill:
		/* OEM: refill — move next queued buffer into a free bank slot */
		if (!list_empty(&vic_dev->free_head) &&
		    !list_empty(&vic_dev->queue_head)) {
			struct vic_buffer_entry *q_buf, *f_buf;

			/* Pop from queue_head */
			q_buf = list_first_entry(&vic_dev->queue_head,
						 struct vic_buffer_entry, list);
			list_del(&q_buf->list);

			/* Pop a free bank */
			f_buf = list_first_entry(&vic_dev->free_head,
						 struct vic_buffer_entry, list);
			list_del(&f_buf->list);

			/* Transfer address to the bank entry */
			f_buf->buffer_addr = q_buf->buffer_addr;

			/* Program VIC DMA slot register */
			if (vic_base)
				writel(f_buf->buffer_addr,
				       vic_base + ((f_buf->buffer_index + 0xc6) << 2));

			/* Move bank to done (active) list */
			list_add_tail(&f_buf->list, &vic_dev->done_head);
			vic_dev->active_buffer_count++;

			kfree(q_buf);
		}
	}

	return 0;
}

/* Probe-time VIC init stays out of the OEM IRQ route/mask path. */
int tx_isp_vic_hw_init(struct tx_isp_subdev *sd)
{
    struct tx_isp_vic_device *vic_dev = container_of(sd, struct tx_isp_vic_device, sd);

    if (!vic_dev)
        return -EINVAL;

    vic_raw_irq_flag_set(vic_dev, 0);
    return 0;
}

/* Stop VIC processing */
int tx_isp_vic_stop(struct tx_isp_subdev *sd)
{
    struct tx_isp_vic_device *vic_dev;
    u32 ctrl;

    if (!sd || !ourISPdev)
        return -EINVAL;

    vic_dev = container_of(sd, struct tx_isp_vic_device, sd);
    mutex_lock(&vic_dev->vic_frame_end_lock);

    /* Stop processing */
    ctrl = vic_read32(VIC_CTRL);
    ctrl &= ~VIC_CTRL_START;
    ctrl |= VIC_CTRL_STOP;
    vic_write32(VIC_CTRL, ctrl);

    /* Wait for stop to complete */
    while (vic_read32(VIC_STATUS) & STATUS_BUSY) {
        udelay(10);
    }

    mutex_unlock(&vic_dev->vic_frame_end_lock);
    return 0;
}

/* Configure VIC frame buffer */
int tx_isp_vic_set_buffer(struct tx_isp_subdev *sd, dma_addr_t addr, u32 size)
{
    struct tx_isp_vic_device *vic_dev;

    if (!sd || !ourISPdev)
        return -EINVAL;

    vic_dev = container_of(sd, struct tx_isp_vic_device, sd);
    mutex_lock(&vic_dev->vic_frame_end_lock);

    /* Set frame buffer address and size */
    vic_write32(VIC_BUFFER_ADDR, addr);
    vic_write32(VIC_FRAME_SIZE, size);

    mutex_unlock(&vic_dev->vic_frame_end_lock);
    return 0;
}

/* Wait for frame completion */
int tx_isp_vic_wait_frame_done(struct tx_isp_subdev *sd, int channel, int timeout_ms)
{
    struct tx_isp_vic_device *vic_dev;
    int ret;

    if (!sd || channel >= VIC_MAX_CHAN) {
        pr_err("Invalid subdev or channel\n");
        return -EINVAL;
    }

    vic_dev = container_of(sd, struct tx_isp_vic_device, sd);
    ret = wait_for_completion_timeout(
        &vic_dev->vic_frame_end_completion[channel],
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
    capture_buf = dma_alloc_coherent(sd->module.dev, buf_size, &dma_addr, GFP_KERNEL);
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
    capture_buf = dma_alloc_coherent(sd->module.dev, buf_size, &dma_addr, GFP_KERNEL);
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
        dma_free_coherent(sd->module.dev, buf_size, capture_buf, dma_addr);
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

    capture_buf = dma_alloc_coherent(sd->module.dev, buf_size, &dma_addr, GFP_KERNEL);
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
    if (capture_buf) dma_free_coherent(sd->module.dev, buf_size, capture_buf, dma_addr);
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
    struct tx_isp_dev *isp_dev;
    void __iomem *vic_regs;
    struct tx_isp_sensor_attribute *sensor_attr;
    u32 interface_type;
    u32 actual_width, actual_height;
    u32 stride;
    int ret;

    /* Binary Ninja: 00010244 void* $v1 = *(arg1 + 0x110) */
    if (!vic_dev)
        return -EINVAL;

    sensor_attr = vic_raw_sensor_attr_get(vic_dev);
    if (!sensor_attr)
        return -EINVAL;

    interface_type = sensor_attr->dbus_type;

    actual_width = vic_raw_width_get(vic_dev);
    actual_height = vic_raw_height_get(vic_dev);

    vic_regs = vic_stream_regs_resolve(vic_dev);
    if (!vic_regs)
        return -EINVAL;

    isp_dev = ourISPdev;
    ret = tx_isp_core_prepare_prestream(isp_dev, "tx_isp_vic_start");
    if (ret < 0) {
        pr_err("tx_isp_vic_start: pre-stream core IRQ prepare failed: %d\n", ret);
        return ret;
    }

    /* Binary Ninja: Branch on interface type at 00010250 */
    /* CRITICAL FIX: Use correct enum values - MIPI=1, DVP=2 */
    if (interface_type == TX_SENSOR_DATA_INTERFACE_MIPI) {  /* MIPI = 1 in our enum */
        struct vic_mipi_sensor_ctl *mipi_sc = &sensor_attr->mipi.mipi_sc;
        u32 image_twidth = sensor_attr->mipi.image_twidth ? sensor_attr->mipi.image_twidth : actual_width;
        u32 bits_per_pixel = 0;
        u32 frame_mode_reg = 0x4440;
			u32 sensor_csi_fmt = mipi_sc->sensor_csi_fmt;
        u32 mipi_vcomp_en = mipi_sc->mipi_vcomp_en;
        u32 mipi_hcomp_en = mipi_sc->mipi_hcomp_en;
        u32 line_sync_mode = mipi_sc->line_sync_mode;
        u32 work_start_flag = mipi_sc->work_start_flag;
        u32 data_type_en = mipi_sc->data_type_en;
        u32 data_type_value = mipi_sc->data_type_value;
        u32 unlock_1a0;
        u32 reg_10c;
        u32 reg_1a4;

        if (sensor_attr->mipi.mode == SENSOR_MIPI_SONY_MODE) {
            writel(0x20000, vic_regs + 0x10);
            reg_1a4 = 0x100010;
            pr_info("sensor type is SONY_MIPI!\n");
        } else {
            reg_1a4 = 0xa000a;
            pr_info("sensor type is OTHER_MIPI!\n");
        }

			switch (sensor_csi_fmt) {
        case TX_SENSOR_RAW10:
            bits_per_pixel = 10;
            break;
        case TX_SENSOR_RAW12:
            bits_per_pixel = 12;
            break;
        case TX_SENSOR_YUV422:
            bits_per_pixel = 16;
            break;
        case TX_SENSOR_RAW8:
            bits_per_pixel = 8;
            break;
        default:
            bits_per_pixel = 0;
            break;
        }

        /* OEM BN writes 0x1a4 before the remaining MIPI sizing/control
         * registers in the stream-on path.
         */
        writel(reg_1a4, vic_regs + 0x1a4);
        writel(((bits_per_pixel * image_twidth) + 0x1f) >> 5, vic_regs + 0x100);
        writel(2, vic_regs + 0xc);
		writel(sensor_csi_fmt, vic_regs + 0x14);
        writel((actual_width << 16) | actual_height, vic_regs + 0x4);
        reg_10c = (mipi_sc->hcrop_diff_en << 25) |
                  (mipi_vcomp_en << 24) |
                  (mipi_hcomp_en << 23) |
                  (line_sync_mode << 22) |
                  (work_start_flag << 20) |
                  (data_type_en << 18) |
                  (data_type_value << 12) |
                  (mipi_sc->del_start << 8) |
                  (mipi_sc->sensor_frame_mode << 4) |
                  (mipi_sc->sensor_fid_mode << 2) |
                  mipi_sc->sensor_mode;
        writel(reg_10c, vic_regs + 0x10c);
        writel((image_twidth << 16) | mipi_sc->mipi_crop_start0x, vic_regs + 0x110);
        writel(mipi_sc->mipi_crop_start1x, vic_regs + 0x114);
        writel(mipi_sc->mipi_crop_start2x, vic_regs + 0x118);
        writel(mipi_sc->mipi_crop_start3x, vic_regs + 0x11c);

        switch (mipi_sc->sensor_frame_mode) {
        case TX_SENSOR_WDR_2_FRAME_MODE:
            frame_mode_reg = 0x4140;
            break;
        case TX_SENSOR_WDR_3_FRAME_MODE:
            frame_mode_reg = 0x4240;
            break;
        case TX_SENSOR_DEFAULT_FRAME_MODE:
        default:
            frame_mode_reg = 0x4440;
            break;
        }

        writel(frame_mode_reg, vic_regs + 0x1ac);
        writel(frame_mode_reg, vic_regs + 0x1a8);
        writel(0x10, vic_regs + 0x1b0);

        /* OEM BN: tight 2 -> 4 -> 1a0 write stream, no extra calls before arm */
        writel(2, vic_regs + 0x0);
        writel(4, vic_regs + 0x0);
        unlock_1a0 = (mipi_sc->sensor_frame_mode << 4) | mipi_sc->sensor_mode;
        writel(unlock_1a0, vic_regs + 0x1a0);

        if (vic_wait_reg0_zero_timeout(vic_dev, vic_regs, 100,
                                       "tx_isp_vic_start") < 0) {
            pr_err("tx_isp_vic_start: MIPI unlock stalled reg1a4=0x%x reg10c=0x%x reg1a0=0x%x fmt=%u dims=%ux%u twidth=%u\n",
                   reg_1a4, reg_10c, unlock_1a0, sensor_csi_fmt,
                   actual_width, actual_height, image_twidth);
            return -ETIMEDOUT;
        }

        writel((mipi_sc->mipi_crop_start1y << 16) | mipi_sc->mipi_crop_start0y, vic_regs + 0x104);
        writel((mipi_sc->mipi_crop_start3y << 16) | mipi_sc->mipi_crop_start2y, vic_regs + 0x108);
        /* OEM BN EXACT: VIC RUN issued immediately in tx_isp_vic_start.
         * MDMA enable (reg 0x300/0x308) is a SEPARATE step done later by
         * ispvic_frame_channel_s_stream.  VIC RUN starts the input engine
         * (sensor→VIC FIFO), MDMA enable starts the output engine
         * (VIC FIFO→DRAM).  They are independent. */
        writel(1, vic_regs + 0x0);
        wmb();
        pr_info("tx_isp_vic_start: MIPI config done, VIC RUN issued (reg 0x0 = 1)\n");

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

        /* OEM BN: VIC RUN issued immediately */
        writel(1, vic_regs + 0x0);
        wmb();
        pr_info("tx_isp_vic_start: BT601 config done, VIC RUN issued\n");

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

        /* OEM BN: VIC RUN issued immediately */
        writel(1, vic_regs + 0x0);
        wmb();
        pr_info("tx_isp_vic_start: BT656 config done, VIC RUN issued\n");

    } else if (interface_type == TX_SENSOR_DATA_INTERFACE_BT1120) {
        /* BT1120 - Binary Ninja 00010500-00010684 */
        pr_info("BT1120 interface configuration\n");

        writel(4, vic_regs + 0xc);
        writel(0x800c0000, vic_regs + 0x10);
        writel((actual_width << 16) | actual_height, vic_regs + 0x4);
        writel(actual_width << 1, vic_regs + 0x18);
        writel(0x100010, vic_regs + 0x1a4);
        writel(0x4440, vic_regs + 0x1ac);

        /* OEM BN: VIC RUN issued immediately */
        writel(1, vic_regs + 0x0);
        wmb();
        pr_info("tx_isp_vic_start: BT1120 config done, VIC RUN issued\n");

    } else {
        pr_info("Unsupported interface type %d\n", interface_type);
        return -1;
    }

    /* Binary Ninja: 00010b48-00010b74 - Log WDR mode */
    if (sensor_attr->wdr_cache != 0)
        pr_info("%s:%d::wdr mode\n", "tx_isp_vic_start", __LINE__);
    else
        pr_info("%s:%d::linear mode\n", "tx_isp_vic_start", __LINE__);

    stride = vic_mdma_stride_resolve(vic_dev, actual_width, vic_dev->pixel_format);
    vic_reassert_core_irq_route(vic_dev, actual_width, actual_height, stride,
                                "tx_isp_vic_start");
    vic_program_irq_registers(vic_dev, "tx_isp_vic_start");

    /* Binary Ninja: 00010b84 - Set vic_start_ok */
    debug_vic_start_ok_change(1, __func__, __LINE__);

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
                    /* Write offset 0x0 of the MMIO base stored in the raw +0xb8 slot,
                     * not MMIO offset 0xb8.
                     */
                    if (vic_raw_regs_get(vic_dev)) {
                        writel(0x10, vic_raw_regs_get(vic_dev));
                    }
                    return 0;

                case 0x2000013:
                    /* Binary Ninja: **($a0 + 0xb8) = 0, then = 4 */
                    /* Write offset 0x0 of the MMIO base stored in the raw +0xb8 slot,
                     * not MMIO offset 0xb8.
                     */
                    if (vic_raw_regs_get(vic_dev)) {
                        writel(0, vic_raw_regs_get(vic_dev));
                        writel(4, vic_raw_regs_get(vic_dev));
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
    size_t sensor_attr_bytes = 0x4c;

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
        memset(&vic_dev->sensor_attr, 0, sensor_attr_bytes);
        vic_dev->sensor_attr_ptr = NULL;
    } else {
        /* Copy first 0x4c bytes (OEM partial cache) */
        memcpy(&vic_dev->sensor_attr, attr, sensor_attr_bytes);
        /* OEM: store pointer to the FULL sensor_attr for tx_isp_vic_start */
        vic_dev->sensor_attr_ptr = attr;
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

    if (cmd == 0x1000001) {
        result = -ENOTSUPP;
        if (sd != NULL) {
            struct tx_isp_subdev_pad *inpad0;

            /* Binary Ninja: void* $v0_2 = *(*(arg1 + 0xc4) + 0xc) */
            inpad0 = tx_isp_subdev_raw_inpad_get(sd, 0);
            if (inpad0 && inpad0->priv) {
                callback_ptr = inpad0->priv;
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
            vic_dev->buffer_count = (buffer_count > 5) ? 5 : buffer_count;
            vic_dev->active_buffer_count = 0;
            pr_info("*** vic_core_ops_ioctl: Channel 0 - buffer_count=%u active_buffer_count reset to %u ***\n",
                    vic_dev->buffer_count, vic_dev->active_buffer_count);
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
            struct tx_isp_subdev_pad *inpad0;

            /* Binary Ninja: void* $v0_5 = **(arg1 + 0xc4) */
            inpad0 = tx_isp_subdev_raw_inpad_get(sd, 0);
            if (inpad0 && inpad0->priv) {
                callback_ptr = inpad0->priv;
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
        pr_info("vic_core_ops_ioctl: Unknown cmd=0x%x, returning -ENOIOCTLCMD\n", cmd);
        return -ENOIOCTLCMD;
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

    if (!sd)
        return -EINVAL;

    vic_dev = (struct tx_isp_vic_device *)tx_isp_get_subdevdata(sd);
    if (!vic_dev) {
        pr_err("VIC device is NULL\n");
        return -EINVAL;
    }

    mutex_lock(&vic_dev->state_lock);

    if (vic_raw_state_get(vic_dev) == 1) {
        vic_raw_state_set(vic_dev, 2); /* INIT -> READY */
        pr_info("VIC activated: state %d -> 2 (READY)\n", 1);
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
    state = vic_raw_state_get(vic_dev);

    /* Binary Ninja: if (arg2 == 0) - disable path */
    if (enable == 0) {
        /* Binary Ninja: if ($v0_2 != 2) */
        if (state != 2) {
            /* Binary Ninja: tx_vic_disable_irq() */
            tx_vic_disable_irq(vic_dev);
            /* Binary Ninja: *($s1_1 + 0x128) = 2 */
            vic_raw_state_set(vic_dev, 2);
            pr_info("vic_core_ops_init: Disabled VIC, state -> 2\n");
        }
    } else {
        /* Binary Ninja: else - enable path */
        /* Binary Ninja: if ($v0_2 != 3) */
        if (state != 3) {
            /* Binary Ninja: tx_vic_enable_irq() */
            tx_vic_enable_irq(vic_dev);
            /* Binary Ninja: *($s1_1 + 0x128) = 3 */
            vic_raw_state_set(vic_dev, 3);
            pr_info("vic_core_ops_init: Enabled VIC, state -> 3\n");
        }
    }

    pr_info("vic_core_ops_init: enable=%d, final state=%d\n", enable,
            vic_raw_state_get(vic_dev));
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

    pr_info("*** tx_isp_vic_slake_subdev: ENTRY - state=%d ***\n",
            vic_raw_state_get(vic_dev));

    /* Binary Ninja: Read state at offset 0x128 */
    state = vic_raw_state_get(vic_dev);

    /* Binary Ninja: if (state == 4) call vic_core_s_stream and re-read state */
    if (state == 4) {
        pr_info("tx_isp_vic_slake_subdev: VIC streaming (state=4) -> stop stream\n");
        vic_core_s_stream(sd, 0);
        state = vic_raw_state_get(vic_dev);  /* Re-read state after stopping stream */
    }

    /* Binary Ninja: if (state == 3) call vic_core_ops_init(sd, 0) */
    if (state == 3) {
        pr_info("tx_isp_vic_slake_subdev: VIC state 3 -> vic_core_ops_init(0)\n");
        vic_core_ops_init(sd, 0);
    }

    /* Binary Ninja: Lock mutex at offset 0x130 (state_lock) */
    mutex_lock(&vic_dev->state_lock);

    /* Binary Ninja: Only transition from state 2 to state 1 */
    if (vic_raw_state_get(vic_dev) == 2) {
        vic_raw_state_set(vic_dev, 1);
        pr_info("tx_isp_vic_slake_subdev: VIC state 2 -> 1 (INIT)\n");
    }

    mutex_unlock(&vic_dev->state_lock);

    pr_info("*** tx_isp_vic_slake_subdev: EXIT - state=%d ***\n",
            vic_raw_state_get(vic_dev));
    return 0;
}

/* VIC PIPO MDMA Enable function - OEM HLIL: uses *(arg1 + 0xb8) for all writes.
 *
 * OEM vic_pipo_mdma_enable only sets stride/size/enable config registers.
 * Buffer addresses are programmed by ispvic_frame_channel_qbuf (0x3000005 events)
 * BEFORE this function is called via the 0x3000003 STREAMON event.
 */
static void vic_pipo_mdma_enable(struct tx_isp_vic_device *vic_dev)
{
    void __iomem *vic_base;
    u32 width, height, stride, pixfmt;

    if (!vic_dev) {
        pr_err("vic_pipo_mdma_enable: NULL vic_dev parameter\n");
        return;
    }

    vic_base = vic_raw_regs_get(vic_dev);
    if (!vic_base)
        vic_base = vic_primary_regs_resolve(vic_dev);
    if (!vic_base || (unsigned long)vic_base < 0x80000000) {
        pr_err("vic_pipo_mdma_enable: Invalid VIC register base %p\n", vic_base);
        return;
    }

    width = vic_dev->width ? vic_dev->width : vic_raw_width_get(vic_dev);
    height = vic_dev->height ? vic_dev->height : vic_raw_height_get(vic_dev);
    if (width == 0 || height == 0) {
        width = vic_raw_width_get(vic_dev);
        height = vic_raw_height_get(vic_dev);
    }
    if (width == 0 || height == 0) {
        width = width ? width : 1920;
        height = height ? height : 1080;
    }

    pixfmt = vic_dev->pixel_format;
    /* OEM BN/HLIL programs both MDMA stride registers with width<<1 even when
     * the userspace-visible format is NV12. Keep any larger cached bytesperline,
     * but never go below the OEM 16bpp-style stride.
     */
    stride = vic_mdma_stride_resolve(vic_dev, width, pixfmt);

    pr_info("vic_pipo_mdma_enable: base=%p dims=%dx%d stride=%u pixfmt=0x%x cached_stride=%u\n",
            vic_base, width, height, stride, pixfmt, vic_dev->stride);

    /* OEM HLIL: MDMA config registers — stride/size/enable only */
    writel(1, vic_base + 0x308);                           /* MDMA enable */
    writel((width << 16) | height, vic_base + 0x304);      /* frame size */
    writel(stride, vic_base + 0x310);                      /* Y stride */
    writel(stride, vic_base + 0x314);                      /* UV stride */

    wmb();

    pr_info("vic_pipo_mdma_enable: ctrl308=0x%x size304=0x%x stride310=0x%x stride314=0x%x slot0=0x%x slot1=0x%x\n",
            readl(vic_base + 0x308), readl(vic_base + 0x304),
            readl(vic_base + 0x310), readl(vic_base + 0x314),
            readl(vic_base + 0x318), readl(vic_base + 0x31c));
}

/* ISPVIC Frame Channel S_Stream - OEM HLIL EXACT Implementation
 * OEM: $s0 = *(arg1 + 0xd4)   -- vic_dev self pointer
 * OEM: *(*($s0 + 0xb8) + 0x300) = ctrl   -- uses raw +0xb8 slot
 */
int ispvic_frame_channel_s_stream(struct tx_isp_vic_device *vic_dev, int enable)
{
    void __iomem *vic_base = NULL;
    int32_t var_18 = 0;
    const char *stream_op;

    if (!vic_dev) {
        pr_err("%s[%d]: invalid parameter\n", "ispvic_frame_channel_s_stream", __LINE__);
        return -EINVAL;
    }

    stream_op = (enable != 0) ? "streamon" : "streamoff";
    pr_info("%s[%d]: %s\n", "ispvic_frame_channel_s_stream", __LINE__, stream_op);

    /* OEM HLIL: all accesses via *($s0 + 0xb8) — the raw slot */
    vic_base = vic_raw_regs_get(vic_dev);
    if (!vic_base)
        vic_base = vic_primary_regs_resolve(vic_dev);
    if (!vic_base || (unsigned long)vic_base < 0x80000000) {
        pr_err("ispvic_frame_channel_s_stream: invalid VIC reg base %p\n", vic_base);
        return -EINVAL;
    }

    /* OEM HLIL: if (arg2 == *($s0 + 0x210)) return 0 */
    if (enable == vic_dev->stream_state)
        return 0;

    /* OEM HLIL: __private_spin_lock_irqsave($s0 + 0x1f4, &var_18) */
    __private_spin_lock_irqsave(&vic_dev->buffer_mgmt_lock, &var_18);

    if (enable == 0) {
        /* OEM HLIL: *(*($s0 + 0xb8) + 0x300) = 0 */
        writel(0, vic_base + 0x300);
        /* OEM HLIL: *($s0 + 0x210) = 0, *($s0 + 0x214) = 0 */
        vic_dev->stream_state = 0;
        vic_dev->streaming = 0;
	    } else {
	        u32 active_banks;

	        /* OEM BN: vic_pipo_mdma_enable($s0) */
	        vic_pipo_mdma_enable(vic_dev);
		    vic_program_irq_registers(vic_dev, "ispvic_frame_channel_s_stream");
	        active_banks = vic_dev->active_buffer_count;
	        if (active_banks > 5)
	            active_banks = 5;
	        /* OEM BN EXACT: *(regs + 0x300) = *(s0 + 0x218) << 16 | 0x80000020
	         * No format bits in [2:0]. The OEM uses 0x80000020 unconditionally.
	         * Offset 0x218 in the OEM struct is the number of programmed banks,
	         * not an unconditional 5-bank default.
	         */
	        {
	            u32 ctrl = (active_banks << 16) | 0x80000020;

	            writel(ctrl, vic_base + 0x300);
	            wmb();
	            pr_info("ispvic_frame_channel_s_stream: ctrl=0x%x readback=0x%x base=%p\n",
	                    ctrl, readl(vic_base + 0x300), vic_base);
	        }
        /* OEM HLIL: *($s0 + 0x210) = 1, *($s0 + 0x214) = 1 */
        vic_dev->stream_state = 1;
        vic_dev->streaming = 1;
    }

    /* OEM HLIL: private_spin_unlock_irqrestore($s0 + 0x1f4, var_18) */
    private_spin_unlock_irqrestore(&vic_dev->buffer_mgmt_lock, var_18);
    return 0;
}

/* OEM-style event-dispatch object for the live VIC subdev.
 * tx_isp_send_event_to_remote() treats sd->event_callback_struct as a
 * tx_isp_channel_config-compatible table, so bind VIC the same way the core
 * channels are bound: handler at +0x1c, priv/live object at +0x20.
 */
static struct tx_isp_channel_config vic_event_dispatch;

/* VIC event callback handler for frame-channel REQBUFS/QBUF/STREAM events. */
static int vic_pad_event_handler(void *priv, unsigned int cmd, void *data)
{
    struct tx_isp_channel_config *dispatch = priv;
    struct tx_isp_vic_device *vic_dev;
    struct tx_isp_subdev *sd;
    int ret = 0;

    if (!dispatch) {
        pr_err("VIC event callback: Missing dispatch table\n");
        return -EINVAL;
    }

    vic_dev = (struct tx_isp_vic_device *)dispatch->event_priv;
    sd = vic_dev ? &vic_dev->sd : NULL;
    if (!vic_dev || !sd) {
        pr_err("VIC event callback: No vic_dev/subdev (dispatch=%p)\n",
               dispatch);
        return -EINVAL;
    }

    pr_info("*** VIC EVENT CALLBACK: cmd=0x%x, data=%p, vic_dev=%p ***\n",
            cmd, data, vic_dev);

    switch (cmd) {
        case 0x3000003:
            pr_info("*** VIC EVENT: STREAM_START (0x3000003) - ACTIVATING VIC HARDWARE ***\n");
            /* Regression fix: frame-channel STREAM_START must still arm the
             * MDMA/control path even when the VIC core is already in state 4.
             * The last known green-stream commit routed this event through the
             * frame-channel helper, while vic_core_s_stream() becomes a no-op
             * once the VIC has already been started by the main stream path.
             */
            ret = ispvic_frame_channel_s_stream(vic_dev, 1);
            break;
        case 0x3000004:
            pr_info("*** VIC EVENT: STREAM_STOP/CANCEL (0x3000004) - DEACTIVATING VIC HARDWARE ***\n");
            ret = ispvic_frame_channel_s_stream(vic_dev, 0);
            break;
        case 0x3000005:
            pr_info("*** VIC EVENT: BUFFER_ENQUEUE (0x3000005) via dispatch ***\n");
            ret = data ? vic_core_ops_ioctl(sd, cmd, data) : -EINVAL;
            break;
        case 0x3000008:
            pr_info("*** VIC EVENT: REQBUFS (0x3000008) via dispatch ***\n");
            ret = data ? vic_core_ops_ioctl(sd, cmd, data) : -EINVAL;
            break;
        default:
            pr_info("VIC: Unknown event cmd=0x%x\n", cmd);
            ret = -ENOIOCTLCMD;
            break;
    }

    pr_info("*** VIC EVENT CALLBACK: returning %d ***\n", ret);
    return ret;
}

static void vic_bind_event_dispatch_table(struct tx_isp_vic_device *vic_dev)
{
    if (!vic_dev)
        return;

    memset(&vic_event_dispatch, 0, sizeof(vic_event_dispatch));
    vic_event_dispatch.channel_id = 0;
    vic_event_dispatch.enabled = 1;
    vic_event_dispatch.state = 3;
    vic_event_dispatch.event_handler = vic_pad_event_handler;
    vic_event_dispatch.event_priv = vic_dev;

    vic_dev->event_callback_struct = &vic_event_dispatch;

    pr_info("*** VIC event dispatch bound: sd=%p table=%p handler=%p priv=%p ***\n",
            &vic_dev->sd, &vic_event_dispatch,
            vic_event_dispatch.event_handler,
            vic_event_dispatch.event_priv);
}

/* BINARY NINJA EXACT: vic_core_s_stream implementation with CSI register updates */
/* BINARY NINJA EXACT: vic_core_s_stream implementation with CSI register updates */
int vic_core_s_stream(struct tx_isp_subdev *sd, int enable)
{
    struct tx_isp_vic_device *vic_dev;
    u32 current_state;
    int ret = -EINVAL;

    if (!sd || (unsigned long)sd >= 0xfffff001)
        return -EINVAL;

    vic_dev = vic_raw_self_from_sd(sd);
    if (!vic_dev || (unsigned long)vic_dev >= 0xfffff001)
        return -EINVAL;

    current_state = vic_dev->state;

    if (enable == 0) {
        /* Do NOT reset vic_dev->state to 3 here.
         * VIC hardware stays running across the OEM stop-start cycle.
         * Resetting to 3 causes the second enable=1 call to re-arm
         * VIC, which always times out because hardware is already running.
         */
        return 0;
    }

    /* OEM BN: disable_irq -> vic_start -> state=4 -> enable_irq. No extras. */
    if (current_state != 4) {
        tx_vic_disable_irq(vic_dev);
        ret = tx_isp_vic_start(vic_dev);
        vic_dev->state = 4;
        tx_vic_enable_irq(vic_dev);
        return ret;
    }

    return 0;
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

struct tx_isp_vic_device *dump_vsd = NULL;
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

    /* tx_isp_subdev_init() repopulates a large portion of the embedded subdev.
     * Re-seed the OEM raw self slot and private-data pointer afterwards so the
     * stream-on hot path sees the same layout assumptions as stock.
     */
    tx_isp_set_subdevdata(sd, vic_dev);
    tx_isp_set_subdev_hostdata(sd, vic_dev);
    vic_bind_event_dispatch_table(vic_dev);
    vic_raw_self_set(vic_dev);
    {
        void __iomem *seed_regs = vic_stream_regs_resolve(vic_dev);
        u32 seed_width = vic_raw_width_get(vic_dev);
        u32 seed_height = vic_raw_height_get(vic_dev);

        if ((!seed_width || !seed_height) &&
            vic_dev->sensor_attr.total_width && vic_dev->sensor_attr.total_height) {
            seed_width = vic_dev->sensor_attr.total_width;
            seed_height = vic_dev->sensor_attr.total_height;
        }
        if ((!seed_width || !seed_height) && ourISPdev &&
            ourISPdev->sensor_width && ourISPdev->sensor_height) {
            seed_width = ourISPdev->sensor_width;
            seed_height = ourISPdev->sensor_height;
        }
        if (!seed_width || !seed_height) {
            seed_width = 1920;
            seed_height = 1080;
        }

        vic_dev->width = seed_width;
        vic_dev->height = seed_height;
        vic_raw_regs_set(vic_dev, seed_regs);
        vic_raw_dims_set(vic_dev, seed_width, seed_height);
    }
    vic_raw_sensor_attr_sync(vic_dev, &vic_dev->sensor_attr);
    pr_info("*** tx_isp_vic_probe: Re-seeded raw self/dev_priv after subdev init (raw_self=%p dev_priv=%p) ***\n",
            vic_raw_self_from_sd(sd), tx_isp_get_subdevdata(sd));
    pr_info("*** tx_isp_vic_probe: Re-seeded OEM VIC slots regs=%p dims=%ux%u raw_attr=%p ***\n",
            vic_raw_regs_get(vic_dev), vic_raw_width_get(vic_dev),
            vic_raw_height_get(vic_dev), vic_raw_sensor_attr_get(vic_dev));

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

    /* Restore load-time VIC MMIO bring-up so insmod reaches the real hardware
     * path before userspace starts probing the pipeline.
     */
    pr_info("*** tx_isp_vic_probe: Calling tx_isp_vic_hw_init during probe ***\n");
    ret = tx_isp_vic_hw_init(sd);
    if (ret != 0) {
        pr_err("*** tx_isp_vic_probe: tx_isp_vic_hw_init failed: %d ***\n", ret);
        return ret;
    }

    pr_info("*** tx_isp_vic_probe: VIC hardware init complete, leaving state at %d ***\n",
            vic_raw_state_get(vic_dev));

    /* Store global reference (binary uses 'dump_vsd' global) */
    dump_vsd = vic_dev;
    vic_dev->irq = vic_resolve_irq_number(vic_dev);
    vic_dev->irq_number = vic_dev->irq;
    vic_raw_irq_flag_set(vic_dev, 0);
    vic_raw_irq_slot_seed(vic_dev, vic_dev->irq);

    /* Set test_addr to point to irqdev (the OEM +0x80 was the irqdev offset) */
    test_addr = (char *)&vic_dev->sd.irqdev;

    pr_info("*** tx_isp_vic_probe: VIC device initialized successfully ***\n");
    pr_info("VIC device: vic_dev=%p, size=%zu\n", vic_dev, sizeof(struct tx_isp_vic_device));
    pr_info("  sd: %p\n", sd);
    pr_info("  state: %u\n", vic_raw_state_get(vic_dev));
    pr_info("  irqdev_addr: %p\n", test_addr);

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

/* OEM BN: queue incoming node, consume one free bank, program one slot, move bank to active list. */
static int ispvic_frame_channel_qbuf(void *arg1, void *arg2)
{
	struct tx_isp_vic_device *vic_dev = NULL;
	struct tx_isp_subdev *sd = (struct tx_isp_subdev *)arg1;
	struct vic_buffer_entry *incoming = (struct vic_buffer_entry *)arg2;
	struct vic_buffer_entry *queued_buffer;
	struct vic_buffer_entry *bank_buffer;
	unsigned long irq_flags = 0;
	void __iomem *vic_base;
	u32 buffer_addr, buffer_index, reg_offset;
	bool attempted_bootstrap = false;
	void *raw_pipe[5] = { 0 };

	pr_info("*** ispvic_frame_channel_qbuf: entry arg1=%p, arg2=%p ***\n", arg1, arg2);

	if (sd != NULL && (unsigned long)sd < 0xfffff001)
		vic_dev = (struct tx_isp_vic_device *)tx_isp_get_subdev_hostdata(sd);

	if (!vic_dev)
		return 0;

	vic_base = vic_stream_regs_resolve(vic_dev);
	if (!vic_base)
		return 0;

	retry_qbuf:
	private_spin_lock_irqsave(&vic_dev->buffer_mgmt_lock, irq_flags);

	if (list_empty(&vic_dev->free_head) && list_empty(&vic_dev->done_head) &&
	    list_empty(&vic_dev->queue_head)) {
		private_spin_unlock_irqrestore(&vic_dev->buffer_mgmt_lock, irq_flags);
		if (!attempted_bootstrap) {
			attempted_bootstrap = true;
			pr_info("*** ispvic_frame_channel_qbuf: free list empty, bootstrapping pipo ***\n");
			tx_isp_subdev_pipo(sd, raw_pipe);
			goto retry_qbuf;
		}
	}

	if (incoming) {
		list_add_tail(&incoming->list, &vic_dev->queue_head);
		incoming = NULL;
	}

	if (list_empty(&vic_dev->queue_head)) {
		pr_info("ispvic_frame_channel_qbuf: qbuffer null\n");
		private_spin_unlock_irqrestore(&vic_dev->buffer_mgmt_lock, irq_flags);
		return 0;
	}

	if (list_empty(&vic_dev->free_head)) {
		pr_info("ispvic_frame_channel_qbuf: bank no free\n");
		private_spin_unlock_irqrestore(&vic_dev->buffer_mgmt_lock, irq_flags);
		return 0;
	}

	queued_buffer = list_first_entry(&vic_dev->queue_head, struct vic_buffer_entry, list);
	list_del(&queued_buffer->list);

	bank_buffer = list_first_entry(&vic_dev->free_head, struct vic_buffer_entry, list);
	list_del(&bank_buffer->list);

	buffer_addr = queued_buffer->buffer_addr;
	buffer_index = bank_buffer->buffer_index;
	bank_buffer->buffer_addr = buffer_addr;
	bank_buffer->channel = queued_buffer->channel;
	bank_buffer->buffer_length = queued_buffer->buffer_length;
	reg_offset = (buffer_index + 0xc6) << 2;

	/* OEM BN: program exactly one slot base address at (bank + 0xc6) << 2.
	 * The VIC/MDMA path handles the contiguous output layout; QBUF does not
	 * program separate UV/shadow plane registers.
	 */
	writel(buffer_addr, vic_base + reg_offset);
	wmb();

	list_add_tail(&bank_buffer->list, &vic_dev->done_head);
	vic_dev->active_buffer_count += 1;

	pr_info("*** VIC QBUF: bank=%u base=0x%x len=%u reg_off=0x%x active=%u ***\n",
		buffer_index, buffer_addr,
		bank_buffer->buffer_length,
		reg_offset,
		vic_dev->active_buffer_count);

	private_spin_unlock_irqrestore(&vic_dev->buffer_mgmt_lock, irq_flags);
	kfree(queued_buffer);

	/* CRITICAL FIX: Deferred VIC MDMA enable.
	 * The OEM enables MDMA via event 0x3000003 (VIDIOC_STREAMON on frame
	 * channel).  In our flow libimp never sends STREAMON to the frame channel
	 * — it uses link_stream instead.  So MDMA never gets armed and the ISP
	 * core writes frames without proper bank rotation → frame tearing.
	 *
	 * Trigger MDMA enable on the first QBUF when buffers are available.
	 */
	if (vic_dev->stream_state == 0 && vic_dev->active_buffer_count >= 3) {
		pr_info("*** VIC QBUF: DEFERRED MDMA ENABLE - %u banks ready, arming MDMA now ***\n",
			vic_dev->active_buffer_count);
		ispvic_frame_channel_s_stream(vic_dev, 1);
	}

	return 0;
}

/* ISPVIC Frame Channel Clear Buffer - drain lists and clear slots safely */
static int ispvic_frame_channel_clearbuf(void)
{
    struct tx_isp_vic_device *vic_dev = NULL;
    unsigned long flags;
    int i;

    if (!ourISPdev)
        return -ENODEV;
    vic_dev = (struct tx_isp_vic_device *)ourISPdev->vic_dev;
    if (!vic_dev)
        return -ENODEV;

	/* Drain pending queue nodes, recycle active bank nodes, and reset stream state. */
    spin_lock_irqsave(&vic_dev->buffer_mgmt_lock, flags);
    if (!list_empty(&vic_dev->queue_head)) {
        struct list_head *pos, *n;
        list_for_each_safe(pos, n, &vic_dev->queue_head) {
            list_del(pos);
			kfree(list_entry(pos, struct vic_buffer_entry, list));
        }
    }
    if (!list_empty(&vic_dev->done_head)) {
        struct list_head *pos, *n;
        list_for_each_safe(pos, n, &vic_dev->done_head) {
			struct vic_buffer_entry *entry = list_entry(pos, struct vic_buffer_entry, list);
			entry->buffer_addr = 0;
			entry->channel = 0;
				entry->buffer_length = 0;
            list_del(pos);
            list_add_tail(pos, &vic_dev->free_head);
        }
    }
    vic_dev->active_buffer_count = 0;
    vic_dev->processing = 0;
    vic_dev->stream_state = 0; /* OEM clears 0x210 (stream) and 0x214 (processing) */
    vic_dev->streaming = 0;
    spin_unlock_irqrestore(&vic_dev->buffer_mgmt_lock, flags);

    /* Clear programmed VIC buffer slots safely */
    if (vic_dev->vic_regs) {
        void __iomem *r = vic_dev->vic_regs;
        for (i = 0; i < 5; ++i) {
            writel(0, r + (0x318 + (i * 4))); /* Y base */
            writel(0, r + (0x32c + (i * 4))); /* shadow/alt base */
            writel(0, r + (0x340 + (i * 4))); /* UV base */
        }
        wmb();
    }
    if (vic_dev->vic_regs_secondary) {
        void __iomem *r = vic_dev->vic_regs_secondary;
        for (i = 0; i < 5; ++i) {
            writel(0, r + (0x318 + (i * 4)));
            writel(0, r + (0x32c + (i * 4)));
            writel(0, r + (0x340 + (i * 4)));
        }
        wmb();
    }
    pr_info("ispvic_frame_channel_clearbuf: drained lists, cleared slots, mdma reset\n");
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

		/* Re-seed the OEM-style queue/free/active lists with persistent nodes. */
		vic_free_buffer_list(&vic_dev->queue_head);
		vic_free_buffer_list(&vic_dev->done_head);
		vic_free_buffer_list(&vic_dev->free_head);

        INIT_LIST_HEAD(&vic_dev->queue_head);
        INIT_LIST_HEAD(&vic_dev->done_head);
        INIT_LIST_HEAD(&vic_dev->free_head);

        pr_info("tx_isp_subdev_pipo: initialized linked list heads (safe Linux API)\n");

        /* SAFE: Use proper spinlock initialization */
        spin_lock_init(&vic_dev->buffer_mgmt_lock);
        pr_info("tx_isp_subdev_pipo: initialized spinlock\n");

        /* SAFE: Set function pointers using proper array indexing.
         * raw_pipe[1] is the DQBUF callback set by the DOWNSTREAM consumer
         * (libimp frame channel) — preserve whatever the caller already put there.
         */
        raw_pipe[0] = (void *)ispvic_frame_channel_qbuf;
        raw_pipe[2] = (void *)ispvic_frame_channel_clearbuf;  /* offset 8 / 4 = index 2 */
        raw_pipe[3] = (void *)ispvic_frame_channel_s_stream;  /* offset 0xc / 4 = index 3 */
        raw_pipe[4] = (void *)sd;                             /* offset 0x10 / 4 = index 4 */

        /* Persist into the global so vic_mdma_irq_function can deliver frames */
        memcpy(raw_pipe_global, raw_pipe, sizeof(raw_pipe_global));

        pr_info("tx_isp_subdev_pipo: set function pointers - qbuf=%p, dqbuf=%p, clearbuf=%p, s_stream=%p, sd=%p\n",
                raw_pipe_global[0], raw_pipe_global[1],
                raw_pipe_global[2], raw_pipe_global[3], raw_pipe_global[4]);

		vic_dev->active_buffer_count = 0;

        for (i = 0; i < 5; i++) {
			struct vic_buffer_entry *buffer_entry;

            if (i < sizeof(vic_dev->buffer_index) / sizeof(vic_dev->buffer_index[0])) {
                vic_dev->buffer_index[i] = i;
            }

			buffer_entry = kzalloc(sizeof(*buffer_entry), GFP_KERNEL);
            if (buffer_entry) {
				buffer_entry->buffer_addr = 0;
				buffer_entry->buffer_index = i;
				buffer_entry->channel = 0;
					buffer_entry->buffer_length = 0;
				list_add_tail(&buffer_entry->list, &vic_dev->free_head);
                pr_info("tx_isp_subdev_pipo: added buffer entry %d to free list\n", i);
            }

            /* NOTE: Do NOT clear VIC buffer registers (0x318-0x328) here.
             * vic_pipo_mdma_enable programs them with actual DMA addresses
             * when streaming starts.  Clearing them here was preventing
             * MDMA from having valid destination addresses.
             */
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
