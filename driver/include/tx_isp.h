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

/* Platform data structure for subdevices - Binary Ninja compatibility */
struct tx_isp_subdev_platform_data {
    int interface_type;  /* Interface type (1=MIPI, 2=DVP, etc.) */
    int clk_num;        /* Number of clocks */
    int sensor_type;    /* Sensor type */
    /* Additional platform-specific data */
};

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

/* Platform data structure - compatible with Binary Ninja reference implementation */
struct tx_isp_platform_data {
    uint16_t reserved;      /* Padding to offset 2 */
    uint32_t sensor_type;   /* Sensor type at offset 2 */
    uint32_t device_id;     /* Device ID at offset 4 - Binary Ninja: zx.d(*($s2_1 + 4)) */
    uint32_t flags;         /* Additional flags */
    uint32_t version;       /* Version info */
    /* Binary Ninja: Platform devices array at offset 8 - *($s2_1 + 8) */
    struct platform_device **devices;  /* Array of platform device pointers */
} __attribute__((packed));

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

    // Binary Ninja compatibility members
    struct tx_isp_sensor_attribute *sensor_attr;  // Binary Ninja: offset 0x110
};

/* Core ISP device structure */
struct tx_isp_dev {
    struct tx_isp_subdev sd;        // Base subdev at offset 0
    /* Core device info */
    struct device *dev;
    struct device *tisp_device;
    struct miscdevice miscdev;
    struct cdev tisp_cdev;
    spinlock_t lock;
    struct mutex mutex;          /* General mutex for device operations */
    struct proc_context *proc_context;
    struct list_head periph_clocks;
    spinlock_t clock_lock;

    int refcnt;
    //struct tx_isp_subdev_ops *ops;

    /* Device identifiers */
    int major;
    int minor;
    char sensor_name[32];
    u32 sensor_type;
    u32 sensor_mode;
    uint32_t sensor_height;
    uint32_t sensor_width;
    u32 sensor_interface_type;
    u32 vic_status;
    bool is_open;
    struct tx_isp_chip_ident *chip;
    int active_link;

    /* VIC specific */
    uint32_t vic_started;
    int vic_processing;
    u32 vic_frame_size;
    struct list_head vic_buf_queue;

    /* Memory management */
    dma_addr_t rmem_addr;
    size_t rmem_size;
    dma_addr_t dma_addr;
    size_t dma_size;
    void *dma_buf;
    dma_addr_t param_addr;
    void *param_virt;
    uint32_t frame_buf_offset;
    void *y_virt;
    dma_addr_t y_phys;
    void *uv_virt;
    dma_addr_t uv_phys;
    struct resource *mem_region;
    struct resource *csi_region;

    /* Frame sources */
    struct isp_channel channels[ISP_MAX_CHAN];
    struct tx_isp_sensor_win_setting *sensor_window_size;

    /* Hardware subsystems */
    struct tx_isp_csi_device *csi_dev;
    atomic_t csi_configured;

    /* Status tracking */
    struct isp_device_status status;
    struct isp_device_link_state link_state;
    struct isp_component_status core;
    struct isp_component_status vic;
    struct isp_component_status vin;

    /* Platform devices */
    struct platform_device *pdev;
    struct platform_device *vic_pdev;
    struct platform_device *csi_pdev;
    struct platform_device *vin_pdev;
    struct platform_device *core_pdev;
    struct platform_device *fs_pdev;

    /* Clocks */
    struct clk *cgu_isp;
    struct clk *isp_clk;
    struct clk *ipu_clk;
    struct clk *csi_clk;
    
    /* Centralized register mappings */
    void __iomem *core_regs;     /* ISP core registers */
    void __iomem *csi_regs;      /* CSI registers */
    /* vic_regs is already declared above at line 209 */
    /* phy_base is already declared above at line 214 */

    /* GPIO control */
    int reset_gpio;
    int pwdn_gpio;

    /* I2C */
    struct i2c_client *sensor_i2c_client;
    struct i2c_adapter *i2c_adapter;
    struct tx_isp_subdev *sensor_sd;
    struct tx_isp_sensor *sensor;
    /* REMOVED: sensor_attr - use vic_dev->sensor_attr or sensor->video.attr instead */
    struct tx_isp_subdev_ops *sensor_subdev_ops;  /* Sensor subdev operations */
    bool sensor_ops_initialized;                  /* Sensor operations initialization flag */

    /* IRQs */
    int isp_irq;
    int isp_irq2;
    spinlock_t irq_lock;
    volatile u32 irq_enabled;
    void (*irq_handler)(void *);
    void (*irq_disable)(void *);
    void *irq_priv;
    
    /* Binary Ninja interrupt function pointers */
    void (*irq_enable_func)(struct tx_isp_dev *);   /* arg2[1] = tx_isp_enable_irq */
    void (*irq_disable_func)(struct tx_isp_dev *);  /* arg2[2] = tx_isp_disable_irq */
    struct irq_handler_data *isp_irq_data;
    struct completion frame_complete;
    struct task_struct *fw_thread;
    uint32_t frame_count;

    /* VIC IRQ */
    int vic_irq;
    void (*vic_irq_handler)(void *);
    void (*vic_irq_disable)(void *);
    void *vic_irq_priv;
    volatile u32 vic_irq_enabled;
    struct irq_handler_data *vic_irq_data;
    void __iomem *vic_regs;      /* Primary VIC register space (0x133e0000) */
    void __iomem *vic_regs2;     /* Secondary VIC register space (0x10023000) */
    struct tx_isp_vic_device *vic_dev;
    struct ddr_device *ddr_dev;
    struct tx_isp_vin_device *vin_dev;
    struct frame_source_device *fs_dev;
    void __iomem *phy_base;

    /* Statistics */
    struct ae_statistics ae_stats;
    spinlock_t ae_lock;
    bool ae_valid;

    /* Module support */
    struct list_head modules;
    spinlock_t modules_lock;
    int module_count;

    /* Format info */
    uint32_t width;
    uint32_t height;
    uint32_t format;
    uint32_t frame_wait_cnt;
    uint32_t buffer_count;

    /* AE Algorithm */
    int ae_algo_enabled;
    void (*ae_algo_cb)(void *priv, int, int);
    void *ae_priv_data;

    /* Tuning attributes */
    uint32_t g_isp_deamon_info;
    struct isp_tuning_data *tuning_data;
    struct isp_tuning_state *tuning_state;
    int tuning_enabled;
    bool streaming_enabled;
    bool bypass_enabled;
    bool links_enabled;
    u32 instance;
    struct ae_info *ae_info;
    struct awb_info *awb_info;
    uint32_t wdr_mode;
    uint32_t day_night;
    uint32_t custom_mode;
    uint32_t poll_state;
    wait_queue_head_t poll_wait;
    
    /* Pipeline state management */
    int pipeline_state;
    
    /* Binary Ninja compatibility members */
    int subdev_count;                        /* Number of subdevices at offset 0x80 */
    struct platform_device **subdev_list;   /* Subdevice list at offset 0x84 */
    void *subdev_graph[ISP_MAX_SUBDEVS];     /* Subdevice graph array */
    struct proc_dir_entry *proc_dir;         /* Proc directory at offset 0x11c */
    
    /* CRITICAL: Binary Ninja subdev array at offset 0x38 - tx_isp_video_link_stream depends on this */
    struct tx_isp_subdev *subdevs[16];       /* Subdev array at offset 0x38 for tx_isp_video_link_stream */
    
    /* Frame channel devices - needed for tx_isp_create_framechan_devices */
    struct miscdevice *fs_miscdevs[4];       /* Frame source misc devices (/dev/isp-fs*) */
    
    /* ISP proc directory - needed for tx_isp_create_graph_proc_entries */
    struct proc_dir_entry *isp_proc_dir;     /* ISP-specific proc directory */
} __attribute__((aligned(4)));


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
void tx_isp_subdev_deinit(struct tx_isp_subdev *sd);
int tx_isp_sysfs_init(struct tx_isp_dev *isp);
void tx_isp_sysfs_exit(struct tx_isp_dev *isp);
irqreturn_t tx_isp_core_irq_handler(int irq, void *dev_id);

/* Note: tx_isp_reg_set is declared in tx-isp-device.h */
/* Note: tx_isp_enable_irq and tx_isp_disable_irq are declared in tx-isp-module.c */

/* TISP (Tiziano ISP) function declarations */
int tisp_deinit(void);
void tisp_param_operate_deinit(void);
void tisp_event_exit(void);
void tisp_deinit_free(void);
void tisp_netlink_exit(void);
int tisp_code_destroy_tuning_node(void);
void tisp_event_push(void **event);

/* TISP global data pointers - declared as extern in tisp_deinit */
extern void *data_b2f3c;
extern void *data_b2f54;
extern void *data_b2f6c;
extern void *data_b2f78;
extern void *data_b2f84;
extern void *data_b2f90;
extern void *data_b2f9c;
extern void *tparams_day;
extern void *tparams_night;
extern void *tparams_cust;
extern void *opmsg;
extern void *data_ca490;
extern void *data_ca48c;

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
