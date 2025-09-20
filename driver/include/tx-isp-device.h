#ifndef __TX_ISP_DEVICE_H__
#define __TX_ISP_DEVICE_H__

#include <linux/errno.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/types.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <linux/miscdevice.h>
#include <linux/v4l2-common.h>
#include "tx-isp-debug.h"
#include "tx-libimp.h"


enum tx_isp_subdev_id {
	TX_ISP_CORE_SUBDEV_ID,
	TX_ISP_MAX_SUBDEV_ID,
};


#define ISP_MAX_CHAN 6
#define VIC_MAX_CHAN 6
#define WDR_SHADOW_SIZE (64 * 1024)
#define ISP_CLK_RATE 100000000


#define TX_ISP_ENTITY_ENUM_MAX_DEPTH	16

/* Pad state definitions */
#define TX_ISP_PADSTATE_FREE        (0x2)
#define TX_ISP_PADSTATE_LINKED      (0x3)
#define TX_ISP_PADSTATE_STREAM      (0x4)


/* Device identification */
#define TX_ISP_DEVICE_NAME "tx-isp-t31"
#define TX_ISP_DRIVER_VERSION "1.0.0"

/* Hardware capabilities */
#define TX_ISP_MAX_WIDTH    2592
#define TX_ISP_MAX_HEIGHT   1944
#define TX_ISP_MIN_WIDTH    64
#define TX_ISP_MIN_HEIGHT   64

/* Register base addresses */
#define TX_ISP_CORE_BASE   0x13300000
#define TX_ISP_VIC_BASE    0x10023000
#define TX_ISP_CSI_BASE    0x10022000

/* Register offsets */
/* Core registers */
#define ISP_CTRL           0x0000
#define ISP_STATUS         0x0004
#define ISP_INT_MASK       0x0008
#define ISP_INT_STATUS     0x000C
#define ISP_FRM_SIZE       0x0010
#define ISP_FRM_FORMAT     0x0014

/* VIC registers */
#define VIC_CTRL           0x0000
#define VIC_STATUS         0x0004
#define VIC_INT_MASK       0x0008
#define VIC_INT_STATUS     0x000C
#define VIC_FRAME_SIZE     0x0010
#define VIC_BUFFER_ADDR    0x0014

/* CSI registers */
#define CSI_CTRL           0x0000
#define CSI_STATUS         0x0004
#define CSI_INT_MASK       0x0008
#define CSI_INT_STATUS     0x000C
#define CSI_DATA_TYPE      0x0010
#define CSI_LANE_CTRL      0x0014

/* VIN registers - see tx_isp_vin.h for detailed definitions */

/* Control register bit definitions */
/* ISP_CTRL bits */
#define ISP_CTRL_EN        BIT(0)
#define ISP_CTRL_RST       BIT(1)
#define ISP_CTRL_CAPTURE   BIT(2)
#define ISP_CTRL_WDR_EN    BIT(3)

/* VIC_CTRL bits */
#define VIC_CTRL_EN        BIT(0)
#define VIC_CTRL_RST       BIT(1)
#define VIC_CTRL_START     BIT(2)
#define VIC_CTRL_STOP      BIT(3)

/* CSI_CTRL bits */
#define CSI_CTRL_EN        BIT(0)
#define CSI_CTRL_RST       BIT(1)
#define CSI_CTRL_LANES_1   (0 << 2)
#define CSI_CTRL_LANES_2   (1 << 2)
#define CSI_CTRL_LANES_4   (2 << 2)

/* VIN_CTRL bits */
#define VIN_CTRL_EN        BIT(0)
#define VIN_CTRL_RST       BIT(1)
#define VIN_CTRL_START     BIT(2)
#define VIN_CTRL_STOP      BIT(3)

/* Interrupt bits - common across all components */
#define INT_FRAME_START    BIT(0)
#define INT_FRAME_DONE     BIT(1)
#define INT_ERROR          BIT(2)
#define INT_OVERFLOW       BIT(3)

/* Format definitions - see tx_isp_vin.h for VIN-specific format constants */

/* Status register bit definitions - see tx_isp_vin.h for VIN-specific status bits */
#define STATUS_ERROR       BIT(2)

struct isp_device_status {
	u32 power_state;
	u32 sensor_state;
	u32 isp_state;
	u32 error_flags;
};

struct isp_device_link_state {
	bool csi_linked;
	bool vic_linked;
	bool vin_linked;
	u32 link_flags;
};

struct isp_component_status {
	u32 state;
	u32 flags;
	u32 error_count;
	u32 frame_count;
};

struct ae_statistics {
	u32 exposure_time;
	u32 gain;
	u32 histogram[256];
	u32 average_brightness;
	bool converged;
};




/* Declare functions but don't define them */
/* isp_read32 removed - use system_reg_read from reference driver instead */
void isp_write32(u32 reg, u32 val);
u32 vic_read32(u32 reg);
void vic_write32(u32 reg, u32 val);
u32 csi_read32(u32 reg);
void csi_write32(u32 reg, u32 val);


/* Device configuration structure */
struct tx_isp_config {
    u32 width;
    u32 height;
    u32 format;
    u32 fps;
    bool wdr_enable;
    u32 lane_num;
};

/* Forward declaration of device structure */
struct tx_isp_dev;

/* Device operations structure */
struct tx_isp_ops {
    int (*init)(struct tx_isp_dev *isp);
    void (*cleanup)(struct tx_isp_dev *isp);
    int (*start)(struct tx_isp_dev *isp);
    void (*stop)(struct tx_isp_dev *isp);
    int (*set_format)(struct tx_isp_dev *isp, struct tx_isp_config *config);
    int (*get_format)(struct tx_isp_dev *isp, struct tx_isp_config *config);
};

/* Description of the connection between modules */
struct link_pad_description {
	char *name; 		// the module name
	unsigned char type;	// the pad type
	unsigned char index;	// the index in array of some pad type
};

struct tx_isp_link_config {
	struct link_pad_description src;
	struct link_pad_description dst;
	unsigned int flag;
};

struct tx_isp_link_configs {
	struct tx_isp_link_config *config;
	unsigned int length;
};

/* The description of module entity */
struct tx_isp_subdev_link {
	struct tx_isp_subdev_pad *source;	/* Source pad */
	struct tx_isp_subdev_pad *sink;		/* Sink pad  */
	struct tx_isp_subdev_link *reverse;	/* Link in the reverse direction */
	unsigned int flag;				/* Link flag (TX_ISP_LINKTYPE_*) */
	unsigned int state;				/* Link state (TX_ISP_MODULE_*) */
};

struct tx_isp_subdev_pad {
	struct tx_isp_subdev *sd;	/* Subdev this pad belongs to */
	unsigned char index;			/* Pad index in the entity pads array */
	unsigned char type;			/* Pad type (TX_ISP_PADTYPE_*) */
	unsigned char links_type;			/* Pad link type (TX_ISP_PADLINK_*) */
	unsigned char state;				/* Pad state (TX_ISP_PADSTATE_*) */
	struct tx_isp_subdev_link link;	/* The active link */
	int (*event)(struct tx_isp_subdev_pad *, unsigned int event, void *data);
    void (*event_callback)(struct tx_isp_subdev_pad *pad, void *priv);
	void *priv;
};

struct tx_isp_dbg_register {
	char *name;
	unsigned int size;
	unsigned long long reg;
	unsigned long long val;
};

struct tx_isp_dbg_register_list {
	char *name;
	unsigned int size;
	unsigned short *reg;
};

struct tx_isp_chip_ident {
	char name[32];
	char *revision;
	unsigned int ident;
};

struct tx_isp_subdev_core_ops {
	int (*g_chip_ident)(struct tx_isp_subdev *sd, struct tx_isp_chip_ident *chip);
	int (*init)(struct tx_isp_subdev *sd, int on);		// clk's, power's and init ops.
	int (*reset)(struct tx_isp_subdev *sd, int on);
	int (*g_register)(struct tx_isp_subdev *sd, struct tx_isp_dbg_register *reg);
	int (*g_register_list)(struct tx_isp_subdev *sd, struct tx_isp_dbg_register_list *reg);
	int (*g_register_all)(struct tx_isp_subdev *sd, struct tx_isp_dbg_register_list *reg);
	int (*s_register)(struct tx_isp_subdev *sd, const struct tx_isp_dbg_register *reg);
	int (*ioctl)(struct tx_isp_subdev *sd, unsigned int cmd, void *arg);
	irqreturn_t (*interrupt_service_routine)(struct tx_isp_subdev *sd, u32 status, bool *handled);
	irqreturn_t (*interrupt_service_thread)(struct tx_isp_subdev *sd, void *data);
};

struct tx_isp_subdev_video_ops {
	int (*s_stream)(struct tx_isp_subdev *sd, int enable);
	int (*link_stream)(struct tx_isp_subdev *sd, int enable);
	int (*link_setup)(const struct tx_isp_subdev_pad *local,
			  const struct tx_isp_subdev_pad *remote, u32 flags);
};

struct tx_isp_subdev_sensor_ops {
	int (*release_all_sensor)(struct tx_isp_subdev *sd);
	int (*sync_sensor_attr)(struct tx_isp_subdev *sd, void *arg);
	int (*ioctl)(struct tx_isp_subdev *sd, unsigned int cmd, void *arg);
};

/* Sensor operations structure - needed for sensor_init */
struct tx_isp_sensor_ops {
	int (*s_stream)(struct tx_isp_subdev *sd, int enable);
	int (*g_register)(struct tx_isp_subdev *sd, struct tx_isp_dbg_register *reg);
	int (*s_register)(struct tx_isp_subdev *sd, const struct tx_isp_dbg_register *reg);
};

struct tx_isp_subdev_pad_ops {
	int (*g_fmt)(struct tx_isp_subdev *sd, struct v4l2_format *f);
	int (*s_fmt)(struct tx_isp_subdev *sd, struct v4l2_format *f);
	int (*streamon)(struct tx_isp_subdev *sd, void *data);
	int (*streamoff)(struct tx_isp_subdev *sd, void *data);
};

struct tx_isp_irq_device {
	spinlock_t slock;
	/*struct mutex mlock;*/
	int irq;
	void (*enable_irq)(struct tx_isp_irq_device *irq_dev);
	void (*disable_irq)(struct tx_isp_irq_device *irq_dev);
};

/* IRQ info structure for Binary Ninja compatibility */
struct tx_isp_irq_info {
	int irq;
	void *handler;
	void *data;
};

enum tx_isp_module_state {
	TX_ISP_MODULE_UNDEFINE = 0,
	TX_ISP_MODULE_SLAKE,
	TX_ISP_MODULE_ACTIVATE,
	TX_ISP_MODULE_DEINIT = TX_ISP_MODULE_ACTIVATE,
	TX_ISP_MODULE_INIT,
	TX_ISP_MODULE_RUNNING,
};



/* All TX descriptors have these 2 fields at the beginning */
struct tx_isp_descriptor {
	unsigned char  type;
	unsigned char  subtype;
	unsigned char  parentid;
	unsigned char  unitid;
};


struct tx_isp_module {
	struct tx_isp_descriptor desc;
	struct device *dev;
	const char *name;
	struct miscdevice miscdev;
	struct file_operations *ops;
	struct file_operations *debug_ops;
	void *parent;
	int (*notify)(struct tx_isp_module *module, unsigned int notification, void *data);
};


struct tx_isp_subdev {
	/* Base module */
	struct tx_isp_module module;
	struct tx_isp_irq_device irqdev;
	struct tx_isp_chip_ident chip;

	/* CRITICAL: Fixed padding to match Binary Ninja offsets */
	char padding_to_0xc0[0x40];

	/* Basic device info */
	struct device *dev;                 /* 0xc0: Device pointer */
	struct platform_device *pdev;      /* 0xc4: Platform device */
	struct resource *res;               /* 0xc8: Resource */
	struct tx_isp_subdev_ops *ops;      /* 0xcc: Operations */
	void *dev_priv;                     /* 0xd0: Private data */
	void *host_priv;                    /* 0xd4: Host private data */

	/* Memory mappings */
	void __iomem *base;         /* Common register base */
	void __iomem *regs;         /* Binary Ninja: *(arg2 + 0xb8) register mapping */
	struct resource *mem_res;   /* Binary Ninja: *(arg2 + 0xb4) memory resource */
	struct tx_isp_irq_info irq_info;  /* Binary Ninja: arg2 + 0x80 IRQ information */
	int clk_num;                /* Binary Ninja: *(arg2 + 0xc0) clock count */
	void __iomem *isp;          /* ISP register base */
	void __iomem *csi_base;     /* CSI register base */

	/* Clocks */
	struct clk **clks;

	/* Synchronization */
	spinlock_t lock;
	spinlock_t vic_lock;
	struct mutex mutex;
	struct mutex vic_frame_end_lock;
	struct mutex csi_lock;
	struct mutex vin_lbc_lock;
	struct completion frame_end;
	struct completion vic_frame_end_completion[VIC_MAX_CHAN];

	/* Pad configuration */
	unsigned short num_outpads;    /* Number of sink pads */
	unsigned short num_inpads;     /* Number of source pads */
	struct tx_isp_subdev_pad *outpads;  /* OutPads array */
	struct tx_isp_subdev_pad *inpads;   /* InPads array */

	/* Specific subsystem data */
	void *fs_wdr_shadow;      /* FS specific shadow memory */

	/* VIN specific fields needed to match OEM behavior */
	struct tx_isp_sensor *active_sensor;  /* Replaces sensor field */
	int vin_state;                        /* Replaces state field */

     /* Sensor list management */
    struct list_head sensor_list;       /* Head of the sensor list */
    struct list_head sensor_list_next;  /* Next entry in sensor list */

    struct tx_isp_frame_channel *frame_chans;
    int num_channels;
};


/* Channel configuration structure - SAFE replacement for raw offset access */
struct tx_isp_channel_config {
    void *reserved[7];                       /* +0x00-0x18: Reserved space (28 bytes) */
    int (*event_handler)(void*);             /* +0x1c: Event handler function - SAFE ACCESS */
    uint32_t padding[2];                     /* +0x20-0x24: Padding to 0x24 size */
} __attribute__((packed, aligned(4)));

/* Netlink socket structure - SAFE replacement for raw offset access at 0x130 */
struct tx_isp_netlink_socket {
    char reserved[0x130];                    /* +0x00-0x12F: Reserved space */
    struct socket *socket_ptr;               /* +0x130: Socket pointer - SAFE ACCESS */
    uint32_t padding[4];                     /* Additional padding */
} __attribute__((packed, aligned(4)));

/* Global frame source device structure - 0xe8 bytes as per Binary Ninja */
struct tx_isp_fs_device {
	struct tx_isp_subdev subdev;            /* Base subdev structure */
	void __iomem *base_regs;                /* Base register mapping +0xb8 */

	struct tx_isp_channel_config *channel_configs;  /* SAFE: Proper typed channel config array */
	void *channel_buffer;                    /* kmalloc'ed channel buffer */
	uint32_t channel_count;                  /* number of channels */
	uint32_t initialized;                    /* initialization flag */
	void *self_ptr;                          /* Self-pointer for validation */
} __attribute__((packed));


/* Device structures */
//struct isp_channel {
//	int id;
//	bool enabled;
//	u32 format;
//	u32 width;
//	u32 height;
//	void *private;
//};

struct imp_channel_attr {
    uint32_t enable;          // 0x00
    uint32_t width;           // 0x04
    uint32_t height;          // 0x08
    uint32_t format;          // 0x0c
    uint32_t crop_enable;     // 0x10
    struct {
        uint32_t x;           // 0x14
        uint32_t y;           // 0x18
        uint32_t width;       // 0x1c
        uint32_t height;      // 0x20
    } crop;
    uint32_t scaler_enable;   // 0x24
    uint32_t scaler_outwidth; // 0x28
    uint32_t scaler_outheight;// 0x2c
    uint32_t picwidth;        // 0x30
    uint32_t picheight;       // 0x34
    uint32_t fps_num;         // 0x38
    uint32_t fps_den;         // 0x3c
    // Total size must be 0x50 bytes to match firmware copy size
    uint32_t reserved[4];     // Pad to 0x50 bytes
} __attribute__((aligned(4)));

struct isp_channel {
    int id;
    /* Core identification */
    uint32_t magic;                    // Magic identifier
    int channel_id;                    // Channel number
    uint32_t state;                    // Channel state flags
    uint32_t type;                     // Channel type
    struct device *dev;                // Parent device
    struct tx_isp_subdev subdev;  // Add this
    struct tx_isp_subdev_pad pad; // And this
    uint32_t flags;                    // Channel flags
    uint32_t is_open;                  // Open count
    bool enabled;                    // Streaming state
    struct vm_area_struct *vma;

    /* Channel attributes */
    struct imp_channel_attr attr;      // Channel attributes (maintains firmware layout)
    struct frame_thread_data *thread_data;    // Thread data

    /* Format and frame info */
    uint32_t width;                    // Frame width
    uint32_t height;                   // Frame height
    uint32_t fmt;                      // Pixel format
    uint32_t sequence;                 // Frame sequence number

    /* VBM Pool Management */
    struct {
        uint32_t pool_id;
        uint32_t pool_flags;  // Add pool flags from reqbuf
        bool bound;
        void *pool_ptr;      // VBM pool pointer
        void *ctrl_ptr;      // VBM control pointer
    } pool;                  // Consolidate pool-related fields

    /* Event Management */
    void *remote_dev;             // 0x2bc: Remote device handlers
    struct isp_event_handler *event_hdlr;  // Event handler structure
    spinlock_t event_lock;        // 0x2c4: Event lock
    struct completion done;       // 0x2d4: Completion
    struct isp_channel_event *event;
    isp_event_cb event_cb;
    void *event_priv;

    /* Memory management */
    void *buf_base;                    // Virtual base address
    dma_addr_t dma_addr;               // DMA base address
    dma_addr_t *buffer_dma_addrs;      // DMA addresses of each video_buffer
    void *dma_y_virt;
    dma_addr_t dma_y_phys;
    void *dma_uv_virt;
    dma_addr_t dma_uv_phys;
    uint32_t group_offset;             // Group offset
    uint32_t buf_size;                 // Size per buffer
    uint32_t buffer_count;                // Number of buffers
    uint32_t channel_offset;           // Channel memory offset
    uint32_t memory_type;              // Memory allocation type
    uint32_t required_size;            // Required buffer size
    void **vbm_table;                  // VBM entries array
    u32 vbm_count;                     // Number of VBM entries
    uint32_t data_offset;              // Frame data offset
    uint32_t metadata_offset;          // Metadata offset
    dma_addr_t *meta_dma;              // Array of metadata DMA addresses
    uint32_t phys_size;                // Physical memory size per buffer
    uint32_t virt_size;                // Virtual memory size per buffer
    struct vbm_pool *vbm_ptr;          // Pointer to VBM pool structure
    struct vbm_ctrl *ctrl_ptr;
    void __iomem *mapped_vbm;  // Mapped VBM pool memory
    uint32_t stride;                   // Line stride in bytes for frame data
    uint32_t buffer_stride;            // Total stride between buffers including metadata
    bool pre_dequeue_enabled;          // Whether pre-dequeue is enabled for ch0
    void __iomem *ctrl_block;     // Mapped control block
    bool ctrl_block_mapped;       // Control block mapping status
    // For 3.10, just store the physical address and size
    unsigned long ctrl_phys;
    size_t ctrl_size;

    /* Queue and buffer management */
    struct frame_queue *queue;
    atomic_t queued_bufs;              // Available buffer count
    struct frame_group *group;          // Frame grouping info
    struct group_data *group_data;      // Group data
    unsigned long group_phys_mem;       // Physical memory pages
    void __iomem *group_mapped_addr;    // Mapped memory address
    struct frame_node *last_frame;      // Last processed frame

    /* Thread management */
    struct task_struct *frame_thread;   // Frame processing thread
    atomic_t thread_running;            // Thread state
    atomic_t thread_should_stop;        // Thread stop flag

    /* Synchronization */
    spinlock_t vbm_lock;               // VBM access protection
    spinlock_t state_lock;             // State protection
    struct completion frame_complete;   // Frame completion tracking

    /* Statistics */
    struct ae_statistics ae_stats;      // AE statistics
    spinlock_t ae_lock;                // AE lock
    bool ae_valid;                     // AE validity flag
    uint32_t last_irq;                 // Last IRQ status
    uint32_t error_count;              // Error counter
} __attribute__((aligned(8)));

// Frame thread data structure
struct frame_thread_data {
    struct isp_channel *chn;    // Channel being processed
    atomic_t should_stop;       // Stop flag
    atomic_t thread_running;    // Running state
    struct task_struct *task;   // Thread task structure
};

struct tx_isp_frame_channel {
    struct miscdevice misc;
    char name[32];
    struct tx_isp_subdev_pad *pad;
    int pad_id;
    spinlock_t slock;
    struct mutex mlock;
    struct completion frame_done;
    int state;
    int active;

    // Queue management from OEM
    struct list_head queue_head;
    struct list_head done_head;
    int queued_count;
    int done_count;
    wait_queue_head_t wait;
};

/* Frame channel state management - shared between tx-isp-module.c and tx_isp_vic.c */
struct tx_isp_channel_state {
    bool enabled;
    bool streaming;
    int format;
    int width;
    int height;
    int buffer_count;
    uint32_t sequence;           /* Frame sequence counter */

    /* CRITICAL FIX: Store buffer structures like reference driver */
    uint32_t *buffer_addresses;  /* Array of buffer structure pointers */

    /* VBM buffer management for VBMFillPool compatibility */
    uint32_t *vbm_buffer_addresses;       /* Array of VBM buffer addresses */
    int vbm_buffer_count;                 /* Number of VBM buffers */
    uint32_t vbm_buffer_size;             /* Size of each VBM buffer */
    spinlock_t vbm_lock;                  /* Protect VBM buffer access */

    /* Reference driver buffer queue management */
    struct list_head queued_buffers;       /* List of queued buffers (ready for VIC) */
    struct list_head completed_buffers;    /* List of completed buffers (ready for DQBUF) */
    spinlock_t queue_lock;                 /* Protect queue access */
    wait_queue_head_t frame_wait;          /* Wait queue for frame completion */
    int queued_count;                      /* Number of queued buffers */
    int completed_count;                   /* Number of completed buffers */

    /* Legacy fields for compatibility */
    struct frame_buffer current_buffer;     /* Current active buffer */
    spinlock_t buffer_lock;                /* Protect buffer access */
    bool frame_ready;                      /* Simple frame ready flag */
};

// Frame channel devices - create video channel devices like reference
// CRITICAL: Add proper alignment and validation for MIPS
struct frame_channel_device {
    struct miscdevice miscdev;
    int channel_num;
    struct tx_isp_channel_state state;

    /* Binary Ninja buffer management fields - ALIGNED for MIPS */
    struct mutex buffer_mutex;           /* Offset 0x28 - private_mutex_lock($s0 + 0x28) */
    spinlock_t buffer_queue_lock;        /* Offset 0x2c4 - __private_spin_lock_irqsave($s0 + 0x2c4) */
    void *buffer_queue_head;             /* Offset 0x214 - *($s0 + 0x214) */
    void *buffer_queue_base;             /* Offset 0x210 - $s0 + 0x210 */
    int buffer_queue_count;              /* Offset 0x218 - *($s0 + 0x218) */
    int streaming_flags;                 /* Offset 0x230 - *($s0 + 0x230) & 1 */
    void *vic_subdev;                    /* Offset 0x2bc - *($s0 + 0x2bc) */
    int buffer_type;                     /* Offset 0x24 - *($s0 + 0x24) */
    int field;                           /* Offset 0x3c - *($s0 + 0x3c) */
    void *buffer_array[64];              /* Buffer array for index lookup */

    /* CRITICAL: Add validation magic number to detect corruption */
    uint32_t magic;                      /* Magic number for validation */
} __attribute__((aligned(8), packed));   /* MIPS-safe alignment */

#define FRAME_CHANNEL_MAGIC 0xDEADBEEF

/* External declarations for frame channel arrays */
extern struct frame_channel_device frame_channels[];
extern int num_channels;

/*
 * Internal ops. Never call this from drivers, only the tx isp device can call
 * these ops.
 *
 * activate_module: called when this subdev is enabled. When called the module
 * could be operated;
 *
 * slake_module: called when this subdev is disabled. When called the
 *	module couldn't be operated.
 *
 */
struct tx_isp_subdev_internal_ops {
	int (*activate_module)(struct tx_isp_subdev *sd);
	int (*slake_module)(struct tx_isp_subdev *sd);
};

struct tx_isp_subdev_ops {
	struct tx_isp_subdev_core_ops		*core;
	struct tx_isp_subdev_video_ops		*video;
	struct tx_isp_subdev_pad_ops		*pad;
	struct tx_isp_subdev_sensor_ops		*sensor;
	struct tx_isp_subdev_internal_ops	*internal;
};

#define tx_isp_call_module_notify(ent, args...)				\
	(!(ent) ? -ENOENT : (((ent)->notify) ?				\
			     (ent)->notify(((ent)->parent), ##args) : -ENOIOCTLCMD))

#define tx_isp_call_subdev_notify(ent, args...)				\
	(!(ent) ? -ENOENT : (((ent)->module.notify) ?			\
			     ((ent)->module.notify(&((ent)->module), ##args)): -ENOIOCTLCMD))

#define tx_isp_call_subdev_event(ent, args...)				\
	(!(ent) ? -ENOENT : (((ent)->event) ?				\
			     (ent)->event((ent), ##args) : -ENOIOCTLCMD))

#define tx_isp_subdev_call(sd, o, f, args...)				\
	(!(sd) ? -ENODEV : (((sd)->ops->o && (sd)->ops->o->f) ?		\
			    (sd)->ops->o->f((sd) , ##args) : -ENOIOCTLCMD))

#define TX_ISP_PLATFORM_MAX_NUM 16

struct tx_isp_platform {
	struct platform_device *dev;
	struct platform_driver *drv;
};

#define miscdev_to_module(mdev) (container_of(mdev, struct tx_isp_module, miscdev))
#define module_to_subdev(mod) (container_of(mod, struct tx_isp_subdev, module))
#define irqdev_to_subdev(dev) (container_of(dev, struct tx_isp_subdev, irqdev))
#define module_to_ispdev(mod) (container_of(mod, struct tx_isp_dev, module))


#define tx_isp_sd_readl(sd, reg)		\
	tx_isp_readl(((sd)->base), reg)
#define tx_isp_sd_writel(sd, reg, value)	\
	tx_isp_writel(((sd)->base), reg, value)

int tx_isp_reg_set(struct tx_isp_subdev *sd, unsigned int reg, int start, int end, int val);
int tx_isp_subdev_init(struct platform_device *pdev, struct tx_isp_subdev *sd, struct tx_isp_subdev_ops *ops);
void tx_isp_subdev_deinit(struct tx_isp_subdev *sd);
int tx_isp_sensor_complete_init(struct tx_isp_subdev *sd);

/* Auto-linking function to connect subdevices to global ISP device */
void tx_isp_subdev_auto_link(struct platform_device *pdev, struct tx_isp_subdev *sd);

static inline void tx_isp_set_module_nodeops(struct tx_isp_module *module, struct file_operations *ops)
{
	module->ops = ops;
}

static inline void tx_isp_set_module_debugops(struct tx_isp_module *module, struct file_operations *ops)
{
	module->debug_ops = ops;
}

static inline void tx_isp_set_subdev_nodeops(struct tx_isp_subdev *sd, struct file_operations *ops)
{
	tx_isp_set_module_nodeops(&sd->module, ops);
}

static inline void tx_isp_set_subdev_debugops(struct tx_isp_subdev *sd, struct file_operations *ops)
{
	tx_isp_set_module_debugops(&sd->module, ops);
}

static inline void tx_isp_set_subdevdata(struct tx_isp_subdev *sd, void *data)
{
	sd->dev_priv = data;
}

static inline void *tx_isp_get_subdevdata(struct tx_isp_subdev *sd)
{
	return sd->dev_priv;
}

static inline void tx_isp_set_subdev_hostdata(struct tx_isp_subdev *sd, void *data)
{
	sd->host_priv = data;
}

static inline void *tx_isp_get_subdev_hostdata(struct tx_isp_subdev *sd)
{
	return sd->host_priv;
}

/* Forward declarations for structures used in tx_isp_dev */
struct isp_tuning_state;
struct ae_info;
struct awb_info;
struct ae_state_info;
struct af_zone_info;
struct isp_core_ctrl;
struct IspModule;

/* White balance gains structure */
struct wb_gains {
	uint32_t r;
	uint32_t g;
	uint32_t b;
};

/* AF zone data structure matching Binary Ninja reference */
struct af_zone_data {
	uint32_t status;
	uint32_t zone_metrics[MAX_AF_ZONES];
};

/* Global AF zone data - Binary Ninja reference */
extern struct af_zone_data af_zone_data;

/* AF zone info structure */
struct af_zone_info {
    uint32_t zone_metrics[MAX_AF_ZONES];  // Zone metrics like contrast values
    uint32_t zone_status;                 // Overall AF status
    uint32_t flags;                       // Zone configuration flags
    struct {
        uint16_t x;                       // Zone X position
        uint16_t y;                       // Zone Y position
        uint16_t width;                   // Zone width
        uint16_t height;                  // Zone height
    } windows[MAX_AF_ZONES];              // AF windows configuration
};

/* AE state info structure */
struct ae_state_info {
	uint32_t exposure;
	uint32_t gain;
	uint32_t status;
};

/* ISP Core Control structure - Binary Ninja reference */
struct isp_core_ctrl {
	uint32_t cmd;     /* Control command */
	int32_t value;    /* Control value */
	u32 flag;         // Additional flags/data
};

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

#endif/*__TX_ISP_DEVICE_H__*/
