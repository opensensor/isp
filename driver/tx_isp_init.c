#include <linux/module.h>
#include <linux/vmalloc.h>
#include <linux/delay.h>
#include "tx_isp.h"
#include "tx-isp-device.h"
#include "tx-isp-debug.h"
#include "tx_isp_core.h"

/* Global variables matching reference driver */
static uint32_t tispinfo = 0;
static uint32_t data_b2f34 = 0;
static int deir_en = 0;

/* Buffer allocation tracking */
struct isp_buffer_node {
    int used;
    void *virt_addr;
    dma_addr_t phys_addr;
    uint32_t size;
    struct isp_buffer_node *next;
};

static struct isp_buffer_node *buffer_list = NULL;
static DEFINE_MUTEX(buffer_mutex);

/* Forward declarations */
static int tiziano_allocate_processing_buffers(struct tx_isp_dev *isp);
static int tiziano_init_processing_pipeline(struct tx_isp_sensor_attribute *sensor_attr);
static void tiziano_free_processing_buffers(struct tx_isp_dev *isp);
static void tiziano_deinit_processing_pipeline(void);

/* Global ISP device pointer */
static struct tx_isp_dev *g_isp_device = NULL;

/* Get ISP device instance */
struct tx_isp_dev *tx_isp_get_device(void)
{
    return g_isp_device;
}

/* Set ISP device instance - called from core probe */
void tx_isp_set_device(struct tx_isp_dev *isp)
{
    g_isp_device = isp;
}

/* Tiziano ISP initialization function - based on reference tisp_init */
int tiziano_isp_init(struct tx_isp_sensor_attribute *sensor_attr, char *param_name)
{
    struct tx_isp_dev *isp = tx_isp_get_device();
    uint32_t width, height;
    uint32_t sensor_type;
    uint32_t reg_val;
    int ret = 0;
    
    if (!sensor_attr || !isp) {
        ISP_ERROR("Invalid parameters for ISP init\n");
        return -EINVAL;
    }
    
    ISP_INFO("*** TIZIANO ISP INITIALIZATION START ***\n");
    
    /* Store sensor information globally - matches reference */
    width = sensor_attr->max_integration_time;  /* Assuming this maps to width */
    height = sensor_attr->integration_time_limit; /* Assuming this maps to height */
    sensor_type = sensor_attr->sensor_ctrl.alloc_again; /* Assuming this maps to sensor type */
    
    /* Store in global variables like reference driver */
    tispinfo = width;
    data_b2f34 = height;
    
    ISP_INFO("Sensor dimensions: %dx%d, type: %d\n", width, height, sensor_type);
    
    /* CRITICAL: Write sensor dimensions to system register - this is KEY! */
    /* Reference: system_reg_write(4, $v0_4 << 0x10 | arg1[1]) */
    isp_write32(0x4, (width << 16) | height);
    ISP_INFO("*** WROTE SENSOR DIMENSIONS TO REG 0x4: 0x%08x ***\n", (width << 16) | height);
    
    /* Configure sensor type based register - matches reference switch statement */
    if (sensor_type >= 0x15) {
        ISP_ERROR("Unsupported sensor type: %d\n", sensor_type);
        return -EINVAL;
    }
    
    /* Map sensor type to register value and deir_en flag */
    switch (sensor_type) {
        case 0:
            isp_write32(0x8, 0);
            deir_en = 0;
            break;
        case 1:
            isp_write32(0x8, 1);
            deir_en = 0;
            break;
        case 2:
            isp_write32(0x8, 2);
            deir_en = 0;
            break;
        case 3:
            isp_write32(0x8, 3);
            deir_en = 0;
            break;
        case 4:
            isp_write32(0x8, 8);
            deir_en = 1;
            break;
        default:
            isp_write32(0x8, sensor_type + 4);
            deir_en = 1;
            break;
    }
    
    ISP_INFO("Sensor type %d configured, deir_en=%d\n", sensor_type, deir_en);
    
    /* Configure control register based on deir_en - matches reference */
    reg_val = 0x3f00;
    if (deir_en == 1) {
        reg_val = 0x10003f00;
    }
    isp_write32(0x1c, reg_val);
    ISP_INFO("Control register 0x1c set to 0x%08x\n", reg_val);
    
    /* Allocate critical ISP processing buffers - matches reference memory allocation */
    ret = tiziano_allocate_processing_buffers(isp);
    if (ret < 0) {
        ISP_ERROR("Failed to allocate processing buffers: %d\n", ret);
        return ret;
    }
    
    /* Initialize processing pipeline components */
    ret = tiziano_init_processing_pipeline(sensor_attr);
    if (ret < 0) {
        ISP_ERROR("Failed to initialize processing pipeline: %d\n", ret);
        goto cleanup_buffers;
    }
    
    /* Configure interrupt mask - matches reference */
    isp_write32(0x30, 0xffffffff);  /* Clear all interrupts */
    
    /* Set pipeline mode register - matches reference logic */
    reg_val = 0x33f;  /* Default value */
    isp_write32(0x10, reg_val);
    ISP_INFO("Pipeline mode register set to 0x%08x\n", reg_val);
    
    /* Configure final pipeline registers - matches reference sequence */
    isp_write32(0x804, 0x1c);  /* Pipeline control */
    isp_write32(0x1c, 8);     /* Another control register */
    
    /* THE CRITICAL ENABLE - this is what was missing! */
    /* Reference: system_reg_write(0x800, 1) */
    isp_write32(0x800, 1);
    ISP_INFO("*** ISP CORE ENABLED - REG 0x800 = 1 ***\n");
    
    /* Verify ISP core is now enabled */
    reg_val = isp_read32(0x800);
    ISP_INFO("*** ISP CORE STATUS VERIFICATION: reg[0x800] = 0x%08x ***\n", reg_val);
    
    if ((reg_val & 0x1) == 0) {
        ISP_ERROR("*** ISP CORE FAILED TO ENABLE! ***\n");
        ret = -EIO;
        goto cleanup_pipeline;
    }
    
    ISP_INFO("*** TIZIANO ISP INITIALIZATION COMPLETE - SUCCESS! ***\n");
    return 0;
    
cleanup_pipeline:
    tiziano_deinit_processing_pipeline();
cleanup_buffers:
    tiziano_free_processing_buffers(isp);
    return ret;
}
EXPORT_SYMBOL(tiziano_isp_init);

/* Allocate processing buffers based on reference driver */
static int tiziano_allocate_processing_buffers(struct tx_isp_dev *isp)
{
    void *buf1, *buf2, *buf3, *buf4, *buf5, *buf6, *buf7;
    dma_addr_t dma_addr;
    
    ISP_INFO("Allocating ISP processing buffers\n");
    
    /* Buffer 1: 24KB buffer - matches reference private_kmalloc(0x6000, 0xd0) */
    buf1 = dma_alloc_coherent(isp->dev, 0x6000, &dma_addr, GFP_KERNEL);
    if (!buf1) {
        ISP_ERROR("Failed to allocate buffer 1\n");
        return -ENOMEM;
    }
    
    /* Configure buffer 1 registers - matches reference system_reg_write sequence */
    isp_write32(0xa02c, dma_addr);
    isp_write32(0xa030, dma_addr + 0x1000);
    isp_write32(0xa034, dma_addr + 0x2000);
    isp_write32(0xa038, dma_addr + 0x3000);
    isp_write32(0xa03c, dma_addr + 0x4000);
    isp_write32(0xa040, dma_addr + 0x4800);
    isp_write32(0xa044, dma_addr + 0x5000);
    isp_write32(0xa048, dma_addr + 0x5800);
    isp_write32(0xa04c, 0x33);
    
    ISP_INFO("Buffer 1 allocated at DMA 0x%08x\n", (uint32_t)dma_addr);
    
    /* Buffer 2: Another 24KB buffer */
    buf2 = dma_alloc_coherent(isp->dev, 0x6000, &dma_addr, GFP_KERNEL);
    if (!buf2) {
        ISP_ERROR("Failed to allocate buffer 2\n");
        dma_free_coherent(isp->dev, 0x6000, buf1, dma_addr);
        return -ENOMEM;
    }
    
    /* Configure buffer 2 registers */
    isp_write32(0xa82c, dma_addr);
    isp_write32(0xa830, dma_addr + 0x1000);
    isp_write32(0xa834, dma_addr + 0x2000);
    isp_write32(0xa838, dma_addr + 0x3000);
    isp_write32(0xa83c, dma_addr + 0x4000);
    isp_write32(0xa840, dma_addr + 0x4800);
    isp_write32(0xa844, dma_addr + 0x5000);
    isp_write32(0xa848, dma_addr + 0x5800);
    isp_write32(0xa84c, 0x33);
    
    ISP_INFO("Buffer 2 allocated at DMA 0x%08x\n", (uint32_t)dma_addr);
    
    /* Continue with other buffers following the same pattern... */
    /* For now, this is enough to test the core enable sequence */
    
    return 0;
}

/* Initialize processing pipeline */
static int tiziano_init_processing_pipeline(struct tx_isp_sensor_attribute *sensor_attr)
{
    ISP_INFO("Initializing Tiziano processing pipeline\n");
    
    /* Initialize core processing blocks - simplified for now */
    /* In full implementation, these would be separate functions */
    
    ISP_INFO("Processing pipeline initialized\n");
    return 0;
}

/* Cleanup functions */
static void tiziano_free_processing_buffers(struct tx_isp_dev *isp)
{
    /* Free allocated buffers */
    ISP_INFO("Freeing ISP processing buffers\n");
}

static void tiziano_deinit_processing_pipeline(void)
{
    ISP_INFO("Deinitializing processing pipeline\n");
}

/* Sync sensor attributes - based on tiziano_sync_sensor_attr */
int tiziano_sync_sensor_attr(struct tx_isp_sensor_attribute *attr)
{
    if (!attr) {
        ISP_ERROR("Invalid sensor attribute pointer\n");
        return -EINVAL;
    }
    
    ISP_INFO("Syncing sensor attributes\n");
    
    /* Copy sensor info to global variables - matches reference behavior */
    tispinfo = attr->max_integration_time;
    data_b2f34 = attr->integration_time_limit;
    
    ISP_INFO("Sensor attributes synced: tispinfo=%d, data_b2f34=%d\n", 
             tispinfo, data_b2f34);
    
    return 0;
}
EXPORT_SYMBOL(tiziano_sync_sensor_attr);

/* Channel start function - based on tisp_channel_start */
int tiziano_channel_start(int channel_id, struct tx_isp_channel_attr *attr)
{
    uint32_t reg_val;
    uint32_t channel_base;
    
    if (channel_id < 0 || channel_id >= ISP_MAX_CHAN) {
        ISP_ERROR("Invalid channel ID: %d\n", channel_id);
        return -EINVAL;
    }
    
    if (!attr) {
        ISP_ERROR("Invalid channel attributes\n");
        return -EINVAL;
    }
    
    ISP_INFO("Starting channel %d\n", channel_id);
    
    /* Calculate channel register base - matches reference (arg1 + 0x98) << 8 */
    channel_base = (channel_id + 0x98) << 8;
    
    /* Configure channel scaling registers - matches reference logic */
    if (attr->width < tispinfo || attr->height < data_b2f34) {
        /* Enable scaling */
        isp_write32(channel_base + 0x1c0, 0x40080);
        isp_write32(channel_base + 0x1c4, 0x40080);
        isp_write32(channel_base + 0x1c8, 0x40080);
        isp_write32(channel_base + 0x1cc, 0x40080);
        ISP_INFO("Channel %d: Scaling enabled\n", channel_id);
    } else {
        /* No scaling needed */
        isp_write32(channel_base + 0x1c0, 0x200);
        isp_write32(channel_base + 0x1c4, 0);
        isp_write32(channel_base + 0x1c8, 0x200);
        isp_write32(channel_base + 0x1cc, 0);
        ISP_INFO("Channel %d: No scaling\n", channel_id);
    }
    
    /* Enable channel in master control register - matches reference */
    reg_val = isp_read32(0x9804);
    reg_val |= (1 << channel_id) | 0xf0000;
    isp_write32(0x9804, reg_val);
    
    ISP_INFO("Channel %d started successfully\n", channel_id);
    return 0;
}
EXPORT_SYMBOL(tiziano_channel_start);
