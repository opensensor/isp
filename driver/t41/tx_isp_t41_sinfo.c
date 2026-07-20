/* Thingino sensor registry ABI used by current T41 sensor modules. */

#define T41_SINFO_MAX_SENSORS 4

enum t41_sinfo_key {
	T41_SINFO_NAME,
	T41_SINFO_CHIP_ID,
	T41_SINFO_I2C_ADDR,
	T41_SINFO_I2C_ADAPTER,
	T41_SINFO_WIDTH,
	T41_SINFO_HEIGHT,
	T41_SINFO_FPS,
	T41_SINFO_STATUS,
	T41_SINFO_MIN_FPS,
	T41_SINFO_MAX_FPS,
	T41_SINFO_MCLK,
	T41_SINFO_BOOT,
	T41_SINFO_VIDEO_INTERFACE,
	T41_SINFO_RST_GPIO,
	T41_SINFO_PWDN_GPIO,
	T41_SINFO_NKEYS,
};

static const char *const t41_sinfo_key_name[T41_SINFO_NKEYS] = {
	[T41_SINFO_NAME] = "name",
	[T41_SINFO_CHIP_ID] = "chip_id",
	[T41_SINFO_I2C_ADDR] = "i2c_addr",
	[T41_SINFO_I2C_ADAPTER] = "i2c_adapter",
	[T41_SINFO_WIDTH] = "width",
	[T41_SINFO_HEIGHT] = "height",
	[T41_SINFO_FPS] = "fps",
	[T41_SINFO_STATUS] = "status",
	[T41_SINFO_MIN_FPS] = "min_fps",
	[T41_SINFO_MAX_FPS] = "max_fps",
	[T41_SINFO_MCLK] = "mclk",
	[T41_SINFO_BOOT] = "boot",
	[T41_SINFO_VIDEO_INTERFACE] = "video_interface",
	[T41_SINFO_RST_GPIO] = "rst_gpio",
	[T41_SINFO_PWDN_GPIO] = "pwdn_gpio",
};

struct t41_sinfo_slot;

struct t41_sinfo_file {
	struct t41_sinfo_slot *slot;
	enum t41_sinfo_key key;
};

struct t41_sinfo_slot {
	bool used;
	struct i2c_driver *drv;
	struct module *owner;
	unsigned short def_i2c_addr;
	void *subdev;
	struct proc_dir_entry *dir;
	char dirname[16];
	struct t41_sinfo_file files[T41_SINFO_NKEYS];
};

static struct t41_sinfo_slot t41_sinfo_slots[T41_SINFO_MAX_SENSORS];
static DEFINE_MUTEX(t41_sinfo_lock);
static struct proc_dir_entry *t41_sinfo_root;

static int t41_sinfo_show(struct seq_file *m, void *unused)
{
	struct t41_sinfo_file *file = m->private;
	struct t41_sinfo_slot *slot = file->slot;

	(void)unused;
	mutex_lock(&t41_sinfo_lock);
	if (!slot->used) {
		mutex_unlock(&t41_sinfo_lock);
		return 0;
	}

	switch (file->key) {
	case T41_SINFO_NAME:
		if (slot->drv && slot->drv->driver.name)
			seq_printf(m, "%s\n", slot->drv->driver.name);
		break;
	case T41_SINFO_I2C_ADDR:
		seq_printf(m, "0x%x\n", slot->def_i2c_addr);
		break;
	case T41_SINFO_I2C_ADAPTER:
		seq_printf(m, "0\n");
		break;
	case T41_SINFO_STATUS:
		seq_printf(m, "%s\n", slot->subdev ? "active" : "loaded");
		break;
	case T41_SINFO_MCLK:
		seq_printf(m, "1\n");
		break;
	case T41_SINFO_BOOT:
	case T41_SINFO_VIDEO_INTERFACE:
		seq_printf(m, "0\n");
		break;
	case T41_SINFO_RST_GPIO:
	case T41_SINFO_PWDN_GPIO:
		seq_printf(m, "-1\n");
		break;
	default:
		/* Unknown until the sensor has been configured by userspace. */
		break;
	}
	mutex_unlock(&t41_sinfo_lock);
	return 0;
}

static int t41_sinfo_open(struct inode *inode, struct file *file)
{
	return single_open(file, t41_sinfo_show, PDE_DATA(inode));
}

static const struct file_operations t41_sinfo_fops = {
	.owner = THIS_MODULE,
	.open = t41_sinfo_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static int t41_sinfo_count_show(struct seq_file *m, void *unused)
{
	int i;
	int count = 0;

	(void)unused;
	mutex_lock(&t41_sinfo_lock);
	for (i = 0; i < T41_SINFO_MAX_SENSORS; ++i)
		if (t41_sinfo_slots[i].used)
			++count;
	mutex_unlock(&t41_sinfo_lock);
	seq_printf(m, "%d\n", count);
	return 0;
}

static int t41_sinfo_count_open(struct inode *inode, struct file *file)
{
	(void)inode;
	return single_open(file, t41_sinfo_count_show, NULL);
}

static const struct file_operations t41_sinfo_count_fops = {
	.owner = THIS_MODULE,
	.open = t41_sinfo_count_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static void t41_sinfo_slot_publish(struct t41_sinfo_slot *slot, int index)
{
	int key;

	snprintf(slot->dirname, sizeof(slot->dirname), "sensor%d", index);
	slot->dir = proc_mkdir(slot->dirname, t41_sinfo_root);
	if (!slot->dir)
		return;

	for (key = 0; key < T41_SINFO_NKEYS; ++key) {
		slot->files[key].slot = slot;
		slot->files[key].key = key;
		proc_create_data(t41_sinfo_key_name[key], 0444, slot->dir,
			&t41_sinfo_fops, &slot->files[key]);
	}
}

static void t41_sinfo_slot_unpublish(struct t41_sinfo_slot *slot)
{
	if (slot->dir) {
		remove_proc_subtree(slot->dirname, t41_sinfo_root);
		slot->dir = NULL;
	}
}

int tx_isp_sinfo_driver_add(struct i2c_driver *drv, int def_i2c_addr,
			    struct module *owner)
{
	int i;

	if (!drv || !t41_sinfo_root)
		return -EINVAL;

	mutex_lock(&t41_sinfo_lock);
	for (i = 0; i < T41_SINFO_MAX_SENSORS; ++i) {
		struct t41_sinfo_slot *slot = &t41_sinfo_slots[i];

		if (!slot->used) {
			memset(slot, 0, sizeof(*slot));
			slot->used = true;
			slot->drv = drv;
			slot->owner = owner;
			slot->def_i2c_addr = (unsigned short)def_i2c_addr;
			t41_sinfo_slot_publish(slot, i);
			break;
		}
	}
	mutex_unlock(&t41_sinfo_lock);
	return i == T41_SINFO_MAX_SENSORS ? -ENOSPC : 0;
}
EXPORT_SYMBOL(tx_isp_sinfo_driver_add);

void tx_isp_sinfo_driver_del(struct i2c_driver *drv)
{
	int i;

	mutex_lock(&t41_sinfo_lock);
	for (i = 0; i < T41_SINFO_MAX_SENSORS; ++i) {
		struct t41_sinfo_slot *slot = &t41_sinfo_slots[i];

		if (slot->used && slot->drv == drv) {
			t41_sinfo_slot_unpublish(slot);
			memset(slot, 0, sizeof(*slot));
		}
	}
	mutex_unlock(&t41_sinfo_lock);
}
EXPORT_SYMBOL(tx_isp_sinfo_driver_del);

int tx_isp_sinfo_sensor_bind(void *subdev, struct module *owner)
{
	int i;

	if (!subdev)
		return -EINVAL;
	mutex_lock(&t41_sinfo_lock);
	for (i = 0; i < T41_SINFO_MAX_SENSORS; ++i) {
		struct t41_sinfo_slot *slot = &t41_sinfo_slots[i];

		if (slot->used && slot->owner == owner && !slot->subdev) {
			slot->subdev = subdev;
			break;
		}
	}
	mutex_unlock(&t41_sinfo_lock);
	return i == T41_SINFO_MAX_SENSORS ? -ENOSPC : 0;
}
EXPORT_SYMBOL(tx_isp_sinfo_sensor_bind);

void tx_isp_sinfo_sensor_unbind(void *subdev, struct module *owner)
{
	int i;

	(void)owner;
	mutex_lock(&t41_sinfo_lock);
	for (i = 0; i < T41_SINFO_MAX_SENSORS; ++i)
		if (t41_sinfo_slots[i].used &&
		    t41_sinfo_slots[i].subdev == subdev)
			t41_sinfo_slots[i].subdev = NULL;
	mutex_unlock(&t41_sinfo_lock);
}
EXPORT_SYMBOL(tx_isp_sinfo_sensor_unbind);

static int tx_isp_t41_sinfo_init(void)
{
	t41_sinfo_root = proc_mkdir("jz/sensor", NULL);
	if (!t41_sinfo_root) {
		pr_warn("tx-isp-sinfo: cannot create /proc/jz/sensor\n");
		return 0;
	}
	proc_create("count", 0444, t41_sinfo_root,
		&t41_sinfo_count_fops);
	return 0;
}

static void tx_isp_t41_sinfo_exit(void)
{
	mutex_lock(&t41_sinfo_lock);
	memset(t41_sinfo_slots, 0, sizeof(t41_sinfo_slots));
	mutex_unlock(&t41_sinfo_lock);
	if (t41_sinfo_root) {
		remove_proc_subtree("jz/sensor", NULL);
		t41_sinfo_root = NULL;
	}
}
