#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
#include <linux/jiffies.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>

#define ISP_MONITOR_VERSION "2.0"
#define TRACE_FILE_PATH "/opt/isp-trace.txt"

/* ISP core base: 0x13300000 (isp-m0), size 0x100000.
 * All ISP registers (0x0000-0xFFFF) live within this region.
 * The trace module reads these registers periodically and logs
 * any changes to /opt/isp-trace.txt for OEM vs open-source comparison. */

static void __iomem *isp_base;
static DEFINE_MUTEX(trace_file_mutex);
static struct file *trace_file;
static struct delayed_work trace_work;
static bool tracing_active;

/* Snapshot storage for change detection */
static u32 *reg_snapshot;
static int snapshot_count;

/* Register groups we care about for tuning comparison.
 * Each entry: { start_offset, end_offset, "label" }
 * Offsets are relative to ISP core base 0x13300000. */
struct trace_range {
	u32 start;
	u32 end;
	const char *label;
};

static const struct trace_range trace_ranges[] = {
	/* White Balance gains — the critical path for color */
	{ 0x1800, 0x1850, "WB" },

	/* Color Correction Matrix */
	{ 0x5000, 0x5018, "CCM" },

	/* BCSH (Brightness/Contrast/Saturation/Hue) */
	{ 0x8000, 0x8078, "BCSH" },

	/* AWB stats config + DMA */
	{ 0xb000, 0xb054, "AWB_HW" },

	/* AE0 zone config + DMA */
	{ 0xa000, 0xa054, "AE0" },

	/* AE1 zone config + DMA */
	{ 0xa800, 0xa854, "AE1" },

	/* Gamma */
	{ 0x2000, 0x2028, "GAMMA" },

	/* ISP top control + bypass */
	{ 0x0000, 0x0030, "TOP" },

	/* ISP pipeline control */
	{ 0x0800, 0x0810, "PIPE" },

	/* ISP interrupt status */
	{ 0x00b0, 0x00bc, "IRQ" },

	/* Digital gain (GB block) */
	{ 0x1000, 0x1018, "GB_DGAIN" },

	/* DPC control */
	{ 0x5b80, 0x5b94, "DPC" },

	/* LSC (Lens Shading Correction) */
	{ 0x2800, 0x2830, "LSC" },

	/* SDNS (Spatial Denoise) */
	{ 0x6800, 0x6830, "SDNS" },

	/* MDNS top config */
	{ 0x7808, 0x7818, "MDNS_TOP" },

	/* Sharpen */
	{ 0x6000, 0x6030, "SHARP" },

	/* CFA (demosaic) */
	{ 0x4800, 0x4810, "CFA" },

	/* RDNS AWB gain */
	{ 0x3000, 0x3008, "RDNS_WB" },

	/* Defog / ADR */
	{ 0x4490, 0x44a8, "ADR" },

	/* MSCA (scaler) channel 0 */
	{ 0x9800, 0x9810, "MSCA_CTL" },
	{ 0x9900, 0x9910, "MSCA_CH0" },
	{ 0x9968, 0x9988, "MSCA_FIFO" },
};

#define NUM_TRACE_RANGES ARRAY_SIZE(trace_ranges)

/* Count total registers across all ranges */
static int count_total_regs(void)
{
	int i, total = 0;
	for (i = 0; i < NUM_TRACE_RANGES; i++)
		total += (trace_ranges[i].end - trace_ranges[i].start) / 4 + 1;
	return total;
}

/* Map range index + offset to flat snapshot index */
static int reg_to_snapshot_idx(int range_idx, u32 offset)
{
	int i, idx = 0;
	for (i = 0; i < range_idx; i++)
		idx += (trace_ranges[i].end - trace_ranges[i].start) / 4 + 1;
	idx += (offset - trace_ranges[range_idx].start) / 4;
	return idx;
}

static int open_trace_file(void)
{
	if (trace_file)
		return 0;

	trace_file = filp_open(TRACE_FILE_PATH, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (IS_ERR(trace_file)) {
		int ret = PTR_ERR(trace_file);
		pr_err("isp-trace: failed to open %s: %d\n", TRACE_FILE_PATH, ret);
		trace_file = NULL;
		return ret;
	}
	return 0;
}

static void close_trace_file(void)
{
	if (trace_file && !IS_ERR(trace_file)) {
		filp_close(trace_file, NULL);
		trace_file = NULL;
	}
}

static void trace_write(const char *fmt, ...)
{
	va_list args;
	char buf[256];
	int len;
	mm_segment_t old_fs;

	if (!trace_file)
		return;

	va_start(args, fmt);
	len = vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);

	if (len > 0 && len < sizeof(buf)) {
		mutex_lock(&trace_file_mutex);
		if (trace_file && !IS_ERR(trace_file)) {
			old_fs = get_fs();
			set_fs(KERNEL_DS);
			vfs_write(trace_file, buf, len, &trace_file->f_pos);
			set_fs(old_fs);
		}
		mutex_unlock(&trace_file_mutex);
	}
}

/* Dump full snapshot of all tracked registers (called once at start) */
static void dump_full_snapshot(const char *tag)
{
	int i;
	u32 off;

	trace_write("=== %s SNAPSHOT (jiffies=%lu) ===\n", tag, jiffies);

	for (i = 0; i < NUM_TRACE_RANGES; i++) {
		trace_write("[%s] 0x%04x-0x%04x:\n",
			    trace_ranges[i].label,
			    trace_ranges[i].start,
			    trace_ranges[i].end);
		for (off = trace_ranges[i].start;
		     off <= trace_ranges[i].end; off += 4) {
			u32 val = readl(isp_base + off);
			int idx = reg_to_snapshot_idx(i, off);
			reg_snapshot[idx] = val;
			trace_write("  0x%04x = 0x%08x\n", off, val);
		}
	}
	trace_write("=== END %s ===\n\n", tag);
}

/* Periodic check: log only changes */
static void trace_check_changes(struct work_struct *work)
{
	int i;
	u32 off;
	int changes = 0;

	for (i = 0; i < NUM_TRACE_RANGES; i++) {
		for (off = trace_ranges[i].start;
		     off <= trace_ranges[i].end; off += 4) {
			int idx = reg_to_snapshot_idx(i, off);
			u32 val = readl(isp_base + off);

			if (val != reg_snapshot[idx]) {
				if (changes == 0)
					trace_write("--- CHANGES (jiffies=%lu) ---\n",
						    jiffies);
				trace_write("[%s] 0x%04x: 0x%08x -> 0x%08x\n",
					    trace_ranges[i].label,
					    off, reg_snapshot[idx], val);
				reg_snapshot[idx] = val;
				changes++;
			}
		}
	}

	if (changes > 0)
		trace_write("--- %d register(s) changed ---\n\n", changes);

	if (tracing_active)
		schedule_delayed_work(&trace_work, HZ / 10); /* 100ms */
}

static int __init isp_trace_init(void)
{
	int ret;

	pr_info("ISP Tuning Trace v%s initializing\n", ISP_MONITOR_VERSION);

	isp_base = ioremap(0x13300000, 0x100000);
	if (!isp_base) {
		pr_err("isp-trace: failed to map ISP core\n");
		return -ENOMEM;
	}

	snapshot_count = count_total_regs();
	reg_snapshot = kzalloc(snapshot_count * sizeof(u32), GFP_KERNEL);
	if (!reg_snapshot) {
		iounmap(isp_base);
		return -ENOMEM;
	}

	ret = open_trace_file();
	if (ret) {
		pr_warn("isp-trace: no trace file, using pr_info only\n");
	}

	trace_write("ISP Tuning Trace v%s — %d registers across %d ranges\n",
		    ISP_MONITOR_VERSION, snapshot_count,
		    (int)NUM_TRACE_RANGES);

	/* Initial full dump */
	dump_full_snapshot("INITIAL");

	/* Start periodic change detection */
	INIT_DELAYED_WORK(&trace_work, trace_check_changes);
	tracing_active = true;
	schedule_delayed_work(&trace_work, HZ); /* first check after 1s */

	pr_info("isp-trace: monitoring %d regs, output to %s\n",
		snapshot_count, TRACE_FILE_PATH);
	return 0;
}

static void __exit isp_trace_exit(void)
{
	tracing_active = false;
	cancel_delayed_work_sync(&trace_work);

	/* Final snapshot for comparison */
	if (isp_base && reg_snapshot)
		dump_full_snapshot("FINAL");

	close_trace_file();
	kfree(reg_snapshot);
	if (isp_base)
		iounmap(isp_base);

	pr_info("isp-trace: unloaded\n");
}

module_init(isp_trace_init);
module_exit(isp_trace_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ISP Driver Team");
MODULE_DESCRIPTION("ISP tuning register tracer — logs WB/CCM/BCSH/AWB/AE changes to /opt/isp-trace.txt");
MODULE_VERSION(ISP_MONITOR_VERSION);
