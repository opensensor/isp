#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/interrupt.h>
#include "../include/tx_isp.h"
#include "../include/tx_isp_vin.h"
#include "../include/tx-isp-device.h"

/* VIN start operation */
int tx_isp_vin_start(struct tx_isp_subdev *sd)
{
    u32 ctrl;

    if (!sd)
        return -EINVAL;

    mutex_lock(&sd->vin_lbc_lock);

    /* Enable VIN */
    ctrl = VIN_CTRL_EN | VIN_CTRL_START;
    writel(ctrl, sd->base + VIN_CTRL);

    mutex_unlock(&sd->vin_lbc_lock);
    return 0;
}

/* VIN stop operation */
int tx_isp_vin_stop(struct tx_isp_subdev *sd)
{
    if (!sd)
        return -EINVAL;

    mutex_lock(&sd->vin_lbc_lock);

    /* Stop VIN */
    writel(VIN_CTRL_STOP, sd->base + VIN_CTRL);

    /* Wait for stop to complete */
    while (readl(sd->base + VIN_STATUS) & STATUS_BUSY) {
        udelay(10);
    }

    mutex_unlock(&sd->vin_lbc_lock);
    return 0;
}

/* VIN format configuration */
int tx_isp_vin_set_format(struct tx_isp_subdev *sd, struct tx_isp_config *config)
{
    u32 format = 0;

    if (!sd || !config)
        return -EINVAL;

    if (config->width < VIN_MIN_WIDTH || config->width > VIN_MAX_WIDTH ||
        config->height < VIN_MIN_HEIGHT || config->height > VIN_MAX_HEIGHT)
        return -EINVAL;

    mutex_lock(&sd->vin_lbc_lock);

    /* Set frame size */
    writel((config->height << 16) | config->width, sd->base + VIN_FRAME_SIZE);

    /* Set format */
    switch (config->format) {
    case FMT_YUV422:
        format = VIN_FMT_YUV422;
        break;
    case FMT_RGB888:
        format = VIN_FMT_RGB888;
        break;
    case FMT_RAW8:
        format = VIN_FMT_RAW8;
        break;
    case FMT_RAW10:
        format = VIN_FMT_RAW10;
        break;
    case FMT_RAW12:
        format = VIN_FMT_RAW12;
        break;
    default:
        mutex_unlock(&sd->vin_lbc_lock);
        return -EINVAL;
    }
    writel(format, sd->base + VIN_FORMAT);

    mutex_unlock(&sd->vin_lbc_lock);
    return 0;
}

#define VIN_STATE_IDLE      1
#define VIN_STATE_INIT      2
#define VIN_STATE_INIT_ON   3
#define VIN_STATE_STREAM_ON 4
#define VIN_STATE_STREAM_OFF 3
static int vin_s_stream(struct tx_isp_subdev *sd, int enable)
{
    int ret = 0;
    struct tx_isp_sensor *sensor = sd->active_sensor;
    void **ops_table;

    if (!sensor) {
        sd->vin_state = enable ? VIN_STATE_STREAM_ON : VIN_STATE_STREAM_OFF;
        return 0;
    }

    ops_table = (void**)((char*)sensor + 0xc4);
    if (ops_table && *ops_table) {
        void **stream_fn = *ops_table + 4;
        if (*stream_fn) {
            typedef int (*stream_fn_t)(void*, int);
            ret = ((stream_fn_t)*stream_fn)(sensor, enable);
            if (ret == 0) {
                sd->vin_state = enable ? VIN_STATE_STREAM_ON : VIN_STATE_STREAM_OFF;
            }
        } else {
            ret = -0x203;
        }
    }

    return ret;
}

static int tx_isp_vin_init(struct tx_isp_subdev *sd, int on)
{
    int ret = 0;
    void **ops_table;
    struct tx_isp_sensor *sensor;

    // During initial probe, just set state and return success
    if (!sd || sd->vin_state == 1) {  // 1 is the initial state we set in probe
        sd->vin_state = on ? VIN_STATE_INIT_ON : VIN_STATE_INIT;
        return 0;
    }

    // Only check for active sensor after initial setup
    sensor = sd->active_sensor;
    if (!sensor) {
        ISP_ERROR("Don't have active sensor on init!\n");
        return -1;
    }

//    ops_table = (void**)((char*)sensor + 0xc4);
//    if (ops_table && *ops_table) {
//        void **init_fn = *ops_table + 4;
//        if (*init_fn) {
//            typedef int (*init_fn_t)(void*);
//            ret = ((init_fn_t)*init_fn)(sensor);
//            if (ret == -0x203) {
//                ret = 0;
//            }
//        }
//    }

    sd->vin_state = on ? VIN_STATE_INIT_ON : VIN_STATE_INIT;
    return ret;
}

static int tx_isp_vin_reset(struct tx_isp_subdev *sd)
{
    int ret = 0;
    void **ops_table;
    struct tx_isp_sensor *sensor;

    if (!sd || sd->vin_state == 1) {
        return 0;
    }

    if (!sensor) {
        ISP_ERROR("Don't have active sensor on reset!\n");
        return -1;
    }

    ops_table = (void**)((char*)sensor + 0xc4);
    if (ops_table && *ops_table) {
        void **reset_fn = *ops_table + 8;
        if (*reset_fn) {
            typedef int (*reset_fn_t)(void*);
            ret = ((reset_fn_t)*reset_fn)(sensor);
            if (ret == -0x203) {
                ret = 0;
            }
        }
    }

    return ret;
}

static int vic_core_ops_ioctl(struct tx_isp_subdev *sd, int cmd)
{
    int ret = 0;
    struct tx_isp_sensor *sensor = sd->active_sensor;
    void **ops_table;

    pr_info("VIC ioctl cmd: 0x%x\n", cmd);

    if (cmd == 0x1000000) {
        if (!sensor)
            return 0;

        ops_table = (void**)((char*)sensor + 0xc4);
        if (ops_table && *ops_table) {
            void **init_fn = *ops_table + 4;
            if (*init_fn) {
                typedef int (*init_fn_t)(void*);
                ret = ((init_fn_t)*init_fn)(sensor);
                if (ret == -0x203) {
                    return 0;
                }
            }
        }
    }

    return ret;
}

static struct tx_isp_subdev_core_ops vin_subdev_core_ops = {
    // Let's modify this first - these should be NULL during probe
    .init = NULL,  // Will set to tx_isp_vin_init later
    .reset = NULL, // Will set to tx_isp_vin_reset later
    .ioctl = vic_core_ops_ioctl,
};

static struct tx_isp_subdev_video_ops vin_subdev_video_ops = {
    .s_stream = vin_s_stream,
};

static struct tx_isp_subdev_ops vin_subdev_ops = {
    .core = &vin_subdev_core_ops,
    .video = &vin_subdev_video_ops,
};

int tx_isp_vin_probe(struct platform_device *pdev)
{
    struct tx_isp_subdev *sd = NULL;
    int32_t ret = 0;

    printk("tx_isp_vin_probe\n");

    sd = private_kmalloc(sizeof(struct tx_isp_subdev), GFP_KERNEL);
    if (sd == NULL) {
        ISP_ERROR("Failed to allocate sensor subdev\n");
        return -ENOMEM;
    }

    memset(sd, 0, sizeof(struct tx_isp_subdev));

    // Initialize the mutex first
    private_raw_mutex_init(&sd->vin_lbc_lock, "vin_lock", NULL);

    ret = tx_isp_subdev_init(pdev, sd, &vin_subdev_ops);
    if (ret < 0) {
        ISP_ERROR("tx_isp_subdev_init failed!\n");
        private_kfree(sd);
        return ret;
    }

    // Now that init is complete, we can set up the ops
    vin_subdev_core_ops.init = tx_isp_vin_init;
    vin_subdev_core_ops.reset = tx_isp_vin_reset;

    private_platform_set_drvdata(pdev, sd);
    sd->vin_state = 1;

    return 0;
}

/* VIN remove function from decompiled code */
int tx_isp_vin_remove(struct platform_device *pdev)
{
    struct tx_isp_subdev *sd = private_platform_get_drvdata(pdev);

    if (!sd)
        return -EINVAL;

    /* Stop VIN */
    tx_isp_vin_stop(sd);

    /* Unmap registers */
    if (sd->base)
        private_iounmap(sd->base);

    tx_isp_subdev_deinit(sd);
    private_kfree(sd);

    return 0;
}
