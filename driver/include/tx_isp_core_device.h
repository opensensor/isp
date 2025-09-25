#ifndef __TX_ISP_CORE_DEVICE_H__
#define __TX_ISP_CORE_DEVICE_H__

#include "tx-isp-device.h"
#include "tx_isp.h"


/* ISP Tuning Data structure - Based on Binary Ninja analysis and crash offset */
struct isp_tuning_data {
	/* Base tuning parameters - ensure proper alignment */
	void *regs;                          /* 0x00: Register base pointer */
	spinlock_t lock;                     /* 0x04: Tuning lock */
	struct mutex mutex;                  /* 0x08: Tuning mutex (32-bit aligned) */
	uint32_t state;                      /* 0x0c: Tuning state */

	/* Page allocation tracking for proper cleanup */
	unsigned long allocation_pages;      /* 0x10: Pages allocated via __get_free_pages */
	int allocation_order;                /* 0x14: Allocation order for cleanup */

	/* Control values - CRITICAL: saturation must be at offset +0x68 */
	uint32_t reserved1[20];              /* 0x18-0x67: Reserved for proper alignment (reduced by 2) */

	/* CRITICAL: These must be at the correct offsets for the controls */
	uint32_t saturation;                 /* 0x68: Saturation control (cmd 0x980902) - CRASH LOCATION */
	uint32_t brightness;                 /* 0x6c: Brightness control (cmd 0x980900) */
	uint32_t contrast;                   /* 0x70: Contrast control (cmd 0x980901) */
	uint32_t sharpness;                  /* 0x74: Sharpness control (cmd 0x98091b) */

	/* Additional controls */
	uint32_t hflip;                      /* 0x70: Horizontal flip (cmd 0x980914) */
	uint32_t vflip;                      /* 0x74: Vertical flip (cmd 0x980915) */
	uint32_t antiflicker;                /* 0x78: Anti-flicker (cmd 0x980918) */
	uint32_t shading;                    /* 0x7c: Shading control */

	/* Extended controls */
	uint32_t running_mode;               /* 0x80: ISP running mode */
	uint32_t custom_mode;                /* 0x84: ISP custom mode */
	uint32_t move_state;                 /* 0x88: Move state */
	uint32_t ae_comp;                    /* 0x8c: AE compensation */

	/* Gain controls */
	uint32_t max_again;                  /* 0x90: Maximum analog gain */
	uint32_t max_dgain;                  /* 0x94: Maximum digital gain */
	uint32_t total_gain;                 /* 0x98: Total gain */
	uint32_t exposure;                   /* 0x9c: Exposure value */

	/* Strength controls */
	uint32_t defog_strength;             /* 0xa0: Defog strength */
	uint32_t dpc_strength;               /* 0xa4: DPC strength */
	uint32_t drc_strength;               /* 0xa8: DRC strength */
	uint32_t temper_strength;            /* 0xac: Temper strength */
	uint32_t sinter_strength;            /* 0xb0: Sinter strength */

	/* White balance */
	struct wb_gains wb_gains;            /* 0xb4: WB gains (R,G,B) */
	uint32_t wb_temp;                    /* 0xc0: WB color temperature */

	/* BCSH controls */
	uint8_t bcsh_hue;                    /* 0xc4: BCSH Hue */
	uint8_t bcsh_brightness;             /* 0xc5: BCSH Brightness */
	uint8_t bcsh_contrast;               /* 0xc6: BCSH Contrast */
	uint8_t bcsh_saturation;             /* 0xc7: BCSH Saturation */

	/* FPS control */
	uint32_t fps_num;                    /* 0xc8: FPS numerator */
	uint32_t fps_den;                    /* 0xcc: FPS denominator */

	/* BCSH EV processing - Binary Ninja reference */
	uint32_t bcsh_ev;                    /* 0xd0: BCSH EV value */
	uint32_t bcsh_au32EvList_now[9];     /* 0xd4: EV list array */
	uint32_t bcsh_au32SminListS_now[9];  /* 0xf8: S min list S */
	uint32_t bcsh_au32SmaxListS_now[9];  /* 0x11c: S max list S */
	uint32_t bcsh_au32SminListM_now[9];  /* 0x140: S min list M */
	uint32_t bcsh_au32SmaxListM_now[9];  /* 0x164: S max list M */

	/* BCSH saturation processing */
	uint32_t bcsh_saturation_value;      /* 0x188: Current saturation value */
	uint32_t bcsh_saturation_max;        /* 0x18c: Max saturation */
	uint32_t bcsh_saturation_min;        /* 0x190: Min saturation */
	uint32_t bcsh_saturation_mult;       /* 0x194: Saturation multiplier */

	/* Padding to ensure structure is large enough for all accesses */
	uint32_t reserved2[1000];            /* 0x198+: Reserved for future use and safety */
} __attribute__((aligned(4)));

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
    
    /* REMOVED: Core device state - ALL state management happens through VIC device */
    /* Based on Binary Ninja MCP analysis, core device is stateless */
    int streaming;                              /* Streaming state: 0=off, 1=on */
    struct mutex mlock;                         /* Core operations mutex */
    spinlock_t lock;                           /* Core spinlock for interrupt handling */
    
    /* Core-specific clocks */
    struct clk *core_clk;                      /* Core ISP clock */
    struct clk *ipu_clk;                       /* IPU clock */
    
    /* Core interrupt handling */
    int irq;                                   /* Core interrupt number */
    irqreturn_t (*irq_handler)(int irq, void *dev_id);  /* Core IRQ handler */
    int irq_enabled;                           /* IRQ enabled flag */
    uint32_t irq_mask;                         /* IRQ mask register */

    /* Core tuning system */
    void *tuning_dev;                          /* Tuning device pointer */
    struct isp_tuning_data *tuning_data;        /* Tuning data structure */
    bool tuning_enabled;                       /* Tuning system enabled flag */
    bool bypass_enabled;                       /* ISP bypass mode enabled flag */

    /* Core processing */
    struct work_struct fs_work;                /* Frame sync work structure */
    atomic_t processing_counter;               /* Processing counter */
    bool tisp_initialized;                     /* TISP initialization flag */

    /* Core image processing settings */
    uint32_t wdr_mode;                             /* WDR mode setting */

    /* Core frame channels */
    struct tx_isp_frame_channel *frame_channels;   /* Frame channel array */
    uint32_t channel_count;                        /* Number of channels */

    /* Binary Ninja compatibility - image dimensions at specific offsets */
    char padding_to_ec[0xec - 0x60];               /* Padding to reach offset 0xec */
    uint32_t width;                                /* 0xec: Image width */
    uint32_t height;                               /* 0xf0: Image height */

    /* Core statistics */
    uint32_t frame_count;                      /* Frame counter */
    uint32_t error_count;                      /* Error counter */
    uint32_t drop_count;                       /* Drop counter */
    uint32_t total_frames;                     /* Total frame counter */
    
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

/* REMOVED: Core device state management - ALL state management happens through VIC device */
/* Based on Binary Ninja MCP analysis, core device is stateless */

/* Core device streaming control */
int tx_isp_core_device_start_streaming(struct tx_isp_core_device *core_dev);
int tx_isp_core_device_stop_streaming(struct tx_isp_core_device *core_dev);

/* Core device sensor interface */
int tx_isp_core_device_set_sensor_attr(struct tx_isp_core_device *core_dev,
                                       struct tx_isp_sensor_attribute *attr);

/* Memory management functions */
int parse_rmem_bootarg(unsigned long *base, unsigned long *size);

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
