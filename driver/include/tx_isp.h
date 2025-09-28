#ifndef __TX_ISP_H__
#define __TX_ISP_H__

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/i2c.h>
#include <linux/completion.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/proc_fs.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/miscdevice.h>

#include "tx-isp-common.h"

#define TX_ISP_LINKFLAG_ENABLED		(0x1)

/* ISP subdevice types */
#define TX_ISP_SUBDEV_CSI        1
#define TX_ISP_SUBDEV_VIC        2
#define TX_ISP_SUBDEV_SENSOR     3

/* ISP pipeline states */
#define ISP_PIPELINE_IDLE        0
#define ISP_PIPELINE_CONFIGURED  1
#define ISP_PIPELINE_STREAMING   2

/* ISP constants for Binary Ninja compatibility */
#define ISP_MAX_SUBDEVS          32

/* Forward declarations */
struct tx_isp_subdev;
struct tx_isp_core;
struct isp_tuning_data;
struct isp_tuning_state;
struct ae_info;
struct awb_info;
struct tx_isp_chip_ident;
struct tx_isp_sensor_win_setting;
struct irq_handler_data;
struct ddr_device {
    struct tx_isp_subdev *sd;   // Pointer to subdev
    void __iomem *ddr_regs;     // DDR registers
    int state;                  // Device state
};

struct frame_source_device;


// CSI device structure for MIPI interface (based on Binary Ninja analysis)
struct tx_isp_csi_device {
    struct tx_isp_subdev sd;        // Base subdev at offset 0
    struct clk *csi_clk;           // CSI clock
    int state;                     // 1=init, 2=active, 3=streaming_off, 4=streaming_on
    struct mutex mlock;            // Mutex for state changes
    int interface_type;            // 1=MIPI interface
    int lanes;                     // Number of MIPI lanes

    char device_name[32];     // 0x00: Device name
    struct device *dev;       // Device pointer
    uint32_t offset_10;       // 0x10: Referenced in init
    struct IspModule *module_info;  // Module info pointer

    // CSI register access - changed to single pointer like VIC
    void __iomem *cpm_regs;   // CPM registers
    void __iomem *phy_regs;   // MIPI PHY registers
    void __iomem *csi_regs;   // Single pointer to mapped csi regs
    struct resource *phy_res;  // PHY memory resource

    // State management
    struct clk *clk;
    struct mutex mutex;       // Synchronization
    spinlock_t lock;         // Protect register access
};


/* Channel attribute structure for ISP channels */
struct tx_isp_channel_attr {
    uint32_t width;     /* Channel output width */
    uint32_t height;    /* Channel output height */
    uint32_t fps_num;   /* FPS numerator */
    uint32_t fps_den;   /* FPS denominator */
    uint32_t format;    /* Output format */
    uint32_t stride;    /* Line stride */
};

struct link_pad_desc {
    char *name;              // Compared with string compare
    unsigned char type;      // Checked for 1 or 2
    char pad[2];            // Alignment padding
    unsigned int index;      // Used at offset +5
};

/* Link structures */
struct link_config {
    struct link_pad_desc src;     // Size 0x8
    struct link_pad_desc dst;     // Size 0x8
    unsigned int flags;           // Size 0x4
};

/* Function declarations */
struct tx_isp_dev;
void tx_isp_subdev_deinit(struct tx_isp_subdev *sd);
int tx_isp_sysfs_init(struct tx_isp_dev *isp);
void tx_isp_sysfs_exit(struct tx_isp_dev *isp);
irqreturn_t tx_isp_core_irq_handler(int irq, void *dev_id);

void tx_isp_frame_chan_init(struct tx_isp_frame_channel *chan);
void tx_isp_frame_chan_deinit(struct tx_isp_frame_channel *chan);
int tx_isp_setup_default_links(struct tx_isp_dev *dev);

/* Platform device declarations - defined in tx-isp-device.c */
extern struct platform_device tx_isp_csi_platform_device;
extern struct platform_device tx_isp_vic_platform_device;
extern struct platform_device tx_isp_vin_platform_device;
extern struct platform_device tx_isp_fs_platform_device;
extern struct platform_device tx_isp_core_platform_device;

/* Sensor control functions - defined in tx-isp-module.c */
int sensor_fps_control(int fps);

#endif /* __TX_ISP_H__ */
