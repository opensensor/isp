#ifndef __TX_ISP_TUNING_H__
#define __TX_ISP_TUNING_H__

#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/err.h>

#include "tx-isp-common.h"

int isp_m0_chardev_ioctl(struct file *file, unsigned int cmd, void __user *arg);
int isp_core_tuning_release(struct tx_isp_dev *dev);
int isp_m0_chardev_release(struct inode *inode, struct file *file);
int isp_m0_chardev_open(struct inode *inode, struct file *file);
int tisp_code_create_tuning_node(void);
#endif //__TX_ISP_TUNING_H__
