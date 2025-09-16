#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
#include <linux/jiffies.h>

#define ISP_MONITOR_VERSION "1.3"

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

                // CRITICAL FIX: Root cause fixed - CSI PHY now writes to correct base address
                // CSI PHY writes now go to 0x10022000 (isp-csi) instead of 0x133e0000 (isp-w02)
                // This prevents conflicts with VIC interrupt registers at 0x04, 0x0c, 0x1e8
                if ((offset == 0x04 || offset == 0x0c || offset == 0x1e8) && strcmp(region->name, "isp-w02") == 0) {
                    pr_debug("*** VIC REGISTER CHANGE: Register 0x%x changed (should be rare now) ***\n", offset);
                }
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

    return 0;
}

static void __exit isp_monitor_exit(void)
{
    int i;

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
