#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/i2c.h>

#include <sensor-info.h>

#include "tx-isp.h"
#include "tx-isp-main.h"
#include "tx-isp-sensor.h"

// Private variables
int reset_gpio = GPIO_PA(18);  // Default reset GPIO
int pwdn_gpio = -1;  // Default power down GPIO disabled

struct sensor_info registered_sensors[MAX_SENSORS];
int num_registered_sensors = 0;

extern struct IMPISPDev *ourISPdev;

static const struct i2c_device_id sensor_id[] = {
    { "tx-sensor", 0 },
    { }
};

// Sensor I2C/Register access

/* Helper function to configure sensor via I2C */
int isp_sensor_write_reg(struct i2c_client *client, u16 reg, u8 val)
{
    u8 buf[3];
    struct i2c_msg msg;
    int ret;

    if (!client || !client->adapter) {
        pr_err("Invalid I2C client\n");
        return -EINVAL;
    }

    // Construct i2c message
    buf[0] = (reg >> 8) & 0xFF;  // MSB
    buf[1] = reg & 0xFF;         // LSB
    buf[2] = val;                // Value

    msg.addr = client->addr;
    msg.flags = 0;               // Write
    msg.buf = buf;
    msg.len = 3;                 // 16-bit reg + 8-bit val

    // Try with retry
    ret = i2c_transfer(client->adapter, &msg, 1);
    if (ret < 0) {
        pr_err("i2c write failed reg=0x%04x val=0x%02x ret=%d\n",
               reg, val, ret);
        return ret;
    }

    pr_info("write reg 0x%04x = 0x%02x\n", reg, val);
    return 0;
}

int isp_sensor_read_reg(struct i2c_client *client, u16 reg, u8 *val)
{
    struct i2c_msg msgs[2];
    u8 buf[2];
    int ret;

    if (!client || !client->adapter || !val) {
        pr_err("Invalid parameters\n");
        return -EINVAL;
    }

    buf[0] = (reg >> 8) & 0xFF;
    buf[1] = reg & 0xFF;

    // Write register address
    msgs[0].addr = client->addr;
    msgs[0].flags = 0;
    msgs[0].buf = buf;
    msgs[0].len = 2;

    // Read value
    msgs[1].addr = client->addr;
    msgs[1].flags = I2C_M_RD;
    msgs[1].buf = val;
    msgs[1].len = 1;

    ret = i2c_transfer(client->adapter, msgs, 2);
    if (ret < 0) {
        pr_err("i2c read failed reg=0x%04x ret=%d\n", reg, ret);
        return ret;
    }

    pr_info("read reg 0x%04x = 0x%02x\n", reg, *val);
    return 0;
}


int debug_sensor_registers(struct IMPISPDev *dev)
{
    struct i2c_client *client = dev->sensor_i2c_client;
    u8 val;
    int ret;

    pr_info("SC2336 Register Dump:\n");

    // Read critical registers
    ret = isp_sensor_read_reg(client, 0x0100, &val);
    pr_info("Streaming (0x0100): 0x%02x\n", val);

    ret |= isp_sensor_read_reg(client, 0x3031, &val);
    pr_info("Format control (0x3031): 0x%02x\n", val);

    // Test pattern control - IMPORTANT
    ret |= isp_sensor_read_reg(client, 0x4501, &val);
    pr_info("Test pattern (0x4501): 0x%02x\n", val);

    // Output format & timing
    ret |= isp_sensor_read_reg(client, 0x3108, &val);
    pr_info("Output format (0x3108): 0x%02x\n", val);

    ret |= isp_sensor_read_reg(client, 0x3200, &val);
    pr_info("Output width high: 0x%02x\n", val);

    ret |= isp_sensor_read_reg(client, 0x3201, &val);
    pr_info("Output width low: 0x%02x\n", val);

    return ret;
}


// Sensor setup and configuration
int sc2336_hw_reset(struct IMPISPDev *dev)
{
    if (!dev || dev->reset_gpio < 0)
        return -EINVAL;

    pr_info("Resetting sensor via GPIO %d\n", dev->reset_gpio);

    // Reset sequence
    gpio_direction_output(dev->reset_gpio, 0);
    msleep(20);
    gpio_direction_output(dev->reset_gpio, 1);
    msleep(20);

    return 0;
}

// I2C initialization

int setup_i2c_adapter(struct IMPISPDev *dev)
{
    struct i2c_board_info board_info = {
        .type = "sc2336", // TODO
        .addr = 0x30,  // SC2336 I2C address
    };
    struct i2c_adapter *adapter;
    int ret;

    pr_info("Setting up I2C infrastructure for SC2336...\n");

    adapter = i2c_get_adapter(0);
    if (!adapter) {
        pr_err("Failed to get I2C adapter\n");
        return -ENODEV;
    }

    // Create new I2C device
    dev->sensor_i2c_client = i2c_new_device(adapter, &board_info);
    if (!dev->sensor_i2c_client) {
        pr_err("Failed to create I2C device\n");
        i2c_put_adapter(adapter);
        return -ENODEV;
    }

    i2c_put_adapter(adapter);
    pr_info("I2C sensor initialized: addr=0x%02x adapter=%p\n",
            dev->sensor_i2c_client->addr, dev->sensor_i2c_client->adapter);

    return 0;
}

void cleanup_i2c_adapter(struct IMPISPDev *dev)
{
    if (dev->sensor_i2c_client) {
        i2c_unregister_device(dev->sensor_i2c_client);
        dev->sensor_i2c_client = NULL;
    }
}
struct i2c_client *isp_i2c_new_sensor_device(struct i2c_adapter *adapter,
                                                   struct isp_i2c_board_info *info)
{
    struct i2c_board_info board_info;
    int ret = 0;

    /* Initialize board_info properly */
    memset(&board_info, 0, sizeof(board_info));
    strlcpy(board_info.type, info->type, I2C_NAME_SIZE);
    board_info.addr = info->addr;
    board_info.platform_data = info->platform_data;
    board_info.irq = info->irq;
    board_info.flags = info->flags;

    /* Try to load the sensor module first */
    request_module(info->type);

    /* Create new I2C device */
    struct i2c_client *client = i2c_new_device(adapter, &board_info);
    if (!client) {
        pr_err("Failed to create I2C device for %s\n", info->type);
        return NULL;
    }

    /* Verify the driver bound successfully */
    if (!client->dev.driver) {
        pr_err("No driver bound to %s sensor\n", info->type);
        i2c_unregister_device(client);
        return NULL;
    }

    return client;
}

struct i2c_client *isp_i2c_new_subdev_board(struct i2c_adapter *adapter,
                                            struct isp_i2c_board_info *info,
                                            void *client_data)
{
    struct i2c_client *client;
    int ret;

    if (!adapter || !info) {
        pr_err("Invalid adapter or board info\n");
        return NULL;
    }

    /* Create the I2C device */
    client = isp_i2c_new_sensor_device(adapter, info);
    if (!client) {
        pr_err("Failed to create sensor I2C device\n");
        return NULL;
    }

    /* Store any private data */
    i2c_set_clientdata(client, client_data);

    /* Perform basic sensor validation by reading a chip ID register */
    struct i2c_msg msgs[2];
    u8 reg_addr = 0x30; /* Chip ID register - adjust based on your sensor */
    u16 chip_id = 0;  // Make sure to zero-initialize

    // Prepare I2C messages to read the chip ID
    msgs[0].addr = client->addr;
    msgs[0].flags = 0;
    msgs[0].len = 1;
    msgs[0].buf = &reg_addr;

    // Read 2 bytes for the 16-bit chip ID
    msgs[1].addr = client->addr;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = 2; // Correct length to read both bytes
    msgs[1].buf = (u8 *)&chip_id;


    /* Perform the I2C transfer */
    ret = i2c_transfer(adapter, msgs, 2);
    if (ret != 2) {
        pr_err("Failed to read sensor chip ID from address 0x%02x\n", client->addr);
        i2c_unregister_device(client);
        return NULL;
    }

    /* Handle endianness if necessary */
    chip_id = be16_to_cpu(chip_id); // Convert from big-endian to CPU endianness if needed

    /* Print the chip ID */
    pr_info("Sensor %s initialized successfully at address 0x%02x with chip ID 0x%04x\n",
            info->type, info->addr, chip_id);

    /* Return the initialized client */
    return client;
}

// GPIO handling
int init_gpio_config(struct IMPISPDev *dev)
{
    int ret;

    dev->reset_gpio = GPIO_PA(18);
    ret = devm_gpio_request_one(dev->dev, dev->reset_gpio,
                               GPIOF_OUT_INIT_HIGH, "sc2336_reset");
    if (ret) {
        dev_err(dev->dev, "Failed to request reset GPIO: %d\n", ret);
        return ret;
    }

    dev->pwdn_gpio = -1;  // No power down by default
    dev_info(dev->dev, "GPIO setup: reset=%d pwdn=%d\n",
             dev->reset_gpio, dev->pwdn_gpio);

    return 0;
}


