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

struct vin_device;
struct frame_source_device;


struct vic_device {
    void __iomem *regs;         // Base registers
    struct tx_isp_subdev *sd;
    spinlock_t lock;            // IRQ lock
    struct mutex state_lock;    // Now should be 32-bit aligned

    // State tracking
    int state;                  // Track states: 1=INIT, 2=READY, etc
    u32 mdma_en;               // Group 32-bit values together
    u32 ch0_buf_idx;
    u32 ch0_sub_get_num;
    struct completion frame_complete;

    // Status flags (keep these together)
    bool started;
    bool processing;
    u16 pad2;              // Pad to ensure 32-bit alignment

    // Rest unchanged...
    u32 width;
    u32 height;
    u32 stride;

    void (*irq_handler)(void *);
    void (*irq_disable)(void *);
    void *irq_priv;
    int irq_enabled;
} __attribute__((aligned(4)));  // Force overall structure alignment


struct csi_device {
    char device_name[32];     // 0x00: Device name
    struct device *dev;       // Device pointer
    uint32_t offset_10;       // 0x10: Referenced in init
    struct IspModule *module_info;  // Module info pointer

    struct tx_isp_subdev *sd;

    // CSI register access - changed to single pointer like VIC
    void __iomem *cpm_regs;   // CPM registers
    void __iomem *phy_regs;   // MIPI PHY registers
    void __iomem *csi_regs;   // Single pointer to mapped csi regs
    struct resource *phy_res;  // PHY memory resource

    // State management
    struct clk *clk;
    struct mutex mutex;       // Synchronization
    int state;               // CSI state (1=init, 2=enabled)
    spinlock_t lock;         // Protect register access
};

/* Core ISP device structure */
struct tx_isp_dev {
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
    struct csi_device *csi_dev;
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

    /* GPIO control */
    int reset_gpio;
    int pwdn_gpio;

    /* I2C */
    struct i2c_client *sensor_i2c_client;
    struct i2c_adapter *i2c_adapter;
    struct tx_isp_subdev *sensor_sd;
    struct tx_isp_sensor *sensor;

    /* IRQs */
    int isp_irq;
    spinlock_t irq_lock;
    volatile u32 irq_enabled;
    void (*irq_handler)(void *);
    void (*irq_disable)(void *);
    void *irq_priv;
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
    void __iomem *vic_regs;
    struct vic_device *vic_dev;
    struct ddr_device *ddr_dev;
    struct vin_device *vin_dev;
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
} __attribute__((aligned(4)));


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
int tx_isp_proc_init(struct tx_isp_dev *isp);
void tx_isp_proc_exit(struct tx_isp_dev *isp);
int tx_isp_sysfs_init(struct tx_isp_dev *isp);
void tx_isp_sysfs_exit(struct tx_isp_dev *isp);
irqreturn_t tx_isp_core_irq_handler(int irq, void *dev_id);

void tx_isp_frame_chan_init(struct tx_isp_frame_channel *chan);
void tx_isp_frame_chan_deinit(struct tx_isp_frame_channel *chan);
int tx_isp_setup_default_links(struct tx_isp_dev *dev);

int tx_isp_subdev_register_drivers(struct tx_isp_dev *isp);
void tx_isp_subdev_unregister_drivers(void);

#endif /* __TX_ISP_H__ */
