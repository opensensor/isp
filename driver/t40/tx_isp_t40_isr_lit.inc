/*
 * Literal mips2c port of the OEM interrupt service routine and ISP-OSD
 * updater (OEM 0x7b29c / 0x7b210). This is the forensic frame-delivery
 * path: ack status0/1 at +0x40028/+0x40038, run the ch0 pre-dequeue
 * diagnostics, update the ISP OSD on status bit0, walk the per-channel
 * states reading the MSCA done FIFOs and pushing buffers to the framechan
 * remotes, and dispatch irq_func_cb[] per status bit -- all in OEM order.
 * Gated by enable_oem_isr_lit (default off until live-validated); when
 * enabled, the regtrace frame-done shim (masks/delays/spare) is bypassed.
 *
 * Hand-fixed translation hazards (audited against the OEM disasm):
 * - 4 jalr-via-.text sites: ispcore_irqstatus_func (x2, main) and
 *   ispcore_irqstatusSec_func (x2, sec).
 * - 2 jalr s6 sites the tracker misbound to &ispcore_sd: they are
 *   Tzn_Msca_addr_fifo_read (s6 bound at OEM 7b8f4).
 * - 2 jalr-v0 sites in the bit-dispatch loop: indirect irq_func_cb[] /
 *   irq_func_cb[bit+42] calls, not tx_isp_send_event_to_remote.
 */
static bool regtrace_enable_oem_isr_lit;
static uint32_t regtrace_oem_isr_lit_count;
module_param_named(enable_oem_isr_lit, regtrace_enable_oem_isr_lit, bool, 0644);
module_param_named(oem_isr_lit_count, regtrace_oem_isr_lit_count, uint, 0444);

/*
 * Bisection cut-point for localizing the hard-hang the full lit ISR causes.
 * oem_isr_lit_cut = N runs only stages 1..N of the OEM ISR then returns; 0
 * (default) runs the whole routine. Both status words are read+ACKed at the
 * very top of the function, BEFORE every cut point, so even cut=1 cannot
 * cause an interrupt storm -- a cut just suppresses frame delivery (stream
 * stalls but the box stays alive). Walk N up until the box hard-hangs; the
 * first hanging N is the offending stage. Stages:
 *   1 = status read + ACK only (no ch0-diag, no OSD, no walk)
 *   2 = + ch0 pre-dq diagnostic + update_isp_osd_lit (status0 bit0)
 *   3 = + s5 error/overflow + frame_vb timestamps + per-bit static counters
 *   4 = + main-channel frame-done walk (fifo_read/send_event/irqstatus_func)
 *   0/>=5 = full (status1 sec-channel walks + irq_func_cb dispatch)
 */
static uint32_t regtrace_oem_isr_lit_cut;
module_param_named(oem_isr_lit_cut, regtrace_oem_isr_lit_cut, uint, 0644);

/* OEM .bss state private to the lit ISR (fragment scalars stay untouched) */
static uint32_t tv_frame_start_next_lit[4];
static uint32_t tv_frame_end_lit[4];
static uint32_t tv_frame_start_lit[4];
static uint32_t frame_vb_measure_lit;
static uint32_t awb_static_tmp_lit[2];
static uint32_t ae_hist_tmp_lit[2];
static uint32_t ae_static_tmp_lit[2];
static uint32_t lce_static_tmp_lit[2];
static uint32_t defog_static_tmp_lit[2];
static uint32_t adr_static_tmp_lit[2];
static uint8_t frame_use_num_lit[6];
static uint8_t frame_use_num_tmp_lit[6];
static uint32_t tmp_addr_uv_lit;
static uint32_t tmp_addr_lit;
static uint32_t irq_count_lit;
static uint32_t savenum_lit;
static uint32_t saveflag_lit;
static uint32_t isp_breakfrm_lit;
static uint32_t isp_overflow_lit;
static uint32_t isp_err_lit;
static uint32_t isp_ch0_frm_done_lit;

static const char isr_lc181[] = "isr: ch0 pre-dq fifo=%x st=%x\n";
static const char isr_lc182[] = "isr: %s frame err\n";
static const char isr_lc183[] = "isr: overflow %x\n";
static const char isr_lc_aux[] = "ispcore";

static int32_t update_isp_osd_lit(void);
static int32_t ispcore_interrupt_service_routine_lit(uintptr_t a0_in);

static uint32_t regtrace_isr_lit_gettimeofday(uint32_t tvp)
{
    struct timeval tv;

    do_gettimeofday(&tv);
    if (tvp) {
        *(uint32_t *)(uintptr_t)tvp = (uint32_t)tv.tv_sec;
        *(uint32_t *)((uintptr_t)tvp + 4) = (uint32_t)tv.tv_usec;
    }
    return 0;
}

#undef REGCALL_system_reg_read
#undef REGCALL_isp_printf
#undef REGCALL_private_do_gettimeofday
#undef REGCALL_private_schedule_work
#undef REGCALL_tisp_push_frame_count
#undef REGCALL_tx_isp_send_event_to_remote
#undef REGCALL_Tzn_Msca_addr_fifo_read
#undef REGCALL_Tzn_Msca_addr_fifo_write
#undef REGCALL_update_isp_osd
#undef REGCALL_tisp_msca_state
#undef REGCALL_tisp_msca_Shd_ctrl
#define REGCALL_system_reg_read(a, b, c, d) ((uint32_t)system_reg_read((a)))
#define REGCALL_isp_printf(a, b, c, d) \
    ((uint32_t)isp_printf((a), (const char *)(uintptr_t)(b), (c), (d)))
#define REGCALL_private_do_gettimeofday(a, b, c, d) \
    regtrace_isr_lit_gettimeofday((a))
#define REGCALL_private_schedule_work(a, b, c, d) \
    ((uint32_t)private_schedule_work((int32_t)(a)))
#define REGCALL_tisp_push_frame_count(a, b, c, d) \
    ((uint32_t)tisp_push_frame_count((int32_t)(a)))
#define REGCALL_tx_isp_send_event_to_remote(a, b, c, d) \
    ((uint32_t)tx_isp_send_event_to_remote((void *)(uintptr_t)(a), (b), \
                                           (void *)(uintptr_t)(c)))
#define REGCALL_Tzn_Msca_addr_fifo_read(a, b, c, d) \
    ((uint32_t)Tzn_Msca_addr_fifo_read((a), (b), (uintptr_t)(c), (uintptr_t)(d)))
#define REGCALL_Tzn_Msca_addr_fifo_write(a, b, c, d) \
    ((uint32_t)Tzn_Msca_addr_fifo_write((a), (b), (c), (d)))
#define REGCALL_update_isp_osd(a, b, c, d) ((uint32_t)update_isp_osd_lit())
#define REGCALL_tisp_msca_state(a, b, c, d) ((uint32_t)tisp_msca_state())
#define REGCALL_tisp_msca_Shd_ctrl(a, b, c, d) \
    ((uint32_t)tisp_msca_Shd_ctrl((a), (b)))

static int32_t update_isp_osd_lit(void)
{
    uint32_t r_v0 = 0, r_v1 = 0;
    uint32_t r_t0 = 0, r_t1 = 0, r_t2 = 0, r_t3 = 0, r_t4 = 0, r_t5 = 0;
    uint32_t r_t6 = 0, r_t7 = 0, r_t8 = 0, r_t9 = 0;
    uint32_t r_s0 = 0, r_s1 = 0, r_s2 = 0, r_s3 = 0, r_s4 = 0, r_s5 = 0;
    uint32_t r_s6 = 0, r_s7 = 0, r_s8 = 0, r_ra = 0, r_at = 0;
    uint32_t mips_lo = 0, mips_hi = 0, mips_cond = 0;
    uint32_t r_a0 = 0;
    uint32_t r_a1 = 0;
    uint32_t r_a2 = 0;
    uint32_t r_a3 = 0;
    uint8_t mips_stack[448] __attribute__((aligned(8)));
    uint32_t r_sp = (uint32_t)(uintptr_t)(mips_stack + 384);

    (void)r_at; (void)r_ra; (void)r_t8; (void)r_t9; (void)mips_lo;
    (void)mips_hi; (void)r_sp; (void)mips_cond; (void)r_a2; (void)r_a3;
    r_v0 = (uint32_t)(uintptr_t)&ispcore_sd; /* HI16 ispcore_sd */
    r_v0 = *(uint32_t *)(uintptr_t)(r_v0 + 0);
    r_sp = r_sp + -0x20U;
    *(uint32_t *)(uintptr_t)(r_sp + 28) = (uint32_t)r_ra;
    *(uint32_t *)(uintptr_t)(r_sp + 24) = (uint32_t)r_s1;
    *(uint32_t *)(uintptr_t)(r_sp + 20) = (uint32_t)r_s0;
    r_s1 = *(uint32_t *)(uintptr_t)(r_v0 + 268);
    r_v0 = (uint32_t)(uintptr_t)&tisp_msca_state; /* HI16 tisp_msca_state */
    r_v0 = (uint32_t)(uintptr_t)&tisp_msca_state;
    /* nop */
    r_v0 = (uint32_t)REGCALL_tisp_msca_state(r_a0, r_a1, r_a2, r_a3);
    mips_cond = (r_v0 != 0) ? 1U : 0U;
    r_s0 = 12U;
    if (mips_cond) goto L_7b288;
    r_a1 = *(uint32_t *)(uintptr_t)(r_s1 + 692);
    r_v0 = (uint32_t)((int32_t)r_a1 * (int32_t)r_s0);
    r_s0 = r_v0 + r_s1;
    r_v0 = *(uint32_t *)(uintptr_t)(r_s0 + 656);
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_v0 = (uint32_t)(uintptr_t)&tisp_msca_Shd_ctrl; /* HI16 tisp_msca_Shd_ctrl */
    if (mips_cond) goto L_7b274;
    r_v0 = (uint32_t)(uintptr_t)&tisp_msca_Shd_ctrl;
    r_a0 = 0;
    r_v0 = (uint32_t)REGCALL_tisp_msca_Shd_ctrl(r_a0, r_a1, r_a2, r_a3);
    r_v0 = *(uint32_t *)(uintptr_t)(r_s0 + 656);
    r_v0 = r_v0 + -0x1U;
    *(uint32_t *)(uintptr_t)(r_s0 + 656) = (uint32_t)r_v0;
L_7b274:
    r_v0 = *(uint32_t *)(uintptr_t)(r_s1 + 692);
    r_v0 = r_v0 + 0x1U;
    r_v1 = (r_v0 < (uint32_t)3) ? 1U : 0U;
    if (r_v1 == 0) r_v0 = 0;
    *(uint32_t *)(uintptr_t)(r_s1 + 692) = (uint32_t)r_v0;
L_7b288:
    r_ra = *(uint32_t *)(uintptr_t)(r_sp + 28);
    r_s1 = *(uint32_t *)(uintptr_t)(r_sp + 24);
    r_s0 = *(uint32_t *)(uintptr_t)(r_sp + 20);
    r_sp = r_sp + 0x20U;
    goto fn_exit;
fn_exit:
    return (int32_t)r_v0;
}

static int32_t ispcore_interrupt_service_routine_lit(uintptr_t a0_in)
{
    uint32_t r_v0 = 0, r_v1 = 0;
    uint32_t r_t0 = 0, r_t1 = 0, r_t2 = 0, r_t3 = 0, r_t4 = 0, r_t5 = 0;
    uint32_t r_t6 = 0, r_t7 = 0, r_t8 = 0, r_t9 = 0;
    uint32_t r_s0 = 0, r_s1 = 0, r_s2 = 0, r_s3 = 0, r_s4 = 0, r_s5 = 0;
    uint32_t r_s6 = 0, r_s7 = 0, r_s8 = 0, r_ra = 0, r_at = 0;
    uint32_t mips_lo = 0, mips_hi = 0, mips_cond = 0;
    uint32_t r_a0 = (uint32_t)a0_in;
    uint32_t r_a1 = 0;
    uint32_t r_a2 = 0;
    uint32_t r_a3 = 0;
    uint8_t mips_stack[448] __attribute__((aligned(8)));
    uint32_t r_sp = (uint32_t)(uintptr_t)(mips_stack + 384);

    (void)r_at; (void)r_ra; (void)r_t8; (void)r_t9; (void)mips_lo;
    (void)mips_hi; (void)r_sp; (void)mips_cond; (void)r_a2; (void)r_a3;
    regtrace_oem_isr_lit_count++;
    r_sp = r_sp + -0x80U;
    *(uint32_t *)(uintptr_t)(r_sp + 112) = (uint32_t)r_s6;
    *(uint32_t *)(uintptr_t)(r_sp + 88) = (uint32_t)r_s0;
    *(uint32_t *)(uintptr_t)(r_sp + 124) = (uint32_t)r_ra;
    *(uint32_t *)(uintptr_t)(r_sp + 120) = (uint32_t)r_s8;
    *(uint32_t *)(uintptr_t)(r_sp + 116) = (uint32_t)r_s7;
    *(uint32_t *)(uintptr_t)(r_sp + 108) = (uint32_t)r_s5;
    *(uint32_t *)(uintptr_t)(r_sp + 104) = (uint32_t)r_s4;
    *(uint32_t *)(uintptr_t)(r_sp + 100) = (uint32_t)r_s3;
    *(uint32_t *)(uintptr_t)(r_sp + 96) = (uint32_t)r_s2;
    *(uint32_t *)(uintptr_t)(r_sp + 92) = (uint32_t)r_s1;
    r_v0 = *(uint32_t *)(uintptr_t)(r_a0 + 232);
    r_v1 = 0x40000U;
    r_s3 = *(uint32_t *)(uintptr_t)(r_a0 + 268);
    r_v0 = r_v0 + r_v1;
    r_s2 = *(uint32_t *)(uintptr_t)(r_v0 + 40);
    r_s0 = r_a0;
    *(uint32_t *)(uintptr_t)(r_v0 + 36) = (uint32_t)r_s2;
    r_v0 = *(uint32_t *)(uintptr_t)(r_a0 + 232);
    r_s6 = r_s2 & 0x2000U;
    r_v0 = r_v0 + r_v1;
    r_s1 = *(uint32_t *)(uintptr_t)(r_v0 + 56);
    *(uint32_t *)(uintptr_t)(r_v0 + 52) = (uint32_t)r_s1;
    r_v0 = *(uint32_t *)(uintptr_t)(r_a0 + 232);
    r_v0 = r_v0 + r_v1;
    r_s5 = *(uint32_t *)(uintptr_t)(r_v0 + 104);
    /* cut-point 1: status read+ACK complete */
    if (regtrace_oem_isr_lit_cut && regtrace_oem_isr_lit_cut <= 1) { r_v0 = 1U; goto fn_exit; }
    mips_cond = (r_s6 == 0) ? 1U : 0U;
    r_v0 = (uint32_t)(uintptr_t)&isp_ch0_pre_dequeue_time; /* HI16 isp_ch0_pre_dequeue_time */
    if (mips_cond) goto L_7b370;
    r_v0 = *(uint32_t *)(uintptr_t)(r_v0 + 0);
    mips_cond = ((int32_t)r_v0 <= 0) ? 1U : 0U;
    r_s4 = r_s2 & 0x1U;
    if (mips_cond) goto L_7b374;
    r_a2 = 0x10000U;
    r_v1 = (uint32_t)(uintptr_t)&system_reg_read; /* HI16 system_reg_read */
    r_v1 = (uint32_t)(uintptr_t)&system_reg_read;
    r_a0 = r_a2 + 0x6170U;
    r_v0 = (uint32_t)REGCALL_system_reg_read(r_a0, r_a1, r_a2, r_a3);
    r_a0 = r_a2 + 0x6188U;
    r_a3 = (r_v0 >> 8) & 0xfU;
    r_v0 = (uint32_t)REGCALL_system_reg_read(r_a0, r_a1, r_a2, r_a3);
    r_a0 = r_a2 + 0x67f4U;
    r_a1 = r_v0;
    r_v0 = (uint32_t)REGCALL_system_reg_read(r_a0, r_a1, r_a2, r_a3);
    mips_cond = (r_a3 == 0) ? 1U : 0U;
    r_s4 = r_s2 & 0x1U;
    if (mips_cond) goto L_7b374;
    r_a1 = (r_a1 >> 8) & 0xfU;
    *(uint32_t *)(uintptr_t)(r_sp + 16) = (uint32_t)r_a1;
    r_a2 = r_v0 >> 16;
    r_a1 = (uint32_t)(uintptr_t)isr_lc181; /* HI16 $LC181 */
    r_v0 = (uint32_t)(uintptr_t)&isp_printf; /* HI16 isp_printf */
    r_a1 = (uint32_t)(uintptr_t)isr_lc181;
    r_v0 = (uint32_t)(uintptr_t)&isp_printf;
    r_a0 = 1U;
    r_v0 = (uint32_t)REGCALL_isp_printf(r_a0, r_a1, r_a2, r_a3);
L_7b370:
    r_s4 = r_s2 & 0x1U;
L_7b374:
    mips_cond = (r_s4 == 0) ? 1U : 0U;
    r_v0 = (uint32_t)(uintptr_t)&update_isp_osd; /* HI16 update_isp_osd */
    if (mips_cond) goto L_7b388;
    r_v0 = (uint32_t)(uintptr_t)&update_isp_osd;
    /* nop */
    r_v0 = (uint32_t)REGCALL_update_isp_osd(r_a0, r_a1, r_a2, r_a3);
L_7b388:
    /* cut-point 2: ch0 pre-dq diag + update_isp_osd complete */
    if (regtrace_oem_isr_lit_cut && regtrace_oem_isr_lit_cut <= 2) { r_v0 = 1U; goto fn_exit; }
    mips_cond = (r_s5 == 0) ? 1U : 0U;
    r_s8 = 0x40000U;
    if (mips_cond) goto L_7b434;
    r_v0 = *(uint32_t *)(uintptr_t)(r_s0 + 232);
    r_a0 = 0x10000U;
    r_v1 = r_v0 + r_s8;
    r_v0 = r_v0 + r_a0;
    r_v1 = *(uint32_t *)(uintptr_t)(r_v1 + 256);
    r_v0 = *(uint32_t *)(uintptr_t)(r_v0 + 26448);
    r_a2 = (uint32_t)(uintptr_t)isr_lc_aux;
    r_a1 = (uint32_t)(uintptr_t)isr_lc182; /* HI16 $LC182 */
    r_s7 = (uint32_t)(uintptr_t)&isp_printf; /* HI16 isp_printf */
    *(uint32_t *)(uintptr_t)(r_sp + 20) = (uint32_t)r_v1;
    *(uint32_t *)(uintptr_t)(r_sp + 24) = (uint32_t)r_v0;
    *(uint32_t *)(uintptr_t)(r_sp + 16) = (uint32_t)r_s5;
    r_a3 = 742U;
    r_a2 = (uint32_t)(uintptr_t)isr_lc_aux;
    r_a1 = (uint32_t)(uintptr_t)isr_lc182;
    r_s7 = (uint32_t)(uintptr_t)&isp_printf;
    r_a0 = 0;
    r_v0 = (uint32_t)REGCALL_isp_printf(r_a0, r_a1, r_a2, r_a3);
    r_v0 = *(uint32_t *)(uintptr_t)(r_s0 + 232);
    r_v1 = 0; /* HI16 .bss anchor */
    r_v0 = r_v0 + r_s8;
    *(uint32_t *)(uintptr_t)(r_v0 + 100) = (uint32_t)r_s5;
    r_v0 = *(uint32_t *)(uintptr_t)((uintptr_t)&isp_err_lit);
    r_v0 = r_v0 + 0x1U;
    *(uint32_t *)((uintptr_t)((uintptr_t)&isp_err_lit) + 0) = (uint32_t)r_v0;
    r_v0 = r_s5 & 0x20U;
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_v0 = 0; /* HI16 .bss anchor */
    if (mips_cond) goto L_7b41c;
    r_a2 = *(uint32_t *)(uintptr_t)((uintptr_t)&isp_overflow_lit);
    r_a1 = (uint32_t)(uintptr_t)isr_lc183; /* HI16 $LC183 */
    r_a1 = (uint32_t)(uintptr_t)isr_lc183;
    r_a2 = r_a2 + 0x1U;
    r_a0 = 1U;
    *(uint32_t *)((uintptr_t)((uintptr_t)&isp_overflow_lit) + 0) = (uint32_t)r_a2;
    r_v0 = (uint32_t)REGCALL_isp_printf(r_a0, r_a1, r_a2, r_a3);
L_7b41c:
    r_s5 = r_s5 & 0x10U;
    mips_cond = (r_s5 == 0) ? 1U : 0U;
    r_v1 = 0; /* HI16 .bss anchor */
    if (mips_cond) goto L_7b434;
    r_v0 = *(uint32_t *)(uintptr_t)((uintptr_t)&isp_breakfrm_lit);
    r_v0 = r_v0 + 0x1U;
    *(uint32_t *)((uintptr_t)((uintptr_t)&isp_breakfrm_lit) + 0) = (uint32_t)r_v0;
L_7b434:
    r_v0 = *(uint32_t *)(uintptr_t)(r_s0 + 232);
    r_v1 = 0x40000U;
    r_v0 = r_v0 + r_v1;
    r_v1 = *(uint32_t *)(uintptr_t)(r_v0 + 168);
    *(uint32_t *)(uintptr_t)(r_sp + 64) = (uint32_t)r_v1;
    *(uint32_t *)(uintptr_t)(r_v0 + 164) = (uint32_t)r_v1;
    mips_cond = (r_s6 == 0) ? 1U : 0U;
    r_v0 = r_s1 & 0x2000U;
    if (mips_cond) goto L_7b4e8;
    r_s5 = 0; /* HI16 .bss anchor */
    r_v1 = *(uint8_t *)(uintptr_t)((uintptr_t)&frame_vb_measure_lit);
    r_v0 = 1U;
    mips_cond = (r_v1 != r_v0) ? 1U : 0U;
    r_v0 = 3U;
    if (mips_cond) goto L_7b48c;
    r_a0 = 0; /* HI16 .bss anchor */
    r_v0 = (uint32_t)(uintptr_t)&private_do_gettimeofday; /* HI16 private_do_gettimeofday */
    r_v0 = (uint32_t)(uintptr_t)&private_do_gettimeofday;
    r_a0 = (uint32_t)(uintptr_t)&tv_frame_start_lit[0];
    r_v0 = (uint32_t)REGCALL_private_do_gettimeofday(r_a0, r_a1, r_a2, r_a3);
    r_v0 = 2U;
    *(uint8_t *)((uintptr_t)((uintptr_t)&frame_vb_measure_lit) + 0) = (uint8_t)r_v0;
    r_v1 = *(uint8_t *)(uintptr_t)((uintptr_t)&frame_vb_measure_lit);
    r_v0 = 3U;
L_7b48c:
    mips_cond = (r_v1 != r_v0) ? 1U : 0U;
    r_v0 = (uint32_t)(uintptr_t)&tisp_push_frame_count; /* HI16 tisp_push_frame_count */
    if (mips_cond) goto L_7b4b4;
    r_a0 = 0; /* HI16 .bss anchor */
    r_v0 = (uint32_t)(uintptr_t)&private_do_gettimeofday; /* HI16 private_do_gettimeofday */
    r_v0 = (uint32_t)(uintptr_t)&private_do_gettimeofday;
    r_a0 = (uint32_t)(uintptr_t)&tv_frame_start_next_lit[0];
    r_v0 = (uint32_t)REGCALL_private_do_gettimeofday(r_a0, r_a1, r_a2, r_a3);
    r_v0 = 4U;
    *(uint8_t *)((uintptr_t)((uintptr_t)&frame_vb_measure_lit) + 0) = (uint8_t)r_v0;
    r_v0 = (uint32_t)(uintptr_t)&tisp_push_frame_count; /* HI16 tisp_push_frame_count */
L_7b4b4:
    r_v0 = (uint32_t)(uintptr_t)&tisp_push_frame_count;
    r_a0 = 0;
    r_v0 = (uint32_t)REGCALL_tisp_push_frame_count(r_a0, r_a1, r_a2, r_a3);
    r_v0 = (uint32_t)(uintptr_t)&isp_ch0_pre_dequeue_time; /* HI16 isp_ch0_pre_dequeue_time */
    r_v0 = *(uint32_t *)(uintptr_t)(r_v0 + 0);
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_v0 = r_s1 & 0x2000U;
    if (mips_cond) goto L_7b4e8;
    r_a0 = (uint32_t)(uintptr_t)&pre_frame_dequeue; /* HI16 pre_frame_dequeue */
    r_v0 = (uint32_t)(uintptr_t)&private_schedule_work; /* HI16 private_schedule_work */
    r_v0 = (uint32_t)(uintptr_t)&private_schedule_work;
    r_a0 = (uint32_t)(uintptr_t)&pre_frame_dequeue;
    r_v0 = (uint32_t)REGCALL_private_schedule_work(r_a0, r_a1, r_a2, r_a3);
    r_v0 = r_s1 & 0x2000U;
L_7b4e8:
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_v0 = r_s2 & 0x200U;
    if (mips_cond) goto L_7b564;
    r_s5 = 0; /* HI16 .bss anchor */
    r_s5 = (uint32_t)(uintptr_t)&frame_vb_measure_lit;
    r_v1 = *(uint8_t *)(uintptr_t)(r_s5 + 1);
    r_v0 = 1U;
    mips_cond = (r_v1 != r_v0) ? 1U : 0U;
    r_v0 = 3U;
    if (mips_cond) goto L_7b52c;
    r_a0 = 0; /* HI16 .bss anchor */
    r_v0 = (uint32_t)(uintptr_t)&private_do_gettimeofday; /* HI16 private_do_gettimeofday */
    r_v0 = (uint32_t)(uintptr_t)&private_do_gettimeofday;
    r_a0 = (uint32_t)(uintptr_t)&tv_frame_start_lit[2];
    r_v0 = (uint32_t)REGCALL_private_do_gettimeofday(r_a0, r_a1, r_a2, r_a3);
    r_v0 = 2U;
    *(uint8_t *)(uintptr_t)(r_s5 + 1) = (uint8_t)r_v0;
    r_v1 = *(uint8_t *)(uintptr_t)(r_s5 + 1);
    r_v0 = 3U;
L_7b52c:
    mips_cond = (r_v1 != r_v0) ? 1U : 0U;
    r_v0 = (uint32_t)(uintptr_t)&tisp_push_frame_count; /* HI16 tisp_push_frame_count */
    if (mips_cond) goto L_7b554;
    r_a0 = 0; /* HI16 .bss anchor */
    r_v0 = (uint32_t)(uintptr_t)&private_do_gettimeofday; /* HI16 private_do_gettimeofday */
    r_v0 = (uint32_t)(uintptr_t)&private_do_gettimeofday;
    r_a0 = (uint32_t)(uintptr_t)&tv_frame_start_next_lit[2];
    r_v0 = (uint32_t)REGCALL_private_do_gettimeofday(r_a0, r_a1, r_a2, r_a3);
    r_v0 = 4U;
    *(uint8_t *)(uintptr_t)(r_s5 + 1) = (uint8_t)r_v0;
    r_v0 = (uint32_t)(uintptr_t)&tisp_push_frame_count; /* HI16 tisp_push_frame_count */
L_7b554:
    r_v0 = (uint32_t)(uintptr_t)&tisp_push_frame_count;
    r_a0 = 1U;
    r_v0 = (uint32_t)REGCALL_tisp_push_frame_count(r_a0, r_a1, r_a2, r_a3);
    r_v0 = r_s2 & 0x200U;
L_7b564:
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_v0 = r_s2 & 0x400U;
    if (mips_cond) goto L_7b580;
    r_v1 = 0; /* HI16 .bss anchor */
    r_v0 = *(uint32_t *)(uintptr_t)((uintptr_t)&adr_static_tmp_lit[0]);
    r_v0 = r_v0 + 0x1U;
    *(uint32_t *)((uintptr_t)((uintptr_t)&adr_static_tmp_lit[0]) + 0) = (uint32_t)r_v0;
    r_v0 = r_s2 & 0x400U;
L_7b580:
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_v0 = r_s2 & 0x100U;
    if (mips_cond) goto L_7b610;
    r_v1 = 0; /* HI16 .bss anchor */
    r_v0 = *(uint32_t *)(uintptr_t)((uintptr_t)&defog_static_tmp_lit[0]);
    r_a1 = *(uint32_t *)(uintptr_t)(r_s0 + 232);
    r_a3 = 0; /* HI16 .bss anchor */
    r_v0 = r_v0 + 0x1U;
    *(uint32_t *)((uintptr_t)((uintptr_t)&defog_static_tmp_lit[0]) + 0) = (uint32_t)r_v0;
    r_v0 = 0x10000U;
    r_v0 = r_a1 + r_v0;
    r_a2 = *(uint32_t *)(uintptr_t)(r_v0 + 24584);
    r_v1 = 0; /* HI16 .bss anchor */
    r_v1 = (uint32_t)(uintptr_t)&frame_use_num_tmp_lit[0];
    r_a0 = 0;
    r_a3 = (uint32_t)(uintptr_t)&frame_use_num_lit[0];
    r_t1 = 1U;
    r_t0 = 3U;
L_7b5c4:
    r_v0 = *(uint8_t *)(uintptr_t)(r_v1 + 0);
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_v0 = r_v0 + -0x1U;
    if (mips_cond) goto L_7b5f4;
    r_v0 = r_v0 & 0xffU;
    mips_cond = (r_v0 != 0) ? 1U : 0U;
    *(uint8_t *)(uintptr_t)(r_v1 + 0) = (uint8_t)r_v0;
    if (mips_cond) goto L_7b5f4;
    r_v0 = r_a3 + r_a0;
    r_t2 = *(uint8_t *)(uintptr_t)(r_v0 + 0);
    mips_cond = (r_t2 == 0) ? 1U : 0U;
    r_t2 = r_t1 << (r_a0 & 31);
    if (mips_cond) goto L_7b5f4;
    r_a2 = r_a2 | r_t2;
    *(uint8_t *)(uintptr_t)(r_v0 + 0) = (uint8_t)0;
L_7b5f4:
    r_a0 = r_a0 + 0x1U;
    mips_cond = (r_a0 != r_t0) ? 1U : 0U;
    r_v1 = r_v1 + 0x1U;
    if (mips_cond) goto L_7b5c4;
    r_v0 = 0x10000U;
    r_a1 = r_a1 + r_v0;
    *(uint32_t *)(uintptr_t)(r_a1 + 24584) = (uint32_t)r_a2;
    r_v0 = r_s2 & 0x100U;
L_7b610:
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_v0 = r_s2 & 0x10U;
    if (mips_cond) goto L_7b62c;
    r_v1 = 0; /* HI16 .bss anchor */
    r_v0 = *(uint32_t *)(uintptr_t)((uintptr_t)&lce_static_tmp_lit[0]);
    r_v0 = r_v0 + 0x1U;
    *(uint32_t *)((uintptr_t)((uintptr_t)&lce_static_tmp_lit[0]) + 0) = (uint32_t)r_v0;
    r_v0 = r_s2 & 0x10U;
L_7b62c:
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_v0 = r_s2 & 0x20U;
    if (mips_cond) goto L_7b648;
    r_v1 = 0; /* HI16 .bss anchor */
    r_v0 = *(uint32_t *)(uintptr_t)((uintptr_t)&ae_static_tmp_lit[0]);
    r_v0 = r_v0 + 0x1U;
    *(uint32_t *)((uintptr_t)((uintptr_t)&ae_static_tmp_lit[0]) + 0) = (uint32_t)r_v0;
    r_v0 = r_s2 & 0x20U;
L_7b648:
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_v0 = r_s2 & 0x8U;
    if (mips_cond) goto L_7b664;
    r_v1 = 0; /* HI16 .bss anchor */
    r_v0 = *(uint32_t *)(uintptr_t)((uintptr_t)&ae_hist_tmp_lit[0]);
    r_v0 = r_v0 + 0x1U;
    *(uint32_t *)((uintptr_t)((uintptr_t)&ae_hist_tmp_lit[0]) + 0) = (uint32_t)r_v0;
    r_v0 = r_s2 & 0x8U;
L_7b664:
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_v1 = 0; /* HI16 .bss anchor */
    if (mips_cond) goto L_7b678;
    r_v0 = *(uint32_t *)(uintptr_t)((uintptr_t)&awb_static_tmp_lit[0]);
    r_v0 = r_v0 + 0x1U;
    *(uint32_t *)((uintptr_t)((uintptr_t)&awb_static_tmp_lit[0]) + 0) = (uint32_t)r_v0;
L_7b678:
    /* cut-point 3: s5 error/overflow + timestamps + per-bit static counters complete */
    if (regtrace_oem_isr_lit_cut && regtrace_oem_isr_lit_cut <= 3) { r_v0 = 1U; goto fn_exit; }
    mips_cond = (r_s4 == 0) ? 1U : 0U;
    r_v1 = *(uint32_t *)(uintptr_t)(r_sp + 64);
    if (mips_cond) goto L_7b7f8;
    r_a0 = 0; /* HI16 .bss anchor */
    r_v0 = *(uint32_t *)(uintptr_t)((uintptr_t)&isp_ch0_frm_done_lit);
    r_s7 = *(uint32_t *)(uintptr_t)(r_s3 + 628);
    r_s5 = 0; /* HI16 .bss anchor */
    r_v0 = r_v0 + 0x1U;
    *(uint32_t *)((uintptr_t)((uintptr_t)&isp_ch0_frm_done_lit) + 0) = (uint32_t)r_v0;
    r_v0 = (uint32_t)(uintptr_t)&update_isp_osd; /* HI16 update_isp_osd */
    r_v0 = (uint32_t)(uintptr_t)&update_isp_osd;
    r_s4 = *(uint8_t *)(uintptr_t)(r_s7 + 211);
    r_v0 = (uint32_t)REGCALL_update_isp_osd(r_a0, r_a1, r_a2, r_a3);
    r_a0 = *(uint8_t *)(uintptr_t)((uintptr_t)&frame_vb_measure_lit);
    r_v0 = 2U;
    mips_cond = (r_a0 != r_v0) ? 1U : 0U;
    r_a0 = 0; /* HI16 .bss anchor */
    if (mips_cond) goto L_7b6d0;
    r_v0 = (uint32_t)(uintptr_t)&private_do_gettimeofday; /* HI16 private_do_gettimeofday */
    r_v0 = (uint32_t)(uintptr_t)&private_do_gettimeofday;
    r_a0 = (uint32_t)(uintptr_t)&tv_frame_end_lit[0];
    r_v0 = (uint32_t)REGCALL_private_do_gettimeofday(r_a0, r_a1, r_a2, r_a3);
    r_v0 = 3U;
    *(uint8_t *)((uintptr_t)((uintptr_t)&frame_vb_measure_lit) + 0) = (uint8_t)r_v0;
L_7b6d0:
    r_a0 = 0; /* HI16 .bss anchor */
    r_v0 = *(uint32_t *)(uintptr_t)((uintptr_t)&saveflag_lit);
    mips_cond = (r_v0 != 0) ? 1U : 0U;
    r_s5 = r_a0;
    if (mips_cond) goto L_7c00c;
    r_v0 = 0; /* HI16 .bss anchor */
    r_a1 = *(uint8_t *)(uintptr_t)((uintptr_t)&frame_use_num_lit[0]);
    mips_cond = (r_a1 == 0) ? 1U : 0U;
    r_a0 = 0x10000U;
    if (mips_cond) goto L_7b710;
    r_v0 = *(uint32_t *)(uintptr_t)(r_s0 + 232);
    r_a2 = 4294967294U;
    r_v0 = r_v0 + r_a0;
    r_a0 = *(uint32_t *)(uintptr_t)(r_v0 + 24584);
    r_a0 = r_a0 & r_a2;
    *(uint32_t *)(uintptr_t)(r_v0 + 24584) = (uint32_t)r_a0;
    r_v0 = 0; /* HI16 .bss anchor */
    *(uint8_t *)((uintptr_t)((uintptr_t)&frame_use_num_tmp_lit[0]) + 0) = (uint8_t)r_a1;
L_7b710:
    r_v0 = *(uint8_t *)(uintptr_t)(r_s7 + 210);
    r_v0 = r_v0 + 0x1U;
    r_v0 = r_v0 & 0xffU;
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    *(uint8_t *)(uintptr_t)(r_s7 + 210) = (uint8_t)r_v0;
    if (mips_cond) goto L_7b7e4;
    r_v0 = 216U;
    r_v1 = (uint32_t)((int32_t)r_s4 * (int32_t)r_v0);
    r_s4 = r_v1 + r_s7;
    r_v0 = *(uint8_t *)(uintptr_t)(r_s4 + 210);
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_v0 = 0U; /* .text anchor; target bound at call site */
    if (mips_cond) goto L_7b7e8;
    r_s5 = *(uint8_t *)(uintptr_t)(r_s7 + 208);
    r_v0 = r_s7;
    r_a0 = 3U;
    if (r_s5 == 0) r_v0 = r_s4;
    r_s5 = r_v0;
    r_v0 = *(uint8_t *)(uintptr_t)(r_v0 + 209);
    r_s6 = (uint32_t)(uintptr_t)&Tzn_Msca_addr_fifo_read; /* HI16 Tzn_Msca_addr_fifo_read */
    r_s6 = (uint32_t)(uintptr_t)&Tzn_Msca_addr_fifo_read;
    if (r_a0) { mips_lo = (uint32_t)(r_v0 / r_a0); mips_hi = (uint32_t)(r_v0 % r_a0); }
    r_s8 = (uint32_t)(uintptr_t)&ispcore_sd; /* HI16 ispcore_sd */
    r_v0 = mips_hi;
    r_v0 = r_v0 & 0xffU;
    *(uint32_t *)(uintptr_t)(r_sp + 72) = (uint32_t)r_v0;
    r_v0 = 0; /* HI16 .bss anchor */
    r_v1 = mips_lo;
    *(uint32_t *)(uintptr_t)(r_sp + 76) = (uint32_t)r_v0;
    r_v0 = 0; /* HI16 .bss anchor */
    *(uint32_t *)(uintptr_t)(r_sp + 68) = (uint32_t)r_v1;
    *(uint32_t *)(uintptr_t)(r_sp + 80) = (uint32_t)r_v0;
L_7b788:
    r_v0 = *(uint32_t *)(uintptr_t)(r_s0 + 232);
    r_a0 = 0x10000U;
    r_v0 = r_v0 + r_a0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_v0 + 24956);
    r_v0 = r_v0 & 0x1U;
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_a1 = 0x3000000U;
    if (mips_cond) goto L_7bf40;
    r_a0 = *(uint32_t *)(uintptr_t)(r_s5 + 128);
    r_v0 = (uint32_t)(uintptr_t)&tx_isp_send_event_to_remote; /* HI16 tx_isp_send_event_to_remote */
    r_a2 = r_sp + 0x20U;
    r_v0 = (uint32_t)(uintptr_t)&tx_isp_send_event_to_remote;
    r_a1 = r_a1 + 0x6U;
    r_v0 = (uint32_t)REGCALL_tx_isp_send_event_to_remote(r_a0, r_a1, r_a2, r_a3);
    r_v0 = *(uint8_t *)(uintptr_t)(r_s7 + 210);
    r_v0 = r_v0 + -0x1U;
    *(uint8_t *)(uintptr_t)(r_s7 + 210) = (uint8_t)r_v0;
    r_v0 = *(uint8_t *)(uintptr_t)(r_s7 + 212);
    r_v0 = (r_v0 < (uint32_t)2) ? 1U : 0U;
    mips_cond = (r_v0 != 0) ? 1U : 0U;
    r_v0 = 0U; /* .text anchor; target bound at call site */
    if (mips_cond) goto L_7b7e8;
    r_v0 = *(uint8_t *)(uintptr_t)(r_s4 + 210);
    r_v0 = r_v0 + -0x1U;
    *(uint8_t *)(uintptr_t)(r_s4 + 210) = (uint8_t)r_v0;
L_7b7e4:
    r_v0 = 0U; /* .text anchor; target bound at call site */
L_7b7e8:
    r_v0 = 0U; /* .text anchor; target bound at call site */
    r_a0 = r_s3;
    r_v0 = (uint32_t)ispcore_irqstatus_func((uintptr_t)r_a0);
    r_v1 = *(uint32_t *)(uintptr_t)(r_sp + 64);
L_7b7f8:
    /* cut-point 4: main-channel frame-done walk complete (sec walks follow) */
    if (regtrace_oem_isr_lit_cut && regtrace_oem_isr_lit_cut <= 4) { r_v0 = 1U; goto fn_exit; }
    r_v0 = 0x2000000U;
    r_v0 = r_v1 & r_v0;
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    *(uint32_t *)(uintptr_t)(r_sp + 64) = (uint32_t)r_v0;
    if (mips_cond) goto L_7b838;
    r_a0 = 0x10000U;
    r_v0 = (uint32_t)(uintptr_t)&system_reg_read; /* HI16 system_reg_read */
    r_v0 = (uint32_t)(uintptr_t)&system_reg_read;
    r_a0 = r_a0 + 0x6008U;
    r_v0 = (uint32_t)REGCALL_system_reg_read(r_a0, r_a1, r_a2, r_a3);
    r_v0 = r_v0 & 0x1U;
    mips_cond = (r_v0 != 0) ? 1U : 0U;
    r_v0 = r_s2 & 0x2U;
    if (mips_cond) goto L_7b83c;
    r_v0 = 0U; /* .text anchor; target bound at call site */
    r_v0 = 0U; /* .text anchor; target bound at call site */
    r_a0 = r_s3;
    r_v0 = (uint32_t)ispcore_irqstatus_func((uintptr_t)r_a0);
L_7b838:
    r_v0 = r_s2 & 0x2U;
L_7b83c:
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_v0 = r_s2 & 0x4U;
    if (mips_cond) goto L_7b98c;
    r_v0 = 0; /* HI16 .bss anchor */
    r_v0 = *(uint32_t *)(uintptr_t)((uintptr_t)&saveflag_lit);
    mips_cond = (r_v0 != 0) ? 1U : 0U;
    r_v0 = r_s2 & 0x4U;
    if (mips_cond) goto L_7b98c;
    r_v0 = 0; /* HI16 .bss anchor */
    r_s4 = *(uint32_t *)(uintptr_t)(r_s3 + 628);
    r_a0 = *(uint8_t *)(uintptr_t)((uintptr_t)&frame_use_num_lit[1]);
    mips_cond = (r_a0 == 0) ? 1U : 0U;
    r_s8 = *(uint8_t *)(uintptr_t)(r_s4 + 427);
    if (mips_cond) goto L_7b88c;
    r_v0 = *(uint32_t *)(uintptr_t)(r_s0 + 232);
    r_v1 = 0x10000U;
    r_a1 = 4294967293U;
    r_v0 = r_v0 + r_v1;
    r_v1 = *(uint32_t *)(uintptr_t)(r_v0 + 24584);
    r_v1 = r_v1 & r_a1;
    *(uint32_t *)(uintptr_t)(r_v0 + 24584) = (uint32_t)r_v1;
    r_v0 = 0; /* HI16 .bss anchor */
    *(uint8_t *)((uintptr_t)((uintptr_t)&frame_use_num_tmp_lit[1]) + 0) = (uint8_t)r_a0;
L_7b88c:
    r_v0 = *(uint8_t *)(uintptr_t)(r_s4 + 426);
    r_v0 = r_v0 + 0x1U;
    r_v0 = r_v0 & 0xffU;
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    *(uint8_t *)(uintptr_t)(r_s4 + 426) = (uint8_t)r_v0;
    if (mips_cond) goto L_7b988;
    r_v0 = 216U;
    r_v1 = (uint32_t)((int32_t)r_s8 * (int32_t)r_v0);
    r_s8 = r_v1 + r_s4;
    r_v0 = *(uint8_t *)(uintptr_t)(r_s8 + 210);
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_v0 = r_s2 & 0x4U;
    if (mips_cond) goto L_7b98c;
    r_v0 = *(uint8_t *)(uintptr_t)(r_s4 + 424);
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_s6 = r_s8;
    if (mips_cond) goto L_7b8c8;
    r_s6 = r_s4 + 0xd8U;
L_7b8c8:
    r_s7 = *(uint8_t *)(uintptr_t)(r_s6 + 209);
    r_a0 = 3U;
    r_v1 = *(uint8_t *)(uintptr_t)(r_s4 + 428);
    if (r_a0) { mips_lo = (uint32_t)(r_s7 / r_a0); mips_hi = (uint32_t)(r_s7 % r_a0); }
    r_v1 = (r_v1 < (uint32_t)2) ? 1U : 0U;
    r_s5 = mips_hi;
    r_v0 = r_s5 & 0xffU;
    *(uint32_t *)(uintptr_t)(r_sp + 68) = (uint32_t)r_v0;
    r_v0 = (uint32_t)(uintptr_t)&Tzn_Msca_addr_fifo_read; /* HI16 Tzn_Msca_addr_fifo_read */
    r_s7 = mips_lo;
    mips_cond = (r_v1 != 0) ? 1U : 0U;
    r_s5 = (uint32_t)(uintptr_t)&Tzn_Msca_addr_fifo_read;
    if (mips_cond) goto L_7b92c;
    r_a1 = *(uint32_t *)(uintptr_t)(r_s4 + 328);
    r_v1 = *(uint8_t *)(uintptr_t)(r_s4 + 425);
    mips_cond = (r_a1 != r_v1) ? 1U : 0U;
    /* nop */
    if (mips_cond) goto L_7b90c;
    r_a1 = *(uint32_t *)(uintptr_t)(r_s8 + 112);
L_7b90c:
    if (r_a0) { mips_lo = (uint32_t)((int32_t)r_a1 / (int32_t)r_a0); mips_hi = (uint32_t)((int32_t)r_a1 % (int32_t)r_a0); }
    r_a3 = r_sp + 0x2cU;
    r_a2 = r_sp + 0x28U;
    r_a1 = mips_hi;
    r_a1 = r_a1 & 0xffU;
    r_a0 = mips_lo;
    r_a0 = r_a0 & 0xffU;
    r_v0 = (uint32_t)REGCALL_isp_printf(r_a0, r_a1, r_a2, r_a3);
L_7b92c:
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 68);
    r_a3 = r_sp + 0x2cU;
    r_a2 = r_sp + 0x28U;
    r_a0 = r_s7;
    *(uint8_t *)(uintptr_t)(r_sp + 60) = (uint8_t)0;
    r_v0 = (uint32_t)REGCALL_isp_printf(r_a0, r_a1, r_a2, r_a3);
    r_a0 = *(uint32_t *)(uintptr_t)(r_s6 + 128);
    r_a1 = 0x3000000U;
    r_v0 = (uint32_t)(uintptr_t)&tx_isp_send_event_to_remote; /* HI16 tx_isp_send_event_to_remote */
    r_a2 = r_sp + 0x20U;
    r_v0 = (uint32_t)(uintptr_t)&tx_isp_send_event_to_remote;
    r_a1 = r_a1 + 0x6U;
    r_v0 = (uint32_t)REGCALL_tx_isp_send_event_to_remote(r_a0, r_a1, r_a2, r_a3);
    r_v0 = *(uint8_t *)(uintptr_t)(r_s4 + 426);
    r_v0 = r_v0 + -0x1U;
    *(uint8_t *)(uintptr_t)(r_s4 + 426) = (uint8_t)r_v0;
    r_v0 = *(uint8_t *)(uintptr_t)(r_s4 + 428);
    r_v0 = (r_v0 < (uint32_t)2) ? 1U : 0U;
    mips_cond = (r_v0 != 0) ? 1U : 0U;
    r_v0 = r_s2 & 0x4U;
    if (mips_cond) goto L_7b98c;
    r_v0 = *(uint8_t *)(uintptr_t)(r_s8 + 210);
    r_v0 = r_v0 + -0x1U;
    *(uint8_t *)(uintptr_t)(r_s8 + 210) = (uint8_t)r_v0;
L_7b988:
    r_v0 = r_s2 & 0x4U;
L_7b98c:
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_v0 = r_s1 & 0x1U;
    if (mips_cond) goto L_7ba18;
    r_v0 = 0; /* HI16 .bss anchor */
    r_v0 = *(uint32_t *)(uintptr_t)((uintptr_t)&saveflag_lit);
    mips_cond = (r_v0 != 0) ? 1U : 0U;
    r_v0 = r_s1 & 0x1U;
    if (mips_cond) goto L_7ba18;
    r_v0 = 0; /* HI16 .bss anchor */
    r_v1 = *(uint8_t *)(uintptr_t)((uintptr_t)&frame_use_num_lit[2]);
    mips_cond = (r_v1 == 0) ? 1U : 0U;
    r_s4 = *(uint32_t *)(uintptr_t)(r_s3 + 628);
    if (mips_cond) goto L_7b9d8;
    r_v0 = *(uint32_t *)(uintptr_t)(r_s0 + 232);
    r_a0 = 0x10000U;
    r_a1 = 4294967291U;
    r_v0 = r_v0 + r_a0;
    r_a0 = *(uint32_t *)(uintptr_t)(r_v0 + 24584);
    r_a0 = r_a0 & r_a1;
    *(uint32_t *)(uintptr_t)(r_v0 + 24584) = (uint32_t)r_a0;
    r_v0 = 0; /* HI16 .bss anchor */
    *(uint8_t *)((uintptr_t)((uintptr_t)&frame_use_num_tmp_lit[2]) + 0) = (uint8_t)r_v1;
L_7b9d8:
    r_v0 = (uint32_t)(uintptr_t)&Tzn_Msca_addr_fifo_read; /* HI16 Tzn_Msca_addr_fifo_read */
    r_a2 = r_sp + 0x28U;
    r_a1 = 2U;
    r_a0 = 0;
    r_v0 = (uint32_t)(uintptr_t)&Tzn_Msca_addr_fifo_read;
    r_a3 = r_sp + 0x2cU;
    *(uint8_t *)(uintptr_t)(r_sp + 60) = (uint8_t)r_v1;
    r_v0 = (uint32_t)REGCALL_Tzn_Msca_addr_fifo_read(r_a0, r_a1, r_a2, r_a3);
    r_a0 = *(uint32_t *)(uintptr_t)(r_s4 + 560);
    r_a1 = 0x3000000U;
    r_v0 = (uint32_t)(uintptr_t)&tx_isp_send_event_to_remote; /* HI16 tx_isp_send_event_to_remote */
    r_a2 = r_sp + 0x20U;
    r_v0 = (uint32_t)(uintptr_t)&tx_isp_send_event_to_remote;
    r_a1 = r_a1 + 0x6U;
    r_v0 = (uint32_t)REGCALL_tx_isp_send_event_to_remote(r_a0, r_a1, r_a2, r_a3);
    r_v0 = r_s1 & 0x1U;
L_7ba18:
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_v0 = *(uint32_t *)(uintptr_t)(r_sp + 64);
    if (mips_cond) goto L_7bb4c;
    r_v0 = 0; /* HI16 .bss anchor */
    r_v0 = (uint32_t)(uintptr_t)&isp_ch0_frm_done_lit;
    r_a0 = *(uint32_t *)(uintptr_t)(r_v0 + 4);
    r_s7 = *(uint32_t *)(uintptr_t)(r_s3 + 628);
    r_a0 = r_a0 + 0x1U;
    *(uint32_t *)(uintptr_t)(r_v0 + 4) = (uint32_t)r_a0;
    r_v0 = 0; /* HI16 .bss anchor */
    r_a1 = *(uint8_t *)(uintptr_t)((uintptr_t)&frame_use_num_lit[3]);
    mips_cond = (r_a1 == 0) ? 1U : 0U;
    r_s4 = *(uint8_t *)(uintptr_t)(r_s7 + 859);
    if (mips_cond) goto L_7ba6c;
    r_v0 = *(uint32_t *)(uintptr_t)(r_s0 + 232);
    r_a0 = 0x10000U;
    r_a2 = 4294967294U;
    r_v0 = r_v0 + r_a0;
    r_a0 = *(uint32_t *)(uintptr_t)(r_v0 + 28680);
    r_a0 = r_a0 & r_a2;
    *(uint32_t *)(uintptr_t)(r_v0 + 28680) = (uint32_t)r_a0;
    r_v0 = 0; /* HI16 .bss anchor */
    *(uint8_t *)((uintptr_t)((uintptr_t)&frame_use_num_tmp_lit[3]) + 0) = (uint8_t)r_a1;
L_7ba6c:
    r_v0 = *(uint8_t *)(uintptr_t)(r_s7 + 858);
    r_v0 = r_v0 + 0x1U;
    r_v0 = r_v0 & 0xffU;
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    *(uint8_t *)(uintptr_t)(r_s7 + 858) = (uint8_t)r_v0;
    if (mips_cond) goto L_7bb38;
    r_v0 = 216U;
    r_v1 = (uint32_t)((int32_t)r_s4 * (int32_t)r_v0);
    r_s4 = r_v1 + r_s7;
    r_v0 = *(uint8_t *)(uintptr_t)(r_s4 + 210);
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_v0 = 0U; /* .text anchor; target bound at call site */
    if (mips_cond) goto L_7bb3c;
    r_v0 = *(uint8_t *)(uintptr_t)(r_s7 + 856);
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_s6 = r_s4;
    if (mips_cond) goto L_7baa8;
    r_s6 = r_s7 + 0x288U;
L_7baa8:
    r_v0 = *(uint8_t *)(uintptr_t)(r_s6 + 209);
    r_a0 = 3U;
    r_s5 = (uint32_t)(uintptr_t)&Tzn_Msca_addr_fifo_read; /* HI16 Tzn_Msca_addr_fifo_read */
    if (r_a0) { mips_lo = (uint32_t)(r_v0 / r_a0); mips_hi = (uint32_t)(r_v0 % r_a0); }
    r_s5 = (uint32_t)(uintptr_t)&Tzn_Msca_addr_fifo_read;
    r_s8 = 0; /* HI16 .bss anchor */
    r_v0 = mips_hi;
    r_v0 = r_v0 & 0xffU;
    *(uint32_t *)(uintptr_t)(r_sp + 72) = (uint32_t)r_v0;
    r_v1 = mips_lo;
    r_v0 = 0; /* HI16 .bss anchor */
    *(uint32_t *)(uintptr_t)(r_sp + 68) = (uint32_t)r_v1;
    *(uint32_t *)(uintptr_t)(r_sp + 76) = (uint32_t)r_v0;
L_7badc:
    r_v0 = *(uint32_t *)(uintptr_t)(r_s0 + 232);
    r_a0 = 0x10000U;
    r_v0 = r_v0 + r_a0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_v0 + 29052);
    r_v0 = r_v0 & 0x1U;
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_a1 = 0x3000000U;
    if (mips_cond) goto L_7c0cc;
    r_a0 = *(uint32_t *)(uintptr_t)(r_s6 + 128);
    r_v0 = (uint32_t)(uintptr_t)&tx_isp_send_event_to_remote; /* HI16 tx_isp_send_event_to_remote */
    r_a2 = r_sp + 0x20U;
    r_v0 = (uint32_t)(uintptr_t)&tx_isp_send_event_to_remote;
    r_a1 = r_a1 + 0x6U;
    r_v0 = (uint32_t)REGCALL_tx_isp_send_event_to_remote(r_a0, r_a1, r_a2, r_a3);
    r_v0 = *(uint8_t *)(uintptr_t)(r_s7 + 858);
    r_v0 = r_v0 + -0x1U;
    *(uint8_t *)(uintptr_t)(r_s7 + 858) = (uint8_t)r_v0;
    r_v0 = *(uint8_t *)(uintptr_t)(r_s7 + 860);
    r_v0 = (r_v0 < (uint32_t)2) ? 1U : 0U;
    mips_cond = (r_v0 != 0) ? 1U : 0U;
    r_v0 = 0U; /* .text anchor; target bound at call site */
    if (mips_cond) goto L_7bb3c;
    r_v0 = *(uint8_t *)(uintptr_t)(r_s4 + 210);
    r_v0 = r_v0 + -0x1U;
    *(uint8_t *)(uintptr_t)(r_s4 + 210) = (uint8_t)r_v0;
L_7bb38:
    r_v0 = 0U; /* .text anchor; target bound at call site */
L_7bb3c:
    r_v0 = 0U; /* .text anchor; target bound at call site */
    r_a0 = r_s3;
    r_v0 = (uint32_t)ispcore_irqstatusSec_func((uintptr_t)r_a0);
    r_v0 = *(uint32_t *)(uintptr_t)(r_sp + 64);
L_7bb4c:
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_v0 = r_s1 & 0x2U;
    if (mips_cond) goto L_7bb88;
    r_a0 = 0x10000U;
    r_v0 = (uint32_t)(uintptr_t)&system_reg_read; /* HI16 system_reg_read */
    r_v0 = (uint32_t)(uintptr_t)&system_reg_read;
    r_a0 = r_a0 + 0x7008U;
    r_v0 = (uint32_t)REGCALL_system_reg_read(r_a0, r_a1, r_a2, r_a3);
    r_v0 = r_v0 & 0x1U;
    mips_cond = (r_v0 != 0) ? 1U : 0U;
    r_v0 = r_s1 & 0x2U;
    if (mips_cond) goto L_7bb88;
    r_v0 = 0U; /* .text anchor; target bound at call site */
    r_v0 = 0U; /* .text anchor; target bound at call site */
    r_a0 = r_s3;
    r_v0 = (uint32_t)ispcore_irqstatusSec_func((uintptr_t)r_a0);
    r_v0 = r_s1 & 0x2U;
L_7bb88:
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_v0 = r_s1 & 0x4U;
    if (mips_cond) goto L_7bcd8;
    r_v0 = 0; /* HI16 .bss anchor */
    r_v0 = *(uint32_t *)(uintptr_t)((uintptr_t)&saveflag_lit);
    mips_cond = (r_v0 != 0) ? 1U : 0U;
    r_v0 = r_s1 & 0x4U;
    if (mips_cond) goto L_7bcd8;
    r_v0 = 0; /* HI16 .bss anchor */
    r_s4 = *(uint32_t *)(uintptr_t)(r_s3 + 628);
    r_a0 = *(uint8_t *)(uintptr_t)((uintptr_t)&frame_use_num_lit[4]);
    mips_cond = (r_a0 == 0) ? 1U : 0U;
    r_s8 = *(uint8_t *)(uintptr_t)(r_s4 + 1075);
    if (mips_cond) goto L_7bbd8;
    r_v0 = *(uint32_t *)(uintptr_t)(r_s0 + 232);
    r_v1 = 0x10000U;
    r_a1 = 4294967293U;
    r_v0 = r_v0 + r_v1;
    r_v1 = *(uint32_t *)(uintptr_t)(r_v0 + 28680);
    r_v1 = r_v1 & r_a1;
    *(uint32_t *)(uintptr_t)(r_v0 + 28680) = (uint32_t)r_v1;
    r_v0 = 0; /* HI16 .bss anchor */
    *(uint8_t *)((uintptr_t)((uintptr_t)&frame_use_num_tmp_lit[4]) + 0) = (uint8_t)r_a0;
L_7bbd8:
    r_v0 = *(uint8_t *)(uintptr_t)(r_s4 + 1074);
    r_v0 = r_v0 + 0x1U;
    r_v0 = r_v0 & 0xffU;
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    *(uint8_t *)(uintptr_t)(r_s4 + 1074) = (uint8_t)r_v0;
    if (mips_cond) goto L_7bcd4;
    r_v0 = 216U;
    r_v1 = (uint32_t)((int32_t)r_s8 * (int32_t)r_v0);
    r_s8 = r_v1 + r_s4;
    r_v0 = *(uint8_t *)(uintptr_t)(r_s8 + 210);
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_v0 = r_s1 & 0x4U;
    if (mips_cond) goto L_7bcd8;
    r_v0 = *(uint8_t *)(uintptr_t)(r_s4 + 1072);
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_s6 = r_s8;
    if (mips_cond) goto L_7bc14;
    r_s6 = r_s4 + 0x360U;
L_7bc14:
    r_s7 = *(uint8_t *)(uintptr_t)(r_s6 + 209);
    r_a1 = 3U;
    r_v1 = *(uint8_t *)(uintptr_t)(r_s4 + 1076);
    if (r_a1) { mips_lo = (uint32_t)(r_s7 / r_a1); mips_hi = (uint32_t)(r_s7 % r_a1); }
    r_v1 = (r_v1 < (uint32_t)2) ? 1U : 0U;
    r_s5 = mips_hi;
    r_v0 = r_s5 & 0xffU;
    *(uint32_t *)(uintptr_t)(r_sp + 64) = (uint32_t)r_v0;
    r_v0 = (uint32_t)(uintptr_t)&Tzn_Msca_addr_fifo_read; /* HI16 Tzn_Msca_addr_fifo_read */
    r_s7 = mips_lo;
    mips_cond = (r_v1 != 0) ? 1U : 0U;
    r_s5 = (uint32_t)(uintptr_t)&Tzn_Msca_addr_fifo_read;
    if (mips_cond) goto L_7bc78;
    r_a0 = *(uint32_t *)(uintptr_t)(r_s4 + 976);
    r_v1 = *(uint8_t *)(uintptr_t)(r_s4 + 1073);
    mips_cond = (r_a0 != r_v1) ? 1U : 0U;
    /* nop */
    if (mips_cond) goto L_7bc58;
    r_a0 = *(uint32_t *)(uintptr_t)(r_s8 + 112);
L_7bc58:
    if (r_a1) { mips_lo = (uint32_t)((int32_t)r_a0 / (int32_t)r_a1); mips_hi = (uint32_t)((int32_t)r_a0 % (int32_t)r_a1); }
    r_a3 = r_sp + 0x2cU;
    r_a2 = r_sp + 0x28U;
    r_a1 = mips_hi;
    r_a1 = r_a1 & 0xffU;
    r_a0 = mips_lo;
    r_a0 = r_a0 & 0xffU;
    r_v0 = (uint32_t)REGCALL_Tzn_Msca_addr_fifo_read(r_a0, r_a1, r_a2, r_a3);
L_7bc78:
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 64);
    r_a3 = r_sp + 0x2cU;
    r_a2 = r_sp + 0x28U;
    r_a0 = r_s7;
    *(uint8_t *)(uintptr_t)(r_sp + 60) = (uint8_t)0;
    r_v0 = (uint32_t)REGCALL_Tzn_Msca_addr_fifo_read(r_a0, r_a1, r_a2, r_a3);
    r_a0 = *(uint32_t *)(uintptr_t)(r_s6 + 128);
    r_a1 = 0x3000000U;
    r_v0 = (uint32_t)(uintptr_t)&tx_isp_send_event_to_remote; /* HI16 tx_isp_send_event_to_remote */
    r_a2 = r_sp + 0x20U;
    r_v0 = (uint32_t)(uintptr_t)&tx_isp_send_event_to_remote;
    r_a1 = r_a1 + 0x6U;
    r_v0 = (uint32_t)REGCALL_tx_isp_send_event_to_remote(r_a0, r_a1, r_a2, r_a3);
    r_v0 = *(uint8_t *)(uintptr_t)(r_s4 + 1074);
    r_v0 = r_v0 + -0x1U;
    *(uint8_t *)(uintptr_t)(r_s4 + 1074) = (uint8_t)r_v0;
    r_v0 = *(uint8_t *)(uintptr_t)(r_s4 + 1076);
    r_v0 = (r_v0 < (uint32_t)2) ? 1U : 0U;
    mips_cond = (r_v0 != 0) ? 1U : 0U;
    r_v0 = r_s1 & 0x4U;
    if (mips_cond) goto L_7bcd8;
    r_v0 = *(uint8_t *)(uintptr_t)(r_s8 + 210);
    r_v0 = r_v0 + -0x1U;
    *(uint8_t *)(uintptr_t)(r_s8 + 210) = (uint8_t)r_v0;
L_7bcd4:
    r_v0 = r_s1 & 0x4U;
L_7bcd8:
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_v0 = r_s1 & 0x200U;
    if (mips_cond) goto L_7bd64;
    r_v0 = 0; /* HI16 .bss anchor */
    r_v0 = *(uint32_t *)(uintptr_t)((uintptr_t)&saveflag_lit);
    mips_cond = (r_v0 != 0) ? 1U : 0U;
    r_v0 = r_s1 & 0x200U;
    if (mips_cond) goto L_7bd64;
    r_v0 = 0; /* HI16 .bss anchor */
    r_v1 = *(uint8_t *)(uintptr_t)((uintptr_t)&frame_use_num_lit[5]);
    mips_cond = (r_v1 == 0) ? 1U : 0U;
    r_s3 = *(uint32_t *)(uintptr_t)(r_s3 + 628);
    if (mips_cond) goto L_7bd24;
    r_v0 = *(uint32_t *)(uintptr_t)(r_s0 + 232);
    r_a0 = 0x10000U;
    r_a1 = 4294967291U;
    r_v0 = r_v0 + r_a0;
    r_a0 = *(uint32_t *)(uintptr_t)(r_v0 + 28680);
    r_a0 = r_a0 & r_a1;
    *(uint32_t *)(uintptr_t)(r_v0 + 28680) = (uint32_t)r_a0;
    r_v0 = 0; /* HI16 .bss anchor */
    *(uint8_t *)((uintptr_t)((uintptr_t)&frame_use_num_tmp_lit[5]) + 0) = (uint8_t)r_v1;
L_7bd24:
    r_v0 = (uint32_t)(uintptr_t)&Tzn_Msca_addr_fifo_read; /* HI16 Tzn_Msca_addr_fifo_read */
    r_a2 = r_sp + 0x28U;
    r_a1 = 2U;
    r_a0 = 1U;
    r_v0 = (uint32_t)(uintptr_t)&Tzn_Msca_addr_fifo_read;
    r_a3 = r_sp + 0x2cU;
    *(uint8_t *)(uintptr_t)(r_sp + 60) = (uint8_t)r_v1;
    r_v0 = (uint32_t)REGCALL_Tzn_Msca_addr_fifo_read(r_a0, r_a1, r_a2, r_a3);
    r_a0 = *(uint32_t *)(uintptr_t)(r_s3 + 1208);
    r_a1 = 0x3000000U;
    r_v0 = (uint32_t)(uintptr_t)&tx_isp_send_event_to_remote; /* HI16 tx_isp_send_event_to_remote */
    r_a2 = r_sp + 0x20U;
    r_v0 = (uint32_t)(uintptr_t)&tx_isp_send_event_to_remote;
    r_a1 = r_a1 + 0x6U;
    r_v0 = (uint32_t)REGCALL_tx_isp_send_event_to_remote(r_a0, r_a1, r_a2, r_a3);
    r_v0 = r_s1 & 0x200U;
L_7bd64:
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_v0 = r_s1 & 0x400U;
    if (mips_cond) goto L_7bd84;
    r_v0 = 0; /* HI16 .bss anchor */
    r_v0 = (uint32_t)(uintptr_t)&adr_static_tmp_lit[0];
    r_v1 = *(uint32_t *)(uintptr_t)(r_v0 + 4);
    r_v1 = r_v1 + 0x1U;
    *(uint32_t *)(uintptr_t)(r_v0 + 4) = (uint32_t)r_v1;
    r_v0 = r_s1 & 0x400U;
L_7bd84:
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_v0 = r_s1 & 0x100U;
    if (mips_cond) goto L_7bda4;
    r_v0 = 0; /* HI16 .bss anchor */
    r_v0 = (uint32_t)(uintptr_t)&defog_static_tmp_lit[0];
    r_v1 = *(uint32_t *)(uintptr_t)(r_v0 + 4);
    r_v1 = r_v1 + 0x1U;
    *(uint32_t *)(uintptr_t)(r_v0 + 4) = (uint32_t)r_v1;
    r_v0 = r_s1 & 0x100U;
L_7bda4:
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_v0 = r_s1 & 0x10U;
    if (mips_cond) goto L_7bdc4;
    r_v0 = 0; /* HI16 .bss anchor */
    r_v0 = (uint32_t)(uintptr_t)&lce_static_tmp_lit[0];
    r_v1 = *(uint32_t *)(uintptr_t)(r_v0 + 4);
    r_v1 = r_v1 + 0x1U;
    *(uint32_t *)(uintptr_t)(r_v0 + 4) = (uint32_t)r_v1;
    r_v0 = r_s1 & 0x10U;
L_7bdc4:
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_v0 = r_s1 & 0x20U;
    if (mips_cond) goto L_7be5c;
    r_v0 = 0; /* HI16 .bss anchor */
    r_v0 = (uint32_t)(uintptr_t)&ae_static_tmp_lit[0];
    r_v1 = *(uint32_t *)(uintptr_t)(r_v0 + 4);
    r_a1 = *(uint32_t *)(uintptr_t)(r_s0 + 232);
    r_a3 = 0; /* HI16 .bss anchor */
    r_v1 = r_v1 + 0x1U;
    *(uint32_t *)(uintptr_t)(r_v0 + 4) = (uint32_t)r_v1;
    r_v0 = 0x10000U;
    r_v0 = r_a1 + r_v0;
    r_a2 = *(uint32_t *)(uintptr_t)(r_v0 + 28680);
    r_v1 = 0; /* HI16 .bss anchor */
    r_v1 = (uint32_t)(uintptr_t)&frame_use_num_tmp_lit[0];
    r_a0 = 0;
    r_a3 = (uint32_t)(uintptr_t)&frame_use_num_lit[0];
    r_t1 = 1U;
    r_t0 = 3U;
L_7be0c:
    r_v0 = *(uint8_t *)(uintptr_t)(r_v1 + 3);
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_v0 = r_v0 + -0x1U;
    if (mips_cond) goto L_7be40;
    r_v0 = r_v0 & 0xffU;
    mips_cond = (r_v0 != 0) ? 1U : 0U;
    *(uint8_t *)(uintptr_t)(r_v1 + 3) = (uint8_t)r_v0;
    if (mips_cond) goto L_7be40;
    r_v0 = r_a3 + r_a0;
    r_t2 = *(uint8_t *)(uintptr_t)(r_v0 + 3);
    mips_cond = (r_t2 == 0) ? 1U : 0U;
    /* nop */
    if (mips_cond) goto L_7be40;
    r_t2 = r_t1 << (r_a0 & 31);
    r_a2 = r_a2 | r_t2;
    *(uint8_t *)(uintptr_t)(r_v0 + 3) = (uint8_t)0;
L_7be40:
    r_a0 = r_a0 + 0x1U;
    mips_cond = (r_a0 != r_t0) ? 1U : 0U;
    r_v1 = r_v1 + 0x1U;
    if (mips_cond) goto L_7be0c;
    r_v0 = 0x10000U;
    r_a1 = r_a1 + r_v0;
    *(uint32_t *)(uintptr_t)(r_a1 + 28680) = (uint32_t)r_a2;
    r_v0 = r_s1 & 0x20U;
L_7be5c:
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_v0 = r_s1 & 0x8U;
    if (mips_cond) goto L_7be7c;
    r_v0 = 0; /* HI16 .bss anchor */
    r_v0 = (uint32_t)(uintptr_t)&ae_hist_tmp_lit[0];
    r_v1 = *(uint32_t *)(uintptr_t)(r_v0 + 4);
    r_v1 = r_v1 + 0x1U;
    *(uint32_t *)(uintptr_t)(r_v0 + 4) = (uint32_t)r_v1;
    r_v0 = r_s1 & 0x8U;
L_7be7c:
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_s0 = (uint32_t)(uintptr_t)&irq_func_cb; /* HI16 irq_func_cb */
    if (mips_cond) goto L_7be98;
    r_v0 = 0; /* HI16 .bss anchor */
    r_v0 = (uint32_t)(uintptr_t)&awb_static_tmp_lit[0];
    r_v1 = *(uint32_t *)(uintptr_t)(r_v0 + 4);
    r_v1 = r_v1 + 0x1U;
    *(uint32_t *)(uintptr_t)(r_v0 + 4) = (uint32_t)r_v1;
L_7be98:
    r_s0 = (uint32_t)(uintptr_t)&irq_func_cb;
    r_s4 = 0;
    r_s3 = 1U;
    r_s7 = 1U;
    r_s6 = 32U;
L_7beac:
    r_s5 = r_s7 << (r_s4 & 31);
    r_v0 = r_s5 & r_s2;
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    /* nop */
    if (mips_cond) goto L_7bed8;
    r_v0 = *(uint32_t *)(uintptr_t)(r_s0 + 0);
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    /* nop */
    if (mips_cond) goto L_7bed8;
    /* nop */
    { uint32_t __cb = r_v0; r_v0 = __cb ? ((uint32_t (*)(void))(uintptr_t)__cb)() : 0U; } /* irq_func_cb dispatch */
    r_v1 = r_v0 ^ 0x1U;
    if (r_v1 != 0) r_s3 = r_v0;
L_7bed8:
    r_s5 = r_s5 & r_s1;
    mips_cond = (r_s5 == 0) ? 1U : 0U;
    /* nop */
    if (mips_cond) goto L_7bf00;
    r_v0 = *(uint32_t *)(uintptr_t)(r_s0 + 168);
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    /* nop */
    if (mips_cond) goto L_7bf00;
    /* nop */
    { uint32_t __cb = r_v0; r_v0 = __cb ? ((uint32_t (*)(void))(uintptr_t)__cb)() : 0U; } /* irq_func_cb dispatch */
    r_v1 = r_v0 ^ 0x1U;
    if (r_v1 != 0) r_s3 = r_v0;
L_7bf00:
    r_s4 = r_s4 + 0x1U;
    mips_cond = (r_s4 != r_s6) ? 1U : 0U;
    r_s0 = r_s0 + 0x4U;
    if (mips_cond) goto L_7beac;
    r_ra = *(uint32_t *)(uintptr_t)(r_sp + 124);
    r_v0 = r_s3;
    r_s8 = *(uint32_t *)(uintptr_t)(r_sp + 120);
    r_s7 = *(uint32_t *)(uintptr_t)(r_sp + 116);
    r_s6 = *(uint32_t *)(uintptr_t)(r_sp + 112);
    r_s5 = *(uint32_t *)(uintptr_t)(r_sp + 108);
    r_s4 = *(uint32_t *)(uintptr_t)(r_sp + 104);
    r_s3 = *(uint32_t *)(uintptr_t)(r_sp + 100);
    r_s2 = *(uint32_t *)(uintptr_t)(r_sp + 96);
    r_s1 = *(uint32_t *)(uintptr_t)(r_sp + 92);
    r_s0 = *(uint32_t *)(uintptr_t)(r_sp + 88);
    r_sp = r_sp + 0x80U;
    goto fn_exit;
L_7bf40:
    r_v0 = *(uint8_t *)(uintptr_t)(r_s7 + 212);
    r_v0 = (r_v0 < (uint32_t)2) ? 1U : 0U;
    mips_cond = (r_v0 != 0) ? 1U : 0U;
    r_a0 = *(uint32_t *)(uintptr_t)(r_sp + 68);
    if (mips_cond) goto L_7bf88;
    r_a0 = *(uint32_t *)(uintptr_t)(r_s7 + 112);
    r_v0 = *(uint8_t *)(uintptr_t)(r_s7 + 209);
    mips_cond = (r_a0 != r_v0) ? 1U : 0U;
    r_a1 = 3U;
    if (mips_cond) goto L_7bf64;
    r_a0 = *(uint32_t *)(uintptr_t)(r_s4 + 112);
L_7bf64:
    if (r_a1) { mips_lo = (uint32_t)((int32_t)r_a0 / (int32_t)r_a1); mips_hi = (uint32_t)((int32_t)r_a0 % (int32_t)r_a1); }
    r_a3 = r_sp + 0x2cU;
    r_a2 = r_sp + 0x28U;
    r_a1 = mips_hi;
    r_a1 = r_a1 & 0xffU;
    r_a0 = mips_lo;
    r_a0 = r_a0 & 0xffU;
    r_v0 = (uint32_t)REGCALL_Tzn_Msca_addr_fifo_read(r_a0, r_a1, r_a2, r_a3);
    r_a0 = *(uint32_t *)(uintptr_t)(r_sp + 68);
L_7bf88:
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 72);
    r_a3 = r_sp + 0x2cU;
    r_a2 = r_sp + 0x28U;
    r_v0 = (uint32_t)REGCALL_Tzn_Msca_addr_fifo_read(r_a0, r_a1, r_a2, r_a3);
    r_v0 = *(uint32_t *)(uintptr_t)(r_sp + 40);
    r_a0 = 0xc0000000U;
    r_v0 = (r_v0 < r_a0) ? 1U : 0U;
    mips_cond = (r_v0 != 0) ? 1U : 0U;
    r_v0 = *(uint32_t *)(uintptr_t)(r_s8 + 0);
    if (mips_cond) goto L_7bfc8;
    r_v0 = *(uint32_t *)(uintptr_t)(r_sp + 76);
    r_v0 = *(uint32_t *)(uintptr_t)((uintptr_t)&tmp_addr_lit);
    *(uint32_t *)(uintptr_t)(r_sp + 40) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_sp + 80);
    r_v0 = *(uint32_t *)(uintptr_t)((uintptr_t)&tmp_addr_uv_lit);
    *(uint32_t *)(uintptr_t)(r_sp + 44) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_s8 + 0);
L_7bfc8:
    *(uint32_t *)(uintptr_t)(r_sp + 48) = (uint32_t)0;
    r_a0 = 0x10000U;
    r_v0 = *(uint32_t *)(uintptr_t)(r_v0 + 232);
    r_a1 = 96U;
    r_v0 = r_v0 + r_a0;
    r_a0 = *(uint32_t *)(uintptr_t)(r_v0 + 26460);
    r_v0 = *(uint32_t *)(uintptr_t)(r_s5 + 120);
    r_a0 = r_a0 >> 16;
    r_v1 = (uint32_t)((int32_t)r_v0 * (int32_t)r_a1);
    r_v0 = r_v1 + r_s3;
    r_v0 = *(uint32_t *)(uintptr_t)(r_v0 + 380);
    *(uint32_t *)(uintptr_t)(r_sp + 52) = (uint32_t)0;
    *(uint8_t *)(uintptr_t)(r_sp + 60) = (uint8_t)0;
    r_v0 = r_v0 << 16;
    r_v0 = r_v0 | r_a0;
    *(uint32_t *)(uintptr_t)(r_sp + 56) = (uint32_t)r_v0;
    goto L_7b788;
L_7c00c:
    r_a0 = 1U;
    mips_cond = (r_v0 != r_a0) ? 1U : 0U;
    r_v0 = 0U; /* .text anchor; target bound at call site */
    if (mips_cond) goto L_7b7e8;
    r_s4 = 0; /* HI16 .bss anchor */
    r_v0 = *(uint32_t *)(uintptr_t)((uintptr_t)&irq_count_lit);
    mips_cond = (r_v0 != 0) ? 1U : 0U;
    r_v0 = 0; /* HI16 .bss anchor */
    if (mips_cond) goto L_7c050;
    r_a3 = 0; /* HI16 .bss anchor */
    r_a2 = 0; /* HI16 .bss anchor */
    r_v0 = (uint32_t)(uintptr_t)&Tzn_Msca_addr_fifo_read; /* HI16 Tzn_Msca_addr_fifo_read */
    r_a3 = (uint32_t)(uintptr_t)&tmp_addr_uv_lit;
    r_a2 = (uint32_t)(uintptr_t)&tmp_addr_lit;
    r_a1 = 0;
    r_v0 = (uint32_t)(uintptr_t)&Tzn_Msca_addr_fifo_read;
    r_a0 = 0;
    r_v0 = (uint32_t)REGCALL_Tzn_Msca_addr_fifo_read(r_a0, r_a1, r_a2, r_a3);
    r_v0 = 0; /* HI16 .bss anchor */
L_7c050:
    r_a2 = *(uint32_t *)(uintptr_t)((uintptr_t)&irq_count_lit);
    r_a0 = *(uint32_t *)(uintptr_t)((uintptr_t)&savenum_lit);
    r_t0 = (uint32_t)(uintptr_t)&Tzn_Msca_addr_fifo_write; /* HI16 Tzn_Msca_addr_fifo_write */
    mips_cond = (r_a2 != r_a0) ? 1U : 0U;
    r_t0 = (uint32_t)(uintptr_t)&Tzn_Msca_addr_fifo_write;
    if (mips_cond) goto L_7c09c;
    *(uint32_t *)((uintptr_t)((uintptr_t)&savenum_lit) + 0) = (uint32_t)0;
    r_v0 = 0; /* HI16 .bss anchor */
    r_a3 = *(uint32_t *)(uintptr_t)((uintptr_t)&tmp_addr_uv_lit);
    r_v0 = 0; /* HI16 .bss anchor */
    r_a2 = *(uint32_t *)(uintptr_t)((uintptr_t)&tmp_addr_lit);
    *(uint32_t *)((uintptr_t)((uintptr_t)&saveflag_lit) + 0) = (uint32_t)0;
    *(uint32_t *)((uintptr_t)((uintptr_t)&irq_count_lit) + 0) = (uint32_t)0;
L_7c080:
    r_a1 = 0;
    r_a0 = 0;
    r_v0 = (uint32_t)REGCALL_Tzn_Msca_addr_fifo_write(r_a0, r_a1, r_a2, r_a3);
    r_v0 = *(uint32_t *)(uintptr_t)((uintptr_t)&irq_count_lit);
    r_v0 = r_v0 + 0x1U;
    *(uint32_t *)((uintptr_t)((uintptr_t)&irq_count_lit) + 0) = (uint32_t)r_v0;
    goto L_7b7e4;
L_7c09c:
    r_v0 = *(uint32_t *)(uintptr_t)(r_s7 + 8);
    r_a3 = *(uint32_t *)(uintptr_t)(r_s7 + 4);
    r_a3 = (uint32_t)((int32_t)r_a3 * (int32_t)r_v0);
    r_v0 = r_a3 << 1;
    r_v0 = r_v0 + r_a3;
    r_v0 = r_v0 >> 1;
    r_v0 = (uint32_t)((int32_t)r_v0 * (int32_t)r_a2);
    r_a2 = 0xc0000000U;
    r_a3 = r_a3 + r_a2;
    r_a3 = r_a3 + r_v0;
    r_a2 = r_v0 + r_a2;
    goto L_7c080;
L_7c0cc:
    r_v0 = *(uint8_t *)(uintptr_t)(r_s7 + 860);
    r_v0 = (r_v0 < (uint32_t)2) ? 1U : 0U;
    mips_cond = (r_v0 != 0) ? 1U : 0U;
    r_a0 = *(uint32_t *)(uintptr_t)(r_sp + 68);
    if (mips_cond) goto L_7c114;
    r_a0 = *(uint32_t *)(uintptr_t)(r_s7 + 760);
    r_v0 = *(uint8_t *)(uintptr_t)(r_s7 + 857);
    mips_cond = (r_a0 != r_v0) ? 1U : 0U;
    r_a1 = 3U;
    if (mips_cond) goto L_7c0f0;
    r_a0 = *(uint32_t *)(uintptr_t)(r_s4 + 112);
L_7c0f0:
    if (r_a1) { mips_lo = (uint32_t)((int32_t)r_a0 / (int32_t)r_a1); mips_hi = (uint32_t)((int32_t)r_a0 % (int32_t)r_a1); }
    r_a3 = r_sp + 0x2cU;
    r_a2 = r_sp + 0x28U;
    r_a1 = mips_hi;
    r_a1 = r_a1 & 0xffU;
    r_a0 = mips_lo;
    r_a0 = r_a0 & 0xffU;
    r_v0 = (uint32_t)REGCALL_Tzn_Msca_addr_fifo_read(r_a0, r_a1, r_a2, r_a3);
    r_a0 = *(uint32_t *)(uintptr_t)(r_sp + 68);
L_7c114:
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 72);
    r_a3 = r_sp + 0x2cU;
    r_a2 = r_sp + 0x28U;
    r_v0 = (uint32_t)REGCALL_Tzn_Msca_addr_fifo_read(r_a0, r_a1, r_a2, r_a3);
    r_v0 = *(uint32_t *)(uintptr_t)(r_sp + 40);
    r_a0 = 0xc0000000U;
    r_v0 = (r_v0 < r_a0) ? 1U : 0U;
    mips_cond = (r_v0 != 0) ? 1U : 0U;
    r_v0 = *(uint32_t *)(uintptr_t)(r_sp + 76);
    if (mips_cond) goto L_7c148;
    r_v0 = *(uint32_t *)(uintptr_t)((uintptr_t)&tmp_addr_lit);
    *(uint32_t *)(uintptr_t)(r_sp + 40) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)((uintptr_t)&tmp_addr_uv_lit);
    *(uint32_t *)(uintptr_t)(r_sp + 44) = (uint32_t)r_v0;
L_7c148:
    *(uint8_t *)(uintptr_t)(r_sp + 60) = (uint8_t)0;
    goto L_7badc;
fn_exit:
    return (int32_t)r_v0;
}
