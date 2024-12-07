#ifndef TX_ISP_SENSOR_H
#define TX_ISP_SENSOR_H

/* Include kernel headers in proper order */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/cdev.h>    // For character device support
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/videodev2.h>

#include "tx-isp-main.h"

int isp_sensor_write_reg(struct i2c_client *client, u16 reg, u8 val);
int isp_sensor_read_reg(struct i2c_client *client, u16 reg, u8 *val);
int debug_sensor_registers(struct IMPISPDev *dev);

// Sensor setup and configuration
int sensor_hw_reset(struct IMPISPDev *dev);

// I2C initialization
int setup_i2c_adapter(struct IMPISPDev *dev);
void cleanup_i2c_adapter(struct IMPISPDev *dev);
static struct i2c_client *isp_i2c_new_sensor_device(struct i2c_adapter *adapter,
                                                   struct isp_i2c_board_info *info);
static struct i2c_client *isp_i2c_new_subdev_board(struct i2c_adapter *adapter,
                                                  struct isp_i2c_board_info *info,
                                                  void *client_data);

// GPIO handling
int init_gpio_config(struct IMPISPDev *dev);


#endif //TX_ISP_SENSOR_H
