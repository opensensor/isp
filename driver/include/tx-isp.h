#ifndef _TX_ISP_H_
#define _TX_ISP_H_

/* Include kernel headers in proper order */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/cdev.h>    // For character device support


#include "tx-isp-main.h"
#include "tx-isp-hw.h"
#include "tx-isp-sensor.h"
#include "tx-libimp.h"

#define VIDIOC_SET_ALLOC_MODE _IOW('V', 200, int)
#define VIDIOC_SET_CHN_ATTR 0xc0505640

#define FRAME_FMT_SET BIT(1)  // Format has been set for this channel

#define DMA_BUFFER_OFFSET 0x400000 // TODO 4MB offset


// Add these at the top with other defines
#define ISP_ALLOC_KMALLOC 0
#define ISP_ALLOC_DMA    1

// Frame channel flags
#define FRAME_POOL_BOUND  BIT(0)

// DMA register offsets - update these based on your hardware spec
#define FRAME_DMA_BASE_OFFSET   0x1000
#define FRAME_DMA_ADDR_OFFSET   0x00
#define FRAME_DMA_SIZE_OFFSET   0x04

/* ISP Control Register Offsets */
/* ISP Control Register Offsets */
#define ISP_CTRL_REG_BASE     0x13380000

/* Image control registers - based on OEM array offsets */
#define ISP_CONTRAST_OFFSET   0x4090    // [0x1023] Contrast
#define ISP_BRIGHTNESS_OFFSET 0x4088    // [0x1022] Brightness referenced as data_4088
#define ISP_SATURATION_OFFSET 0x4094    // [0x1024] Saturation
#define ISP_SHARPNESS_OFFSET  0x3F7C    // [0xfdb] Sharpness
#define ISP_HFLIP_OFFSET     0x3AD0    // [0x3ad]
#define ISP_VFLIP_OFFSET     0x3AC0    // [0x3ac]

/* ISP Control Commands */
#define ISP_CTRL_CONTRAST    0x980901
#define ISP_CTRL_BRIGHTNESS  0x980900
#define ISP_CTRL_SATURATION  0x980902
#define ISP_CTRL_SHARPNESS   0x98091b
#define ISP_CTRL_HFLIP       0x980914
#define ISP_CTRL_VFLIP       0x980915

// CSI specific bits
#define CPM_CSI_CLK_MASK    (1 << 23)  // CSI clock gate in CLKGR
#define CPM_CSI_PWR_MASK    (1 << 2)   // CSI power domain in CLKGR1
#define CPM_CSI_RST_MASK    (1 << 20)  // CSI reset bit in SRSR
#define CSI_CLK_MASK     (1 << 23)  // CSI clock gate
#define CSI_PWR_MASK     (1 << 2)   // CSI power domain
#define CSI_SRST_MASK    (1 << 20)  // CSI soft reset

#define CPM_BASE         0x10000000
#define CPM_CLKGR        0x20    // Clock gate register
#define CPM_CLKGR1       0x28    // Clock gate register 1
#define CPM_SRSR         0xc4    // Software reset status register
#define CPM_SRBC         0xc8    // Soft reset bus control
#define CPM_SPCR0        0xb8    // Special purpose clock register 0
#define CPM_CSICDR       0x88    // CSI clock divider register
#define CPM_PCSR         0x2c    // Power control status register

// CSI specific bits
#define CPM_CSI_CLK_MASK    (1 << 23)  // CSI clock gate in CLKGR
#define CPM_CSI_PWR_MASK    (1 << 2)   // CSI power domain in CLKGR1
#define CPM_CSI_RST_MASK    (1 << 20)  // CSI reset bit in SRSR

#define ISP_BASE_ADDR    0x13300000  // Main ISP
#define MIPI_PHY_ADDR    0x10022000  // MIPI PHY
#define ISP_DEVICE_ADDR  0x10023000  // ISP device
#define CSI_BASE_ADDR    0x10023000  // CSI should be here

#define ISP_REG_SIZE     0x10000     // 64KB for main ISP
#define CSI_REG_SIZE     0x1000      // 4KB for CSI/MIPI
#define PHY_REG_SIZE     0x1000      // 4KB for PHY



// Add these register definitions
#define ISP_INT_FRAME_DONE    BIT(0)
#define ISP_INT_ERR           BIT(1)
#define ISP_INT_OVERFLOW      BIT(2)
#define ISP_INT_MASK_ALL     (ISP_INT_FRAME_DONE | ISP_INT_ERR | ISP_INT_OVERFLOW)



static struct class *framechan_class;
static dev_t framechan_dev;
static struct cdev framechan_cdev;


/* Function prototypes */
// CSI
int tx_isp_csi_probe(struct platform_device *pdev);
int tx_isp_csi_remove(struct platform_device *pdev);

// Sensor functions
int tx_isp_sensor_probe(struct IMPISPDev *dev);
void tx_isp_sensor_remove(struct IMPISPDev *dev);

// Video functions
int tx_isp_video_probe(struct IMPISPDev *dev);
void tx_isp_video_remove(struct IMPISPDev *dev);

// Hardware functions
int tx_isp_hw_probe(struct IMPISPDev *dev);
void tx_isp_hw_remove(struct IMPISPDev *dev);


// tx-isp-csi.c
// Core CSI functions
int init_csi_phy(struct IMPISPDev *dev);
int probe_csi_registers(struct IMPISPDev *dev);
int configure_mipi_csi(struct IMPISPDev *dev);
int verify_csi_signals(struct IMPISPDev *dev);
void dump_csi_reg(struct IMPISPDev *dev);

// CSI initialization sequence
int init_csi_early(struct IMPISPDev *dev);
int setup_csi_phy(struct IMPISPDev *dev);
void cleanup_csi_phy(struct IMPISPDev *dev);

// CSI operations
int csi_core_ops_init(struct IMPISPDev *dev, int enable);
int csi_set_lanes(struct IMPISPDev *dev, u8 lanes);
// int csi_video_s_stream(struct IMPISPDev *dev, bool enable);
int configure_csi_streaming(struct IMPISPDev *dev);
int configure_vic_for_streaming(struct IMPISPDev *dev);

// CSI interrupt handling
irqreturn_t tx_csi_irq_handler(int irq, void *dev_id);

// Video functions
int enable_isp_streaming(struct IMPISPDev *dev, struct file *file, int channel, bool enable);
int tx_isp_video_s_stream(struct IMPISPDev *dev, int enable);

void tx_isp_module_deinit(struct tx_isp_subdev *tisp_dev);
int32_t isp_subdev_release_clks(struct IspSubdev* isp_subdev);
void tx_isp_free_irq(int32_t* irq_pointer);

int tx_isp_module_init(struct platform_device *pdev, struct tx_isp_subdev *sd);
int tx_isp_subdev_init(struct platform_device *pdev, struct tx_isp_subdev *sd,
                      struct tx_isp_subdev_ops *ops);
void tx_isp_subdev_deinit(struct tx_isp_subdev *sd);

/* Pad link types */
#define TX_ISP_PADTYPE_INPUT     0
#define TX_ISP_PADTYPE_OUTPUT    1

/* Link types */
#define TX_ISP_PADLINK_DDR      (1 << 0)  // Memory interface
#define TX_ISP_PADLINK_ISP      (1 << 1)  // ISP processing
#define TX_ISP_PADLINK_CSI      (1 << 2)  // CSI interface
#define TX_ISP_PADLINK_VIC      (1 << 3)  // Video interface

/* Pad states */
#define TX_ISP_PADSTATE_FREE     0
#define TX_ISP_PADSTATE_LINKED   1
#define TX_ISP_PADSTATE_ACTIVE   2

#endif /* _TX_ISP_H_ */