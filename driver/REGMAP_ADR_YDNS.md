# TX-ISP T31 Register Map Sheet — ADR/DRC and YDNS (BN HLIL parity)

This sheet summarizes the relevant register windows, packing and sources for ADR/DRC and YDNS based on Binary Ninja HLIL of `tx-isp-t31.ko`. Use it to verify write counts, packing, and parity across modes.

## ADR/DRC (Adaptive Dynamic Range / DRC)

- Programming ranges (absolute addresses used by system_reg_write):
  - CTRL: 0x4004..0x4068 (per‑module control params)
  - KNEE: 0x406c..0x4080 (map/CTC kneepoints header)
  - LUT:  0x4084..0x4290 (main window; HLIL loop writes [start..end), end=0x4294 exclusive)
  - EXTRA:0x4294..0x433c (additional params)
  - CTC:  0x4340..0x4458 (CTC/COC kneepoints and tails)

- Packing:
  - 32-bit word = (high 16 bits << 16) | (low 16 bits)
  - HLIL pattern (adr_set_params): `system_reg_write(i, *(p+4) << 16 | *p)`

- Bank switching:
  - `tisp_adr_wdr_en(enable)` swaps all ADR "now" pointers between LIN/WDR banks and calls:
    - `tiziano_adr_params_refresh()`
    - `tiziano_adr_params_init()`

- Strength path:
  - `tisp_s_drc_strength` → `tisp_s_adr_str_internal(strength)`
  - Behavior: pivot ~0x80, clamps observed around 0x190/0x1F4/0x258; sets `ev_changed=1` and re‑inits ADR

- Expected word counts (inclusive ranges):
  - CTRL words = ((0x4068-0x4004)/4)+1
  - KNEE words = ((0x4080-0x406c)/4)+1
  - LUT  words = ((0x4294-0x4084)/4)    ⇒ 132 writes (end exclusive)
  - EXTRAwords = ((0x433c-0x4294)/4)+1
  - CTC  words = ((0x4458-0x4340)/4)+1


- ADR LUT window composition (HLIL-style, 16-bit lanes concatenated then globally 16:16-packed):
  1) adr_map_mode_now: first 6 ints (WDR-banked)
  2) Weights: param_adr_weight_20/02/12/22/21, 32 each (total 160 ints)
  3) mapb1..mapb4: 9 each (36 ints, WDR-banked)
  4) ctc_map2cut_y: 9 ints (WDR-banked)
  5) light_end: 29 ints (WDR-banked)
  6) block_light: 15 ints (WDR-banked)
  7) blp2_list: 9 ints (WDR-banked)
  = 6 + 160 + 36 + 9 + 29 + 15 + 9 = 264 lanes = 132 words

- Primary source arrays (exposed via GET/SET param IDs, see tx_isp_tuning.c):
  - 0x380..0x3AB block: `param_adr_para_array`, kneepoint arrays, mapb[1..4], ctc/coc, EV/LIGB lists, gamma X/Y, tool control, etc.

- Per-frame/ISR:
  - `tiziano_adr_interrupt_static` → `tisp_adr_set_params`

## YDNS (Luma Denoise)

- Programming range:
  - YDNS regs: 0x7af0..0x7afc

- Packing and cross-deps:
  - `tisp_ydns_param_cfg` packs fields with 16:16 words. Some fields are derived from SDNS arrays:
    - `sdns_h_s_9/10_array`, `sdns_mv_num_thr_7x7_array`, etc.
  - Ensure SDNS arrays are refreshed before YDNS param pack runs during DN or gain updates.

- Refresh paths:
  - `tisp_ydns_par_refresh` checks gain delta ≥ 0x100 or first-run (0xffffffff) then calls `tisp_ydns_intp_reg_refresh`
  - `tisp_ydns_all_reg_refresh` calls intp then param_cfg

## WDR Toggle Ordering (Global)

- `tisp_s_wdr_en` calls module hooks in this order (per HLIL):
  1) DPC, LSC, Gamma, Sharpen, CCM, BCSH, RDNS
  2) ADR, Defog, MDNS, DMSC, AE, SDNS
  3) CLM init, YDNS init

## Day/Night (DN) Refresh Ordering

- `tisp_day_or_night_s_ctrl` / `tisp_cust_mode_s_ctrl` refresh in this order:
  - Defog, AE, AWB → DMSC, Sharpen, MDNS, SDNS → GIB, LSC, CCM, CLM → Gamma, ADR, DPC, AF, BCSH → RDNS, YDNS

## Quick parity checklist

- [ ] LUT write counts match ranges above (ADR_LUT has 132 writes)
- [ ] All ADR banked arrays have both LIN and WDR sets; WDR toggle re-seats "now" pointers
- [ ] Strength blending pivot at 0x80; clamps respected; `ev_changed` set and ISR path active
- [ ] YDNS pack uses current SDNS arrays; sequencing prevents stale reads
- [ ] Per-frame ISR apply order verified against HLIL for your platform

Notes:
- A convenience header `include/tx_isp_regmap.h` defines these address ranges and helper PACK16.
- `tiziano_adr_init()` now logs the ADR/YDNS window counts at init as a parity aid.

