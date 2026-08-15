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
#include <linux/math64.h>
#include <linux/kthread.h>
#include <linux/ktime.h>

#ifndef READ_ONCE
#define READ_ONCE(x) (*(volatile typeof(x) *)&(x))
#endif

#define ISP_MONITOR_VERSION "3.20"
#define TRACE_FILE_PATH "/tmp/isp-trace.txt"

/* Two MMIO windows:
 *   isp_base : ISP core 0x13300000 size 0x100000  (covers ISP blocks + VIC@0x80000 + MSCA@0x16000)
 *   csi_base : CSI/MIPI 0x10020000 size 0x40000   (mipi-phy 0x10022000, csi1 0x10023000, csi0 0x10054000)
 * The module reads tracked registers periodically and logs any changes to
 * /opt/isp-trace.txt for OEM vs recovered comparison.  Offsets in trace_ranges
 * are relative to the selected base (base_sel: 0=isp_base, 1=csi_base). */

#define BASE_ISP 0
#define BASE_CSI 1
#define BASE_CPM 2

/* T31's vendor reset helper accesses CPM through the uncached KSEG1 alias.
 * Use the same path here: on the Ingenic 3.10 kernel an ioremap() view of
 * SRBC is not guaranteed to preserve the vendor transaction semantics. */
#define CPM_KSEG1_BASE ((void __iomem *)0xb0000000UL)

static void __iomem *isp_base;   /* 0x13300000 */
static void __iomem *csi_base;   /* 0x10020000 */
static DEFINE_MUTEX(trace_file_mutex);
static struct file *trace_file;
static struct delayed_work trace_work;
static struct task_struct *t31_fast_reset_task;
static bool tracing_active;
static bool trace_sequence_mode;
static bool trace_core_only;
static bool trace_quality_only;
static bool trace_t31_gib_only;
static bool trace_t31_ingress_only;
static bool trace_t31_ae_only;
static bool trace_t31_front_only;
static bool trace_snapshot_only;
static uint trace_interval_ms = 1000;
static unsigned long system_reg_read_addr;
static unsigned long tmo_info_addr;
static unsigned long gamma_info_addr;
static unsigned long ysp_info_addr;
static unsigned long gib_info_addr;
static unsigned long dmsc_info_addr;
static unsigned long lsc_info_addr;
static unsigned long ae_info_addr;
static unsigned long awb_info_addr;
static unsigned long adr_info_addr;
static unsigned long lce_info_addr;
static unsigned long stat_y_out_addr;
static unsigned long msca_info_addr;
static unsigned long mscaler_addr;
static unsigned long msca_hardpar_addr;
static unsigned long memory_addr;
static uint memory_words;
static char *memory_symbol;

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
	/* T41 continues the tuning aperture above the legacy 0xbfff boundary:
	 * gamma/CCM/BCSH/YSP/SDNS and the late color/tone blocks live here. */
	{ BASE_ISP, 0x0c000, 0x15ffc, "ISPCORE_T41_EXT0" },
	{ BASE_ISP, 0x18000, 0x1effc, "ISPCORE_T41_EXT1" },

	/* ---- MSCA scaler / output FIFO engine ---- */
	{ BASE_ISP, 0x16000, 0x162fc, "MSCA0" },
	{ BASE_ISP, 0x17000, 0x172fc, "MSCA1" },
	/* T41 moved MSCA to the final 64 KiB of the ISP aperture. */
	{ BASE_ISP, 0xf0000, 0xf08d0, "MSCA_T41" },

	/* T41 ISP main interrupt/status banks. */
	{ BASE_ISP, 0x40000, 0x400b8, "ISP_IRQ_T41" },

	/* ---- VIC (capture/MDMA), base 0x13380000 = isp+0x80000 ---- */
	{ BASE_ISP, 0x80000, 0x80400, "VIC" },

	/* ---- CSI / MIPI PHY (base1 @ 0x10020000) ---- */
	{ BASE_CSI, 0x02000, 0x023fc, "MIPI_PHY" },  /* 0x10022000 dphy timing/lanes */
	{ BASE_CSI, 0x03000, 0x030fc, "CSI1" },      /* 0x10023000 csi1/w01 */
	{ BASE_CSI, 0x34000, 0x341fc, "CSI0" },      /* 0x10054000 csi0 */
};

static const struct trace_range sequence_trace_ranges[] = {
	/* Narrow T41 comparison mode: start/IRQ/MSCA/VIC without the full ISP
	 * tuning aperture.  Calling the vendor accessor for every tuning register
	 * at video rate can starve the recovered driver's workqueues. */
	{ BASE_ISP, 0x00000, 0x004ff, "ISP_TOP_SEQ" },
	{ BASE_ISP, 0x01000, 0x011ff, "ISP_STREAM_SEQ" },
	{ BASE_ISP, 0x40000, 0x400b8, "ISP_IRQ_T41_SEQ" },
	{ BASE_ISP, 0xf0000, 0xf08d0, "MSCA_T41_SEQ" },
	{ BASE_CSI, 0x02000, 0x023fc, "MIPI_PHY_SEQ" },
	{ BASE_CSI, 0x34000, 0x341fc, "CSI0_SEQ" },
	{ BASE_ISP, 0x80000, 0x80400, "VIC_SEQ" },
};

static const struct trace_range core_trace_ranges[] = {
	/* Tiziano tuning only.  Deliberately excludes T41 MSCA completion FIFO
	 * read ports, whose reads consume queue state. */
	{ BASE_ISP, 0x00000, 0x0bffc, "ISPCORE" },
};

static const struct trace_range quality_trace_ranges[] = {
	/* T41 HLIL live bases/triggers, not the older-family bank map. */
	{ BASE_ISP, 0x00040, 0x00040, "TOP_BYPASS" },
	{ BASE_ISP, 0x08000, 0x08050, "GIB" },
	{ BASE_ISP, 0x0a000, 0x0a3ff, "DMSC" },
	/* T41 tisp_mdns_reg_cfg writes through base+0x2b0.  The earlier
	 * 0x200-byte window silently omitted the final tables, so cover the full
	 * 0x400-byte unit aperture before deriving a replay profile. */
	{ BASE_ISP, 0x0f000, 0x0f3ff, "MDNS" },
	{ BASE_ISP, 0x10000, 0x101ff, "YDNS" },
	{ BASE_ISP, 0x13000, 0x131ff, "YSP" },
	{ BASE_ISP, 0x14000, 0x141ff, "SDNS" },
	{ BASE_ISP, 0x1c000, 0x1c1ff, "CDNS" },
	{ BASE_ISP, 0x1d000, 0x1d3ff, "LCE_DEFOG" },
	/* T41's tone-mapping unit is outside the legacy 0x0000-0xbfff
	 * ISPCORE window.  The stock HLIL PM snapshot covers 0x1e000-0x1e118
	 * (with the documented hole at 0x1e030/0x1e034), so include the whole
	 * readable configuration aperture for matched stock/open evidence. */
	{ BASE_ISP, 0x1e000, 0x1e118, "TMO" },
};

static const struct trace_range t31_gib_trace_ranges[] = {
	/* T31 raw front-end state.  Keep this intentionally narrow so a 1 ms
	 * sampling interval can observe the reset-to-first-frame GIB latch without
	 * delaying ISP bring-up. */
	{ BASE_ISP, 0x00000, 0x00030, "T31_TOP" },
	{ BASE_ISP, 0x01000, 0x01070, "T31_GIB" },
	{ BASE_ISP, 0x02800, 0x02898, "T31_DPC" },
	{ BASE_ISP, 0x80000, 0x8017c, "T31_GIB_DEIR" },
	/* Correlate the GIB gate/commit sequence with the primary VIC run latch.
	 * A single register keeps the 1 ms probe light enough for first-frame
	 * ordering while avoiding the read-sensitive VIC status aperture. */
	{ BASE_ISP, 0xe0000, 0xe0000, "T31_VIC_CTRL" },
};

static const struct trace_range t31_ingress_trace_ranges[] = {
	/* T31 input-latch comparison mode.  The primary VIC is at 0x133e0000,
	 * i.e. ISP base + 0xe0000; the older +0x80000 alias is not the streaming
	 * bank on this SoC.  Keep sparse wrapper registers as separate ranges so
	 * a 1 ms trace does not perturb CSI bring-up. */
	{ BASE_ISP, 0x00000, 0x00030, "T31_TOP" },
	{ BASE_ISP, 0xe0000, 0xe01f4, "T31_VIC" },
	{ BASE_ISP, 0xe03a0, 0xe03e4, "T31_VIC_STATUS" },
	{ BASE_CSI, 0x02000, 0x02044, "T31_MIPI_PHY" },
	{ BASE_CSI, 0x02128, 0x02128, "T31_MIPI_LANES" },
	{ BASE_CSI, 0x02160, 0x02160, "T31_MIPI_RATE0" },
	{ BASE_CSI, 0x021e0, 0x021e0, "T31_MIPI_RATE1" },
	{ BASE_CSI, 0x02260, 0x02260, "T31_MIPI_RATE2" },
	{ BASE_CSI, 0x03000, 0x03040, "T31_CSI_HOST" },
};

static const struct trace_range t31_ae_trace_ranges[] = {
	/* T31 AE0/AE1 control, DMA-address, commit, and active-bank state.
	 * The engines only expose configuration through 0x050; statistics live in
	 * DMA memory and are deliberately not read through the MMIO aperture. */
	{ BASE_ISP, 0x00000, 0x00030, "T31_TOP" },
	{ BASE_ISP, 0x0a000, 0x0a050, "T31_AE0" },
	{ BASE_ISP, 0x0a800, 0x0a850, "T31_AE1" },
};

static const struct trace_range t31_front_trace_ranges[] = {
	/* T31 raw-front-end control/status that differs on SC301IOT while the
	 * readable GIB and AE programming already matches stock.  Keep this
	 * sparse enough for 1 ms boot sampling: 0x100-0x110 and 0x818-0x840 are
	 * the unexplained upstream deltas, while TOP/GIB correlate their latch
	 * transitions with the established reference trace. */
	{ BASE_ISP, 0x00000, 0x00030, "T31_TOP" },
	{ BASE_ISP, 0x00100, 0x00120, "T31_RAW_FRONT0" },
	{ BASE_ISP, 0x00800, 0x00850, "T31_RAW_FRONT1" },
	{ BASE_ISP, 0x01000, 0x01084, "T31_GIB" },
	/* Preserve the relative ordering between the raw-front/GIB latch and the
	 * MIPI receiver.  Open currently performs csi_core_ops_init() after
	 * tisp_init(); stock ordering must be observed directly because all of
	 * these registers converge to the same steady-state values. */
	{ BASE_CSI, 0x02000, 0x02010, "T31_MIPI_PHY" },
	{ BASE_CSI, 0x02128, 0x02128, "T31_MIPI_LANES" },
	{ BASE_CSI, 0x03000, 0x03014, "T31_CSI_HOST" },
	/* Correlate ISP +0x28's reset-state response with the clock gates and
	 * exact SRBC handshake that drives it.  Keep the registers sparse so the
	 * 1 ms startup probe remains non-invasive. */
	{ BASE_CPM, 0x00020, 0x00020, "T31_CPM_CLKGR0" },
	{ BASE_CPM, 0x00028, 0x00028, "T31_CPM_CLKGR1" },
	{ BASE_CPM, 0x00030, 0x00030, "T31_CPM_VPUCDR" },
	{ BASE_CPM, 0x000c4, 0x000c4, "T31_CPM_SRBC" },
};

static const struct trace_range *active_trace_ranges = trace_ranges;
static int active_trace_range_count = ARRAY_SIZE(trace_ranges);

module_param_named(sequence_mode, trace_sequence_mode, bool, 0644);
module_param_named(core_only, trace_core_only, bool, 0644);
module_param_named(quality_only, trace_quality_only, bool, 0644);
module_param_named(t31_gib_only, trace_t31_gib_only, bool, 0644);
module_param_named(t31_ingress_only, trace_t31_ingress_only, bool, 0644);
module_param_named(t31_ae_only, trace_t31_ae_only, bool, 0644);
module_param_named(t31_front_only, trace_t31_front_only, bool, 0644);
module_param_named(snapshot_only, trace_snapshot_only, bool, 0644);
module_param_named(interval_ms, trace_interval_ms, uint, 0644);
module_param_named(system_reg_read_addr, system_reg_read_addr, ulong, 0400);
module_param_named(tmo_info_addr, tmo_info_addr, ulong, 0400);
module_param_named(gamma_info_addr, gamma_info_addr, ulong, 0400);
module_param_named(ysp_info_addr, ysp_info_addr, ulong, 0400);
module_param_named(gib_info_addr, gib_info_addr, ulong, 0400);
module_param_named(dmsc_info_addr, dmsc_info_addr, ulong, 0400);
module_param_named(lsc_info_addr, lsc_info_addr, ulong, 0400);
module_param_named(ae_info_addr, ae_info_addr, ulong, 0400);
module_param_named(awb_info_addr, awb_info_addr, ulong, 0400);
module_param_named(adr_info_addr, adr_info_addr, ulong, 0400);
module_param_named(lce_info_addr, lce_info_addr, ulong, 0400);
module_param_named(stat_y_out_addr, stat_y_out_addr, ulong, 0400);
module_param_named(msca_info_addr, msca_info_addr, ulong, 0400);
module_param_named(mscaler_addr, mscaler_addr, ulong, 0400);
module_param_named(msca_hardpar_addr, msca_hardpar_addr, ulong, 0400);
module_param_named(memory_addr, memory_addr, ulong, 0400);
module_param_named(memory_words, memory_words, uint, 0400);
module_param_named(memory_symbol, memory_symbol, charp, 0400);

static void trace_write(const char *fmt, ...);
static u32 trace_reg_read(u8 base_sel, u32 offset);

static bool trace_kernel_ptr(unsigned long ptr)
{
	return ptr >= 0x80000000UL && ptr <= 0xfffffff0UL;
}

static void dump_tmo_memory(void)
{
	u32 info;
	u32 params;
	u32 shadow;
	u32 runtime;
	u32 dma;
	u32 off;

	if (!trace_kernel_ptr(tmo_info_addr))
		return;
	info = READ_ONCE(*(u32 *)(uintptr_t)tmo_info_addr);
	trace_write("[TMO_MEMORY] tmo_info_symbol=0x%08lx info=0x%08x\n",
		    tmo_info_addr, info);
	if (!trace_kernel_ptr(info))
		return;
	params = READ_ONCE(*(u32 *)(uintptr_t)(info + 0));
	shadow = READ_ONCE(*(u32 *)(uintptr_t)(info + 4));
	runtime = READ_ONCE(*(u32 *)(uintptr_t)(info + 8));
	trace_write("[TMO_MEMORY] params=0x%08x shadow=0x%08x runtime=0x%08x\n",
		    params, shadow, runtime);
	trace_write("[TMO_INFO] 0x000-0x02f:\n");
	for (off = 0; off < 48; off += 4)
		trace_write("  I+0x%03x = 0x%08x\n", off,
			    READ_ONCE(*(u32 *)(uintptr_t)(info + off)));
	dma = READ_ONCE(*(u32 *)(uintptr_t)(info + 20));
	/* TMO's bit-17 interrupt alternates between the two 0x4000-byte halves
	 * of this 0x8000-byte DMA allocation, then splits each interleaved
	 * u16 sum/count pair into statYSum/statYNum.  Capturing the source next
	 * to statYOut lets us recover that transformation from stock evidence. */
	if (trace_kernel_ptr(dma)) {
		trace_write("[TMO_DMA_MEMORY] dma=0x%08x bytes=32768\n", dma);
		for (off = 0; off < 32768; off += 4)
			trace_write("  D+0x%04x = 0x%08x\n", off,
				    READ_ONCE(*(u32 *)(uintptr_t)(dma + off)));
	}
	if (trace_kernel_ptr(shadow)) {
		trace_write("[TMO_SHADOW] 0x000-0x191:\n");
		for (off = 0; off < 402; off += 2)
			trace_write("  S+0x%03x = 0x%04x\n", off,
				    READ_ONCE(*(u16 *)(uintptr_t)(shadow + off)));
	}
	if (trace_kernel_ptr(runtime)) {
		trace_write("[TMO_RUNTIME] 0x000-0x315:\n");
		for (off = 0; off < 790; off += 2)
			trace_write("  R+0x%03x = 0x%04x\n", off,
				    READ_ONCE(*(u16 *)(uintptr_t)(runtime + off)));
	}
}

static void dump_adr_memory(void)
{
	u32 info;
	u32 params;
	u32 curve;
	u32 runtime;
	u32 algo;
	u32 stats;
	u32 off;

	/* T41 tisp_adr_init stores a pointer in adr_info[channel].  Channel 0's
	 * 36-byte descriptor owns the tuning parameters and the 800/1140/10056
	 * byte workspaces used by func_adr_reg_write_every().  Capturing these
	 * objects is the only reliable oracle for the write-only 0x50304 port. */
	if (!trace_kernel_ptr(adr_info_addr))
		return;
	info = READ_ONCE(*(u32 *)(uintptr_t)adr_info_addr);
	trace_write("[ADR_MEMORY] adr_info_symbol=0x%08lx info=0x%08x\n",
		    adr_info_addr, info);
	if (!trace_kernel_ptr(info))
		return;
	params = READ_ONCE(*(u32 *)(uintptr_t)(info + 0));
	curve = READ_ONCE(*(u32 *)(uintptr_t)(info + 4));
	runtime = READ_ONCE(*(u32 *)(uintptr_t)(info + 8));
	algo = READ_ONCE(*(u32 *)(uintptr_t)(info + 12));
	stats = READ_ONCE(*(u32 *)(uintptr_t)(info + 24));
	trace_write("[ADR_MEMORY] params=0x%08x curve=0x%08x runtime=0x%08x algo=0x%08x stats=0x%08x\n",
		    params, curve, runtime, algo, stats);
	trace_write("[ADR_INFO] 0x000-0x023:\n");
	for (off = 0; off < 36; off += 4)
		trace_write("  I+0x%03x = 0x%08x\n", off,
			    READ_ONCE(*(u32 *)(uintptr_t)(info + off)));
	if (trace_kernel_ptr(params)) {
		trace_write("[ADR_PARAMS] 0x000-0x1a3:\n");
		for (off = 0; off < 420; off += 2)
			trace_write("  P+0x%03x = 0x%04x\n", off,
				    READ_ONCE(*(u16 *)(uintptr_t)(params + off)));
	}
	if (trace_kernel_ptr(curve)) {
		trace_write("[ADR_CURVE] 0x000-0x31f:\n");
		for (off = 0; off < 800; off += 2)
			trace_write("  C+0x%03x = 0x%04x\n", off,
				    READ_ONCE(*(u16 *)(uintptr_t)(curve + off)));
	}
	if (trace_kernel_ptr(runtime)) {
		trace_write("[ADR_RUNTIME] 0x000-0x473:\n");
		for (off = 0; off < 1140; off += 2)
			trace_write("  R+0x%03x = 0x%04x\n", off,
				    READ_ONCE(*(u16 *)(uintptr_t)(runtime + off)));
	}
	if (trace_kernel_ptr(algo)) {
		trace_write("[ADR_ALGO] 0x000-0x2747:\n");
		for (off = 0; off < 10056; off += 4)
			trace_write("  A+0x%04x = 0x%08x\n", off,
				    READ_ONCE(*(u32 *)(uintptr_t)(algo + off)));
	}
	if (trace_kernel_ptr(stats)) {
		trace_write("[ADR_STATS] 0x0000-0x3fff:\n");
		for (off = 0; off < 16384; off += 4)
			trace_write("  S+0x%04x = 0x%08x\n", off,
				    READ_ONCE(*(u32 *)(uintptr_t)(stats + off)));
	}
}

static void dump_lce_memory(void)
{
	u32 state;
	u32 off;

	/* T41 owns one 22,836-byte LCE object.  Its visible 0x0e000 register
	 * image is insufficient: the interrupt path fills the histogram region,
	 * the event worker derives two 5,760-byte curve banks, and only then does
	 * tisp_lce_write_all_reg() feed the write-only 0x501e0 port.  Preserve the
	 * complete stock object so those regions and their geometry can be
	 * reconstructed from evidence rather than inferred from MMIO. */
	if (!trace_kernel_ptr(lce_info_addr))
		return;
	state = READ_ONCE(*(u32 *)(uintptr_t)lce_info_addr);
	trace_write("[LCE_MEMORY] lce_info_symbol=0x%08lx state=0x%08x bytes=22836\n",
		    lce_info_addr, state);
	if (!trace_kernel_ptr(state))
		return;
	for (off = 0; off < 22836; off += 4)
		trace_write("  L+0x%04x = 0x%08x\n", off,
			    READ_ONCE(*(u32 *)(uintptr_t)(state + off)));
}

static void dump_tmo_stat_memory(void)
{
	u32 stats;
	u32 off;

	/* Exact tisp_tmo_ram_reg_refresh() consumes two 7500-byte groups, each
	 * with five 1500-byte components, from the 15000-byte statYOut buffer.
	 * Capture the full source image so the write-only 0x50260 RAM
	 * transaction can be compared and replayed rather than guessed. */
	if (!trace_kernel_ptr(stat_y_out_addr))
		return;
	stats = READ_ONCE(*(u32 *)(uintptr_t)stat_y_out_addr);
	trace_write("[TMO_STAT_MEMORY] statYOut_symbol=0x%08lx stats=0x%08x bytes=15000\n",
		    stat_y_out_addr, stats);
	if (!trace_kernel_ptr(stats))
		return;
	for (off = 0; off < 15000; off += 4)
		trace_write("  Y+0x%04x = 0x%08x\n", off,
			    READ_ONCE(*(u32 *)(uintptr_t)(stats + off)));
}

static void dump_gamma_memory(void)
{
	u32 info;
	u32 off;

	if (!trace_kernel_ptr(gamma_info_addr))
		return;
	info = READ_ONCE(*(u32 *)(uintptr_t)gamma_info_addr);
	trace_write("[GAMMA_MEMORY] gamma_info_symbol=0x%08lx info=0x%08x\n",
		    gamma_info_addr, info);
	if (!trace_kernel_ptr(info))
		return;
	trace_write("[GAMMA_INFO] 0x000-0x423:\n");
	for (off = 0; off < 1060; off += 2)
		trace_write("  G+0x%03x = 0x%04x\n", off,
			    READ_ONCE(*(u16 *)(uintptr_t)(info + off)));
}

static void dump_ysp_memory(void)
{
	u32 info;
	u32 params;
	u32 runtime;
	u32 off;

	if (!trace_kernel_ptr(ysp_info_addr))
		return;
	info = READ_ONCE(*(u32 *)(uintptr_t)ysp_info_addr);
	trace_write("[YSP_MEMORY] ysp_info_symbol=0x%08lx info=0x%08x\n",
		    ysp_info_addr, info);
	if (!trace_kernel_ptr(info))
		return;
	params = READ_ONCE(*(u32 *)(uintptr_t)(info + 0));
	runtime = READ_ONCE(*(u32 *)(uintptr_t)(info + 4));
	trace_write("[YSP_MEMORY] params=0x%08x runtime=0x%08x last_ev=0x%08x\n",
		    params, runtime,
		    READ_ONCE(*(u32 *)(uintptr_t)(info + 8)));
	trace_write("[YSP_INFO] 0x000-0x00f:\n");
	for (off = 0; off < 16; off += 4)
		trace_write("  I+0x%03x = 0x%08x\n", off,
			    READ_ONCE(*(u32 *)(uintptr_t)(info + off)));
	if (trace_kernel_ptr(runtime)) {
		trace_write("[YSP_RUNTIME] 0x000-0x111:\n");
		for (off = 0; off < 274; off += 2)
			trace_write("  R+0x%03x = 0x%04x\n", off,
				    READ_ONCE(*(u16 *)(uintptr_t)(runtime + off)));
	}
}

static void dump_gib_memory(void)
{
	u32 info;
	u32 workspace;
	u32 off;

	if (!trace_kernel_ptr(gib_info_addr))
		return;
	info = READ_ONCE(*(u32 *)(uintptr_t)gib_info_addr);
	trace_write("[GIB_MEMORY] gib_info_symbol=0x%08lx info=0x%08x\n",
		    gib_info_addr, info);
	if (!trace_kernel_ptr(info))
		return;
	workspace = READ_ONCE(*(u32 *)(uintptr_t)(info + 4));
	trace_write("[GIB_INFO] 0x000-0x02b workspace=0x%08x:\n", workspace);
	for (off = 0; off < 44; off += 4)
		trace_write("  I+0x%03x = 0x%08x\n", off,
			    READ_ONCE(*(u32 *)(uintptr_t)(info + off)));
	if (trace_kernel_ptr(workspace)) {
		trace_write("[GIB_WORKSPACE] 0x000-0x0e3:\n");
		for (off = 0; off < 228; off += 2)
			trace_write("  W+0x%03x = 0x%04x\n", off,
				    READ_ONCE(*(u16 *)(uintptr_t)(workspace + off)));
	}
}

static void dump_dmsc_memory(void)
{
	u32 info;
	u32 runtime;
	u32 off;

	if (!trace_kernel_ptr(dmsc_info_addr))
		return;
	info = READ_ONCE(*(u32 *)(uintptr_t)dmsc_info_addr);
	trace_write("[DMSC_MEMORY] dmsc_info_symbol=0x%08lx info=0x%08x\n",
		    dmsc_info_addr, info);
	if (!trace_kernel_ptr(info))
		return;
	runtime = READ_ONCE(*(u32 *)(uintptr_t)(info + 4));
	trace_write("[DMSC_INFO] runtime=0x%08x last_ev=0x%08x sharp=%u\n",
		    runtime, READ_ONCE(*(u32 *)(uintptr_t)(info + 8)),
		    READ_ONCE(*(u32 *)(uintptr_t)(info + 16)));
	if (trace_kernel_ptr(runtime)) {
		trace_write("[DMSC_RUNTIME] 0x000-0x179:\n");
		for (off = 0; off < 378; off += 2)
			trace_write("  R+0x%03x = 0x%04x\n", off,
				    READ_ONCE(*(u16 *)(uintptr_t)(runtime + off)));
	}
}

static void dump_lsc_memory(void)
{
	u32 info;
	u32 params;
	u32 count;
	u32 off;

	if (!trace_kernel_ptr(lsc_info_addr))
		return;
	info = READ_ONCE(*(u32 *)(uintptr_t)lsc_info_addr);
	trace_write("[LSC_MEMORY] lsc_info_symbol=0x%08lx info=0x%08x\n",
		    lsc_info_addr, info);
	if (!trace_kernel_ptr(info))
		return;
	params = READ_ONCE(*(u32 *)(uintptr_t)(info + 0));
	trace_write("[LSC_INFO] params=0x%08x size=%u/%u ct=%u gain=0x%08x "
		    "gain-byte=%02x/%02x flags=%02x/%02x/%02x/%02x/%02x/%02x/%02x\n",
		    params,
		    READ_ONCE(*(u32 *)(uintptr_t)(info + 4)),
		    READ_ONCE(*(u32 *)(uintptr_t)(info + 8)),
		    READ_ONCE(*(u32 *)(uintptr_t)(info + 12)),
		    READ_ONCE(*(u32 *)(uintptr_t)(info + 24)),
		    READ_ONCE(*(u8 *)(uintptr_t)(info + 27689)),
		    READ_ONCE(*(u8 *)(uintptr_t)(info + 27690)),
		    READ_ONCE(*(u8 *)(uintptr_t)(info + 27692)),
		    READ_ONCE(*(u8 *)(uintptr_t)(info + 27693)),
		    READ_ONCE(*(u8 *)(uintptr_t)(info + 27694)),
		    READ_ONCE(*(u8 *)(uintptr_t)(info + 27695)),
		    READ_ONCE(*(u8 *)(uintptr_t)(info + 27696)),
		    READ_ONCE(*(u8 *)(uintptr_t)(info + 27697)),
		    READ_ONCE(*(u8 *)(uintptr_t)(info + 27698)));
	if (!trace_kernel_ptr(params))
		return;
	if (READ_ONCE(*(u8 *)(uintptr_t)(params + 26)))
		count = READ_ONCE(*(u8 *)(uintptr_t)(info + 27688)) ?
			0x480 : 0x240;
	else
		count = READ_ONCE(*(u16 *)(uintptr_t)(params + 24));
	if (!count || count > 0x480)
		return;
	trace_write("[LSC_PARAMS] bp=%u/%u/%u/%u count=%u mode=%u "
		    "mesh-flags=%02x/%02x/%02x,%02x/%02x/%02x,%02x/%02x/%02x\n",
		    READ_ONCE(*(u16 *)(uintptr_t)(params + 0x10)),
		    READ_ONCE(*(u16 *)(uintptr_t)(params + 0x12)),
		    READ_ONCE(*(u16 *)(uintptr_t)(params + 0x14)),
		    READ_ONCE(*(u16 *)(uintptr_t)(params + 0x16)),
		    READ_ONCE(*(u16 *)(uintptr_t)(params + 0x18)),
		    READ_ONCE(*(u8 *)(uintptr_t)(params + 0x1a)),
		    READ_ONCE(*(u8 *)(uintptr_t)(params + 0x53)),
		    READ_ONCE(*(u8 *)(uintptr_t)(params + 0x54)),
		    READ_ONCE(*(u8 *)(uintptr_t)(params + 0x55)),
		    READ_ONCE(*(u8 *)(uintptr_t)(params + 0x57)),
		    READ_ONCE(*(u8 *)(uintptr_t)(params + 0x58)),
		    READ_ONCE(*(u8 *)(uintptr_t)(params + 0x59)),
		    READ_ONCE(*(u8 *)(uintptr_t)(params + 0x5b)),
		    READ_ONCE(*(u8 *)(uintptr_t)(params + 0x5c)),
		    READ_ONCE(*(u8 *)(uintptr_t)(params + 0x5d)));
	trace_write("[LSC_LUT] packed entries=%u bytes=%u:\n", count,
		    count * 12);
	for (off = 0; off < count * 12; off += 4)
		trace_write("  L+0x%04x = 0x%08x\n", off,
			    READ_ONCE(*(u32 *)(uintptr_t)(info + 32 + off)));
}

static void dump_ae_memory(void)
{
	u32 info;
	u32 params;
	u32 state;
	u32 off;
	unsigned int bank;

	if (!trace_kernel_ptr(ae_info_addr))
		return;
	info = READ_ONCE(*(u32 *)(uintptr_t)ae_info_addr);
	trace_write("[AE_MEMORY] ae_info_symbol=0x%08lx info=0x%08x\n",
		    ae_info_addr, info);
	if (!trace_kernel_ptr(info))
		return;
	params = READ_ONCE(*(u32 *)(uintptr_t)(info + 0));
	state = READ_ONCE(*(u32 *)(uintptr_t)(info + 4));
	trace_write("[AE_INFO] params=0x%08x state=0x%08x aux=0x%08x mode=%u\n",
		    params, state,
		    READ_ONCE(*(u32 *)(uintptr_t)(info + 8)),
		    READ_ONCE(*(u32 *)(uintptr_t)(info + 12)));
	if (!trace_kernel_ptr(state))
		return;

	/* tisp_ae_get_hist() copies two 256-bin, 21-bit histograms into the
	 * first 0x800 bytes of the AE state.  Preserve the individual nonzero
	 * bins as well as compact moments so stock/open target selection can be
	 * based on the same statistics consumed by the vendor AE algorithm. */
	for (bank = 0; bank < 2; ++bank) {
		u64 samples = 0;
		u64 weighted = 0;
		u32 nonzero = 0;

		for (off = 0; off < 256; ++off) {
			u32 value = READ_ONCE(*(u32 *)(uintptr_t)
				(state + bank * 0x400 + off * 4)) & 0x1fffff;

			if (!value)
				continue;
			samples += value;
			weighted += (u64)value * off;
			nonzero++;
			trace_write("  AE%u[%03u] = %u\n", bank, off, value);
		}
		trace_write("[AE_HIST%u] samples=%llu weighted=%llu mean_q8=%llu nonzero=%u\n",
			    bank, (unsigned long long)samples,
			    (unsigned long long)weighted,
			    (unsigned long long)(samples ?
				div64_u64(weighted << 8, samples) : 0), nonzero);
	}
}

static void dump_awb_memory(void)
{
	u32 info;
	u32 params;
	u32 algo;
	u32 dma;
	u32 bank_reg;
	u32 bank_size;
	unsigned int bank;

	if (!trace_kernel_ptr(awb_info_addr))
		return;
	info = READ_ONCE(*(u32 *)(uintptr_t)awb_info_addr);
	trace_write("[AWB_MEMORY] awb_info_symbol=0x%08lx info=0x%08x\n",
		    awb_info_addr, info);
	if (!trace_kernel_ptr(info))
		return;

	params = READ_ONCE(*(u32 *)(uintptr_t)(info + 0));
	algo = READ_ONCE(*(u32 *)(uintptr_t)(info + 4));
	dma = READ_ONCE(*(u32 *)(uintptr_t)(info + 20));
	if (!trace_kernel_ptr(params) || !trace_kernel_ptr(dma)) {
		trace_write("[AWB_MEMORY] params=0x%08x algo=0x%08x dma=0x%08x invalid\n",
			    params, algo, dma);
		return;
	}

	/* Exact T41 tisp_awb_main_interrupt_static derives each of four DMA
	 * banks from params[3274]: normal linear mode is 0x8000 bytes.  Summarize
	 * the packed 16-byte records at their 128-byte group stride using the
	 * same field extraction proven on T40 before changing the open driver. */
	bank_size = ((8U - 6U * READ_ONCE(*(u8 *)(uintptr_t)
		(params + 3274))) & 0xffU) << 12;
	bank_reg = trace_reg_read(BASE_ISP, 0x18050U);
	trace_write("[AWB_MEMORY] params=0x%08x algo=0x%08x dma=0x%08x bank_size=%u bank_reg=0x%08x\n",
		    params, algo, dma, bank_size, bank_reg);
	if (!bank_size || bank_size > 0x8000U)
		return;

	for (bank = 0; bank < 4; ++bank) {
		u64 red = 0;
		u64 green = 0;
		u64 blue = 0;
		u64 pixels = 0;
		u32 zones = 0;
		u32 off;
		u32 raw[4];
		u32 base = dma + bank * bank_size;

		if (!trace_kernel_ptr(base) ||
		    !trace_kernel_ptr(base + bank_size - 1U))
			continue;
		raw[0] = READ_ONCE(*(u32 *)(uintptr_t)(base + 0));
		raw[1] = READ_ONCE(*(u32 *)(uintptr_t)(base + 4));
		raw[2] = READ_ONCE(*(u32 *)(uintptr_t)(base + 8));
		raw[3] = READ_ONCE(*(u32 *)(uintptr_t)(base + 12));

		for (off = 0; off + 16 <= bank_size; off += 128) {
			u32 w0 = READ_ONCE(*(u32 *)(uintptr_t)(base + off));
			u32 w1 = READ_ONCE(*(u32 *)(uintptr_t)(base + off + 4));
			u32 w2 = READ_ONCE(*(u32 *)(uintptr_t)(base + off + 8));
			u32 w3 = READ_ONCE(*(u32 *)(uintptr_t)(base + off + 12));
			u32 count = ((w3 & 0x3fU) << 8) | (w2 >> 24);

			if (!count)
				continue;
			red += w0 & 0x3fffffU;
			green += ((w1 & 0xfffU) << 10) | (w0 >> 22);
			blue += ((w2 & 3U) << 20) | (w1 >> 12);
			pixels += count;
			zones++;
		}
		trace_write("[AWB_BANK%u] zones=%u pixels=%llu rgb=%llu/%llu/%llu ratio_q10=0x%llx/0x%llx raw=%08x/%08x/%08x/%08x%s\n",
			    bank, zones, (unsigned long long)pixels,
			    (unsigned long long)red, (unsigned long long)green,
			    (unsigned long long)blue,
			    (unsigned long long)(red ?
				div64_u64(green << 10, red) : 0),
			    (unsigned long long)(blue ?
				div64_u64(green << 10, blue) : 0),
			    raw[0], raw[1], raw[2], raw[3],
			    bank == (bank_reg & 3U) ? " selected" : "");
	}
}

static void dump_msca_memory(void)
{
	u32 workspace;
	u32 params;
	u32 level_params;
	u32 descriptor;
	u32 off;
	unsigned int channel;

	if (!trace_kernel_ptr(msca_info_addr))
		return;
	workspace = READ_ONCE(*(u32 *)(uintptr_t)msca_info_addr);
	trace_write("[MSCA_MEMORY] msca_info_symbol=0x%08lx workspace=0x%08x\n",
		    msca_info_addr, workspace);
	if (!trace_kernel_ptr(workspace))
		return;
	params = READ_ONCE(*(u32 *)(uintptr_t)(workspace + 0));
	level_params = READ_ONCE(*(u32 *)(uintptr_t)(workspace + 4));
	trace_write("[MSCA_MEMORY] params=0x%08x level_params=0x%08x\n",
		    params, level_params);
	trace_write("[MSCA_WORKSPACE] 0x000-0x513:\n");
	for (off = 0; off < 0x514; off += 2)
		trace_write("  W+0x%03x = 0x%04x\n", off,
			    READ_ONCE(*(u16 *)(uintptr_t)(workspace + off)));
	if (trace_kernel_ptr(params)) {
		trace_write("[MSCA_PARAMS] 0x000-0x0e7:\n");
		for (off = 0; off < 0xe8; off += 2)
			trace_write("  P+0x%03x = 0x%04x\n", off,
				    READ_ONCE(*(u16 *)(uintptr_t)(params + off)));
	}
	if (trace_kernel_ptr(level_params)) {
		trace_write("[MSCA_LEVEL_PARAMS] 0x000-0x1c3:\n");
		for (off = 0; off < 0x1c4; off += 2)
			trace_write("  L+0x%03x = 0x%04x\n", off,
				    READ_ONCE(*(u16 *)(uintptr_t)(level_params + off)));
	}
	if (trace_kernel_ptr(mscaler_addr)) {
		trace_write("[MSCALER_STATE] symbol=0x%08lx flags=%08x/%08x/%08x:\n",
			    mscaler_addr,
			    READ_ONCE(*(u32 *)(uintptr_t)(mscaler_addr + 0xa54)),
			    READ_ONCE(*(u32 *)(uintptr_t)(mscaler_addr + 0xa58)),
			    READ_ONCE(*(u32 *)(uintptr_t)(mscaler_addr + 0xa5c)));
		for (channel = 0; channel < 3; ++channel) {
			u32 base = channel * 0x264;

			trace_write("[MSCALER_CH%u] +0x%03x-0x%03x:\n",
				    channel, base, base + 0x27);
			for (off = 0; off < 0x28; off += 4)
				trace_write("  C%u+0x%03x = 0x%08x\n", channel,
					    off, READ_ONCE(*(u32 *)(uintptr_t)
					    (mscaler_addr + base + off)));
		}
	}
	if (trace_kernel_ptr(msca_hardpar_addr)) {
		trace_write("[MSCA_HARDPAR] symbol=0x%08lx:\n", msca_hardpar_addr);
		for (channel = 0; channel < 3; ++channel) {
			descriptor = READ_ONCE(*(u32 *)(uintptr_t)
					       (msca_hardpar_addr + channel * 4));
			trace_write("  channel=%u descriptor=0x%08x\n",
				    channel, descriptor);
			if (!trace_kernel_ptr(descriptor))
				continue;
			for (off = 0; off < 0x1a; off += 2)
				trace_write("  D%u+0x%02x = 0x%04x\n", channel, off,
					    READ_ONCE(*(u16 *)(uintptr_t)
					    (descriptor + off)));
		}
	}
}

static void __iomem *base_for(u8 sel)
{
	if (sel == BASE_CSI)
		return csi_base;
	if (sel == BASE_CPM)
		return CPM_KSEG1_BASE;
	return isp_base;
}

static u32 trace_reg_read(u8 base_sel, u32 offset)
{
	if (base_sel == BASE_ISP && system_reg_read_addr) {
		u32 (*vendor_read)(u32) =
			(void *)(uintptr_t)system_reg_read_addr;

		return vendor_read(offset);
	}

	return readl(base_for(base_sel) + offset);
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

static int t31_fast_reset_probe(void *unused)
{
	s64 start_ns = ktime_to_ns(ktime_get());
	s64 now_ns;
	u32 top = readl(isp_base + 0x28);
	u32 clkgr0 = readl(CPM_KSEG1_BASE + 0x20);
	u32 clkgr1 = readl(CPM_KSEG1_BASE + 0x28);
	u32 srbc = readl(CPM_KSEG1_BASE + 0xc4);
	u32 last_top = top;
	u32 last_clkgr0 = clkgr0;
	u32 last_clkgr1 = clkgr1;
	u32 last_srbc = srbc;

	/* delayed_work is quantized to 10 ms on the OEM CONFIG_HZ=100 kernel,
	 * which can hide the short 1->4->2->1 reset response.  For the first four
	 * seconds only, sample the four relevant words at sub-millisecond cadence.
	 * The probe is read-only and sleeps between samples. */
	trace_write("[T31_FAST_RESET] +0 us top=%08x clkgr0=%08x clkgr1=%08x srbc=%08x\n",
		    top, clkgr0, clkgr1, srbc);

	while (!kthread_should_stop()) {
		now_ns = ktime_to_ns(ktime_get());
		if (now_ns - start_ns >= 4000000000LL)
			break;

		top = readl(isp_base + 0x28);
		clkgr0 = readl(CPM_KSEG1_BASE + 0x20);
		clkgr1 = readl(CPM_KSEG1_BASE + 0x28);
		srbc = readl(CPM_KSEG1_BASE + 0xc4);
		if (top != last_top || clkgr0 != last_clkgr0 ||
		    clkgr1 != last_clkgr1 || srbc != last_srbc) {
			trace_write("[T31_FAST_RESET] +%lld us top=%08x clkgr0=%08x clkgr1=%08x srbc=%08x\n",
				    (long long)div_s64(now_ns - start_ns, 1000),
				    top, clkgr0, clkgr1, srbc);
			last_top = top;
			last_clkgr0 = clkgr0;
			last_clkgr1 = clkgr1;
			last_srbc = srbc;
		}

		usleep_range(100, 200);
	}

	trace_write("[T31_FAST_RESET] done\n");
	return 0;
}

static void dump_memory_range(void)
{
	u32 off;
	u32 words = min(memory_words, 1024U);

	if (!words || !trace_kernel_ptr(memory_addr) ||
	    !trace_kernel_ptr(memory_addr + words * sizeof(u32) - 1U))
		return;

	trace_write("[MEMORY] addr=0x%08lx words=%u:\n", memory_addr, words);
	for (off = 0; off < words; ++off)
		trace_write("  M+0x%04x = 0x%08x\n", off * 4U,
			    READ_ONCE(*(u32 *)(uintptr_t)(memory_addr + off * 4U)));
}

static void dump_full_snapshot(const char *tag)
{
	int i;
	u32 off;

	trace_write("=== %s SNAPSHOT (jiffies=%lu) ===\n", tag, jiffies);

	for (i = 0; i < active_trace_range_count; i++) {
		trace_write("[%s] base%u 0x%05x-0x%05x:\n",
			    active_trace_ranges[i].label,
			    active_trace_ranges[i].base_sel,
			    active_trace_ranges[i].start,
			    active_trace_ranges[i].end);
		for (off = active_trace_ranges[i].start;
		     off <= active_trace_ranges[i].end; off += 4) {
			u32 val = trace_reg_read(
				active_trace_ranges[i].base_sel, off);
			int idx = reg_to_snapshot_idx(i, off);
			reg_snapshot[idx] = val;
			trace_write("  0x%05x = 0x%08x\n", off, val);
		}
	}
	dump_tmo_memory();
	dump_adr_memory();
	dump_lce_memory();
	dump_tmo_stat_memory();
	dump_gamma_memory();
	dump_ysp_memory();
	dump_gib_memory();
	dump_dmsc_memory();
	dump_lsc_memory();
	dump_ae_memory();
	dump_awb_memory();
	dump_msca_memory();
	dump_memory_range();
	trace_write("=== END %s ===\n\n", tag);
}

static void trace_check_changes(struct work_struct *work)
{
	int i;
	u32 off;
	int changes = 0;

	if (trace_snapshot_only) {
		dump_full_snapshot("CAPTURE");
		tracing_active = false;
		return;
	}

	for (i = 0; i < active_trace_range_count; i++) {
		for (off = active_trace_ranges[i].start;
		     off <= active_trace_ranges[i].end; off += 4) {
			int idx = reg_to_snapshot_idx(i, off);
			u32 val = trace_reg_read(
				active_trace_ranges[i].base_sel, off);

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
	void *symbol_addr;

	pr_info("ISP Tuning Trace v%s initializing vendor_read=%#lx\n",
		ISP_MONITOR_VERSION, system_reg_read_addr);
	if (memory_symbol && memory_symbol[0]) {
		symbol_addr = __symbol_get(memory_symbol);
		if (!symbol_addr) {
			pr_warn("isp-trace: exported symbol %s was not found\n",
				memory_symbol);
		} else {
			memory_addr = (unsigned long)symbol_addr;
			pr_info("isp-trace: resolved %s at 0x%08lx\n",
				memory_symbol, memory_addr);
			__symbol_put(memory_symbol);
		}
	}
	if (trace_t31_front_only) {
		active_trace_ranges = t31_front_trace_ranges;
		active_trace_range_count = ARRAY_SIZE(t31_front_trace_ranges);
		if (!trace_interval_ms)
			trace_interval_ms = 1;
	} else if (trace_t31_ae_only) {
		active_trace_ranges = t31_ae_trace_ranges;
		active_trace_range_count = ARRAY_SIZE(t31_ae_trace_ranges);
		if (!trace_interval_ms)
			trace_interval_ms = 1;
	} else if (trace_t31_ingress_only) {
		active_trace_ranges = t31_ingress_trace_ranges;
		active_trace_range_count = ARRAY_SIZE(t31_ingress_trace_ranges);
		if (!trace_interval_ms)
			trace_interval_ms = 1;
	} else if (trace_t31_gib_only) {
		active_trace_ranges = t31_gib_trace_ranges;
		active_trace_range_count = ARRAY_SIZE(t31_gib_trace_ranges);
		if (!trace_interval_ms)
			trace_interval_ms = 1;
	} else if (trace_quality_only) {
		active_trace_ranges = quality_trace_ranges;
		active_trace_range_count = ARRAY_SIZE(quality_trace_ranges);
		if (!trace_interval_ms)
			trace_interval_ms = 1000;
	} else if (trace_core_only) {
		active_trace_ranges = core_trace_ranges;
		active_trace_range_count = ARRAY_SIZE(core_trace_ranges);
		if (!trace_interval_ms)
			trace_interval_ms = 1000;
	} else if (trace_sequence_mode) {
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

	/* The T41 vendor wrapper uses __ioremap(..., 1024); the generic ioremap
	 * mapping reads as zero on this Ingenic kernel. */
	isp_base = __ioremap(0x13300000, 0x100000, 1024);
	if (!isp_base) {
		pr_err("isp-trace: failed to map ISP core\n");
		return -ENOMEM;
	}
	csi_base = __ioremap(0x10020000, 0x40000, 1024);
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
		    trace_t31_front_only ? "t31-front" :
		    (trace_t31_ae_only ? "t31-ae" :
		    (trace_t31_ingress_only ? "t31-ingress" :
		    (trace_t31_gib_only ? "t31-gib" :
		    (trace_quality_only ? "quality" :
		    (trace_core_only ? "core" :
		    (trace_sequence_mode ? "sequence" : "wide")))))),
		    trace_interval_ms);

	if (!trace_snapshot_only)
		dump_full_snapshot("INITIAL");

	if (trace_t31_front_only) {
		t31_fast_reset_task = kthread_run(t31_fast_reset_probe, NULL,
						  "t31-fast-reset");
		if (IS_ERR(t31_fast_reset_task)) {
			pr_warn("isp-trace: failed to start T31 fast reset probe: %ld\n",
				PTR_ERR(t31_fast_reset_task));
			t31_fast_reset_task = NULL;
		}
	}

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
	if (t31_fast_reset_task) {
		kthread_stop(t31_fast_reset_task);
		t31_fast_reset_task = NULL;
	}
	cancel_delayed_work_sync(&trace_work);

	if (!trace_snapshot_only && isp_base && reg_snapshot)
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
MODULE_DESCRIPTION("ISP register tracer — MSCA/VIC/CSI snapshots+changes to RAM-backed /tmp/isp-trace.txt");
MODULE_VERSION(ISP_MONITOR_VERSION);
