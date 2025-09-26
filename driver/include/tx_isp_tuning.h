#ifndef __TX_ISP_TUNING_H__
#define __TX_ISP_TUNING_H__

#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/err.h>

#include "tx-isp-common.h"

int isp_core_tunning_unlocked_ioctl(struct file *file, unsigned int cmd, void __user *arg);
int isp_core_tuning_release(struct tx_isp_dev *dev);
int isp_m0_chardev_release(struct inode *inode, struct file *file);
int tisp_code_tuning_open(struct inode *inode, struct file *file);
int tisp_code_create_tuning_node(void);
int tisp_code_destroy_tuning_node(void);

/* DMSC helpers (BN MCP naming style) */
void tisp_dmsc_set_cfa_from_mbus(u32 mbus_code, int hflip, int vflip);

/* ISP event callback function array - exported for external SDK */
extern void (*isp_event_func_cb[32])(void);

#endif //__TX_ISP_TUNING_H__
