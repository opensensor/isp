# Ingenic T40 ISP Recovery

This directory is the T40 bring-up workspace for matching the OEM
`tx-isp-t40.ko` behavior on Linux 4.4.94 targets.

The initial `tx_isp_t40_recovered.c` file is a recovered whole-driver seed plus
the local parity fixes from the Wyze Cam 3 Pro T40XP/GC4653 smoke-test loop.
Hardware sequencing and tuning remain local, while reviewed object-layout
adapters now let T40 consume the same small shared-library contracts as the
other open drivers.

Build from a compatible 4.4.94 kernel tree with:

```sh
make -C <kernel-src> M=$(pwd)/driver/t40 modules
```

Expected artifact:

- `driver/t40/tx-isp-t40.ko`

## Current organization

- `tx_isp_t40_recovered.c` owns the recovered pipeline, device lifecycle,
  hardware programming, diagnostics, and T40 tuning policy.
- `tx_isp_t40_sinfo.c` adapts the T40 sensor object layout to the common typed
  registry.
- `tx_isp_t40_subdev.c` adapts the extended T40 graph and subdevice layout to
  the common graph resolver, remote-event resolver, readiness policy, and
  pad-link operations.

The T40 adapter retains generation-local pointer validation and the recovered
queue/state offsets (`0x1fc` and `0x218`). Remote-event filtering, diagnostics,
and the local frame-done fallback also remain in the recovered driver.

The first shared-subdevice device cycle ran on the T40XP/GC4653 camera at
`.144` on 2026-07-31. The open module registered one sensor, accepted forced
day mode, kept IRQ 38/39 active, and supplied a valid 1920x1080 H.264 RTSP
stream; FFmpeg decoded eight frames without an error and saved a fresh JPEG.
The module stayed resident with all 326 recovery parameters exposed and no
kernel-fatal signature during the sustained check.

Do not use an unbounded full read of `/proc/tx_isp_t40_recovered` as a health
probe. That legacy diagnostic path remains unsafe and caused the one-shot
test boot to fall back to stock. Use the bounded T40 probe tooling and the
specific `/proc/jz/sensor` nodes instead. The fail-safe boot loader now keeps
its early dmesg and kmsg captures in its persistent state directory so this
class of watchdog fallback retains evidence.

## Validated parity repairs

The 2026-07-18 parity batch replaced three unconditional kernel-build stubs
with behavior checked against the T40 OEM disassembly:

- `fix_point_div_32`: unsigned integer and fractional Q-format division
- `tisp_round_int64`: signed 64-bit right-shift with half-up rounding
- `defog_wei_interpcot`: 32-bin accumulation, rounded averaging, clamping,
  and monotonic output
- `defog_count_weight35abc`: preserve the OEM reducer count across the final
  call's MIPS delay slot and restore cleanup of all 13 temporary buffers

The batch builds against the Thingino T40XP/GC4653 Linux 4.4.94 tree. A smoke
run on the `.242` test camera completed block initialization, produced a valid
1920x1080 RTSP frame, kept Raptor and both ISP interrupt lines active, and
returned the camera to a clean boot. Evidence is in
`logs/20260718-t40-parity-batch1-final2-smoke/`.

The final OEM/recovered assembly audit reports 32 hard stubs, down from 35
before this batch, with no increase in collapsed functions (18).

## Forensic image-quality checkpoint

The 2026-07-18 GC4653 comparison loop reached the retained "good enough"
checkpoint on the `.242` test camera.  The stock oracle is
`logs/20260718-t40-forensic-stock-2105/`; the most useful recovered-driver
evidence sets are:

- `logs/20260718-t40-v42-lsc-active-exposure-trim-settled/`: best stored
  full-frame score and the retained stable visual baseline
- `logs/20260718-t40-v52-ysp-lsc-gain-settled/`: independent YSP/DNS gain
  control with the fixed YSP register groups matching stock at gain 229376
- `logs/20260718-t40-v53-dmsc-literal-gain212541/`: complete literal DMSC
  smoke, state/core capture, frame comparison, and live gain sweep

The retained implementation includes the exact LSC mesh, fixed CLM topology,
live CCM/BCSH controls, independent `ysp_gain_ev`, and a self-contained
source-derived DMSC chain.  The DMSC literal path remains opt-in through
`enable_dmsc_lit=1`: its complete consumer/evidence cycle was run at the stock
event gain of 212541.  A disposable live sweep found that gain 229376 was the
closest match for the primary stock DMSC register groups, but it was not
promoted to the default because it did not receive a separate full consumer
cycle.  The sweep was discarded with a clean reboot.

Exact source-derived BLC/GIB paths are also retained behind
`enable_blc_lit`/`enable_gib_lit`, but are deliberately disabled in the visual
profile.  The OEM absolute GIB calibration values applied to the recovered raw
domain produced a nearly black frame, showing that the missing prerequisite is
an upstream raw-domain calibration rather than another approximation of those
two functions.

The date/time OSD in the upper-left corner is outside the ISP comparison.  It
is rendered by the userspace overlay using the camera's system clock, so any
remaining offset or formatting issue should be fixed in the Thingino
time/NTP/timezone and OSD configuration without changing this driver profile.
