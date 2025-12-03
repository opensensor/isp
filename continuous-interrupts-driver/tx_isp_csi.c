/*
 * tx_isp_csi.c - CSI (Camera Serial Interface) driver for Ingenic T31 ISP
 *
 * CORRECTED VERSION based on actual kernel source from:
 *   external/ingenic-sdk/3.10/isp/t30/videoin/tx-isp-csi.c
 *
 * CRITICAL FIXES from kernel source analysis:
 * 1. Lane enable mask is ALWAYS 0x3f - NOT conditional based on lane count
 * 2. Only the N_LANES register (offset 0x04) changes based on lane count
 * 3. PHY initialization sequence must match kernel source exactly
 * 4. State machine uses TX_ISP_MODULE_* enum values, not arbitrary numbers
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include "../include/tx_isp.h"
#include "../include/tx_isp_csi.h"
#include "../include/tx_isp_vic.h"
#include "../include/tx-isp-device.h"

/* ============================================================================
 * CSI Host Register Definitions (from kernel source tx-isp-csi.h)
 * ============================================================================ */
#define CSI_VERSION             0x00
#define CSI_N_LANES             0x04
#define CSI_PHY_SHUTDOWNZ       0x08
#define CSI_DPHY_RSTZ           0x0C
#define CSI_CSI2_RESETN         0x10
#define CSI_PHY_STATE           0x14
#define CSI_DATA_IDS_1          0x18
#define CSI_DATA_IDS_2          0x1C
#define CSI_ERR1                0x20
#define CSI_ERR2                0x24
#define CSI_MASK1               0x28
#define CSI_MASK2               0x2C
#define CSI_PHY_TST_CTRL0       0x30
#define CSI_PHY_TST_CTRL1       0x34

/* PHY Register Definitions (T30/T31 specific) */
#define PHY_CRTL0               0x000
#define PHY_CRTL1               0x080
#define CK_LANE_SETTLE_TIME     0x100
#define CK_LANE_CONFIG          0x128
#define PHY_DT0_LANE_SETTLE_TIME 0x180
#define PHY_DT1_LANE_SETTLE_TIME 0x200
#define PHY_MODEL_SWITCH        0x2CC
#define PHY_LVDS_MODE           0x300
#define PHY_FORCE_MODE          0x34

/* Forward declarations */
static int csi_core_ops_init(struct tx_isp_subdev *sd, int on);
static int csi_set_on_lanes(struct tx_isp_csi_device *csd, unsigned char lanes);
static int csi_video_s_stream(struct tx_isp_subdev *sd, int enable);
void dump_csi_reg(struct tx_isp_csi_device *csd);
int tx_isp_csi_slake_subdev(struct tx_isp_subdev *sd);
int tx_isp_csi_activate_subdev(struct tx_isp_subdev *sd);
static int csi_sensor_ops_ioctl(struct tx_isp_subdev *sd, unsigned int cmd, void *arg);
static int csi_sensor_ops_sync_sensor_attr(struct tx_isp_subdev *sd, void *arg);

extern struct tx_isp_dev *ourISPdev;

/* Global CSI device pointer for debug */
static struct tx_isp_csi_device *dump_csd = NULL;

/* ============================================================================
 * Register Access Macros (from kernel source)
 * ============================================================================ */
#define csi_core_read(csi, addr) \
    __raw_readl((unsigned int *)((csi)->sd.base + (addr)))

#define csi_core_write(csi, addr, value) \
    __raw_writel((value), (unsigned int *)((csi)->sd.base + (addr)))

#define csi_phy_read(csi, addr) \
    __raw_readl((unsigned int *)((csi)->phy_base + (addr)))

#define csi_phy_write(csi, addr, value) \
    __raw_writel((value), (unsigned int *)((csi)->phy_base + (addr)))

/* Helper: write partial register (from kernel source) */
static unsigned char csi_core_write_part(struct tx_isp_csi_device *csd,
                                         unsigned int address,
                                         unsigned int data,
                                         unsigned char shift,
                                         unsigned char width)
{
    unsigned int mask = (1 << width) - 1;
    unsigned int temp = csi_core_read(csd, address);
    temp &= ~(mask << shift);
    temp |= (data & mask) << shift;
    csi_core_write(csd, address, temp);
    return 0;
}

/* ============================================================================
 * CSI Error Checking (for debug)
 * ============================================================================ */
void tx_isp_csi_check_errors(struct tx_isp_dev *isp_dev)
{
    struct tx_isp_csi_device *csd;
    u32 err1, err2;

    if (!isp_dev || !isp_dev->csi_dev)
        return;

    csd = isp_dev->csi_dev;
    if (!csd->sd.base)
        return;

    err1 = csi_core_read(csd, CSI_ERR1);
    err2 = csi_core_read(csd, CSI_ERR2);

    if (err1 || err2) {
        pr_info("CSI errors: ERR1=0x%08x ERR2=0x%08x PHY_STATE=0x%08x\n",
                err1, err2, csi_core_read(csd, CSI_PHY_STATE));
    }
}

/* ============================================================================
 * CSI Set Lanes - CORRECTED from kernel source
 *
 * CRITICAL: Only the N_LANES register changes based on lane count.
 * The lane enable mask (CK_LANE_CONFIG, PHY_CRTL1) is ALWAYS 0x3f!
 * ============================================================================ */
static int csi_set_on_lanes(struct tx_isp_csi_device *csd, unsigned char lanes)
{
    pr_info("%s: Setting lane count to %d\n", __func__, lanes);
    /* From kernel source: csi_core_write_part(csd, N_LANES, (lanes - 1), 0, 2) */
    return csi_core_write_part(csd, CSI_N_LANES, (lanes - 1), 0, 2);
}

/* ============================================================================
 * CSI PHY Start/Stop - from kernel source
 * ============================================================================ */
static long csi_phy_start(struct tx_isp_csi_device *csd)
{
    csd->state = TX_ISP_MODULE_RUNNING;
    return 0;
}

static int csi_phy_stop(struct tx_isp_csi_device *csd)
{
    csd->state = TX_ISP_MODULE_INIT;
    return 0;
}

/* ============================================================================
 * CSI Video Stream Control - CORRECTED from kernel source
 * ============================================================================ */
static int csi_video_s_stream(struct tx_isp_subdev *sd, int enable)
{
    struct tx_isp_csi_device *csd = IS_ERR_OR_NULL(sd) ? NULL : sd_to_csi_device(sd);
    int ret = 0;

    if (IS_ERR_OR_NULL(csd)) {
        pr_err("%s: Invalid CSI device\n", __func__);
        return -EINVAL;
    }

    /* Only handle MIPI interface */
    if (csd->vin.attr && csd->vin.attr->dbus_type == TX_SENSOR_DATA_INTERFACE_MIPI) {
        if (enable) {
            ret = csi_phy_start(csd);
            pr_info("CSI: Stream ON\n");
        } else {
            ret = csi_phy_stop(csd);
            pr_info("CSI: Stream OFF\n");
        }
    }
    return ret;
}

/* ============================================================================
 * CSI Sensor Operations - CORRECTED from kernel source
 * ============================================================================ */
static int csi_sensor_ops_ioctl(struct tx_isp_subdev *sd, unsigned int cmd, void *arg)
{
    long ret = 0;
    struct tx_isp_csi_device *csd = IS_ERR_OR_NULL(sd) ? NULL : sd_to_csi_device(sd);

    if (IS_ERR_OR_NULL(csd))
        return 0;

    switch (cmd) {
    case TX_ISP_EVENT_SENSOR_PREPARE_CHANGE:
        if (csd->vin.attr && csd->vin.attr->dbus_type == TX_SENSOR_DATA_INTERFACE_MIPI) {
            ret = csi_phy_stop(csd);
        }
        break;
    case TX_ISP_EVENT_SENSOR_FINISH_CHANGE:
        if (csd->vin.attr && csd->vin.attr->dbus_type == TX_SENSOR_DATA_INTERFACE_MIPI) {
            ret = csi_phy_start(csd);
        }
        break;
    default:
        break;
    }
    return ret;
}

static int csi_sensor_ops_sync_sensor_attr(struct tx_isp_subdev *sd, void *arg)
{
    struct tx_isp_csi_device *csd = IS_ERR_OR_NULL(sd) ? NULL : sd_to_csi_device(sd);

    if (IS_ERR_OR_NULL(csd)) {
        pr_err("%s: Invalid CSI device\n", __func__);
        return -EINVAL;
    }

    if (arg)
        memcpy(&csd->vin, (void *)arg, sizeof(struct tx_isp_video_in));
    else
        memset(&csd->vin, 0, sizeof(struct tx_isp_video_in));

    return 0;
}

/* ============================================================================
 * csi_core_ops_init - CORRECTED from kernel source tx-isp-csi.c lines 122-192
 *
 * CRITICAL FIXES:
 * 1. Lane enable mask (CK_LANE_CONFIG, PHY_CRTL1) is ALWAYS 0x3f - NOT conditional!
 * 2. Only N_LANES register changes based on sensor lane count
 * 3. PHY initialization sequence must match kernel source exactly
 * 4. State machine uses TX_ISP_MODULE_* enum values
 * ============================================================================ */
static int csi_core_ops_init(struct tx_isp_subdev *sd, int on)
{
    struct tx_isp_csi_device *csd = IS_ERR_OR_NULL(sd) ? NULL : tx_isp_get_subdevdata(sd);

    if (IS_ERR_OR_NULL(csd)) {
        pr_err("%s: Invalid CSI device\n", __func__);
        return -EINVAL;
    }

    pr_info("%s: on=%d, current_state=%d\n", __func__, on, csd->state);

    if (on) {
        /* ================================================================
         * MIPI Interface Initialization - FROM KERNEL SOURCE
         * ================================================================ */
        if (csd->vin.attr && csd->vin.attr->dbus_type == TX_SENSOR_DATA_INTERFACE_MIPI) {
            pr_info("CSI: Initializing MIPI interface with %d lanes\n",
                    csd->vin.attr->mipi.lans);

            /* Step 1: Set lane count (only register that changes based on lanes) */
            csi_set_on_lanes(csd, csd->vin.attr->mipi.lans);

            /* Step 2: PHY shutdown sequence */
            csi_core_write_part(csd, CSI_PHY_SHUTDOWNZ, 0, 0, 1);
            mdelay(10);
            csi_core_write_part(csd, CSI_PHY_SHUTDOWNZ, 1, 0, 1);

            /* Step 3: DPHY reset sequence */
            csi_core_write_part(csd, CSI_DPHY_RSTZ, 0, 0, 1);
            msleep(10);
            csi_core_write_part(csd, CSI_DPHY_RSTZ, 1, 0, 1);

            /* Step 4: PHY test control initialization */
            csi_core_write_part(csd, CSI_PHY_TST_CTRL0, 0, 0, 1);
            csi_core_write(csd, CSI_PHY_TST_CTRL1, 0);
            csi_core_write_part(csd, CSI_PHY_TST_CTRL1, 0, 16, 1);
            csi_core_write_part(csd, CSI_PHY_TST_CTRL0, 0, 1, 1);

            /* ================================================================
             * CRITICAL: PHY initialization - ALWAYS use 0x3f for lane enable!
             * This is the key fix - kernel source lines 146-150 show:
             *   csi_phy_write(csd, PHY_CRTL0, 0x7d);
             *   csi_phy_write(csd, CK_LANE_CONFIG, 0x3f);  // ALWAYS 0x3f!
             *   csi_phy_write(csd, PHY_CRTL1, 0x3f);       // ALWAYS 0x3f!
             *   csi_phy_write(csd, PHY_LVDS_MODE, 0x1e);
             *   csi_phy_write(csd, PHY_CRTL1, 0x1f);
             * ================================================================ */
            csi_phy_write(csd, PHY_CRTL0, 0x7d);
            csi_phy_write(csd, CK_LANE_CONFIG, 0x3f);  /* ALWAYS 0x3f regardless of lane count! */
            csi_phy_write(csd, PHY_CRTL1, 0x3f);       /* ALWAYS 0x3f regardless of lane count! */
            csi_phy_write(csd, PHY_LVDS_MODE, 0x1e);
            csi_phy_write(csd, PHY_CRTL1, 0x1f);
            msleep(10);

            /* Step 6: Sony mode settle time configuration (if applicable) */
            if (csd->vin.attr->mipi.mode == SENSOR_MIPI_SONY_MODE) {
                pr_info("CSI: Configuring Sony MIPI mode settle times\n");
                csi_phy_write(csd, CK_LANE_SETTLE_TIME, 0x86);
                csi_phy_write(csd, PHY_DT0_LANE_SETTLE_TIME, 0x86);
                csi_phy_write(csd, PHY_DT1_LANE_SETTLE_TIME, 0x86);
                msleep(10);
            }

            /* Step 7: CSI2 reset sequence */
            csi_core_write_part(csd, CSI_CSI2_RESETN, 0, 0, 1);
            mdelay(10);
            csi_core_write_part(csd, CSI_CSI2_RESETN, 1, 0, 1);
            mdelay(10);

            pr_info("CSI: MIPI initialization complete\n");

        /* ================================================================
         * DVP Interface Initialization - FROM KERNEL SOURCE
         * ================================================================ */
        } else if (csd->vin.attr && csd->vin.attr->dbus_type == TX_SENSOR_DATA_INTERFACE_DVP) {
            pr_info("CSI: Initializing DVP interface\n");

            csi_core_write(csd, CSI_DPHY_RSTZ, 0x0);
            csi_core_write(csd, CSI_DPHY_RSTZ, 0x1);

            csi_phy_write(csd, PHY_CRTL0, 0x7d);
            csi_phy_write(csd, PHY_CRTL1, 0x3e);
            csi_phy_write(csd, PHY_LVDS_MODE, 0x1e);
            csi_phy_write(csd, PHY_MODEL_SWITCH, 0x1);

            pr_info("CSI: DVP initialization complete\n");

        } else {
            pr_warn("CSI: Unknown sensor dbus_type %d\n",
                    csd->vin.attr ? csd->vin.attr->dbus_type : -1);
        }

        csd->state = TX_ISP_MODULE_INIT;

    } else {
        /* ================================================================
         * Shutdown/Reset - FROM KERNEL SOURCE lines 185-189
         * ================================================================ */
        pr_info("CSI: Shutting down PHY\n");
        csi_core_write_part(csd, CSI_PHY_SHUTDOWNZ, 0, 0, 1);
        csi_core_write_part(csd, CSI_DPHY_RSTZ, 0, 0, 1);
        csi_core_write_part(csd, CSI_CSI2_RESETN, 0, 0, 1);
        csd->state = TX_ISP_MODULE_DEINIT;
    }

    return 0;
}

/* ============================================================================
 * CSI Clock Operations - from kernel source
 * ============================================================================ */
static int csi_clks_ops(struct tx_isp_subdev *sd, int on)
{
    struct clk **clks = IS_ERR_OR_NULL(sd) ? NULL : sd->clks;
    int i = 0;

    if (IS_ERR_OR_NULL(clks))
        return 0;

    if (on) {
        for (i = 0; i < sd->clk_num; i++)
            clk_enable(clks[i]);
    } else {
        for (i = sd->clk_num - 1; i >= 0; i--)
            clk_disable(clks[i]);
    }
    return 0;
}

/* ============================================================================
 * tx_isp_csi_activate_subdev - CORRECTED from kernel source
 * ============================================================================ */
int tx_isp_csi_activate_subdev(struct tx_isp_subdev *sd)
{
    struct tx_isp_csi_device *csd = IS_ERR_OR_NULL(sd) ? NULL : tx_isp_get_subdevdata(sd);

    if (IS_ERR_OR_NULL(csd))
        return -EINVAL;

    mutex_lock(&csd->mlock);
    if (csd->state == TX_ISP_MODULE_SLAKE) {
        csd->state = TX_ISP_MODULE_ACTIVATE;
        csi_clks_ops(sd, 1);
        pr_info("CSI: Activated (clocks enabled)\n");
    }
    mutex_unlock(&csd->mlock);
    return 0;
}

/* ============================================================================
 * tx_isp_csi_slake_subdev - CORRECTED from kernel source
 * ============================================================================ */
int tx_isp_csi_slake_subdev(struct tx_isp_subdev *sd)
{
    struct tx_isp_csi_device *csd = IS_ERR_OR_NULL(sd) ? NULL : tx_isp_get_subdevdata(sd);

    if (IS_ERR_OR_NULL(csd))
        return -EINVAL;

    /* Stop streaming if running */
    if (csd->state == TX_ISP_MODULE_RUNNING) {
        csi_video_s_stream(sd, 0);
    }

    /* Deinit if initialized */
    if (csd->state == TX_ISP_MODULE_INIT) {
        csi_core_ops_init(sd, 0);
    }

    mutex_lock(&csd->mlock);
    if (csd->state == TX_ISP_MODULE_ACTIVATE) {
        csd->state = TX_ISP_MODULE_SLAKE;
        csi_clks_ops(sd, 0);
        pr_info("CSI: Slaked (clocks disabled)\n");
    }
    mutex_unlock(&csd->mlock);
    return 0;
}

/* ============================================================================
 * CSI Subdev Operations - CORRECTED from kernel source
 * ============================================================================ */
static struct tx_isp_subdev_internal_ops csi_subdev_internal_ops = {
    .activate_module = tx_isp_csi_activate_subdev,
    .slake_module = tx_isp_csi_slake_subdev,
};

static struct tx_isp_subdev_core_ops csi_core_ops = {
    .init = csi_core_ops_init,
};

static struct tx_isp_subdev_video_ops csi_video_ops = {
    .s_stream = csi_video_s_stream,
};

static struct tx_isp_subdev_sensor_ops csi_sensor_ops = {
    .sync_sensor_attr = csi_sensor_ops_sync_sensor_attr,
    .ioctl = csi_sensor_ops_ioctl,
};

static struct tx_isp_subdev_ops csi_subdev_ops = {
    .core = &csi_core_ops,
    .video = &csi_video_ops,
    .sensor = &csi_sensor_ops,
    .internal = &csi_subdev_internal_ops,
};

/* ============================================================================
 * CSI Debug/Proc Interface - CORRECTED from kernel source
 * ============================================================================ */
static int isp_csi_show(struct seq_file *m, void *v)
{
    int len = 0;
    struct tx_isp_module *module = (void *)(m->private);
    struct tx_isp_subdev *sd = IS_ERR_OR_NULL(module) ? NULL : module_to_subdev(module);
    struct tx_isp_csi_device *csd = IS_ERR_OR_NULL(sd) ? NULL : tx_isp_get_subdevdata(sd);
    volatile unsigned int err1, err2;

    if (IS_ERR_OR_NULL(csd)) {
        pr_err("%s: Invalid CSI device\n", __func__);
        return 0;
    }

    err1 = csi_core_read(csd, CSI_ERR1);
    err2 = csi_core_read(csd, CSI_ERR2);

    if (err1 != 0)
        len += seq_printf(m, "ERR1: 0x%08x\n", err1);
    if (err2 != 0)
        len += seq_printf(m, "ERR2: 0x%08x\n", err2);
    if ((err1 != 0) || (err2 != 0))
        len += seq_printf(m, "PHY_STATE: 0x%08x\n", csi_core_read(csd, CSI_PHY_STATE));

    return len;
}

static int dump_isp_csi_open(struct inode *inode, struct file *file)
{
    return single_open_size(file, isp_csi_show, PDE_DATA(inode), 1024);
}

/* CSI file operations */
static struct file_operations isp_csi_fops = {
    .read = seq_read,
    .open = dump_isp_csi_open,
    .llseek = seq_lseek,
    .release = single_release,
};

/* MIPI PHY base address - from kernel source */
#define MIPI_PHY_IOBASE 0x10022000

/* ============================================================================
 * tx_isp_csi_probe - CORRECTED from kernel source
 * ============================================================================ */
int tx_isp_csi_probe(struct platform_device *pdev)
{
    struct tx_isp_csi_device *csd = NULL;
    struct tx_isp_subdev *sd = NULL;
    struct tx_isp_device_descriptor *desc = NULL;
    struct resource *res = NULL;
    int ret = 0;

    csd = (struct tx_isp_csi_device *)kzalloc(sizeof(*csd), GFP_KERNEL);
    if (!csd) {
        pr_err("Failed to allocate CSI device\n");
        ret = -ENOMEM;
        goto exit;
    }

    desc = pdev->dev.platform_data;
    sd = &csd->sd;
    ret = tx_isp_subdev_init(pdev, sd, &csi_subdev_ops);
    if (ret) {
        pr_err("Failed to init ISP module(%d.%d)\n",
               desc ? desc->parentid : -1, desc ? desc->unitid : -1);
        ret = -ENOMEM;
        goto failed_to_ispmodule;
    }

    /* Request and map MIPI PHY registers */
    res = request_mem_region(MIPI_PHY_IOBASE, 0x1000, "mipi-phy");
    if (!res) {
        pr_err("%s: Not enough memory for MIPI PHY resources\n", __func__);
        ret = -EBUSY;
        goto failed_req;
    }

    csd->phy_base = ioremap(res->start, res->end - res->start + 1);
    if (!csd->phy_base) {
        pr_err("%s: Unable to ioremap MIPI PHY registers\n", __func__);
        ret = -ENXIO;
        goto failed_ioremap;
    }
    csd->phy_res = res;

    tx_isp_set_subdev_debugops(sd, &isp_csi_fops);
    mutex_init(&csd->mlock);
    platform_set_drvdata(pdev, &sd->module);
    tx_isp_set_subdevdata(sd, csd);
    csd->state = TX_ISP_MODULE_SLAKE;

    dump_csd = csd;

    pr_info("CSI: Probe complete, phy_base=%p\n", csd->phy_base);
    return 0;

failed_ioremap:
    release_mem_region(res->start, res->end - res->start + 1);
failed_req:
    tx_isp_subdev_deinit(sd);
failed_to_ispmodule:
    kfree(csd);
exit:
    return ret;
}

/* ============================================================================
 * tx_isp_csi_remove - CORRECTED from kernel source
 * ============================================================================ */
int tx_isp_csi_remove(struct platform_device *pdev)
{
    struct tx_isp_module *module = platform_get_drvdata(pdev);
    struct tx_isp_subdev *sd = IS_ERR_OR_NULL(module) ? NULL : module_to_subdev(module);
    struct tx_isp_csi_device *csd = IS_ERR_OR_NULL(sd) ? NULL : sd_to_csi_device(sd);
    struct resource *res;

    if (IS_ERR_OR_NULL(csd))
        return 0;

    res = csd->phy_res;

    /* Reset CSI2 */
    csi_core_write_part(csd, CSI_CSI2_RESETN, 0, 0, 1);
    csi_core_write_part(csd, CSI_CSI2_RESETN, 1, 0, 1);

    platform_set_drvdata(pdev, NULL);

    if (csd->phy_base)
        iounmap(csd->phy_base);
    if (res)
        release_mem_region(res->start, res->end - res->start + 1);

    tx_isp_subdev_deinit(sd);
    kfree(csd);

    pr_info("CSI: Remove complete\n");
    return 0;
}

/* ============================================================================
 * dump_csi_reg - CORRECTED from kernel source
 * ============================================================================ */
void dump_csi_reg(struct tx_isp_csi_device *csd)
{
    if (IS_ERR_OR_NULL(csd) || !csd->sd.base)
        return;

    pr_info("****>>>>> dump csi reg <<<<<******\n");
    pr_info("**********VERSION =%08x\n", csi_core_read(csd, CSI_VERSION));
    pr_info("**********N_LANES =%08x\n", csi_core_read(csd, CSI_N_LANES));
    pr_info("**********PHY_SHUTDOWNZ = %08x\n", csi_core_read(csd, CSI_PHY_SHUTDOWNZ));
    pr_info("**********DPHY_RSTZ = %08x\n", csi_core_read(csd, CSI_DPHY_RSTZ));
    pr_info("**********CSI2_RESETN =%08x\n", csi_core_read(csd, CSI_CSI2_RESETN));
    pr_info("**********PHY_STATE = %08x\n", csi_core_read(csd, CSI_PHY_STATE));
    pr_info("**********DATA_IDS_1 = %08x\n", csi_core_read(csd, CSI_DATA_IDS_1));
    pr_info("**********DATA_IDS_2 = %08x\n", csi_core_read(csd, CSI_DATA_IDS_2));
    pr_info("**********ERR1 = %08x\n", csi_core_read(csd, CSI_ERR1));
    pr_info("**********ERR2 = %08x\n", csi_core_read(csd, CSI_ERR2));
    pr_info("**********MASK1 =%08x\n", csi_core_read(csd, CSI_MASK1));
    pr_info("**********MASK2 =%08x\n", csi_core_read(csd, CSI_MASK2));
    pr_info("**********PHY_TST_CTRL0 = %08x\n", csi_core_read(csd, CSI_PHY_TST_CTRL0));
    pr_info("**********PHY_TST_CTRL1 = %08x\n", csi_core_read(csd, CSI_PHY_TST_CTRL1));
}

/* ============================================================================
 * Platform Driver Structure
 * ============================================================================ */
struct platform_driver tx_isp_csi_driver = {
    .probe = tx_isp_csi_probe,
    .remove = tx_isp_csi_remove,
    .driver = {
        .name = TX_ISP_CSI_NAME,
        .owner = THIS_MODULE,
    },
};

EXPORT_SYMBOL(tx_isp_csi_driver);
EXPORT_SYMBOL(tx_isp_csi_activate_subdev);
EXPORT_SYMBOL(tx_isp_csi_slake_subdev);
