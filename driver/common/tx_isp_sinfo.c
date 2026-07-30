/*
 * Shared Thingino sensor registry ABI.
 *
 * This implementation is compiled through a small per-SoC adapter translation
 * unit. The adapter provides the Linux module, I2C, procfs, seq_file, mutex,
 * and errno headers, plus a static `tx_isp_sinfo_config` describing its sensor
 * ABI and any required lifecycle callbacks.
 */

#include "../include/tx_isp/tx_isp_sinfo.h"

#define TX_ISP_SINFO_MAX_SENSORS 4

enum tx_isp_sinfo_key {
	TX_ISP_SINFO_NAME,
	TX_ISP_SINFO_CHIP_ID,
	TX_ISP_SINFO_I2C_ADDR,
	TX_ISP_SINFO_I2C_ADAPTER,
	TX_ISP_SINFO_WIDTH,
	TX_ISP_SINFO_HEIGHT,
	TX_ISP_SINFO_FPS,
	TX_ISP_SINFO_STATUS,
	TX_ISP_SINFO_MIN_FPS,
	TX_ISP_SINFO_MAX_FPS,
	TX_ISP_SINFO_MCLK,
	TX_ISP_SINFO_BOOT,
	TX_ISP_SINFO_VIDEO_INTERFACE,
	TX_ISP_SINFO_RST_GPIO,
	TX_ISP_SINFO_PWDN_GPIO,
	TX_ISP_SINFO_NKEYS,
};

static const char *const tx_isp_sinfo_key_name[TX_ISP_SINFO_NKEYS] = {
	[TX_ISP_SINFO_NAME] = "name",
	[TX_ISP_SINFO_CHIP_ID] = "chip_id",
	[TX_ISP_SINFO_I2C_ADDR] = "i2c_addr",
	[TX_ISP_SINFO_I2C_ADAPTER] = "i2c_adapter",
	[TX_ISP_SINFO_WIDTH] = "width",
	[TX_ISP_SINFO_HEIGHT] = "height",
	[TX_ISP_SINFO_FPS] = "fps",
	[TX_ISP_SINFO_STATUS] = "status",
	[TX_ISP_SINFO_MIN_FPS] = "min_fps",
	[TX_ISP_SINFO_MAX_FPS] = "max_fps",
	[TX_ISP_SINFO_MCLK] = "mclk",
	[TX_ISP_SINFO_BOOT] = "boot",
	[TX_ISP_SINFO_VIDEO_INTERFACE] = "video_interface",
	[TX_ISP_SINFO_RST_GPIO] = "rst_gpio",
	[TX_ISP_SINFO_PWDN_GPIO] = "pwdn_gpio",
};

struct tx_isp_sinfo_slot;

struct tx_isp_sinfo_file {
	struct tx_isp_sinfo_slot *slot;
	enum tx_isp_sinfo_key key;
};

struct tx_isp_sinfo_slot {
	bool used;
	struct i2c_driver *drv;
	struct module *owner;
	unsigned short default_i2c_addr;
	void *subdev;
	struct proc_dir_entry *dir;
	char dirname[16];
	struct tx_isp_sinfo_file files[TX_ISP_SINFO_NKEYS];
};

static struct tx_isp_sinfo_slot
	tx_isp_sinfo_slots[TX_ISP_SINFO_MAX_SENSORS];
static DEFINE_MUTEX(tx_isp_sinfo_lock);
static struct proc_dir_entry *tx_isp_sinfo_root;

/*
 * The prebuilt sensor modules pass their private object as an opaque subdev.
 * These offsets are stable parts of each SoC's binary ABI, recovered from the
 * installed drivers. Keeping the traversal here avoids duplicating the procfs
 * and ownership logic while leaving the ABI differences explicit at each
 * include site.
 */
static void *tx_isp_sinfo_pointer_at(const void *base, unsigned int offset)
{
	if (!base)
		return NULL;
	return *(void * const *)((const unsigned char *)base + offset);
}

static unsigned int
tx_isp_sinfo_u32_at(const void *base, unsigned int offset)
{
	if (!base)
		return 0;
	return *(const unsigned int *)((const unsigned char *)base + offset);
}

static int tx_isp_sinfo_show(struct seq_file *m, void *unused)
{
	struct tx_isp_sinfo_file *file = m->private;
	struct tx_isp_sinfo_slot *slot = file->slot;
	struct i2c_client *client;
	const void *attr;
	const char *name;
	unsigned int fps;
	unsigned int denominator;

	(void)unused;
	mutex_lock(&tx_isp_sinfo_lock);
	if (!slot->used) {
		mutex_unlock(&tx_isp_sinfo_lock);
		return 0;
	}

	client = NULL;
	attr = NULL;
	if (!(tx_isp_sinfo_config.flags & TX_ISP_SINFO_STATIC_METADATA)) {
		client = tx_isp_sinfo_pointer_at(
			slot->subdev, tx_isp_sinfo_config.client_offset);
		attr = tx_isp_sinfo_pointer_at(
			slot->subdev, tx_isp_sinfo_config.attr_offset);
	}

	switch (file->key) {
	case TX_ISP_SINFO_NAME:
		name = NULL;
		if (!(tx_isp_sinfo_config.flags &
		      TX_ISP_SINFO_STATIC_METADATA))
			name = tx_isp_sinfo_pointer_at(
				attr, tx_isp_sinfo_config.attr_name_offset);
		if (!name && slot->drv)
			name = slot->drv->driver.name;
		if (name)
			seq_printf(m, "%s\n", name);
		break;
	case TX_ISP_SINFO_CHIP_ID:
		if (tx_isp_sinfo_config.flags &
		    TX_ISP_SINFO_STATIC_METADATA)
			seq_printf(m, "0x%x\n",
				   tx_isp_sinfo_config.static_chip_id);
		else if (attr)
			seq_printf(m, "0x%x\n",
				   tx_isp_sinfo_u32_at(
					   attr,
					   tx_isp_sinfo_config.
					   attr_chip_id_offset));
		break;
	case TX_ISP_SINFO_I2C_ADDR:
		if (tx_isp_sinfo_config.flags &
		    TX_ISP_SINFO_STATIC_METADATA)
			seq_printf(m, "0x%x\n", slot->default_i2c_addr);
		else
			seq_printf(m, "0x%x\n",
				   client ? client->addr :
				   slot->default_i2c_addr);
		break;
	case TX_ISP_SINFO_I2C_ADAPTER:
		if (tx_isp_sinfo_config.flags &
		    TX_ISP_SINFO_STATIC_METADATA)
			seq_printf(m, "%u\n",
				   tx_isp_sinfo_config.static_i2c_adapter);
		else
			seq_printf(m, "%u\n",
				   client && client->adapter ?
				   tx_isp_sinfo_u32_at(
					   client->adapter,
					   tx_isp_sinfo_config.
					   adapter_nr_offset) : 0);
		break;
	case TX_ISP_SINFO_WIDTH:
		if (tx_isp_sinfo_config.flags &
		    TX_ISP_SINFO_STATIC_METADATA)
			seq_printf(m, "%u\n",
				   tx_isp_sinfo_config.static_width);
		else if (slot->subdev)
			seq_printf(m, "%u\n",
				   tx_isp_sinfo_u32_at(
					   slot->subdev,
					   tx_isp_sinfo_config.width_offset));
		break;
	case TX_ISP_SINFO_HEIGHT:
		if (tx_isp_sinfo_config.flags &
		    TX_ISP_SINFO_STATIC_METADATA)
			seq_printf(m, "%u\n",
				   tx_isp_sinfo_config.static_height);
		else if (slot->subdev)
			seq_printf(m, "%u\n",
				   tx_isp_sinfo_u32_at(
					   slot->subdev,
					   tx_isp_sinfo_config.height_offset));
		break;
	case TX_ISP_SINFO_FPS:
		if (tx_isp_sinfo_config.flags &
		    TX_ISP_SINFO_STATIC_METADATA) {
			seq_printf(m, "%u\n",
				   tx_isp_sinfo_config.static_fps);
		} else if (slot->subdev) {
			fps = tx_isp_sinfo_u32_at(slot->subdev,
						 tx_isp_sinfo_config.
						 fps_offset);
			denominator = fps & 0xffffU;
			seq_printf(m, "%u\n",
				   (fps >> 16) /
				   (denominator ? denominator : 1));
		}
		break;
	case TX_ISP_SINFO_STATUS:
		seq_printf(m, "%s\n", slot->subdev ? "active" : "loaded");
		break;
	case TX_ISP_SINFO_MCLK:
		if (!(tx_isp_sinfo_config.flags &
		      TX_ISP_SINFO_EXTENDED_ATTRS))
			break;
		if (attr)
			seq_printf(m, "%u\n",
				   tx_isp_sinfo_u32_at(
					   attr,
					   tx_isp_sinfo_config.
					   attr_mclk_offset));
		else
			seq_printf(m, "1\n");
		break;
	case TX_ISP_SINFO_VIDEO_INTERFACE:
		if (!(tx_isp_sinfo_config.flags &
		      TX_ISP_SINFO_EXTENDED_ATTRS))
			break;
		if (attr)
			seq_printf(m, "%u\n",
				   tx_isp_sinfo_u32_at(
					   attr,
					   tx_isp_sinfo_config.
					   attr_interface_offset));
		else
			seq_printf(m, "0\n");
		break;
	case TX_ISP_SINFO_BOOT:
		if (!(tx_isp_sinfo_config.flags &
		      TX_ISP_SINFO_EXTENDED_ATTRS))
			break;
		if (attr)
			seq_printf(m, "%u\n",
				   tx_isp_sinfo_u32_at(
					   attr,
					   tx_isp_sinfo_config.
					   attr_boot_offset));
		else
			seq_printf(m, "0\n");
		break;
	case TX_ISP_SINFO_RST_GPIO:
		if (!(tx_isp_sinfo_config.flags &
		      TX_ISP_SINFO_EXTENDED_ATTRS))
			break;
		if (attr)
			seq_printf(m, "%u\n",
				   tx_isp_sinfo_u32_at(
					   attr,
					   tx_isp_sinfo_config.
					   attr_rst_gpio_offset));
		else
			seq_printf(m, "-1\n");
		break;
	case TX_ISP_SINFO_PWDN_GPIO:
		if (!(tx_isp_sinfo_config.flags &
		      TX_ISP_SINFO_EXTENDED_ATTRS))
			break;
		if (attr)
			seq_printf(m, "%u\n",
				   tx_isp_sinfo_u32_at(
					   attr,
					   tx_isp_sinfo_config.
					   attr_pwdn_gpio_offset));
		else
			seq_printf(m, "-1\n");
		break;
	case TX_ISP_SINFO_MIN_FPS:
		if (!(tx_isp_sinfo_config.flags &
		      TX_ISP_SINFO_EXTENDED_ATTRS))
			break;
		if (slot->subdev) {
			fps = tx_isp_sinfo_u32_at(
				slot->subdev,
				tx_isp_sinfo_config.min_fps_offset);
			denominator = fps & 0xffffU;
			seq_printf(m, "%u\n",
				   (fps >> 16) /
				   (denominator ? denominator : 1));
		}
		break;
	case TX_ISP_SINFO_MAX_FPS:
		if (!(tx_isp_sinfo_config.flags &
		      TX_ISP_SINFO_EXTENDED_ATTRS))
			break;
		if (slot->subdev) {
			fps = tx_isp_sinfo_u32_at(
				slot->subdev,
				tx_isp_sinfo_config.max_fps_offset);
			denominator = fps & 0xffffU;
			seq_printf(m, "%u\n",
				   (fps >> 16) /
				   (denominator ? denominator : 1));
		}
		break;
	default:
		/* This field is not present in the selected SoC ABI. */
		break;
	}
	mutex_unlock(&tx_isp_sinfo_lock);
	return 0;
}

static int tx_isp_sinfo_open(struct inode *inode, struct file *file)
{
	return single_open(file, tx_isp_sinfo_show, PDE_DATA(inode));
}

static const struct file_operations tx_isp_sinfo_fops = {
	.owner = THIS_MODULE,
	.open = tx_isp_sinfo_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static int tx_isp_sinfo_count_show(struct seq_file *m, void *unused)
{
	int i;
	int count = 0;

	(void)unused;
	mutex_lock(&tx_isp_sinfo_lock);
	for (i = 0; i < TX_ISP_SINFO_MAX_SENSORS; ++i)
		if (tx_isp_sinfo_slots[i].used)
			++count;
	mutex_unlock(&tx_isp_sinfo_lock);
	seq_printf(m, "%d\n", count);
	return 0;
}

static int tx_isp_sinfo_count_open(struct inode *inode, struct file *file)
{
	(void)inode;
	return single_open(file, tx_isp_sinfo_count_show, NULL);
}

static const struct file_operations tx_isp_sinfo_count_fops = {
	.owner = THIS_MODULE,
	.open = tx_isp_sinfo_count_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static void tx_isp_sinfo_slot_publish(struct tx_isp_sinfo_slot *slot,
				      int index)
{
	int key;

	snprintf(slot->dirname, sizeof(slot->dirname), "sensor%d", index);
	slot->dir = proc_mkdir(slot->dirname, tx_isp_sinfo_root);
	if (!slot->dir)
		return;

	for (key = 0; key < TX_ISP_SINFO_NKEYS; ++key) {
		slot->files[key].slot = slot;
		slot->files[key].key = key;
		proc_create_data(tx_isp_sinfo_key_name[key], 0444, slot->dir,
				 &tx_isp_sinfo_fops, &slot->files[key]);
	}
}

static void tx_isp_sinfo_slot_unpublish(struct tx_isp_sinfo_slot *slot)
{
	if (slot->dir) {
		remove_proc_subtree(slot->dirname, tx_isp_sinfo_root);
		slot->dir = NULL;
	}
}

int tx_isp_sinfo_driver_add(struct i2c_driver *drv, int default_i2c_addr,
			    struct module *owner)
{
	int i;

	if (!drv || !tx_isp_sinfo_root)
		return -EINVAL;

	mutex_lock(&tx_isp_sinfo_lock);
	/*
	 * A few sensor generations bind their subdevice before registering the
	 * I2C driver. Complete that owner-matched slot instead of publishing a
	 * duplicate entry.
	 */
	for (i = 0; i < TX_ISP_SINFO_MAX_SENSORS; ++i) {
		struct tx_isp_sinfo_slot *slot = &tx_isp_sinfo_slots[i];

		if (slot->used && !slot->drv && slot->owner == owner) {
			slot->drv = drv;
			slot->default_i2c_addr =
				(unsigned short)default_i2c_addr;
			break;
		}
	}
	if (i == TX_ISP_SINFO_MAX_SENSORS) {
		for (i = 0; i < TX_ISP_SINFO_MAX_SENSORS; ++i) {
			struct tx_isp_sinfo_slot *slot =
				&tx_isp_sinfo_slots[i];

			if (slot->used)
				continue;
			memset(slot, 0, sizeof(*slot));
			slot->used = true;
			slot->drv = drv;
			slot->owner = owner;
			slot->default_i2c_addr =
				(unsigned short)default_i2c_addr;
			tx_isp_sinfo_slot_publish(slot, i);
			break;
		}
	}
	mutex_unlock(&tx_isp_sinfo_lock);
	if (i == TX_ISP_SINFO_MAX_SENSORS)
		return -ENOSPC;
	if (tx_isp_sinfo_config.driver_added)
		tx_isp_sinfo_config.driver_added(
			drv, default_i2c_addr, owner);
	return 0;
}
EXPORT_SYMBOL(tx_isp_sinfo_driver_add);

void tx_isp_sinfo_driver_del(struct i2c_driver *drv)
{
	int i;

	if (tx_isp_sinfo_config.driver_removing)
		tx_isp_sinfo_config.driver_removing(drv);
	mutex_lock(&tx_isp_sinfo_lock);
	for (i = 0; i < TX_ISP_SINFO_MAX_SENSORS; ++i) {
		struct tx_isp_sinfo_slot *slot = &tx_isp_sinfo_slots[i];

		if (slot->used && slot->drv == drv) {
			tx_isp_sinfo_slot_unpublish(slot);
			memset(slot, 0, sizeof(*slot));
		}
	}
	mutex_unlock(&tx_isp_sinfo_lock);
}
EXPORT_SYMBOL(tx_isp_sinfo_driver_del);

int tx_isp_sinfo_sensor_bind(void *subdev, struct module *owner)
{
	int i;
	int target = -1;
	int source = -1;

	if (!subdev)
		return -EINVAL;
	if (tx_isp_sinfo_config.sensor_bound)
		tx_isp_sinfo_config.sensor_bound(subdev, owner);
	mutex_lock(&tx_isp_sinfo_lock);
	for (i = 0; i < TX_ISP_SINFO_MAX_SENSORS; ++i) {
		struct tx_isp_sinfo_slot *slot = &tx_isp_sinfo_slots[i];

		if (slot->used && slot->owner == owner && !slot->subdev) {
			slot->subdev = subdev;
			break;
		}
	}

	/*
	 * Some current sensor modules register the I2C driver and bind the
	 * subdevice with different owner tokens. The installed T31/T41 drivers
	 * preserve that ABI by publishing a subdevice-only fallback slot.
	 */
	if (i == TX_ISP_SINFO_MAX_SENSORS) {
		for (i = 0; i < TX_ISP_SINFO_MAX_SENSORS; ++i) {
			if (!tx_isp_sinfo_slots[i].used && target < 0)
				target = i;
			if (tx_isp_sinfo_slots[i].used &&
			    tx_isp_sinfo_slots[i].owner == owner)
				source = i;
		}

		if (target >= 0) {
			struct tx_isp_sinfo_slot *slot =
				&tx_isp_sinfo_slots[target];

			memset(slot, 0, sizeof(*slot));
			slot->used = true;
			slot->owner = owner;
			slot->subdev = subdev;
			if (source >= 0) {
				slot->drv = tx_isp_sinfo_slots[source].drv;
				slot->default_i2c_addr =
					tx_isp_sinfo_slots[source].
					default_i2c_addr;
			}
			tx_isp_sinfo_slot_publish(slot, target);
			i = target;
		}
	}
	mutex_unlock(&tx_isp_sinfo_lock);
	return i == TX_ISP_SINFO_MAX_SENSORS ? -ENOSPC : 0;
}
EXPORT_SYMBOL(tx_isp_sinfo_sensor_bind);

void tx_isp_sinfo_sensor_unbind(void *subdev, struct module *owner)
{
	int i;

	if (!subdev)
		return;
	mutex_lock(&tx_isp_sinfo_lock);
	for (i = 0; i < TX_ISP_SINFO_MAX_SENSORS; ++i)
		if (tx_isp_sinfo_slots[i].used &&
		    tx_isp_sinfo_slots[i].subdev == subdev)
			tx_isp_sinfo_slots[i].subdev = NULL;
	mutex_unlock(&tx_isp_sinfo_lock);
	if (tx_isp_sinfo_config.sensor_unbound)
		tx_isp_sinfo_config.sensor_unbound(subdev, owner);
}
EXPORT_SYMBOL(tx_isp_sinfo_sensor_unbind);

int tx_isp_sinfo_init(void)
{
	tx_isp_sinfo_root = proc_mkdir("jz/sensor", NULL);
	if (!tx_isp_sinfo_root) {
		pr_warn("tx-isp-sinfo: cannot create /proc/jz/sensor\n");
		return 0;
	}
	proc_create("count", 0444, tx_isp_sinfo_root,
		    &tx_isp_sinfo_count_fops);
	return 0;
}

void tx_isp_sinfo_exit(void)
{
	mutex_lock(&tx_isp_sinfo_lock);
	memset(tx_isp_sinfo_slots, 0, sizeof(tx_isp_sinfo_slots));
	mutex_unlock(&tx_isp_sinfo_lock);
	if (tx_isp_sinfo_root) {
		remove_proc_subtree("jz/sensor", NULL);
		tx_isp_sinfo_root = NULL;
	}
}
