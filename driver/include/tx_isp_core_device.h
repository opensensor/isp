#ifndef __TX_ISP_CORE_DEVICE_H__
#define __TX_ISP_CORE_DEVICE_H__

#include "tx-isp-device.h"
#include "tx_isp.h"

/* Forward declarations */
struct tx_isp_dev;
struct tx_isp_sensor_attribute;

/**
 * tx_isp_core_device - ISP Core Device Structure
 * 
 * This structure represents the ISP Core as a separate subdevice,
 * following the same pattern as VIC, VIN, CSI, and FS devices.
 * The core functionality is separated from the main tx_isp_dev structure
 * to match the reference driver architecture.
 */
struct tx_isp_core_device {
    /* CRITICAL: Base subdev structure MUST be first for container_of() to work */
    struct tx_isp_subdev sd;                    /* 0x00: Base subdev structure */

    /* Core-specific register mappings */
    void __iomem *core_regs;                    /* ISP core registers */
    void __iomem *tuning_regs;                  /* Tuning registers */
    
    /* Core device state */
    int state;                                  /* Core state: 1=init, 2=ready, 3=active, 4=streaming */
    int streaming;                              /* Streaming state: 0=off, 1=on */
    struct mutex mlock;                         /* Core operations mutex */
    spinlock_t lock;                           /* Core spinlock for interrupt handling */
    
    /* Core-specific clocks */
    struct clk *core_clk;                      /* Core ISP clock */
    struct clk *ipu_clk;                       /* IPU clock */
    
    /* Core interrupt handling */
    int irq;                                   /* Core interrupt number */
    irqreturn_t (*irq_handler)(int irq, void *dev_id);  /* Core IRQ handler */
    
    /* Core tuning system */
    void *tuning_dev;                          /* Tuning device pointer */
    void *tuning_data;                         /* Tuning data structure */
    bool tuning_enabled;                       /* Tuning system enabled flag */
    
    /* Core processing */
    struct work_struct fs_work;                /* Frame sync work structure */
    atomic_t processing_counter;               /* Processing counter */
    bool tisp_initialized;                     /* TISP initialization flag */
    
    /* Core sensor interface */
    struct tx_isp_sensor_attribute *sensor_attr;  /* Current sensor attributes */

    /* Core image processing settings */
    uint32_t wdr_mode;                             /* WDR mode setting */
    
    /* Core frame channels */
    void *frame_channels;                      /* Frame channel array */
    uint32_t channel_count;                    /* Number of channels */
    
    /* Core memory management */
    void *channel_array;                       /* Binary Ninja channel array */
    
    /* Device linkage */
    struct tx_isp_dev *isp_dev;               /* Back-reference to main ISP device */
    struct device *dev;                        /* Platform device */
    struct platform_device *pdev;             /* Platform device pointer */
    
    /* Core-specific status */
    uint32_t core_status;                      /* Core status flags */
    bool is_initialized;                       /* Initialization complete flag */
    
    /* Binary Ninja compatibility */
    void *self_ptr;                           /* Self-pointer for validation */
    uint32_t magic;                           /* Magic number for validation */
};

/* Core device operations */
struct tx_isp_core_ops {
    int (*init)(struct tx_isp_core_device *core_dev);
    int (*deinit)(struct tx_isp_core_device *core_dev);
    int (*start_streaming)(struct tx_isp_core_device *core_dev);
    int (*stop_streaming)(struct tx_isp_core_device *core_dev);
    int (*set_sensor_attr)(struct tx_isp_core_device *core_dev, struct tx_isp_sensor_attribute *attr);
};

/* Core device management functions */
struct tx_isp_core_device *tx_isp_create_core_device(struct platform_device *pdev);
void tx_isp_destroy_core_device(struct tx_isp_core_device *core_dev);
int tx_isp_core_device_init(struct tx_isp_core_device *core_dev);
int tx_isp_core_device_deinit(struct tx_isp_core_device *core_dev);

/* Core device linking functions */
int tx_isp_link_core_device(struct tx_isp_dev *isp_dev, struct tx_isp_core_device *core_dev);
void tx_isp_unlink_core_device(struct tx_isp_dev *isp_dev);

/* Core device state management */
int tx_isp_core_device_set_state(struct tx_isp_core_device *core_dev, int state);
int tx_isp_core_device_get_state(struct tx_isp_core_device *core_dev);

/* Core device streaming control */
int tx_isp_core_device_start_streaming(struct tx_isp_core_device *core_dev);
int tx_isp_core_device_stop_streaming(struct tx_isp_core_device *core_dev);

/* Core device sensor interface */
int tx_isp_core_device_set_sensor_attr(struct tx_isp_core_device *core_dev, 
                                       struct tx_isp_sensor_attribute *attr);

/* Core device register access */
static inline void tx_isp_core_reg_write(struct tx_isp_core_device *core_dev, 
                                         uint32_t reg, uint32_t value)
{
    if (core_dev && core_dev->core_regs) {
        writel(value, core_dev->core_regs + reg);
    }
}

static inline uint32_t tx_isp_core_reg_read(struct tx_isp_core_device *core_dev, 
                                            uint32_t reg)
{
    if (core_dev && core_dev->core_regs) {
        return readl(core_dev->core_regs + reg);
    }
    return 0;
}

/* Core device validation */
static inline bool tx_isp_core_device_is_valid(struct tx_isp_core_device *core_dev)
{
    return (core_dev && core_dev->self_ptr == core_dev && core_dev->magic == 0x434F5245); /* 'CORE' */
}

/* Core device container_of helper */
static inline struct tx_isp_core_device *tx_isp_subdev_to_core_device(struct tx_isp_subdev *sd)
{
    return container_of(sd, struct tx_isp_core_device, sd);
}

#endif /* __TX_ISP_CORE_DEVICE_H__ */
