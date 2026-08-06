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

struct tx_isp_sinfo_stats {
	unsigned int magic;
	unsigned int driver_add_calls;
	unsigned int driver_add_successes;
	unsigned int driver_del_calls;
	unsigned int driver_del_slots;
	unsigned int sensor_bind_calls;
	unsigned int sensor_bind_successes;
	unsigned int sensor_unbind_calls;
	unsigned int sensor_unbind_slots;
	struct tx_isp_sinfo_slot *slots;
};

static struct tx_isp_sinfo_stats tx_isp_sinfo_stats = {
	.magic = 0x53494e46U,
};
#define tx_isp_sinfo_heap_slots tx_isp_sinfo_stats.slots
static DEFINE_MUTEX(tx_isp_sinfo_lock);
#ifdef TX_ISP_SINFO_BSS_COMPAT_SLOTS
/*
 * Some recovered monoliths address anonymous core state beyond their last
 * named BSS object. A per-SoC adapter may preserve the old registry object's
 * BSS footprint and its initial contents while the authoritative slots move
 * to protected heap storage.
 */
static struct {
	struct proc_dir_entry *root;
	struct tx_isp_sinfo_slot slots[TX_ISP_SINFO_MAX_SENSORS];
} tx_isp_sinfo_bss_layout __attribute__((used));
#endif
#ifdef TX_ISP_SINFO_STABLE_PROC_SNAPSHOT
/*
 * Keep every live registry pointer away from a compatibility BSS tail which
 * a recovered monolith may address as anonymous private state.  The public
 * slots pointer still points directly at the slot array, so the stats ABI and
 * the BSS footprint remain unchanged.
 */
struct tx_isp_sinfo_heap_state {
	struct proc_dir_entry *root;
	struct tx_isp_sinfo_slot slots[TX_ISP_SINFO_MAX_SENSORS];
};
#define tx_isp_sinfo_heap_state_ptr \
	((struct tx_isp_sinfo_heap_state *)((unsigned char *) \
	 tx_isp_sinfo_heap_slots - \
	 offsetof(struct tx_isp_sinfo_heap_state, slots)))
#define tx_isp_sinfo_root tx_isp_sinfo_heap_state_ptr->root
#define tx_isp_sinfo_slots tx_isp_sinfo_heap_slots
#define tx_isp_sinfo_heap_allocation tx_isp_sinfo_heap_state_ptr
#elif defined(TX_ISP_SINFO_BSS_COMPAT_SLOTS)
#define tx_isp_sinfo_root tx_isp_sinfo_bss_layout.root
#define tx_isp_sinfo_slots tx_isp_sinfo_bss_layout.slots
#define tx_isp_sinfo_heap_allocation tx_isp_sinfo_heap_slots
#else
static struct proc_dir_entry *tx_isp_sinfo_root;
#define tx_isp_sinfo_slots tx_isp_sinfo_heap_slots
#define tx_isp_sinfo_heap_allocation tx_isp_sinfo_heap_slots
#endif
#ifdef TX_ISP_SINFO_STABLE_PROC_SNAPSHOT
#define tx_isp_sinfo_proc_slots tx_isp_sinfo_heap_slots
#else
#define tx_isp_sinfo_proc_slots tx_isp_sinfo_slots
#endif
#ifndef TX_ISP_SINFO_CONFIG_FLAGS
#define TX_ISP_SINFO_CONFIG_FLAGS tx_isp_sinfo_config.flags
#endif

static bool tx_isp_sinfo_key_supported(enum tx_isp_sinfo_key key)
{
	unsigned int config_flags = TX_ISP_SINFO_CONFIG_FLAGS;

	switch (key) {
	case TX_ISP_SINFO_MIN_FPS:
	case TX_ISP_SINFO_MAX_FPS:
		return config_flags & TX_ISP_SINFO_EXTENDED_ATTRS;
	case TX_ISP_SINFO_MCLK:
	case TX_ISP_SINFO_BOOT:
	case TX_ISP_SINFO_VIDEO_INTERFACE:
	case TX_ISP_SINFO_RST_GPIO:
	case TX_ISP_SINFO_PWDN_GPIO:
		return config_flags &
		       (TX_ISP_SINFO_EXTENDED_ATTRS |
			TX_ISP_SINFO_REGINFO_WIRING);
	default:
		return true;
	}
}

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

static int tx_isp_sinfo_s32_at(const void *base, unsigned int offset)
{
	if (!base)
		return 0;
	return *(const int *)((const unsigned char *)base + offset);
}

static int tx_isp_sinfo_show(struct seq_file *m, void *unused)
{
	struct tx_isp_sinfo_file *file = m->private;
	struct tx_isp_sinfo_slot *slot = file->slot;
	enum tx_isp_sinfo_key selected_key = file->key;
	struct i2c_client *client;
	const void *attr;
	const void *info;
	const void *wiring;
	const char *name;
	unsigned int fps;
	unsigned int denominator;
	unsigned int config_flags = TX_ISP_SINFO_CONFIG_FLAGS;
	unsigned int wiring_mclk_offset;
	unsigned int wiring_boot_offset;
	unsigned int wiring_interface_offset;
	unsigned int wiring_rst_gpio_offset;
	unsigned int wiring_pwdn_gpio_offset;

	(void)unused;
	mutex_lock(&tx_isp_sinfo_lock);
	if (!slot->used) {
		mutex_unlock(&tx_isp_sinfo_lock);
		return 0;
	}

	client = NULL;
	attr = NULL;
	info = NULL;
	if (!(config_flags & TX_ISP_SINFO_STATIC_METADATA)) {
		client = tx_isp_sinfo_pointer_at(
			slot->subdev, tx_isp_sinfo_config.client_offset);
		attr = tx_isp_sinfo_pointer_at(
			slot->subdev, tx_isp_sinfo_config.attr_offset);
		if (slot->subdev &&
		    (config_flags & TX_ISP_SINFO_REGINFO_WIRING))
			info = (const unsigned char *)slot->subdev +
			       tx_isp_sinfo_config.info_offset;
	}
	if (config_flags & TX_ISP_SINFO_REGINFO_WIRING) {
		wiring = info;
		wiring_mclk_offset = tx_isp_sinfo_config.info_mclk_offset;
		wiring_boot_offset = tx_isp_sinfo_config.info_boot_offset;
		wiring_interface_offset =
			tx_isp_sinfo_config.info_interface_offset;
		wiring_rst_gpio_offset =
			tx_isp_sinfo_config.info_rst_gpio_offset;
		wiring_pwdn_gpio_offset =
			tx_isp_sinfo_config.info_pwdn_gpio_offset;
	} else {
		wiring = attr;
		wiring_mclk_offset = tx_isp_sinfo_config.attr_mclk_offset;
		wiring_boot_offset = tx_isp_sinfo_config.attr_boot_offset;
		wiring_interface_offset =
			tx_isp_sinfo_config.attr_interface_offset;
		wiring_rst_gpio_offset =
			tx_isp_sinfo_config.attr_rst_gpio_offset;
		wiring_pwdn_gpio_offset =
			tx_isp_sinfo_config.attr_pwdn_gpio_offset;
	}

	switch (selected_key) {
	case TX_ISP_SINFO_NAME:
		name = NULL;
		if (!(config_flags &
		      TX_ISP_SINFO_STATIC_METADATA))
			name = tx_isp_sinfo_pointer_at(
				attr, tx_isp_sinfo_config.attr_name_offset);
		if (!name && slot->drv)
			name = slot->drv->driver.name;
		if (name)
			seq_printf(m, "%s\n", name);
		break;
	case TX_ISP_SINFO_CHIP_ID:
		if (config_flags &
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
		if (config_flags &
		    TX_ISP_SINFO_STATIC_METADATA)
			seq_printf(m, "0x%x\n", slot->default_i2c_addr);
		else
			seq_printf(m, "0x%x\n",
				   client ? client->addr :
				   slot->default_i2c_addr);
		break;
	case TX_ISP_SINFO_I2C_ADAPTER:
		if (config_flags &
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
		if (config_flags &
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
		if (config_flags &
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
		if (config_flags &
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
		if (!(config_flags &
		      (TX_ISP_SINFO_EXTENDED_ATTRS |
		       TX_ISP_SINFO_REGINFO_WIRING)))
			break;
		if (wiring)
			seq_printf(m, "%u\n",
				   tx_isp_sinfo_u32_at(
					   wiring, wiring_mclk_offset));
		else
			seq_printf(m, "1\n");
		break;
	case TX_ISP_SINFO_VIDEO_INTERFACE:
		if (!(config_flags &
		      (TX_ISP_SINFO_EXTENDED_ATTRS |
		       TX_ISP_SINFO_REGINFO_WIRING)))
			break;
		if (wiring)
			seq_printf(m, "%u\n",
				   tx_isp_sinfo_u32_at(
					   wiring, wiring_interface_offset));
		else
			seq_printf(m, "0\n");
		break;
	case TX_ISP_SINFO_BOOT:
		if (!(config_flags &
		      (TX_ISP_SINFO_EXTENDED_ATTRS |
		       TX_ISP_SINFO_REGINFO_WIRING)))
			break;
		if (wiring)
			seq_printf(m, "%u\n",
				   tx_isp_sinfo_u32_at(
					   wiring, wiring_boot_offset));
		else
			seq_printf(m, "0\n");
		break;
	case TX_ISP_SINFO_RST_GPIO:
		if (!(config_flags &
		      (TX_ISP_SINFO_EXTENDED_ATTRS |
		       TX_ISP_SINFO_REGINFO_WIRING)))
			break;
		if (wiring)
			seq_printf(m, "%d\n",
				   tx_isp_sinfo_s32_at(
					   wiring, wiring_rst_gpio_offset));
		else
			seq_printf(m, "-1\n");
		break;
	case TX_ISP_SINFO_PWDN_GPIO:
		if (!(config_flags &
		      (TX_ISP_SINFO_EXTENDED_ATTRS |
		       TX_ISP_SINFO_REGINFO_WIRING)))
			break;
		if (wiring)
			seq_printf(m, "%d\n",
				   tx_isp_sinfo_s32_at(
					   wiring, wiring_pwdn_gpio_offset));
		else
			seq_printf(m, "-1\n");
		break;
	case TX_ISP_SINFO_MIN_FPS:
		if (!(config_flags &
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
		if (!(config_flags &
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
#ifdef TX_ISP_SINFO_STABLE_PROC_SNAPSHOT
	for (i = 0; i < TX_ISP_SINFO_MAX_SENSORS; ++i)
		if (tx_isp_sinfo_heap_slots[i].used)
			++count;
#else
	if (tx_isp_sinfo_slots)
		for (i = 0; i < TX_ISP_SINFO_MAX_SENSORS; ++i)
			if (tx_isp_sinfo_slots[i].used)
				++count;
#endif
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

static int tx_isp_sinfo_events_show(struct seq_file *m, void *unused)
{
	int i;
#ifdef TX_ISP_SINFO_STABLE_PROC_SNAPSHOT
	const struct tx_isp_sinfo_slot *slot;
#endif

	(void)unused;
	mutex_lock(&tx_isp_sinfo_lock);
	seq_printf(m, "magic=0x%08x\n", tx_isp_sinfo_stats.magic);
	seq_printf(m, "driver_add calls=%u successes=%u\n",
		   tx_isp_sinfo_stats.driver_add_calls,
		   tx_isp_sinfo_stats.driver_add_successes);
	seq_printf(m, "driver_del calls=%u slots=%u\n",
		   tx_isp_sinfo_stats.driver_del_calls,
		   tx_isp_sinfo_stats.driver_del_slots);
	seq_printf(m, "sensor_bind calls=%u successes=%u\n",
		   tx_isp_sinfo_stats.sensor_bind_calls,
		   tx_isp_sinfo_stats.sensor_bind_successes);
	seq_printf(m, "sensor_unbind calls=%u slots=%u\n",
		   tx_isp_sinfo_stats.sensor_unbind_calls,
		   tx_isp_sinfo_stats.sensor_unbind_slots);
#ifdef TX_ISP_SINFO_STABLE_PROC_SNAPSHOT
	for (i = 0, slot = tx_isp_sinfo_heap_slots;
	     i < TX_ISP_SINFO_MAX_SENSORS;
	     ++i, ++slot) {
#else
	for (i = 0; tx_isp_sinfo_slots &&
		    i < TX_ISP_SINFO_MAX_SENSORS; ++i) {
		const struct tx_isp_sinfo_slot *slot =
			&tx_isp_sinfo_slots[i];
#endif

		seq_printf(m,
			   "slot%d used=%u drv=%p owner=%p subdev=%p dir=%p addr=0x%x\n",
			   i, slot->used ? 1U : 0U, slot->drv, slot->owner,
			   slot->subdev,
#ifdef TX_ISP_SINFO_STABLE_PROC_SNAPSHOT
			   NULL,
#else
			   slot->dir,
#endif
			   slot->default_i2c_addr);
	}
	mutex_unlock(&tx_isp_sinfo_lock);
	return 0;
}

static int tx_isp_sinfo_events_open(struct inode *inode, struct file *file)
{
	(void)inode;
	return single_open(file, tx_isp_sinfo_events_show, NULL);
}

static const struct file_operations tx_isp_sinfo_events_fops = {
	.owner = THIS_MODULE,
	.open = tx_isp_sinfo_events_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static void tx_isp_sinfo_slot_sync_compat(int index);

#ifdef TX_ISP_SINFO_STABLE_PROC_SNAPSHOT
#define TX_ISP_SINFO_LAYOUT_OPT	__attribute__((optimize("Os")))
#else
#define TX_ISP_SINFO_LAYOUT_OPT
#endif

static TX_ISP_SINFO_LAYOUT_OPT void
tx_isp_sinfo_slot_publish(struct tx_isp_sinfo_slot *slot, int index)
{
	int key;
#ifdef TX_ISP_SINFO_STABLE_PROC_SNAPSHOT
	struct tx_isp_sinfo_slot *proc_slot;
#endif

	snprintf(slot->dirname, sizeof(slot->dirname), "sensor%d", index);
	slot->dir = proc_mkdir(slot->dirname, tx_isp_sinfo_root);
	if (!slot->dir)
		return;

#ifdef TX_ISP_SINFO_STABLE_PROC_SNAPSHOT
	/*
	 * Preserve the recovered lifecycle writes in the compatibility BSS, but
	 * give procfs private data an address in the stable heap snapshot. The
	 * recovered core may reuse the BSS after registration without invalidating
	 * an already-open proc file.
	 */
	tx_isp_sinfo_slot_sync_compat(index);
	proc_slot = &tx_isp_sinfo_heap_slots[index];
#endif
	for (key = 0; key < TX_ISP_SINFO_NKEYS; ++key) {
		if (!tx_isp_sinfo_key_supported(key))
			continue;
		slot->files[key].slot = slot;
		slot->files[key].key = key;
#ifdef TX_ISP_SINFO_STABLE_PROC_SNAPSHOT
		proc_slot->files[key].slot = proc_slot;
		proc_slot->files[key].key = key;
#endif
		proc_create_data(tx_isp_sinfo_key_name[key], 0444, slot->dir,
				 &tx_isp_sinfo_fops,
#ifdef TX_ISP_SINFO_STABLE_PROC_SNAPSHOT
				 &proc_slot->files[key]);
#else
				 &slot->files[key]);
#endif
	}
}
#undef TX_ISP_SINFO_LAYOUT_OPT

static void tx_isp_sinfo_slot_unpublish(struct tx_isp_sinfo_slot *slot)
{
	if (slot->dir) {
		remove_proc_subtree(slot->dirname, tx_isp_sinfo_root);
		slot->dir = NULL;
	}
}

static void
tx_isp_sinfo_slot_sync_compat(int index)
{
#if defined(TX_ISP_SINFO_BSS_COMPAT_SLOTS) && \
	!defined(TX_ISP_SINFO_STABLE_PROC_SNAPSHOT)
	struct tx_isp_sinfo_slot *snapshot;
	int key;

	if (!tx_isp_sinfo_heap_slots || index < 0 ||
	    index >= TX_ISP_SINFO_MAX_SENSORS)
		return;

	snapshot = &tx_isp_sinfo_heap_slots[index];
	*snapshot = tx_isp_sinfo_slots[index];
	for (key = 0; key < TX_ISP_SINFO_NKEYS; ++key) {
		snapshot->files[key].slot = snapshot;
	}
#else
	(void)index;
#endif
}

int tx_isp_sinfo_driver_add(struct i2c_driver *drv, int default_i2c_addr,
			    struct module *owner)
{
	int i;

	if (!drv || !tx_isp_sinfo_slots || !tx_isp_sinfo_root)
		return -EINVAL;

	mutex_lock(&tx_isp_sinfo_lock);
	tx_isp_sinfo_stats.driver_add_calls++;
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
	if (i != TX_ISP_SINFO_MAX_SENSORS) {
		tx_isp_sinfo_slot_sync_compat(i);
		tx_isp_sinfo_stats.driver_add_successes++;
	}
	mutex_unlock(&tx_isp_sinfo_lock);
	if (i == TX_ISP_SINFO_MAX_SENSORS) {
		pr_warn("tx-isp-sinfo: driver_add full drv=%p owner=%p addr=0x%x\n",
			drv, owner, default_i2c_addr);
		return -ENOSPC;
	}
	pr_info("tx-isp-sinfo: driver_add slot=%d drv=%p owner=%p addr=0x%x\n",
		i, drv, owner, default_i2c_addr);
	if (tx_isp_sinfo_config.driver_added)
		tx_isp_sinfo_config.driver_added(
			drv, default_i2c_addr, owner);
	return 0;
}
EXPORT_SYMBOL(tx_isp_sinfo_driver_add);

void tx_isp_sinfo_driver_del(struct i2c_driver *drv)
{
	int i;
	int removed = 0;

	if (tx_isp_sinfo_config.driver_removing)
		tx_isp_sinfo_config.driver_removing(drv);
	mutex_lock(&tx_isp_sinfo_lock);
	tx_isp_sinfo_stats.driver_del_calls++;
	for (i = 0; i < TX_ISP_SINFO_MAX_SENSORS; ++i) {
		struct tx_isp_sinfo_slot *slot = &tx_isp_sinfo_slots[i];

		if (slot->used && slot->drv == drv) {
			pr_info("tx-isp-sinfo: driver_del slot=%d drv=%p owner=%p subdev=%p\n",
				i, drv, slot->owner, slot->subdev);
			tx_isp_sinfo_slot_unpublish(slot);
			memset(slot, 0, sizeof(*slot));
			tx_isp_sinfo_slot_sync_compat(i);
			removed++;
			tx_isp_sinfo_stats.driver_del_slots++;
		}
	}
	mutex_unlock(&tx_isp_sinfo_lock);
	if (!removed)
		pr_info("tx-isp-sinfo: driver_del unmatched drv=%p\n", drv);
}
EXPORT_SYMBOL(tx_isp_sinfo_driver_del);

int tx_isp_sinfo_sensor_bind(void *subdev, struct module *owner)
{
	int i;
	int target = -1;
	int source = -1;

	if (!subdev || !tx_isp_sinfo_slots)
		return -EINVAL;
	if (tx_isp_sinfo_config.sensor_bound)
		tx_isp_sinfo_config.sensor_bound(subdev, owner);
	mutex_lock(&tx_isp_sinfo_lock);
	tx_isp_sinfo_stats.sensor_bind_calls++;
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
	if (i != TX_ISP_SINFO_MAX_SENSORS) {
		tx_isp_sinfo_slot_sync_compat(i);
		tx_isp_sinfo_stats.sensor_bind_successes++;
	}
	mutex_unlock(&tx_isp_sinfo_lock);
	if (i == TX_ISP_SINFO_MAX_SENSORS) {
		pr_warn("tx-isp-sinfo: sensor_bind full subdev=%p owner=%p\n",
			subdev, owner);
		return -ENOSPC;
	}
	pr_info("tx-isp-sinfo: sensor_bind slot=%d subdev=%p owner=%p\n",
		i, subdev, owner);
	return 0;
}
EXPORT_SYMBOL(tx_isp_sinfo_sensor_bind);

void tx_isp_sinfo_sensor_unbind(void *subdev, struct module *owner)
{
	int i;
	int unbound = 0;

	if (!subdev)
		return;
	mutex_lock(&tx_isp_sinfo_lock);
	tx_isp_sinfo_stats.sensor_unbind_calls++;
	for (i = 0; i < TX_ISP_SINFO_MAX_SENSORS; ++i)
		if (tx_isp_sinfo_slots[i].used &&
		    tx_isp_sinfo_slots[i].subdev == subdev) {
			pr_info("tx-isp-sinfo: sensor_unbind slot=%d subdev=%p owner=%p\n",
				i, subdev, owner);
			tx_isp_sinfo_slots[i].subdev = NULL;
			tx_isp_sinfo_slot_sync_compat(i);
			unbound++;
			tx_isp_sinfo_stats.sensor_unbind_slots++;
		}
	mutex_unlock(&tx_isp_sinfo_lock);
	if (!unbound)
		pr_info("tx-isp-sinfo: sensor_unbind unmatched subdev=%p owner=%p\n",
			subdev, owner);
	if (tx_isp_sinfo_config.sensor_unbound)
		tx_isp_sinfo_config.sensor_unbound(subdev, owner);
}
EXPORT_SYMBOL(tx_isp_sinfo_sensor_unbind);

int tx_isp_sinfo_init(void)
{
	enum tx_isp_sinfo_config_status config_status;
#ifdef TX_ISP_SINFO_STABLE_PROC_SNAPSHOT
	struct tx_isp_sinfo_heap_state *heap_state;
#endif

	config_status = tx_isp_sinfo_config_check(
		&tx_isp_sinfo_config, TX_ISP_SINFO_CONFIG_FLAGS);
	if (config_status != TX_ISP_SINFO_CONFIG_OK) {
		pr_err("tx-isp-sinfo: invalid ABI config status=%u flags=0x%x\n",
		       (unsigned int)config_status,
		       (unsigned int)TX_ISP_SINFO_CONFIG_FLAGS);
		return -EINVAL;
	}

#ifdef TX_ISP_SINFO_STABLE_PROC_SNAPSHOT
	heap_state = kzalloc(sizeof(*heap_state), GFP_KERNEL);
	if (heap_state)
		tx_isp_sinfo_heap_slots = heap_state->slots;
#else
	tx_isp_sinfo_heap_slots = kzalloc(
		sizeof(*tx_isp_sinfo_heap_slots) * TX_ISP_SINFO_MAX_SENSORS,
		GFP_KERNEL);
#endif
	if (!tx_isp_sinfo_heap_slots)
		return -ENOMEM;

	tx_isp_sinfo_root = proc_mkdir("jz/sensor", NULL);
	if (!tx_isp_sinfo_root) {
		pr_warn("tx-isp-sinfo: cannot create /proc/jz/sensor\n");
		kfree(tx_isp_sinfo_heap_allocation);
		tx_isp_sinfo_heap_slots = NULL;
		return 0;
	}
	proc_create("count", 0444, tx_isp_sinfo_root,
		    &tx_isp_sinfo_count_fops);
	proc_create("events", 0444, tx_isp_sinfo_root,
		    &tx_isp_sinfo_events_fops);
	pr_info("tx-isp-sinfo: initialized max_sensors=%u\n",
		TX_ISP_SINFO_MAX_SENSORS);
	return 0;
}

void tx_isp_sinfo_exit(void)
{
	struct proc_dir_entry *root = NULL;
	void *heap_allocation = NULL;

	mutex_lock(&tx_isp_sinfo_lock);
	if (tx_isp_sinfo_heap_slots) {
		root = tx_isp_sinfo_root;
		tx_isp_sinfo_root = NULL;
		heap_allocation = tx_isp_sinfo_heap_allocation;
	}
	mutex_unlock(&tx_isp_sinfo_lock);
	if (root)
		remove_proc_subtree("jz/sensor", NULL);
	mutex_lock(&tx_isp_sinfo_lock);
	kfree(heap_allocation);
	tx_isp_sinfo_heap_slots = NULL;
	mutex_unlock(&tx_isp_sinfo_lock);
	pr_info("tx-isp-sinfo: exited\n");
}
