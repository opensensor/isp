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
int configure_csi_streaming(struct IMPISPDev *dev);
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