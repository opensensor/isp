#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
#include <linux/jiffies.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>

#define ISP_MONITOR_VERSION "1.4"

// Region types for classification
enum region_type {
    REG_TYPE_CONTROL,    // Control/status registers
    REG_TYPE_DATA,       // Data/configuration blocks
    REG_TYPE_VIC,        // VIC-specific registers
    REG_TYPE_TUNING,     // Tuning parameters and tables
    REG_TYPE_UNKNOWN
};

// Structure to track sequential writes
struct seq_write_info {
    u32 start_offset;
    u32 count;
    u32 last_value;
    bool in_progress;
};

struct isp_region {
    phys_addr_t phys_addr;
    void __iomem *virt_addr;
    u32 *last_values;
    unsigned long *last_change_time;
    size_t size;
    const char *name;
    struct delayed_work monitor_work;
    bool monitoring;
    struct seq_write_info seq_write;
};

// VIC register monitoring structure
struct vic_reg_monitor {
    u32 last_0x04;
    u32 last_0x0c;
    u32 last_0x24;
    unsigned long last_check_time;
    struct proc_dir_entry *proc_entry;
};

static struct vic_reg_monitor vic_monitor = {0};

// Register ranges to classify regions
struct reg_range {
    u32 start;
    u32 end;
    enum region_type type;
    const char *description;
};

// Known register ranges and their types
static const struct reg_range isp_ranges[] = {
    // Keep control ranges for tracing
    {0x9800, 0x98FF, REG_TYPE_CONTROL, "ISP Control"},
    {0x9A00, 0x9AFF, REG_TYPE_VIC, "VIC Control"},
    {0xB000, 0xB0FF, REG_TYPE_CONTROL, "Core Control"},

    // Add CSI PHY control ranges
    {0x0000, 0x00FF, REG_TYPE_CONTROL, "CSI PHY Control"},  // Basic PHY control
    {0x0100, 0x01FF, REG_TYPE_CONTROL, "CSI PHY Config"},   // PHY configuration
    {0x0200, 0x02FF, REG_TYPE_CONTROL, "CSI Lane Config"},  // Lane configuration

    // Mark tuning parameter ranges to be filtered
    {0x51000, 0x51FFF, REG_TYPE_TUNING, "Tuning Parameters"},
    {0x49000, 0x49FFF, REG_TYPE_TUNING, "Tuning Tables"},

    // Data regions that need monitoring
    {0x48000, 0x48FFF, REG_TYPE_DATA, "Active Configuration"}
};

#define NUM_RANGES ARRAY_SIZE(isp_ranges)

// Restored original addresses - trace module was working correctly
static struct isp_region isp_regions[] = {
    {
        .phys_addr = 0x10023000,
        .size = 0x1000,
        .name = "isp-w01"
    },
    {
        .phys_addr = 0x13300000,
        .size = 0x100000,
        .name = "isp-m0"
    },
    {
        .phys_addr = 0x133e0000,
        .size = 0x10000,
        .name = "isp-w02"
    },
    {
        .phys_addr = 0x10022000,  // CSI PHY base address
        .size = 0x1000,
        .name = "isp-csi"
    }
};

#define NUM_REGIONS ARRAY_SIZE(isp_regions)

// Get register type based on offset
static enum region_type get_reg_type(u32 offset, const char **desc)
{
    int i;

    for (i = 0; i < NUM_RANGES; i++) {
        if (offset >= isp_ranges[i].start && offset <= isp_ranges[i].end) {
            if (desc)
                *desc = isp_ranges[i].description;
            return isp_ranges[i].type;
        }
    }

    if (desc)
        *desc = "Unknown";
    return REG_TYPE_UNKNOWN;
}

// Check if a write continues a sequence
static bool is_sequential_write(struct seq_write_info *seq, u32 offset, u32 value)
{
    if (!seq->in_progress)
        return false;

    if (offset == seq->start_offset + (seq->count * 4)) {
        // Look for patterns in sequential values
        if (value == seq->last_value + 1 ||
            value == seq->last_value + 0x100 ||
            (value & 0xFFFF0000) == (seq->last_value & 0xFFFF0000)) {
            return true;
        }
    }

    return false;
}

// Start tracking a new sequence
static void start_sequence(struct seq_write_info *seq, u32 offset, u32 value)
{
    seq->start_offset = offset;
    seq->count = 1;
    seq->last_value = value;
    seq->in_progress = true;
}

// End and report a sequence
static void end_sequence(struct seq_write_info *seq, const char *region_name)
{
    if (seq->count > 4) {
        pr_info("ISP %s: Sequential write at 0x%x: %d registers from 0x%x\n",
                region_name, seq->start_offset, seq->count, seq->last_value);
    }
    seq->in_progress = false;
}

// VIC register monitoring function - called from driver code
void trace_vic_registers(void __iomem *vic_regs, const char *context)
{
    u32 reg_0x04, reg_0x0c, reg_0x24;
    unsigned long now = jiffies;
    unsigned long delta_ms = 0;

    if (!vic_regs) {
        printk(KERN_ALERT "*** VIC TRACE: NULL vic_regs pointer from %s ***\n", context);
        return;
    }

    // Read the critical VIC registers
    reg_0x04 = readl(vic_regs + 0x04);  // IMR
    reg_0x0c = readl(vic_regs + 0x0c);  // IMCR
    reg_0x24 = readl(vic_regs + 0x24);  // IMR1

    if (vic_monitor.last_check_time)
        delta_ms = jiffies_to_msecs(now - vic_monitor.last_check_time);

    // Check for changes and log both to printk and trace
    if (reg_0x04 != vic_monitor.last_0x04) {
        printk(KERN_ALERT "*** VIC TRACE [%s]: IMR [0x04] changed: 0x%08x -> 0x%08x (delta: %lu ms) ***\n",
               context, vic_monitor.last_0x04, reg_0x04, delta_ms);
        vic_monitor.last_0x04 = reg_0x04;
    }

    if (reg_0x0c != vic_monitor.last_0x0c) {
        printk(KERN_ALERT "*** VIC TRACE [%s]: IMCR [0x0c] changed: 0x%08x -> 0x%08x (delta: %lu ms) ***\n",
               context, vic_monitor.last_0x0c, reg_0x0c, delta_ms);
        vic_monitor.last_0x0c = reg_0x0c;
    }

    if (reg_0x24 != vic_monitor.last_0x24) {
        printk(KERN_ALERT "*** VIC TRACE [%s]: IMR1 [0x24] changed: 0x%08x -> 0x%08x (delta: %lu ms) ***\n",
               context, vic_monitor.last_0x24, reg_0x24, delta_ms);
        vic_monitor.last_0x24 = reg_0x24;
    }

    vic_monitor.last_check_time = now;
}
EXPORT_SYMBOL(trace_vic_registers);

static void check_region_changes(struct work_struct *work)
{
    struct isp_region *region = container_of(to_delayed_work(work),
                                           struct isp_region,
                                           monitor_work);
    u32 current_val;
    size_t i;
    size_t num_regs = region->size / sizeof(u32);
    unsigned long now = jiffies;
    const char *reg_desc;

    // Check each register in the region
    for (i = 0; i < num_regs; i++) {
        u32 offset = i * sizeof(u32);
        current_val = readl(region->virt_addr + offset);

        if (current_val != region->last_values[i]) {
            enum region_type type = get_reg_type(offset, &reg_desc);
            unsigned long delta_jiffies = 0;

            if (region->last_change_time[i])
                delta_jiffies = now - region->last_change_time[i];

            // Filter out tuning parameter loads and unknown regions
            if (type == REG_TYPE_TUNING || type == REG_TYPE_UNKNOWN) {
                continue;
            }

            // Handle sequential writes for data and VIC
            if (type == REG_TYPE_DATA) {
                if (is_sequential_write(&region->seq_write, offset, current_val)) {
                    region->seq_write.count++;
                    region->seq_write.last_value = current_val;
                } else {
                    if (region->seq_write.in_progress)
                        end_sequence(&region->seq_write, region->name);
                    start_sequence(&region->seq_write, offset, current_val);
                }
            } else {
                // End any ongoing sequence
                if (region->seq_write.in_progress)
                    end_sequence(&region->seq_write, region->name);

                // Log control and VIC register writes with timing
                pr_info("ISP %s: [%s] write at offset 0x%x: 0x%x -> 0x%x (delta: %lu.%03lu ms)\n",
                       region->name, reg_desc, offset,
                       region->last_values[i], current_val,
                       jiffies_to_msecs(delta_jiffies),
                       jiffies_to_usecs(delta_jiffies) % 1000);
            }

            region->last_values[i] = current_val;
            region->last_change_time[i] = now;
        }
    }

    // End any ongoing sequence
    if (region->seq_write.in_progress)
        end_sequence(&region->seq_write, region->name);

    // Reschedule if still monitoring
    if (region->monitoring) {
        schedule_delayed_work(&region->monitor_work, HZ/100); // 10ms interval to catch rapid changes
    }
}

static int init_region(struct isp_region *region)
{
    size_t num_regs, i;

    // Map the region
    region->virt_addr = ioremap(region->phys_addr, region->size);
    if (!region->virt_addr) {
        pr_err("Failed to map ISP region %s at 0x%pap\n",
               region->name, &region->phys_addr);
        return -ENOMEM;
    }

    // Allocate storage for last known values and timestamps
    num_regs = region->size / sizeof(u32);
    region->last_values = kzalloc(num_regs * sizeof(u32), GFP_KERNEL);
    region->last_change_time = kzalloc(num_regs * sizeof(unsigned long), GFP_KERNEL);
    if (!region->last_values || !region->last_change_time) {
        if (region->last_values)
            kfree(region->last_values);
        if (region->last_change_time)
            kfree(region->last_change_time);
        iounmap(region->virt_addr);
        return -ENOMEM;
    }

    // Initialize sequential write tracking
    memset(&region->seq_write, 0, sizeof(region->seq_write));

    // Initialize work for monitoring
    INIT_DELAYED_WORK(&region->monitor_work, check_region_changes);

    // Take initial snapshot of values
    for (i = 0; i < num_regs; i++) {
        region->last_values[i] = readl(region->virt_addr + (i * sizeof(u32)));
        region->last_change_time[i] = 0;
    }

    region->monitoring = true;
    schedule_delayed_work(&region->monitor_work, HZ/100); // Start with 10ms interval

    pr_info("ISP Monitor: initialized region %s at phys 0x%pap size 0x%zx\n",
            region->name, &region->phys_addr, region->size);

    return 0;
}

static void cleanup_region(struct isp_region *region)
{
    if (!region)
        return;

    region->monitoring = false;
    cancel_delayed_work_sync(&region->monitor_work);

    if (region->last_values) {
        kfree(region->last_values);
        region->last_values = NULL;
    }

    if (region->last_change_time) {
        kfree(region->last_change_time);
        region->last_change_time = NULL;
    }

    if (region->virt_addr) {
        iounmap(region->virt_addr);
        region->virt_addr = NULL;
    }
}

// Proc entry for VIC register status
static int vic_regs_show(struct seq_file *m, void *v)
{
    void __iomem *vic_regs = NULL;
    struct isp_region *vic_region = NULL;
    int i;

    // Find the VIC region (isp-w02)
    for (i = 0; i < NUM_REGIONS; i++) {
        if (strcmp(isp_regions[i].name, "isp-w02") == 0) {
            vic_region = &isp_regions[i];
            vic_regs = vic_region->virt_addr;
            break;
        }
    }

    if (!vic_regs) {
        seq_printf(m, "VIC registers not mapped\n");
        return 0;
    }

    seq_printf(m, "VIC Register Status:\n");
    seq_printf(m, "IMR  [0x04]: 0x%08x (last tracked: 0x%08x)\n",
               readl(vic_regs + 0x04), vic_monitor.last_0x04);
    seq_printf(m, "IMCR [0x0c]: 0x%08x (last tracked: 0x%08x)\n",
               readl(vic_regs + 0x0c), vic_monitor.last_0x0c);
    seq_printf(m, "IMR1 [0x24]: 0x%08x (last tracked: 0x%08x)\n",
               readl(vic_regs + 0x24), vic_monitor.last_0x24);
    seq_printf(m, "Last check: %lu jiffies ago\n",
               vic_monitor.last_check_time ? jiffies - vic_monitor.last_check_time : 0);

    return 0;
}

static int vic_regs_open(struct inode *inode, struct file *file)
{
    return single_open(file, vic_regs_show, NULL);
}

static const struct file_operations vic_regs_fops = {
    .open = vic_regs_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};

static int __init isp_monitor_init(void)
{
    int i, ret;

    pr_info("ISP Register Monitor v%s initializing\n", ISP_MONITOR_VERSION);

    for (i = 0; i < NUM_REGIONS; i++) {
        ret = init_region(&isp_regions[i]);
        if (ret) {
            // Cleanup any regions we managed to initialize
            while (--i >= 0)
                cleanup_region(&isp_regions[i]);
            return ret;
        }
    }

    // Create proc entry for VIC register monitoring
    vic_monitor.proc_entry = proc_create("vic_regs", 0444, NULL, &vic_regs_fops);
    if (!vic_monitor.proc_entry) {
        pr_warn("Failed to create /proc/vic_regs entry\n");
    } else {
        pr_info("Created /proc/vic_regs for VIC register monitoring\n");
    }

    return 0;
}

static void __exit isp_monitor_exit(void)
{
    int i;

    // Remove proc entry
    if (vic_monitor.proc_entry) {
        proc_remove(vic_monitor.proc_entry);
        vic_monitor.proc_entry = NULL;
    }

    for (i = 0; i < NUM_REGIONS; i++)
        cleanup_region(&isp_regions[i]);

    pr_info("ISP Register Monitor unloaded\n");
}

module_init(isp_monitor_init);
module_exit(isp_monitor_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Monitors ISP and VIC register changes with smart filtering");
MODULE_VERSION(ISP_MONITOR_VERSION);