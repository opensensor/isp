#ifndef ISP_DRIVER_COMMON_H
#define ISP_DRIVER_COMMON_H

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/netlink.h>
#include <linux/proc_fs.h>
#include <linux/miscdevice.h>
#include <linux/kthread.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>
#include <linux/completion.h>
#include <linux/seq_file.h>
#include <linux/err.h>
#include <linux/sched.h>

// Forward declarations for kernel functions
extern void msleep(unsigned int msecs);
extern int platform_driver_register(struct platform_driver *driver);
extern void platform_driver_unregister(struct platform_driver *driver);
extern void* platform_get_drvdata(struct platform_device *pdev);
extern int platform_set_drvdata(struct platform_device *pdev, void *data);
extern struct platform_device* platform_device_register(struct platform_device *pdev);
extern void platform_device_unregister(struct platform_device *pdev);
extern void* platform_get_resource(struct platform_device *pdev, unsigned int type, unsigned int num);
extern void dev_set_drvdata(struct device *dev, void *data);
extern void* dev_get_drvdata(struct device *dev);
extern int platform_get_irq(struct platform_device *pdev, unsigned int index);
extern void* i2c_get_adapter(int adap_id);
extern int i2c_put_adapter(struct i2c_adapter *adap);
extern int i2c_transfer(struct i2c_adapter *adap, struct i2c_msg *msgs, int num);
extern int i2c_register_driver(struct i2c_driver *driver);
extern void i2c_del_driver(struct i2c_driver *driver);
extern void gpio_request(unsigned int gpio, const char *label);
extern void gpio_free(unsigned int gpio);
extern int gpio_direction_output(unsigned int gpio, int value);
extern int gpio_direction_input(unsigned int gpio);
extern void gpio_set_debounce(unsigned int gpio, unsigned int debounce);
extern int jzgpio_set_func(unsigned int gpio, unsigned int func);
extern int jzgpio_ctrl_pull(unsigned int gpio, unsigned int pull);
extern int try_module_get(struct module *mod);
extern void module_put(struct module *mod);
extern int capable(int capability);
extern void* get_isp_priv_mem(void);
extern int misc_register(struct miscdevice *misc);
extern void misc_deregister(struct miscdevice *misc);
extern struct proc_dir_entry* proc_create_data(const char *name, umode_t mode, struct proc_dir_entry *parent, const struct proc_ops *proc_ops, void *data);
extern void proc_remove(struct proc_dir_entry *pde);
extern int seq_read(struct file *file, struct seq_file *m, void *v);
extern loff_t seq_lseek(struct file *file, loff_t offset, int whence);
extern int single_open_size(struct file *file, int (*show)(struct seq_file *, void *), void *data);
extern int single_release(struct inode *inode, struct file *file);
extern void init_completion(struct completion *x);
extern void complete(struct completion *x);
extern int wait_for_completion_interruptible(struct completion *x);
extern int wait_event_interruptible(struct wait_queue_head *wq, int condition);
extern void wake_up_all(struct wait_queue_head *wq);
extern void wake_up(struct wait_queue_head *wq);
extern void init_waitqueue_head(struct wait_queue_head *q);
extern unsigned long wait_for_completion_timeout(struct completion *x, unsigned long timeout);
extern struct netlink_sock* netlink_unicast(struct sock *sk, struct sk_buff *skb, u32 pid, int flags);
extern int sock_release(struct socket *sock);
extern void* filp_open(const char *path, int flags, umode_t mode);
extern int filp_close(struct file *filp, fl_owner_t id);
extern struct fs_struct* private_get_fs(void);
extern void private_set_fs(struct fs_struct *fs);
extern ssize_t vfs_read(struct file *file, char __user *buf, size_t count, loff_t *pos);
extern ssize_t vfs_write(struct file *file, const char __user *buf, size_t count, loff_t *pos);
extern loff_t vfs_llseek(struct file *file, loff_t offset, int whence);
extern void dma_cache_sync(struct device *dev, dma_addr_t handle, size_t size, enum dma_data_direction dir);

struct jz_driver_common_interfaces {
    unsigned int flags_0;
    unsigned int flags_1;

    /* platform interface */
    int (*priv_platform_driver_register)(struct platform_driver *driver);
    void (*priv_platform_driver_unregister)(struct platform_driver *driver);
    void* (*priv_platform_get_drvdata)(struct platform_device *pdev);
    int (*priv_platform_set_drvdata)(struct platform_device *pdev, void *data);
    struct platform_device* (*priv_platform_device_register)(struct platform_device *pdev);
    void (*priv_platform_device_unregister)(struct platform_device *pdev);
    void* (*priv_platform_get_resource)(struct platform_device *pdev, unsigned int type, unsigned int num);
    void (*priv_dev_set_drvdata)(struct device *dev, void *data);
    void* (*priv_dev_get_drvdata)(struct device *dev);
    int (*priv_platform_get_irq)(struct platform_device *pdev, unsigned int index);

    /* i2c interface */
    void* (*priv_i2c_get_adapter)(int adap_id);
    int (*priv_i2c_put_adapter)(struct i2c_adapter *adap);
    int (*priv_i2c_transfer)(struct i2c_adapter *adap, struct i2c_msg *msgs, int num);
    int (*priv_i2c_register_driver)(struct i2c_driver *driver);
    void (*priv_i2c_del_driver)(struct i2c_driver *driver);

    /* gpio interface */
    void (*priv_gpio_request)(unsigned int gpio, const char *label);
    void (*priv_gpio_free)(unsigned int gpio);
    int (*priv_gpio_direction_output)(unsigned int gpio, int value);
    int (*priv_gpio_direction_input)(unsigned int gpio);
    void (*priv_gpio_set_debounce)(unsigned int gpio, unsigned int debounce);
    int (*priv_jzgpio_set_func)(unsigned int gpio, unsigned int func);
    int (*priv_jzgpio_ctrl_pull)(unsigned int gpio, unsigned int pull);

    /* system interface */
    void (*priv_msleep)(unsigned int msecs);
    int (*priv_capable)(int capability);
    unsigned long (*priv_sched_clock)(void);
    int (*priv_try_module_get)(struct module *mod);
    void (*priv_module_put)(struct module *mod);

    /* proc interface */
    int (*priv_proc_create_data)(const char *name, umode_t mode, struct proc_dir_entry *parent, const struct proc_ops *proc_ops, void *data);
    void (*priv_proc_remove)(struct proc_dir_entry *pde);
    int (*priv_seq_read)(struct file *file, struct seq_file *m, void *v);
    loff_t (*priv_seq_lseek)(struct file *file, loff_t offset, int whence);

    /* memory interface */
    void* (*priv_kmalloc)(size_t size, gfp_t flags);
    void (*priv_kfree)(void *ptr);
    ssize_t (*priv_copy_from_user)(void *to, const void __user *from, unsigned long len);
    ssize_t (*priv_copy_to_user)(void __user *to, const void *from, unsigned long len);
};

void *get_driver_common_interfaces(void);

#endif /* ISP_DRIVER_COMMON_H */
