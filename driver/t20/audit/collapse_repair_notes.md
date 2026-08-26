# T20 integration and collapse repair notes

Date: 2026-08-26

## Build result

- `driver/t20/tx-isp-t20.ko` clean-builds and links against the Thingino T20
  Linux 3.10.14 tree used by `wyze_cam2_t20x_jxf23_rtl8189ftv`.
- MODPOST completes without warnings.
- Module ABI: ELF32 little-endian MIPS, o32, MIPS32 R1.
- Vermagic: `3.10.14 preempt mod_unload MIPS32_R1 32BIT`.
- This is a static integration milestone, not a runtime-parity claim.

## Upstream and common split

- 27 source files are compiled from the tracked Ingenic T20 SDK.
- 477 archive-backed firmware functions are compiled from the T20 recovery.
- The sensor registry is shared through `driver/common/tx_isp_sinfo.c` with
  offsets taken from the T20 `tx-isp-sinfo.o` instruction stream.
- Five fixed-point functions with identical normalized T20/T30 OEM assembly
  use the shared math implementation: `line_offset`,
  `log2_fixed_to_fixed`, `multiplication_fixed_to_fixed`,
  `solving_lin_equation_b`, and `solving_nth_root_045`.

## One-off repairs

- Restored the exact SDK implementation of `register_tx_isp_vic_device`; its
  isolated normalized assembly match was 416/416 instructions.
- Repaired decompiler type/scope failures in fixed-point lookup, firmware
  state, AWB range interpolation, color-matrix conversion, SBUS copy,
  sharpening calibration, and DIS/GMV address arithmetic.
- Recovered the missing `cmos_fsm_process_interrupt` logic. The generated
  candidate contained hundreds of declarations and no executable body, so the
  aggregate compiler reduced it to 13 instructions. The repaired ISR is 640
  instructions versus 366 in OEM at a different optimization level.

## Storage collapse

Generated absolute addresses around OEM `__fw` base `0x46700` became thirteen
independent 16 KiB arrays. They are now typed offsets into the single
12,240-byte `__fw` object, matching the already-reviewed T30 layout. Generated
aliases around OEM `stab` base `0x428f0` are now byte/halfword fields in the
single 60-byte `stab` object. Unreferenced 16 KiB placeholders were removed,
and exposure-partition reads now target the actual table.

| Metric | Before collapse | After collapse | OEM |
|---|---:|---:|---:|
| Module BSS | 529,696 | 38,128 | 34,016 |
| BSS delta from OEM | +495,680 | +4,112 | 0 |

## Binary audit

The checked-in replacement map groups SDK dispatcher wrappers with helpers
that the OEM compiler inlined. After that accounting:

- OEM functions: 742
- recovered functions: 860
- matched functions: 721
- direct matches: 711
- documented replacement matches: 10
- literal stubs: 0
- collapsed findings: 0

The audit remains structural. It cannot prove sensor binding, interrupt
cadence, DMA correctness, image stability, or unload safety.

## Regression result

The T21 and T30 modules both rebuilt successfully after the shared T20
integration. T21 retains its pre-existing generated-source warnings; neither
regression build introduced a link or MODPOST failure.

## Hardware gate

Do not install this module persistently. The first device test must preserve
the OEM module, use a one-boot replacement, capture boot/bind/stream logs and a
bounded frame sample, and reboot back to stock after the test.
