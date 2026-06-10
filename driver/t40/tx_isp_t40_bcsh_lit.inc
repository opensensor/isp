/*
 * Generated T40 BCSH literal chain from OEM tx-isp-t40.ko.
 * Keep this self-contained and gated; native recovered BCSH has GP-relative
 * decompiler artifacts and is intentionally not called by bring-up.
 */
static bool regtrace_enable_bcsh;
static uint32_t regtrace_bcsh_mode = 2; /* OEM tisp_main_init passes 2 */
module_param_named(enable_bcsh, regtrace_enable_bcsh, bool, 0644);
module_param_named(bcsh_mode, regtrace_bcsh_mode, uint, 0644);

static uint32_t pstMainBcshOri_lit[72];
static uintptr_t stMainBcshOuter_lit;
static uint8_t s_bcsh_mjpeg_mode_lit[2];
static uint8_t s_bcsh_mjpeg_y_range_low_lit[2];

static unsigned char __attribute__((aligned(4))) s_bcsh_mjpeg_y_range_high_lit[16] = {
    0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
static const unsigned char __attribute__((aligned(4))) bcsh_rgb2yuv_base_matrix_lit[36] = {
    0x01, 0x04, 0x00, 0x00, 0x04, 0xfe, 0xff, 0xff, 0xf3, 0xfe, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff,
    0x05, 0x09, 0x00, 0x00, 0x5f, 0x01, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0xf1, 0x02, 0x00, 0x00,
    0x9b, 0x0a, 0x00, 0x00,
};
static unsigned char __attribute__((aligned(4))) stMainBcshInterOri_lit[460] = {
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x04, 0x00, 0x00, 0x04, 0x3e, 0x00, 0x00, 0xf3, 0x3e, 0x00, 0x00, 0xfe, 0x3f, 0x00, 0x00,
    0x05, 0x09, 0x00, 0x00, 0x5f, 0x01, 0x00, 0x00, 0xfe, 0x3f, 0x00, 0x00, 0xf1, 0x02, 0x00, 0x00,
    0x9b, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
    0x00, 0x05, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
    0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
    0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
    0x22, 0x0b, 0x00, 0x00, 0xa6, 0x0e, 0x00, 0x00, 0x0a, 0x0f, 0x00, 0x00, 0x56, 0x13, 0x00, 0x00,
    0x3c, 0x80, 0x00, 0x00, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0xff, 0x03, 0x00, 0x00, 0xff, 0x03,
    0x00, 0x00, 0xff, 0x03, 0x00, 0x00, 0xff, 0x03, 0x00, 0x00, 0xff, 0x03, 0x00, 0x00, 0xff, 0x03,
    0x6d, 0x36, 0x00, 0x00, 0x17, 0xb7, 0x00, 0x00, 0x7c, 0x12, 0x00, 0x00, 0xaa, 0xe2, 0xff, 0xff,
    0x56, 0x9d, 0xff, 0xff, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0xba, 0x8b, 0xff, 0xff,
    0x46, 0xf4, 0xff, 0xff, 0x00, 0x00, 0x01, 0x00, 0xf6, 0xff, 0xff, 0xff, 0x24, 0x93, 0x01, 0x00,
    0x00, 0x00, 0x01, 0x00, 0x0e, 0xd0, 0xff, 0xff, 0x29, 0x88, 0xff, 0xff, 0x00, 0x00, 0x01, 0x00,
    0x09, 0xdb, 0x01, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x90, 0x01, 0x00, 0x00, 0x90, 0x01, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x70, 0x17, 0x70, 0x17,
    0x64, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00,
};

static int32_t tisp_bcsh_reg2para_lit(uint32_t a0_in, uint32_t a1_in);
static int32_t tisp_bcsh_para2reg_lit(uint32_t a0_in, uint32_t a1_in);
static int32_t tisp_bcsh_Toffset_RGBYUV_lit(uint32_t a0_in, uint32_t a1_in, uint32_t a2_in);
static int32_t tisp_bcsh_Toffset_RGB2YUV_lit(uint32_t a0_in, uint32_t a1_in, uint32_t a2_in);
static int32_t tisp_bcsh_Tccm_Comp2Orig_lit(uint32_t a0_in);
static int32_t tisp_bcsh_Tccm_RGBYUV_lit(uint32_t a0_in, uint32_t a1_in, uint32_t a2_in, uint32_t a3_in, uint32_t a4_in);
static int32_t tisp_bcsh_Tccm_RGB2YUV_lit(uint32_t a0_in);
static int32_t tisp_bcsh_ct_interpolation_lit(uint32_t a0_in);
static int32_t tisp_bcsh_ct_set_parameter_lit(uint32_t a0_in, uint32_t a1_in);
static int32_t tisp_bcsh_ev_update_TransitParam_lit(uint32_t a0_in, uint32_t a1_in);
static int32_t tisp_bcsh_ev_set_parameter_lit(uint32_t a0_in, uint32_t a1_in);
static int32_t tisp_bcsh_ev_update_lit(uint32_t a0_in, uint32_t a1_in);
static int32_t tisp_bcsh_ct_update_lit(uint32_t a0_in, uint32_t a1_in);
static int32_t tisp_bcsh_params_transmit_lit(uint32_t a0_in, uint32_t a1_in, uint32_t a2_in);
static int32_t tisp_bcsh_params_refresh_lit(uint32_t a0_in, uint32_t a1_in, uint32_t a2_in);

static inline uint32_t bcsh_abs_u32(uint32_t a, uint32_t b)
{
    return (a < b) ? (b - a) : (a - b);
}

static uint32_t tisp_bcsh_StrenCal_part0_lit(uint32_t a0, uint32_t a1, uint32_t a2,
                                             uint32_t a3, uint32_t a4)
{
    uint32_t d10 = bcsh_abs_u32(a1, a0);
    uint32_t d12 = bcsh_abs_u32(a1, a2);
    uint32_t d34 = bcsh_abs_u32(a3, a4);
    uint32_t num;

    if (!d12)
        return a3;
    num = d12 * a3 + d10 * d34;
    return num / d12;
}

static uint32_t tisp_bcsh_StrenCal_lit(uint32_t a0, uint32_t a1, uint32_t a2,
                                       uint32_t a3, uint32_t a4, uint32_t a5)
{
    uint32_t d02;
    uint32_t d12;
    uint32_t d34;
    uint32_t num;

    if (!a5)
        return tisp_bcsh_StrenCal_part0_lit(a0, a1, a2, a3, a4);
    d02 = bcsh_abs_u32(a0, a2);
    d12 = bcsh_abs_u32(a1, a2);
    d34 = bcsh_abs_u32(a3, a4);
    if (!d12)
        return a4;
    num = d12 * a4 + d02 * d34;
    return num / d12;
}

#undef REGCALL_system_reg_write
#undef REGCALL_memcpy
#undef REGCALL_fix_point_mult2_32
#undef REGCALL_tisp_bcsh_reg2para
#undef REGCALL_tisp_bcsh_para2reg
#undef REGCALL_tisp_bcsh_Toffset_RGBYUV
#undef REGCALL_tisp_bcsh_Toffset_RGB2YUV
#undef REGCALL_tisp_bcsh_Tccm_Comp2Orig
#undef REGCALL_tisp_bcsh_Tccm_RGBYUV
#undef REGCALL_tisp_bcsh_Tccm_RGB2YUV
#undef REGCALL_tisp_bcsh_ct_interpolation
#undef REGCALL_tisp_bcsh_ct_set_parameter
#undef REGCALL_tisp_bcsh_ev_update_TransitParam
#undef REGCALL_tisp_bcsh_ev_set_parameter
#undef REGCALL_tisp_bcsh_ev_update
#undef REGCALL_tisp_bcsh_ct_update
#undef REGCALL_tisp_bcsh_StrenCal
#undef REGCALL_tisp_bcsh_StrenCal_part_0
#define REGCALL_system_reg_write(a, b, c, d) system_reg_write((a), (b))
#define REGCALL_memcpy(a, b, c, d)     ((uint32_t)(uintptr_t)memcpy((void *)(uintptr_t)(a),                                  (const void *)(uintptr_t)(b), (size_t)(c)))
#define REGCALL_fix_point_mult2_32(a, b, c, d) ((uint32_t)fix_point_mult2_32((a), (b), (c)))
#define REGCALL_tisp_bcsh_reg2para(a, b, c, d) ((uint32_t)tisp_bcsh_reg2para_lit((a), (b)))
#define REGCALL_tisp_bcsh_para2reg(a, b, c, d) ((uint32_t)tisp_bcsh_para2reg_lit((a), (b)))
#define REGCALL_tisp_bcsh_Toffset_RGBYUV(a, b, c, d) ((uint32_t)tisp_bcsh_Toffset_RGBYUV_lit((a), (b), (c)))
#define REGCALL_tisp_bcsh_Toffset_RGB2YUV(a, b, c, d) ((uint32_t)tisp_bcsh_Toffset_RGB2YUV_lit((a), (b), (c)))
#define REGCALL_tisp_bcsh_Tccm_Comp2Orig(a, b, c, d) ((uint32_t)tisp_bcsh_Tccm_Comp2Orig_lit((a)))
#define REGCALL_tisp_bcsh_Tccm_RGBYUV(a, b, c, d)     ((uint32_t)tisp_bcsh_Tccm_RGBYUV_lit((a), (b), (c), (d),         *(uint32_t *)(uintptr_t)(r_sp + 16)))
#define REGCALL_tisp_bcsh_Tccm_RGB2YUV(a, b, c, d) ((uint32_t)tisp_bcsh_Tccm_RGB2YUV_lit((a)))
#define REGCALL_tisp_bcsh_ct_interpolation(a, b, c, d) ((uint32_t)tisp_bcsh_ct_interpolation_lit((a)))
#define REGCALL_tisp_bcsh_ct_set_parameter(a, b, c, d) ((uint32_t)tisp_bcsh_ct_set_parameter_lit((a), (b)))
#define REGCALL_tisp_bcsh_ev_update_TransitParam(a, b, c, d) ((uint32_t)tisp_bcsh_ev_update_TransitParam_lit((a), (b)))
#define REGCALL_tisp_bcsh_ev_set_parameter(a, b, c, d) ((uint32_t)tisp_bcsh_ev_set_parameter_lit((a), (b)))
#define REGCALL_tisp_bcsh_ev_update(a, b, c, d) ((uint32_t)tisp_bcsh_ev_update_lit((a), (b)))
#define REGCALL_tisp_bcsh_ct_update(a, b, c, d) ((uint32_t)tisp_bcsh_ct_update_lit((a), (b)))
#define REGCALL_tisp_bcsh_StrenCal(a, b, c, d)     ((uint32_t)tisp_bcsh_StrenCal_lit((a), (b), (c), (d),         *(uint32_t *)(uintptr_t)(r_sp + 16), *(uint32_t *)(uintptr_t)(r_sp + 20)))
#define REGCALL_tisp_bcsh_StrenCal_part_0(a, b, c, d)     ((uint32_t)tisp_bcsh_StrenCal_part0_lit((a), (b), (c), (d),         *(uint32_t *)(uintptr_t)(r_sp + 16)))

static int32_t tisp_bcsh_reg2para_lit(uint32_t a0_in, uint32_t a1_in)
{
    uint32_t r_v0 = 0, r_v1 = 0;
    uint32_t r_at = 0, r_a0 = 0, r_a1 = 0, r_a2 = 0, r_a3 = 0;
    uint32_t r_t0 = 0, r_t1 = 0, r_t2 = 0, r_t3 = 0, r_t4 = 0, r_t5 = 0;
    uint32_t r_t6 = 0, r_t7 = 0, r_t8 = 0, r_t9 = 0;
    uint32_t r_s0 = 0, r_s1 = 0, r_s2 = 0, r_s3 = 0, r_s4 = 0, r_s5 = 0;
    uint32_t r_s6 = 0, r_s7 = 0, r_s8 = 0, r_k0 = 0, r_k1 = 0, r_gp = 0, r_ra = 0;
    uint32_t mips_lo = 0, mips_hi = 0, mips_cond = 0;
    uint8_t mips_stack[1024] __attribute__((aligned(8)));
    uint32_t r_sp = (uint32_t)(uintptr_t)(mips_stack + 512);
    r_a0 = a0_in;
    r_a1 = a1_in;
    (void)r_at; (void)r_k0; (void)r_k1; (void)r_gp; (void)r_t8; (void)r_t9;
    (void)r_ra; (void)mips_hi; (void)mips_lo; (void)mips_cond;
    r_v1 = r_a1 + 0x24U;
L_69a98:
    r_v0 = *(uint32_t *)(uintptr_t)(r_a1 + 0);
    r_a2 = (r_v0 < (uint32_t)8192) ? 1U : 0U;
    mips_cond = (r_a2 != 0) ? 1U : 0U;
    /* nop */
    if (mips_cond) goto L_69aac;
    r_v0 = r_v0 + -0x4000U;
L_69aac:
    r_a1 = r_a1 + 0x4U;
    *(uint32_t *)(uintptr_t)(r_a0 + 0) = (uint32_t)r_v0;
    mips_cond = (r_a1 != r_v1) ? 1U : 0U;
    r_a0 = r_a0 + 0x4U;
    if (mips_cond) goto L_69a98;
    /* nop */
    goto fn_exit;
fn_exit:
    return (int32_t)r_v0;
}

static int32_t tisp_bcsh_para2reg_lit(uint32_t a0_in, uint32_t a1_in)
{
    uint32_t r_v0 = 0, r_v1 = 0;
    uint32_t r_at = 0, r_a0 = 0, r_a1 = 0, r_a2 = 0, r_a3 = 0;
    uint32_t r_t0 = 0, r_t1 = 0, r_t2 = 0, r_t3 = 0, r_t4 = 0, r_t5 = 0;
    uint32_t r_t6 = 0, r_t7 = 0, r_t8 = 0, r_t9 = 0;
    uint32_t r_s0 = 0, r_s1 = 0, r_s2 = 0, r_s3 = 0, r_s4 = 0, r_s5 = 0;
    uint32_t r_s6 = 0, r_s7 = 0, r_s8 = 0, r_k0 = 0, r_k1 = 0, r_gp = 0, r_ra = 0;
    uint32_t mips_lo = 0, mips_hi = 0, mips_cond = 0;
    uint8_t mips_stack[1024] __attribute__((aligned(8)));
    uint32_t r_sp = (uint32_t)(uintptr_t)(mips_stack + 512);
    r_a0 = a0_in;
    r_a1 = a1_in;
    (void)r_at; (void)r_k0; (void)r_k1; (void)r_gp; (void)r_t8; (void)r_t9;
    (void)r_ra; (void)mips_hi; (void)mips_lo; (void)mips_cond;
    r_v1 = r_a1 + 0x24U;
L_6a4d8:
    r_v0 = *(uint32_t *)(uintptr_t)(r_a1 + 0);
    mips_cond = ((int32_t)r_v0 >= 0) ? 1U : 0U;
    /* nop */
    if (mips_cond) goto L_6a4e8;
    r_v0 = r_v0 & 0x3fffU;
L_6a4e8:
    r_a1 = r_a1 + 0x4U;
    *(uint32_t *)(uintptr_t)(r_a0 + 0) = (uint32_t)r_v0;
    mips_cond = (r_a1 != r_v1) ? 1U : 0U;
    r_a0 = r_a0 + 0x4U;
    if (mips_cond) goto L_6a4d8;
    /* nop */
    goto fn_exit;
fn_exit:
    return (int32_t)r_v0;
}

static int32_t tisp_bcsh_Toffset_RGBYUV_lit(uint32_t a0_in, uint32_t a1_in, uint32_t a2_in)
{
    uint32_t r_v0 = 0, r_v1 = 0;
    uint32_t r_at = 0, r_a0 = 0, r_a1 = 0, r_a2 = 0, r_a3 = 0;
    uint32_t r_t0 = 0, r_t1 = 0, r_t2 = 0, r_t3 = 0, r_t4 = 0, r_t5 = 0;
    uint32_t r_t6 = 0, r_t7 = 0, r_t8 = 0, r_t9 = 0;
    uint32_t r_s0 = 0, r_s1 = 0, r_s2 = 0, r_s3 = 0, r_s4 = 0, r_s5 = 0;
    uint32_t r_s6 = 0, r_s7 = 0, r_s8 = 0, r_k0 = 0, r_k1 = 0, r_gp = 0, r_ra = 0;
    uint32_t mips_lo = 0, mips_hi = 0, mips_cond = 0;
    uint8_t mips_stack[1024] __attribute__((aligned(8)));
    uint32_t r_sp = (uint32_t)(uintptr_t)(mips_stack + 512);
    r_a0 = a0_in;
    r_a1 = a1_in;
    r_a2 = a2_in;
    (void)r_at; (void)r_k0; (void)r_k1; (void)r_gp; (void)r_t8; (void)r_t9;
    (void)r_ra; (void)mips_hi; (void)mips_lo; (void)mips_cond;
    r_sp = r_sp + -0xd0U;
    r_v0 = r_sp + 0x58U;
    r_a3 = 4294967295U;
    *(uint32_t *)(uintptr_t)(r_sp + 168) = (uint32_t)r_s0;
    *(uint32_t *)(uintptr_t)(r_sp + 204) = (uint32_t)r_ra;
    r_s0 = r_a0;
    *(uint32_t *)(uintptr_t)(r_sp + 200) = (uint32_t)r_s8;
    *(uint32_t *)(uintptr_t)(r_sp + 196) = (uint32_t)r_s7;
    *(uint32_t *)(uintptr_t)(r_sp + 192) = (uint32_t)r_s6;
    *(uint32_t *)(uintptr_t)(r_sp + 188) = (uint32_t)r_s5;
    *(uint32_t *)(uintptr_t)(r_sp + 184) = (uint32_t)r_s4;
    *(uint32_t *)(uintptr_t)(r_sp + 180) = (uint32_t)r_s3;
    *(uint32_t *)(uintptr_t)(r_sp + 176) = (uint32_t)r_s2;
    *(uint32_t *)(uintptr_t)(r_sp + 172) = (uint32_t)r_s1;
    r_a0 = r_a1 + 0x24U;
    r_t0 = 1U;
L_683a0:
    r_v1 = *(uint32_t *)(uintptr_t)(r_a1 + 0);
    mips_cond = ((int32_t)r_v1 < 0) ? 1U : 0U;
    /* nop */
    if (mips_cond) goto L_685a8;
    *(uint32_t *)(uintptr_t)(r_v0 + 0) = (uint32_t)r_t0;
L_683b0:
    r_a1 = r_a1 + 0x4U;
    *(uint32_t *)(uintptr_t)(r_v0 + 4) = (uint32_t)r_v1;
    mips_cond = (r_a0 != r_a1) ? 1U : 0U;
    r_v0 = r_v0 + 0x8U;
    if (mips_cond) goto L_683a0;
    r_v0 = r_sp + 0x10U;
    r_a0 = r_a2 + 0x24U;
    r_a1 = 4294967295U;
    r_a3 = 1U;
L_683d0:
    r_v1 = *(uint32_t *)(uintptr_t)(r_a2 + 0);
    mips_cond = ((int32_t)r_v1 < 0) ? 1U : 0U;
    /* nop */
    if (mips_cond) goto L_685b4;
    *(uint32_t *)(uintptr_t)(r_v0 + 0) = (uint32_t)r_a3;
L_683e0:
    r_a2 = r_a2 + 0x4U;
    *(uint32_t *)(uintptr_t)(r_v0 + 4) = (uint32_t)r_v1;
    mips_cond = (r_a0 != r_a2) ? 1U : 0U;
    r_v0 = r_v0 + 0x8U;
    if (mips_cond) goto L_683d0;
    r_s4 = *(uint32_t *)(uintptr_t)(r_sp + 16);
    r_s8 = *(uint32_t *)(uintptr_t)(r_sp + 88);
    r_s7 = *(uint32_t *)(uintptr_t)(r_sp + 20);
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 92);
    r_s8 = (uint32_t)((int32_t)r_s4 * (int32_t)r_s8);
    r_s7 = r_s7 << 6;
    r_s1 = (uint32_t)(uintptr_t)&fix_point_mult2_32; /* HI16 fix_point_mult2_32 */
    r_s1 = (uint32_t)(uintptr_t)&fix_point_mult2_32;
    r_a2 = r_s7;
    r_a0 = 16U;
    r_v0 = (uint32_t)REGCALL_fix_point_mult2_32(r_a0, r_a1, r_a2, r_a3);
    r_s3 = *(uint32_t *)(uintptr_t)(r_sp + 24);
    r_s6 = *(uint32_t *)(uintptr_t)(r_sp + 28);
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 100);
    r_a0 = 16U;
    r_s6 = r_s6 << 6;
    r_a2 = r_s6;
    r_s2 = (uint32_t)((int32_t)r_s8 * (int32_t)r_v0);
    r_s8 = *(uint32_t *)(uintptr_t)(r_sp + 96);
    r_v0 = (uint32_t)REGCALL_fix_point_mult2_32(r_a0, r_a1, r_a2, r_a3);
    r_s5 = *(uint32_t *)(uintptr_t)(r_sp + 36);
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 108);
    r_s5 = r_s5 << 6;
    r_a2 = r_s5;
    r_s8 = (uint32_t)((int32_t)r_s3 * (int32_t)r_s8);
    r_a0 = (uint32_t)((int32_t)r_s8 * (int32_t)r_v0);
    r_s8 = *(uint32_t *)(uintptr_t)(r_sp + 104);
    r_v1 = r_a0 + r_s2;
    r_s2 = *(uint32_t *)(uintptr_t)(r_sp + 32);
    r_a0 = 16U;
    *(uint32_t *)(uintptr_t)(r_sp + 160) = (uint32_t)r_v1;
    r_s8 = (uint32_t)((int32_t)r_s2 * (int32_t)r_s8);
    r_v0 = (uint32_t)REGCALL_fix_point_mult2_32(r_a0, r_a1, r_a2, r_a3);
    r_v1 = *(uint32_t *)(uintptr_t)(r_sp + 160);
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 116);
    r_a2 = r_s7;
    r_a0 = (uint32_t)((int32_t)r_s8 * (int32_t)r_v0);
    r_s8 = *(uint32_t *)(uintptr_t)(r_sp + 112);
    r_s8 = (uint32_t)((int32_t)r_s4 * (int32_t)r_s8);
    r_v0 = r_a0 + r_v1;
    *(uint32_t *)(uintptr_t)(r_s0 + 0) = (uint32_t)r_v0;
    r_a0 = 16U;
    r_v0 = (uint32_t)REGCALL_fix_point_mult2_32(r_a0, r_a1, r_a2, r_a3);
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 124);
    r_a2 = r_s6;
    r_a0 = 16U;
    r_v1 = (uint32_t)((int32_t)r_s8 * (int32_t)r_v0);
    r_s8 = *(uint32_t *)(uintptr_t)(r_sp + 120);
    r_s8 = (uint32_t)((int32_t)r_s3 * (int32_t)r_s8);
    *(uint32_t *)(uintptr_t)(r_sp + 160) = (uint32_t)r_v1;
    r_v0 = (uint32_t)REGCALL_fix_point_mult2_32(r_a0, r_a1, r_a2, r_a3);
    r_v1 = *(uint32_t *)(uintptr_t)(r_sp + 160);
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 132);
    r_a2 = r_s5;
    r_a0 = (uint32_t)((int32_t)r_s8 * (int32_t)r_v0);
    r_s8 = *(uint32_t *)(uintptr_t)(r_sp + 128);
    r_s8 = (uint32_t)((int32_t)r_s2 * (int32_t)r_s8);
    r_v1 = r_a0 + r_v1;
    r_a0 = 16U;
    *(uint32_t *)(uintptr_t)(r_sp + 160) = (uint32_t)r_v1;
    r_v0 = (uint32_t)REGCALL_fix_point_mult2_32(r_a0, r_a1, r_a2, r_a3);
    r_v1 = *(uint32_t *)(uintptr_t)(r_sp + 160);
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 140);
    r_a2 = r_s7;
    r_a0 = (uint32_t)((int32_t)r_s8 * (int32_t)r_v0);
    r_v0 = r_a0 + r_v1;
    *(uint32_t *)(uintptr_t)(r_s0 + 4) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_sp + 136);
    r_a0 = 16U;
    r_s4 = (uint32_t)((int32_t)r_s4 * (int32_t)r_v0);
    r_v0 = (uint32_t)REGCALL_fix_point_mult2_32(r_a0, r_a1, r_a2, r_a3);
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 148);
    r_a2 = r_s6;
    r_a0 = 16U;
    r_s4 = (uint32_t)((int32_t)r_s4 * (int32_t)r_v0);
    r_v0 = *(uint32_t *)(uintptr_t)(r_sp + 144);
    r_s3 = (uint32_t)((int32_t)r_s3 * (int32_t)r_v0);
    r_v0 = (uint32_t)REGCALL_fix_point_mult2_32(r_a0, r_a1, r_a2, r_a3);
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 156);
    r_a0 = 16U;
    r_a2 = r_s5;
    r_v1 = (uint32_t)((int32_t)r_s3 * (int32_t)r_v0);
    r_v0 = *(uint32_t *)(uintptr_t)(r_sp + 152);
    r_s2 = (uint32_t)((int32_t)r_s2 * (int32_t)r_v0);
    r_s3 = r_v1 + r_s4;
    r_v0 = (uint32_t)REGCALL_fix_point_mult2_32(r_a0, r_a1, r_a2, r_a3);
    r_a0 = r_s0;
    r_v1 = (uint32_t)((int32_t)r_s2 * (int32_t)r_v0);
    r_v0 = r_v1 + r_s3;
    *(uint32_t *)(uintptr_t)(r_s0 + 8) = (uint32_t)r_v0;
    r_v1 = r_s0 + 0xcU;
L_6855c:
    r_v0 = *(uint32_t *)(uintptr_t)(r_a0 + 0);
    mips_cond = ((int32_t)r_v0 < 0) ? 1U : 0U;
    /* nop */
    if (mips_cond) goto L_685c0;
    r_v0 = (uint32_t)((int32_t)r_v0 >> 6);
L_6856c:
    *(uint32_t *)(uintptr_t)(r_a0 + 0) = (uint32_t)r_v0;
    r_a0 = r_a0 + 0x4U;
    mips_cond = (r_v1 != r_a0) ? 1U : 0U;
    r_ra = *(uint32_t *)(uintptr_t)(r_sp + 204);
    if (mips_cond) goto L_6855c;
    r_s8 = *(uint32_t *)(uintptr_t)(r_sp + 200);
    r_s7 = *(uint32_t *)(uintptr_t)(r_sp + 196);
    r_s6 = *(uint32_t *)(uintptr_t)(r_sp + 192);
    r_s5 = *(uint32_t *)(uintptr_t)(r_sp + 188);
    r_s4 = *(uint32_t *)(uintptr_t)(r_sp + 184);
    r_s3 = *(uint32_t *)(uintptr_t)(r_sp + 180);
    r_s2 = *(uint32_t *)(uintptr_t)(r_sp + 176);
    r_s1 = *(uint32_t *)(uintptr_t)(r_sp + 172);
    r_s0 = *(uint32_t *)(uintptr_t)(r_sp + 168);
    r_sp = r_sp + 0xd0U;
    goto fn_exit;
L_685a8:
    *(uint32_t *)(uintptr_t)(r_v0 + 0) = (uint32_t)r_a3;
    r_v1 = (uint32_t)(-(int32_t)r_v1);
    goto L_683b0;
L_685b4:
    *(uint32_t *)(uintptr_t)(r_v0 + 0) = (uint32_t)r_a1;
    r_v1 = (uint32_t)(-(int32_t)r_v1);
    goto L_683e0;
L_685c0:
    r_v0 = (uint32_t)(-(int32_t)r_v0);
    r_v0 = (uint32_t)((int32_t)r_v0 >> 6);
    r_v0 = (uint32_t)(-(int32_t)r_v0);
    goto L_6856c;
fn_exit:
    return (int32_t)r_v0;
}

static int32_t tisp_bcsh_Toffset_RGB2YUV_lit(uint32_t a0_in, uint32_t a1_in, uint32_t a2_in)
{
    uint32_t r_v0 = 0, r_v1 = 0;
    uint32_t r_at = 0, r_a0 = 0, r_a1 = 0, r_a2 = 0, r_a3 = 0;
    uint32_t r_t0 = 0, r_t1 = 0, r_t2 = 0, r_t3 = 0, r_t4 = 0, r_t5 = 0;
    uint32_t r_t6 = 0, r_t7 = 0, r_t8 = 0, r_t9 = 0;
    uint32_t r_s0 = 0, r_s1 = 0, r_s2 = 0, r_s3 = 0, r_s4 = 0, r_s5 = 0;
    uint32_t r_s6 = 0, r_s7 = 0, r_s8 = 0, r_k0 = 0, r_k1 = 0, r_gp = 0, r_ra = 0;
    uint32_t mips_lo = 0, mips_hi = 0, mips_cond = 0;
    uint8_t mips_stack[1024] __attribute__((aligned(8)));
    uint32_t r_sp = (uint32_t)(uintptr_t)(mips_stack + 512);
    r_a0 = a0_in;
    r_a1 = a1_in;
    r_a2 = a2_in;
    (void)r_at; (void)r_k0; (void)r_k1; (void)r_gp; (void)r_t8; (void)r_t9;
    (void)r_ra; (void)mips_hi; (void)mips_lo; (void)mips_cond;
    r_sp = r_sp + -0x30U;
    r_v0 = r_a1;
    *(uint32_t *)(uintptr_t)(r_sp + 40) = (uint32_t)r_s0;
    *(uint32_t *)(uintptr_t)(r_sp + 44) = (uint32_t)r_ra;
    r_v1 = *(uint32_t *)(uintptr_t)(r_v0 + 0);
    r_s0 = r_a0;
    r_a1 = r_a2;
    r_v1 = r_v1 + -0x400U;
    *(uint32_t *)(uintptr_t)(r_sp + 28) = (uint32_t)r_v1;
    r_v1 = *(uint32_t *)(uintptr_t)(r_v0 + 4);
    r_v0 = *(uint32_t *)(uintptr_t)(r_v0 + 8);
    r_a2 = r_sp + 0x1cU;
    r_v1 = r_v1 + -0x400U;
    r_v0 = r_v0 + -0x400U;
    *(uint32_t *)(uintptr_t)(r_sp + 36) = (uint32_t)r_v0;
    r_v0 = (uint32_t)(uintptr_t)&tisp_bcsh_Toffset_RGBYUV; /* HI16 tisp_bcsh_Toffset_RGBYUV */
    r_a0 = r_sp + 0x10U;
    r_v0 = (uint32_t)(uintptr_t)&tisp_bcsh_Toffset_RGBYUV;
    *(uint32_t *)(uintptr_t)(r_sp + 16) = (uint32_t)0;
    *(uint32_t *)(uintptr_t)(r_sp + 20) = (uint32_t)0;
    *(uint32_t *)(uintptr_t)(r_sp + 24) = (uint32_t)0;
    *(uint32_t *)(uintptr_t)(r_sp + 32) = (uint32_t)r_v1;
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_Toffset_RGBYUV(r_a0, r_a1, r_a2, r_a3);
    r_v0 = *(uint32_t *)(uintptr_t)(r_sp + 16);
    r_v0 = r_v0 + 0x400U;
    *(uint32_t *)(uintptr_t)(r_s0 + 0) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_sp + 20);
    r_v0 = r_v0 + 0x400U;
    *(uint32_t *)(uintptr_t)(r_s0 + 4) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_sp + 24);
    r_v0 = r_v0 + 0x400U;
    *(uint32_t *)(uintptr_t)(r_s0 + 8) = (uint32_t)r_v0;
    r_ra = *(uint32_t *)(uintptr_t)(r_sp + 44);
    r_s0 = *(uint32_t *)(uintptr_t)(r_sp + 40);
    r_sp = r_sp + 0x30U;
    goto fn_exit;
fn_exit:
    return (int32_t)r_v0;
}

static int32_t tisp_bcsh_Tccm_Comp2Orig_lit(uint32_t a0_in)
{
    uint32_t r_v0 = 0, r_v1 = 0;
    uint32_t r_at = 0, r_a0 = 0, r_a1 = 0, r_a2 = 0, r_a3 = 0;
    uint32_t r_t0 = 0, r_t1 = 0, r_t2 = 0, r_t3 = 0, r_t4 = 0, r_t5 = 0;
    uint32_t r_t6 = 0, r_t7 = 0, r_t8 = 0, r_t9 = 0;
    uint32_t r_s0 = 0, r_s1 = 0, r_s2 = 0, r_s3 = 0, r_s4 = 0, r_s5 = 0;
    uint32_t r_s6 = 0, r_s7 = 0, r_s8 = 0, r_k0 = 0, r_k1 = 0, r_gp = 0, r_ra = 0;
    uint32_t mips_lo = 0, mips_hi = 0, mips_cond = 0;
    uint8_t mips_stack[1024] __attribute__((aligned(8)));
    uint32_t r_sp = (uint32_t)(uintptr_t)(mips_stack + 512);
    r_a0 = a0_in;
    (void)r_at; (void)r_k0; (void)r_k1; (void)r_gp; (void)r_t8; (void)r_t9;
    (void)r_ra; (void)mips_hi; (void)mips_lo; (void)mips_cond;
    r_sp = r_sp + -0x18U;
    *(uint32_t *)(uintptr_t)(r_sp + 20) = (uint32_t)r_ra;
    r_a3 = r_a0;
    r_a1 = *(uint32_t *)(uintptr_t)(r_a0 + 196);
    r_t9 = (uint32_t)(uintptr_t)&tisp_bcsh_reg2para; /* HI16 tisp_bcsh_reg2para */
    r_t9 = (uint32_t)(uintptr_t)&tisp_bcsh_reg2para;
    r_a0 = *(uint32_t *)(uintptr_t)(r_a0 + 28);
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_reg2para(r_a0, r_a1, r_a2, r_a3);
    r_a1 = *(uint32_t *)(uintptr_t)(r_a3 + 200);
    r_a0 = *(uint32_t *)(uintptr_t)(r_a3 + 32);
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_reg2para(r_a0, r_a1, r_a2, r_a3);
    r_a1 = *(uint32_t *)(uintptr_t)(r_a3 + 204);
    r_ra = *(uint32_t *)(uintptr_t)(r_sp + 20);
    r_a0 = *(uint32_t *)(uintptr_t)(r_a3 + 36);
    r_sp = r_sp + 0x18U;
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_reg2para(r_a0, r_a1, r_a2, r_a3); goto fn_exit;
fn_exit:
    return (int32_t)r_v0;
}

static int32_t tisp_bcsh_Tccm_RGBYUV_lit(uint32_t a0_in, uint32_t a1_in, uint32_t a2_in, uint32_t a3_in, uint32_t a4_in)
{
    uint32_t r_v0 = 0, r_v1 = 0;
    uint32_t r_at = 0, r_a0 = 0, r_a1 = 0, r_a2 = 0, r_a3 = 0;
    uint32_t r_t0 = 0, r_t1 = 0, r_t2 = 0, r_t3 = 0, r_t4 = 0, r_t5 = 0;
    uint32_t r_t6 = 0, r_t7 = 0, r_t8 = 0, r_t9 = 0;
    uint32_t r_s0 = 0, r_s1 = 0, r_s2 = 0, r_s3 = 0, r_s4 = 0, r_s5 = 0;
    uint32_t r_s6 = 0, r_s7 = 0, r_s8 = 0, r_k0 = 0, r_k1 = 0, r_gp = 0, r_ra = 0;
    uint32_t mips_lo = 0, mips_hi = 0, mips_cond = 0;
    uint8_t mips_stack[1024] __attribute__((aligned(8)));
    uint32_t r_sp = (uint32_t)(uintptr_t)(mips_stack + 512);
    r_a0 = a0_in;
    r_a1 = a1_in;
    r_a2 = a2_in;
    r_a3 = a3_in;
    *(uint32_t *)(uintptr_t)(r_sp + 16) = (uint32_t)a4_in;
    (void)r_at; (void)r_k0; (void)r_k1; (void)r_gp; (void)r_t8; (void)r_t9;
    (void)r_ra; (void)mips_hi; (void)mips_lo; (void)mips_cond;
    r_sp = r_sp + -0x190U;
    *(uint32_t *)(uintptr_t)(r_sp + 364) = (uint32_t)r_s1;
    r_s1 = *(uint8_t *)(uintptr_t)(r_sp + 416);
    *(uint32_t *)(uintptr_t)(r_sp + 376) = (uint32_t)r_s4;
    r_s4 = r_sp + 0xe8U;
    *(uint32_t *)(uintptr_t)(r_sp + 372) = (uint32_t)r_s3;
    *(uint32_t *)(uintptr_t)(r_sp + 396) = (uint32_t)r_ra;
    r_s3 = r_a0;
    *(uint32_t *)(uintptr_t)(r_sp + 392) = (uint32_t)r_s8;
    *(uint32_t *)(uintptr_t)(r_sp + 388) = (uint32_t)r_s7;
    *(uint32_t *)(uintptr_t)(r_sp + 384) = (uint32_t)r_s6;
    *(uint32_t *)(uintptr_t)(r_sp + 380) = (uint32_t)r_s5;
    *(uint32_t *)(uintptr_t)(r_sp + 368) = (uint32_t)r_s2;
    *(uint32_t *)(uintptr_t)(r_sp + 360) = (uint32_t)r_s0;
    r_a0 = r_a1 + 0x24U;
    r_v0 = r_s4;
    r_t0 = 4294967295U;
    r_t1 = 1U;
L_69e2c:
    r_v1 = *(uint32_t *)(uintptr_t)(r_a1 + 0);
    mips_cond = ((int32_t)r_v1 < 0) ? 1U : 0U;
    /* nop */
    if (mips_cond) goto L_6a39c;
    *(uint32_t *)(uintptr_t)(r_v0 + 0) = (uint32_t)r_t1;
L_69e3c:
    r_a1 = r_a1 + 0x4U;
    *(uint32_t *)(uintptr_t)(r_v0 + 4) = (uint32_t)r_v1;
    mips_cond = (r_a0 != r_a1) ? 1U : 0U;
    r_v0 = r_v0 + 0x8U;
    if (mips_cond) goto L_69e2c;
    r_v0 = r_sp + 0xa0U;
    r_a0 = r_a2 + 0x24U;
    r_a1 = 4294967295U;
    r_t0 = 1U;
L_69e5c:
    r_v1 = *(uint32_t *)(uintptr_t)(r_a2 + 0);
    mips_cond = ((int32_t)r_v1 < 0) ? 1U : 0U;
    /* nop */
    if (mips_cond) goto L_6a3a8;
    *(uint32_t *)(uintptr_t)(r_v0 + 0) = (uint32_t)r_t0;
L_69e6c:
    r_a2 = r_a2 + 0x4U;
    *(uint32_t *)(uintptr_t)(r_v0 + 4) = (uint32_t)r_v1;
    mips_cond = (r_a0 != r_a2) ? 1U : 0U;
    r_v0 = r_v0 + 0x8U;
    if (mips_cond) goto L_69e5c;
    r_v0 = r_sp + 0x58U;
    r_a0 = r_a3 + 0x24U;
    r_a1 = 4294967295U;
    r_a2 = 1U;
L_69e8c:
    r_v1 = *(uint32_t *)(uintptr_t)(r_a3 + 0);
    mips_cond = ((int32_t)r_v1 < 0) ? 1U : 0U;
    /* nop */
    if (mips_cond) goto L_6a3b4;
    *(uint32_t *)(uintptr_t)(r_v0 + 0) = (uint32_t)r_a2;
L_69e9c:
    r_a3 = r_a3 + 0x4U;
    *(uint32_t *)(uintptr_t)(r_v0 + 4) = (uint32_t)r_v1;
    mips_cond = (r_a0 != r_a3) ? 1U : 0U;
    r_v0 = r_v0 + 0x8U;
    if (mips_cond) goto L_69e8c;
    r_v0 = r_s3 + 0x24U;
    r_s0 = (uint32_t)(uintptr_t)&fix_point_mult2_32; /* HI16 fix_point_mult2_32 */
    r_s2 = r_s3;
    *(uint32_t *)(uintptr_t)(r_sp + 328) = (uint32_t)r_v0;
    r_s5 = r_s3;
    r_s0 = (uint32_t)(uintptr_t)&fix_point_mult2_32;
L_69ec4:
    r_v1 = *(uint32_t *)(uintptr_t)(r_s4 + 0);
    r_t1 = *(uint32_t *)(uintptr_t)(r_sp + 160);
    r_t5 = *(uint32_t *)(uintptr_t)(r_s4 + 4);
    r_a2 = *(uint32_t *)(uintptr_t)(r_sp + 164);
    r_s6 = (uint32_t)((int32_t)r_v1 * (int32_t)r_t1);
    r_a1 = r_t5;
    r_a2 = r_a2 << 6;
    r_a0 = 16U;
    *(uint32_t *)(uintptr_t)(r_sp + 352) = (uint32_t)r_v1;
    *(uint32_t *)(uintptr_t)(r_sp + 348) = (uint32_t)r_t5;
    r_v0 = (uint32_t)REGCALL_fix_point_mult2_32(r_a0, r_a1, r_a2, r_a3);
    r_t0 = *(uint32_t *)(uintptr_t)(r_s4 + 8);
    r_t6 = *(uint32_t *)(uintptr_t)(r_sp + 184);
    r_s8 = *(uint32_t *)(uintptr_t)(r_s4 + 12);
    r_a2 = *(uint32_t *)(uintptr_t)(r_sp + 188);
    r_a0 = 16U;
    r_a1 = r_s8;
    r_a2 = r_a2 << 6;
    *(uint32_t *)(uintptr_t)(r_sp + 340) = (uint32_t)r_t0;
    r_t1 = (uint32_t)((int32_t)r_s6 * (int32_t)r_v0);
    r_s6 = (uint32_t)((int32_t)r_t0 * (int32_t)r_t6);
    *(uint32_t *)(uintptr_t)(r_sp + 344) = (uint32_t)r_t1;
    r_v0 = (uint32_t)REGCALL_fix_point_mult2_32(r_a0, r_a1, r_a2, r_a3);
    r_s7 = *(uint32_t *)(uintptr_t)(r_s4 + 16);
    r_t2 = *(uint32_t *)(uintptr_t)(r_sp + 208);
    r_a2 = *(uint32_t *)(uintptr_t)(r_sp + 212);
    r_a0 = 16U;
    r_a2 = r_a2 << 6;
    r_t6 = (uint32_t)((int32_t)r_s6 * (int32_t)r_v0);
    r_s6 = *(uint32_t *)(uintptr_t)(r_s4 + 20);
    r_a1 = r_s6;
    r_t2 = (uint32_t)((int32_t)r_s7 * (int32_t)r_t2);
    *(uint32_t *)(uintptr_t)(r_sp + 336) = (uint32_t)r_t6;
    *(uint32_t *)(uintptr_t)(r_sp + 332) = (uint32_t)r_t2;
    r_v0 = (uint32_t)REGCALL_fix_point_mult2_32(r_a0, r_a1, r_a2, r_a3);
    r_t2 = *(uint32_t *)(uintptr_t)(r_sp + 332);
    r_t6 = *(uint32_t *)(uintptr_t)(r_sp + 336);
    r_t1 = *(uint32_t *)(uintptr_t)(r_sp + 344);
    r_v1 = (uint32_t)((int32_t)r_t2 * (int32_t)r_v0);
    r_t5 = *(uint32_t *)(uintptr_t)(r_sp + 348);
    r_t1 = r_t1 + r_t6;
    r_a2 = *(uint32_t *)(uintptr_t)(r_sp + 172);
    r_a1 = r_t5;
    r_a0 = 16U;
    r_a2 = r_a2 << 6;
    r_v0 = r_v1 + r_t1;
    r_v1 = *(uint32_t *)(uintptr_t)(r_sp + 352);
    r_t1 = *(uint32_t *)(uintptr_t)(r_sp + 168);
    *(uint32_t *)(uintptr_t)(r_s5 + 0) = (uint32_t)r_v0;
    r_t1 = (uint32_t)((int32_t)r_v1 * (int32_t)r_t1);
    *(uint32_t *)(uintptr_t)(r_sp + 332) = (uint32_t)r_t1;
    r_v0 = (uint32_t)REGCALL_fix_point_mult2_32(r_a0, r_a1, r_a2, r_a3);
    r_t0 = *(uint32_t *)(uintptr_t)(r_sp + 340);
    r_t1 = *(uint32_t *)(uintptr_t)(r_sp + 332);
    r_t6 = *(uint32_t *)(uintptr_t)(r_sp + 192);
    r_a2 = *(uint32_t *)(uintptr_t)(r_sp + 196);
    r_t1 = (uint32_t)((int32_t)r_t1 * (int32_t)r_v0);
    r_a1 = r_s8;
    r_a2 = r_a2 << 6;
    r_a0 = 16U;
    r_t6 = (uint32_t)((int32_t)r_t0 * (int32_t)r_t6);
    *(uint32_t *)(uintptr_t)(r_sp + 344) = (uint32_t)r_t1;
    *(uint32_t *)(uintptr_t)(r_sp + 332) = (uint32_t)r_t6;
    r_v0 = (uint32_t)REGCALL_fix_point_mult2_32(r_a0, r_a1, r_a2, r_a3);
    r_t6 = *(uint32_t *)(uintptr_t)(r_sp + 332);
    r_t2 = *(uint32_t *)(uintptr_t)(r_sp + 216);
    r_a2 = *(uint32_t *)(uintptr_t)(r_sp + 220);
    r_t6 = (uint32_t)((int32_t)r_t6 * (int32_t)r_v0);
    r_a1 = r_s6;
    r_a2 = r_a2 << 6;
    r_a0 = 16U;
    r_t2 = (uint32_t)((int32_t)r_s7 * (int32_t)r_t2);
    *(uint32_t *)(uintptr_t)(r_sp + 336) = (uint32_t)r_t6;
    *(uint32_t *)(uintptr_t)(r_sp + 332) = (uint32_t)r_t2;
    r_v0 = (uint32_t)REGCALL_fix_point_mult2_32(r_a0, r_a1, r_a2, r_a3);
    r_t2 = *(uint32_t *)(uintptr_t)(r_sp + 332);
    r_t6 = *(uint32_t *)(uintptr_t)(r_sp + 336);
    r_t1 = *(uint32_t *)(uintptr_t)(r_sp + 344);
    r_v1 = (uint32_t)((int32_t)r_t2 * (int32_t)r_v0);
    r_t5 = *(uint32_t *)(uintptr_t)(r_sp + 348);
    r_t1 = r_t1 + r_t6;
    r_a2 = *(uint32_t *)(uintptr_t)(r_sp + 180);
    r_a1 = r_t5;
    r_a0 = 16U;
    r_a2 = r_a2 << 6;
    r_v0 = r_v1 + r_t1;
    *(uint32_t *)(uintptr_t)(r_s5 + 4) = (uint32_t)r_v0;
    r_v1 = *(uint32_t *)(uintptr_t)(r_sp + 352);
    r_v0 = *(uint32_t *)(uintptr_t)(r_sp + 176);
    r_v1 = (uint32_t)((int32_t)r_v1 * (int32_t)r_v0);
    *(uint32_t *)(uintptr_t)(r_sp + 332) = (uint32_t)r_v1;
    r_v0 = (uint32_t)REGCALL_fix_point_mult2_32(r_a0, r_a1, r_a2, r_a3);
    r_v1 = *(uint32_t *)(uintptr_t)(r_sp + 332);
    r_t0 = *(uint32_t *)(uintptr_t)(r_sp + 340);
    r_a2 = *(uint32_t *)(uintptr_t)(r_sp + 204);
    r_v1 = (uint32_t)((int32_t)r_v1 * (int32_t)r_v0);
    r_v0 = *(uint32_t *)(uintptr_t)(r_sp + 200);
    r_a1 = r_s8;
    r_a2 = r_a2 << 6;
    r_a0 = 16U;
    r_t0 = (uint32_t)((int32_t)r_t0 * (int32_t)r_v0);
    *(uint32_t *)(uintptr_t)(r_sp + 336) = (uint32_t)r_v1;
    *(uint32_t *)(uintptr_t)(r_sp + 332) = (uint32_t)r_t0;
    r_v0 = (uint32_t)REGCALL_fix_point_mult2_32(r_a0, r_a1, r_a2, r_a3);
    r_t0 = *(uint32_t *)(uintptr_t)(r_sp + 332);
    r_a2 = *(uint32_t *)(uintptr_t)(r_sp + 228);
    r_a1 = r_s6;
    r_s8 = (uint32_t)((int32_t)r_t0 * (int32_t)r_v0);
    r_v0 = *(uint32_t *)(uintptr_t)(r_sp + 224);
    r_a2 = r_a2 << 6;
    r_a0 = 16U;
    r_s7 = (uint32_t)((int32_t)r_s7 * (int32_t)r_v0);
    r_v0 = (uint32_t)REGCALL_fix_point_mult2_32(r_a0, r_a1, r_a2, r_a3);
    r_v1 = *(uint32_t *)(uintptr_t)(r_sp + 336);
    r_a1 = r_s5;
    r_a2 = 3U;
    r_v1 = r_v1 + r_s8;
    r_a0 = (uint32_t)((int32_t)r_s7 * (int32_t)r_v0);
    r_v0 = r_sp + 0x130U;
    r_v1 = r_a0 + r_v1;
    *(uint32_t *)(uintptr_t)(r_s5 + 8) = (uint32_t)r_v1;
    r_a0 = 0;
L_6a0ac:
    r_v1 = *(uint32_t *)(uintptr_t)(r_a1 + 0);
    mips_cond = ((int32_t)r_v1 < 0) ? 1U : 0U;
    r_a3 = 1U;
    if (mips_cond) goto L_6a3c0;
    *(uint32_t *)(uintptr_t)(r_v0 + 0) = (uint32_t)r_a3;
L_6a0bc:
    r_a0 = r_a0 + 0x1U;
    *(uint32_t *)(uintptr_t)(r_v0 + 4) = (uint32_t)r_v1;
    r_a1 = r_a1 + 0x4U;
    mips_cond = (r_a0 != r_a2) ? 1U : 0U;
    r_v0 = r_v0 + 0x8U;
    if (mips_cond) goto L_6a0ac;
    r_t5 = *(uint32_t *)(uintptr_t)(r_sp + 308);
    r_t0 = *(uint32_t *)(uintptr_t)(r_sp + 304);
    r_t3 = *(uint32_t *)(uintptr_t)(r_sp + 88);
    r_a2 = *(uint32_t *)(uintptr_t)(r_sp + 92);
    r_a1 = r_t5;
    r_a0 = 16U;
    r_s6 = (uint32_t)((int32_t)r_t0 * (int32_t)r_t3);
    *(uint32_t *)(uintptr_t)(r_sp + 348) = (uint32_t)r_t0;
    *(uint32_t *)(uintptr_t)(r_sp + 344) = (uint32_t)r_t5;
    r_v0 = (uint32_t)REGCALL_fix_point_mult2_32(r_a0, r_a1, r_a2, r_a3);
    r_a3 = *(uint32_t *)(uintptr_t)(r_sp + 312);
    r_t3 = *(uint32_t *)(uintptr_t)(r_sp + 112);
    r_s8 = *(uint32_t *)(uintptr_t)(r_sp + 316);
    r_a2 = *(uint32_t *)(uintptr_t)(r_sp + 116);
    r_a0 = 16U;
    r_a1 = r_s8;
    *(uint32_t *)(uintptr_t)(r_sp + 340) = (uint32_t)r_a3;
    r_s5 = r_s5 + 0xcU;
    r_s4 = r_s4 + 0x18U;
    r_s7 = (uint32_t)((int32_t)r_a3 * (int32_t)r_t3);
    r_s6 = (uint32_t)((int32_t)r_s6 * (int32_t)r_v0);
    r_v0 = (uint32_t)REGCALL_fix_point_mult2_32(r_a0, r_a1, r_a2, r_a3);
    r_t1 = *(uint32_t *)(uintptr_t)(r_sp + 136);
    r_a2 = *(uint32_t *)(uintptr_t)(r_sp + 140);
    r_a0 = 16U;
    r_v1 = (uint32_t)((int32_t)r_s7 * (int32_t)r_v0);
    r_s7 = *(uint32_t *)(uintptr_t)(r_sp + 320);
    r_t1 = (uint32_t)((int32_t)r_s7 * (int32_t)r_t1);
    r_t3 = r_v1 + r_s6;
    r_s6 = *(uint32_t *)(uintptr_t)(r_sp + 324);
    *(uint32_t *)(uintptr_t)(r_sp + 336) = (uint32_t)r_t3;
    r_a1 = r_s6;
    *(uint32_t *)(uintptr_t)(r_sp + 332) = (uint32_t)r_t1;
    r_v0 = (uint32_t)REGCALL_fix_point_mult2_32(r_a0, r_a1, r_a2, r_a3);
    r_t1 = *(uint32_t *)(uintptr_t)(r_sp + 332);
    r_t3 = *(uint32_t *)(uintptr_t)(r_sp + 336);
    r_t0 = *(uint32_t *)(uintptr_t)(r_sp + 348);
    r_v1 = (uint32_t)((int32_t)r_t1 * (int32_t)r_v0);
    r_t5 = *(uint32_t *)(uintptr_t)(r_sp + 344);
    r_a2 = *(uint32_t *)(uintptr_t)(r_sp + 100);
    r_a0 = 16U;
    r_a1 = r_t5;
    r_v0 = r_v1 + r_t3;
    r_t3 = *(uint32_t *)(uintptr_t)(r_sp + 96);
    *(uint32_t *)(uintptr_t)(r_s5 + -12) = (uint32_t)r_v0;
    r_t3 = (uint32_t)((int32_t)r_t0 * (int32_t)r_t3);
    *(uint32_t *)(uintptr_t)(r_sp + 332) = (uint32_t)r_t3;
    r_v0 = (uint32_t)REGCALL_fix_point_mult2_32(r_a0, r_a1, r_a2, r_a3);
    r_t3 = *(uint32_t *)(uintptr_t)(r_sp + 332);
    r_a3 = *(uint32_t *)(uintptr_t)(r_sp + 340);
    r_a2 = *(uint32_t *)(uintptr_t)(r_sp + 124);
    r_t1 = (uint32_t)((int32_t)r_t3 * (int32_t)r_v0);
    r_t3 = *(uint32_t *)(uintptr_t)(r_sp + 120);
    r_a1 = r_s8;
    r_a0 = 16U;
    r_t3 = (uint32_t)((int32_t)r_a3 * (int32_t)r_t3);
    *(uint32_t *)(uintptr_t)(r_sp + 336) = (uint32_t)r_t1;
    *(uint32_t *)(uintptr_t)(r_sp + 332) = (uint32_t)r_t3;
    r_v0 = (uint32_t)REGCALL_fix_point_mult2_32(r_a0, r_a1, r_a2, r_a3);
    r_t3 = *(uint32_t *)(uintptr_t)(r_sp + 332);
    r_t1 = *(uint32_t *)(uintptr_t)(r_sp + 336);
    r_a2 = *(uint32_t *)(uintptr_t)(r_sp + 148);
    r_v1 = (uint32_t)((int32_t)r_t3 * (int32_t)r_v0);
    r_a1 = r_s6;
    r_a0 = 16U;
    r_t3 = r_v1 + r_t1;
    r_t1 = *(uint32_t *)(uintptr_t)(r_sp + 144);
    *(uint32_t *)(uintptr_t)(r_sp + 336) = (uint32_t)r_t3;
    r_t1 = (uint32_t)((int32_t)r_s7 * (int32_t)r_t1);
    *(uint32_t *)(uintptr_t)(r_sp + 332) = (uint32_t)r_t1;
    r_v0 = (uint32_t)REGCALL_fix_point_mult2_32(r_a0, r_a1, r_a2, r_a3);
    r_t1 = *(uint32_t *)(uintptr_t)(r_sp + 332);
    r_t3 = *(uint32_t *)(uintptr_t)(r_sp + 336);
    r_t0 = *(uint32_t *)(uintptr_t)(r_sp + 348);
    r_v1 = (uint32_t)((int32_t)r_t1 * (int32_t)r_v0);
    r_t5 = *(uint32_t *)(uintptr_t)(r_sp + 344);
    r_a2 = *(uint32_t *)(uintptr_t)(r_sp + 108);
    r_a0 = 16U;
    r_a1 = r_t5;
    r_v0 = r_v1 + r_t3;
    *(uint32_t *)(uintptr_t)(r_s5 + -8) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_sp + 104);
    r_t0 = (uint32_t)((int32_t)r_t0 * (int32_t)r_v0);
    *(uint32_t *)(uintptr_t)(r_sp + 332) = (uint32_t)r_t0;
    r_v0 = (uint32_t)REGCALL_fix_point_mult2_32(r_a0, r_a1, r_a2, r_a3);
    r_t0 = *(uint32_t *)(uintptr_t)(r_sp + 332);
    r_a3 = *(uint32_t *)(uintptr_t)(r_sp + 340);
    r_a2 = *(uint32_t *)(uintptr_t)(r_sp + 132);
    r_t0 = (uint32_t)((int32_t)r_t0 * (int32_t)r_v0);
    r_v0 = *(uint32_t *)(uintptr_t)(r_sp + 128);
    r_a1 = r_s8;
    r_a0 = 16U;
    r_a3 = (uint32_t)((int32_t)r_a3 * (int32_t)r_v0);
    *(uint32_t *)(uintptr_t)(r_sp + 336) = (uint32_t)r_t0;
    *(uint32_t *)(uintptr_t)(r_sp + 332) = (uint32_t)r_a3;
    r_v0 = (uint32_t)REGCALL_fix_point_mult2_32(r_a0, r_a1, r_a2, r_a3);
    r_a3 = *(uint32_t *)(uintptr_t)(r_sp + 332);
    r_t0 = *(uint32_t *)(uintptr_t)(r_sp + 336);
    r_a2 = *(uint32_t *)(uintptr_t)(r_sp + 156);
    r_v1 = (uint32_t)((int32_t)r_a3 * (int32_t)r_v0);
    r_v0 = *(uint32_t *)(uintptr_t)(r_sp + 152);
    r_a0 = 16U;
    r_a1 = r_s6;
    r_a3 = r_v1 + r_t0;
    r_v1 = (uint32_t)((int32_t)r_s7 * (int32_t)r_v0);
    *(uint32_t *)(uintptr_t)(r_sp + 336) = (uint32_t)r_a3;
    *(uint32_t *)(uintptr_t)(r_sp + 332) = (uint32_t)r_v1;
    r_v0 = (uint32_t)REGCALL_fix_point_mult2_32(r_a0, r_a1, r_a2, r_a3);
    r_v1 = *(uint32_t *)(uintptr_t)(r_sp + 332);
    r_a3 = *(uint32_t *)(uintptr_t)(r_sp + 336);
    r_a0 = (uint32_t)((int32_t)r_v1 * (int32_t)r_v0);
    r_v0 = r_a0 + r_a3;
    *(uint32_t *)(uintptr_t)(r_s5 + -4) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_sp + 328);
    mips_cond = (r_v0 != r_s5) ? 1U : 0U;
    r_v0 = 60U;
    if (mips_cond) goto L_69ec4;
    mips_cond = (r_s1 == r_v0) ? 1U : 0U;
    r_v1 = (r_s1 < (uint32_t)61) ? 1U : 0U;
    if (mips_cond) goto L_6a3fc;
    mips_cond = (r_v1 != 0) ? 1U : 0U;
    r_s4 = r_s1 << 15;
    if (mips_cond) goto L_6a3d0;
    r_s1 = r_s1 + -0x3cU;
    r_s4 = r_s1 << 15;
    if (r_v0) { mips_lo = (uint32_t)(r_s4 / r_v0); mips_hi = (uint32_t)(r_s4 % r_v0); }
    r_v1 = 0x10000U;
    r_s7 = 1U;
    r_s4 = mips_lo;
    r_s4 = r_v1 - r_s4;
    r_v1 = 56756U;
    r_s1 = (uint32_t)((int32_t)r_s1 * (int32_t)r_v1);
    if (r_v0) { mips_lo = (uint32_t)(r_s1 / r_v0); mips_hi = (uint32_t)(r_s1 % r_v0); }
    r_s1 = mips_lo;
L_6a2dc:
    r_s6 = r_sp + 0x10U;
    r_v0 = r_s6;
    r_s8 = r_s3;
    r_a1 = 4294967295U;
    r_a2 = 1U;
L_6a2f0:
    r_a0 = *(uint32_t *)(uintptr_t)(r_s8 + 0);
    mips_cond = ((int32_t)r_a0 < 0) ? 1U : 0U;
    /* nop */
    if (mips_cond) goto L_6a40c;
    *(uint32_t *)(uintptr_t)(r_v0 + 0) = (uint32_t)r_a2;
L_6a300:
    r_s8 = r_s8 + 0x4U;
    *(uint32_t *)(uintptr_t)(r_v0 + 4) = (uint32_t)r_a0;
    mips_cond = (r_s5 != r_s8) ? 1U : 0U;
    r_v0 = r_v0 + 0x8U;
    if (mips_cond) goto L_6a2f0;
    r_v0 = (uint32_t)(-(int32_t)r_s7);
    r_t0 = r_s3;
    *(uint32_t *)(uintptr_t)(r_sp + 328) = (uint32_t)r_v0;
    r_s3 = 0;
L_6a320:
    r_v0 = ((int32_t)r_s3 < 3) ? 1U : 0U;
    r_s5 = *(uint32_t *)(uintptr_t)(r_s6 + 0);
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_t2 = *(uint32_t *)(uintptr_t)(r_s6 + 4);
    if (mips_cond) goto L_6a418;
    r_s5 = (uint32_t)((int32_t)r_s5 * (int32_t)r_t2);
    *(uint32_t *)(uintptr_t)(r_t0 + 0) = (uint32_t)r_s5;
L_6a338:
    r_s3 = r_s3 + 0x1U;
    r_v0 = 9U;
    r_s6 = r_s6 + 0x8U;
    mips_cond = (r_s3 != r_v0) ? 1U : 0U;
    r_t0 = r_t0 + 0x4U;
    if (mips_cond) goto L_6a320;
L_6a34c:
    r_v0 = *(uint32_t *)(uintptr_t)(r_s2 + 0);
    mips_cond = ((int32_t)r_v0 < 0) ? 1U : 0U;
    /* nop */
    if (mips_cond) goto L_6a4c4;
    r_v0 = (uint32_t)((int32_t)r_v0 >> 6);
L_6a35c:
    *(uint32_t *)(uintptr_t)(r_s2 + 0) = (uint32_t)r_v0;
    r_s2 = r_s2 + 0x4U;
    mips_cond = (r_s8 != r_s2) ? 1U : 0U;
    /* nop */
    if (mips_cond) goto L_6a34c;
    r_ra = *(uint32_t *)(uintptr_t)(r_sp + 396);
    r_s8 = *(uint32_t *)(uintptr_t)(r_sp + 392);
    r_s7 = *(uint32_t *)(uintptr_t)(r_sp + 388);
    r_s6 = *(uint32_t *)(uintptr_t)(r_sp + 384);
    r_s5 = *(uint32_t *)(uintptr_t)(r_sp + 380);
    r_s4 = *(uint32_t *)(uintptr_t)(r_sp + 376);
    r_s3 = *(uint32_t *)(uintptr_t)(r_sp + 372);
    r_s2 = *(uint32_t *)(uintptr_t)(r_sp + 368);
    r_s1 = *(uint32_t *)(uintptr_t)(r_sp + 364);
    r_s0 = *(uint32_t *)(uintptr_t)(r_sp + 360);
    r_sp = r_sp + 0x190U;
    goto fn_exit;
L_6a39c:
    *(uint32_t *)(uintptr_t)(r_v0 + 0) = (uint32_t)r_t0;
    r_v1 = (uint32_t)(-(int32_t)r_v1);
    goto L_69e3c;
L_6a3a8:
    *(uint32_t *)(uintptr_t)(r_v0 + 0) = (uint32_t)r_a1;
    r_v1 = (uint32_t)(-(int32_t)r_v1);
    goto L_69e6c;
L_6a3b4:
    *(uint32_t *)(uintptr_t)(r_v0 + 0) = (uint32_t)r_a1;
    r_v1 = (uint32_t)(-(int32_t)r_v1);
    goto L_69e9c;
L_6a3c0:
    r_a3 = 4294967295U;
    *(uint32_t *)(uintptr_t)(r_v0 + 0) = (uint32_t)r_a3;
    r_v1 = (uint32_t)(-(int32_t)r_v1);
    goto L_6a0bc;
L_6a3d0:
    if (r_v0) { mips_lo = (uint32_t)(r_s4 / r_v0); mips_hi = (uint32_t)(r_s4 % r_v0); }
    r_v1 = 32768U;
    r_s7 = 4294967295U;
    r_s4 = mips_lo;
    r_s4 = r_s4 + r_v1;
    r_v1 = 56756U;
    r_s1 = (uint32_t)((int32_t)r_s1 * (int32_t)r_v1);
    if (r_v0) { mips_lo = (uint32_t)(r_s1 / r_v0); mips_hi = (uint32_t)(r_s1 % r_v0); }
    r_s1 = mips_lo;
    r_s1 = r_v1 - r_s1;
    goto L_6a2dc;
L_6a3fc:
    r_s1 = 0;
    r_s7 = 1U;
    r_s4 = 0x10000U;
    goto L_6a2dc;
L_6a40c:
    *(uint32_t *)(uintptr_t)(r_v0 + 0) = (uint32_t)r_a1;
    r_a0 = (uint32_t)(-(int32_t)r_a0);
    goto L_6a300;
L_6a418:
    r_v0 = ((int32_t)r_s3 < 6) ? 1U : 0U;
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_v0 = *(uint32_t *)(uintptr_t)(r_sp + 328);
    if (mips_cond) goto L_6a470;
    r_a2 = r_t2;
    r_a1 = r_s4;
    r_a0 = 16U;
    *(uint32_t *)(uintptr_t)(r_sp + 336) = (uint32_t)r_t0;
    r_v0 = (uint32_t)REGCALL_fix_point_mult2_32(r_a0, r_a1, r_a2, r_a3);
    r_t1 = *(uint32_t *)(uintptr_t)(r_s6 + 24);
    r_a2 = *(uint32_t *)(uintptr_t)(r_s6 + 28);
    r_a1 = r_s1;
    r_t1 = (uint32_t)((int32_t)r_s7 * (int32_t)r_t1);
    r_a0 = 16U;
    r_s5 = (uint32_t)((int32_t)r_v0 * (int32_t)r_s5);
    *(uint32_t *)(uintptr_t)(r_sp + 332) = (uint32_t)r_t1;
    r_v0 = (uint32_t)REGCALL_fix_point_mult2_32(r_a0, r_a1, r_a2, r_a3);
    r_t1 = *(uint32_t *)(uintptr_t)(r_sp + 332);
    r_v1 = (uint32_t)((int32_t)r_t1 * (int32_t)r_v0);
    r_v0 = r_v1 + r_s5;
L_6a464:
    r_t0 = *(uint32_t *)(uintptr_t)(r_sp + 336);
    *(uint32_t *)(uintptr_t)(r_t0 + 0) = (uint32_t)r_v0;
    goto L_6a338;
L_6a470:
    r_t1 = *(uint32_t *)(uintptr_t)(r_s6 + -24);
    r_a2 = *(uint32_t *)(uintptr_t)(r_s6 + -20);
    r_a1 = r_s1;
    r_t1 = (uint32_t)((int32_t)r_v0 * (int32_t)r_t1);
    r_a0 = 16U;
    *(uint32_t *)(uintptr_t)(r_sp + 340) = (uint32_t)r_t2;
    *(uint32_t *)(uintptr_t)(r_sp + 336) = (uint32_t)r_t0;
    *(uint32_t *)(uintptr_t)(r_sp + 332) = (uint32_t)r_t1;
    r_v0 = (uint32_t)REGCALL_fix_point_mult2_32(r_a0, r_a1, r_a2, r_a3);
    r_t1 = *(uint32_t *)(uintptr_t)(r_sp + 332);
    r_t2 = *(uint32_t *)(uintptr_t)(r_sp + 340);
    r_a1 = r_s4;
    r_t1 = (uint32_t)((int32_t)r_t1 * (int32_t)r_v0);
    r_a2 = r_t2;
    r_a0 = 16U;
    *(uint32_t *)(uintptr_t)(r_sp + 332) = (uint32_t)r_t1;
    r_v0 = (uint32_t)REGCALL_fix_point_mult2_32(r_a0, r_a1, r_a2, r_a3);
    r_v1 = (uint32_t)((int32_t)r_v0 * (int32_t)r_s5);
    r_t1 = *(uint32_t *)(uintptr_t)(r_sp + 332);
    r_v0 = r_v1 + r_t1;
    goto L_6a464;
L_6a4c4:
    r_v0 = (uint32_t)(-(int32_t)r_v0);
    r_v0 = (uint32_t)((int32_t)r_v0 >> 6);
    r_v0 = (uint32_t)(-(int32_t)r_v0);
    goto L_6a35c;
fn_exit:
    return (int32_t)r_v0;
}

static int32_t tisp_bcsh_Tccm_RGB2YUV_lit(uint32_t a0_in)
{
    uint32_t r_v0 = 0, r_v1 = 0;
    uint32_t r_at = 0, r_a0 = 0, r_a1 = 0, r_a2 = 0, r_a3 = 0;
    uint32_t r_t0 = 0, r_t1 = 0, r_t2 = 0, r_t3 = 0, r_t4 = 0, r_t5 = 0;
    uint32_t r_t6 = 0, r_t7 = 0, r_t8 = 0, r_t9 = 0;
    uint32_t r_s0 = 0, r_s1 = 0, r_s2 = 0, r_s3 = 0, r_s4 = 0, r_s5 = 0;
    uint32_t r_s6 = 0, r_s7 = 0, r_s8 = 0, r_k0 = 0, r_k1 = 0, r_gp = 0, r_ra = 0;
    uint32_t mips_lo = 0, mips_hi = 0, mips_cond = 0;
    uint8_t mips_stack[1024] __attribute__((aligned(8)));
    uint32_t r_sp = (uint32_t)(uintptr_t)(mips_stack + 512);
    r_a0 = a0_in;
    (void)r_at; (void)r_k0; (void)r_k1; (void)r_gp; (void)r_t8; (void)r_t9;
    (void)r_ra; (void)mips_hi; (void)mips_lo; (void)mips_cond;
    r_sp = r_sp + -0x58U;
    *(uint32_t *)(uintptr_t)(r_sp + 84) = (uint32_t)r_ra;
    *(uint32_t *)(uintptr_t)(r_sp + 80) = (uint32_t)r_s4;
    *(uint32_t *)(uintptr_t)(r_sp + 76) = (uint32_t)r_s3;
    *(uint32_t *)(uintptr_t)(r_sp + 72) = (uint32_t)r_s2;
    *(uint32_t *)(uintptr_t)(r_sp + 68) = (uint32_t)r_s1;
    *(uint32_t *)(uintptr_t)(r_sp + 64) = (uint32_t)r_s0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_a0 + 100);
    r_s2 = *(uint32_t *)(uintptr_t)(r_a0 + 88);
    r_s1 = *(uint32_t *)(uintptr_t)(r_a0 + 16);
    r_s3 = *(uint32_t *)(uintptr_t)(r_a0 + 20);
    r_s4 = *(uint8_t *)(uintptr_t)(r_v0 + 0);
    r_a1 = (uint32_t)(uintptr_t)&bcsh_rgb2yuv_base_matrix_lit[0]; /* HI16 .rodata */
    r_v0 = (uint32_t)(uintptr_t)&memcpy; /* HI16 memcpy */
    r_s0 = *(uint32_t *)(uintptr_t)(r_a0 + 40);
    r_a2 = 36U;
    r_a0 = r_sp + 0x18U;
    r_v0 = (uint32_t)(uintptr_t)&memcpy;
    r_a1 = (uint32_t)(uintptr_t)&bcsh_rgb2yuv_base_matrix_lit[0];
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_v0 = (uint32_t)(uintptr_t)&tisp_bcsh_Tccm_RGBYUV; /* HI16 tisp_bcsh_Tccm_RGBYUV */
    *(uint32_t *)(uintptr_t)(r_sp + 16) = (uint32_t)r_s4;
    r_a3 = r_s3;
    r_a2 = r_s2;
    r_a1 = r_s1;
    r_v0 = (uint32_t)(uintptr_t)&tisp_bcsh_Tccm_RGBYUV;
    r_a0 = r_sp + 0x18U;
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_Tccm_RGBYUV(r_a0, r_a1, r_a2, r_a3);
    r_v0 = (uint32_t)(uintptr_t)&tisp_bcsh_para2reg; /* HI16 tisp_bcsh_para2reg */
    r_a1 = r_sp + 0x18U;
    r_v0 = (uint32_t)(uintptr_t)&tisp_bcsh_para2reg;
    r_a0 = r_s0;
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_para2reg(r_a0, r_a1, r_a2, r_a3);
    r_ra = *(uint32_t *)(uintptr_t)(r_sp + 84);
    r_s4 = *(uint32_t *)(uintptr_t)(r_sp + 80);
    r_s3 = *(uint32_t *)(uintptr_t)(r_sp + 76);
    r_s2 = *(uint32_t *)(uintptr_t)(r_sp + 72);
    r_s1 = *(uint32_t *)(uintptr_t)(r_sp + 68);
    r_s0 = *(uint32_t *)(uintptr_t)(r_sp + 64);
    r_sp = r_sp + 0x58U;
    goto fn_exit;
fn_exit:
    return (int32_t)r_v0;
}

static int32_t tisp_bcsh_ct_interpolation_lit(uint32_t a0_in)
{
    uint32_t r_v0 = 0, r_v1 = 0;
    uint32_t r_at = 0, r_a0 = 0, r_a1 = 0, r_a2 = 0, r_a3 = 0;
    uint32_t r_t0 = 0, r_t1 = 0, r_t2 = 0, r_t3 = 0, r_t4 = 0, r_t5 = 0;
    uint32_t r_t6 = 0, r_t7 = 0, r_t8 = 0, r_t9 = 0;
    uint32_t r_s0 = 0, r_s1 = 0, r_s2 = 0, r_s3 = 0, r_s4 = 0, r_s5 = 0;
    uint32_t r_s6 = 0, r_s7 = 0, r_s8 = 0, r_k0 = 0, r_k1 = 0, r_gp = 0, r_ra = 0;
    uint32_t mips_lo = 0, mips_hi = 0, mips_cond = 0;
    uint8_t mips_stack[1024] __attribute__((aligned(8)));
    uint32_t r_sp = (uint32_t)(uintptr_t)(mips_stack + 512);
    r_a0 = a0_in;
    (void)r_at; (void)r_k0; (void)r_k1; (void)r_gp; (void)r_t8; (void)r_t9;
    (void)r_ra; (void)mips_hi; (void)mips_lo; (void)mips_cond;
    r_a3 = *(uint32_t *)(uintptr_t)(r_a0 + 96);
    r_a1 = *(uint32_t *)(uintptr_t)(r_a0 + 164);
    r_t0 = *(uint32_t *)(uintptr_t)(r_a0 + 176);
    r_t1 = *(uint32_t *)(uintptr_t)(r_a3 + 12);
    r_a1 = *(uint16_t *)(uintptr_t)(r_a1 + 0);
    r_v1 = *(uint32_t *)(uintptr_t)(r_a0 + 180);
    r_v0 = *(uint32_t *)(uintptr_t)(r_a0 + 88);
    r_t1 = (r_t1 < r_a1) ? 1U : 0U;
    r_t4 = *(uint32_t *)(uintptr_t)(r_a0 + 28);
    r_a2 = *(uint32_t *)(uintptr_t)(r_a0 + 32);
    mips_cond = (r_t1 == 0) ? 1U : 0U;
    r_t5 = *(uint32_t *)(uintptr_t)(r_a0 + 36);
    if (mips_cond) goto L_69b64;
    *(uint16_t *)(uintptr_t)(r_t0 + 0) = (uint16_t)0;
L_69b38:
    r_a0 = *(uint32_t *)(uintptr_t)(r_a0 + 24);
    r_a0 = *(uint8_t *)(uintptr_t)(r_a0 + 0);
    mips_cond = (r_a0 != 0) ? 1U : 0U;
    r_t1 = *(uint16_t *)(uintptr_t)(r_t0 + 0);
    if (mips_cond) goto L_69bdc;
    mips_cond = (r_t1 != 0) ? 1U : 0U;
    r_a0 = 2U;
    if (mips_cond) goto L_69bac;
    r_a0 = *(uint16_t *)(uintptr_t)(r_v1 + 0);
    mips_cond = (r_a0 != 0) ? 1U : 0U;
    /* nop */
    if (mips_cond) goto L_69bdc;
    /* nop */
    goto fn_exit;
L_69b64:
    r_t1 = *(uint32_t *)(uintptr_t)(r_a3 + 8);
    r_t1 = (r_t1 < r_a1) ? 1U : 0U;
    mips_cond = (r_t1 == 0) ? 1U : 0U;
    r_t1 = 1U;
    if (mips_cond) goto L_69b7c;
L_69b74:
    *(uint16_t *)(uintptr_t)(r_t0 + 0) = (uint16_t)r_t1;
    goto L_69b38;
L_69b7c:
    r_t1 = *(uint32_t *)(uintptr_t)(r_a3 + 4);
    r_t1 = (r_t1 < r_a1) ? 1U : 0U;
    mips_cond = (r_t1 != 0) ? 1U : 0U;
    r_t1 = 2U;
    if (mips_cond) goto L_69b74;
    r_t1 = *(uint32_t *)(uintptr_t)(r_a3 + 0);
    r_t1 = (r_t1 < r_a1) ? 1U : 0U;
    mips_cond = (r_t1 == 0) ? 1U : 0U;
    /* nop */
    if (mips_cond) goto L_69ba4;
    r_t1 = 3U;
    goto L_69b74;
L_69ba4:
    r_t1 = 4U;
    goto L_69b74;
L_69bac:
    mips_cond = (r_t1 != r_a0) ? 1U : 0U;
    r_a0 = 4U;
    if (mips_cond) goto L_69bc8;
    r_a0 = *(uint16_t *)(uintptr_t)(r_v1 + 0);
    mips_cond = (r_a0 != r_t1) ? 1U : 0U;
    /* nop */
    if (mips_cond) goto L_69bdc;
    /* nop */
    goto fn_exit;
L_69bc8:
    mips_cond = (r_t1 != r_a0) ? 1U : 0U;
    /* nop */
    if (mips_cond) goto L_69bdc;
    r_a0 = *(uint16_t *)(uintptr_t)(r_v1 + 0);
    mips_cond = (r_a0 == r_t1) ? 1U : 0U;
    /* nop */
    if (mips_cond) goto L_69ddc;
L_69bdc:
    *(uint16_t *)(uintptr_t)(r_v1 + 0) = (uint16_t)r_t1;
    r_v1 = *(uint16_t *)(uintptr_t)(r_t0 + 0);
    r_a0 = (r_v1 < (uint32_t)5) ? 1U : 0U;
    mips_cond = (r_a0 == 0) ? 1U : 0U;
    r_a0 = r_v1 << 2;
    if (mips_cond) goto L_69dc0;
    switch (r_v1) {
    case 0: goto L_69c08;
    case 1: goto L_69c2c;
    case 2: goto L_69cd4;
    case 3: goto L_69cf4;
    case 4: goto L_69d9c;
    default: goto L_69dc0;
    }
L_69c08:
    r_v1 = r_t4;
    r_a0 = r_t4 + 0x24U;
L_69c10:
    r_a1 = *(uint32_t *)(uintptr_t)(r_v1 + 0);
    r_v1 = r_v1 + 0x4U;
    r_v0 = r_v0 + 0x4U;
    mips_cond = (r_a0 != r_v1) ? 1U : 0U;
    *(uint32_t *)(uintptr_t)(r_v0 + -4) = (uint32_t)r_a1;
    if (mips_cond) goto L_69c10;
    /* nop */
    goto fn_exit;
L_69c2c:
    r_t2 = 0;
    r_t7 = 36U;
L_69c34:
    r_v1 = *(uint32_t *)(uintptr_t)(r_a3 + 8);
    r_t3 = *(uint32_t *)(uintptr_t)(r_a3 + 12);
    r_a0 = r_a2 + r_t2;
    mips_cond = (r_v1 == r_t3) ? 1U : 0U;
    r_a0 = *(uint32_t *)(uintptr_t)(r_a0 + 0);
    if (mips_cond) goto L_69c8c;
    r_t0 = r_t4 + r_t2;
    r_t0 = *(uint32_t *)(uintptr_t)(r_t0 + 0);
    r_t6 = (r_v1 < r_a1) ? 1U : 0U;
    r_t1 = ((int32_t)r_t0 < (int32_t)r_a0) ? 1U : 0U;
    mips_cond = (r_t1 == 0) ? 1U : 0U;
    r_t5 = (r_t3 < r_v1) ? 1U : 0U;
    if (mips_cond) goto L_69ca4;
    r_t0 = r_a0 - r_t0;
    mips_cond = (r_t6 == 0) ? 1U : 0U;
    r_t1 = r_v1 - r_a1;
    if (mips_cond) goto L_69c70;
    r_t1 = r_a1 - r_v1;
L_69c70:
    r_t0 = (uint32_t)((int32_t)r_t1 * (int32_t)r_t0);
    mips_cond = (r_t5 == 0) ? 1U : 0U;
    r_t1 = r_t3 - r_v1;
    if (mips_cond) goto L_69c80;
    r_t1 = r_v1 - r_t3;
L_69c80:
    if (r_t1) { mips_lo = (uint32_t)(r_t0 / r_t1); mips_hi = (uint32_t)(r_t0 % r_t1); }
    r_t0 = mips_lo;
    r_a0 = r_a0 - r_t0;
L_69c8c:
    r_v1 = r_v0 + r_t2;
    r_t2 = r_t2 + 0x4U;
    mips_cond = (r_t2 != r_t7) ? 1U : 0U;
    *(uint32_t *)(uintptr_t)(r_v1 + 0) = (uint32_t)r_a0;
    if (mips_cond) goto L_69c34;
    /* nop */
    goto fn_exit;
L_69ca4:
    r_t1 = r_t0 - r_a0;
    mips_cond = (r_t6 == 0) ? 1U : 0U;
    r_t0 = r_v1 - r_a1;
    if (mips_cond) goto L_69cb4;
    r_t0 = r_a1 - r_v1;
L_69cb4:
    r_t0 = (uint32_t)((int32_t)r_t0 * (int32_t)r_t1);
    mips_cond = (r_t5 == 0) ? 1U : 0U;
    r_t1 = r_t3 - r_v1;
    if (mips_cond) goto L_69cc4;
    r_t1 = r_v1 - r_t3;
L_69cc4:
    if (r_t1) { mips_lo = (uint32_t)(r_t0 / r_t1); mips_hi = (uint32_t)(r_t0 % r_t1); }
    r_t0 = mips_lo;
    r_a0 = r_t0 + r_a0;
    goto L_69c8c;
L_69cd4:
    r_v1 = r_a2 + 0x24U;
L_69cd8:
    r_a0 = *(uint32_t *)(uintptr_t)(r_a2 + 0);
    r_a2 = r_a2 + 0x4U;
    r_v0 = r_v0 + 0x4U;
    mips_cond = (r_v1 != r_a2) ? 1U : 0U;
    *(uint32_t *)(uintptr_t)(r_v0 + -4) = (uint32_t)r_a0;
    if (mips_cond) goto L_69cd8;
    /* nop */
    goto fn_exit;
L_69cf4:
    r_t2 = 0;
    r_t7 = 36U;
L_69cfc:
    r_v1 = *(uint32_t *)(uintptr_t)(r_a3 + 0);
    r_t3 = *(uint32_t *)(uintptr_t)(r_a3 + 4);
    r_a0 = r_t5 + r_t2;
    mips_cond = (r_v1 == r_t3) ? 1U : 0U;
    r_a0 = *(uint32_t *)(uintptr_t)(r_a0 + 0);
    if (mips_cond) goto L_69d54;
    r_t0 = r_a2 + r_t2;
    r_t0 = *(uint32_t *)(uintptr_t)(r_t0 + 0);
    r_t6 = (r_v1 < r_a1) ? 1U : 0U;
    r_t1 = ((int32_t)r_t0 < (int32_t)r_a0) ? 1U : 0U;
    mips_cond = (r_t1 == 0) ? 1U : 0U;
    r_t4 = (r_t3 < r_v1) ? 1U : 0U;
    if (mips_cond) goto L_69d6c;
    r_t0 = r_a0 - r_t0;
    mips_cond = (r_t6 == 0) ? 1U : 0U;
    r_t1 = r_v1 - r_a1;
    if (mips_cond) goto L_69d38;
    r_t1 = r_a1 - r_v1;
L_69d38:
    r_t0 = (uint32_t)((int32_t)r_t1 * (int32_t)r_t0);
    mips_cond = (r_t4 == 0) ? 1U : 0U;
    r_t1 = r_t3 - r_v1;
    if (mips_cond) goto L_69d48;
    r_t1 = r_v1 - r_t3;
L_69d48:
    if (r_t1) { mips_lo = (uint32_t)(r_t0 / r_t1); mips_hi = (uint32_t)(r_t0 % r_t1); }
    r_t0 = mips_lo;
    r_a0 = r_a0 - r_t0;
L_69d54:
    r_v1 = r_v0 + r_t2;
    r_t2 = r_t2 + 0x4U;
    mips_cond = (r_t2 != r_t7) ? 1U : 0U;
    *(uint32_t *)(uintptr_t)(r_v1 + 0) = (uint32_t)r_a0;
    if (mips_cond) goto L_69cfc;
    /* nop */
    goto fn_exit;
L_69d6c:
    r_t1 = r_t0 - r_a0;
    mips_cond = (r_t6 == 0) ? 1U : 0U;
    r_t0 = r_v1 - r_a1;
    if (mips_cond) goto L_69d7c;
    r_t0 = r_a1 - r_v1;
L_69d7c:
    r_t0 = (uint32_t)((int32_t)r_t0 * (int32_t)r_t1);
    mips_cond = (r_t4 == 0) ? 1U : 0U;
    r_t1 = r_t3 - r_v1;
    if (mips_cond) goto L_69d8c;
    r_t1 = r_v1 - r_t3;
L_69d8c:
    if (r_t1) { mips_lo = (uint32_t)(r_t0 / r_t1); mips_hi = (uint32_t)(r_t0 % r_t1); }
    r_t0 = mips_lo;
    r_a0 = r_t0 + r_a0;
    goto L_69d54;
L_69d9c:
    r_v1 = r_t5;
    r_a0 = r_t5 + 0x24U;
L_69da4:
    r_a1 = *(uint32_t *)(uintptr_t)(r_v1 + 0);
    r_v1 = r_v1 + 0x4U;
    r_v0 = r_v0 + 0x4U;
    mips_cond = (r_a0 != r_v1) ? 1U : 0U;
    *(uint32_t *)(uintptr_t)(r_v0 + -4) = (uint32_t)r_a1;
    if (mips_cond) goto L_69da4;
    /* nop */
    goto fn_exit;
L_69dc0:
    r_v1 = r_t4;
    r_a0 = r_t4 + 0x24U;
L_69dc8:
    r_a1 = *(uint32_t *)(uintptr_t)(r_v1 + 0);
    r_v1 = r_v1 + 0x4U;
    r_v0 = r_v0 + 0x4U;
    mips_cond = (r_a0 != r_v1) ? 1U : 0U;
    *(uint32_t *)(uintptr_t)(r_v0 + -4) = (uint32_t)r_a1;
    if (mips_cond) goto L_69dc8;
L_69ddc:
    /* nop */
    goto fn_exit;
fn_exit:
    return (int32_t)r_v0;
}

static int32_t tisp_bcsh_ct_set_parameter_lit(uint32_t a0_in, uint32_t a1_in)
{
    uint32_t r_v0 = 0, r_v1 = 0;
    uint32_t r_at = 0, r_a0 = 0, r_a1 = 0, r_a2 = 0, r_a3 = 0;
    uint32_t r_t0 = 0, r_t1 = 0, r_t2 = 0, r_t3 = 0, r_t4 = 0, r_t5 = 0;
    uint32_t r_t6 = 0, r_t7 = 0, r_t8 = 0, r_t9 = 0;
    uint32_t r_s0 = 0, r_s1 = 0, r_s2 = 0, r_s3 = 0, r_s4 = 0, r_s5 = 0;
    uint32_t r_s6 = 0, r_s7 = 0, r_s8 = 0, r_k0 = 0, r_k1 = 0, r_gp = 0, r_ra = 0;
    uint32_t mips_lo = 0, mips_hi = 0, mips_cond = 0;
    uint8_t mips_stack[1024] __attribute__((aligned(8)));
    uint32_t r_sp = (uint32_t)(uintptr_t)(mips_stack + 512);
    r_a0 = a0_in;
    r_a1 = a1_in;
    (void)r_at; (void)r_k0; (void)r_k1; (void)r_gp; (void)r_t8; (void)r_t9;
    (void)r_ra; (void)mips_hi; (void)mips_lo; (void)mips_cond;
    r_sp = r_sp + -0x40U;
    *(uint32_t *)(uintptr_t)(r_sp + 60) = (uint32_t)r_ra;
    *(uint32_t *)(uintptr_t)(r_sp + 56) = (uint32_t)r_s8;
    *(uint32_t *)(uintptr_t)(r_sp + 52) = (uint32_t)r_s7;
    *(uint32_t *)(uintptr_t)(r_sp + 48) = (uint32_t)r_s6;
    *(uint32_t *)(uintptr_t)(r_sp + 44) = (uint32_t)r_s5;
    *(uint32_t *)(uintptr_t)(r_sp + 40) = (uint32_t)r_s4;
    *(uint32_t *)(uintptr_t)(r_sp + 36) = (uint32_t)r_s3;
    *(uint32_t *)(uintptr_t)(r_sp + 32) = (uint32_t)r_s2;
    *(uint32_t *)(uintptr_t)(r_sp + 28) = (uint32_t)r_s1;
    *(uint32_t *)(uintptr_t)(r_sp + 24) = (uint32_t)r_s0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_a0 + 40);
    r_v1 = *(uint32_t *)(uintptr_t)(r_v0 + 4);
    r_a0 = *(uint32_t *)(uintptr_t)(r_v0 + 0);
    r_s2 = *(uint32_t *)(uintptr_t)(r_v0 + 16);
    r_v1 = r_v1 << 16;
    r_v1 = r_v1 | r_a0;
    r_a0 = *(uint32_t *)(uintptr_t)(r_v0 + 12);
    r_s2 = r_s2 << 16;
    r_s1 = *(uint32_t *)(uintptr_t)(r_v0 + 28);
    r_s2 = r_s2 | r_a0;
    r_a0 = *(uint32_t *)(uintptr_t)(r_v0 + 24);
    r_s1 = r_s1 << 16;
    r_a3 = *(uint32_t *)(uintptr_t)(r_v0 + 8);
    r_s1 = r_s1 | r_a0;
    r_a2 = *(uint32_t *)(uintptr_t)(r_v0 + 20);
    r_s8 = *(uint32_t *)(uintptr_t)(r_v0 + 32);
    mips_cond = (r_a1 == 0) ? 1U : 0U;
    r_a0 = 0x10000U;
    if (mips_cond) goto L_6a6bc;
    r_s3 = r_a0 + 0x1138U;
    r_s4 = r_a0 + 0x1134U;
    r_s5 = r_a0 + 0x1130U;
    r_s6 = r_a0 + 0x112cU;
    r_s7 = r_a0 + 0x1128U;
    r_a0 = r_a0 + 0x1124U;
L_6a630:
    r_s0 = (uint32_t)(uintptr_t)&system_reg_write; /* HI16 system_reg_write */
    r_s0 = (uint32_t)(uintptr_t)&system_reg_write;
    r_a1 = r_v1;
    *(uint32_t *)(uintptr_t)(r_sp + 20) = (uint32_t)r_a2;
    *(uint32_t *)(uintptr_t)(r_sp + 16) = (uint32_t)r_a3;
    r_v0 = (uint32_t)REGCALL_system_reg_write(r_a0, r_a1, r_a2, r_a3);
    r_a3 = *(uint32_t *)(uintptr_t)(r_sp + 16);
    r_a0 = r_s7;
    r_a1 = r_a3;
    r_v0 = (uint32_t)REGCALL_system_reg_write(r_a0, r_a1, r_a2, r_a3);
    r_a1 = r_s2;
    r_a0 = r_s6;
    r_v0 = (uint32_t)REGCALL_system_reg_write(r_a0, r_a1, r_a2, r_a3);
    r_a2 = *(uint32_t *)(uintptr_t)(r_sp + 20);
    r_a0 = r_s5;
    r_a1 = r_a2;
    r_v0 = (uint32_t)REGCALL_system_reg_write(r_a0, r_a1, r_a2, r_a3);
    r_a1 = r_s1;
    r_a0 = r_s4;
    r_v0 = (uint32_t)REGCALL_system_reg_write(r_a0, r_a1, r_a2, r_a3);
    r_ra = *(uint32_t *)(uintptr_t)(r_sp + 60);
    r_s7 = *(uint32_t *)(uintptr_t)(r_sp + 52);
    r_s6 = *(uint32_t *)(uintptr_t)(r_sp + 48);
    r_s5 = *(uint32_t *)(uintptr_t)(r_sp + 44);
    r_s4 = *(uint32_t *)(uintptr_t)(r_sp + 40);
    r_s2 = *(uint32_t *)(uintptr_t)(r_sp + 32);
    r_s1 = *(uint32_t *)(uintptr_t)(r_sp + 28);
    r_a1 = r_s8;
    r_a0 = r_s3;
    r_s8 = *(uint32_t *)(uintptr_t)(r_sp + 56);
    r_s3 = *(uint32_t *)(uintptr_t)(r_sp + 36);
    r_t9 = r_s0;
    r_s0 = *(uint32_t *)(uintptr_t)(r_sp + 24);
    r_sp = r_sp + 0x40U;
    r_v0 = (uint32_t)REGCALL_system_reg_write(r_a0, r_a1, r_a2, r_a3); goto fn_exit;
L_6a6bc:
    r_s3 = r_a0 + 0x1038U;
    r_s4 = r_a0 + 0x1034U;
    r_s5 = r_a0 + 0x1030U;
    r_s6 = r_a0 + 0x102cU;
    r_s7 = r_a0 + 0x1028U;
    r_a0 = r_a0 + 0x1024U;
    goto L_6a630;
fn_exit:
    return (int32_t)r_v0;
}

static int32_t tisp_bcsh_ev_update_TransitParam_lit(uint32_t a0_in, uint32_t a1_in)
{
    uint32_t r_v0 = 0, r_v1 = 0;
    uint32_t r_at = 0, r_a0 = 0, r_a1 = 0, r_a2 = 0, r_a3 = 0;
    uint32_t r_t0 = 0, r_t1 = 0, r_t2 = 0, r_t3 = 0, r_t4 = 0, r_t5 = 0;
    uint32_t r_t6 = 0, r_t7 = 0, r_t8 = 0, r_t9 = 0;
    uint32_t r_s0 = 0, r_s1 = 0, r_s2 = 0, r_s3 = 0, r_s4 = 0, r_s5 = 0;
    uint32_t r_s6 = 0, r_s7 = 0, r_s8 = 0, r_k0 = 0, r_k1 = 0, r_gp = 0, r_ra = 0;
    uint32_t mips_lo = 0, mips_hi = 0, mips_cond = 0;
    uint8_t mips_stack[1024] __attribute__((aligned(8)));
    uint32_t r_sp = (uint32_t)(uintptr_t)(mips_stack + 512);
    r_a0 = a0_in;
    r_a1 = a1_in;
    (void)r_at; (void)r_k0; (void)r_k1; (void)r_gp; (void)r_t8; (void)r_t9;
    (void)r_ra; (void)mips_hi; (void)mips_lo; (void)mips_cond;
    r_sp = r_sp + -0x70U;
    *(uint32_t *)(uintptr_t)(r_sp + 96) = (uint32_t)r_s6;
    *(uint32_t *)(uintptr_t)(r_sp + 84) = (uint32_t)r_s3;
    *(uint32_t *)(uintptr_t)(r_sp + 80) = (uint32_t)r_s2;
    *(uint32_t *)(uintptr_t)(r_sp + 76) = (uint32_t)r_s1;
    *(uint32_t *)(uintptr_t)(r_sp + 108) = (uint32_t)r_ra;
    *(uint32_t *)(uintptr_t)(r_sp + 104) = (uint32_t)r_s8;
    *(uint32_t *)(uintptr_t)(r_sp + 100) = (uint32_t)r_s7;
    *(uint32_t *)(uintptr_t)(r_sp + 92) = (uint32_t)r_s5;
    *(uint32_t *)(uintptr_t)(r_sp + 88) = (uint32_t)r_s4;
    *(uint32_t *)(uintptr_t)(r_sp + 72) = (uint32_t)r_s0;
    r_v1 = *(uint32_t *)(uintptr_t)(r_a0 + 0);
    r_s2 = r_a1;
    r_a1 = *(uint32_t *)(uintptr_t)(r_a0 + 92);
    *(uint32_t *)(uintptr_t)(r_sp + 32) = (uint32_t)r_v1;
    r_v1 = *(uint32_t *)(uintptr_t)(r_a0 + 192);
    r_t3 = *(uint32_t *)(uintptr_t)(r_a1 + 0);
    r_a1 = *(uint32_t *)(uintptr_t)(r_a0 + 184);
    r_t1 = *(uint8_t *)(uintptr_t)(r_v1 + 0);
    r_v1 = *(uint32_t *)(uintptr_t)(r_a0 + 56);
    r_t2 = *(uint8_t *)(uintptr_t)(r_a1 + 0);
    r_a1 = *(uint32_t *)(uintptr_t)(r_a0 + 188);
    *(uint32_t *)(uintptr_t)(r_sp + 36) = (uint32_t)r_v1;
    r_v1 = *(uint32_t *)(uintptr_t)(r_a0 + 60);
    r_v0 = *(uint32_t *)(uintptr_t)(r_a0 + 80);
    r_s7 = *(uint32_t *)(uintptr_t)(r_a0 + 84);
    r_s4 = *(uint32_t *)(uintptr_t)(r_a0 + 68);
    r_s8 = *(uint32_t *)(uintptr_t)(r_a0 + 76);
    r_t5 = *(uint32_t *)(uintptr_t)(r_a0 + 276);
    r_t6 = *(uint32_t *)(uintptr_t)(r_a0 + 16);
    r_s5 = *(uint32_t *)(uintptr_t)(r_a0 + 252);
    r_s0 = *(uint8_t *)(uintptr_t)(r_a1 + 0);
    *(uint32_t *)(uintptr_t)(r_sp + 40) = (uint32_t)r_v1;
    r_v1 = *(uint32_t *)(uintptr_t)(r_a0 + 64);
    r_s1 = *(uint32_t *)(uintptr_t)(r_a0 + 228);
    r_s3 = *(uint32_t *)(uintptr_t)(r_a0 + 72);
    *(uint32_t *)(uintptr_t)(r_sp + 44) = (uint32_t)r_v1;
    r_v1 = *(uint32_t *)(uintptr_t)(r_a0 + 8);
    r_s6 = *(uint32_t *)(uintptr_t)(r_a0 + 208);
    *(uint32_t *)(uintptr_t)(r_sp + 60) = (uint32_t)r_v1;
    r_v1 = *(uint32_t *)(uintptr_t)(r_a0 + 212);
    *(uint32_t *)(uintptr_t)(r_sp + 24) = (uint32_t)r_v1;
    r_v1 = *(uint32_t *)(uintptr_t)(r_a0 + 216);
    *(uint32_t *)(uintptr_t)(r_sp + 28) = (uint32_t)r_v1;
    r_v1 = *(uint32_t *)(uintptr_t)(r_a0 + 44);
    *(uint32_t *)(uintptr_t)(r_sp + 48) = (uint32_t)r_v1;
    r_v1 = *(uint32_t *)(uintptr_t)(r_a0 + 48);
    *(uint32_t *)(uintptr_t)(r_sp + 52) = (uint32_t)r_v1;
    r_v1 = *(uint32_t *)(uintptr_t)(r_a0 + 52);
    *(uint32_t *)(uintptr_t)(r_sp + 56) = (uint32_t)r_v1;
    r_v1 = *(uint32_t *)(uintptr_t)(r_sp + 32);
    r_a0 = *(uint16_t *)(uintptr_t)(r_v1 + 0);
    *(uint32_t *)(uintptr_t)(r_v0 + 0) = (uint32_t)r_a0;
    r_a0 = 1024U;
    *(uint32_t *)(uintptr_t)(r_v0 + 4) = (uint32_t)r_a0;
    *(uint32_t *)(uintptr_t)(r_v0 + 8) = (uint32_t)r_a0;
    r_v0 = (uint32_t)(int32_t)(int8_t)(uint8_t)r_t1; /* seb v0,t1 */
    mips_cond = ((int32_t)r_v0 < 0) ? 1U : 0U;
    r_t4 = (uint32_t)(uintptr_t)&tisp_bcsh_StrenCal_part0_lit; /* HI16 .text */
    if (mips_cond) goto L_68a64;
    r_v0 = *(uint32_t *)(uintptr_t)(r_s4 + 0);
    r_a0 = r_t1;
    r_t4 = (uint32_t)(uintptr_t)&tisp_bcsh_StrenCal_part0_lit;
    r_a3 = 0;
    r_a2 = 128U;
    r_a1 = 0;
    *(uint32_t *)(uintptr_t)(r_sp + 16) = (uint32_t)r_v0;
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_StrenCal_part_0(r_a0, r_a1, r_a2, r_a3);
    *(uint32_t *)(uintptr_t)(r_s4 + 0) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_s4 + 4);
    r_a0 = r_t1;
    r_a3 = 0;
    r_a2 = 128U;
    r_a1 = 0;
    *(uint32_t *)(uintptr_t)(r_sp + 16) = (uint32_t)r_v0;
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_StrenCal_part_0(r_a0, r_a1, r_a2, r_a3);
    *(uint32_t *)(uintptr_t)(r_s4 + 4) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_s4 + 8);
    r_a3 = 0;
    r_a2 = 128U;
    r_a1 = 0;
    *(uint32_t *)(uintptr_t)(r_sp + 16) = (uint32_t)r_v0;
    r_a0 = r_t1;
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_StrenCal_part_0(r_a0, r_a1, r_a2, r_a3);
    *(uint32_t *)(uintptr_t)(r_s4 + 8) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_s4 + 12);
    r_a3 = 0;
    r_a2 = 128U;
    *(uint32_t *)(uintptr_t)(r_sp + 16) = (uint32_t)r_v0;
    r_a1 = 0;
L_687c4:
    r_a0 = r_t1;
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_StrenCal_part_0(r_a0, r_a1, r_a2, r_a3);
    *(uint32_t *)(uintptr_t)(r_s4 + 12) = (uint32_t)r_v0;
L_687d0:
    r_t3 = (uint32_t)((int32_t)r_t2 * (int32_t)r_t3);
    r_v0 = 1900U;
    r_t3 = r_t3 >> 7;
    r_a0 = (r_t3 < (uint32_t)1901) ? 1U : 0U;
    if (r_a0 != 0) r_v0 = r_t3;
    *(uint32_t *)(uintptr_t)(r_sp + 64) = (uint32_t)r_v0;
    r_v0 = (uint32_t)(int32_t)(int8_t)(uint8_t)r_t2; /* seb v0,t2 */
    mips_cond = ((int32_t)r_v0 < 0) ? 1U : 0U;
    r_t1 = (uint32_t)(uintptr_t)&tisp_bcsh_StrenCal_part0_lit; /* HI16 .text */
    if (mips_cond) goto L_68878;
    r_v0 = *(uint32_t *)(uintptr_t)(r_s4 + 0);
    r_a0 = r_t2;
    r_t1 = (uint32_t)(uintptr_t)&tisp_bcsh_StrenCal_part0_lit;
    r_a3 = 0;
    r_a2 = 128U;
    r_a1 = 0;
    *(uint32_t *)(uintptr_t)(r_sp + 16) = (uint32_t)r_v0;
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_StrenCal_part_0(r_a0, r_a1, r_a2, r_a3);
    *(uint32_t *)(uintptr_t)(r_s4 + 0) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_s4 + 4);
    r_a0 = r_t2;
    r_a3 = 0;
    r_a2 = 128U;
    r_a1 = 0;
    *(uint32_t *)(uintptr_t)(r_sp + 16) = (uint32_t)r_v0;
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_StrenCal_part_0(r_a0, r_a1, r_a2, r_a3);
    *(uint32_t *)(uintptr_t)(r_s4 + 4) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_s4 + 8);
    r_a0 = r_t2;
    r_a3 = 0;
    r_a2 = 128U;
    r_a1 = 0;
    *(uint32_t *)(uintptr_t)(r_sp + 16) = (uint32_t)r_v0;
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_StrenCal_part_0(r_a0, r_a1, r_a2, r_a3);
    *(uint32_t *)(uintptr_t)(r_s4 + 8) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_s4 + 12);
    r_a3 = 0;
    r_a2 = 128U;
    *(uint32_t *)(uintptr_t)(r_sp + 16) = (uint32_t)r_v0;
    r_a1 = 0;
    r_a0 = r_t2;
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_StrenCal_part_0(r_a0, r_a1, r_a2, r_a3);
    *(uint32_t *)(uintptr_t)(r_s4 + 12) = (uint32_t)r_v0;
L_68878:
    r_v0 = (uint32_t)(uintptr_t)&tisp_bcsh_Toffset_RGB2YUV; /* HI16 tisp_bcsh_Toffset_RGB2YUV */
    r_a0 = r_s8;
    r_v0 = (uint32_t)(uintptr_t)&tisp_bcsh_Toffset_RGB2YUV;
    r_a2 = r_t6;
    r_a1 = r_t5;
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_Toffset_RGB2YUV(r_a0, r_a1, r_a2, r_a3);
    r_v1 = *(uint32_t *)(uintptr_t)(r_sp + 32);
    r_v0 = *(uint32_t *)(uintptr_t)(r_s8 + 0);
    r_a0 = *(uint16_t *)(uintptr_t)(r_v1 + 2);
    r_v1 = *(uint32_t *)(uintptr_t)(r_sp + 64);
    r_v0 = r_v0 + -0x800U;
    r_v0 = r_v0 + r_a0;
    r_v0 = r_v0 + r_v1;
    *(uint32_t *)(uintptr_t)(r_s7 + 0) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_s8 + 4);
    *(uint32_t *)(uintptr_t)(r_s7 + 4) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_s8 + 8);
    *(uint32_t *)(uintptr_t)(r_s7 + 8) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_s5 + 0);
    r_a3 = *(uint32_t *)(uintptr_t)(r_s5 + 4);
    r_s8 = *(uint32_t *)(uintptr_t)(r_s5 + 8);
    r_t4 = *(uint32_t *)(uintptr_t)(r_s5 + 12);
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    r_t7 = *(uint32_t *)(uintptr_t)(r_s5 + 16);
    if (mips_cond) goto L_688e4;
    r_v0 = 128U;
    mips_cond = (r_s0 != r_v0) ? 1U : 0U;
    r_t5 = (uint32_t)(uintptr_t)&tisp_bcsh_StrenCal_part0_lit; /* HI16 .text */
    if (mips_cond) goto L_68ae0;
L_688e4:
    r_s7 = r_a3;
L_688e8:
    r_v0 = (uint32_t)(uintptr_t)&s_bcsh_mjpeg_mode_lit[0];
    r_v0 = r_s2 + r_v0;
    r_v0 = *(uint8_t *)(uintptr_t)(r_v0 + 0);
    r_a0 = 1U;
    *(uint32_t *)(uintptr_t)(r_s5 + 4) = (uint32_t)r_s7;
    *(uint32_t *)(uintptr_t)(r_s5 + 8) = (uint32_t)r_s8;
    *(uint32_t *)(uintptr_t)(r_s5 + 12) = (uint32_t)r_t4;
    mips_cond = (r_v0 != r_a0) ? 1U : 0U;
    *(uint32_t *)(uintptr_t)(r_s5 + 16) = (uint32_t)r_t7;
    if (mips_cond) goto L_68954;
    *(uint32_t *)(uintptr_t)(r_s5 + 0) = (uint32_t)r_v0;
    r_v0 = (uint32_t)(uintptr_t)&s_bcsh_mjpeg_y_range_low_lit[0];
    r_v0 = r_s2 + r_v0;
    r_v0 = *(uint8_t *)(uintptr_t)(r_v0 + 0);
    *(uint32_t *)(uintptr_t)(r_s5 + 12) = (uint32_t)0;
    r_v0 = r_v0 << 2;
    *(uint32_t *)(uintptr_t)(r_s5 + 4) = (uint32_t)r_v0;
    r_v0 = (uint32_t)(uintptr_t)&s_bcsh_mjpeg_y_range_high_lit[0];
    r_s2 = r_s2 + r_v0;
    r_v0 = *(uint8_t *)(uintptr_t)(r_s2 + 0);
    r_v0 = r_v0 << 2;
    r_v0 = r_v0 + 0x3U;
    *(uint32_t *)(uintptr_t)(r_s5 + 8) = (uint32_t)r_v0;
    r_v0 = 1023U;
    *(uint32_t *)(uintptr_t)(r_s5 + 16) = (uint32_t)r_v0;
L_68954:
    r_a0 = *(uint32_t *)(uintptr_t)(r_s5 + 4);
    mips_cond = (r_a0 != 0) ? 1U : 0U;
    r_v1 = *(uint32_t *)(uintptr_t)(r_sp + 36);
    if (mips_cond) goto L_68ca8;
    r_v0 = *(uint32_t *)(uintptr_t)(r_sp + 36);
    *(uint32_t *)(uintptr_t)(r_v0 + 0) = (uint32_t)0;
L_68968:
    r_a2 = *(uint32_t *)(uintptr_t)(r_s5 + 4);
    r_a0 = *(uint32_t *)(uintptr_t)(r_s5 + 8);
    r_v0 = (r_a2 < r_a0) ? 1U : 0U;
    mips_cond = (r_v0 != 0) ? 1U : 0U;
    r_v0 = *(uint32_t *)(uintptr_t)(r_sp + 40);
    if (mips_cond) goto L_68cc0;
    *(uint32_t *)(uintptr_t)(r_s5 + 4) = (uint32_t)r_a0;
    *(uint32_t *)(uintptr_t)(r_v0 + 0) = (uint32_t)0;
L_68984:
    r_v0 = *(uint32_t *)(uintptr_t)(r_sp + 60);
    r_a1 = *(uint32_t *)(uintptr_t)(r_s5 + 8);
    r_a0 = *(uint16_t *)(uintptr_t)(r_v0 + 2);
    r_v0 = (r_a1 < r_a0) ? 1U : 0U;
    mips_cond = (r_v0 != 0) ? 1U : 0U;
    r_v0 = *(uint32_t *)(uintptr_t)(r_sp + 44);
    if (mips_cond) goto L_68cfc;
    *(uint32_t *)(uintptr_t)(r_s5 + 8) = (uint32_t)r_a0;
    *(uint32_t *)(uintptr_t)(r_v0 + 0) = (uint32_t)0;
L_689a4:
    r_a3 = *(uint32_t *)(uintptr_t)(r_s1 + 4);
    r_v0 = *(uint32_t *)(uintptr_t)(r_s1 + 8);
    r_a0 = (r_a3 < r_v0) ? 1U : 0U;
    mips_cond = (r_a0 != 0) ? 1U : 0U;
    /* nop */
    if (mips_cond) goto L_68d34;
    *(uint32_t *)(uintptr_t)(r_s1 + 4) = (uint32_t)r_v0;
    *(uint32_t *)(uintptr_t)(r_s3 + 0) = (uint32_t)0;
    *(uint32_t *)(uintptr_t)(r_s3 + 4) = (uint32_t)0;
L_689c4:
    r_v1 = *(uint16_t *)(uintptr_t)(r_s6 + 2);
    r_v0 = *(uint16_t *)(uintptr_t)(r_s6 + 4);
    r_a0 = (r_v1 < r_v0) ? 1U : 0U;
    mips_cond = (r_a0 != 0) ? 1U : 0U;
    /* nop */
    if (mips_cond) goto L_68d9c;
    *(uint16_t *)(uintptr_t)(r_s6 + 2) = (uint16_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_sp + 48);
    *(uint32_t *)(uintptr_t)(r_v0 + 0) = (uint32_t)0;
L_689e4:
    r_v0 = *(uint32_t *)(uintptr_t)(r_sp + 24);
    r_v1 = *(uint16_t *)(uintptr_t)(r_v0 + 2);
    r_v0 = *(uint16_t *)(uintptr_t)(r_v0 + 4);
    r_a0 = (r_v1 < r_v0) ? 1U : 0U;
    mips_cond = (r_a0 != 0) ? 1U : 0U;
    /* nop */
    if (mips_cond) goto L_68db8;
    r_v1 = *(uint32_t *)(uintptr_t)(r_sp + 24);
    *(uint16_t *)(uintptr_t)(r_v1 + 2) = (uint16_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_sp + 52);
    *(uint32_t *)(uintptr_t)(r_v0 + 0) = (uint32_t)0;
L_68a0c:
    r_v0 = *(uint32_t *)(uintptr_t)(r_sp + 28);
    r_v1 = *(uint16_t *)(uintptr_t)(r_v0 + 2);
    r_v0 = *(uint16_t *)(uintptr_t)(r_v0 + 4);
    r_a0 = (r_v1 < r_v0) ? 1U : 0U;
    mips_cond = (r_a0 != 0) ? 1U : 0U;
    /* nop */
    if (mips_cond) goto L_68dd4;
    r_v1 = *(uint32_t *)(uintptr_t)(r_sp + 28);
    *(uint16_t *)(uintptr_t)(r_v1 + 2) = (uint16_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_sp + 56);
    *(uint32_t *)(uintptr_t)(r_v0 + 0) = (uint32_t)0;
L_68a34:
    r_ra = *(uint32_t *)(uintptr_t)(r_sp + 108);
    r_s8 = *(uint32_t *)(uintptr_t)(r_sp + 104);
    r_s7 = *(uint32_t *)(uintptr_t)(r_sp + 100);
    r_s6 = *(uint32_t *)(uintptr_t)(r_sp + 96);
    r_s5 = *(uint32_t *)(uintptr_t)(r_sp + 92);
    r_s4 = *(uint32_t *)(uintptr_t)(r_sp + 88);
    r_s3 = *(uint32_t *)(uintptr_t)(r_sp + 84);
    r_s2 = *(uint32_t *)(uintptr_t)(r_sp + 80);
    r_s1 = *(uint32_t *)(uintptr_t)(r_sp + 76);
    r_s0 = *(uint32_t *)(uintptr_t)(r_sp + 72);
    r_sp = r_sp + 0x70U;
    goto fn_exit;
L_68a64:
    r_v0 = 128U;
    mips_cond = (r_t1 == r_v0) ? 1U : 0U;
    r_t7 = 6144U;
    if (mips_cond) goto L_687d0;
    r_a3 = *(uint32_t *)(uintptr_t)(r_s4 + 0);
    r_t4 = (uint32_t)(uintptr_t)&tisp_bcsh_StrenCal_part0_lit; /* HI16 .text */
    r_a0 = r_t1;
    *(uint32_t *)(uintptr_t)(r_sp + 16) = (uint32_t)r_t7;
    r_t4 = (uint32_t)(uintptr_t)&tisp_bcsh_StrenCal_part0_lit;
    r_a2 = 256U;
    r_a1 = 128U;
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_StrenCal_part_0(r_a0, r_a1, r_a2, r_a3);
    r_a3 = *(uint32_t *)(uintptr_t)(r_s4 + 4);
    r_a0 = r_t1;
    *(uint32_t *)(uintptr_t)(r_s4 + 0) = (uint32_t)r_v0;
    r_a2 = 256U;
    *(uint32_t *)(uintptr_t)(r_sp + 16) = (uint32_t)r_t7;
    r_a1 = 128U;
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_StrenCal_part_0(r_a0, r_a1, r_a2, r_a3);
    r_a3 = *(uint32_t *)(uintptr_t)(r_s4 + 8);
    *(uint32_t *)(uintptr_t)(r_s4 + 4) = (uint32_t)r_v0;
    r_a2 = 256U;
    r_a1 = 128U;
    *(uint32_t *)(uintptr_t)(r_sp + 16) = (uint32_t)r_t7;
    r_a0 = r_t1;
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_StrenCal_part_0(r_a0, r_a1, r_a2, r_a3);
    *(uint32_t *)(uintptr_t)(r_s4 + 8) = (uint32_t)r_v0;
    r_a3 = *(uint32_t *)(uintptr_t)(r_s4 + 12);
    r_a2 = 256U;
    *(uint32_t *)(uintptr_t)(r_sp + 16) = (uint32_t)r_t7;
    r_a1 = 128U;
    goto L_687c4;
L_68ae0:
    r_t6 = (uint32_t)(uintptr_t)&tisp_bcsh_StrenCal; /* HI16 tisp_bcsh_StrenCal */
    r_v0 = (uint32_t)(int32_t)(int8_t)(uint8_t)r_s0; /* seb v0,s0 */
    r_t5 = (uint32_t)(uintptr_t)&tisp_bcsh_StrenCal_part0_lit;
    mips_cond = ((int32_t)r_v0 < 0) ? 1U : 0U;
    r_t6 = (uint32_t)(uintptr_t)&tisp_bcsh_StrenCal;
    if (mips_cond) goto L_68c10;
    r_v0 = (r_t4 < r_t7) ? 1U : 0U;
    mips_cond = (r_v0 != 0) ? 1U : 0U;
    r_t8 = r_t7 - r_t4;
    if (mips_cond) goto L_68b04;
    r_t8 = r_t4 - r_t7;
L_68b04:
    *(uint32_t *)(uintptr_t)(r_sp + 16) = (uint32_t)r_a3;
    r_a2 = 128U;
    r_a3 = 0;
    r_a1 = 0;
    r_a0 = r_s0;
    r_t8 = r_t8 >> 1;
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_StrenCal_part_0(r_a0, r_a1, r_a2, r_a3);
    r_t2 = 1U;
    *(uint32_t *)(uintptr_t)(r_sp + 20) = (uint32_t)r_t2;
    *(uint32_t *)(uintptr_t)(r_sp + 16) = (uint32_t)r_s8;
    r_a3 = 1023U;
    r_a2 = 128U;
    r_a1 = 0;
    r_a0 = r_s0;
    *(uint32_t *)(uintptr_t)(r_sp + 32) = (uint32_t)r_t2;
    r_s7 = r_v0;
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_StrenCal(r_a0, r_a1, r_a2, r_a3);
    r_t2 = *(uint32_t *)(uintptr_t)(r_sp + 32);
    r_t8 = r_t8 + r_t4;
    r_a3 = r_t8;
    *(uint32_t *)(uintptr_t)(r_sp + 20) = (uint32_t)r_t2;
    *(uint32_t *)(uintptr_t)(r_sp + 16) = (uint32_t)r_t4;
    r_a2 = 128U;
    r_a1 = 0;
    r_a0 = r_s0;
    r_s8 = r_v0;
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_StrenCal(r_a0, r_a1, r_a2, r_a3);
    *(uint32_t *)(uintptr_t)(r_sp + 16) = (uint32_t)r_t7;
    r_a2 = 128U;
    r_a1 = 0;
    r_a0 = r_s0;
    r_t4 = r_v0;
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_StrenCal_part_0(r_a0, r_a1, r_a2, r_a3);
    r_t7 = r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_s4 + 0);
    r_a3 = 0;
    r_a2 = 128U;
    r_a1 = 0;
    r_a0 = r_s0;
    *(uint32_t *)(uintptr_t)(r_sp + 16) = (uint32_t)r_v0;
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_StrenCal_part_0(r_a0, r_a1, r_a2, r_a3);
    *(uint32_t *)(uintptr_t)(r_s4 + 0) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_s4 + 4);
    r_a3 = 0;
    r_a2 = 128U;
    r_a1 = 0;
    r_a0 = r_s0;
    *(uint32_t *)(uintptr_t)(r_sp + 16) = (uint32_t)r_v0;
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_StrenCal_part_0(r_a0, r_a1, r_a2, r_a3);
    *(uint32_t *)(uintptr_t)(r_s4 + 4) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_s4 + 8);
    r_a3 = 0;
    r_a2 = 128U;
    r_a1 = 0;
    r_a0 = r_s0;
    *(uint32_t *)(uintptr_t)(r_sp + 16) = (uint32_t)r_v0;
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_StrenCal_part_0(r_a0, r_a1, r_a2, r_a3);
    *(uint32_t *)(uintptr_t)(r_s4 + 8) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_s4 + 12);
    r_a3 = 0;
    r_a2 = 128U;
    *(uint32_t *)(uintptr_t)(r_sp + 16) = (uint32_t)r_v0;
    r_a1 = 0;
    r_a0 = r_s0;
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_StrenCal_part_0(r_a0, r_a1, r_a2, r_a3);
    *(uint32_t *)(uintptr_t)(r_s4 + 12) = (uint32_t)r_v0;
    goto L_688e8;
L_68c10:
    r_v0 = (r_a3 < r_s8) ? 1U : 0U;
    mips_cond = (r_v0 != 0) ? 1U : 0U;
    r_t2 = r_s8 - r_a3;
    if (mips_cond) goto L_68c20;
    r_t2 = r_a3 - r_s8;
L_68c20:
    r_t2 = r_t2 >> 1;
    r_t2 = r_t2 + r_a3;
    *(uint32_t *)(uintptr_t)(r_sp + 16) = (uint32_t)r_t2;
    r_a2 = 255U;
    r_a1 = 128U;
    r_a0 = r_s0;
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_StrenCal_part_0(r_a0, r_a1, r_a2, r_a3);
    r_t8 = 1U;
    *(uint32_t *)(uintptr_t)(r_sp + 20) = (uint32_t)r_t8;
    *(uint32_t *)(uintptr_t)(r_sp + 16) = (uint32_t)r_t2;
    r_a3 = r_s8;
    r_a2 = 255U;
    r_a1 = 128U;
    r_a0 = r_s0;
    r_s7 = r_v0;
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_StrenCal(r_a0, r_a1, r_a2, r_a3);
    r_a3 = r_t4;
    *(uint32_t *)(uintptr_t)(r_sp + 20) = (uint32_t)r_t8;
    r_a2 = 255U;
    r_a1 = 128U;
    r_a0 = r_s0;
    *(uint32_t *)(uintptr_t)(r_sp + 16) = (uint32_t)0;
    r_s8 = r_v0;
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_StrenCal(r_a0, r_a1, r_a2, r_a3);
    r_t4 = r_v0;
    r_v0 = 1023U;
    r_a3 = r_t7;
    *(uint32_t *)(uintptr_t)(r_sp + 16) = (uint32_t)r_v0;
    r_a2 = 255U;
    r_a1 = 128U;
    r_a0 = r_s0;
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_StrenCal_part_0(r_a0, r_a1, r_a2, r_a3);
    r_t7 = r_v0;
    goto L_688e8;
L_68ca8:
    r_v0 = *(uint32_t *)(uintptr_t)(r_s5 + 12);
    r_v0 = r_v0 << 10;
    if (r_a0) { mips_lo = (uint32_t)(r_v0 / r_a0); mips_hi = (uint32_t)(r_v0 % r_a0); }
    r_v0 = mips_lo;
    *(uint32_t *)(uintptr_t)(r_v1 + 0) = (uint32_t)r_v0;
    goto L_68968;
L_68cc0:
    r_a1 = *(uint32_t *)(uintptr_t)(r_s5 + 16);
    r_v0 = *(uint32_t *)(uintptr_t)(r_s5 + 12);
    r_a3 = (r_v0 < r_a1) ? 1U : 0U;
    mips_cond = (r_a3 == 0) ? 1U : 0U;
    /* nop */
    if (mips_cond) goto L_68cf4;
    r_v0 = r_a1 - r_v0;
L_68cd8:
    r_v0 = r_v0 << 10;
    r_a0 = r_a0 - r_a2;
    if (r_a0) { mips_lo = (uint32_t)(r_v0 / r_a0); mips_hi = (uint32_t)(r_v0 % r_a0); }
    r_v1 = *(uint32_t *)(uintptr_t)(r_sp + 40);
    r_v0 = mips_lo;
    *(uint32_t *)(uintptr_t)(r_v1 + 0) = (uint32_t)r_v0;
    goto L_68984;
L_68cf4:
    r_v0 = r_v0 - r_a1;
    goto L_68cd8;
L_68cfc:
    r_v0 = *(uint32_t *)(uintptr_t)(r_s5 + 16);
    r_a2 = (r_v0 < r_a0) ? 1U : 0U;
    mips_cond = (r_a2 == 0) ? 1U : 0U;
    /* nop */
    if (mips_cond) goto L_68d2c;
    r_v0 = r_a0 - r_v0;
L_68d10:
    r_v0 = r_v0 << 10;
    r_a0 = r_a0 - r_a1;
    if (r_a0) { mips_lo = (uint32_t)(r_v0 / r_a0); mips_hi = (uint32_t)(r_v0 % r_a0); }
    r_v1 = *(uint32_t *)(uintptr_t)(r_sp + 44);
    r_v0 = mips_lo;
    *(uint32_t *)(uintptr_t)(r_v1 + 0) = (uint32_t)r_v0;
    goto L_689a4;
L_68d2c:
    r_v0 = r_v0 - r_a0;
    goto L_68d10;
L_68d34:
    r_a2 = *(uint32_t *)(uintptr_t)(r_s4 + 4);
    r_a1 = *(uint32_t *)(uintptr_t)(r_s4 + 0);
    r_t0 = (r_a1 < r_a2) ? 1U : 0U;
    mips_cond = (r_t0 == 0) ? 1U : 0U;
    r_a0 = r_a1 - r_a2;
    if (mips_cond) goto L_68d4c;
    r_a0 = r_a2 - r_a1;
L_68d4c:
    r_v0 = r_v0 - r_a3;
    if (r_v0) { mips_lo = (uint32_t)(r_a0 / r_v0); mips_hi = (uint32_t)(r_a0 % r_v0); }
    r_a0 = mips_lo;
    *(uint32_t *)(uintptr_t)(r_s3 + 0) = (uint32_t)r_a0;
    r_a0 = *(uint32_t *)(uintptr_t)(r_s4 + 12);
    r_v1 = *(uint32_t *)(uintptr_t)(r_s4 + 8);
    r_a1 = (r_v1 < r_a0) ? 1U : 0U;
    mips_cond = (r_a1 == 0) ? 1U : 0U;
    r_v0 = r_v1 - r_a0;
    if (mips_cond) goto L_68d74;
    r_v0 = r_a0 - r_v1;
L_68d74:
    r_a1 = *(uint32_t *)(uintptr_t)(r_s1 + 8);
    r_a0 = *(uint32_t *)(uintptr_t)(r_s1 + 4);
    r_a2 = (r_a0 < r_a1) ? 1U : 0U;
    mips_cond = (r_a2 == 0) ? 1U : 0U;
    r_v1 = r_a0 - r_a1;
    if (mips_cond) goto L_68d8c;
    r_v1 = r_a1 - r_a0;
L_68d8c:
    if (r_v1) { mips_lo = (uint32_t)(r_v0 / r_v1); mips_hi = (uint32_t)(r_v0 % r_v1); }
    r_v0 = mips_lo;
    *(uint32_t *)(uintptr_t)(r_s3 + 4) = (uint32_t)r_v0;
    goto L_689c4;
L_68d9c:
    r_v0 = r_v0 - r_v1;
    r_v1 = 1024U;
    if (r_v0) { mips_lo = (uint32_t)((int32_t)r_v1 / (int32_t)r_v0); mips_hi = (uint32_t)((int32_t)r_v1 % (int32_t)r_v0); }
    r_v0 = *(uint32_t *)(uintptr_t)(r_sp + 48);
    r_v1 = mips_lo;
    *(uint32_t *)(uintptr_t)(r_v0 + 0) = (uint32_t)r_v1;
    goto L_689e4;
L_68db8:
    r_v0 = r_v0 - r_v1;
    r_v1 = 1024U;
    if (r_v0) { mips_lo = (uint32_t)((int32_t)r_v1 / (int32_t)r_v0); mips_hi = (uint32_t)((int32_t)r_v1 % (int32_t)r_v0); }
    r_v0 = *(uint32_t *)(uintptr_t)(r_sp + 52);
    r_v1 = mips_lo;
    *(uint32_t *)(uintptr_t)(r_v0 + 0) = (uint32_t)r_v1;
    goto L_68a0c;
L_68dd4:
    r_v0 = r_v0 - r_v1;
    r_v1 = 1024U;
    if (r_v0) { mips_lo = (uint32_t)((int32_t)r_v1 / (int32_t)r_v0); mips_hi = (uint32_t)((int32_t)r_v1 % (int32_t)r_v0); }
    r_v0 = *(uint32_t *)(uintptr_t)(r_sp + 56);
    r_v1 = mips_lo;
    *(uint32_t *)(uintptr_t)(r_v0 + 0) = (uint32_t)r_v1;
    goto L_68a34;
fn_exit:
    return (int32_t)r_v0;
}

static int32_t tisp_bcsh_ev_set_parameter_lit(uint32_t a0_in, uint32_t a1_in)
{
    uint32_t r_v0 = 0, r_v1 = 0;
    uint32_t r_at = 0, r_a0 = 0, r_a1 = 0, r_a2 = 0, r_a3 = 0;
    uint32_t r_t0 = 0, r_t1 = 0, r_t2 = 0, r_t3 = 0, r_t4 = 0, r_t5 = 0;
    uint32_t r_t6 = 0, r_t7 = 0, r_t8 = 0, r_t9 = 0;
    uint32_t r_s0 = 0, r_s1 = 0, r_s2 = 0, r_s3 = 0, r_s4 = 0, r_s5 = 0;
    uint32_t r_s6 = 0, r_s7 = 0, r_s8 = 0, r_k0 = 0, r_k1 = 0, r_gp = 0, r_ra = 0;
    uint32_t mips_lo = 0, mips_hi = 0, mips_cond = 0;
    uint8_t mips_stack[1024] __attribute__((aligned(8)));
    uint32_t r_sp = (uint32_t)(uintptr_t)(mips_stack + 512);
    r_a0 = a0_in;
    r_a1 = a1_in;
    (void)r_at; (void)r_k0; (void)r_k1; (void)r_gp; (void)r_t8; (void)r_t9;
    (void)r_ra; (void)mips_hi; (void)mips_lo; (void)mips_cond;
    r_sp = r_sp + -0xc8U;
    *(uint32_t *)(uintptr_t)(r_sp + 196) = (uint32_t)r_ra;
    *(uint32_t *)(uintptr_t)(r_sp + 192) = (uint32_t)r_s8;
    *(uint32_t *)(uintptr_t)(r_sp + 188) = (uint32_t)r_s7;
    *(uint32_t *)(uintptr_t)(r_sp + 184) = (uint32_t)r_s6;
    *(uint32_t *)(uintptr_t)(r_sp + 180) = (uint32_t)r_s5;
    *(uint32_t *)(uintptr_t)(r_sp + 176) = (uint32_t)r_s4;
    *(uint32_t *)(uintptr_t)(r_sp + 172) = (uint32_t)r_s3;
    *(uint32_t *)(uintptr_t)(r_sp + 168) = (uint32_t)r_s2;
    *(uint32_t *)(uintptr_t)(r_sp + 164) = (uint32_t)r_s1;
    *(uint32_t *)(uintptr_t)(r_sp + 160) = (uint32_t)r_s0;
    r_a3 = *(uint32_t *)(uintptr_t)(r_a0 + 4);
    r_a2 = *(uint32_t *)(uintptr_t)(r_a0 + 8);
    r_v0 = *(uint16_t *)(uintptr_t)(r_a3 + 0);
    r_v1 = *(uint16_t *)(uintptr_t)(r_a3 + 2);
    r_t0 = *(uint16_t *)(uintptr_t)(r_a2 + 2);
    r_v0 = r_v0 << 16;
    r_v0 = r_v0 | r_v1;
    r_v1 = *(uint16_t *)(uintptr_t)(r_a2 + 0);
    r_v1 = r_v1 << 16;
    r_v1 = r_v1 | r_t0;
    *(uint32_t *)(uintptr_t)(r_sp + 20) = (uint32_t)r_v1;
    r_v1 = *(uint32_t *)(uintptr_t)(r_a0 + 12);
    r_t0 = *(uint16_t *)(uintptr_t)(r_v1 + 0);
    r_t1 = *(uint16_t *)(uintptr_t)(r_v1 + 2);
    r_t0 = r_t0 << 16;
    r_t0 = r_t0 | r_t1;
    *(uint32_t *)(uintptr_t)(r_sp + 24) = (uint32_t)r_t0;
    r_t0 = *(uint16_t *)(uintptr_t)(r_a3 + 4);
    r_a3 = *(uint16_t *)(uintptr_t)(r_a3 + 6);
    r_t0 = r_t0 << 16;
    r_a3 = r_t0 | r_a3;
    *(uint32_t *)(uintptr_t)(r_sp + 28) = (uint32_t)r_a3;
    r_a3 = *(uint16_t *)(uintptr_t)(r_a2 + 4);
    r_a2 = *(uint16_t *)(uintptr_t)(r_a2 + 6);
    r_a3 = r_a3 << 16;
    r_a2 = r_a3 | r_a2;
    *(uint32_t *)(uintptr_t)(r_sp + 32) = (uint32_t)r_a2;
    r_a2 = *(uint16_t *)(uintptr_t)(r_v1 + 4);
    r_v1 = *(uint16_t *)(uintptr_t)(r_v1 + 6);
    r_a2 = r_a2 << 16;
    r_v1 = r_a2 | r_v1;
    *(uint32_t *)(uintptr_t)(r_sp + 36) = (uint32_t)r_v1;
    r_v1 = *(uint32_t *)(uintptr_t)(r_a0 + 84);
    r_a2 = *(uint32_t *)(uintptr_t)(r_a0 + 80);
    r_a3 = *(uint32_t *)(uintptr_t)(r_v1 + 0);
    r_t0 = *(uint32_t *)(uintptr_t)(r_a2 + 0);
    r_a3 = r_a3 << 16;
    r_a3 = r_a3 | r_t0;
    *(uint32_t *)(uintptr_t)(r_sp + 40) = (uint32_t)r_a3;
    r_a3 = *(uint32_t *)(uintptr_t)(r_v1 + 4);
    r_v1 = *(uint32_t *)(uintptr_t)(r_v1 + 8);
    r_t0 = *(uint32_t *)(uintptr_t)(r_a2 + 4);
    r_a2 = *(uint32_t *)(uintptr_t)(r_a2 + 8);
    r_v1 = r_v1 << 16;
    r_a3 = r_a3 << 16;
    r_v1 = r_v1 | r_a2;
    r_a2 = *(uint32_t *)(uintptr_t)(r_a0 + 44);
    *(uint32_t *)(uintptr_t)(r_sp + 48) = (uint32_t)r_v1;
    r_v1 = *(uint32_t *)(uintptr_t)(r_a0 + 208);
    r_a3 = r_a3 | r_t0;
    r_a2 = *(uint32_t *)(uintptr_t)(r_a2 + 0);
    *(uint32_t *)(uintptr_t)(r_sp + 44) = (uint32_t)r_a3;
    r_a3 = *(uint16_t *)(uintptr_t)(r_v1 + 0);
    r_a2 = r_a2 << 16;
    r_a2 = r_a2 | r_a3;
    *(uint32_t *)(uintptr_t)(r_sp + 52) = (uint32_t)r_a2;
    r_a2 = REGTRACE_LWLR(r_v1, 2); /* lwl/lwr pair */
    /* lwr handled with lwl above */
    r_v1 = *(uint32_t *)(uintptr_t)(r_a0 + 212);
    *(uint32_t *)(uintptr_t)(r_sp + 104) = (uint32_t)r_a2;
    r_a2 = *(uint32_t *)(uintptr_t)(r_a0 + 48);
    r_a3 = *(uint16_t *)(uintptr_t)(r_v1 + 0);
    r_a2 = *(uint32_t *)(uintptr_t)(r_a2 + 0);
    r_a2 = r_a2 << 16;
    r_a2 = r_a2 | r_a3;
    *(uint32_t *)(uintptr_t)(r_sp + 56) = (uint32_t)r_a2;
    r_a2 = REGTRACE_LWLR(r_v1, 2); /* lwl/lwr pair */
    /* lwr handled with lwl above */
    r_v1 = *(uint32_t *)(uintptr_t)(r_a0 + 216);
    *(uint32_t *)(uintptr_t)(r_sp + 100) = (uint32_t)r_a2;
    r_a2 = *(uint32_t *)(uintptr_t)(r_a0 + 52);
    r_a2 = *(uint32_t *)(uintptr_t)(r_a2 + 0);
    r_a3 = *(uint16_t *)(uintptr_t)(r_v1 + 0);
    r_a2 = r_a2 << 16;
    r_a2 = r_a2 | r_a3;
    *(uint32_t *)(uintptr_t)(r_sp + 60) = (uint32_t)r_a2;
    r_a2 = REGTRACE_LWLR(r_v1, 2); /* lwl/lwr pair */
    /* lwr handled with lwl above */
    r_v1 = *(uint32_t *)(uintptr_t)(r_a0 + 252);
    *(uint32_t *)(uintptr_t)(r_sp + 96) = (uint32_t)r_a2;
    r_a2 = *(uint32_t *)(uintptr_t)(r_a0 + 56);
    r_a3 = *(uint32_t *)(uintptr_t)(r_v1 + 0);
    r_a2 = *(uint32_t *)(uintptr_t)(r_a2 + 0);
    r_a2 = r_a2 << 16;
    r_a2 = r_a2 | r_a3;
    *(uint32_t *)(uintptr_t)(r_sp + 64) = (uint32_t)r_a2;
    r_a2 = *(uint32_t *)(uintptr_t)(r_a0 + 64);
    r_a3 = *(uint32_t *)(uintptr_t)(r_a0 + 60);
    r_a2 = *(uint32_t *)(uintptr_t)(r_a2 + 0);
    r_a3 = *(uint32_t *)(uintptr_t)(r_a3 + 0);
    r_a2 = r_a2 << 16;
    r_a2 = r_a2 | r_a3;
    *(uint32_t *)(uintptr_t)(r_sp + 68) = (uint32_t)r_a2;
    r_a2 = *(uint32_t *)(uintptr_t)(r_v1 + 8);
    r_a3 = *(uint32_t *)(uintptr_t)(r_v1 + 4);
    r_a2 = r_a2 << 16;
    r_a2 = r_a2 | r_a3;
    *(uint32_t *)(uintptr_t)(r_sp + 72) = (uint32_t)r_a2;
    r_a2 = *(uint32_t *)(uintptr_t)(r_v1 + 16);
    r_v1 = *(uint32_t *)(uintptr_t)(r_v1 + 12);
    r_a3 = *(uint32_t *)(uintptr_t)(r_a0 + 72);
    r_a2 = r_a2 << 16;
    r_v1 = r_a2 | r_v1;
    *(uint32_t *)(uintptr_t)(r_sp + 76) = (uint32_t)r_v1;
    r_v1 = *(uint32_t *)(uintptr_t)(r_a3 + 4);
    r_a3 = *(uint32_t *)(uintptr_t)(r_a3 + 0);
    r_a2 = *(uint32_t *)(uintptr_t)(r_a0 + 228);
    r_v1 = r_v1 << 16;
    r_a3 = r_a3 << 3;
    r_v1 = r_v1 | r_a3;
    r_a3 = *(uint32_t *)(uintptr_t)(r_a2 + 0);
    r_v1 = r_v1 | r_a3;
    *(uint32_t *)(uintptr_t)(r_sp + 80) = (uint32_t)r_v1;
    r_v1 = *(uint32_t *)(uintptr_t)(r_a2 + 8);
    r_a2 = *(uint32_t *)(uintptr_t)(r_a2 + 4);
    r_v1 = r_v1 << 16;
    r_v1 = r_v1 | r_a2;
    *(uint32_t *)(uintptr_t)(r_sp + 84) = (uint32_t)r_v1;
    r_v1 = *(uint32_t *)(uintptr_t)(r_a0 + 68);
    r_a0 = *(uint32_t *)(uintptr_t)(r_v1 + 0);
    r_a2 = *(uint32_t *)(uintptr_t)(r_v1 + 4);
    r_a0 = r_a0 << 16;
    r_a0 = r_a0 | r_a2;
    *(uint32_t *)(uintptr_t)(r_sp + 88) = (uint32_t)r_a0;
    r_a0 = *(uint32_t *)(uintptr_t)(r_v1 + 8);
    r_v1 = *(uint32_t *)(uintptr_t)(r_v1 + 12);
    r_a0 = r_a0 << 16;
    r_v1 = r_a0 | r_v1;
    *(uint32_t *)(uintptr_t)(r_sp + 92) = (uint32_t)r_v1;
    mips_cond = (r_a1 != 0) ? 1U : 0U;
    r_a0 = 0x10000U;
    if (mips_cond) goto L_69238;
    r_v1 = r_a0 + 0x1070U;
    *(uint32_t *)(uintptr_t)(r_sp + 16) = (uint32_t)r_v1;
    r_s1 = r_a0 + 0x106cU;
    r_s2 = r_a0 + 0x1068U;
    r_s3 = r_a0 + 0x1064U;
    r_s4 = r_a0 + 0x1060U;
    r_s5 = r_a0 + 0x105cU;
    r_s6 = r_a0 + 0x1058U;
    r_s7 = r_a0 + 0x1054U;
    r_s8 = r_a0 + 0x1050U;
    r_v1 = r_a0 + 0x104cU;
    r_a2 = r_a0 + 0x1048U;
    r_a3 = r_a0 + 0x1044U;
    r_t0 = r_a0 + 0x1040U;
    r_t1 = r_a0 + 0x103cU;
    r_t2 = r_a0 + 0x1020U;
    r_t3 = r_a0 + 0x101cU;
    r_t4 = r_a0 + 0x1018U;
    r_t5 = r_a0 + 0x1014U;
    r_t6 = r_a0 + 0x1010U;
    r_t7 = r_a0 + 0x100cU;
    r_t8 = r_a0 + 0x1008U;
    r_t9 = r_a0 + 0x1004U;
    r_a0 = r_a0 + 0x1000U;
L_69088:
    r_s0 = (uint32_t)(uintptr_t)&system_reg_write; /* HI16 system_reg_write */
    r_s0 = (uint32_t)(uintptr_t)&system_reg_write;
    r_a1 = r_v0;
    *(uint32_t *)(uintptr_t)(r_sp + 156) = (uint32_t)r_v1;
    *(uint32_t *)(uintptr_t)(r_sp + 152) = (uint32_t)r_a2;
    *(uint32_t *)(uintptr_t)(r_sp + 148) = (uint32_t)r_a3;
    *(uint32_t *)(uintptr_t)(r_sp + 144) = (uint32_t)r_t0;
    *(uint32_t *)(uintptr_t)(r_sp + 140) = (uint32_t)r_t1;
    *(uint32_t *)(uintptr_t)(r_sp + 136) = (uint32_t)r_t2;
    *(uint32_t *)(uintptr_t)(r_sp + 132) = (uint32_t)r_t3;
    *(uint32_t *)(uintptr_t)(r_sp + 128) = (uint32_t)r_t4;
    *(uint32_t *)(uintptr_t)(r_sp + 124) = (uint32_t)r_t5;
    *(uint32_t *)(uintptr_t)(r_sp + 120) = (uint32_t)r_t6;
    *(uint32_t *)(uintptr_t)(r_sp + 116) = (uint32_t)r_t7;
    *(uint32_t *)(uintptr_t)(r_sp + 112) = (uint32_t)r_t8;
    *(uint32_t *)(uintptr_t)(r_sp + 108) = (uint32_t)r_t9;
    r_v0 = (uint32_t)REGCALL_system_reg_write(r_a0, r_a1, r_a2, r_a3);
    r_t9 = *(uint32_t *)(uintptr_t)(r_sp + 108);
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 20);
    r_a0 = r_t9;
    r_v0 = (uint32_t)REGCALL_system_reg_write(r_a0, r_a1, r_a2, r_a3);
    r_t8 = *(uint32_t *)(uintptr_t)(r_sp + 112);
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 24);
    r_a0 = r_t8;
    r_v0 = (uint32_t)REGCALL_system_reg_write(r_a0, r_a1, r_a2, r_a3);
    r_t7 = *(uint32_t *)(uintptr_t)(r_sp + 116);
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 28);
    r_a0 = r_t7;
    r_v0 = (uint32_t)REGCALL_system_reg_write(r_a0, r_a1, r_a2, r_a3);
    r_t6 = *(uint32_t *)(uintptr_t)(r_sp + 120);
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 32);
    r_a0 = r_t6;
    r_v0 = (uint32_t)REGCALL_system_reg_write(r_a0, r_a1, r_a2, r_a3);
    r_t5 = *(uint32_t *)(uintptr_t)(r_sp + 124);
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 36);
    r_a0 = r_t5;
    r_v0 = (uint32_t)REGCALL_system_reg_write(r_a0, r_a1, r_a2, r_a3);
    r_t4 = *(uint32_t *)(uintptr_t)(r_sp + 128);
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 40);
    r_a0 = r_t4;
    r_v0 = (uint32_t)REGCALL_system_reg_write(r_a0, r_a1, r_a2, r_a3);
    r_t3 = *(uint32_t *)(uintptr_t)(r_sp + 132);
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 44);
    r_a0 = r_t3;
    r_v0 = (uint32_t)REGCALL_system_reg_write(r_a0, r_a1, r_a2, r_a3);
    r_t2 = *(uint32_t *)(uintptr_t)(r_sp + 136);
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 48);
    r_a0 = r_t2;
    r_v0 = (uint32_t)REGCALL_system_reg_write(r_a0, r_a1, r_a2, r_a3);
    r_t1 = *(uint32_t *)(uintptr_t)(r_sp + 140);
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 52);
    r_a0 = r_t1;
    r_v0 = (uint32_t)REGCALL_system_reg_write(r_a0, r_a1, r_a2, r_a3);
    r_t0 = *(uint32_t *)(uintptr_t)(r_sp + 144);
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 104);
    r_a0 = r_t0;
    r_v0 = (uint32_t)REGCALL_system_reg_write(r_a0, r_a1, r_a2, r_a3);
    r_a3 = *(uint32_t *)(uintptr_t)(r_sp + 148);
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 56);
    r_a0 = r_a3;
    r_v0 = (uint32_t)REGCALL_system_reg_write(r_a0, r_a1, r_a2, r_a3);
    r_a2 = *(uint32_t *)(uintptr_t)(r_sp + 152);
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 100);
    r_a0 = r_a2;
    r_v0 = (uint32_t)REGCALL_system_reg_write(r_a0, r_a1, r_a2, r_a3);
    r_v1 = *(uint32_t *)(uintptr_t)(r_sp + 156);
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 60);
    r_a0 = r_v1;
    r_v0 = (uint32_t)REGCALL_system_reg_write(r_a0, r_a1, r_a2, r_a3);
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 96);
    r_a0 = r_s8;
    r_v0 = (uint32_t)REGCALL_system_reg_write(r_a0, r_a1, r_a2, r_a3);
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 64);
    r_a0 = r_s7;
    r_v0 = (uint32_t)REGCALL_system_reg_write(r_a0, r_a1, r_a2, r_a3);
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 68);
    r_a0 = r_s6;
    r_v0 = (uint32_t)REGCALL_system_reg_write(r_a0, r_a1, r_a2, r_a3);
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 72);
    r_a0 = r_s5;
    r_v0 = (uint32_t)REGCALL_system_reg_write(r_a0, r_a1, r_a2, r_a3);
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 76);
    r_a0 = r_s4;
    r_v0 = (uint32_t)REGCALL_system_reg_write(r_a0, r_a1, r_a2, r_a3);
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 80);
    r_a0 = r_s3;
    r_v0 = (uint32_t)REGCALL_system_reg_write(r_a0, r_a1, r_a2, r_a3);
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 84);
    r_a0 = r_s2;
    r_v0 = (uint32_t)REGCALL_system_reg_write(r_a0, r_a1, r_a2, r_a3);
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 88);
    r_a0 = r_s1;
    r_v0 = (uint32_t)REGCALL_system_reg_write(r_a0, r_a1, r_a2, r_a3);
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 92);
    r_a0 = *(uint32_t *)(uintptr_t)(r_sp + 16);
    r_ra = *(uint32_t *)(uintptr_t)(r_sp + 196);
    r_s8 = *(uint32_t *)(uintptr_t)(r_sp + 192);
    r_s7 = *(uint32_t *)(uintptr_t)(r_sp + 188);
    r_s6 = *(uint32_t *)(uintptr_t)(r_sp + 184);
    r_s5 = *(uint32_t *)(uintptr_t)(r_sp + 180);
    r_s4 = *(uint32_t *)(uintptr_t)(r_sp + 176);
    r_s3 = *(uint32_t *)(uintptr_t)(r_sp + 172);
    r_s2 = *(uint32_t *)(uintptr_t)(r_sp + 168);
    r_s1 = *(uint32_t *)(uintptr_t)(r_sp + 164);
    r_t9 = r_s0;
    r_s0 = *(uint32_t *)(uintptr_t)(r_sp + 160);
    r_sp = r_sp + 0xc8U;
    r_v0 = (uint32_t)REGCALL_system_reg_write(r_a0, r_a1, r_a2, r_a3); goto fn_exit;
L_69238:
    r_v1 = r_a0 + 0x1170U;
    *(uint32_t *)(uintptr_t)(r_sp + 16) = (uint32_t)r_v1;
    r_s1 = r_a0 + 0x116cU;
    r_s2 = r_a0 + 0x1168U;
    r_s3 = r_a0 + 0x1164U;
    r_s4 = r_a0 + 0x1160U;
    r_s5 = r_a0 + 0x115cU;
    r_s6 = r_a0 + 0x1158U;
    r_s7 = r_a0 + 0x1154U;
    r_s8 = r_a0 + 0x1150U;
    r_v1 = r_a0 + 0x114cU;
    r_a2 = r_a0 + 0x1148U;
    r_a3 = r_a0 + 0x1144U;
    r_t0 = r_a0 + 0x1140U;
    r_t1 = r_a0 + 0x113cU;
    r_t2 = r_a0 + 0x1120U;
    r_t3 = r_a0 + 0x111cU;
    r_t4 = r_a0 + 0x1118U;
    r_t5 = r_a0 + 0x1114U;
    r_t6 = r_a0 + 0x1110U;
    r_t7 = r_a0 + 0x110cU;
    r_t8 = r_a0 + 0x1108U;
    r_t9 = r_a0 + 0x1104U;
    r_a0 = r_a0 + 0x1100U;
    goto L_69088;
fn_exit:
    return (int32_t)r_v0;
}

static int32_t tisp_bcsh_ev_update_lit(uint32_t a0_in, uint32_t a1_in)
{
    uint32_t r_v0 = 0, r_v1 = 0;
    uint32_t r_at = 0, r_a0 = 0, r_a1 = 0, r_a2 = 0, r_a3 = 0;
    uint32_t r_t0 = 0, r_t1 = 0, r_t2 = 0, r_t3 = 0, r_t4 = 0, r_t5 = 0;
    uint32_t r_t6 = 0, r_t7 = 0, r_t8 = 0, r_t9 = 0;
    uint32_t r_s0 = 0, r_s1 = 0, r_s2 = 0, r_s3 = 0, r_s4 = 0, r_s5 = 0;
    uint32_t r_s6 = 0, r_s7 = 0, r_s8 = 0, r_k0 = 0, r_k1 = 0, r_gp = 0, r_ra = 0;
    uint32_t mips_lo = 0, mips_hi = 0, mips_cond = 0;
    uint8_t mips_stack[1024] __attribute__((aligned(8)));
    uint32_t r_sp = (uint32_t)(uintptr_t)(mips_stack + 512);
    r_a0 = a0_in;
    r_a1 = a1_in;
    (void)r_at; (void)r_k0; (void)r_k1; (void)r_gp; (void)r_t8; (void)r_t9;
    (void)r_ra; (void)mips_hi; (void)mips_lo; (void)mips_cond;
    r_sp = r_sp + -0x48U;
    *(uint32_t *)(uintptr_t)(r_sp + 32) = (uint32_t)r_s0;
    *(uint32_t *)(uintptr_t)(r_sp + 68) = (uint32_t)r_ra;
    *(uint32_t *)(uintptr_t)(r_sp + 64) = (uint32_t)r_s8;
    *(uint32_t *)(uintptr_t)(r_sp + 60) = (uint32_t)r_s7;
    *(uint32_t *)(uintptr_t)(r_sp + 56) = (uint32_t)r_s6;
    *(uint32_t *)(uintptr_t)(r_sp + 52) = (uint32_t)r_s5;
    *(uint32_t *)(uintptr_t)(r_sp + 48) = (uint32_t)r_s4;
    *(uint32_t *)(uintptr_t)(r_sp + 44) = (uint32_t)r_s3;
    *(uint32_t *)(uintptr_t)(r_sp + 40) = (uint32_t)r_s2;
    *(uint32_t *)(uintptr_t)(r_sp + 36) = (uint32_t)r_s1;
    r_s0 = r_a0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_a0 + 152);
    r_a0 = *(uint32_t *)(uintptr_t)(r_a0 + 232);
    *(uint32_t *)(uintptr_t)(r_sp + 24) = (uint32_t)r_a1;
    r_v0 = *(uint32_t *)(uintptr_t)(r_v0 + 0);
    r_v1 = *(uint32_t *)(uintptr_t)(r_a0 + 0);
    r_t5 = *(uint32_t *)(uintptr_t)(r_s0 + 216);
    r_s8 = *(uint32_t *)(uintptr_t)(r_s0 + 220);
    r_v1 = (r_v1 < r_v0) ? 1U : 0U;
    r_s5 = *(uint32_t *)(uintptr_t)(r_s0 + 224);
    r_a1 = *(uint32_t *)(uintptr_t)(r_s0 + 68);
    r_t6 = *(uint32_t *)(uintptr_t)(r_s0 + 236);
    r_t7 = *(uint32_t *)(uintptr_t)(r_s0 + 240);
    r_t8 = *(uint32_t *)(uintptr_t)(r_s0 + 244);
    r_t9 = *(uint32_t *)(uintptr_t)(r_s0 + 248);
    r_a2 = *(uint32_t *)(uintptr_t)(r_s0 + 252);
    r_ra = *(uint32_t *)(uintptr_t)(r_s0 + 256);
    r_s1 = *(uint32_t *)(uintptr_t)(r_s0 + 260);
    r_s2 = *(uint32_t *)(uintptr_t)(r_s0 + 264);
    r_s3 = *(uint32_t *)(uintptr_t)(r_s0 + 268);
    r_s7 = *(uint32_t *)(uintptr_t)(r_s0 + 92);
    mips_cond = (r_v1 != 0) ? 1U : 0U;
    r_s6 = *(uint32_t *)(uintptr_t)(r_s0 + 272);
    if (mips_cond) goto L_693d0;
    r_v0 = *(uint16_t *)(uintptr_t)(r_s8 + 0);
    *(uint16_t *)(uintptr_t)(r_t5 + 2) = (uint16_t)r_v0;
    r_v0 = *(uint16_t *)(uintptr_t)(r_s5 + 0);
    *(uint16_t *)(uintptr_t)(r_t5 + 4) = (uint16_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_t6 + 0);
    *(uint32_t *)(uintptr_t)(r_a1 + 0) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_t7 + 0);
    *(uint32_t *)(uintptr_t)(r_a1 + 4) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_t8 + 0);
    *(uint32_t *)(uintptr_t)(r_a1 + 8) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_t9 + 0);
    *(uint32_t *)(uintptr_t)(r_a1 + 12) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_ra + 0);
    *(uint32_t *)(uintptr_t)(r_a2 + 4) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_s1 + 0);
    *(uint32_t *)(uintptr_t)(r_a2 + 8) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_s2 + 0);
    *(uint32_t *)(uintptr_t)(r_a2 + 12) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_s3 + 0);
    *(uint32_t *)(uintptr_t)(r_a2 + 16) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_s6 + 0);
L_69378:
    *(uint32_t *)(uintptr_t)(r_s7 + 0) = (uint32_t)r_v0;
L_6937c:
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 24);
L_69380:
    r_v0 = (uint32_t)(uintptr_t)&tisp_bcsh_ev_update_TransitParam; /* HI16 tisp_bcsh_ev_update_TransitParam */
    r_v0 = (uint32_t)(uintptr_t)&tisp_bcsh_ev_update_TransitParam;
    r_a0 = r_s0;
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_ev_update_TransitParam(r_a0, r_a1, r_a2, r_a3);
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 24);
    r_ra = *(uint32_t *)(uintptr_t)(r_sp + 68);
    r_s8 = *(uint32_t *)(uintptr_t)(r_sp + 64);
    r_s7 = *(uint32_t *)(uintptr_t)(r_sp + 60);
    r_s6 = *(uint32_t *)(uintptr_t)(r_sp + 56);
    r_s5 = *(uint32_t *)(uintptr_t)(r_sp + 52);
    r_s4 = *(uint32_t *)(uintptr_t)(r_sp + 48);
    r_s3 = *(uint32_t *)(uintptr_t)(r_sp + 44);
    r_s2 = *(uint32_t *)(uintptr_t)(r_sp + 40);
    r_s1 = *(uint32_t *)(uintptr_t)(r_sp + 36);
    r_a0 = r_s0;
    r_s0 = *(uint32_t *)(uintptr_t)(r_sp + 32);
    r_t9 = (uint32_t)(uintptr_t)&tisp_bcsh_ev_set_parameter; /* HI16 tisp_bcsh_ev_set_parameter */
    r_t9 = (uint32_t)(uintptr_t)&tisp_bcsh_ev_set_parameter;
    r_sp = r_sp + 0x48U;
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_ev_set_parameter(r_a0, r_a1, r_a2, r_a3); goto fn_exit;
L_693d0:
    r_v1 = *(uint32_t *)(uintptr_t)(r_a0 + 32);
    r_v1 = (r_v0 < r_v1) ? 1U : 0U;
    mips_cond = (r_v1 != 0) ? 1U : 0U;
    r_v1 = 0;
    if (mips_cond) goto L_69438;
    r_v0 = *(uint16_t *)(uintptr_t)(r_s8 + 16);
    *(uint16_t *)(uintptr_t)(r_t5 + 2) = (uint16_t)r_v0;
    r_v0 = *(uint16_t *)(uintptr_t)(r_s5 + 16);
    *(uint16_t *)(uintptr_t)(r_t5 + 4) = (uint16_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_t6 + 32);
    *(uint32_t *)(uintptr_t)(r_a1 + 0) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_t7 + 32);
    *(uint32_t *)(uintptr_t)(r_a1 + 4) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_t8 + 32);
    *(uint32_t *)(uintptr_t)(r_a1 + 8) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_t9 + 32);
    *(uint32_t *)(uintptr_t)(r_a1 + 12) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_ra + 32);
    *(uint32_t *)(uintptr_t)(r_a2 + 4) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_s1 + 32);
    *(uint32_t *)(uintptr_t)(r_a2 + 8) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_s2 + 32);
    *(uint32_t *)(uintptr_t)(r_a2 + 12) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_s3 + 32);
    *(uint32_t *)(uintptr_t)(r_a2 + 16) = (uint32_t)r_v0;
    r_v0 = *(uint32_t *)(uintptr_t)(r_s6 + 32);
    goto L_69378;
L_69438:
    r_t3 = *(uint32_t *)(uintptr_t)(r_a0 + 0);
    r_t0 = r_v1 + 0x1U;
    *(uint32_t *)(uintptr_t)(r_sp + 16) = (uint32_t)r_t0;
    r_t1 = (r_t3 < r_v0) ? 1U : 0U;
    r_a3 = r_v1 << 2;
    mips_cond = (r_t1 == 0) ? 1U : 0U;
    r_t0 = r_a0 + 0x4U;
    if (mips_cond) goto L_69a7c;
    r_t4 = *(uint32_t *)(uintptr_t)(r_a0 + 4);
    r_t2 = (r_t4 < r_v0) ? 1U : 0U;
    mips_cond = (r_t2 != 0) ? 1U : 0U;
    r_t1 = r_a3 + 0x4U;
    if (mips_cond) goto L_69a7c;
    r_v1 = r_v1 << 1;
    *(uint32_t *)(uintptr_t)(r_sp + 20) = (uint32_t)r_v1;
    r_v1 = r_s8 + r_v1;
    mips_cond = (r_t3 == r_t4) ? 1U : 0U;
    r_t2 = *(uint16_t *)(uintptr_t)(r_v1 + 0);
    if (mips_cond) goto L_694c4;
    r_v1 = *(uint32_t *)(uintptr_t)(r_sp + 16);
    r_s4 = (r_t4 < r_t3) ? 1U : 0U;
    *(uint32_t *)(uintptr_t)(r_sp + 28) = (uint32_t)r_s4;
    r_v1 = r_v1 << 1;
    r_s8 = r_s8 + r_v1;
    r_v1 = *(uint16_t *)(uintptr_t)(r_s8 + 0);
    r_s4 = (r_v1 < r_t2) ? 1U : 0U;
    mips_cond = (r_s4 == 0) ? 1U : 0U;
    r_s8 = r_v0 - r_t3;
    if (mips_cond) goto L_69874;
    r_v1 = r_t2 - r_v1;
    r_s4 = (r_t4 < r_t3) ? 1U : 0U;
    r_v1 = (uint32_t)((int32_t)r_v1 * (int32_t)r_s8);
    mips_cond = (r_s4 == 0) ? 1U : 0U;
    r_s8 = r_t4 - r_t3;
    if (mips_cond) goto L_694b4;
    r_s8 = r_t3 - r_t4;
L_694b4:
    if (r_s8) { mips_lo = (uint32_t)(r_v1 / r_s8); mips_hi = (uint32_t)(r_v1 % r_s8); }
    r_v1 = mips_lo;
    r_t2 = r_t2 - r_v1;
L_694c0:
    r_t2 = r_t2 & 0xffffU;
L_694c4:
    *(uint16_t *)(uintptr_t)(r_t5 + 2) = (uint16_t)r_t2;
    r_v1 = *(uint32_t *)(uintptr_t)(r_sp + 20);
    r_t2 = *(uint32_t *)(uintptr_t)(r_a0 + 0);
    r_t3 = *(uint32_t *)(uintptr_t)(r_t0 + 0);
    r_v1 = r_s5 + r_v1;
    mips_cond = (r_t2 == r_t3) ? 1U : 0U;
    r_v1 = *(uint16_t *)(uintptr_t)(r_v1 + 0);
    if (mips_cond) goto L_69530;
    r_t4 = *(uint32_t *)(uintptr_t)(r_sp + 16);
    r_s8 = (r_t2 < r_v0) ? 1U : 0U;
    r_t4 = r_t4 << 1;
    r_s5 = r_s5 + r_t4;
    r_t4 = *(uint16_t *)(uintptr_t)(r_s5 + 0);
    r_s5 = (r_t4 < r_v1) ? 1U : 0U;
    mips_cond = (r_s5 == 0) ? 1U : 0U;
    r_s4 = (r_t3 < r_t2) ? 1U : 0U;
    if (mips_cond) goto L_6989c;
    r_s5 = r_v1 - r_t4;
    mips_cond = (r_s8 == 0) ? 1U : 0U;
    r_t4 = r_t2 - r_v0;
    if (mips_cond) goto L_69510;
    r_t4 = r_v0 - r_t2;
L_69510:
    r_t4 = (uint32_t)((int32_t)r_t4 * (int32_t)r_s5);
    mips_cond = (r_s4 == 0) ? 1U : 0U;
    r_s5 = r_t3 - r_t2;
    if (mips_cond) goto L_69520;
    r_s5 = r_t2 - r_t3;
L_69520:
    if (r_s5) { mips_lo = (uint32_t)(r_t4 / r_s5); mips_hi = (uint32_t)(r_t4 % r_s5); }
    r_t4 = mips_lo;
    r_v1 = r_v1 - r_t4;
L_6952c:
    r_v1 = r_v1 & 0xffffU;
L_69530:
    *(uint16_t *)(uintptr_t)(r_t5 + 4) = (uint16_t)r_v1;
    r_v1 = *(uint32_t *)(uintptr_t)(r_a0 + 0);
    r_t5 = *(uint32_t *)(uintptr_t)(r_t0 + 0);
    r_t2 = r_t6 + r_a3;
    mips_cond = (r_v1 == r_t5) ? 1U : 0U;
    r_t2 = *(uint32_t *)(uintptr_t)(r_t2 + 0);
    if (mips_cond) goto L_6958c;
    r_t6 = r_t6 + r_t1;
    r_t3 = *(uint32_t *)(uintptr_t)(r_t6 + 0);
    r_s5 = (r_v1 < r_v0) ? 1U : 0U;
    r_t4 = (r_t3 < r_t2) ? 1U : 0U;
    mips_cond = (r_t4 == 0) ? 1U : 0U;
    r_t6 = (r_t5 < r_v1) ? 1U : 0U;
    if (mips_cond) goto L_698cc;
    r_t3 = r_t2 - r_t3;
    mips_cond = (r_s5 == 0) ? 1U : 0U;
    r_t4 = r_v1 - r_v0;
    if (mips_cond) goto L_69570;
    r_t4 = r_v0 - r_v1;
L_69570:
    r_t3 = (uint32_t)((int32_t)r_t4 * (int32_t)r_t3);
    mips_cond = (r_t6 == 0) ? 1U : 0U;
    r_t4 = r_t5 - r_v1;
    if (mips_cond) goto L_69580;
    r_t4 = r_v1 - r_t5;
L_69580:
    if (r_t4) { mips_lo = (uint32_t)(r_t3 / r_t4); mips_hi = (uint32_t)(r_t3 % r_t4); }
    r_t3 = mips_lo;
    r_t2 = r_t2 - r_t3;
L_6958c:
    *(uint32_t *)(uintptr_t)(r_a1 + 0) = (uint32_t)r_t2;
    r_v1 = *(uint32_t *)(uintptr_t)(r_a0 + 0);
    r_t5 = *(uint32_t *)(uintptr_t)(r_t0 + 0);
    r_t2 = r_t7 + r_a3;
    mips_cond = (r_v1 == r_t5) ? 1U : 0U;
    r_t2 = *(uint32_t *)(uintptr_t)(r_t2 + 0);
    if (mips_cond) goto L_695e8;
    r_t7 = r_t7 + r_t1;
    r_t3 = *(uint32_t *)(uintptr_t)(r_t7 + 0);
    r_t6 = (r_t5 < r_v1) ? 1U : 0U;
    r_t4 = (r_t3 < r_t2) ? 1U : 0U;
    mips_cond = (r_t4 == 0) ? 1U : 0U;
    r_t7 = (r_v1 < r_v0) ? 1U : 0U;
    if (mips_cond) goto L_698fc;
    r_t3 = r_t2 - r_t3;
    mips_cond = (r_t7 == 0) ? 1U : 0U;
    r_t4 = r_v1 - r_v0;
    if (mips_cond) goto L_695cc;
    r_t4 = r_v0 - r_v1;
L_695cc:
    r_t3 = (uint32_t)((int32_t)r_t4 * (int32_t)r_t3);
    mips_cond = (r_t6 == 0) ? 1U : 0U;
    r_t4 = r_t5 - r_v1;
    if (mips_cond) goto L_695dc;
    r_t4 = r_v1 - r_t5;
L_695dc:
    if (r_t4) { mips_lo = (uint32_t)(r_t3 / r_t4); mips_hi = (uint32_t)(r_t3 % r_t4); }
    r_t3 = mips_lo;
    r_t2 = r_t2 - r_t3;
L_695e8:
    *(uint32_t *)(uintptr_t)(r_a1 + 4) = (uint32_t)r_t2;
    r_v1 = *(uint32_t *)(uintptr_t)(r_a0 + 0);
    r_t5 = *(uint32_t *)(uintptr_t)(r_t0 + 0);
    r_t2 = r_t8 + r_a3;
    mips_cond = (r_v1 == r_t5) ? 1U : 0U;
    r_t2 = *(uint32_t *)(uintptr_t)(r_t2 + 0);
    if (mips_cond) goto L_69644;
    r_t8 = r_t8 + r_t1;
    r_t3 = *(uint32_t *)(uintptr_t)(r_t8 + 0);
    r_t7 = (r_v1 < r_v0) ? 1U : 0U;
    r_t4 = (r_t3 < r_t2) ? 1U : 0U;
    mips_cond = (r_t4 == 0) ? 1U : 0U;
    r_t6 = (r_t5 < r_v1) ? 1U : 0U;
    if (mips_cond) goto L_6992c;
    r_t3 = r_t2 - r_t3;
    mips_cond = (r_t7 == 0) ? 1U : 0U;
    r_t4 = r_v1 - r_v0;
    if (mips_cond) goto L_69628;
    r_t4 = r_v0 - r_v1;
L_69628:
    r_t3 = (uint32_t)((int32_t)r_t4 * (int32_t)r_t3);
    mips_cond = (r_t6 == 0) ? 1U : 0U;
    r_t4 = r_t5 - r_v1;
    if (mips_cond) goto L_69638;
    r_t4 = r_v1 - r_t5;
L_69638:
    if (r_t4) { mips_lo = (uint32_t)(r_t3 / r_t4); mips_hi = (uint32_t)(r_t3 % r_t4); }
    r_t3 = mips_lo;
    r_t2 = r_t2 - r_t3;
L_69644:
    *(uint32_t *)(uintptr_t)(r_a1 + 8) = (uint32_t)r_t2;
    r_v1 = *(uint32_t *)(uintptr_t)(r_a0 + 0);
    r_t5 = *(uint32_t *)(uintptr_t)(r_t0 + 0);
    r_t2 = r_t9 + r_a3;
    mips_cond = (r_v1 == r_t5) ? 1U : 0U;
    r_t2 = *(uint32_t *)(uintptr_t)(r_t2 + 0);
    if (mips_cond) goto L_696a0;
    r_t9 = r_t9 + r_t1;
    r_t3 = *(uint32_t *)(uintptr_t)(r_t9 + 0);
    r_t7 = (r_v1 < r_v0) ? 1U : 0U;
    r_t4 = (r_t3 < r_t2) ? 1U : 0U;
    mips_cond = (r_t4 == 0) ? 1U : 0U;
    r_t6 = (r_t5 < r_v1) ? 1U : 0U;
    if (mips_cond) goto L_6995c;
    r_t3 = r_t2 - r_t3;
    mips_cond = (r_t7 == 0) ? 1U : 0U;
    r_t4 = r_v1 - r_v0;
    if (mips_cond) goto L_69684;
    r_t4 = r_v0 - r_v1;
L_69684:
    r_t3 = (uint32_t)((int32_t)r_t4 * (int32_t)r_t3);
    mips_cond = (r_t6 == 0) ? 1U : 0U;
    r_t4 = r_t5 - r_v1;
    if (mips_cond) goto L_69694;
    r_t4 = r_v1 - r_t5;
L_69694:
    if (r_t4) { mips_lo = (uint32_t)(r_t3 / r_t4); mips_hi = (uint32_t)(r_t3 % r_t4); }
    r_t3 = mips_lo;
    r_t2 = r_t2 - r_t3;
L_696a0:
    *(uint32_t *)(uintptr_t)(r_a1 + 12) = (uint32_t)r_t2;
    r_v1 = *(uint32_t *)(uintptr_t)(r_a0 + 0);
    r_t4 = *(uint32_t *)(uintptr_t)(r_t0 + 0);
    r_a1 = r_ra + r_a3;
    mips_cond = (r_v1 == r_t4) ? 1U : 0U;
    r_a1 = *(uint32_t *)(uintptr_t)(r_a1 + 0);
    if (mips_cond) goto L_696fc;
    r_ra = r_ra + r_t1;
    r_t2 = *(uint32_t *)(uintptr_t)(r_ra + 0);
    r_t6 = (r_v1 < r_v0) ? 1U : 0U;
    r_t3 = (r_t2 < r_a1) ? 1U : 0U;
    mips_cond = (r_t3 == 0) ? 1U : 0U;
    r_t5 = (r_t4 < r_v1) ? 1U : 0U;
    if (mips_cond) goto L_6998c;
    r_t2 = r_a1 - r_t2;
    mips_cond = (r_t6 == 0) ? 1U : 0U;
    r_t3 = r_v1 - r_v0;
    if (mips_cond) goto L_696e0;
    r_t3 = r_v0 - r_v1;
L_696e0:
    r_t2 = (uint32_t)((int32_t)r_t3 * (int32_t)r_t2);
    mips_cond = (r_t5 == 0) ? 1U : 0U;
    r_t3 = r_t4 - r_v1;
    if (mips_cond) goto L_696f0;
    r_t3 = r_v1 - r_t4;
L_696f0:
    if (r_t3) { mips_lo = (uint32_t)(r_t2 / r_t3); mips_hi = (uint32_t)(r_t2 % r_t3); }
    r_t2 = mips_lo;
    r_a1 = r_a1 - r_t2;
L_696fc:
    *(uint32_t *)(uintptr_t)(r_a2 + 4) = (uint32_t)r_a1;
    r_v1 = *(uint32_t *)(uintptr_t)(r_a0 + 0);
    r_t4 = *(uint32_t *)(uintptr_t)(r_t0 + 0);
    r_a1 = r_s1 + r_a3;
    mips_cond = (r_v1 == r_t4) ? 1U : 0U;
    r_a1 = *(uint32_t *)(uintptr_t)(r_a1 + 0);
    if (mips_cond) goto L_69758;
    r_s1 = r_s1 + r_t1;
    r_t2 = *(uint32_t *)(uintptr_t)(r_s1 + 0);
    r_t6 = (r_v1 < r_v0) ? 1U : 0U;
    r_t3 = (r_t2 < r_a1) ? 1U : 0U;
    mips_cond = (r_t3 == 0) ? 1U : 0U;
    r_t5 = (r_t4 < r_v1) ? 1U : 0U;
    if (mips_cond) goto L_699bc;
    r_t2 = r_a1 - r_t2;
    mips_cond = (r_t6 == 0) ? 1U : 0U;
    r_t3 = r_v1 - r_v0;
    if (mips_cond) goto L_6973c;
    r_t3 = r_v0 - r_v1;
L_6973c:
    r_t2 = (uint32_t)((int32_t)r_t3 * (int32_t)r_t2);
    mips_cond = (r_t5 == 0) ? 1U : 0U;
    r_t3 = r_t4 - r_v1;
    if (mips_cond) goto L_6974c;
    r_t3 = r_v1 - r_t4;
L_6974c:
    if (r_t3) { mips_lo = (uint32_t)(r_t2 / r_t3); mips_hi = (uint32_t)(r_t2 % r_t3); }
    r_t2 = mips_lo;
    r_a1 = r_a1 - r_t2;
L_69758:
    *(uint32_t *)(uintptr_t)(r_a2 + 8) = (uint32_t)r_a1;
    r_v1 = *(uint32_t *)(uintptr_t)(r_a0 + 0);
    r_t4 = *(uint32_t *)(uintptr_t)(r_t0 + 0);
    r_a1 = r_s2 + r_a3;
    mips_cond = (r_v1 == r_t4) ? 1U : 0U;
    r_a1 = *(uint32_t *)(uintptr_t)(r_a1 + 0);
    if (mips_cond) goto L_697b4;
    r_s2 = r_s2 + r_t1;
    r_t2 = *(uint32_t *)(uintptr_t)(r_s2 + 0);
    r_t6 = (r_v1 < r_v0) ? 1U : 0U;
    r_t3 = (r_t2 < r_a1) ? 1U : 0U;
    mips_cond = (r_t3 == 0) ? 1U : 0U;
    r_t5 = (r_t4 < r_v1) ? 1U : 0U;
    if (mips_cond) goto L_699ec;
    r_t2 = r_a1 - r_t2;
    mips_cond = (r_t6 == 0) ? 1U : 0U;
    r_t3 = r_v1 - r_v0;
    if (mips_cond) goto L_69798;
    r_t3 = r_v0 - r_v1;
L_69798:
    r_t2 = (uint32_t)((int32_t)r_t3 * (int32_t)r_t2);
    mips_cond = (r_t5 == 0) ? 1U : 0U;
    r_t3 = r_t4 - r_v1;
    if (mips_cond) goto L_697a8;
    r_t3 = r_v1 - r_t4;
L_697a8:
    if (r_t3) { mips_lo = (uint32_t)(r_t2 / r_t3); mips_hi = (uint32_t)(r_t2 % r_t3); }
    r_t2 = mips_lo;
    r_a1 = r_a1 - r_t2;
L_697b4:
    *(uint32_t *)(uintptr_t)(r_a2 + 12) = (uint32_t)r_a1;
    r_v1 = *(uint32_t *)(uintptr_t)(r_a0 + 0);
    r_t4 = *(uint32_t *)(uintptr_t)(r_t0 + 0);
    r_a1 = r_s3 + r_a3;
    mips_cond = (r_v1 == r_t4) ? 1U : 0U;
    r_a1 = *(uint32_t *)(uintptr_t)(r_a1 + 0);
    if (mips_cond) goto L_69810;
    r_s3 = r_s3 + r_t1;
    r_t2 = *(uint32_t *)(uintptr_t)(r_s3 + 0);
    r_t6 = (r_v1 < r_v0) ? 1U : 0U;
    r_t3 = (r_t2 < r_a1) ? 1U : 0U;
    mips_cond = (r_t3 == 0) ? 1U : 0U;
    r_t5 = (r_t4 < r_v1) ? 1U : 0U;
    if (mips_cond) goto L_69a1c;
    r_t2 = r_a1 - r_t2;
    mips_cond = (r_t6 == 0) ? 1U : 0U;
    r_t3 = r_v1 - r_v0;
    if (mips_cond) goto L_697f4;
    r_t3 = r_v0 - r_v1;
L_697f4:
    r_t2 = (uint32_t)((int32_t)r_t3 * (int32_t)r_t2);
    mips_cond = (r_t5 == 0) ? 1U : 0U;
    r_t3 = r_t4 - r_v1;
    if (mips_cond) goto L_69804;
    r_t3 = r_v1 - r_t4;
L_69804:
    if (r_t3) { mips_lo = (uint32_t)(r_t2 / r_t3); mips_hi = (uint32_t)(r_t2 % r_t3); }
    r_t2 = mips_lo;
    r_a1 = r_a1 - r_t2;
L_69810:
    *(uint32_t *)(uintptr_t)(r_a2 + 16) = (uint32_t)r_a1;
    r_v1 = *(uint32_t *)(uintptr_t)(r_a0 + 0);
    r_t0 = *(uint32_t *)(uintptr_t)(r_t0 + 0);
    r_a3 = r_s6 + r_a3;
    mips_cond = (r_v1 == r_t0) ? 1U : 0U;
    r_a0 = *(uint32_t *)(uintptr_t)(r_a3 + 0);
    if (mips_cond) goto L_6986c;
    r_t1 = r_s6 + r_t1;
    r_a1 = *(uint32_t *)(uintptr_t)(r_t1 + 0);
    r_a3 = (r_t0 < r_v1) ? 1U : 0U;
    r_a2 = (r_a1 < r_a0) ? 1U : 0U;
    mips_cond = (r_a2 == 0) ? 1U : 0U;
    r_t1 = (r_v1 < r_v0) ? 1U : 0U;
    if (mips_cond) goto L_69a4c;
    r_a1 = r_a0 - r_a1;
    mips_cond = (r_t1 == 0) ? 1U : 0U;
    r_a2 = r_v1 - r_v0;
    if (mips_cond) goto L_69850;
    r_a2 = r_v0 - r_v1;
L_69850:
    r_a1 = (uint32_t)((int32_t)r_a2 * (int32_t)r_a1);
    mips_cond = (r_a3 == 0) ? 1U : 0U;
    r_v0 = r_t0 - r_v1;
    if (mips_cond) goto L_69860;
    r_v0 = r_v1 - r_t0;
L_69860:
    if (r_v0) { mips_lo = (uint32_t)(r_a1 / r_v0); mips_hi = (uint32_t)(r_a1 % r_v0); }
    r_a1 = mips_lo;
    r_a0 = r_a0 - r_a1;
L_6986c:
    *(uint32_t *)(uintptr_t)(r_s7 + 0) = (uint32_t)r_a0;
    goto L_6937c;
L_69874:
    r_v1 = r_v1 - r_t2;
    r_s4 = (r_t4 < r_t3) ? 1U : 0U;
    r_v1 = (uint32_t)((int32_t)r_v1 * (int32_t)r_s8);
    mips_cond = (r_s4 == 0) ? 1U : 0U;
    r_s8 = r_t4 - r_t3;
    if (mips_cond) goto L_6988c;
    r_s8 = r_t3 - r_t4;
L_6988c:
    if (r_s8) { mips_lo = (uint32_t)(r_v1 / r_s8); mips_hi = (uint32_t)(r_v1 % r_s8); }
    r_v1 = mips_lo;
    r_t2 = r_t2 + r_v1;
    goto L_694c0;
L_6989c:
    r_t4 = r_t4 - r_v1;
    mips_cond = (r_s8 == 0) ? 1U : 0U;
    r_s5 = r_t2 - r_v0;
    if (mips_cond) goto L_698ac;
    r_s5 = r_v0 - r_t2;
L_698ac:
    r_t4 = (uint32_t)((int32_t)r_s5 * (int32_t)r_t4);
    mips_cond = (r_s4 == 0) ? 1U : 0U;
    r_s5 = r_t3 - r_t2;
    if (mips_cond) goto L_698bc;
    r_s5 = r_t2 - r_t3;
L_698bc:
    if (r_s5) { mips_lo = (uint32_t)(r_t4 / r_s5); mips_hi = (uint32_t)(r_t4 % r_s5); }
    r_t4 = mips_lo;
    r_v1 = r_v1 + r_t4;
    goto L_6952c;
L_698cc:
    r_t4 = r_t3 - r_t2;
    mips_cond = (r_s5 == 0) ? 1U : 0U;
    r_t3 = r_v1 - r_v0;
    if (mips_cond) goto L_698dc;
    r_t3 = r_v0 - r_v1;
L_698dc:
    r_t3 = (uint32_t)((int32_t)r_t3 * (int32_t)r_t4);
    mips_cond = (r_t6 == 0) ? 1U : 0U;
    r_t4 = r_t5 - r_v1;
    if (mips_cond) goto L_698ec;
    r_t4 = r_v1 - r_t5;
L_698ec:
    if (r_t4) { mips_lo = (uint32_t)(r_t3 / r_t4); mips_hi = (uint32_t)(r_t3 % r_t4); }
    r_t3 = mips_lo;
    r_t2 = r_t3 + r_t2;
    goto L_6958c;
L_698fc:
    r_t4 = r_t3 - r_t2;
    mips_cond = (r_t7 == 0) ? 1U : 0U;
    r_t3 = r_v1 - r_v0;
    if (mips_cond) goto L_6990c;
    r_t3 = r_v0 - r_v1;
L_6990c:
    r_t3 = (uint32_t)((int32_t)r_t3 * (int32_t)r_t4);
    mips_cond = (r_t6 == 0) ? 1U : 0U;
    r_t4 = r_t5 - r_v1;
    if (mips_cond) goto L_6991c;
    r_t4 = r_v1 - r_t5;
L_6991c:
    if (r_t4) { mips_lo = (uint32_t)(r_t3 / r_t4); mips_hi = (uint32_t)(r_t3 % r_t4); }
    r_t3 = mips_lo;
    r_t2 = r_t3 + r_t2;
    goto L_695e8;
L_6992c:
    r_t4 = r_t3 - r_t2;
    mips_cond = (r_t7 == 0) ? 1U : 0U;
    r_t3 = r_v1 - r_v0;
    if (mips_cond) goto L_6993c;
    r_t3 = r_v0 - r_v1;
L_6993c:
    r_t3 = (uint32_t)((int32_t)r_t3 * (int32_t)r_t4);
    mips_cond = (r_t6 == 0) ? 1U : 0U;
    r_t4 = r_t5 - r_v1;
    if (mips_cond) goto L_6994c;
    r_t4 = r_v1 - r_t5;
L_6994c:
    if (r_t4) { mips_lo = (uint32_t)(r_t3 / r_t4); mips_hi = (uint32_t)(r_t3 % r_t4); }
    r_t3 = mips_lo;
    r_t2 = r_t3 + r_t2;
    goto L_69644;
L_6995c:
    r_t4 = r_t3 - r_t2;
    mips_cond = (r_t7 == 0) ? 1U : 0U;
    r_t3 = r_v1 - r_v0;
    if (mips_cond) goto L_6996c;
    r_t3 = r_v0 - r_v1;
L_6996c:
    r_t3 = (uint32_t)((int32_t)r_t3 * (int32_t)r_t4);
    mips_cond = (r_t6 == 0) ? 1U : 0U;
    r_t4 = r_t5 - r_v1;
    if (mips_cond) goto L_6997c;
    r_t4 = r_v1 - r_t5;
L_6997c:
    if (r_t4) { mips_lo = (uint32_t)(r_t3 / r_t4); mips_hi = (uint32_t)(r_t3 % r_t4); }
    r_t3 = mips_lo;
    r_t2 = r_t3 + r_t2;
    goto L_696a0;
L_6998c:
    r_t3 = r_t2 - r_a1;
    mips_cond = (r_t6 == 0) ? 1U : 0U;
    r_t2 = r_v1 - r_v0;
    if (mips_cond) goto L_6999c;
    r_t2 = r_v0 - r_v1;
L_6999c:
    r_t2 = (uint32_t)((int32_t)r_t2 * (int32_t)r_t3);
    mips_cond = (r_t5 == 0) ? 1U : 0U;
    r_t3 = r_t4 - r_v1;
    if (mips_cond) goto L_699ac;
    r_t3 = r_v1 - r_t4;
L_699ac:
    if (r_t3) { mips_lo = (uint32_t)(r_t2 / r_t3); mips_hi = (uint32_t)(r_t2 % r_t3); }
    r_t2 = mips_lo;
    r_a1 = r_t2 + r_a1;
    goto L_696fc;
L_699bc:
    r_t3 = r_t2 - r_a1;
    mips_cond = (r_t6 == 0) ? 1U : 0U;
    r_t2 = r_v1 - r_v0;
    if (mips_cond) goto L_699cc;
    r_t2 = r_v0 - r_v1;
L_699cc:
    r_t2 = (uint32_t)((int32_t)r_t2 * (int32_t)r_t3);
    mips_cond = (r_t5 == 0) ? 1U : 0U;
    r_t3 = r_t4 - r_v1;
    if (mips_cond) goto L_699dc;
    r_t3 = r_v1 - r_t4;
L_699dc:
    if (r_t3) { mips_lo = (uint32_t)(r_t2 / r_t3); mips_hi = (uint32_t)(r_t2 % r_t3); }
    r_t2 = mips_lo;
    r_a1 = r_t2 + r_a1;
    goto L_69758;
L_699ec:
    r_t3 = r_t2 - r_a1;
    mips_cond = (r_t6 == 0) ? 1U : 0U;
    r_t2 = r_v1 - r_v0;
    if (mips_cond) goto L_699fc;
    r_t2 = r_v0 - r_v1;
L_699fc:
    r_t2 = (uint32_t)((int32_t)r_t2 * (int32_t)r_t3);
    mips_cond = (r_t5 == 0) ? 1U : 0U;
    r_t3 = r_t4 - r_v1;
    if (mips_cond) goto L_69a0c;
    r_t3 = r_v1 - r_t4;
L_69a0c:
    if (r_t3) { mips_lo = (uint32_t)(r_t2 / r_t3); mips_hi = (uint32_t)(r_t2 % r_t3); }
    r_t2 = mips_lo;
    r_a1 = r_t2 + r_a1;
    goto L_697b4;
L_69a1c:
    r_t3 = r_t2 - r_a1;
    mips_cond = (r_t6 == 0) ? 1U : 0U;
    r_t2 = r_v1 - r_v0;
    if (mips_cond) goto L_69a2c;
    r_t2 = r_v0 - r_v1;
L_69a2c:
    r_t2 = (uint32_t)((int32_t)r_t2 * (int32_t)r_t3);
    mips_cond = (r_t5 == 0) ? 1U : 0U;
    r_t3 = r_t4 - r_v1;
    if (mips_cond) goto L_69a3c;
    r_t3 = r_v1 - r_t4;
L_69a3c:
    if (r_t3) { mips_lo = (uint32_t)(r_t2 / r_t3); mips_hi = (uint32_t)(r_t2 % r_t3); }
    r_t2 = mips_lo;
    r_a1 = r_t2 + r_a1;
    goto L_69810;
L_69a4c:
    r_a2 = r_a1 - r_a0;
    mips_cond = (r_t1 == 0) ? 1U : 0U;
    r_a1 = r_v1 - r_v0;
    if (mips_cond) goto L_69a5c;
    r_a1 = r_v0 - r_v1;
L_69a5c:
    r_v0 = (uint32_t)((int32_t)r_a1 * (int32_t)r_a2);
    mips_cond = (r_a3 == 0) ? 1U : 0U;
    r_a1 = r_t0 - r_v1;
    if (mips_cond) goto L_69a6c;
    r_a1 = r_v1 - r_t0;
L_69a6c:
    if (r_a1) { mips_lo = (uint32_t)(r_v0 / r_a1); mips_hi = (uint32_t)(r_v0 % r_a1); }
    r_v0 = mips_lo;
    r_a0 = r_v0 + r_a0;
    goto L_6986c;
L_69a7c:
    r_v1 = *(uint32_t *)(uintptr_t)(r_sp + 16);
    r_a3 = 8U;
    mips_cond = (r_v1 != r_a3) ? 1U : 0U;
    r_a0 = r_t0;
    if (mips_cond) goto L_69438;
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 24);
    goto L_69380;
fn_exit:
    return (int32_t)r_v0;
}

static int32_t tisp_bcsh_ct_update_lit(uint32_t a0_in, uint32_t a1_in)
{
    uint32_t r_v0 = 0, r_v1 = 0;
    uint32_t r_at = 0, r_a0 = 0, r_a1 = 0, r_a2 = 0, r_a3 = 0;
    uint32_t r_t0 = 0, r_t1 = 0, r_t2 = 0, r_t3 = 0, r_t4 = 0, r_t5 = 0;
    uint32_t r_t6 = 0, r_t7 = 0, r_t8 = 0, r_t9 = 0;
    uint32_t r_s0 = 0, r_s1 = 0, r_s2 = 0, r_s3 = 0, r_s4 = 0, r_s5 = 0;
    uint32_t r_s6 = 0, r_s7 = 0, r_s8 = 0, r_k0 = 0, r_k1 = 0, r_gp = 0, r_ra = 0;
    uint32_t mips_lo = 0, mips_hi = 0, mips_cond = 0;
    uint8_t mips_stack[1024] __attribute__((aligned(8)));
    uint32_t r_sp = (uint32_t)(uintptr_t)(mips_stack + 512);
    r_a0 = a0_in;
    r_a1 = a1_in;
    (void)r_at; (void)r_k0; (void)r_k1; (void)r_gp; (void)r_t8; (void)r_t9;
    (void)r_ra; (void)mips_hi; (void)mips_lo; (void)mips_cond;
    r_sp = r_sp + -0x20U;
    r_v0 = (uint32_t)(uintptr_t)&tisp_bcsh_Tccm_Comp2Orig; /* HI16 tisp_bcsh_Tccm_Comp2Orig */
    r_v0 = (uint32_t)(uintptr_t)&tisp_bcsh_Tccm_Comp2Orig;
    *(uint32_t *)(uintptr_t)(r_sp + 28) = (uint32_t)r_ra;
    *(uint32_t *)(uintptr_t)(r_sp + 24) = (uint32_t)r_s0;
    *(uint32_t *)(uintptr_t)(r_sp + 16) = (uint32_t)r_a1;
    r_s0 = r_a0;
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_Tccm_Comp2Orig(r_a0, r_a1, r_a2, r_a3);
    r_v0 = (uint32_t)(uintptr_t)&tisp_bcsh_ct_interpolation; /* HI16 tisp_bcsh_ct_interpolation */
    r_v0 = (uint32_t)(uintptr_t)&tisp_bcsh_ct_interpolation;
    r_a0 = r_s0;
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_ct_interpolation(r_a0, r_a1, r_a2, r_a3);
    r_v0 = (uint32_t)(uintptr_t)&tisp_bcsh_Tccm_RGB2YUV; /* HI16 tisp_bcsh_Tccm_RGB2YUV */
    r_v0 = (uint32_t)(uintptr_t)&tisp_bcsh_Tccm_RGB2YUV;
    r_a0 = r_s0;
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_Tccm_RGB2YUV(r_a0, r_a1, r_a2, r_a3);
    r_a1 = *(uint32_t *)(uintptr_t)(r_sp + 16);
    r_ra = *(uint32_t *)(uintptr_t)(r_sp + 28);
    r_a0 = r_s0;
    r_s0 = *(uint32_t *)(uintptr_t)(r_sp + 24);
    r_t9 = (uint32_t)(uintptr_t)&tisp_bcsh_ct_set_parameter; /* HI16 tisp_bcsh_ct_set_parameter */
    r_t9 = (uint32_t)(uintptr_t)&tisp_bcsh_ct_set_parameter;
    r_sp = r_sp + 0x20U;
    r_v0 = (uint32_t)REGCALL_tisp_bcsh_ct_set_parameter(r_a0, r_a1, r_a2, r_a3); goto fn_exit;
fn_exit:
    return (int32_t)r_v0;
}

static int32_t tisp_bcsh_params_transmit_lit(uint32_t a0_in, uint32_t a1_in, uint32_t a2_in)
{
    uint32_t r_v0 = 0, r_v1 = 0;
    uint32_t r_at = 0, r_a0 = 0, r_a1 = 0, r_a2 = 0, r_a3 = 0;
    uint32_t r_t0 = 0, r_t1 = 0, r_t2 = 0, r_t3 = 0, r_t4 = 0, r_t5 = 0;
    uint32_t r_t6 = 0, r_t7 = 0, r_t8 = 0, r_t9 = 0;
    uint32_t r_s0 = 0, r_s1 = 0, r_s2 = 0, r_s3 = 0, r_s4 = 0, r_s5 = 0;
    uint32_t r_s6 = 0, r_s7 = 0, r_s8 = 0, r_k0 = 0, r_k1 = 0, r_gp = 0, r_ra = 0;
    uint32_t mips_lo = 0, mips_hi = 0, mips_cond = 0;
    uint8_t mips_stack[1024] __attribute__((aligned(8)));
    uint32_t r_sp = (uint32_t)(uintptr_t)(mips_stack + 512);
    r_a0 = a0_in;
    r_a1 = a1_in;
    r_a2 = a2_in;
    (void)r_at; (void)r_k0; (void)r_k1; (void)r_gp; (void)r_t8; (void)r_t9;
    (void)r_ra; (void)mips_hi; (void)mips_lo; (void)mips_cond;
    r_v0 = r_a1 + 0x4U;
    *(uint32_t *)(uintptr_t)(r_a0 + 4) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0xcU;
    *(uint32_t *)(uintptr_t)(r_a0 + 8) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x14U;
    *(uint32_t *)(uintptr_t)(r_a0 + 12) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x48cU;
    *(uint32_t *)(uintptr_t)(r_a0 + 16) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x4b0U;
    *(uint32_t *)(uintptr_t)(r_a0 + 20) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0x4U;
    *(uint32_t *)(uintptr_t)(r_a0 + 28) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0x28U;
    *(uint32_t *)(uintptr_t)(r_a0 + 32) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0x4cU;
    *(uint32_t *)(uintptr_t)(r_a0 + 36) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0x70U;
    *(uint32_t *)(uintptr_t)(r_a0 + 40) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0x94U;
    *(uint32_t *)(uintptr_t)(r_a0 + 44) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0x98U;
    *(uint32_t *)(uintptr_t)(r_a0 + 48) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0x9cU;
    *(uint32_t *)(uintptr_t)(r_a0 + 52) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0xa0U;
    *(uint32_t *)(uintptr_t)(r_a0 + 56) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0xa4U;
    *(uint32_t *)(uintptr_t)(r_a0 + 60) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0xa8U;
    *(uint32_t *)(uintptr_t)(r_a0 + 64) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0xacU;
    *(uint32_t *)(uintptr_t)(r_a0 + 68) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0xbcU;
    *(uint32_t *)(uintptr_t)(r_a0 + 72) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0xc4U;
    *(uint32_t *)(uintptr_t)(r_a0 + 76) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0xd0U;
    *(uint32_t *)(uintptr_t)(r_a0 + 80) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0xdcU;
    *(uint32_t *)(uintptr_t)(r_a0 + 84) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0xe8U;
    *(uint32_t *)(uintptr_t)(r_a0 + 88) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0x10cU;
    *(uint32_t *)(uintptr_t)(r_a0 + 92) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0x110U;
    *(uint32_t *)(uintptr_t)(r_a0 + 96) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0x120U;
    *(uint32_t *)(uintptr_t)(r_a0 + 100) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0x121U;
    *(uint32_t *)(uintptr_t)(r_a0 + 104) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0x122U;
    *(uint32_t *)(uintptr_t)(r_a0 + 108) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0x124U;
    *(uint32_t *)(uintptr_t)(r_a0 + 112) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0x128U;
    *(uint32_t *)(uintptr_t)(r_a0 + 116) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0x130U;
    *(uint32_t *)(uintptr_t)(r_a0 + 120) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0x138U;
    *(uint32_t *)(uintptr_t)(r_a0 + 124) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0x140U;
    *(uint32_t *)(uintptr_t)(r_a0 + 0) = (uint32_t)r_a1;
    *(uint32_t *)(uintptr_t)(r_a0 + 24) = (uint32_t)r_a2;
    *(uint32_t *)(uintptr_t)(r_a0 + 128) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0x164U;
    *(uint32_t *)(uintptr_t)(r_a0 + 132) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0x188U;
    *(uint32_t *)(uintptr_t)(r_a0 + 136) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0x1acU;
    *(uint32_t *)(uintptr_t)(r_a0 + 140) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0x1adU;
    *(uint32_t *)(uintptr_t)(r_a0 + 144) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0x1aeU;
    *(uint32_t *)(uintptr_t)(r_a0 + 148) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0x1b0U;
    *(uint32_t *)(uintptr_t)(r_a0 + 152) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0x1b4U;
    *(uint32_t *)(uintptr_t)(r_a0 + 156) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0x1b8U;
    *(uint32_t *)(uintptr_t)(r_a0 + 160) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0x1bcU;
    *(uint32_t *)(uintptr_t)(r_a0 + 164) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0x1beU;
    *(uint32_t *)(uintptr_t)(r_a0 + 168) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0x1c0U;
    *(uint32_t *)(uintptr_t)(r_a0 + 172) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0x1c2U;
    *(uint32_t *)(uintptr_t)(r_a0 + 176) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0x1c4U;
    *(uint32_t *)(uintptr_t)(r_a0 + 180) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0x1c8U;
    *(uint32_t *)(uintptr_t)(r_a0 + 184) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0x1c9U;
    *(uint32_t *)(uintptr_t)(r_a0 + 188) = (uint32_t)r_v0;
    r_v0 = r_a2 + 0x1caU;
    *(uint32_t *)(uintptr_t)(r_a0 + 192) = (uint32_t)r_v0;
    r_v0 = *(uint8_t *)(uintptr_t)(r_a2 + 430);
    mips_cond = (r_v0 != 0) ? 1U : 0U;
    r_v0 = r_a1 + 0x254U;
    if (mips_cond) goto L_67b4c;
    r_v0 = r_a1 + 0x1cU;
    *(uint32_t *)(uintptr_t)(r_a0 + 196) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x40U;
    *(uint32_t *)(uintptr_t)(r_a0 + 200) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x64U;
    *(uint32_t *)(uintptr_t)(r_a0 + 204) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x88U;
    *(uint32_t *)(uintptr_t)(r_a0 + 208) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x8eU;
    *(uint32_t *)(uintptr_t)(r_a0 + 212) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x94U;
    *(uint32_t *)(uintptr_t)(r_a0 + 216) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x9aU;
    *(uint32_t *)(uintptr_t)(r_a0 + 220) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0xacU;
    *(uint32_t *)(uintptr_t)(r_a0 + 224) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0xc0U;
    *(uint32_t *)(uintptr_t)(r_a0 + 228) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0xccU;
    *(uint32_t *)(uintptr_t)(r_a0 + 232) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0xf0U;
    *(uint32_t *)(uintptr_t)(r_a0 + 236) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x114U;
    *(uint32_t *)(uintptr_t)(r_a0 + 240) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x138U;
    *(uint32_t *)(uintptr_t)(r_a0 + 244) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x15cU;
    *(uint32_t *)(uintptr_t)(r_a0 + 248) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x180U;
    *(uint32_t *)(uintptr_t)(r_a0 + 252) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x194U;
    *(uint32_t *)(uintptr_t)(r_a0 + 256) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x1b8U;
    *(uint32_t *)(uintptr_t)(r_a0 + 260) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x1dcU;
    *(uint32_t *)(uintptr_t)(r_a0 + 264) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x200U;
    *(uint32_t *)(uintptr_t)(r_a0 + 268) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x224U;
    *(uint32_t *)(uintptr_t)(r_a0 + 272) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x248U;
    *(uint32_t *)(uintptr_t)(r_a0 + 276) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x4d4U;
    *(uint32_t *)(uintptr_t)(r_a0 + 280) = (uint32_t)r_v0;
    r_a1 = r_a1 + 0x4fcU;
L_67b44:
    *(uint32_t *)(uintptr_t)(r_a0 + 284) = (uint32_t)r_a1;
    goto fn_exit;
L_67b4c:
    *(uint32_t *)(uintptr_t)(r_a0 + 196) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x278U;
    *(uint32_t *)(uintptr_t)(r_a0 + 200) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x29cU;
    *(uint32_t *)(uintptr_t)(r_a0 + 204) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x2c0U;
    *(uint32_t *)(uintptr_t)(r_a0 + 208) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x2c6U;
    *(uint32_t *)(uintptr_t)(r_a0 + 212) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x2ccU;
    *(uint32_t *)(uintptr_t)(r_a0 + 216) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x2d2U;
    *(uint32_t *)(uintptr_t)(r_a0 + 220) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x2e4U;
    *(uint32_t *)(uintptr_t)(r_a0 + 224) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x2f8U;
    *(uint32_t *)(uintptr_t)(r_a0 + 228) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x304U;
    *(uint32_t *)(uintptr_t)(r_a0 + 232) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x328U;
    *(uint32_t *)(uintptr_t)(r_a0 + 236) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x34cU;
    *(uint32_t *)(uintptr_t)(r_a0 + 240) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x370U;
    *(uint32_t *)(uintptr_t)(r_a0 + 244) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x394U;
    *(uint32_t *)(uintptr_t)(r_a0 + 248) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x3b8U;
    *(uint32_t *)(uintptr_t)(r_a0 + 252) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x3ccU;
    *(uint32_t *)(uintptr_t)(r_a0 + 256) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x3f0U;
    *(uint32_t *)(uintptr_t)(r_a0 + 260) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x414U;
    *(uint32_t *)(uintptr_t)(r_a0 + 264) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x438U;
    *(uint32_t *)(uintptr_t)(r_a0 + 268) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x45cU;
    *(uint32_t *)(uintptr_t)(r_a0 + 272) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x480U;
    *(uint32_t *)(uintptr_t)(r_a0 + 276) = (uint32_t)r_v0;
    r_v0 = r_a1 + 0x524U;
    *(uint32_t *)(uintptr_t)(r_a0 + 280) = (uint32_t)r_v0;
    r_a1 = r_a1 + 0x54cU;
    goto L_67b44;
fn_exit:
    return (int32_t)r_v0;
}

static int32_t tisp_bcsh_params_refresh_lit(uint32_t a0_in, uint32_t a1_in, uint32_t a2_in)
{
    uint32_t r_v0 = 0, r_v1 = 0;
    uint32_t r_at = 0, r_a0 = 0, r_a1 = 0, r_a2 = 0, r_a3 = 0;
    uint32_t r_t0 = 0, r_t1 = 0, r_t2 = 0, r_t3 = 0, r_t4 = 0, r_t5 = 0;
    uint32_t r_t6 = 0, r_t7 = 0, r_t8 = 0, r_t9 = 0;
    uint32_t r_s0 = 0, r_s1 = 0, r_s2 = 0, r_s3 = 0, r_s4 = 0, r_s5 = 0;
    uint32_t r_s6 = 0, r_s7 = 0, r_s8 = 0, r_k0 = 0, r_k1 = 0, r_gp = 0, r_ra = 0;
    uint32_t mips_lo = 0, mips_hi = 0, mips_cond = 0;
    uint8_t mips_stack[1024] __attribute__((aligned(8)));
    uint32_t r_sp = (uint32_t)(uintptr_t)(mips_stack + 512);
    r_a0 = a0_in;
    r_a1 = a1_in;
    r_a2 = a2_in;
    (void)r_at; (void)r_k0; (void)r_k1; (void)r_gp; (void)r_t8; (void)r_t9;
    (void)r_ra; (void)mips_hi; (void)mips_lo; (void)mips_cond;
    r_sp = r_sp + -0x40U;
    *(uint32_t *)(uintptr_t)(r_sp + 56) = (uint32_t)r_s8;
    *(uint32_t *)(uintptr_t)(r_sp + 52) = (uint32_t)r_s7;
    *(uint32_t *)(uintptr_t)(r_sp + 48) = (uint32_t)r_s6;
    *(uint32_t *)(uintptr_t)(r_sp + 44) = (uint32_t)r_s5;
    *(uint32_t *)(uintptr_t)(r_sp + 40) = (uint32_t)r_s4;
    *(uint32_t *)(uintptr_t)(r_sp + 36) = (uint32_t)r_s3;
    *(uint32_t *)(uintptr_t)(r_sp + 32) = (uint32_t)r_s2;
    *(uint32_t *)(uintptr_t)(r_sp + 28) = (uint32_t)r_s1;
    *(uint32_t *)(uintptr_t)(r_sp + 24) = (uint32_t)r_s0;
    *(uint32_t *)(uintptr_t)(r_sp + 60) = (uint32_t)r_ra;
    r_v0 = *(uint32_t *)(uintptr_t)(r_a0 + 108);
    r_s0 = (uint32_t)(uintptr_t)&memcpy; /* HI16 memcpy */
    r_v1 = 1U;
    r_v0 = *(uint8_t *)(uintptr_t)(r_v0 + 0);
    r_s3 = r_a0;
    r_s1 = r_a1;
    r_s2 = r_a2;
    r_s8 = r_a1 + 0x4U;
    r_s7 = r_a1 + 0xcU;
    r_s6 = r_a1 + 0x14U;
    r_s5 = r_a1 + 0x48cU;
    r_s4 = r_a1 + 0x4b0U;
    mips_cond = (r_v0 == r_v1) ? 1U : 0U;
    r_s0 = (uint32_t)(uintptr_t)&memcpy;
    if (mips_cond) goto L_67d5c;
    mips_cond = (r_v0 == 0) ? 1U : 0U;
    /* nop */
    if (mips_cond) goto L_68190;
    r_v1 = 3U;
    mips_cond = (r_v0 == r_v1) ? 1U : 0U;
    r_a2 = 4U;
    if (mips_cond) goto L_68204;
    r_v1 = 4U;
    mips_cond = (r_v0 != r_v1) ? 1U : 0U;
    /* nop */
    if (mips_cond) goto L_68274;
    r_a1 = *(uint32_t *)(uintptr_t)(r_a0 + 112);
    r_a0 = r_s1;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a1 = *(uint32_t *)(uintptr_t)(r_s3 + 116);
    r_a2 = 4U;
    r_a0 = r_s8;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a1 = *(uint32_t *)(uintptr_t)(r_s3 + 120);
    r_a2 = 4U;
    r_a0 = r_s7;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a1 = *(uint32_t *)(uintptr_t)(r_s3 + 124);
    r_a2 = 4U;
    r_a0 = r_s6;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a1 = *(uint32_t *)(uintptr_t)(r_s3 + 128);
    r_a2 = 4U;
    r_a0 = r_s5;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 4U;
    r_a1 = *(uint32_t *)(uintptr_t)(r_s3 + 132);
    goto L_67dcc;
L_67d5c:
    r_a1 = (uint32_t)(uintptr_t)&au16BT601LimitedOffsetYUVy; /* HI16 au16BT601LimitedOffsetYUVy */
    r_a2 = 4U;
    r_a1 = (uint32_t)(uintptr_t)&au16BT601LimitedOffsetYUVy;
    r_a0 = r_s1;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a1 = (uint32_t)(uintptr_t)&au16BT601LimitedClip0; /* HI16 au16BT601LimitedClip0 */
    r_a2 = 8U;
    r_a1 = (uint32_t)(uintptr_t)&au16BT601LimitedClip0;
    r_a0 = r_s8;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a1 = (uint32_t)(uintptr_t)&au16BT601LimitedClip1; /* HI16 au16BT601LimitedClip1 */
    r_a2 = 8U;
    r_a1 = (uint32_t)(uintptr_t)&au16BT601LimitedClip1;
    r_a0 = r_s7;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a1 = (uint32_t)(uintptr_t)&au16BT601LimitedClip2; /* HI16 au16BT601LimitedClip2 */
    r_a2 = 8U;
    r_a1 = (uint32_t)(uintptr_t)&au16BT601LimitedClip2;
    r_a0 = r_s6;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a1 = (uint32_t)(uintptr_t)&as32BT601LimitedMMatrix; /* HI16 as32BT601LimitedMMatrix */
    r_a2 = 36U;
    r_a1 = (uint32_t)(uintptr_t)&as32BT601LimitedMMatrix;
    r_a0 = r_s5;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a1 = (uint32_t)(uintptr_t)&as32BT601LimitedMinvMatrix; /* HI16 as32BT601LimitedMinvMatrix */
    r_a2 = 36U;
    r_a1 = (uint32_t)(uintptr_t)&as32BT601LimitedMinvMatrix;
L_67dcc:
    r_a0 = r_s4;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_v1 = r_s1 + 0x1cU;
    r_a0 = r_v1;
    r_a2 = 36U;
    r_a1 = r_s2 + 0x1cU;
    r_s8 = r_s1 + 0x40U;
    *(uint32_t *)(uintptr_t)(r_sp + 16) = (uint32_t)r_v1;
    r_s7 = r_s1 + 0x64U;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 36U;
    r_a1 = r_s2 + 0x40U;
    r_a0 = r_s8;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 36U;
    r_a1 = r_s2 + 0x64U;
    r_a0 = r_s7;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 6U;
    r_a1 = r_s2 + 0x88U;
    r_a0 = r_s1 + 0x88U;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 6U;
    r_a1 = r_s2 + 0x8eU;
    r_a0 = r_s1 + 0x8eU;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 6U;
    r_a1 = r_s2 + 0x94U;
    r_a0 = r_s1 + 0x94U;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 18U;
    r_a1 = r_s2 + 0x9aU;
    r_a0 = r_s1 + 0x9aU;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 18U;
    r_a1 = r_s2 + 0xacU;
    r_a0 = r_s1 + 0xacU;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 12U;
    r_a1 = r_s2 + 0xc0U;
    r_a0 = r_s1 + 0xc0U;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 36U;
    r_a1 = r_s2 + 0xccU;
    r_a0 = r_s1 + 0xccU;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 36U;
    r_a1 = r_s2 + 0xf0U;
    r_a0 = r_s1 + 0xf0U;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 36U;
    r_a1 = r_s2 + 0x114U;
    r_a0 = r_s1 + 0x114U;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 36U;
    r_a1 = r_s2 + 0x138U;
    r_a0 = r_s1 + 0x138U;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 36U;
    r_a1 = r_s2 + 0x15cU;
    r_a0 = r_s1 + 0x15cU;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 20U;
    r_a1 = r_s2 + 0x180U;
    r_a0 = r_s1 + 0x180U;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 36U;
    r_a1 = r_s2 + 0x194U;
    r_a0 = r_s1 + 0x194U;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 36U;
    r_a1 = r_s2 + 0x1b8U;
    r_a0 = r_s1 + 0x1b8U;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 36U;
    r_a1 = r_s2 + 0x1dcU;
    r_a0 = r_s1 + 0x1dcU;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 36U;
    r_a1 = r_s2 + 0x200U;
    r_a0 = r_s1 + 0x200U;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 36U;
    r_a1 = r_s2 + 0x224U;
    r_a0 = r_s1 + 0x224U;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 12U;
    r_a1 = r_s2 + 0x248U;
    r_a0 = r_s1 + 0x248U;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 40U;
    r_a1 = r_s2 + 0x4d4U;
    r_a0 = r_s1 + 0x4d4U;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_s6 = r_s1 + 0x254U;
    r_a2 = 40U;
    r_a1 = r_s2 + 0x4fcU;
    r_a0 = r_s1 + 0x4fcU;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_s5 = r_s1 + 0x278U;
    r_a2 = 36U;
    r_a1 = r_s2 + 0x254U;
    r_a0 = r_s6;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_s4 = r_s1 + 0x29cU;
    r_a2 = 36U;
    r_a1 = r_s2 + 0x278U;
    r_a0 = r_s5;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 36U;
    r_a1 = r_s2 + 0x29cU;
    r_a0 = r_s4;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 6U;
    r_a1 = r_s2 + 0x2c0U;
    r_a0 = r_s1 + 0x2c0U;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 6U;
    r_a1 = r_s2 + 0x2c6U;
    r_a0 = r_s1 + 0x2c6U;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 6U;
    r_a1 = r_s2 + 0x2ccU;
    r_a0 = r_s1 + 0x2ccU;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 18U;
    r_a1 = r_s2 + 0x2d2U;
    r_a0 = r_s1 + 0x2d2U;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 18U;
    r_a1 = r_s2 + 0x2e4U;
    r_a0 = r_s1 + 0x2e4U;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 12U;
    r_a1 = r_s2 + 0x2f8U;
    r_a0 = r_s1 + 0x2f8U;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 36U;
    r_a1 = r_s2 + 0x304U;
    r_a0 = r_s1 + 0x304U;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 36U;
    r_a1 = r_s2 + 0x328U;
    r_a0 = r_s1 + 0x328U;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 36U;
    r_a1 = r_s2 + 0x34cU;
    r_a0 = r_s1 + 0x34cU;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 36U;
    r_a1 = r_s2 + 0x370U;
    r_a0 = r_s1 + 0x370U;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 36U;
    r_a1 = r_s2 + 0x394U;
    r_a0 = r_s1 + 0x394U;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 20U;
    r_a1 = r_s2 + 0x3b8U;
    r_a0 = r_s1 + 0x3b8U;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 36U;
    r_a1 = r_s2 + 0x3ccU;
    r_a0 = r_s1 + 0x3ccU;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 36U;
    r_a1 = r_s2 + 0x3f0U;
    r_a0 = r_s1 + 0x3f0U;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 36U;
    r_a1 = r_s2 + 0x414U;
    r_a0 = r_s1 + 0x414U;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 36U;
    r_a1 = r_s2 + 0x438U;
    r_a0 = r_s1 + 0x438U;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 36U;
    r_a1 = r_s2 + 0x45cU;
    r_a0 = r_s1 + 0x45cU;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 12U;
    r_a1 = r_s2 + 0x480U;
    r_a0 = r_s1 + 0x480U;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a2 = 40U;
    r_a1 = r_s2 + 0x524U;
    r_a0 = r_s1 + 0x524U;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a0 = r_s1 + 0x54cU;
    r_a2 = 40U;
    r_a1 = r_s2 + 0x54cU;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_v0 = *(uint32_t *)(uintptr_t)(r_s3 + 140);
    r_a0 = *(uint8_t *)(uintptr_t)(r_v0 + 0);
    r_v0 = 1U;
    mips_cond = (r_a0 != r_v0) ? 1U : 0U;
    r_v1 = *(uint32_t *)(uintptr_t)(r_sp + 16);
    if (mips_cond) goto L_6815c;
    r_a1 = *(uint32_t *)(uintptr_t)(r_s3 + 136);
    r_a0 = r_v1;
    r_a2 = 4U;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a1 = *(uint32_t *)(uintptr_t)(r_s3 + 136);
    r_a2 = 4U;
    r_a0 = r_s8;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a1 = *(uint32_t *)(uintptr_t)(r_s3 + 136);
    r_a2 = 4U;
    r_a0 = r_s7;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a1 = *(uint32_t *)(uintptr_t)(r_s3 + 136);
    r_a2 = 4U;
    r_a0 = r_s6;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a1 = *(uint32_t *)(uintptr_t)(r_s3 + 136);
    r_a2 = 4U;
    r_a0 = r_s5;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a1 = *(uint32_t *)(uintptr_t)(r_s3 + 136);
    r_a2 = 4U;
    r_a0 = r_s4;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_v0 = *(uint32_t *)(uintptr_t)(r_s3 + 144);
    r_v0 = *(uint8_t *)(uintptr_t)(r_v0 + 0);
    mips_cond = (r_v0 != 0) ? 1U : 0U;
    r_ra = *(uint32_t *)(uintptr_t)(r_sp + 60);
    if (mips_cond) goto L_68160;
    *(uint32_t *)(uintptr_t)(r_s1 + 192) = (uint32_t)0;
    *(uint32_t *)(uintptr_t)(r_s1 + 760) = (uint32_t)0;
L_6815c:
    r_ra = *(uint32_t *)(uintptr_t)(r_sp + 60);
L_68160:
    r_s8 = *(uint32_t *)(uintptr_t)(r_sp + 56);
    r_s7 = *(uint32_t *)(uintptr_t)(r_sp + 52);
    r_s6 = *(uint32_t *)(uintptr_t)(r_sp + 48);
    r_s5 = *(uint32_t *)(uintptr_t)(r_sp + 44);
    r_s4 = *(uint32_t *)(uintptr_t)(r_sp + 40);
    r_s3 = *(uint32_t *)(uintptr_t)(r_sp + 36);
    r_s2 = *(uint32_t *)(uintptr_t)(r_sp + 32);
    r_s1 = *(uint32_t *)(uintptr_t)(r_sp + 28);
    r_s0 = *(uint32_t *)(uintptr_t)(r_sp + 24);
    r_v0 = 0;
    r_sp = r_sp + 0x40U;
    goto fn_exit;
L_68190:
    r_a1 = (uint32_t)(uintptr_t)&au16BT601FullOffsetYUVy; /* HI16 au16BT601FullOffsetYUVy */
    r_a2 = 4U;
    r_a1 = (uint32_t)(uintptr_t)&au16BT601FullOffsetYUVy;
    r_a0 = r_s1;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a1 = (uint32_t)(uintptr_t)&au16BT601FullClip0; /* HI16 au16BT601FullClip0 */
    r_a2 = 8U;
    r_a1 = (uint32_t)(uintptr_t)&au16BT601FullClip0;
    r_a0 = r_s8;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a1 = (uint32_t)(uintptr_t)&au16BT601FullClip1; /* HI16 au16BT601FullClip1 */
    r_a2 = 8U;
    r_a1 = (uint32_t)(uintptr_t)&au16BT601FullClip1;
    r_a0 = r_s7;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a1 = (uint32_t)(uintptr_t)&au16BT601FullClip2; /* HI16 au16BT601FullClip2 */
    r_a2 = 8U;
    r_a1 = (uint32_t)(uintptr_t)&au16BT601FullClip2;
    r_a0 = r_s6;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a1 = (uint32_t)(uintptr_t)&as32BT601FullMMatrix; /* HI16 as32BT601FullMMatrix */
    r_a2 = 36U;
    r_a1 = (uint32_t)(uintptr_t)&as32BT601FullMMatrix;
    r_a0 = r_s5;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a1 = (uint32_t)(uintptr_t)&as32BT601FullMinvMatrix; /* HI16 as32BT601FullMinvMatrix */
    r_a2 = 36U;
    r_a1 = (uint32_t)(uintptr_t)&as32BT601FullMinvMatrix;
    goto L_67dcc;
L_68204:
    r_a1 = (uint32_t)(uintptr_t)&au16BT709LimitedOffsetYUVy; /* HI16 au16BT709LimitedOffsetYUVy */
    r_a1 = (uint32_t)(uintptr_t)&au16BT709LimitedOffsetYUVy;
    r_a0 = r_s1;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a1 = (uint32_t)(uintptr_t)&au16BT709LimitedClip0; /* HI16 au16BT709LimitedClip0 */
    r_a2 = 8U;
    r_a1 = (uint32_t)(uintptr_t)&au16BT709LimitedClip0;
    r_a0 = r_s8;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a1 = (uint32_t)(uintptr_t)&au16BT709LimitedClip1; /* HI16 au16BT709LimitedClip1 */
    r_a2 = 8U;
    r_a1 = (uint32_t)(uintptr_t)&au16BT709LimitedClip1;
    r_a0 = r_s7;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a1 = (uint32_t)(uintptr_t)&au16BT709LimitedClip2; /* HI16 au16BT709LimitedClip2 */
    r_a2 = 8U;
    r_a1 = (uint32_t)(uintptr_t)&au16BT709LimitedClip2;
    r_a0 = r_s6;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a1 = (uint32_t)(uintptr_t)&as32BT709LimitedMMatrix; /* HI16 as32BT709LimitedMMatrix */
    r_a2 = 36U;
    r_a1 = (uint32_t)(uintptr_t)&as32BT709LimitedMMatrix;
    r_a0 = r_s5;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a1 = (uint32_t)(uintptr_t)&as32BT709LimitedMinvMatrix; /* HI16 as32BT709LimitedMinvMatrix */
    r_a2 = 36U;
    r_a1 = (uint32_t)(uintptr_t)&as32BT709LimitedMinvMatrix;
    goto L_67dcc;
L_68274:
    r_a1 = (uint32_t)(uintptr_t)&au16BT709FullOffsetYUVy; /* HI16 au16BT709FullOffsetYUVy */
    r_a1 = (uint32_t)(uintptr_t)&au16BT709FullOffsetYUVy;
    r_a0 = r_s1;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a1 = (uint32_t)(uintptr_t)&au16BT709FullClip0; /* HI16 au16BT709FullClip0 */
    r_a2 = 8U;
    r_a1 = (uint32_t)(uintptr_t)&au16BT709FullClip0;
    r_a0 = r_s8;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a1 = (uint32_t)(uintptr_t)&au16BT709FullClip1; /* HI16 au16BT709FullClip1 */
    r_a2 = 8U;
    r_a1 = (uint32_t)(uintptr_t)&au16BT709FullClip1;
    r_a0 = r_s7;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a1 = (uint32_t)(uintptr_t)&au16BT709FullClip2; /* HI16 au16BT709FullClip2 */
    r_a2 = 8U;
    r_a1 = (uint32_t)(uintptr_t)&au16BT709FullClip2;
    r_a0 = r_s6;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a1 = (uint32_t)(uintptr_t)&as32BT709FullMMatrix; /* HI16 as32BT709FullMMatrix */
    r_a2 = 36U;
    r_a1 = (uint32_t)(uintptr_t)&as32BT709FullMMatrix;
    r_a0 = r_s5;
    r_v0 = (uint32_t)REGCALL_memcpy(r_a0, r_a1, r_a2, r_a3);
    r_a1 = (uint32_t)(uintptr_t)&as32BT709FullMinvMatrix; /* HI16 as32BT709FullMinvMatrix */
    r_a2 = 36U;
    r_a1 = (uint32_t)(uintptr_t)&as32BT709FullMinvMatrix;
    goto L_67dcc;
fn_exit:
    return (int32_t)r_v0;
}


#undef REGCALL_system_reg_write
#undef REGCALL_memcpy
#undef REGCALL_fix_point_mult2_32
#undef REGCALL_tisp_bcsh_reg2para
#undef REGCALL_tisp_bcsh_para2reg
#undef REGCALL_tisp_bcsh_Toffset_RGBYUV
#undef REGCALL_tisp_bcsh_Toffset_RGB2YUV
#undef REGCALL_tisp_bcsh_Tccm_Comp2Orig
#undef REGCALL_tisp_bcsh_Tccm_RGBYUV
#undef REGCALL_tisp_bcsh_Tccm_RGB2YUV
#undef REGCALL_tisp_bcsh_ct_interpolation
#undef REGCALL_tisp_bcsh_ct_set_parameter
#undef REGCALL_tisp_bcsh_ev_update_TransitParam
#undef REGCALL_tisp_bcsh_ev_set_parameter
#undef REGCALL_tisp_bcsh_ev_update
#undef REGCALL_tisp_bcsh_ct_update
#undef REGCALL_tisp_bcsh_StrenCal
#undef REGCALL_tisp_bcsh_StrenCal_part_0

static int32_t regtrace_bcsh_main_init_lit(uint32_t mode)
{
    void *outer;
    uint32_t nbuf = *(uint32_t *)((char *)&tparamsN);
    uint8_t *modep;
    uint8_t *flag;

    if (!nbuf)
        return -ENOENT;
    if (!stMainBcshOuter_lit) {
        outer = (void *)(uintptr_t)
            ((uintptr_t (*)(uintptr_t))(uintptr_t)private_vmalloc)(1396);
        if (!outer)
            return -ENOMEM;
        memset(outer, 0, 4);
        stMainBcshOuter_lit = (uintptr_t)outer;
    }

    (void)tisp_bcsh_params_transmit_lit((uint32_t)(uintptr_t)pstMainBcshOri_lit,
                                        (uint32_t)stMainBcshOuter_lit,
                                        (uint32_t)(uintptr_t)stMainBcshInterOri_lit);
    modep = (uint8_t *)(uintptr_t)pstMainBcshOri_lit[27];
    if (modep)
        *modep = (uint8_t)mode;
    (void)tisp_bcsh_params_refresh_lit((uint32_t)(uintptr_t)pstMainBcshOri_lit,
                                       (uint32_t)stMainBcshOuter_lit,
                                       nbuf + 0x11e38U);
    (void)tisp_bcsh_ev_update_lit((uint32_t)(uintptr_t)pstMainBcshOri_lit, 0);
    (void)tisp_bcsh_ct_update_lit((uint32_t)(uintptr_t)pstMainBcshOri_lit, 0);
    flag = (uint8_t *)(uintptr_t)pstMainBcshOri_lit[6];
    if (flag)
        *flag = 0;
    printk(KERN_WARNING "tx_isp_t40_recovered: bcsh-main-init-lit done outer=%p mode=%u\n",
           (void *)(uintptr_t)stMainBcshOuter_lit, mode);
    return 0;
}
