#ifndef _TX_ISP_HW_H_
#define _TX_ISP_HW_H_
#include <linux/ioport.h>

#include "tx-isp.h"

/* ISP Register definitions */
#define ISP_REG_SIZE        0x10000

#define ISP_CTRL_REG        0x0000
#define ISP_STATUS_REG      0x0004
#define ISP_INT_MASK_REG    0x0008
#define ISP_INT_CLEAR_REG   0x000C

#define ISP_STREAM_CTRL     0x0010
#define ISP_STREAM_START    0x0014
#define ISP_FRAME_SIZE      0x0018

#define ISP_BUF0_OFFSET     0x0020
#define ISP_BUF1_OFFSET     0x0028
#define ISP_BUF2_OFFSET     0x0030

#define ISP_DMA_CTRL        0x0040
#define ISP_FRAME_CTRL      0x0044

/* VIC Register offsets */
#define VIC_BASE            0x0300
#define VIC_CTRL            0x0000
#define VIC_STATUS          0x0004
#define VIC_IRQ_EN          0x0008
#define VIC_MASK            0x000C
#define VIC_DMA_CTRL        0x0010
#define VIC_FRAME_CTRL      0x0014

int init_hw_resources(struct IMPISPDev *dev);
int tx_isp_init_memory(struct IMPISPDev *dev);
void tx_isp_cleanup_memory(struct IMPISPDev *dev);

int configure_isp_clocks(struct IMPISPDev *dev);
int init_vic_control(struct IMPISPDev *dev);

int configure_streaming_hardware(struct IMPISPDev *dev);
int configure_isp_buffers(struct IMPISPDev *dev);
int init_vic_control(struct IMPISPDev *dev);
void verify_isp_state(struct IMPISPDev *dev);
int isp_power_on(struct IMPISPDev *dev);
int isp_reset_hw(struct IMPISPDev *dev);
int reset_vic(struct IMPISPDev *dev);

u32 isp_reg_read(void __iomem *base, u32 offset);
void isp_reg_write(void __iomem *base, u32 offset, u32 value);

#endif /* _TX_ISP_HW_H_ */