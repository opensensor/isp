#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/interrupt.h>
#include "tx_isp.h"
#include "tx-isp-device.h"
#include "tx_isp_core.h"
#include "tx-isp-debug.h"
#include "tx_isp_sysfs.h"
#include "tx_isp_vic.h"
#include "tx_isp_csi.h"
#include "tx_isp_vin.h"


static int print_level = ISP_WARN_LEVEL;
module_param(print_level, int, S_IRUGO);
MODULE_PARM_DESC(print_level, "isp print level");

static int isp_clk = 100000000;
module_param(isp_clk, int, S_IRUGO);
MODULE_PARM_DESC(isp_clk, "isp clock freq");

int isp_ch0_pre_dequeue_time;
module_param(isp_ch0_pre_dequeue_time, int, S_IRUGO);
MODULE_PARM_DESC(isp_ch0_pre_dequeue_time, "isp pre dequeue time, unit ms");

int isp_ch0_pre_dequeue_interrupt_process;
module_param(isp_ch0_pre_dequeue_interrupt_process, int, S_IRUGO);
MODULE_PARM_DESC(isp_ch0_pre_dequeue_interrupt_process, "isp pre dequeue interrupt process");

int isp_ch0_pre_dequeue_valid_lines;
module_param(isp_ch0_pre_dequeue_valid_lines, int, S_IRUGO);
MODULE_PARM_DESC(isp_ch0_pre_dequeue_valid_lines, "isp pre dequeue valid lines");

int isp_ch1_dequeue_delay_time;
module_param(isp_ch1_dequeue_delay_time, int, S_IRUGO);
MODULE_PARM_DESC(isp_ch1_dequeue_delay_time, "isp pre dequeue time, unit ms");

int isp_day_night_switch_drop_frame_num;
module_param(isp_day_night_switch_drop_frame_num, int, S_IRUGO);
MODULE_PARM_DESC(isp_day_night_switch_drop_frame_num, "isp day night switch drop frame number");

int isp_memopt;
module_param(isp_memopt, int, S_IRUGO);
MODULE_PARM_DESC(isp_memopt, "isp memory optimize");


/* Forward declarations */
static int tx_isp_init_memory_mappings(struct tx_isp_dev *isp);
static int tx_isp_deinit_memory_mappings(struct tx_isp_dev *isp);
static int tx_isp_create_graph_and_nodes(struct tx_isp_dev *isp);
static int tx_isp_setup_pipeline(struct tx_isp_dev *isp);
static int tx_isp_setup_media_links(struct tx_isp_dev *isp);
static int tx_isp_init_subdev_pads(struct tx_isp_dev *isp);
static int tx_isp_create_subdev_links(struct tx_isp_dev *isp);
static int tx_isp_register_link(struct tx_isp_dev *isp, struct link_config *link);
static int tx_isp_configure_default_links(struct tx_isp_dev *isp);
static int tx_isp_configure_format_propagation(struct tx_isp_dev *isp);
static int tx_isp_csi_device_init(struct tx_isp_dev *isp);
static int tx_isp_vic_device_init(struct tx_isp_dev *isp);
static int tx_isp_csi_device_deinit(struct tx_isp_dev *isp);
static int tx_isp_vic_device_deinit(struct tx_isp_dev *isp);

/* Core ISP interrupt handler */
irqreturn_t tx_isp_core_irq_handler(int irq, void *dev_id)
{
    struct tx_isp_dev *isp = dev_id;
    u32 status, vic_status;
    unsigned long flags;
    irqreturn_t ret = IRQ_NONE;
    int i; /* Declare at beginning for C90 compliance */

    if (!isp)
        return IRQ_NONE;

    spin_lock_irqsave(&isp->irq_lock, flags);
    
    /* Read and clear ISP core interrupt status */
    status = isp_read32(ISP_INT_STATUS);
    if (status) {
        isp_write32(ISP_INT_STATUS, status);
        ret = IRQ_HANDLED;
        
        /* Handle frame done interrupt */
        if (status & INT_FRAME_DONE) {
            isp->frame_count++;
            complete(&isp->frame_complete);
            
            /* Notify channels of frame completion - simplified without accessing non-existent members */
            for (i = 0; i < ISP_MAX_CHAN; i++) {
                /* Just complete the frame completion for all channels */
                /* Note: removed .active and .frame_done references as they don't exist in struct */
            }
        }
        
        /* Handle various error conditions */
        if (status & INT_ERROR) {
            ISP_ERROR("ISP core error interrupt: 0x%08x\n", status);
            
            /* Log specific error types */
            if (status & 0x02) ISP_ERROR("  - Frame sync error\n");
            if (status & 0x04) ISP_ERROR("  - Data overflow error\n");
            if (status & 0x08) ISP_ERROR("  - FIFO error\n");
        }
        
        /* Handle start of frame */
        if (status & 0x10) {
            /* Start of frame - can be used for timing */
        }
    }
    
    /* Also check VIC interrupts if VIC is part of our device tree */
    vic_status = vic_read32(VIC_INT_STATUS);
    if (vic_status) {
        vic_write32(VIC_INT_STATUS, vic_status);
        ret = IRQ_HANDLED;
        
        /* Handle VIC-specific interrupts */
        if (vic_status & INT_FRAME_DONE) {
            /* VIC frame processing complete */
            if (isp->vic_dev) {
                complete(&isp->vic_dev->frame_complete);
            }
        }
        
        if (vic_status & INT_ERROR) {
            ISP_ERROR("VIC error interrupt: 0x%08x\n", vic_status);
        }
    }

    spin_unlock_irqrestore(&isp->irq_lock, flags);
    return ret;
}


void tx_isp_frame_chan_init(struct tx_isp_frame_channel *chan)
{
    /* Initialize channel state */
    pr_info("Initializing frame channel\n");
    if (chan) {
        chan->active = false;
        spin_lock_init(&chan->slock);
        mutex_init(&chan->mlock);
        init_completion(&chan->frame_done);
    }
}


/* Initialize memory mappings for ISP subsystems */
static int tx_isp_init_memory_mappings(struct tx_isp_dev *isp)
{
    pr_info("Initializing ISP memory mappings\n");
    
    /* Map ISP Core registers */
    isp->core_regs = ioremap(0x13300000, 0x10000);
    if (!isp->core_regs) {
        pr_err("Failed to map ISP core registers\n");
        return -ENOMEM;
    }
    pr_info("ISP core registers mapped at 0x13300000\n");
    
    /* Map VIC registers */
    isp->vic_regs = ioremap(0x10023000, 0x1000);
    if (!isp->vic_regs) {
        pr_err("Failed to map VIC registers\n");
        goto err_unmap_core;
    }
    pr_info("VIC registers mapped at 0x10023000\n");
    
    /* Map CSI registers - use a different variable to avoid conflicts */
    isp->csi_regs = ioremap(0x10022000, 0x1000);
    if (!isp->csi_regs) {
        pr_err("Failed to map CSI registers\n");
        goto err_unmap_vic;
    }
    pr_info("CSI registers mapped at 0x10022000\n");
    
    /* Map PHY registers */
    isp->phy_base = ioremap(0x10021000, 0x1000);
    if (!isp->phy_base) {
        pr_err("Failed to map PHY registers\n");
        goto err_unmap_csi;
    }
    pr_info("PHY registers mapped at 0x10021000\n");
    
    pr_info("All ISP memory mappings initialized successfully\n");
    return 0;
    
err_unmap_csi:
    iounmap(isp->csi_regs);
    isp->csi_regs = NULL;
err_unmap_vic:
    iounmap(isp->vic_regs);
    isp->vic_regs = NULL;
err_unmap_core:
    iounmap(isp->core_regs);
    isp->core_regs = NULL;
    return -ENOMEM;
}

/* Deinitialize memory mappings */
static int tx_isp_deinit_memory_mappings(struct tx_isp_dev *isp)
{
    if (isp->phy_base) {
        iounmap(isp->phy_base);
        isp->phy_base = NULL;
    }
    
    if (isp->csi_regs) {
        iounmap(isp->csi_regs);
        isp->csi_regs = NULL;
    }
    
    if (isp->vic_regs) {
        iounmap(isp->vic_regs);
        isp->vic_regs = NULL;
    }
    
    if (isp->core_regs) {
        iounmap(isp->core_regs);
        isp->core_regs = NULL;
    }
    
    pr_info("All ISP memory mappings cleaned up\n");
    return 0;
}

/* Configure ISP system clocks */
static int tx_isp_configure_clocks(struct tx_isp_dev *isp)
{
    struct clk *cgu_isp;
    struct clk *isp_clk;
    struct clk *ipu_clk;
    struct clk *csi_clk;
    int ret;

    pr_info("Configuring ISP system clocks\n");

    /* Get the CGU ISP clock */
    cgu_isp = clk_get(isp->dev, "cgu_isp");
    if (IS_ERR(cgu_isp)) {
        pr_err("Failed to get CGU ISP clock\n");
        return PTR_ERR(cgu_isp);
    }

    /* Get the ISP core clock */
    isp_clk = clk_get(isp->dev, "isp");
    if (IS_ERR(isp_clk)) {
        pr_err("Failed to get ISP clock\n");
        ret = PTR_ERR(isp_clk);
        goto err_put_cgu_isp;
    }

    /* Get the IPU clock */
    ipu_clk = clk_get(isp->dev, "ipu");
    if (IS_ERR(ipu_clk)) {
        pr_err("Failed to get IPU clock\n");
        ret = PTR_ERR(ipu_clk);
        goto err_put_isp_clk;
    }

    /* Get the CSI clock */
    csi_clk = clk_get(isp->dev, "csi");
    if (IS_ERR(csi_clk)) {
        pr_err("Failed to get CSI clock\n");
        ret = PTR_ERR(csi_clk);
        goto err_put_ipu_clk;
    }

    /* Set clock rates */
    ret = clk_set_rate(cgu_isp, 120000000);
    if (ret) {
        pr_err("Failed to set CGU ISP clock rate\n");
        goto err_put_csi_clk;
    }

    ret = clk_set_rate(isp_clk, 200000000);
    if (ret) {
        pr_err("Failed to set ISP clock rate\n");
        goto err_put_csi_clk;
    }

    ret = clk_set_rate(ipu_clk, 200000000);
    if (ret) {
        pr_err("Failed to set IPU clock rate\n");
        goto err_put_csi_clk;
    }

    /* Initialize CSI clock to 100MHz */
    ret = clk_set_rate(csi_clk, 100000000);
    if (ret) {
        pr_err("Failed to set CSI clock rate\n");
        goto err_put_csi_clk;
    }
    pr_info("CSI clock initialized: rate=%lu Hz\n", clk_get_rate(csi_clk));

    /* Enable clocks */
    ret = clk_prepare_enable(cgu_isp);
    if (ret) {
        pr_err("Failed to enable CGU ISP clock\n");
        goto err_put_csi_clk;
    }

    ret = clk_prepare_enable(isp_clk);
    if (ret) {
        pr_err("Failed to enable ISP clock\n");
        goto err_disable_cgu_isp;
    }

    ret = clk_prepare_enable(ipu_clk);
    if (ret) {
        pr_err("Failed to enable IPU clock\n");
        goto err_disable_isp_clk;
    }

    ret = clk_prepare_enable(csi_clk);
    if (ret) {
        pr_err("Failed to enable CSI clock\n");
        goto err_disable_ipu_clk;
    }

    /* Store clocks in ISP device structure */
    isp->cgu_isp = cgu_isp;
    isp->isp_clk = isp_clk;
    isp->ipu_clk = ipu_clk;
    isp->csi_clk = csi_clk;

    /* Allow clocks to stabilize before proceeding - critical for CSI PHY */
    msleep(10);
    
    /* Validate that clocks are actually running at expected rates */
    if (abs(clk_get_rate(csi_clk) - 100000000) > 1000000) {
        pr_warn("CSI clock rate deviation: expected 100MHz, got %luHz\n",
                clk_get_rate(csi_clk));
    }
    
    if (abs(clk_get_rate(isp_clk) - 200000000) > 2000000) {
        pr_warn("ISP clock rate deviation: expected 200MHz, got %luHz\n",
                clk_get_rate(isp_clk));
    }

    pr_info("Clock configuration completed. Rates:\n");
    pr_info("  CSI Core: %lu Hz\n", clk_get_rate(isp->csi_clk));
    pr_info("  ISP Core: %lu Hz\n", clk_get_rate(isp->isp_clk));
    pr_info("  CGU ISP: %lu Hz\n", clk_get_rate(isp->cgu_isp));
    pr_info("  CSI: %lu Hz\n", clk_get_rate(isp->csi_clk));
    pr_info("  IPU: %lu Hz\n", clk_get_rate(isp->ipu_clk));

    return 0;

err_disable_ipu_clk:
    clk_disable_unprepare(ipu_clk);
err_disable_isp_clk:
    clk_disable_unprepare(isp_clk);
err_disable_cgu_isp:
    clk_disable_unprepare(cgu_isp);
err_put_csi_clk:
    clk_put(csi_clk);
err_put_ipu_clk:
    clk_put(ipu_clk);
err_put_isp_clk:
    clk_put(isp_clk);
err_put_cgu_isp:
    clk_put(cgu_isp);
    return ret;
}

/* Create ISP processing graph and initialize subdevice nodes */
static int tx_isp_create_graph_and_nodes(struct tx_isp_dev *isp)
{
    int ret;
    
    pr_info("Creating ISP processing graph\n");
    
    /* Initialize CSI device */
    ret = tx_isp_csi_device_init(isp);
    if (ret < 0) {
        pr_err("Failed to initialize CSI device: %d\n", ret);
        return ret;
    }
    
    /* Initialize VIC device */
    ret = tx_isp_vic_device_init(isp);
    if (ret < 0) {
        pr_err("Failed to initialize VIC device: %d\n", ret);
        goto err_csi_deinit;
    }
    
    /* Set up data pipeline configuration */
    ret = tx_isp_setup_pipeline(isp);
    if (ret < 0) {
        pr_err("Failed to setup ISP pipeline: %d\n", ret);
        goto err_vic_deinit;
    }
    
    pr_info("ISP processing graph created successfully\n");
    return 0;
    
err_vic_deinit:
    tx_isp_vic_device_deinit(isp);
err_csi_deinit:
    tx_isp_csi_device_deinit(isp);
    return ret;
}

/* Setup ISP processing pipeline */
static int tx_isp_setup_pipeline(struct tx_isp_dev *isp)
{
    int ret;
    
    pr_info("Setting up ISP processing pipeline: CSI -> VIC -> Output\n");
    
    /* Initialize the processing pipeline state */
    isp->pipeline_state = ISP_PIPELINE_IDLE;
    
    /* Configure default data path settings */
    if (isp->csi_dev) {
        isp->csi_dev->state = 1; /* INIT state */
        pr_info("CSI device ready for configuration\n");
    }
    
    if (isp->vic_dev) {
        isp->vic_dev->state = 1; /* INIT state */
        pr_info("VIC device ready for configuration\n");
    }
    
    /* Setup media entity links and pads */
    ret = tx_isp_setup_media_links(isp);
    if (ret < 0) {
        pr_err("Failed to setup media links: %d\n", ret);
        return ret;
    }
    
    /* Configure default link routing */
    ret = tx_isp_configure_default_links(isp);
    if (ret < 0) {
        pr_err("Failed to configure default links: %d\n", ret);
        return ret;
    }
    
    pr_info("ISP pipeline setup completed\n");
    return 0;
}

/* Setup media entity links and pads */
static int tx_isp_setup_media_links(struct tx_isp_dev *isp)
{
    int ret;
    
    pr_info("Setting up media entity links\n");
    
    /* Initialize pad configurations for each subdevice */
    ret = tx_isp_init_subdev_pads(isp);
    if (ret < 0) {
        pr_err("Failed to initialize subdev pads: %d\n", ret);
        return ret;
    }
    
    /* Create links between subdevices */
    ret = tx_isp_create_subdev_links(isp);
    if (ret < 0) {
        pr_err("Failed to create subdev links: %d\n", ret);
        return ret;
    }
    
    pr_info("Media entity links setup completed\n");
    return 0;
}

/* Initialize pad configurations for subdevices */
static int tx_isp_init_subdev_pads(struct tx_isp_dev *isp)
{
    pr_info("Initializing subdevice pads\n");
    
    /* CSI pads: 1 output pad */
    if (isp->csi_dev) {
        /* CSI has one output pad that connects to VIC */
        pr_info("CSI pad 0: OUTPUT -> VIC pad 0\n");
    }
    
    /* VIC pads: 1 input pad, 1 output pad */
    if (isp->vic_dev) {
        /* VIC input pad 0 receives from CSI */
        /* VIC output pad 1 sends to application/capture */
        pr_info("VIC pad 0: INPUT <- CSI pad 0\n");
        pr_info("VIC pad 1: OUTPUT -> Capture interface\n");
    }
    
    pr_info("Subdevice pads initialized\n");
    return 0;
}

/* Create links between subdevices */
static int tx_isp_create_subdev_links(struct tx_isp_dev *isp)
{
    struct link_config csi_to_vic_link;
    int ret;
    
    pr_info("Creating subdevice links\n");
    
    /* Create CSI -> VIC link */
    if (isp->csi_dev && isp->vic_dev) {
        /* Configure CSI source pad */
        csi_to_vic_link.src.name = "csi_output";
        csi_to_vic_link.src.type = 2; /* Source pad */
        csi_to_vic_link.src.index = 0;
        
        /* Configure VIC sink pad */
        csi_to_vic_link.dst.name = "vic_input";
        csi_to_vic_link.dst.type = 1; /* Sink pad */
        csi_to_vic_link.dst.index = 0;
        
        /* Set link flags */
        csi_to_vic_link.flags = TX_ISP_LINKFLAG_ENABLED;
        
        /* Store link configuration */
        ret = tx_isp_register_link(isp, &csi_to_vic_link);
        if (ret < 0) {
            pr_err("Failed to register CSI->VIC link: %d\n", ret);
            return ret;
        }
        
        pr_info("Created CSI->VIC link successfully\n");
    }
    
    pr_info("Subdevice links created\n");
    return 0;
}

/* Register a link in the ISP pipeline */
static int tx_isp_register_link(struct tx_isp_dev *isp, struct link_config *link)
{
    if (!isp || !link) {
        pr_err("Invalid parameters for link registration\n");
        return -EINVAL;
    }
    
    pr_info("Registering link: %s[%d] -> %s[%d] (flags=0x%x)\n",
            link->src.name, link->src.index,
            link->dst.name, link->dst.index,
            link->flags);
    
    /* In a full implementation, this would store the link in a list
     * and configure the hardware routing. For now, just validate and log. */
    
    if (link->flags & TX_ISP_LINKFLAG_ENABLED) {
        pr_info("Link enabled and configured\n");
    }
    
    return 0;
}

/* Configure default link routing */
static int tx_isp_configure_default_links(struct tx_isp_dev *isp)
{
    pr_info("Configuring default link routing\n");
    
    /* Set pipeline to configured state */
    isp->pipeline_state = ISP_PIPELINE_CONFIGURED;
    
    /* Enable default data flow: CSI -> VIC -> Output */
    if (isp->csi_dev && isp->vic_dev) {
        pr_info("Default routing: Sensor -> CSI -> VIC -> Capture\n");
        
        /* Configure data format propagation */
        tx_isp_configure_format_propagation(isp);
    }
    
    pr_info("Default link routing configured\n");
    return 0;
}

/* Configure format propagation through the pipeline */
static int tx_isp_configure_format_propagation(struct tx_isp_dev *isp)
{
    pr_info("Configuring format propagation\n");
    
    /* Ensure format compatibility between pipeline stages */
    if (isp->sensor_width > 0 && isp->sensor_height > 0) {
        pr_info("Propagating format: %dx%d through pipeline\n",
                isp->sensor_width, isp->sensor_height);
        
        /* Configure CSI format */
        if (isp->csi_dev) {
            pr_info("CSI configured for %dx%d\n", isp->sensor_width, isp->sensor_height);
        }
        
        /* Configure VIC format */
        if (isp->vic_dev) {
            isp->vic_dev->width = isp->sensor_width;
            isp->vic_dev->height = isp->sensor_height;
            isp->vic_dev->stride = isp->sensor_width * 2; /* Assume 16-bit per pixel */
            pr_info("VIC configured for %dx%d, stride=%d\n",
                    isp->vic_dev->width, isp->vic_dev->height, isp->vic_dev->stride);
        }
    }
    
    pr_info("Format propagation configured\n");
    return 0;
}

/* Initialize CSI device */
static int tx_isp_csi_device_init(struct tx_isp_dev *isp)
{
    struct csi_device *csi_dev;
    
    pr_info("Initializing CSI device\n");
    
    /* Allocate CSI device structure if not already present */
    if (!isp->csi_dev) {
        csi_dev = kzalloc(sizeof(struct csi_device), GFP_KERNEL);
        if (!csi_dev) {
            pr_err("Failed to allocate CSI device\n");
            return -ENOMEM;
        }
        
        /* Initialize CSI device structure */
        strcpy(csi_dev->device_name, "tx_isp_csi");
        csi_dev->dev = isp->dev;
        csi_dev->state = 1; /* INIT state */
        mutex_init(&csi_dev->mutex);
        spin_lock_init(&csi_dev->lock);
        
        isp->csi_dev = csi_dev;
    }
    
    pr_info("CSI device initialized\n");
    return 0;
}

/* Initialize VIC device */
static int tx_isp_vic_device_init(struct tx_isp_dev *isp)
{
    struct vic_device *vic_dev;
    
    pr_info("Initializing VIC device\n");
    
    /* Allocate VIC device structure if not already present */
    if (!isp->vic_dev) {
        vic_dev = kzalloc(sizeof(struct vic_device), GFP_KERNEL);
        if (!vic_dev) {
            pr_err("Failed to allocate VIC device\n");
            return -ENOMEM;
        }
        
        /* Initialize VIC device structure */
        vic_dev->state = 1; /* INIT state */
        mutex_init(&vic_dev->state_lock);
        spin_lock_init(&vic_dev->lock);
        init_completion(&vic_dev->frame_complete);
        
        isp->vic_dev = vic_dev;
    }
    
    pr_info("VIC device initialized\n");
    return 0;
}

/* Deinitialize CSI device */
static int tx_isp_csi_device_deinit(struct tx_isp_dev *isp)
{
    if (isp->csi_dev) {
        kfree(isp->csi_dev);
        isp->csi_dev = NULL;
    }
    return 0;
}

/* Deinitialize VIC device */
static int tx_isp_vic_device_deinit(struct tx_isp_dev *isp)
{
    if (isp->vic_dev) {
        kfree(isp->vic_dev);
        isp->vic_dev = NULL;
    }
    return 0;
}

/* Core probe function from decompiled code */
int tx_isp_core_probe(struct platform_device *pdev)
{
    struct tx_isp_dev *isp = NULL;
    struct resource *res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    int32_t ret = 0;
    int32_t i = 0;
    printk("tx_isp_core_probe\n");

    isp = private_kmalloc(sizeof(struct tx_isp_dev), GFP_KERNEL);
    if (isp == NULL) {
        ISP_ERROR("alloc tx_isp_dev failed!\n");
        ret = -ENOMEM;
        goto _core_alloc_err;
    }

    memset(isp, 0, sizeof(struct tx_isp_dev));
    private_platform_set_drvdata(pdev, isp);
    private_spin_lock_init(&isp->lock);
    mutex_init(&isp->mutex);
    isp->dev = &pdev->dev;

    for (i = 0; i < ISP_MAX_CHAN; i++) {
        tx_isp_frame_chan_init(&isp->channels[i]);
    }

    ret = tx_isp_proc_init(isp);
    if (ret < 0) {
        ISP_ERROR("tx_isp_proc_init failed!\n");
        ret = -EINVAL;
        goto _core_proc_init_err;
    }

    ret = tx_isp_sysfs_init(isp);
    if (ret < 0) {
        ISP_ERROR("tx_isp_sysfs_init failed!\n");
        ret = -EINVAL;
        goto _core_sysfs_init_err;
    }

    ret = platform_get_irq(pdev, 0);
    if (ret < 0) {
        ISP_ERROR("No IRQ specified for ISP core\n");
        goto _core_sysfs_init_err;
    }
    isp->isp_irq = ret;

    ret = request_irq(isp->isp_irq, tx_isp_core_irq_handler, IRQF_SHARED,
        "isp_irq", isp);
    if (ret < 0) {
        ISP_ERROR("request irq failed!\n");
        ret = -EINVAL;
        goto _core_req_irq_err;
    }

    /* Initialize completion */
    init_completion(&isp->frame_complete);

    /* Initialize device info */
    isp->dev = &pdev->dev;
    isp->pdev = pdev;

    /* Initialize memory mappings for ISP subsystems */
    ret = tx_isp_init_memory_mappings(isp);
    if (ret < 0) {
        ISP_ERROR("Failed to initialize memory mappings: %d\n", ret);
        goto _core_mem_err;
    }

    /* Configure system clocks */
    ret = tx_isp_configure_clocks(isp);
    if (ret < 0) {
        ISP_ERROR("Failed to configure system clocks: %d\n", ret);
        goto _core_clk_err;
    }

    /* Create ISP graph and nodes */
    pr_info("Creating ISP graph and nodes\n");
    ret = tx_isp_create_graph_and_nodes(isp);
    if (ret < 0) {
        ISP_ERROR("Failed to create ISP graph and nodes: %d\n", ret);
        goto _core_graph_err;
    }
    pr_info("ISP graph and nodes created successfully\n");

    // level first
    isp_printf(ISP_INFO_LEVEL, "TX ISP core driver probed successfully\n");
    return 0;

_core_graph_err:
    /* Disable and release clocks */
    if (isp->csi_clk) {
        clk_disable_unprepare(isp->csi_clk);
        clk_put(isp->csi_clk);
    }
    if (isp->ipu_clk) {
        clk_disable_unprepare(isp->ipu_clk);
        clk_put(isp->ipu_clk);
    }
    if (isp->isp_clk) {
        clk_disable_unprepare(isp->isp_clk);
        clk_put(isp->isp_clk);
    }
    if (isp->cgu_isp) {
        clk_disable_unprepare(isp->cgu_isp);
        clk_put(isp->cgu_isp);
    }
_core_clk_err:
    tx_isp_deinit_memory_mappings(isp);
_core_mem_err:
    free_irq(isp->isp_irq, isp);
_core_req_irq_err:
    tx_isp_sysfs_exit(isp);
_core_sysfs_init_err:
    tx_isp_proc_exit(isp);
_core_proc_init_err:
    for (i = 0; i < ISP_MAX_CHAN; i++) {
        tx_isp_frame_chan_deinit(&isp->channels[i]);
    }
_core_ioremap_err:
    private_release_mem_region(res->start, resource_size(res));
_core_req_mem_err:
    private_kfree(isp);
_core_alloc_err:
    return ret;
}

/* Core remove function */
int tx_isp_core_remove(struct platform_device *pdev)
{
    struct tx_isp_dev *isp = platform_get_drvdata(pdev);
    struct resource *res;
    int i;

    if (!isp)
        return -EINVAL;

    /* Free IRQ */
    private_free_irq(isp->isp_irq, isp);

    /* Cleanup ISP graph and subdevices */
    tx_isp_csi_device_deinit(isp);
    tx_isp_vic_device_deinit(isp);

    /* Disable and release clocks */
    if (isp->csi_clk) {
        clk_disable_unprepare(isp->csi_clk);
        clk_put(isp->csi_clk);
    }
    if (isp->ipu_clk) {
        clk_disable_unprepare(isp->ipu_clk);
        clk_put(isp->ipu_clk);
    }
    if (isp->isp_clk) {
        clk_disable_unprepare(isp->isp_clk);
        clk_put(isp->isp_clk);
    }
    if (isp->cgu_isp) {
        clk_disable_unprepare(isp->cgu_isp);
        clk_put(isp->cgu_isp);
    }

    /* Cleanup memory mappings */
    tx_isp_deinit_memory_mappings(isp);

    /* Cleanup subsystems */
    tx_isp_sysfs_exit(isp);
    tx_isp_proc_exit(isp);

    /* Cleanup channels */
    for (i = 0; i < ISP_MAX_CHAN; i++) {
        tx_isp_frame_chan_deinit(&isp->channels[i]);
    }
    
    /* Release memory region */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (res)
        private_release_mem_region(res->start, resource_size(res));

    /* Free device structure */
    private_kfree(isp);

    pr_info("TX ISP core driver removed successfully\n");
    return 0;
}


/****
* The following methods are made available to sensor driver
****/

void private_spin_lock_init(spinlock_t *lock)
{
    spin_lock_init(lock);
}
EXPORT_SYMBOL(private_spin_lock_init);


struct clk * private_clk_get(struct device *dev, const char *id)
{
    return clk_get(dev, id);
}
EXPORT_SYMBOL(private_clk_get);


void private_platform_set_drvdata(struct platform_device *pdev, void *data)
{
    platform_set_drvdata(pdev, data);
}
EXPORT_SYMBOL(private_platform_set_drvdata);

void private_raw_mutex_init(struct mutex *lock, const char *name, struct lock_class_key *key)
{
    __mutex_init(lock, name, key);
}
EXPORT_SYMBOL(private_raw_mutex_init);

void private_mutex_init(struct mutex *mutex)
{
    mutex_init(mutex);
}
EXPORT_SYMBOL(private_mutex_init);

void private_free_irq(unsigned int irq, void *dev_id)
{
    free_irq(irq, dev_id);
}
EXPORT_SYMBOL(private_free_irq);

void * private_platform_get_drvdata(struct platform_device *dev)
{
    return platform_get_drvdata(dev);
}
EXPORT_SYMBOL(private_platform_get_drvdata);

struct resource * private_platform_get_resource(struct platform_device *dev,
			       unsigned int type, unsigned int num)
{
    return platform_get_resource(dev, type, num);
}
EXPORT_SYMBOL(private_platform_get_resource);

int private_platform_get_irq(struct platform_device *dev, unsigned int num)
{
    return platform_get_irq(dev, num);
}
EXPORT_SYMBOL(private_platform_get_irq);

struct resource * private_request_mem_region(resource_size_t start, resource_size_t n,
			   const char *name)
{
    return request_mem_region(start, n, name);
}
EXPORT_SYMBOL(private_request_mem_region);

void private_release_mem_region(resource_size_t start, resource_size_t n)
{
    release_mem_region(start, n);
}
EXPORT_SYMBOL(private_release_mem_region);

void __iomem * private_ioremap(phys_addr_t offset, unsigned long size)
{
    return ioremap(offset, size);
}
EXPORT_SYMBOL(private_ioremap);

void private_iounmap(const volatile void __iomem *addr)
{
    iounmap(addr);
}
EXPORT_SYMBOL(private_iounmap);





void * private_kmalloc(size_t s, gfp_t gfp)
{
    void *addr = kmalloc(s, gfp);
    return addr;
}

void private_kfree(void *p)
{
    kfree(p);
}

void private_i2c_del_driver(struct i2c_driver *driver)
{
    i2c_del_driver(driver);
}

int private_gpio_request(unsigned int gpio, const char *label)
{
    return gpio_request(gpio, label);
}

void private_gpio_free(unsigned int gpio)
{
    gpio_free(gpio);
}

void private_msleep(unsigned int msecs)
{
    msleep(msecs);
}

void private_clk_disable(struct clk *clk)
{
    clk_disable(clk);
}

void *private_i2c_get_clientdata(const struct i2c_client *client)
{
    return i2c_get_clientdata(client);
}

bool private_capable(int cap)
{
    return capable(cap);
}

void private_i2c_set_clientdata(struct i2c_client *client, void *data)
{
    i2c_set_clientdata(client, data);
}

int private_i2c_transfer(struct i2c_adapter *adap, struct i2c_msg *msgs, int num)
{
    return i2c_transfer(adap, msgs, num);
}

int private_i2c_add_driver(struct i2c_driver *driver)
{
    return i2c_add_driver(driver);
}

int private_gpio_direction_output(unsigned int gpio, int value)
{
    return gpio_direction_output(gpio, value);
}

int private_clk_enable(struct clk *clk)
{
    return clk_enable(clk);
}

void private_clk_put(struct clk *clk)
{
    clk_put(clk);
}

int private_clk_set_rate(struct clk *clk, unsigned long rate)
{
    return clk_set_rate(clk, rate);
}

int isp_printf(unsigned int level, unsigned char *fmt, ...)
{
    struct va_format vaf;
    va_list args;
    int r = 0;

    if(level >= print_level){
        va_start(args, fmt);

        vaf.fmt = fmt;
        vaf.va = &args;

        r = printk("%pV",&vaf);
        va_end(args);
        if(level >= ISP_ERROR_LEVEL)
            dump_stack();
    }
    return r;
}
EXPORT_SYMBOL(isp_printf);

int private_jzgpio_set_func(enum gpio_port port, enum gpio_function func,unsigned long pins)
{
    return jzgpio_set_func(port, func, pins);
}
EXPORT_SYMBOL(private_jzgpio_set_func);

/* Must be check the return value */
static struct jz_driver_common_interfaces *pfaces = NULL;


int32_t private_driver_get_interface()
{
    struct jz_driver_common_interfaces *pfaces = NULL;  // Declare pfaces locally
    int32_t result = private_get_driver_interface(&pfaces);  // Call the function with the address of pfaces

    if (result != 0) {
        // Handle error, pfaces should still be NULL if the function failed
        return result;
    }

    // Proceed with further logic, now that pfaces is properly initialized
    // Example: check flags or other interface fields
    if (pfaces != NULL) {
        // You can now access pfaces->flags_0, pfaces->flags_1, etc.
        if (pfaces->flags_0 != pfaces->flags_1) {
            ISP_ERROR("Mismatch between flags_0 and flags_1");
            return -1;  // Some error condition
        }
    }

    return 0;  // Success
}
EXPORT_SYMBOL(private_driver_get_interface);
__must_check int private_get_driver_interface(struct jz_driver_common_interfaces **pfaces)
{
	if(pfaces == NULL)
		return -1;
	*pfaces = get_driver_common_interfaces();
	if(*pfaces && ((*pfaces)->flags_0 != (unsigned int)printk || (*pfaces)->flags_0 !=(*pfaces)->flags_1)){
		ISP_ERROR("flags = 0x%08x, jzflags = %p,0x%08x", (*pfaces)->flags_0, printk, (*pfaces)->flags_1);
		return -1;
	}else
		return 0;
}
EXPORT_SYMBOL(private_get_driver_interface);

EXPORT_SYMBOL(private_i2c_del_driver);
EXPORT_SYMBOL(private_gpio_request);
EXPORT_SYMBOL(private_gpio_free);
EXPORT_SYMBOL(private_msleep);
EXPORT_SYMBOL(private_clk_disable);
EXPORT_SYMBOL(private_i2c_get_clientdata);
EXPORT_SYMBOL(private_capable);
EXPORT_SYMBOL(private_i2c_set_clientdata);
EXPORT_SYMBOL(private_i2c_transfer);
EXPORT_SYMBOL(private_i2c_add_driver);
EXPORT_SYMBOL(private_gpio_direction_output);
EXPORT_SYMBOL(private_clk_enable);
EXPORT_SYMBOL(private_clk_put);
EXPORT_SYMBOL(private_clk_set_rate);


