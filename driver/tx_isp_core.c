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


/* Core ISP interrupt handler */
irqreturn_t tx_isp_core_irq_handler(int irq, void *dev_id)
{
    struct tx_isp_dev *isp = dev_id;
    u32 status;
    unsigned long flags;

    spin_lock_irqsave(&isp->irq_lock, flags);
    
    /* Read and clear interrupt status */
    status = isp_read32(ISP_INT_STATUS);
    isp_write32(ISP_INT_STATUS, status);

    if (status & INT_FRAME_DONE) {
        isp->frame_count++;
        complete(&isp->frame_complete);
    }

    if (status & INT_ERROR) {
        ISP_ERROR("ISP error interrupt received\n");
    }

    spin_unlock_irqrestore(&isp->irq_lock, flags);
    return IRQ_HANDLED;
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

    // level first
    isp_printf(ISP_INFO_LEVEL, "TX ISP core driver probed successfully\n");
    return 0;

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


