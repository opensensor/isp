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

#define ISP_MONITOR_VERSION "3.0"
#define TRACE_FILE_PATH "/opt/isp-trace.txt"

/* Two MMIO windows:
 *   isp_base : ISP core 0x13300000 size 0x100000  (covers ISP blocks + VIC@0x80000 + MSCA@0x16000)
 *   csi_base : CSI/MIPI 0x10020000 size 0x40000   (mipi-phy 0x10022000, csi1 0x10023000, csi0 0x10054000)
 * The module reads tracked registers periodically and logs any changes to
 * /opt/isp-trace.txt for OEM vs recovered comparison.  Offsets in trace_ranges
 * are relative to the selected base (base_sel: 0=isp_base, 1=csi_base). */

#define BASE_ISP 0
#define BASE_CSI 1

static void __iomem *isp_base;   /* 0x13300000 */
static void __iomem *csi_base;   /* 0x10020000 */
static DEFINE_MUTEX(trace_file_mutex);
static struct file *trace_file;
static struct delayed_work trace_work;
static bool tracing_active;
static bool trace_sequence_mode;
static uint trace_interval_ms = 1000;

static u32 *reg_snapshot;
static int snapshot_count;

struct trace_range {
	u8 base_sel;
	u32 start;
	u32 end;
	const char *label;
};

static const struct trace_range trace_ranges[] = {
	/* ---- FULL ISP block sweep: every Tiziano processing block lives in
	 * 0x0000-0xBFFF (top, GB/dgain, WB, gamma, LSC, RDNS, DPC, CCM, ADR/defog,
	 * CFA/demosaic, sharpen, SDNS, MDNS, BCSH, AE/AWB stats, ...). One dense
	 * sweep so the stock-vs-recovered diff cannot miss the diamond block. ---- */
	{ BASE_ISP, 0x00000, 0x0bffc, "ISPCORE" },

	/* ---- MSCA scaler / output FIFO engine ---- */
	{ BASE_ISP, 0x16000, 0x162fc, "MSCA0" },
	{ BASE_ISP, 0x17000, 0x172fc, "MSCA1" },

	/* ---- VIC (capture/MDMA), base 0x13380000 = isp+0x80000 ---- */
	{ BASE_ISP, 0x80000, 0x80400, "VIC" },

	/* ---- CSI / MIPI PHY (base1 @ 0x10020000) ---- */
	{ BASE_CSI, 0x02000, 0x023fc, "MIPI_PHY" },  /* 0x10022000 dphy timing/lanes */
	{ BASE_CSI, 0x03000, 0x030fc, "CSI1" },      /* 0x10023000 csi1/w01 */
	{ BASE_CSI, 0x34000, 0x341fc, "CSI0" },      /* 0x10054000 csi0 */
};

static const struct trace_range sequence_trace_ranges[] = {
	/* Narrow fast-poll mode for CSI/VIC bring-up ordering. */
	{ BASE_CSI, 0x02000, 0x023fc, "MIPI_PHY_SEQ" },
	{ BASE_CSI, 0x34000, 0x341fc, "CSI0_SEQ" },
	{ BASE_ISP, 0x80000, 0x80200, "VIC_SEQ" },
};

static const struct trace_range *active_trace_ranges = trace_ranges;
static int active_trace_range_count = ARRAY_SIZE(trace_ranges);

module_param_named(sequence_mode, trace_sequence_mode, bool, 0644);
module_param_named(interval_ms, trace_interval_ms, uint, 0644);

static void __iomem *base_for(u8 sel)
{
	return sel == BASE_CSI ? csi_base : isp_base;
}

static int count_total_regs(void)
{
	int i, total = 0;
	for (i = 0; i < active_trace_range_count; i++)
		total += (active_trace_ranges[i].end -
			  active_trace_ranges[i].start) / 4 + 1;
	return total;
}

static int reg_to_snapshot_idx(int range_idx, u32 offset)
{
	int i, idx = 0;
	for (i = 0; i < range_idx; i++)
		idx += (active_trace_ranges[i].end -
			active_trace_ranges[i].start) / 4 + 1;
	idx += (offset - active_trace_ranges[range_idx].start) / 4;
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

static void dump_full_snapshot(const char *tag)
{
	int i;
	u32 off;

	trace_write("=== %s SNAPSHOT (jiffies=%lu) ===\n", tag, jiffies);

	for (i = 0; i < active_trace_range_count; i++) {
		void __iomem *b = base_for(active_trace_ranges[i].base_sel);
		trace_write("[%s] base%u 0x%05x-0x%05x:\n",
			    active_trace_ranges[i].label,
			    active_trace_ranges[i].base_sel,
			    active_trace_ranges[i].start,
			    active_trace_ranges[i].end);
		for (off = active_trace_ranges[i].start;
		     off <= active_trace_ranges[i].end; off += 4) {
			u32 val = readl(b + off);
			int idx = reg_to_snapshot_idx(i, off);
			reg_snapshot[idx] = val;
			trace_write("  0x%05x = 0x%08x\n", off, val);
		}
	}
	trace_write("=== END %s ===\n\n", tag);
}

static void trace_check_changes(struct work_struct *work)
{
	int i;
	u32 off;
	int changes = 0;

	for (i = 0; i < active_trace_range_count; i++) {
		void __iomem *b = base_for(active_trace_ranges[i].base_sel);
		for (off = active_trace_ranges[i].start;
		     off <= active_trace_ranges[i].end; off += 4) {
			int idx = reg_to_snapshot_idx(i, off);
			u32 val = readl(b + off);

			if (val != reg_snapshot[idx]) {
				if (changes == 0)
					trace_write("--- CHANGES (jiffies=%lu) ---\n",
						    jiffies);
				trace_write("[%s] 0x%05x: 0x%08x -> 0x%08x\n",
					    active_trace_ranges[i].label,
					    off, reg_snapshot[idx], val);
				reg_snapshot[idx] = val;
				changes++;
			}
		}
	}

	if (changes > 0)
		trace_write("--- %d register(s) changed ---\n\n", changes);

	if (tracing_active)
		schedule_delayed_work(&trace_work,
				      max(1UL,
					  msecs_to_jiffies(trace_interval_ms)));
}

static int __init isp_trace_init(void)
{
	int ret;

	pr_info("ISP Tuning Trace v%s initializing\n", ISP_MONITOR_VERSION);
	if (trace_sequence_mode) {
		active_trace_ranges = sequence_trace_ranges;
		active_trace_range_count = ARRAY_SIZE(sequence_trace_ranges);
		if (!trace_interval_ms)
			trace_interval_ms = 10;
	} else {
		active_trace_ranges = trace_ranges;
		active_trace_range_count = ARRAY_SIZE(trace_ranges);
		if (!trace_interval_ms)
			trace_interval_ms = 1000;
	}

	isp_base = ioremap(0x13300000, 0x100000);
	if (!isp_base) {
		pr_err("isp-trace: failed to map ISP core\n");
		return -ENOMEM;
	}
	csi_base = ioremap(0x10020000, 0x40000);
	if (!csi_base) {
		pr_err("isp-trace: failed to map CSI region\n");
		iounmap(isp_base);
		return -ENOMEM;
	}

	snapshot_count = count_total_regs();
	reg_snapshot = kzalloc(snapshot_count * sizeof(u32), GFP_KERNEL);
	if (!reg_snapshot) {
		iounmap(csi_base);
		iounmap(isp_base);
		return -ENOMEM;
	}

	ret = open_trace_file();
	if (ret)
		pr_warn("isp-trace: no trace file, using pr_info only\n");

	trace_write("ISP Tuning Trace v%s — %d registers across %d ranges mode=%s interval_ms=%u\n",
		    ISP_MONITOR_VERSION, snapshot_count,
		    active_trace_range_count,
		    trace_sequence_mode ? "sequence" : "wide",
		    trace_interval_ms);

	dump_full_snapshot("INITIAL");

	INIT_DELAYED_WORK(&trace_work, trace_check_changes);
	tracing_active = true;
	schedule_delayed_work(&trace_work,
			      max(1UL, msecs_to_jiffies(trace_interval_ms)));

	pr_info("isp-trace: monitoring %d regs, output to %s\n",
		snapshot_count, TRACE_FILE_PATH);
	return 0;
}

static void __exit isp_trace_exit(void)
{
	tracing_active = false;
	cancel_delayed_work_sync(&trace_work);

	if (isp_base && reg_snapshot)
		dump_full_snapshot("FINAL");

	close_trace_file();
	kfree(reg_snapshot);
	if (csi_base)
		iounmap(csi_base);
	if (isp_base)
		iounmap(isp_base);

	pr_info("isp-trace: unloaded\n");
}

module_init(isp_trace_init);
module_exit(isp_trace_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ISP Driver Team");
MODULE_DESCRIPTION("ISP register tracer — MSCA/VIC/CSI snapshots+changes to /opt/isp-trace.txt");
MODULE_VERSION(ISP_MONITOR_VERSION);
