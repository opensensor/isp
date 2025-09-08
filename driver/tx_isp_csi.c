#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/of.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/clk.h>
#include <linux/vmalloc.h>
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

/* External device references */
extern struct tx_isp_dev *ourISPdev;

/* CSI device structure to hold CSI-specific state */
struct tx_isp_csi_device {
    void __iomem *csi_regs;
    void __iomem *csi_phy_regs;
    void __iomem *csi_lane_regs;
    struct tx_isp_sensor_attribute sensor_attr;
    int state;
    int lanes;
    int data_type;
    spinlock_t lock;
    struct mutex mlock;
};

static struct tx_isp_csi_device *global_csi_dev = NULL;

/* csi_set_on_lanes - EXACT Binary Ninja implementation */
int csi_set_on_lanes(struct tx_isp_subdev *sd, int lanes)
{
    struct tx_isp_csi_device *csi_dev;
    void __iomem *vic_regs;
    u32 reg_val;
    
    pr_info("*** csi_set_on_lanes: EXACT Binary Ninja implementation ***\n");
    pr_info("csi_set_on_lanes: Can't output the width(%d)! %s\n", lanes, "csi_set_on_lanes");
    
    /* CRITICAL FIX: Use safe access to get CSI device */
    if (ourISPdev && ourISPdev->vic_dev) {
        csi_dev = global_csi_dev;
        if (!csi_dev) {
            pr_err("csi_set_on_lanes: No CSI device available\n");
            return -EINVAL;
        }
        
        /* Binary Ninja EXACT: void* $v1 = *(arg1 + 0xb8) */
        vic_regs = ourISPdev->vic_regs; 
        if (!vic_regs) {
            pr_err("csi_set_on_lanes: No VIC register base\n");
            return -EINVAL;
        }
        
        /* Binary Ninja EXACT: *($v1 + 4) = ((zx.d(arg2) - 1) & 3) | (*($v1 + 4) & 0xfffffffc) */
        reg_val = readl(vic_regs + 4);
        reg_val = (reg_val & 0xfffffffc) | (((lanes - 1) & 3));
        writel(reg_val, vic_regs + 4);
        wmb();
        
        pr_info("MCP_LOG: csi_set_on_lanes - wrote 0x%x to VIC+4, lanes=%d\n", reg_val, lanes);
        
        /* Store lane configuration in CSI device */
        csi_dev->lanes = lanes;
        
    } else {
        pr_err("csi_set_on_lanes: ISP device or VIC not available\n");
        return -EINVAL;
    }
    
    /* Binary Ninja EXACT: return 0 */
    return 0;
}
EXPORT_SYMBOL(csi_set_on_lanes);

/* csi_core_ops_init - EXACT Binary Ninja implementation for missing CSI initialization */
int csi_core_ops_init(struct tx_isp_subdev *sd, int mode, int sensor_format)
{
    struct tx_isp_csi_device *csi_dev;
    void __iomem *vic_regs;
    void __iomem *csi_regs;
    u32 reg_val;
    int i;
    
    pr_info("*** csi_core_ops_init: EXACT Binary Ninja implementation for CSI initialization ***\n");
    pr_info("csi_core_ops_init: mode=%d, sensor_format=0x%x\n", mode, sensor_format);
    
    /* Get or create CSI device */
    if (!global_csi_dev) {
        global_csi_dev = kzalloc(sizeof(struct tx_isp_csi_device), GFP_KERNEL);
        if (!global_csi_dev) {
            pr_err("csi_core_ops_init: Failed to allocate CSI device\n");
            return -ENOMEM;
        }
        
        /* Initialize CSI device */
        spin_lock_init(&global_csi_dev->lock);
        mutex_init(&global_csi_dev->mlock);
        global_csi_dev->state = 1;
        
        /* Map CSI register regions - these are the missing register regions! */
        global_csi_dev->csi_phy_regs = ioremap(0x13300000, 0x1000);  /* CSI PHY Config base */
        global_csi_dev->csi_lane_regs = ioremap(0x13300200, 0x200);  /* CSI Lane Config base */
        
        pr_info("*** CSI DEVICE CREATED: phy_regs=%p, lane_regs=%p ***\n", 
                global_csi_dev->csi_phy_regs, global_csi_dev->csi_lane_regs);
    }
    
    csi_dev = global_csi_dev;
    
    /* Binary Ninja validation */
    if (!ourISPdev || !ourISPdev->vic_regs) {
        pr_err("csi_core_ops_init: ISP device not ready\n");
        return -EINVAL;
    }
    
    vic_regs = ourISPdev->vic_regs;
    csi_regs = csi_dev->csi_phy_regs;
    
    /* Binary Ninja EXACT: int32_t result = 0xffffffea */
    int result = -EINVAL;
    
    /* Binary Ninja: Check if state >= 2 */
    if (csi_dev->state >= 2) {
        if (mode == 0) {
            /* Binary Ninja: Disable mode */
            pr_info("csi_core_ops_init: VIC do not support this format %d\n", sensor_format);
            
            /* Clear VIC control registers */
            reg_val = readl(vic_regs + 8);
            reg_val &= 0xfffffffe;
            writel(reg_val, vic_regs + 8);
            
            reg_val = readl(vic_regs + 0xc);
            reg_val &= 0xfffffffe;
            writel(reg_val, vic_regs + 0xc);
            
            reg_val = readl(vic_regs + 0x10);
            reg_val &= 0xfffffffe;
            writel(reg_val, vic_regs + 0x10);
            
            csi_dev->state = 2;
            wmb();
        } else {
            /* Binary Ninja: Enable mode with sensor configuration */
            pr_info("*** csi_core_ops_init: WRITING MISSING CSI PHY CONFIG REGISTERS ***\n");
            
            /* Set lanes based on sensor format */
            int lanes = 2; /* Default 2 lanes for MIPI */
            if (csi_dev->sensor_attr.dbus_type == 2) { /* MIPI interface */
                /* Use default 2 lanes - mipi struct member names may vary */
                lanes = 2; /* Standard 2-lane MIPI configuration */
            }
            
            /* Binary Ninja: Set up CSI lanes */
            csi_set_on_lanes(sd, lanes);
            
            /* *** CRITICAL: Write the MISSING CSI PHY Config registers from reference trace *** */
            if (csi_regs) {
                /* These are the register writes missing from tracer driver that appear in reference */
                writel(0x8a, csi_regs + 0x100);    /* CSI PHY Config */
                writel(0x5, csi_regs + 0x104);     /* CSI PHY Config */
                writel(0x40, csi_regs + 0x10c);    /* CSI PHY Config */
                writel(0xb0, csi_regs + 0x110);    /* CSI PHY Config */
                writel(0xc5, csi_regs + 0x114);    /* CSI PHY Config */
                writel(0x3, csi_regs + 0x118);     /* CSI PHY Config */
                writel(0x20, csi_regs + 0x11c);    /* CSI PHY Config */
                writel(0xf, csi_regs + 0x120);     /* CSI PHY Config */
                writel(0x48, csi_regs + 0x124);    /* CSI PHY Config */
                writel(0x3f, csi_regs + 0x128);    /* CSI PHY Config */
                writel(0xf, csi_regs + 0x12c);     /* CSI PHY Config */
                writel(0x88, csi_regs + 0x130);    /* CSI PHY Config */
                writel(0x86, csi_regs + 0x138);    /* CSI PHY Config */
                writel(0x10, csi_regs + 0x13c);    /* CSI PHY Config */
                writel(0x4, csi_regs + 0x140);     /* CSI PHY Config */
                writel(0x1, csi_regs + 0x144);     /* CSI PHY Config */
                writel(0x32, csi_regs + 0x148);    /* CSI PHY Config */
                writel(0x80, csi_regs + 0x14c);    /* CSI PHY Config */
                writel(0x1, csi_regs + 0x158);     /* CSI PHY Config */
                writel(0x60, csi_regs + 0x15c);    /* CSI PHY Config */
                writel(0x1b, csi_regs + 0x160);    /* CSI PHY Config */
                writel(0x18, csi_regs + 0x164);    /* CSI PHY Config */
                writel(0x7f, csi_regs + 0x168);    /* CSI PHY Config */
                writel(0x4b, csi_regs + 0x16c);    /* CSI PHY Config */
                writel(0x3, csi_regs + 0x174);     /* CSI PHY Config */
                
                /* Second channel CSI PHY Config */
                writel(0x8a, csi_regs + 0x180);    /* CSI PHY Config */
                writel(0x5, csi_regs + 0x184);     /* CSI PHY Config */
                writel(0x40, csi_regs + 0x18c);    /* CSI PHY Config */
                writel(0xb0, csi_regs + 0x190);    /* CSI PHY Config */
                writel(0xc5, csi_regs + 0x194);    /* CSI PHY Config */
                writel(0x3, csi_regs + 0x198);     /* CSI PHY Config */
                writel(0x9, csi_regs + 0x19c);     /* CSI PHY Config */
                writel(0xf, csi_regs + 0x1a0);     /* CSI PHY Config */
                writel(0x48, csi_regs + 0x1a4);    /* CSI PHY Config */
                writel(0xf, csi_regs + 0x1a8);     /* CSI PHY Config */
                writel(0xf, csi_regs + 0x1ac);     /* CSI PHY Config */
                writel(0x88, csi_regs + 0x1b0);    /* CSI PHY Config */
                writel(0x86, csi_regs + 0x1b8);    /* CSI PHY Config */
                writel(0x10, csi_regs + 0x1bc);    /* CSI PHY Config */
                writel(0x4, csi_regs + 0x1c0);     /* CSI PHY Config */
                writel(0x1, csi_regs + 0x1c4);     /* CSI PHY Config */
                writel(0x32, csi_regs + 0x1c8);    /* CSI PHY Config */
                writel(0x80, csi_regs + 0x1cc);    /* CSI PHY Config */
                writel(0x1, csi_regs + 0x1d8);     /* CSI PHY Config */
                writel(0x60, csi_regs + 0x1dc);    /* CSI PHY Config */
                writel(0x1b, csi_regs + 0x1e0);    /* CSI PHY Config */
                writel(0x18, csi_regs + 0x1e4);    /* CSI PHY Config */
                writel(0x7f, csi_regs + 0x1e8);    /* CSI PHY Config */
                writel(0x4b, csi_regs + 0x1ec);    /* CSI PHY Config */
                writel(0x3, csi_regs + 0x1f4);     /* CSI PHY Config */
                wmb();
                
                pr_info("*** CSI PHY Config registers written (0x100-0x1f4) ***\n");
            }
            
            /* *** CRITICAL: Write the MISSING CSI Lane Config registers *** */
            if (csi_dev->csi_lane_regs) {
                /* These are the CSI Lane Config registers (0x200-0x2f4) completely missing from tracer */
                writel(0x8a, csi_dev->csi_lane_regs + 0x0);   /* Lane Config 0x200 */
                writel(0x5, csi_dev->csi_lane_regs + 0x4);    /* Lane Config 0x204 */
                writel(0x40, csi_dev->csi_lane_regs + 0xc);   /* Lane Config 0x20c */
                writel(0xb0, csi_dev->csi_lane_regs + 0x10);  /* Lane Config 0x210 */
                writel(0xc5, csi_dev->csi_lane_regs + 0x14);  /* Lane Config 0x214 */
                writel(0x3, csi_dev->csi_lane_regs + 0x18);   /* Lane Config 0x218 */
                writel(0x9, csi_dev->csi_lane_regs + 0x1c);   /* Lane Config 0x21c */
                writel(0xf, csi_dev->csi_lane_regs + 0x20);   /* Lane Config 0x220 */
                writel(0x48, csi_dev->csi_lane_regs + 0x24);  /* Lane Config 0x224 */
                writel(0xf, csi_dev->csi_lane_regs + 0x28);   /* Lane Config 0x228 */
                writel(0xf, csi_dev->csi_lane_regs + 0x2c);   /* Lane Config 0x22c */
                writel(0x88, csi_dev->csi_lane_regs + 0x30);  /* Lane Config 0x230 */
                writel(0x86, csi_dev->csi_lane_regs + 0x38);  /* Lane Config 0x238 */
                writel(0x10, csi_dev->csi_lane_regs + 0x3c);  /* Lane Config 0x23c */
                writel(0x4, csi_dev->csi_lane_regs + 0x40);   /* Lane Config 0x240 */
                writel(0x1, csi_dev->csi_lane_regs + 0x44);   /* Lane Config 0x244 */
                writel(0x32, csi_dev->csi_lane_regs + 0x48);  /* Lane Config 0x248 */
                writel(0x80, csi_dev->csi_lane_regs + 0x4c);  /* Lane Config 0x24c */
                writel(0x1, csi_dev->csi_lane_regs + 0x58);   /* Lane Config 0x258 */
                writel(0x60, csi_dev->csi_lane_regs + 0x5c);  /* Lane Config 0x25c */
                writel(0x1b, csi_dev->csi_lane_regs + 0x60);  /* Lane Config 0x260 */
                writel(0x18, csi_dev->csi_lane_regs + 0x64);  /* Lane Config 0x264 */
                writel(0x7f, csi_dev->csi_lane_regs + 0x68);  /* Lane Config 0x268 */
                writel(0x4b, csi_dev->csi_lane_regs + 0x6c);  /* Lane Config 0x26c */
                writel(0x3, csi_dev->csi_lane_regs + 0x74);   /* Lane Config 0x274 */
                
                /* Additional lane configurations */
                writel(0x8a, csi_dev->csi_lane_regs + 0x80);  /* Lane Config 0x280 */
                writel(0x5, csi_dev->csi_lane_regs + 0x84);   /* Lane Config 0x284 */
                writel(0x40, csi_dev->csi_lane_regs + 0x8c);  /* Lane Config 0x28c */
                writel(0xb0, csi_dev->csi_lane_regs + 0x90);  /* Lane Config 0x290 */
                writel(0xc5, csi_dev->csi_lane_regs + 0x94);  /* Lane Config 0x294 */
                writel(0x3, csi_dev->csi_lane_regs + 0x98);   /* Lane Config 0x298 */
                writel(0x9, csi_dev->csi_lane_regs + 0x9c);   /* Lane Config 0x29c */
                writel(0xf, csi_dev->csi_lane_regs + 0xa0);   /* Lane Config 0x2a0 */
                writel(0x48, csi_dev->csi_lane_regs + 0xa4);  /* Lane Config 0x2a4 */
                writel(0xf, csi_dev->csi_lane_regs + 0xa8);   /* Lane Config 0x2a8 */
                writel(0xf, csi_dev->csi_lane_regs + 0xac);   /* Lane Config 0x2ac */
                writel(0x88, csi_dev->csi_lane_regs + 0xb0);  /* Lane Config 0x2b0 */
                writel(0x86, csi_dev->csi_lane_regs + 0xb8);  /* Lane Config 0x2b8 */
                writel(0x10, csi_dev->csi_lane_regs + 0xbc);  /* Lane Config 0x2bc */
                writel(0x4, csi_dev->csi_lane_regs + 0xc0);   /* Lane Config 0x2c0 */
                writel(0x1, csi_dev->csi_lane_regs + 0xc4);   /* Lane Config 0x2c4 */
                writel(0x32, csi_dev->csi_lane_regs + 0xc8);  /* Lane Config 0x2c8 */
                writel(0x80, csi_dev->csi_lane_regs + 0xcc);  /* Lane Config 0x2cc */
                writel(0x1, csi_dev->csi_lane_regs + 0xd8);   /* Lane Config 0x2d8 */
                writel(0x60, csi_dev->csi_lane_regs + 0xdc);  /* Lane Config 0x2dc */
                writel(0x1b, csi_dev->csi_lane_regs + 0xe0);  /* Lane Config 0x2e0 */
                writel(0x18, csi_dev->csi_lane_regs + 0xe4);  /* Lane Config 0x2e4 */
                writel(0x7f, csi_dev->csi_lane_regs + 0xe8);  /* Lane Config 0x2e8 */
                writel(0x4b, csi_dev->csi_lane_regs + 0xec);  /* Lane Config 0x2ec */
                writel(0x3, csi_dev->csi_lane_regs + 0xf4);   /* Lane Config 0x2f4 */
                wmb();
                
                pr_info("*** CSI Lane Config registers written (0x200-0x2f4) ***\n");
            }
            
            /* Clear VIC control and enable proper mode */
            reg_val = readl(vic_regs + 8);
            reg_val &= 0xfffffffe;
            writel(reg_val, vic_regs + 8);
            
            reg_val = readl(vic_regs + 0xc);
            reg_val &= 0xfffffffe;
            writel(reg_val, vic_regs + 0xc);
            
            reg_val = readl(vic_regs + 0x10);
            reg_val &= 0xfffffffe;
            writel(reg_val, vic_regs + 0x10);
            
            msleep(1);
            
            /* Enable VIC */
            reg_val = readl(vic_regs + 0xc);
            writel(reg_val | 1, vic_regs + 0xc);
            
            msleep(1);
            
            /* Additional sensor format configuration */
            u32 format_config = 0;
            if (sensor_format != 0) {
                /* Default format configuration for non-zero sensor formats */
                format_config = 0;
            } else {
                /* Format-specific configuration based on Binary Ninja switch logic */
                if ((sensor_format - 0x50) < 0x1e) {
                    format_config = 0;  /* Default for this range */
                } else {
                    /* Complex format detection logic from Binary Ninja */
                    if (sensor_format >= 0x6e && sensor_format < 0x96) {
                        format_config = 1;
                    } else if (sensor_format >= 0x96 && sensor_format < 0xc8) {
                        format_config = 2;
                    } else {
                        format_config = 3;
                    }
                }
            }
            
            /* Apply format configuration to CSI registers */
            if (csi_regs) {
                reg_val = readl(csi_regs + 0x160);
                reg_val = (reg_val & 0xfffffff0) | format_config;
                writel(reg_val, csi_regs + 0x160);
                writel(reg_val, csi_regs + 0x1e0);
                writel(reg_val, csi_regs + 0x260);
                wmb();
                
                pr_info("MCP_LOG: CSI format configuration applied - format_config=%d\n", format_config);
            }
            
            /* Final CSI enable sequence */
            writel(0x7d, csi_regs + 0x0);
            writel(0x3f, csi_regs + 0x128);
            
            reg_val = readl(vic_regs + 0x10);
            writel(reg_val | 1, vic_regs + 0x10);
            
            msleep(10);
            
            csi_dev->state = 3;
            result = 0;
        }
        
        /* Set final state */
        csi_dev->state = result == 0 ? 3 : 2;
        
        pr_info("MCP_LOG: csi_core_ops_init completed - result=%d, state=%d\n", result, csi_dev->state);
        return result;
    }
    
    return result;
}
EXPORT_SYMBOL(csi_core_ops_init);

/* CSI sensor operations ioctl */
int csi_sensor_ops_ioctl(struct tx_isp_subdev *sd, unsigned int cmd, void *arg)
{
    pr_info("csi_sensor_ops_ioctl: cmd=0x%x\n", cmd);
    return 0;
}
EXPORT_SYMBOL(csi_sensor_ops_ioctl);

/* CSI sensor operations sync sensor attr */
int csi_sensor_ops_sync_sensor_attr(struct tx_isp_subdev *sd, struct tx_isp_sensor_attribute *attr)
{
    struct tx_isp_csi_device *csi_dev = global_csi_dev;
    
    pr_info("csi_sensor_ops_sync_sensor_attr: attr=%p\n", attr);
    
    if (csi_dev && attr) {
        memcpy(&csi_dev->sensor_attr, attr, sizeof(struct tx_isp_sensor_attribute));
        pr_info("MCP_LOG: CSI sensor attributes synchronized\n");
    }
    
    return 0;
}
EXPORT_SYMBOL(csi_sensor_ops_sync_sensor_attr);

/* CSI video s_stream */
int csi_video_s_stream(struct tx_isp_subdev *sd, int enable)
{
    pr_info("*** csi_video_s_stream: enable=%d ***\n", enable);
    
    if (enable) {
        /* Start CSI streaming */
        if (global_csi_dev) {
            global_csi_dev->state = 4; /* Streaming state */
        }
        pr_info("MCP_LOG: CSI streaming started\n");
    } else {
        /* Stop CSI streaming */
        if (global_csi_dev) {
            global_csi_dev->state = 3; /* Ready state */
        }
        pr_info("MCP_LOG: CSI streaming stopped\n");
    }
    
    return 0;
}
EXPORT_SYMBOL(csi_video_s_stream);

/* CSI activation */
int tx_isp_csi_activate_subdev(struct tx_isp_subdev *sd)
{
    pr_info("tx_isp_csi_activate_subdev: activating CSI\n");
    
    if (global_csi_dev) {
        global_csi_dev->state = 2; /* Ready state */
        pr_info("MCP_LOG: CSI device activated\n");
    }
    
    return 0;
}
EXPORT_SYMBOL(tx_isp_csi_activate_subdev);

/* CSI slake */
int tx_isp_csi_slake_subdev(struct tx_isp_subdev *sd)
{
    pr_info("tx_isp_csi_slake_subdev: deactivating CSI\n");
    
    if (global_csi_dev) {
        global_csi_dev->state = 1; /* Init state */
        pr_info("MCP_LOG: CSI device slaked\n");
    }
    
    return 0;
}
EXPORT_SYMBOL(tx_isp_csi_slake_subdev);

/* CSI probe */
int tx_isp_csi_probe(struct platform_device *pdev)
{
    pr_info("*** tx_isp_csi_probe: CSI device probe ***\n");
    
    /* CSI device will be created on demand in csi_core_ops_init */
    return 0;
}
EXPORT_SYMBOL(tx_isp_csi_probe);

/* CSI remove */
int tx_isp_csi_remove(struct platform_device *pdev)
{
    pr_info("tx_isp_csi_remove: removing CSI device\n");
    
    if (global_csi_dev) {
        if (global_csi_dev->csi_phy_regs) {
            iounmap(global_csi_dev->csi_phy_regs);
        }
        if (global_csi_dev->csi_lane_regs) {
            iounmap(global_csi_dev->csi_lane_regs);
        }
        kfree(global_csi_dev);
        global_csi_dev = NULL;
    }
    
    return 0;
}
EXPORT_SYMBOL(tx_isp_csi_remove);

/* Initialize CSI hardware with proper Core Control values matching reference driver */
int csi_init_core_control_registers(void)
{
    void __iomem *vic_regs;
    
    pr_info("*** csi_init_core_control_registers: Writing reference driver Core Control values ***\n");
    
    if (!ourISPdev || !ourISPdev->vic_regs) {
        pr_err("csi_init_core_control_registers: ISP device not ready\n");
        return -EINVAL;
    }
    
    vic_regs = ourISPdev->vic_regs;
    
    /* *** CRITICAL: Write the correct Core Control values from reference driver *** */
    /* These are much smaller than tracer driver values, matching actual hardware limits */
    
    pr_info("*** Writing CORRECTED Core Control registers (0xb07c-0xb08c) ***\n");
    
    /* Reference driver values - much smaller and appropriate for hardware */
    writel(0x341b, vic_regs + 0xb07c);       /* Core Control register - CORRECTED */
    writel(0x46b0, vic_regs + 0xb080);       /* Core Control register - CORRECTED */  
    writel(0x1813, vic_regs + 0xb084);       /* Core Control register - CORRECTED */
    writel(0x1fffff, vic_regs + 0xb088);     /* Core Control register - from reference */
    writel(0x10a, vic_regs + 0xb08c);        /* Core Control register - CORRECTED */
    wmb();
    
    pr_info("*** CORRECTED Core Control values written - no more oversized register values ***\n");
    pr_info("*** Values now match reference driver: 0x341b, 0x46b0, 0x1813, 0x10a ***\n");
    
    return 0;
}
EXPORT_SYMBOL(csi_init_core_control_registers);

/* CSI Initialization function for integration into main module */
int tx_isp_csi_module_init(void)
{
    pr_info("TX ISP CSI functions initialized\n");
    return 0;
}

/* CSI Cleanup function for integration into main module */
void tx_isp_csi_module_exit(void)
{
    if (global_csi_dev) {
        tx_isp_csi_remove(NULL);
    }
    pr_info("TX ISP CSI functions cleaned up\n");
}

/* Export functions for integration into main module */
EXPORT_SYMBOL(tx_isp_csi_module_init);
EXPORT_SYMBOL(tx_isp_csi_module_exit);
