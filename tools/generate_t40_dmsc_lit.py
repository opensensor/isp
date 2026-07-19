#!/usr/bin/env python3
"""Generate the self-contained T40 DMSC literal include from OEM objdump."""

from __future__ import annotations

import argparse
import subprocess
from pathlib import Path


FUNCTIONS = (
    ("tisp_dmsc_reg_trig", "tisp_dmsc_reg_trig_lit", ("a0",)),
    ("tisp_dmsc_intp", "tisp_dmsc_intp_lit", ("a0", "a1")),
    ("tisp_dmsc_wdr_en", "tisp_dmsc_wdr_en_lit", ("a0", "a1")),
    ("tisp_dmsc_noref_reg_cfg", "tisp_dmsc_noref_reg_cfg_lit", ("a0",)),
    ("tisp_dmsc_ref_reg_cfg", "tisp_dmsc_ref_reg_cfg_lit", ("a0",)),
)


def translated(tool: Path, asm: Path, original: str, bssmap: Path) -> str:
    return subprocess.check_output(
        ["python3", str(tool), str(asm), original, str(bssmap)], text=True
    ).rstrip()


def wrapper(name: str, args: tuple[str, ...], body: str) -> str:
    signature = ", ".join(f"uint32_t {arg}_in" for arg in args)
    inputs = {arg: f"{arg}_in" for arg in args}
    reg_lines = [
        "    uint32_t r_at = 0, r_v0 = 0, r_v1 = 0;",
        "    uint32_t r_a0 = %s, r_a1 = %s, r_a2 = 0, r_a3 = 0;"
        % (inputs.get("a0", "0"), inputs.get("a1", "0")),
        "    uint32_t r_t0 = 0, r_t1 = 0, r_t2 = 0, r_t3 = 0;",
        "    uint32_t r_t4 = 0, r_t5 = 0, r_t6 = 0, r_t7 = 0;",
        "    uint32_t r_t8 = 0, r_t9 = 0;",
        "    uint32_t r_s0 = 0, r_s1 = 0, r_s2 = 0, r_s3 = 0;",
        "    uint32_t r_s4 = 0, r_s5 = 0, r_s6 = 0, r_s7 = 0, r_s8 = 0;",
        "    uint32_t r_k0 = 0, r_k1 = 0, r_gp = 0, r_ra = 0;",
        "    uint32_t mips_lo = 0, mips_hi = 0, mips_cond = 0;",
        "    uint8_t mips_stack[512] __attribute__((aligned(8)));",
        "    uint32_t r_sp = (uint32_t)(uintptr_t)(mips_stack + sizeof(mips_stack));",
        "    (void)r_at; (void)r_t8; (void)r_k0; (void)r_k1; (void)r_gp;",
        "    (void)r_s8; (void)mips_lo; (void)mips_hi; (void)mips_cond;",
    ]
    return "\n".join(
        [f"static int32_t {name}({signature})", "{", *reg_lines, body,
         "fn_exit:", "    return (int32_t)r_v0;", "}"]
    )


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--asm", type=Path, required=True)
    parser.add_argument("--bssmap", type=Path, required=True)
    parser.add_argument(
        "--translator", type=Path, default=Path(__file__).with_name("mips2c_literal.py")
    )
    args = parser.parse_args()

    parts = [
        """/*
 * Generated T40 DMSC literal chain from OEM tx-isp-t40.ko.
 * Do not hand-edit the instruction bodies; regenerate with
 * tools/generate_t40_dmsc_lit.py.
 */
static bool regtrace_enable_dmsc_lit;
static uint regtrace_dmsc_lit_gain = 212541U;
static uint32_t regtrace_dmsc_lit_gain_now;
module_param_named(enable_dmsc_lit, regtrace_enable_dmsc_lit, bool, 0644);
module_param_named(dmsc_lit_gain, regtrace_dmsc_lit_gain, uint, 0644);
module_param_named(dmsc_lit_gain_now, regtrace_dmsc_lit_gain_now, uint, 0444);

static uintptr_t regtrace_main_dmsc_lit;
static uintptr_t regtrace_main_dmsc_comb_lit;
static uintptr_t regtrace_main_dmsc_intp_lit;
static uintptr_t regtrace_sec_dmsc_lit;
static uintptr_t regtrace_sec_dmsc_comb_lit;
static uintptr_t regtrace_sec_dmsc_intp_lit;
static uint32_t regtrace_dmsc_wdr_flags_lit[2];

#undef REGCALL_system_reg_write
#undef REGCALL_tisp_simple_intp_int8
#undef REGCALL_tisp_simple_intp_int16
#define REGCALL_system_reg_write(a, b, c, d) system_reg_write((a), (b))
#define REGCALL_tisp_simple_intp_int8(a, b, c, d) \
    tisp_simple_intp_int8((int32_t)(a), (int32_t)(b), (void *)(uintptr_t)(c))
#define REGCALL_tisp_simple_intp_int16(a, b, c, d) \
    tisp_simple_intp_int16((int32_t)(a), (int32_t)(b), (void *)(uintptr_t)(c))
"""
    ]
    for original, name, fn_args in FUNCTIONS:
        body = translated(args.translator, args.asm, original, args.bssmap)
        parts.append(wrapper(name, fn_args, body))

    parts.append(
        """
static int32_t tisp_dmsc_all_reg_refresh_lit(uint32_t ch, uint32_t gain)
{
    (void)tisp_dmsc_intp_lit(ch, gain);
    (void)tisp_dmsc_noref_reg_cfg_lit(ch);
    (void)tisp_dmsc_ref_reg_cfg_lit(ch);
    return tisp_dmsc_reg_trig_lit(ch);
}

static int32_t regtrace_dmsc_main_init_lit(void)
{
    uint32_t nbuf = *(uint32_t *)((char *)&tparamsN);

    if (!nbuf)
        return -ENOENT;
    if (!regtrace_main_dmsc_lit)
        regtrace_main_dmsc_lit = (uintptr_t)vzalloc(6074U);
    if (!regtrace_main_dmsc_comb_lit)
        regtrace_main_dmsc_comb_lit = (uintptr_t)vzalloc(932U);
    if (!regtrace_main_dmsc_intp_lit)
        regtrace_main_dmsc_intp_lit = (uintptr_t)vzalloc(280U);
    if (!regtrace_main_dmsc_lit || !regtrace_main_dmsc_comb_lit ||
        !regtrace_main_dmsc_intp_lit)
        return -ENOMEM;

    memcpy((void *)(uintptr_t)regtrace_main_dmsc_lit,
           (const void *)(uintptr_t)(nbuf + 60780U), 6074U);
    memset((void *)(uintptr_t)regtrace_main_dmsc_comb_lit, 0, 932U);
    memset((void *)(uintptr_t)regtrace_main_dmsc_intp_lit, 0, 280U);
    (void)tisp_dmsc_wdr_en_lit(0, regtrace_dmsc_wdr_flags_lit[0]);
    (void)tisp_dmsc_all_reg_refresh_lit(0, regtrace_dmsc_lit_gain);
    regtrace_dmsc_lit_gain_now = regtrace_dmsc_lit_gain;
    printk(KERN_WARNING
           "tx_isp_t40_recovered: dmsc-main-init-lit gain=%u par=%p comb=%p intp=%p\\n",
           regtrace_dmsc_lit_gain,
           (void *)(uintptr_t)regtrace_main_dmsc_lit,
           (void *)(uintptr_t)regtrace_main_dmsc_comb_lit,
           (void *)(uintptr_t)regtrace_main_dmsc_intp_lit);
    return 0;
}

static void regtrace_dmsc_lit_gain_update(uint32_t gain)
{
    if (!regtrace_main_dmsc_lit || gain == regtrace_dmsc_lit_gain_now)
        return;
    (void)tisp_dmsc_all_reg_refresh_lit(0, gain);
    regtrace_dmsc_lit_gain_now = gain;
}

static void regtrace_dmsc_lit_cleanup(void)
{
    vfree((void *)(uintptr_t)regtrace_main_dmsc_intp_lit);
    vfree((void *)(uintptr_t)regtrace_main_dmsc_comb_lit);
    vfree((void *)(uintptr_t)regtrace_main_dmsc_lit);
    regtrace_main_dmsc_intp_lit = 0;
    regtrace_main_dmsc_comb_lit = 0;
    regtrace_main_dmsc_lit = 0;
}

#undef REGCALL_system_reg_write
#undef REGCALL_tisp_simple_intp_int8
#undef REGCALL_tisp_simple_intp_int16
"""
    )
    print("\n\n".join(parts))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
