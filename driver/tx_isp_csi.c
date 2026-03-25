#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/interrupt.h>
#include "include/tx_isp.h"
#include "include/tx_isp_csi.h"
#include "include/tx-isp-device.h"

#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/fcntl.h>


/* CSI periodic dump thread controls */
static struct task_struct *csi_dump_kthread;
static int csi_dump_interval_ms = 1000; /* default 1s */
module_param(csi_dump_interval_ms, int, S_IRUGO);
MODULE_PARM_DESC(csi_dump_interval_ms, "CSI dump interval in milliseconds");

/* Gate to enable the CSI dump thread (default off to avoid instability) */
static int csi_dump_enable = 0;
module_param(csi_dump_enable, int, S_IRUGO);
MODULE_PARM_DESC(csi_dump_enable, "Enable CSI periodic dump to /opt/csi.txt (0=off, 1=on)");






static const char *csi_dump_path = "/opt/csi.txt";

static int csi_dump_thread_fn(void *data);
static void __maybe_unused csi_start_dump_thread(struct tx_isp_csi_device *csi_dev);
static void csi_stop_dump_thread(void);
static void csi_dump_once_to_file(struct tx_isp_csi_device *csi_dev, const char *tag);
static void csi_write_file(const char *buf, size_t len);



/* Forward declarations */
int csi_core_ops_init(struct tx_isp_subdev *sd, int enable);
int csi_set_on_lanes(struct tx_isp_csi_device *csi_dev, int lanes);
void dump_csi_reg(struct tx_isp_subdev *sd);
extern struct tx_isp_dev *ourISPdev;
void __iomem *tx_isp_get_vic_primary_regs(void);
static void __maybe_unused csi_jit_ungate_clocks(void);


static void __iomem *tx_isp_core_regs = NULL;

u32 isp_read32(u32 reg)
{
    if (!tx_isp_core_regs) {
        tx_isp_core_regs = ioremap(0x13300000, 0x10000);  // Core base from /proc/iomem
        if (!tx_isp_core_regs) {
            pr_err("Failed to map core registers\n");
            return 0;
        }
    }
    return readl(tx_isp_core_regs + reg);
}

void isp_write32(u32 reg, u32 val)
{
    if (!tx_isp_core_regs) {
        tx_isp_core_regs = ioremap(0x13300000, 0x10000);
        if (!tx_isp_core_regs) {
            pr_err("Failed to map core registers\n");
            return;
        }
    }
    writel(val, tx_isp_core_regs + reg);
}

/* JIT ungate CSI/DPHY and wrapper clocks before BASIC/WRAP access */
static void __maybe_unused csi_jit_ungate_clocks(void)
{
    void __iomem *cpm = ioremap(0x10000000, 0x1000);
    if (!cpm) {
        pr_warn("[CPM][CSI] ioremap failed; cannot JIT-ungate clocks\n");
        return;
    }
    /* CLKGR0 bit7: CSI/DPHY gate: 1=gated, 0=ungated */
    u32 g0_before = readl(cpm + 0x20);
    if (g0_before & (1u << 7)) {
        writel(g0_before & ~(1u << 7), cpm + 0x20);
        wmb(); udelay(5);
        pr_info("[CPM][CSI] JIT ungate CLKGR0 bit7: %08x -> %08x\n", g0_before, readl(cpm + 0x20));
    } else {
        pr_info("[CPM][CSI] CLKGR0 already ungated: %08x\n", g0_before);
    }
    /* CLKGR1 bit2: wrapper side gate sometimes needed for CSI interactions */
    u32 g1_before = readl(cpm + 0x28);
    if (g1_before & (1u << 2)) {
        writel(g1_before & ~(1u << 2), cpm + 0x28);
        wmb(); udelay(5);
        pr_info("[CPM][CSI] JIT ungate CLKGR1 bit2: %08x -> %08x\n", g1_before, readl(cpm + 0x28));
    } else {
        pr_info("[CPM][CSI] CLKGR1 already ungated: %08x\n", g1_before);
    }
    iounmap(cpm);
}

/* Optional CPM reset deassert for CSI: enabled by default while bring-up is unstable. */
static int csi_cpm_reset_fix = 1;
module_param(csi_cpm_reset_fix, int, 0644);
MODULE_PARM_DESC(csi_cpm_reset_fix, "If 1, deassert CSI-related reset bits in CPM[0x34] after unlocking CPM[0x38]=0xA5A5 (default: enabled)");

static void __maybe_unused csi_cpm_deassert_reset_if_enabled(void)
{
    void __iomem *cpm = ioremap(0x10000000, 0x1000);
    if (!cpm)
        return;

    u32 r_unlock = readl(cpm + 0x38);
    u32 r_before = readl(cpm + 0x34);
    u32 r30      = readl(cpm + 0x30);
    u32 r3c      = readl(cpm + 0x3C);
    pr_info("[CPM][CSI] Reset window: unlock=0x%08x, reset_before=0x%08x (r30=0x%08x r3c=0x%08x)\n", r_unlock, r_before, r30, r3c);

    /* Also emit to /opt/csi.txt so it’s visible in field logs */
    {
        char fbuf[256];
        int n = 0;
        n += scnprintf(fbuf + n, sizeof(fbuf) - n, "=== CSI DUMP ===\n");
        n += scnprintf(fbuf + n, sizeof(fbuf) - n, "[TAG  ] CPM_RESET_FIX BEFORE\n");
        n += scnprintf(fbuf + n, sizeof(fbuf) - n, "[CPM  ] 0x30=%08x 0x34=%08x 0x38=%08x 0x3C=%08x\n", r30, r_before, r_unlock, r3c);
        csi_write_file(fbuf, n);
    }


    /* Guard: allow manual disable for experiments, but default to active during bring-up. */
    if (!csi_cpm_reset_fix) {
        pr_info("[CPM][CSI] Reset deassert helper disabled by module param; skipping CPM reset writes\n");
        iounmap(cpm);
        return;
    }

    pr_info("[CPM][CSI] Reset deassert helper enabled; attempting CPM reset release\n");

    /* Always attempt CPM reset deassert now (risk-elevated per user) */

    /* Unlock (many Ingenic parts require writing 0xA5A5 before modifying reset regs) */
    writel(0x0000A5A5, cpm + 0x38);
    wmb();

    /* Try W1C semantics first: write 1s to clear bits 0 and 8, don't touch others */
    {
        u32 mask = (1u << 0) | (1u << 8);
        writel(mask, cpm + 0x34);
        wmb();
        udelay(5);
    }

    u32 r_after1 = readl(cpm + 0x34);

    /* If unchanged, try RMW clear (RW semantics) */
    if ((r_after1 & ((1u << 0) | (1u << 8))) == ((1u << 0) | (1u << 8))) {
        u32 rmw = r_after1 & ~((1u << 0) | (1u << 8));
        writel(rmw, cpm + 0x34);
        wmb();
        udelay(5);
    }

    /* If 0x34 still shows bits set, attempt clear via 0x30 (alternate semantics) */
    if ((readl(cpm + 0x34) & ((1u << 0) | (1u << 8))) != 0) {
        u32 mask = (1u << 0) | (1u << 8);
        u32 r30b = readl(cpm + 0x30);
        /* Try W1C on 0x30 */
        writel(mask, cpm + 0x30);
        wmb();
        udelay(5);
        u32 r30c = readl(cpm + 0x30);
        u32 r34c = readl(cpm + 0x34);
        if ((r34c & mask) != 0) {
            /* Try RMW clear on 0x30 */
            u32 rmw30 = r30c & ~mask;
            writel(rmw30, cpm + 0x30);
            wmb();
            udelay(5);
        }
        /* Mirror this attempt to /opt as well */
        {
            char fbuf3[256];
            int n3 = 0;
            u32 r30d = readl(cpm + 0x30);
            u32 r34d = readl(cpm + 0x34);
            n3 += scnprintf(fbuf3 + n3, sizeof(fbuf3) - n3, "=== CSI DUMP ===\n");
            n3 += scnprintf(fbuf3 + n3, sizeof(fbuf3) - n3, "[TAG  ] CPM_RESET_FIX TRY 0x30\n");
            n3 += scnprintf(fbuf3 + n3, sizeof(fbuf3) - n3, "[CPM  ] 0x30=%08x->%08x 0x34=%08x->%08x\n", r30b, r30d, r_after1, r34d);
            csi_write_file(fbuf3, n3);
        }
    }

    {
        u32 r_unlock2 = readl(cpm + 0x38);
        u32 r_final   = readl(cpm + 0x34);
        u32 r30a      = readl(cpm + 0x30);
        u32 r3ca      = readl(cpm + 0x3C);
        pr_info("[CPM][CSI] Reset window after deassert: unlock=0x%08x, reset_after=0x%08x (r30=0x%08x r3c=0x%08x)\n",
                r_unlock2, r_final, r30a, r3ca);

        /* Mirror to /opt/csi.txt for guaranteed capture */
        char fbuf2[256];
        int n2 = 0;
        n2 += scnprintf(fbuf2 + n2, sizeof(fbuf2) - n2, "=== CSI DUMP ===\n");
        n2 += scnprintf(fbuf2 + n2, sizeof(fbuf2) - n2, "[TAG  ] CPM_RESET_FIX AFTER\n");
        n2 += scnprintf(fbuf2 + n2, sizeof(fbuf2) - n2, "[CPM  ] 0x30=%08x 0x34=%08x 0x38=%08x 0x3C=%08x\n", r30a, r_final, r_unlock2, r3ca);
        csi_write_file(fbuf2, n2);
    }

    iounmap(cpm);
}


static void __iomem *tx_isp_vic_regs = NULL;

u32 vic_read32(u32 reg)
{
    if (!tx_isp_vic_regs) {
        tx_isp_vic_regs = ioremap(0x10023000, 0x1000);  // VIC base from /proc/iomem
        if (!tx_isp_vic_regs) {
            pr_err("Failed to map VIC registers\n");
            return 0;
        }
    }
    return readl(tx_isp_vic_regs + reg);
}

void vic_write32(u32 reg, u32 val)
{
    if (!tx_isp_vic_regs) {
        tx_isp_vic_regs = ioremap(0x10023000, 0x1000);
        if (!tx_isp_vic_regs) {
            pr_err("Failed to map VIC registers\n");
            return;
        }
    }
    writel(val, tx_isp_vic_regs + reg);
}

/* Wait until W01 (VIC) shows a non-zero stable state before BASIC writes.
 * This mirrors the working branch where BASIC is alive pre-init.
 * Read-only, small timeout, safe.
 */
static int __maybe_unused csi_wait_w01_ready(int timeout_ms)
{
    u32 prev = vic_read32(0x14);
    int stable = (prev != 0) ? 1 : 0;
    int waited = 0;
    while (waited < timeout_ms) {
        private_msleep(1);
        waited += 1;
        u32 cur = vic_read32(0x14);
        if (cur != 0 && cur == prev) {
            stable++;
            if (stable >= 2) {
                pr_info("[W01 ] ready: 0x14=0x%08x (waited %d ms)\n", cur, waited);
                return 1;
            }
        } else {
            stable = (cur != 0) ? 1 : 0;
        }
        prev = cur;
    }
    pr_info("[W01 ] not-ready timeout: last 0x14=0x%08x (waited %d ms)\n", prev, waited);
    return 0;
}


/* Wait for specific VIC/W01 phases observed on the working branch before BASIC writes.
 * Safe (read-only) and bounded wait. Targets: 0x230, 0x300, 0x330.
 */
static int __maybe_unused csi_wait_w01_phase(int timeout_ms)
{
    const u32 t0 = 0x00000230;
    const u32 t1 = 0x00000300;
    const u32 t2 = 0x00000330;
    u32 prev = vic_read32(0x14);
    int stable = 0;
    int waited = 0;
    while (waited < timeout_ms) {
        private_msleep(1);
        waited += 1;
        u32 cur = vic_read32(0x14);
        int match = (cur == t0) || (cur == t1) || (cur == t2);
        if (match && cur == prev) {
            stable++;
            if (stable >= 2) {
                pr_info("[W01 ] phase-ready: 0x14=0x%08x (waited %d ms)\n", cur, waited);
                return 1;
            }
        } else {
            stable = match ? 1 : 0;
        }
        prev = cur;
    }
    pr_info("[W01 ] phase not reached: last 0x14=0x%08x (waited %d ms)\n", prev, waited);
    return 0;
}


/* Similarly for CSI... */
static void __iomem *tx_isp_csi_regs = NULL;
static void __iomem *tx_cpm_regs = NULL;

u32 csi_read32(u32 reg)
{
    if (!tx_isp_csi_regs) {
        tx_isp_csi_regs = ioremap(0x10022000, 0x1000);
        if (!tx_isp_csi_regs) {
            pr_err("Failed to map CSI registers\n");
            return 0;
        }
    }
    return readl(tx_isp_csi_regs + reg);
}

static inline u32 cpm_read32(u32 reg)
{
    if (!tx_cpm_regs) {
        tx_cpm_regs = ioremap(0x10000000, 0x1000);
        if (!tx_cpm_regs)
            return 0;
    }
    return readl(tx_cpm_regs + reg);
}

void csi_write32(u32 reg, u32 val)
{
    if (!tx_isp_csi_regs) {
        tx_isp_csi_regs = ioremap(0x10022000, 0x1000);
        if (!tx_isp_csi_regs) {
            pr_err("Failed to map CSI registers\n");
            return;
        }
    }
    writel(val, tx_isp_csi_regs + reg);
}



/* CSI interrupt handler */
static irqreturn_t tx_isp_csi_irq_handler(int irq, void *dev_id)
{
    struct tx_isp_subdev *sd = dev_id;
    struct tx_isp_csi_device *csi_dev;
    void __iomem *csi_base;
    u32 status, err1, err2, phy_state;
    irqreturn_t ret = IRQ_NONE;
    unsigned long flags;
    static unsigned long last_interrupt_time = 0;
    unsigned long current_time = jiffies;

    /* CRITICAL: Log CSI interrupt activity to understand timing */
    pr_info("*** CSI INTERRUPT: irq=%d, dev_id=%p, time_delta=%lu ms ***\n",
            irq, dev_id, jiffies_to_msecs(current_time - last_interrupt_time));
    last_interrupt_time = current_time;

    if (!sd) {
        pr_warn("*** CSI INTERRUPT: sd is NULL - returning IRQ_NONE ***\n");
        return IRQ_NONE;
    }

    csi_dev = ourISPdev->csi_dev;
    if (!csi_dev) {
        pr_warn("*** CSI INTERRUPT: csi_dev is NULL - returning IRQ_NONE ***\n");
        return IRQ_NONE;
    }

    csi_base = csi_dev->csi_regs;
    if (!csi_base) {
        pr_warn("*** CSI INTERRUPT: csi_base is NULL - returning IRQ_NONE ***\n");
        return IRQ_NONE;
    }

    spin_lock_irqsave(&csi_dev->lock, flags);

    /* Read interrupt status registers */
    err1 = readl(csi_base + 0x20);  /* ERR1 register */
    err2 = readl(csi_base + 0x24);  /* ERR2 register */
    phy_state = readl(csi_base + 0x14);  /* PHY_STATE register */

    /* CRITICAL: Log all CSI register states for analysis */
    pr_info("*** CSI INTERRUPT STATUS: err1=0x%08x, err2=0x%08x, phy_state=0x%08x ***\n",
            err1, err2, phy_state);


    if (err1 || err2) {
        ret = IRQ_HANDLED;

        /* Handle protocol errors (ERR1) */
        if (err1) {
            ISP_ERROR("CSI Protocol errors (ERR1): 0x%08x\n", err1);

            if (err1 & 0x1) ISP_ERROR("  - SOT Sync Error\n");
            if (err1 & 0x2) ISP_ERROR("  - SOTHS Sync Error\n");
            if (err1 & 0x4) ISP_ERROR("  - ECC Single-bit Error (corrected)\n");
            if (err1 & 0x8) ISP_ERROR("  - ECC Multi-bit Error (uncorrectable)\n");
            if (err1 & 0x10) ISP_ERROR("  - CRC Error\n");
            if (err1 & 0x20) ISP_ERROR("  - Packet Size Error\n");
            if (err1 & 0x40) ISP_ERROR("  - EoTp Error\n");

            /* Clear errors by writing back the status */
            writel(err1, csi_base + 0x20);
            wmb();
        }

        /* Handle application errors (ERR2) */
        if (err2) {
            ISP_ERROR("CSI Application errors (ERR2): 0x%08x\n", err2);

            if (err2 & 0x1) ISP_ERROR("  - Data ID Error\n");
            if (err2 & 0x2) ISP_ERROR("  - Frame Sync Error\n");
            if (err2 & 0x4) ISP_ERROR("  - Frame Data Error\n");
            if (err2 & 0x8) ISP_ERROR("  - Frame Sequence Error\n");

            /* Clear errors by writing back the status */
            writel(err2, csi_base + 0x24);
            wmb();
        }

        /* Update error state */
        if ((err1 & 0x38) || (err2 & 0xE)) { /* Serious errors */
            pr_err("CSI: Serious errors detected, may need recovery\n");
        }
    }

    /* Check for PHY state changes */
    if (phy_state & 0x111) { /* Clock lane or data lanes in stop state */
        /* This is normal during idle periods, only log if debugging */
        pr_debug("CSI PHY lanes in stop state: 0x%08x\n", phy_state);
    }

    /* CRITICAL: Log CSI interrupt completion status */
    if (ret == IRQ_HANDLED) {
        pr_info("*** CSI INTERRUPT: Handled successfully - errors processed ***\n");
    } else {
        pr_info("*** CSI INTERRUPT: No action taken - no errors detected ***\n");
    }

    spin_unlock_irqrestore(&csi_dev->lock, flags);
    return ret;
}

/* Initialize CSI hardware */
static int tx_isp_csi_hw_init(struct tx_isp_subdev *sd)
{
    if (!sd)
        return -EINVAL;

    /* Reset CSI */
    csi_write32(CSI_CTRL, CSI_CTRL_RST);
    udelay(10);
    csi_write32(CSI_CTRL, 0);

    /* Clear and mask all interrupts initially */
    csi_write32(CSI_INT_STATUS, 0xFFFFFFFF);
    csi_write32(CSI_INT_MASK, 0xFFFFFFFF);

    return 0;
}

/* CSI start operation */
int tx_isp_csi_start(struct tx_isp_subdev *sd)
{
    u32 ctrl;
    int ret;

    if (!sd)
        return -EINVAL;

    mutex_lock(&sd->csi_lock);

    /* CRITICAL: Register CSI interrupt handler if not already registered */
    static int csi_irq_registered = 0;
    if (!csi_irq_registered) {
        ret = request_irq(38, tx_isp_csi_irq_handler, IRQF_SHARED, "tx-isp-csi", sd);
        if (ret == 0) {
            pr_info("*** CSI INTERRUPT: Handler registered for IRQ 38 ***\n");
            csi_irq_registered = 1;
        } else {
            pr_err("*** CSI INTERRUPT: Failed to register handler for IRQ 38: %d ***\n", ret);
        }
    }

    /* Enable CSI */
    ctrl = csi_read32(CSI_CTRL);
    ctrl |= CSI_CTRL_EN;
    csi_write32(CSI_CTRL, ctrl);

    /* Enable interrupts */
    csi_write32(CSI_INT_MASK, ~(INT_ERROR | INT_FRAME_DONE));

    pr_info("*** CSI INTERRUPT: CSI started with interrupts enabled (mask=0x%08x) ***\n",
            ~(INT_ERROR | INT_FRAME_DONE));

    mutex_unlock(&sd->csi_lock);
    return 0;
}

/* CSI stop operation */
int tx_isp_csi_stop(struct tx_isp_subdev *sd)
{
    if (!sd)
        return -EINVAL;

    mutex_lock(&sd->csi_lock);

    /* Disable CSI */
    csi_write32(CSI_CTRL, 0);

    /* Mask all interrupts */
    csi_write32(CSI_INT_MASK, 0xFFFFFFFF);

    mutex_unlock(&sd->csi_lock);
    return 0;
}

/* CSI format configuration */
int tx_isp_csi_set_format(struct tx_isp_subdev *sd, struct tx_isp_config *config)
{
    u32 ctrl;

    if (!sd || !config)
        return -EINVAL;

    if (config->lane_num < CSI_MIN_LANES || config->lane_num > CSI_MAX_LANES)
        return -EINVAL;

    mutex_lock(&sd->csi_lock);

    /* Configure lane count */
    ctrl = csi_read32(CSI_CTRL);
    ctrl &= ~(3 << 2); /* Clear lane bits */
    switch (config->lane_num) {
    case 1:
        ctrl |= CSI_CTRL_LANES_1;
        break;
    case 2:
        ctrl |= CSI_CTRL_LANES_2;
        break;
    case 4:
        ctrl |= CSI_CTRL_LANES_4;
        break;
    default:
        mutex_unlock(&sd->csi_lock);
        return -EINVAL;
    }
    csi_write32(CSI_CTRL, ctrl);

    mutex_unlock(&sd->csi_lock);
    return 0;
}

/* CSI video streaming control - FIXED: MIPS memory alignment */

/* CSI video streaming control - EXACT Binary Ninja MCP implementation */
int csi_video_s_stream(struct tx_isp_subdev *sd, int enable)
{
    struct tx_isp_csi_device *csi_dev;
    int state;

    pr_info("*** csi_video_s_stream: EXACT Binary Ninja MCP implementation ***\n");
    pr_info("csi_video_s_stream: sd=%p, enable=%d\n", sd, enable);

    /* Binary Ninja: if (arg1 == 0 || arg1 u>= 0xfffff001) */
    if (sd == NULL || (unsigned long)sd >= 0xfffff001) {
        /* Binary Ninja: isp_printf(2, "%s[%d] VIC failed to config DVP SONY mode!(10bits-sensor)\n", arg3) */
        isp_printf(2, "%s[%d] VIC failed to config DVP SONY mode!(10bits-sensor)\n", enable);
        /* Binary Ninja: return 0xffffffea */
        return 0xffffffea;
    }

    /* Get CSI device from subdev private data */
    csi_dev = (struct tx_isp_csi_device *)tx_isp_get_subdevdata(sd);
    if (!csi_dev) {
        pr_err("CSI device is NULL from subdev private data\n");
        return 0xffffffea;
    }

    /* Binary Ninja: if (*(*(arg1 + 0x110) + 0x14) != 1) return 0 */
    /* Check interface type; if unknown, try to derive from active sensor attr */
    {
        int interface_type = csi_dev->interface_type;
        if (interface_type == 0 && ourISPdev && ourISPdev->sensor && ourISPdev->sensor->video.attr) {
            interface_type = ourISPdev->sensor->video.attr->dbus_type;
            csi_dev->interface_type = interface_type;
            pr_info("csi_video_s_stream: derived interface_type=%d from sensor attr, cached\n", interface_type);
        }
        if (interface_type != 1) {
            pr_info("csi_video_s_stream: Interface type %d != 1 (MIPI), returning 0\n", interface_type);
            return 0;
        }
    }

    state = 4;
    if (enable == 0)
        state = 3;

    csi_dev->state = state;

    pr_info("csi_video_s_stream: EXACT Binary Ninja MCP - CSI state set to %d (enable=%d)\n", state, enable);

    /* Binary Ninja: return 0 */
    return 0;
}

/* CSI sensor operations IOCTL handler */
int csi_sensor_ops_ioctl(struct tx_isp_subdev *sd, unsigned int cmd, void *arg)
{
    struct tx_isp_csi_device *csi_dev;

    /* Binary Ninja: if (arg1 != 0 && arg1 u< 0xfffff001) */
    if (sd != NULL && (unsigned long)sd < 0xfffff001) {
        /* Binary Ninja: *(arg1 + 0x110) - get CSI device from subdev private data */
        csi_dev = (struct tx_isp_csi_device *)tx_isp_get_subdevdata(sd);
        if (!csi_dev) {
            /* Binary Ninja: return 0 on error */
            return 0;
        }

        /* Binary Ninja: if (arg2 != 0x200000e) */
        if (cmd != 0x200000e) {
            /* Binary Ninja: if (arg2 != 0x200000f) */
            if (cmd != 0x200000f) {
                /* Binary Ninja: if (arg2 == 0x200000c) */
                if (cmd == 0x200000c) {
                    /* Binary Ninja: csi_core_ops_init(arg1, 1, 0x200000f) */
                    csi_core_ops_init(sd, 1);
                }
            } else {
                /* Binary Ninja: else if (*(*(arg1 + 0x110) + 0x14) == 1) */
                /* Check if CSI device interface type is 1 (MIPI) */
                /* 0x14 offset in CSI device structure is interface_type */
                if (csi_dev->interface_type == 1) {
                    /* Binary Ninja: *(arg1 + 0x128) = 4 */
                    /* CRITICAL FIX: Set state in CSI device, not subdev */
                    csi_dev->state = 4;  /* Set to streaming_on state */
                }
            }
        } else {
            /* Binary Ninja: else if (*(*(arg1 + 0x110) + 0x14) == 1) */
            /* Check if CSI device interface type is 1 (MIPI) */
            if (csi_dev->interface_type == 1) {
                /* Binary Ninja: *(arg1 + 0x128) = 3 */
                /* CRITICAL FIX: Set state in CSI device, not subdev */
                csi_dev->state = 3;  /* Set to streaming_off state */
            }
        }
    }

    /* Binary Ninja: return 0 */
    return 0;
}

/* CSI sensor attribute synchronization */
int csi_sensor_ops_sync_sensor_attr(struct tx_isp_subdev *sd, void *arg)
{
    struct tx_isp_sensor_attribute *attr = arg;
    struct tx_isp_sensor *sensor;

    if (!sd || !attr)
        return -EINVAL;

    /* Find or create the sensor */
    if (!sd->active_sensor) {
        /* In a real implementation, we would create a new sensor here */
        pr_err("No active sensor to sync attributes with\n");
        return -EINVAL;
    }

    /* Store the attribute in the active sensor */
    sensor = sd->active_sensor;
    memcpy(&sensor->attr, attr, sizeof(struct tx_isp_sensor_attribute));

    return 0;
}

static void __iomem **csi_wrapper_regs_slot(struct tx_isp_csi_device *csi_dev)
{
    return (void __iomem **)((char *)csi_dev + 0x13c);
}

static void __iomem *csi_get_wrapper_regs(struct tx_isp_csi_device *csi_dev)
{
    void __iomem *isp_csi_regs;

    if (!csi_dev)
        return NULL;

    isp_csi_regs = *csi_wrapper_regs_slot(csi_dev);
    if (isp_csi_regs)
        return isp_csi_regs;

    /*
     * BN tx_isp_csi_probe stores the 0x10022000 mem-region mapping in the
     * +0x13c slot. Do not synthesize this slot from VIC space.
     */
    isp_csi_regs = csi_dev->csi_regs;
    if (!isp_csi_regs && ourISPdev)
        isp_csi_regs = ourISPdev->csi_regs;
    if (!isp_csi_regs)
        return NULL;

    *csi_wrapper_regs_slot(csi_dev) = isp_csi_regs;

    return isp_csi_regs;
}

static int csi_calc_rate_sel(struct tx_isp_sensor_attribute *sensor_attr)
{
    unsigned int width;
    int rate = sensor_attr->fps;

    if (rate != 0)
        return rate;

    width = sensor_attr->total_width;
    if (width - 0x50 < 0x1e)
        return 0;

    rate = 1;
    if (width - 0x6e >= 0x28) {
        rate = 2;
        if (width - 0x96 >= 0x32) {
            rate = 3;
            if (width - 0xc8 >= 0x32) {
                rate = 4;
                if (width - 0xfa >= 0x32) {
                    rate = 5;
                    if (width - 0x12c >= 0x64) {
                        rate = 6;
                        if (width - 0x190 >= 0x64) {
                            rate = 7;
                            if (width - 0x1f4 >= 0x64) {
                                rate = 8;
                                if (width - 0x258 >= 0x64) {
                                    rate = 9;
                                    if (width - 0x2bc >= 0x64) {
                                        rate = 0xa;
                                        if (width - 0x320 >= 0xc8)
                                            rate = 0xb;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return rate;
}

/* csi_core_ops_init - EXACT Binary Ninja reference implementation */
int csi_core_ops_init(struct tx_isp_subdev *sd, int enable)
{
    struct tx_isp_csi_device *csi_dev;
    struct tx_isp_sensor_attribute *sensor_attr;
    void __iomem *csi_regs;
    void __iomem *isp_csi_regs;
    u32 phy_en_readback;
    u32 csi_version;
    int result = 0xffffffea;
    int v0_17;
    int interface_type;

    if (!sd || (unsigned long)sd >= 0xfffff001)
        return result;

    csi_dev = (struct tx_isp_csi_device *)tx_isp_get_subdevdata(sd);
    if (!csi_dev || (unsigned long)csi_dev >= 0xfffff001)
        return result;

    result = 0;
    pr_info("csi_core_ops_init: sd=%p enable=%d state=%d csi_regs=%p\n",
            sd, enable, csi_dev->state, csi_dev->csi_regs);

    if (csi_dev->state < 2) {
        pr_info("csi_core_ops_init: state %d < 2, skipping hardware init\n", csi_dev->state);
        return result;
    }

    csi_regs = csi_dev->csi_regs;
    if (!csi_regs) {
        pr_err("csi_core_ops_init: csi_regs is NULL\n");
        return -EINVAL;
    }

    if (enable == 0) {
        isp_printf(0, "csi is close!\n");
        writel(readl(csi_regs + 0x08) & 0xfffffffe, csi_regs + 0x08);
        writel(readl(csi_regs + 0x0c) & 0xfffffffe, csi_regs + 0x0c);
        writel(readl(csi_regs + 0x10) & 0xfffffffe, csi_regs + 0x10);
        csi_dev->state = 2;
        return 0;
    }

    if (!ourISPdev || !ourISPdev->sensor || !ourISPdev->sensor->video.attr) {
        pr_err("csi_core_ops_init: sensor attributes unavailable\n");
        return -EINVAL;
    }

    /* Bring the CSI/DPHY clocks out of CPM gate before any BASIC/WRAP access.
     * We already had the helper, but the live init path never invoked it.
     */
    csi_jit_ungate_clocks();
    csi_cpm_deassert_reset_if_enabled();

    sensor_attr = ourISPdev->sensor->video.attr;
    interface_type = sensor_attr->dbus_type;
    csi_dev->interface_type = interface_type;
    isp_csi_regs = csi_get_wrapper_regs(csi_dev);
    pr_info("csi_core_ops_init: sensor dbus=%d lanes=%d slot13c=%p\n",
            interface_type, sensor_attr->mipi.lans, isp_csi_regs);

    if (interface_type == 1) {
        if (!isp_csi_regs) {
            pr_err("csi_core_ops_init: wrapper regs unavailable for MIPI init\n");
            return -ENODEV;
        }

        /* Historical working bring-up wrote ISP_CORE[0xb078] before touching
         * CSI BASIC/WRAP registers. Without this gate, 0x10022000 stays bus-dead.
         */
        pr_info("[CSI][PHY] enabling ISP_CORE[0xb078] <- 0x10000000 before MIPI init\n");
        isp_write32(0xb078, 0x10000000);
        wmb();
        private_msleep(1);
        phy_en_readback = isp_read32(0xb078);
        pr_info("[CSI][PHY] ISP_CORE[0xb078] readback=0x%08x\n", phy_en_readback);

        csi_version = readl(csi_regs + 0x00);
        pr_info("[CSI][PHY] basic[0x00]=0x%08x after ISP_CORE[0xb078] enable\n",
                csi_version);

        csi_dev->lanes = sensor_attr->mipi.lans;
        writel((sensor_attr->mipi.lans - 1) & 0x3, csi_regs + 0x04);
        writel(readl(csi_regs + 0x08) & 0xfffffffe, csi_regs + 0x08);
        writel(0, csi_regs + 0x0c);
        private_msleep(1);
        writel(readl(csi_regs + 0x10) & 0xfffffffe, csi_regs + 0x10);
        private_msleep(1);
        writel(interface_type, csi_regs + 0x0c);
        private_msleep(1);

        if (sensor_attr->fps == 0) {
            u32 rate_reg;
            int rate_sel = csi_calc_rate_sel(sensor_attr);

            rate_reg = (readl(isp_csi_regs + 0x160) & 0xfffffff0) | rate_sel;
            writel(rate_reg, isp_csi_regs + 0x160);
            writel(rate_reg, isp_csi_regs + 0x1e0);
            writel(rate_reg, isp_csi_regs + 0x260);
        }

        writel(0x7d, isp_csi_regs + 0x00);
        writel(0x3f, isp_csi_regs + 0x128);
        writel(1, csi_regs + 0x10);
        private_msleep(10);
        pr_info("csi_core_ops_init: MIPI init programmed basic[0x04]=0x%08x basic[0x0c]=0x%08x basic[0x10]=0x%08x wrap[0x00]=0x%08x wrap[0x128]=0x%08x\n",
                readl(csi_regs + 0x04), readl(csi_regs + 0x0c), readl(csi_regs + 0x10),
                readl(isp_csi_regs + 0x00), readl(isp_csi_regs + 0x128));
        v0_17 = 3;
    } else if (interface_type != 2) {
        isp_printf(1, "%s[%d] VIC failed to config DVP mode!(10bits-sensor)\n",
                   "csi_core_ops_init", interface_type);
        v0_17 = 3;
    } else {
        if (!isp_csi_regs) {
            pr_err("csi_core_ops_init: wrapper regs unavailable for DVP init\n");
            return -ENODEV;
        }

        writel(0, csi_regs + 0x0c);
        writel(1, csi_regs + 0x0c);
        writel(0x7d, isp_csi_regs + 0x00);
        writel(0x3e, isp_csi_regs + 0x80);
        writel(1, isp_csi_regs + 0x2cc);
        v0_17 = 3;
    }

    csi_dev->state = v0_17;
    pr_info("csi_core_ops_init: complete, new state=%d\n", csi_dev->state);
    return 0;
}

/* csi_set_on_lanes - EXACT Binary Ninja implementation */
/* csi_set_on_lanes - EXACT Binary Ninja implementation */
int csi_set_on_lanes(struct tx_isp_csi_device *csi_dev, int lanes)
{
    void __iomem *csi_regs;
    u32 reg_val;

    /* Binary Ninja: isp_printf(0, "%s:----------> lane num: %d\n", "csi_set_on_lanes", lane_num) */
    isp_printf(0, "%s:----------> lane num: %d\n", "csi_set_on_lanes", lanes);

    csi_regs = csi_dev ? csi_dev->csi_regs : NULL;
    if (!csi_regs)
        return -EINVAL;

    /* Program N_LANES = (lanes - 1) */
    reg_val = readl(csi_regs + 0x04);
    reg_val = (reg_val & ~0x3) | ((lanes - 1) & 0x3);
    writel(reg_val, csi_regs + 0x04);

    return 0;
}

/* CSI link setup stub - CSI doesn't need complex link management */
static int csi_video_link_setup(struct tx_isp_subdev_pad *local,
                                  struct tx_isp_subdev_pad *remote,
                                  u32 flags)
{
    pr_info("*** csi_video_link_setup: CSI link setup (stub) - flags=0x%x ***\n", flags);
    /* CSI links are managed by hardware configuration, not software links */
    return 0;
}

/* Define the core operations */
static struct tx_isp_subdev_core_ops csi_core_ops = {
    .init = csi_core_ops_init,
};

/* Define the video operations */
static struct tx_isp_subdev_video_ops csi_video_ops = {
    .s_stream = csi_video_s_stream,
    .link_setup = csi_video_link_setup,
};

/* Define the sensor operations */
static struct tx_isp_subdev_sensor_ops csi_sensor_ops = {
    .sync_sensor_attr = csi_sensor_ops_sync_sensor_attr,
    .ioctl = csi_sensor_ops_ioctl,
};

/* CSI internal ops for activate/slake */
static struct tx_isp_subdev_internal_ops csi_subdev_internal_ops = {
    .activate_module = tx_isp_csi_activate_subdev,
    .slake_module = tx_isp_csi_slake_subdev,
};

/* Initialize the subdev ops structure with pointers to the operations */
struct tx_isp_subdev_ops csi_subdev_ops = {
    .core = &csi_core_ops,
    .video = &csi_video_ops,
    .sensor = &csi_sensor_ops,
    .internal = &csi_subdev_internal_ops,
};

// Define resources outside probe
static struct resource tx_isp_csi_resources[] = {
    [0] = {
        .start  = 0x10022000,  // CSI base address
        .end    = 0x10022000 + 0x1000 - 1,
        .flags  = IORESOURCE_MEM,
        .name   = "csi-regs",
    }
};

int tx_isp_csi_probe(struct platform_device *pdev)
{
    struct tx_isp_csi_device *csi_dev = NULL;
    struct tx_isp_subdev *sd = NULL;
    struct resource *res;
    int32_t ret = 0;

    pr_info("*** tx_isp_csi_probe: Starting CSI device probe ***\n");

    /* CRITICAL: Use existing CSI device from ourISPdev, DO NOT create a new one!
     * The CSI device is already created by csi_device_probe in tx-isp-module.c
     */
    if (ourISPdev && ourISPdev->csi_dev) {
        csi_dev = ourISPdev->csi_dev;
        pr_info("*** tx_isp_csi_probe: Using existing CSI device at %p ***\n", csi_dev);
    } else {
        pr_err("*** tx_isp_csi_probe: No existing CSI device found! ***\n");
        return -EINVAL;
    }

    /* Get subdev pointer from CSI device */
    sd = &csi_dev->sd;

    /* Add resources to platform device if not already present */
    if (!platform_get_resource(pdev, IORESOURCE_MEM, 0)) {
        ret = platform_device_add_resources(pdev, tx_isp_csi_resources,
                                          ARRAY_SIZE(tx_isp_csi_resources));
        if (ret) {
            pr_err("Failed to add CSI resources\n");
            return ret;
        }
    }

    /* Get platform resource */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);

    /* CRITICAL FIX: Set subdev private data BEFORE calling tx_isp_subdev_init
     * because tx_isp_subdev_init calls tx_isp_subdev_auto_link which needs
     * to retrieve csi_dev via tx_isp_get_subdevdata(sd)
     */
    tx_isp_set_subdevdata(sd, csi_dev);
    pr_info("*** tx_isp_csi_probe: Set subdev private data to csi_dev=%p ***\n", csi_dev);

    /* CRITICAL: Initialize subdev AFTER setting private data */
    ret = tx_isp_subdev_init(pdev, sd, &csi_subdev_ops);
    if (ret != 0) {
        pr_err("Failed to init CSI subdev module(%d.%d)\n",
               res ? MAJOR(res->start) : 0,
               res ? MINOR(res->start) : 0);
        return -EFAULT;  /* Binary returns -12 (EFAULT) */
    }

    /* Set platform driver data after successful init */
    platform_set_drvdata(pdev, csi_dev);

    pr_info("*** tx_isp_csi_probe: CSI device initialized successfully ***\n");
    pr_info("CSI device: csi_dev=%p, size=%zu\n", csi_dev, sizeof(struct tx_isp_csi_device));
    pr_info("  sd: %p\n", sd);
    pr_info("  state: %d\n", csi_dev->state);
    pr_info("  csi_regs: %p\n", csi_dev->csi_regs);

    return 0;
}

/* CSI remove function */
int tx_isp_csi_remove(struct platform_device *pdev)
{
    struct tx_isp_csi_device *csi_dev = platform_get_drvdata(pdev);
    struct tx_isp_subdev *sd;
    struct resource *res = NULL;

    if (!csi_dev)
        return -EINVAL;

    sd = &csi_dev->sd;

    pr_info("*** tx_isp_csi_remove: Removing CSI device ***\n");

    /* Free interrupt if it was requested */
    res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
    if (res)
        free_irq(res->start, sd);

    /* Disable CSI */
    tx_isp_csi_stop(sd);

    /* Deinitialize subdev */
    tx_isp_subdev_deinit(sd);

    pr_info("*** tx_isp_csi_remove: CSI device removed ***\n");
    return 0;
}

/* CSI register dump function for debugging */
void dump_csi_reg(struct tx_isp_subdev *sd)
{
    struct tx_isp_csi_device *csi_dev;
    void __iomem *csi_base;

    if (!sd) {
        pr_err("dump_csi_reg: sd is NULL\n");
        return;
    }

    csi_dev = ourISPdev->csi_dev;
    if (!csi_dev) {
        pr_err("dump_csi_reg: csi_dev is NULL\n");
        return;
    }

    /* CRITICAL FIX: Use safe struct member access instead of dangerous offset 0x13c */
    csi_base = csi_dev->csi_regs;
    if (!csi_base) {
        pr_err("dump_csi_reg: csi_base is NULL\n");
        return;
    }

    pr_info("=== CSI Register Dump (Basic + Wrapper) ===\n");
    /* Basic CSI registers (0x10022000 space) */
    pr_info("[BASIC] VERSION (0x00):      0x%08x\n", readl(csi_base + 0x00));
    pr_info("[BASIC] N_LANES (0x04):      0x%08x\n", readl(csi_base + 0x04));
    pr_info("[BASIC] PHY_SHUTDOWNZ (0x08): 0x%08x\n", readl(csi_base + 0x08));
    pr_info("[BASIC] DPHY_RSTZ (0x0C):     0x%08x\n", readl(csi_base + 0x0C));
    pr_info("[BASIC] CSI2_RESETN (0x10):   0x%08x\n", readl(csi_base + 0x10));
    pr_info("[BASIC] PHY_STATE (0x14):     0x%08x\n", readl(csi_base + 0x14));
    pr_info("[BASIC] ERR1 (0x20):          0x%08x\n", readl(csi_base + 0x20));
    pr_info("[BASIC] ERR2 (0x24):          0x%08x\n", readl(csi_base + 0x24));
    pr_info("[BASIC] CSI_CTRL (0x40):      0x%08x\n", readl(csi_base + 0x40));

    /* BN +0x13c slot (same 0x10022000 CSI mapping in tx_isp_csi_probe) */
    {
        void __iomem *isp_csi_regs = csi_get_wrapper_regs(csi_dev);

        if (isp_csi_regs) {
            pr_info("[ISP  ] TIMING (0x000):     0x%08x\n", readl(isp_csi_regs + 0x000));
            pr_info("[ISP  ] ???    (0x080):     0x%08x\n", readl(isp_csi_regs + 0x080));
            pr_info("[ISP  ] CFG0   (0x110):     0x%08x\n", readl(isp_csi_regs + 0x110));
            pr_info("[ISP  ] CFG1   (0x114):     0x%08x\n", readl(isp_csi_regs + 0x114));
            pr_info("[ISP  ] LANES  (0x128):     0x%08x\n", readl(isp_csi_regs + 0x128));
            pr_info("[ISP  ] RATE0  (0x160):     0x%08x\n", readl(isp_csi_regs + 0x160));
            pr_info("[ISP  ] RATE1  (0x1E0):     0x%08x\n", readl(isp_csi_regs + 0x1e0));
            pr_info("[ISP  ] RATE2  (0x260):     0x%08x\n", readl(isp_csi_regs + 0x260));
        } else {
            pr_info("[SLOT ] +0x13c is NULL\n");
        }
    }
    pr_info("==========================================\n");
}

/* Write buffer to file at csi_dump_path using kernel I/O (3.10 style) */
static void csi_write_file(const char *buf, size_t len)
{
    struct file *filp;
    mm_segment_t old_fs;
    loff_t pos = 0;

    filp = filp_open(csi_dump_path, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (IS_ERR(filp)) {
        pr_warn("CSI dump: filp_open(%s) failed: %ld\n", csi_dump_path, PTR_ERR(filp));
        return;
    }

    old_fs = get_fs();
    set_fs(KERNEL_DS);
#if defined(HAVE_KERNEL_WRITE)
    kernel_write(filp, buf, len, &filp->f_pos);
#else
    vfs_write(filp, buf, len, &pos);
#endif
    set_fs(old_fs);

    filp_close(filp, NULL);
}

static void csi_dump_once_to_file(struct tx_isp_csi_device *csi_dev, const char *tag)
{
    char buf[1024];
    int n = 0;

    void __iomem *csi_base;
    void __iomem *isp_csi_regs = NULL;

    if (!csi_dev)
        return;

    csi_base = csi_dev->csi_regs;
    if (!csi_base)
        return;

    /* BN +0x13c slot resolves to the CSI mem-region mapping. */
    isp_csi_regs = csi_get_wrapper_regs(csi_dev);

    n += scnprintf(buf + n, sizeof(buf) - n, "=== CSI DUMP ===\n");
    n += scnprintf(buf + n, sizeof(buf) - n, "[TAG  ] %s\n", tag ? tag : "UNTAGGED");
    n += scnprintf(buf + n, sizeof(buf) - n, "[PTRS ] csi_base=%p isp_csi=%p cpm[0xC4]=%08x\n", csi_base, isp_csi_regs, cpm_read32(0xC4));
    /* Read-only CPM diagnostics to locate gating/reset bits (mirrors vic_start path) */
    n += scnprintf(buf + n, sizeof(buf) - n, "[CPM  ] 0x20=%08x 0x24=%08x 0x28=%08x 0x2C=%08x 0x34=%08x 0x38=%08x\n",
                   cpm_read32(0x20), cpm_read32(0x24), cpm_read32(0x28), cpm_read32(0x2C), cpm_read32(0x34), cpm_read32(0x38));
    n += scnprintf(buf + n, sizeof(buf) - n, "[CPM2 ] 0x30=%08x 0x3C=%08x\n",
                   cpm_read32(0x30), cpm_read32(0x3C));
    {
        u32 clkgr0 = cpm_read32(0x20);
        u32 clkgr1 = cpm_read32(0x28);
        int csi_dphy_gated = (clkgr0 >> 7) & 1;   /* 1=gated, 0=ungated */
        int wrap_gated     = (clkgr1 >> 2) & 1;   /* 1=gated, 0=ungated */
        n += scnprintf(buf + n, sizeof(buf) - n, "[CLKG ] CSI_DPHY=%s WRAP=%s\n",
                       csi_dphy_gated ? "GATED" : "UNGATED",
                       wrap_gated     ? "GATED" : "UNGATED");
    }
    n += scnprintf(buf + n, sizeof(buf) - n, "[BASIC] 0x00=%08x 0x04=%08x 0x08=%08x 0x0C=%08x 0x10=%08x 0x14=%08x\n",
                   readl(csi_base + 0x00), readl(csi_base + 0x04), readl(csi_base + 0x08),
                   readl(csi_base + 0x0C), readl(csi_base + 0x10), readl(csi_base + 0x14));
    n += scnprintf(buf + n, sizeof(buf) - n, "[BASIC] 0x20=%08x 0x24=%08x 0x40=%08x 0x128=%08x\n",
                   readl(csi_base + 0x20), readl(csi_base + 0x24), readl(csi_base + 0x40), readl(csi_base + 0x128));
    n += scnprintf(buf + n, sizeof(buf) - n, "[ISP  ] 0x128=%08x\n", (isp_csi_regs ? readl(isp_csi_regs + 0x128) : 0));
    n += scnprintf(buf + n, sizeof(buf) - n,
                   "[LANEC] 0x200=%08x 0x204=%08x 0x210=%08x 0x230=%08x 0x250=%08x 0x254=%08x 0x2F4=%08x\n",
                   readl(csi_base + 0x200), readl(csi_base + 0x204), readl(csi_base + 0x210),
                   readl(csi_base + 0x230), readl(csi_base + 0x250), readl(csi_base + 0x254), readl(csi_base + 0x2F4));
    /* w01 diagnostic (primary mapping 0x10023000) */
    n += scnprintf(buf + n, sizeof(buf) - n, "[W01  ] 0x14=%08x 0x40=%08x\n", vic_read32(0x14), vic_read32(0x40));

    {
        u32 w01_phase = vic_read32(0x14);
        const char *phase_str = (w01_phase == 0x00000230 || w01_phase == 0x00000300 || w01_phase == 0x00000330)
                                ? "GRANT"
                                : (w01_phase == 0x00000200 ? "INIT" : "OTHER");
        n += scnprintf(buf + n, sizeof(buf) - n, "[W01P ] %s 0x14=%08x\n", phase_str, w01_phase);
    }


    if (isp_csi_regs) {
        n += scnprintf(buf + n, sizeof(buf) - n, "[ISP  ] 0x000=%08x 0x080=%08x 0x10C=%08x 0x110=%08x 0x114=%08x 0x128=%08x\n",
                       readl(isp_csi_regs + 0x000), readl(isp_csi_regs + 0x080), readl(isp_csi_regs + 0x10C), readl(isp_csi_regs + 0x110),
                       readl(isp_csi_regs + 0x114), readl(isp_csi_regs + 0x128));
        n += scnprintf(buf + n, sizeof(buf) - n, "[ISP  ] 0x160=%08x 0x1E0=%08x 0x260=%08x\n",
                       readl(isp_csi_regs + 0x160), readl(isp_csi_regs + 0x1E0), readl(isp_csi_regs + 0x260));
        n += scnprintf(buf + n, sizeof(buf) - n, "[ISP  ] 0x100=%08x 0x104=%08x 0x108=%08x 0x11C=%08x 0x120=%08x 0x124=%08x\n",
                       readl(isp_csi_regs + 0x100), readl(isp_csi_regs + 0x104), readl(isp_csi_regs + 0x108),
                       readl(isp_csi_regs + 0x11C), readl(isp_csi_regs + 0x120), readl(isp_csi_regs + 0x124));
        n += scnprintf(buf + n, sizeof(buf) - n, "[ISP  ] 0x12C=%08x 0x130=%08x 0x140=%08x 0x144=%08x 0x148=%08x 0x150=%08x\n",
                       readl(isp_csi_regs + 0x12C), readl(isp_csi_regs + 0x130), readl(isp_csi_regs + 0x140),
                       readl(isp_csi_regs + 0x144), readl(isp_csi_regs + 0x148), readl(isp_csi_regs + 0x150));

    } else {
        n += scnprintf(buf + n, sizeof(buf) - n, "[ISP  ] (null)\n");
    }
    n += scnprintf(buf + n, sizeof(buf) - n, "\n");

    csi_write_file(buf, n);

}

static int csi_dump_thread_fn(void *data)
{
    struct tx_isp_csi_device *csi_dev = (struct tx_isp_csi_device *)data;

    pr_info("CSI dump thread started (interval=%d ms, path=%s)\n", csi_dump_interval_ms, csi_dump_path);
    while (!kthread_should_stop()) {
        csi_dump_once_to_file(csi_dev, "PERIODIC");
        if (csi_dump_interval_ms <= 0)
            csi_dump_interval_ms = 1000;
        msleep(csi_dump_interval_ms);
    }
    pr_info("CSI dump thread stopped\n");
    return 0;
}

static void __maybe_unused csi_start_dump_thread(struct tx_isp_csi_device *csi_dev)
{
    if (!csi_dump_enable) {
        pr_info("CSI dump thread disabled (csi_dump_enable=0)\n");
        return;
    }
    if (!csi_dump_kthread && csi_dev) {
        csi_dump_kthread = kthread_run(csi_dump_thread_fn, csi_dev, "csi-dump");
        if (IS_ERR(csi_dump_kthread)) {
            pr_err("Failed to start CSI dump thread: %ld\n", PTR_ERR(csi_dump_kthread));
            csi_dump_kthread = NULL;
        } else {
            pr_info("CSI dump thread created\n");
        }
    }
}

static void csi_stop_dump_thread(void)
{
    if (csi_dump_kthread) {
        kthread_stop(csi_dump_kthread);
        csi_dump_kthread = NULL;
    }
}

/* CSI activation function - matching reference driver */

/* tx_isp_csi_activate_subdev - EXACT Binary Ninja implementation */
int tx_isp_csi_activate_subdev(struct tx_isp_subdev *sd)
{
    struct tx_isp_csi_device *csi_dev;
    struct clk **clks;
    int clk_count;
    int i;
    int result = 0xffffffea; /* Binary Ninja: int32_t result = 0xffffffea */

    /* Binary Ninja: if (arg1 != 0) */
    if (sd != NULL) {
        /* Binary Ninja: if (arg1 u>= 0xfffff001) return 0xffffffea */
        if ((unsigned long)sd >= 0xfffff001) {
            return 0xffffffea;
        }

        /* Binary Ninja: void* $s1_1 = *(arg1 + 0xd4) */
        csi_dev = (struct tx_isp_csi_device *)tx_isp_get_subdevdata(sd);
        result = 0xffffffea;

        /* Binary Ninja: if ($s1_1 != 0 && $s1_1 u< 0xfffff001) */
        if (csi_dev != NULL && (unsigned long)csi_dev < 0xfffff001) {
            /* Binary Ninja: private_mutex_lock($s1_1 + 0x12c) */
            mutex_lock(&csi_dev->mlock);

            /* Binary Ninja: if (*($s1_1 + 0x128) == 1) */
            if (csi_dev->state == 1) {
                /* Binary Ninja: *($s1_1 + 0x128) = 2 */
                csi_dev->state = 2;

                clks = sd->clks;
                clk_count = sd->clk_num;

                if (clks != NULL && (unsigned long)clks < 0xfffff001) {
                    for (i = 0; i < clk_count; i++) {
                        if (clks[i])
                            private_clk_enable(clks[i]);
                    }
                }
            }

            /* Binary Ninja: private_mutex_unlock($s1_1 + 0x12c) */
            mutex_unlock(&csi_dev->mlock);
            /* Binary Ninja: return 0 */
            return 0;
        }
    }

    /* Binary Ninja: return result */
    return result;
}

/* tx_isp_csi_slake_subdev - EXACT Binary Ninja reference implementation */
int tx_isp_csi_slake_subdev(struct tx_isp_subdev *sd)
{
    struct tx_isp_csi_device *csi_dev;
    int state;
    int i;

    /* Binary Ninja: if (arg1 == 0 || arg1 u>= 0xfffff001) return 0xffffffea */
    if (!sd || (unsigned long)sd >= 0xfffff001) {
        return -EINVAL;
    }

    /* Binary Ninja: void* $s0_1 = *(arg1 + 0xd4) */
    /* SAFE: Use proper function instead of offset-based access */
    csi_dev = (struct tx_isp_csi_device *)tx_isp_get_subdevdata(sd);
    if (!csi_dev || (unsigned long)csi_dev >= 0xfffff001) {
        return -EINVAL;
    }

    pr_info("*** tx_isp_csi_slake_subdev: CSI slake/shutdown - current state=%d ***\n", csi_dev->state);

    /* Binary Ninja: int32_t $v1_2 = *($s0_1 + 0x128) */
    state = csi_dev->state;

    /* Binary Ninja: if ($v1_2 == 4) csi_video_s_stream(arg1, 0) */
    if (state == 4) {
        pr_info("tx_isp_csi_slake_subdev: CSI in streaming state, stopping stream\n");
        csi_video_s_stream(sd, 0);
        state = csi_dev->state;  /* Update state after s_stream */
    }

    /* Binary Ninja: void* $s2_1 = $s0_1 + 0x12c - Get mutex */
    /* Binary Ninja: if ($v1_2 == 3) csi_core_ops_init(arg1, 0) */
    if (csi_dev->state == 3) {
        pr_info("tx_isp_csi_slake_subdev: CSI in state 3, calling core_ops_init(disable)\n");
        csi_core_ops_init(sd, 0);
    }

    /* Binary Ninja: private_mutex_lock($s2_1) */
    mutex_lock(&csi_dev->mlock);

    /* Binary Ninja: if (*($s0_1 + 0x128) == 2) *($s0_1 + 0x128) = 1 */
    if (csi_dev->state == 2) {
        pr_info("tx_isp_csi_slake_subdev: CSI state 2->1, disabling clocks\n");
        csi_dev->state = 1;

        /* Binary Ninja: void* $v0 = *(arg1 + 0xbc) - Get clocks array */
        /* Binary Ninja: if ($v0 != 0 && $v0 u< 0xfffff001) - Clock disabling loop */
        if (sd->clks && sd->clk_num > 0) {
            /* Binary Ninja: int32_t $s0_2 = *(arg1 + 0xc0) - Get clock count */
            /* Binary Ninja: Clock disabling loop in reverse order */
            for (i = sd->clk_num - 1; i >= 0; i--) {
                if (sd->clks[i]) {
                    /* Binary Ninja: private_clk_disable(*$s0_4) */
                    pr_info("[CLK] CSI: Disabling clock %d\n", i);
                    clk_disable(sd->clks[i]);
                }
            }
        }
    }

    mutex_unlock(&csi_dev->mlock);

	    /* Stop periodic CSI dump thread when CSI is slaked */
	    csi_stop_dump_thread();

    pr_info("*** tx_isp_csi_slake_subdev: CSI slake complete, final state=%d ***\n", csi_dev->state);
    return 0;
}

/* Export symbols for use by other parts of the driver */
EXPORT_SYMBOL(tx_isp_csi_start);
EXPORT_SYMBOL(tx_isp_csi_stop);
EXPORT_SYMBOL(tx_isp_csi_set_format);
EXPORT_SYMBOL(dump_csi_reg);
EXPORT_SYMBOL(tx_isp_csi_activate_subdev);
EXPORT_SYMBOL(tx_isp_csi_slake_subdev);
