/*
 * Generated T40 LSC literal chain from OEM tx-isp-t40.ko (.text 0x36b50-0x37ad8).
 *
 * The native recovered tisp_lsc_* bodies carry decompiler artifacts that make
 * them unusable: tisp_lsc_write_reg drops the divu/mflo mesh-step quotient and
 * loses the value argument of two system_reg_write calls, and
 * tisp_lsc_real_write_lut pointer-scales the port addresses so the mesh LUT
 * streams to 0x50080/0x500C0 instead of 0x50020/0x50030. Bring-up must not
 * call them; this chain is the faithful re-translation.
 *
 * Hardware map (verified against OEM disassembly):
 *   - config window: base = (bank + 96) << 7 -> 0x3000 main / 0x3080 secondary
 *       base+0x0  cell sizes,  base+0x4 count/mode/msb,
 *       base+0x8  mesh X {flag<<31 | step<<16 | end<<8 | start},
 *       base+0xc  mesh Y,      base+0x10 global gains
 *   - LUT write port: ctrl 0x50020 data 0x50024 (main),
 *                     ctrl 0x50030 data 0x50034 (secondary)
 *   - tool para = tparamsN blob + 13036 (0xa324 bytes):
 *       @0 mode, @2 count, @4 cell_w, @6 cell_h,
 *       @8/@16 mesh X/Y descriptors {flag,start,end,step},
 *       @24/@46 linear gain tables (u16[11]), @72/@94 WDR gain tables,
 *       @68/@116 msb-flag table, @120 lut word count,
 *       @122 CT breakpoints (u16[4]),
 *       @292 lut0, @14116 lut1, @27940 lut2 (3456 u32 words each)
 */
static bool regtrace_enable_lsc_lit;
module_param_named(enable_lsc_lit, regtrace_enable_lsc_lit, bool, 0644);
static uint regtrace_lsc_lit_ct = 5000; /* CT used for the static init interp */
module_param_named(lsc_lit_ct, regtrace_lsc_lit_ct, uint, 0644);
static uint regtrace_lsc_lit_gain; /* 16.16 log-gain for the static init interp */
module_param_named(lsc_lit_gain, regtrace_lsc_lit_gain, uint, 0644);

#define LSC_LIT_TOOL_BYTES   0xa324
#define LSC_LIT_TOOL_OFFSET  13036
#define LSC_LIT_REAL_BYTES   27656
#define LSC_LIT_MAX_WORDS    3456
#define LSC_LIT_MAX_NODES    1152

/* tool para field offsets */
#define LSC_LIT_T_MODE       0
#define LSC_LIT_T_COUNT      2
#define LSC_LIT_T_CELL_W     4
#define LSC_LIT_T_CELL_H     6
#define LSC_LIT_T_MESH_X     8
#define LSC_LIT_T_MESH_Y     16
#define LSC_LIT_T_GAIN0_LIN  24
#define LSC_LIT_T_GAIN1_LIN  46
#define LSC_LIT_T_MSB_LIN    68
#define LSC_LIT_T_GAIN0_WDR  72
#define LSC_LIT_T_GAIN1_WDR  94
#define LSC_LIT_T_MSB_WDR    116
#define LSC_LIT_T_WORDS      120
#define LSC_LIT_T_CT_NODES   122
#define LSC_LIT_T_LUT0       292
#define LSC_LIT_T_LUT1       14116
#define LSC_LIT_T_LUT2       27940

/* real para field offsets */
#define LSC_LIT_R_GAIN0      0
#define LSC_LIT_R_GAIN1      2
#define LSC_LIT_R_INTER      4
#define LSC_LIT_R_NODES      13828
#define LSC_LIT_R_PACKED     13832

struct lsc_lit_mesh { /* mirrors tool @8 / @16 descriptors */
	uint16_t flag;
	uint16_t start;
	uint16_t end;
	uint16_t step;
};

struct lsc_lit_bank {
	uint8_t *tool;
	uint8_t *real;
	spinlock_t lock;
	uint32_t last_gain;
	uint16_t last_ct;
	uint8_t zone; /* CT zone 0..4 */
	uint8_t wdr;
	bool alive;
};

static struct lsc_lit_bank lsc_lit_banks[2];

/* OEM valid mesh cell sizes (stack table in tisp_lsc_lut_valid_judge) */
static const uint8_t lsc_lit_cell_sizes[10] = {
	0x10, 0x14, 0x18, 0x1c, 0x20, 0x24, 0x28, 0x30, 0x40, 0x50,
};

static inline uint16_t lsc_lit_t16(const struct lsc_lit_bank *b, uint32_t off)
{
	return *(const uint16_t *)(b->tool + off);
}

static inline const uint16_t *lsc_lit_gain_tab(const struct lsc_lit_bank *b, int which)
{
	if (which == 0)
		return (const uint16_t *)(b->tool + (b->wdr ? LSC_LIT_T_GAIN0_WDR : LSC_LIT_T_GAIN0_LIN));
	return (const uint16_t *)(b->tool + (b->wdr ? LSC_LIT_T_GAIN1_WDR : LSC_LIT_T_GAIN1_LIN));
}

/* OEM tisp_simple_intp_int16: 11-entry u16 table, idx = gain>>16, frac = gain&0xffff */
static uint32_t lsc_lit_intp_u16(uint32_t idx, uint32_t frac, const uint16_t *tab)
{
	uint32_t lo, hi, diff, adj;

	if (idx >= 10)
		return tab[10];
	lo = tab[idx];
	hi = tab[idx + 1];
	if (lo == hi)
		return hi;
	diff = (lo < hi) ? (hi - lo) : (lo - hi);
	adj = diff * frac;
	adj = (adj >> 16) + ((adj >> 15) & 1);
	return (lo < hi) ? ((lo + adj) & 0xffff) : ((lo - adj) & 0xffff);
}

/* OEM tisp_lsc_lut_valid_judge(height, width, cell_h, cell_w, count) */
static int lsc_lit_valid_judge(uint32_t height, uint32_t width,
			       uint32_t cell_h, uint32_t cell_w, uint32_t count)
{
	bool found_h = false, found_w = false;
	uint32_t i, rows, cols;

	for (i = 0; i < 10; i++) {
		found_h |= (lsc_lit_cell_sizes[i] == cell_h);
		found_w |= (lsc_lit_cell_sizes[i] == cell_w);
	}
	if (!found_h && !found_w) {
		printk(KERN_WARNING "tx_isp_t40_recovered: lsc-lit invalid cell sizes %u x %u\n",
		       cell_w, cell_h);
		return -1;
	}
	if (!cell_h || !cell_w)
		return -1;

	rows = ((height / cell_h) & 0xffff) + ((height % cell_h) ? 2 : 1);
	cols = ((width / cell_w) & 0xffff) + ((width % cell_w) ? 2 : 1);
	if (cols & 1)
		cols++;
	cols &= 0xffff;
	if (cols != count * 2) {
		printk(KERN_WARNING "tx_isp_t40_recovered: lsc-lit cols %u != count*2 (%u)\n",
		       cols, count * 2);
		return -1;
	}
	if (((rows * count) & 0xffff) >= 1153) {
		printk(KERN_WARNING "tx_isp_t40_recovered: lsc-lit node total %u too large\n",
		       (rows * count) & 0xffff);
		return -1;
	}
	return 1;
}

/* OEM tisp_lsc_tool_to_awdr tail: nodes = lut words / 3 */
static void lsc_lit_refresh_nodes(struct lsc_lit_bank *b)
{
	*(uint16_t *)(b->real + LSC_LIT_R_NODES) = lsc_lit_t16(b, LSC_LIT_T_WORDS) / 3;
}

/*
 * OEM pack loop shared by all interp_by_ct exits: repack three 24-bit
 * intermediate words into the 9-byte hardware node format, under the bank
 * lock (real+4 -> real+13832).
 */
static void lsc_lit_pack_and_store(struct lsc_lit_bank *b, uint32_t words)
{
	const uint32_t *src = (const uint32_t *)(b->real + LSC_LIT_R_INTER);
	uint32_t *dst = (uint32_t *)(b->real + LSC_LIT_R_PACKED);
	unsigned long flags;
	uint32_t i;

	spin_lock_irqsave(&b->lock, flags);
	for (i = 0; i < words; i += 3) {
		uint32_t in0 = src[0], in1 = src[1], in2 = src[2];

		dst[0] = (in1 << 24) | (in0 & 0xffffff);
		dst[1] = ((in1 >> 8) & 0xffff) | (in2 << 16);
		dst[2] = (in2 >> 16) & 0xff;
		src += 3;
		dst += 3;
	}
	spin_unlock_irqrestore(&b->lock, flags);
}

/* weighted blend of two LUTs into the intermediate buffer (OEM zone 1/3 loop) */
static void lsc_lit_blend_luts(struct lsc_lit_bank *b, uint32_t lut_a_off,
			       uint32_t lut_b_off, uint32_t w_a, uint32_t frac,
			       uint32_t span, uint32_t words)
{
	const uint32_t *a = (const uint32_t *)(b->tool + lut_a_off);
	const uint32_t *bb = (const uint32_t *)(b->tool + lut_b_off);
	uint32_t *dst = (uint32_t *)(b->real + LSC_LIT_R_INTER);
	uint32_t i;

	for (i = 0; i < words; i++) {
		uint32_t va = a[i], vb = bb[i];
		uint32_t hi = ((((va >> 12) & 0xfff) * w_a +
				((vb >> 12) & 0xfff) * frac) / span) << 12;
		uint32_t lo = (((va & 0xfff) * w_a +
				(vb & 0xfff) * frac) / span) & 0xfff;

		dst[i] = (hi & 0xfff000) | lo;
	}
}

/*
 * OEM tisp_lsc_interp_by_ct(bank, -, ct, -, force): pick/blend the CT LUT.
 * Returns 1 when nothing changed, 0 after repacking, -1 on bad breakpoints.
 */
static int lsc_lit_interp_by_ct(uint32_t bank, uint32_t ct, int force)
{
	struct lsc_lit_bank *b = &lsc_lit_banks[bank];
	const uint16_t *bp = (const uint16_t *)(b->tool + LSC_LIT_T_CT_NODES);
	uint32_t words = lsc_lit_t16(b, LSC_LIT_T_WORDS);
	uint16_t delta;

	ct &= 0xffff;
	delta = (b->last_ct < ct) ? (ct - b->last_ct) : (b->last_ct - ct);
	if (delta < 64 && !force)
		return 1;
	b->last_ct = ct;

	if (ct < bp[0]) {
		if (!force && b->zone == 0)
			return 1;
		memcpy(b->real + LSC_LIT_R_INTER, b->tool + LSC_LIT_T_LUT0, words * 4);
		b->zone = 0;
	} else if (ct < bp[1]) {
		int span = (int)bp[1] - (int)bp[0];

		if (span <= 0) {
			printk(KERN_WARNING "tx_isp_t40_recovered: lsc-lit bad ct span [0]=%u [1]=%u\n",
			       bp[0], bp[1]);
			return -1;
		}
		b->zone = 1;
		lsc_lit_blend_luts(b, LSC_LIT_T_LUT0, LSC_LIT_T_LUT1,
				   span - (ct - bp[0]), ct - bp[0], span, words);
	} else if (ct < bp[2]) {
		if (!force && b->zone == 2)
			return 1;
		memcpy(b->real + LSC_LIT_R_INTER, b->tool + LSC_LIT_T_LUT1, words * 4);
		b->zone = 2;
	} else if (ct < bp[3]) {
		int span = (int)bp[3] - (int)bp[2];

		if (span <= 0) {
			printk(KERN_WARNING "tx_isp_t40_recovered: lsc-lit bad ct span [2]=%u [3]=%u\n",
			       bp[2], bp[3]);
			return -1;
		}
		b->zone = 3;
		lsc_lit_blend_luts(b, LSC_LIT_T_LUT1, LSC_LIT_T_LUT2,
				   span - (ct - bp[2]), ct - bp[2], span, words);
	} else {
		if (!force && b->zone == 4)
			return 1;
		memcpy(b->real + LSC_LIT_R_INTER, b->tool + LSC_LIT_T_LUT2, words * 4);
		b->zone = 4;
	}

	lsc_lit_pack_and_store(b, words);
	return 0;
}

/* OEM tisp_lsc_interp_by_gain(bank, -, gain, -, force): refresh global gains */
static int lsc_lit_interp_by_gain(uint32_t bank, uint32_t gain, int force)
{
	struct lsc_lit_bank *b = &lsc_lit_banks[bank];
	uint32_t delta = (b->last_gain < gain) ? (gain - b->last_gain)
					       : (b->last_gain - gain);
	uint32_t idx, frac;

	if (!(delta >> 8) && !force)
		return 1;
	b->last_gain = gain;
	idx = gain >> 16;
	frac = gain & 0xffff;
	*(uint16_t *)(b->real + LSC_LIT_R_GAIN0) =
		(uint16_t)lsc_lit_intp_u16(idx, frac, lsc_lit_gain_tab(b, 0));
	*(uint16_t *)(b->real + LSC_LIT_R_GAIN1) =
		(uint16_t)lsc_lit_intp_u16(idx, frac, lsc_lit_gain_tab(b, 1));
	return 0;
}

/* OEM tisp_lsc_write_reg: program the 0x3000/0x3080 config window */
static void lsc_lit_write_reg(uint32_t bank)
{
	struct lsc_lit_bank *b = &lsc_lit_banks[bank];
	uint32_t base = (bank + 96) << 7;
	const uint16_t *msb = (const uint16_t *)(b->tool + (b->wdr ? LSC_LIT_T_MSB_WDR : LSC_LIT_T_MSB_LIN));
	struct lsc_lit_mesh *mesh;
	uint32_t val;
	int axis;

	(void)system_reg_write(base,
		(((uint32_t)lsc_lit_t16(b, LSC_LIT_T_CELL_H) << 16) & 0xff0000) |
		(uint8_t)lsc_lit_t16(b, LSC_LIT_T_CELL_W));
	(void)system_reg_write(base + 4,
		(((uint32_t)lsc_lit_t16(b, LSC_LIT_T_COUNT) << 16) & 0x3ff0000) |
		(lsc_lit_t16(b, LSC_LIT_T_MODE) & 3) |
		(((uint32_t)msb[0] << 15) & 0xffff));

	for (axis = 0; axis < 2; axis++) {
		mesh = (struct lsc_lit_mesh *)(b->tool +
			(axis ? LSC_LIT_T_MESH_Y : LSC_LIT_T_MESH_X));
		if (mesh->end >= (uint32_t)mesh->start + 2) {
			uint32_t step = 1024 / (mesh->end - mesh->start);

			mesh->step = (uint16_t)step;
			val = ((uint32_t)mesh->flag << 31) |
			      ((step << 16) & 0x3ff0000) |
			      (((uint32_t)mesh->end << 8) & 0xff00) |
			      (mesh->start & 0xff);
		} else {
			printk(KERN_WARNING "tx_isp_t40_recovered: lsc-lit bad mesh %c start=%u end=%u\n",
			       axis ? 'Y' : 'X', mesh->start, mesh->end);
			val = 0x2000200;
		}
		(void)system_reg_write(base + 8 + axis * 4, val);
	}

	(void)system_reg_write(base + 16,
		(((uint32_t)*(uint16_t *)(b->real + LSC_LIT_R_GAIN1) << 16) & 0xff0000) |
		*(uint8_t *)(b->real + LSC_LIT_R_GAIN0));
}

/* OEM tisp_lsc_real_write_lut: stream the packed mesh through the write port */
static void lsc_lit_real_write_lut(uint32_t bank)
{
	struct lsc_lit_bank *b = &lsc_lit_banks[bank];
	uint32_t ctrl = bank ? 0x50030 : 0x50020;
	uint32_t nodes = *(uint16_t *)(b->real + LSC_LIT_R_NODES);
	const uint32_t *src = (const uint32_t *)(b->real + LSC_LIT_R_PACKED);
	unsigned long flags;
	uint32_t i;

	spin_lock_irqsave(&b->lock, flags);
	(void)system_reg_write(ctrl, 0x101);
	for (i = 0; i < nodes * 3; i++)
		(void)system_reg_write(ctrl + 4, src[i]);
	(void)system_reg_write(ctrl, ((nodes - 1) << 16) | 0x102);
	spin_unlock_irqrestore(&b->lock, flags);
}

/*
 * OEM tisp_lsc_init(bank, width, height), data sourced from the live
 * tparamsN tuning blob at +13036 like the OEM memcpy.
 */
static int regtrace_lsc_main_init_lit(uint32_t bank, const uint8_t *nblob,
				      uint32_t width, uint32_t height)
{
	struct lsc_lit_bank *b = &lsc_lit_banks[bank];
	uint32_t words;
	int judge;

	if (bank > 1 || !nblob)
		return -EINVAL;

	if (!b->alive) {
		spin_lock_init(&b->lock);
		b->tool = vzalloc(LSC_LIT_TOOL_BYTES);
		b->real = vzalloc(LSC_LIT_REAL_BYTES);
		if (!b->tool || !b->real) {
			vfree(b->tool);
			vfree(b->real);
			b->tool = NULL;
			b->real = NULL;
			return -ENOMEM;
		}
		b->alive = true;
	}

	memcpy(b->tool, nblob + LSC_LIT_TOOL_OFFSET, LSC_LIT_TOOL_BYTES);

	words = lsc_lit_t16(b, LSC_LIT_T_WORDS);
	if (!words || words > LSC_LIT_MAX_WORDS || (words % 3)) {
		printk(KERN_WARNING "tx_isp_t40_recovered: lsc-lit bad lut word count %u\n",
		       words);
		return -EINVAL;
	}

	judge = lsc_lit_valid_judge(height, width,
				    lsc_lit_t16(b, LSC_LIT_T_CELL_H),
				    lsc_lit_t16(b, LSC_LIT_T_CELL_W),
				    lsc_lit_t16(b, LSC_LIT_T_COUNT));
	if (judge < 0) {
		/* OEM only warns and continues; refuse instead of writing a
		 * wrong-geometry mesh. */
		printk(KERN_WARNING "tx_isp_t40_recovered: lsc-lit layout judge failed (%ux%u cells %ux%u count %u)\n",
		       width, height,
		       lsc_lit_t16(b, LSC_LIT_T_CELL_W),
		       lsc_lit_t16(b, LSC_LIT_T_CELL_H),
		       lsc_lit_t16(b, LSC_LIT_T_COUNT));
		return -EINVAL;
	}

	lsc_lit_refresh_nodes(b);
	(void)lsc_lit_interp_by_ct(bank, regtrace_lsc_lit_ct, 1);
	(void)lsc_lit_interp_by_gain(bank, regtrace_lsc_lit_gain, 1);
	lsc_lit_write_reg(bank);
	lsc_lit_real_write_lut(bank);

	printk(KERN_WARNING "tx_isp_t40_recovered: lsc-lit init bank=%u %ux%u words=%u nodes=%u ct=%u zone=%u bp=[%u %u %u %u]\n",
	       bank, width, height, words,
	       *(uint16_t *)(b->real + LSC_LIT_R_NODES),
	       regtrace_lsc_lit_ct, b->zone,
	       *(const uint16_t *)(b->tool + LSC_LIT_T_CT_NODES),
	       *(const uint16_t *)(b->tool + LSC_LIT_T_CT_NODES + 2),
	       *(const uint16_t *)(b->tool + LSC_LIT_T_CT_NODES + 4),
	       *(const uint16_t *)(b->tool + LSC_LIT_T_CT_NODES + 6));
	return 0;
}

/* runtime hooks (OEM tisp_lsc_ct_update / tisp_lsc_gain_update without the
 * deferred wlut-frequency machinery: flush immediately on change) */
static void __maybe_unused regtrace_lsc_lit_ct_update(uint32_t bank, uint32_t ct)
{
	if (!lsc_lit_banks[bank].alive)
		return;
	if (lsc_lit_interp_by_ct(bank, ct, 0) == 0)
		lsc_lit_real_write_lut(bank);
}

static void __maybe_unused regtrace_lsc_lit_gain_update(uint32_t bank, uint32_t gain)
{
	if (!lsc_lit_banks[bank].alive)
		return;
	if (lsc_lit_interp_by_gain(bank, gain, 0) == 0)
		lsc_lit_write_reg(bank);
}

static void regtrace_lsc_lit_cleanup(void)
{
	int bank;

	for (bank = 0; bank < 2; bank++) {
		struct lsc_lit_bank *b = &lsc_lit_banks[bank];

		if (!b->alive)
			continue;
		vfree(b->tool);
		vfree(b->real);
		b->tool = NULL;
		b->real = NULL;
		b->alive = false;
	}
}
