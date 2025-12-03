# CSI Bring‑up and Lane Config Sequencing (Working Branch → How-To)

This document preserves the exact bring‑up sequence and verification checkpoints that make the CSI lane configuration “come alive” (i.e., BASIC 0x200–0x2F4 becomes non‑zero), so we can replicate it when switching back to the other branch.

## TL;DR

- Lane config lives in CSI BASIC (0x10022000), not the ISP wrapper.
- Maintain stream ordering: ON = VIC → CSI → Sensor; OFF = Sensor → CSI → VIC.
- In CSI init (MIPI): set lanes‑1 at 0x04; stage 0x08/0x0C/0x10 with small sleeps.
- Do not hard‑code trace values. Ensure the reference init path programs BASIC 0x200–0x2F4 and sets BASIC 0x128 as a consequence of correct sequencing.
- w01[0x14] (0x10023000) toggles (~0x630 → ~0x30 → ~0x0) across phases.
- Validate with /opt/trace.txt and /opt/csi.txt snapshots (reads only).

## Address Map (relevant)

- CPM base: 0x10000000 (CLKGR0 @0x20, CLKGR1 @0x28)
- CSI BASIC: 0x10022000
  - Key regs: 0x00, 0x04, 0x08, 0x0C, 0x10, 0x14, 0x20, 0x24, 0x40
  - Lane enable mask: 0x128
  - Lane Config region: 0x200–0x2F4 (e.g., 0x200, 0x204, 0x210, 0x230, 0x250, 0x254, 0x2F4)
- VIC/“w01” control: 0x10023000 (notably 0x14, 0x40)
- ISP wrapper: 0x133e6600 (not used for lane config)

## Ground Truth From Working Branch (observed)

- CSI BASIC reads/writes succeed; WRAP reads are zero and are not used for lane config.
- Lane Config region (BASIC 0x200–0x2F4) receives non‑zero values during bring‑up.
- Lane enable mask at BASIC 0x128 becomes 0x3f during streaming in the captured run (adjust for your lane count).
- w01[0x14] transitions across phases: ~0x630 (pre), ~0x30 (streaming), ~0x0 (off). Mid‑cycle values like 0x430/0x200 can appear briefly.
- CPM values are stable; heavy CPM manipulation is unnecessary in the working sequence.

## Bring‑up Recipe (to port)

1) Preconditions
- Sensor reports MIPI (`dbus_type = 1`) and correct lane count (`sensor_attr->mipi.lans`).
- BASIC CSI regs mapped at 0x10022000. Do not read/write lane config via WRAP.

2) Stream ON ordering
- Strictly: VIC.s_stream(1) → CSI.s_stream(1) → Sensor.s_stream(1).

3) CSI core init for MIPI (csi_core_ops_init(sd, 1))
- Program lanes‑1 at BASIC 0x04.
- Stage control bits with small sleeps (matches working behavior):
  - Clear bit0 of BASIC 0x08
  - Write 0 to BASIC 0x0C
  - sleep(1 ms)
  - Clear bit0 of BASIC 0x10
  - sleep(1 ms)
  - Write interface_type (1) to BASIC 0x0C
  - sleep(1 ms)

4) Lane Config programming (VERIFY, don’t replicate)
- Do not write the lane-config block by copying trace values. These are hardware state outcomes.
- Ensure your MIPI init path (as decompiled in BN/Ghidra) programs BASIC 0x200–0x2F4 as part of correct sequencing.
- Use the following offsets as verification anchors in traces/dumps (non-zero expected):
  - 0x200, 0x204, 0x210, 0x230, 0x250/0x254, 0x2F4.

5) Lane enable mask
- Set BASIC 0x128 to the expected mask for your lane count:
  - Example masks: 1‑lane 0x31, 2‑lane 0x33, 4‑lane 0x3f (working branch shows 0x3f).

6) w01 coordination (0x10023000)
- Ensure w01[0x14] follows the same phases as the working branch:
  - Pre‑stream ≈ 0x630; stream‑on ≈ 0x30; off ≈ 0x0 (intermediate 0x430/0x200 may occur).
- Keep these writes on primary mapping; do not write to the secondary space (that broke IRQs previously).

7) Stream ON completion (expected readings)
- BASIC 0x00 becomes non‑zero (e.g., 0x7d seen).
- BASIC 0x10 = 0x01.
- BASIC 0x128 = lane mask (e.g., 0x3f).
- BASIC LANEC: 0x250/0x254 reflect active values (e.g., 0x60/0x12 → 0x60/0x22).
- Lane Config region 0x200–0x2F4 is populated with non‑zeros.
- w01[0x14] ≈ 0x30 during stream.

8) Stream OFF ordering
- Sensor.s_stream(0) → CSI.s_stream(0) → VIC.s_stream(0).
- w01[0x14] settles to off state (~0x0).
- BASIC 0x250 returns to 0x00; 0x128 may remain at mask value — acceptable.

## Validation Checklist

Use both /opt/trace.txt and /opt/csi.txt.

- BEFORE csi_core_ops_init:
  - BASIC 0x04/0x0C show pre‑values; BASIC 0x128 often 0x0f before stream.
  - Lane Config region already shows non‑zeros at key offsets (0x200, 0x204, 0x210, 0x230, 0x2F4) in the working branch — OK.
  - w01[0x14] ≈ 0x630.
- AFTER basic CSI init (post interface_type write):
  - BASIC 0x04 = lanes‑1; BASIC 0x0C = 0x1; staged transitions succeed.
  - BASIC 0x128 unchanged pre‑stream (often 0x0f).
- AFTER s_stream enable=1:
  - BASIC 0x00 ≠ 0; BASIC 0x10 = 0x01.
  - BASIC 0x128 = lane mask (e.g., 0x3f).
  - LANEC: 0x250/0x254 = 0x60/0x12 or 0x22.
  - w01[0x14] ≈ 0x30.
- /opt/trace.txt should show a block of “[CSI Lane Config] write … 0x200–0x2F4”.

If Lane Config remains zero:
- Confirm reads target BASIC (0x10022000), not WRAP (0x133e6600).
- Ensure the reference init path that programs 0x200–0x2F4 is actually executing (per BN/Ghidra). Do not write trace values directly.
- Ensure BASIC 0x128 is set to your expected mask.

## Pitfalls (what not to do)

- Writing lane config to the ISP wrapper: lane config lives in BASIC; WRAP reads will be zero and misleading.
- Moving w01 writes to secondary space: this previously broke interrupts (IRQs stalled at 0). Keep on primary mapping.
- Changing stream ordering: keep ON = VIC → CSI → Sensor; OFF = Sensor → CSI → VIC.
- Over‑complicating CPM: JIT ungate is optional; working branch does not require heavy CPM manipulation.

## Minimal Code Anchors (pseudo)

CSI BASIC staging (order and sleeps matter):

```c
// lanes-1
writel(lanes - 1, csi + 0x04);
// stage/reset bits
writel(readl(csi + 0x08) & ~1u, csi + 0x08);
writel(0, csi + 0x0C); msleep(1);
writel(readl(csi + 0x10) & ~1u, csi + 0x10); msleep(1);
// interface_type = 1 (MIPI)
writel(1, csi + 0x0C); msleep(1);
```

Lane mask:

```c
// adjust for your lane count; working case used 0x3f
writel(0x3f, csi + 0x128);
```

Verification anchors (reads only):
- Expect non-zero values at BASIC 0x200, 0x204, 0x210, 0x230, 0x250/0x254 and 0x2F4 when sequencing is correct.
- Use /opt/trace.txt and /opt/csi.txt to confirm; do not write these offsets directly.

w01 coordination (observed pattern):
- Pre-stream 0x14  ~0x630; stream-on ~0x30; off ~0x0.

## When Switching Back to the Other Branch (Checklist)

- [ ] Keep stream ordering (ON: VIC→CSI→Sensor; OFF: Sensor→CSI→VIC).
- [ ] Implement CSI BASIC staging (0x04, 0x08, 0x0C, 0x10 with sleeps).
- [ ] Ensure the reference init path programs BASIC 0x200–0x2F4 (don’t hard‑code trace values).
- [ ] Set BASIC 0x128 to correct lane mask for your configuration.
- [ ] Ensure w01[0x14] follows the same pre/on/off values as working branch.
- [ ] Validate via /opt/trace.txt (“CSI Lane Config” writes) and /opt/csi.txt snapshots at key tags.

## What Success Looks Like

- /opt/csi.txt “AFTER s_stream enable=1” shows:
  - BASIC 0x00 ≠ 0 and 0x10 = 0x01
  - BASIC 0x128 = your lane mask (e.g., 0x3f)
  - Lane Config (0x200–0x2F4) non‑zero with expected anchors
  - w01[0x14] ≈ 0x30
- /opt/trace.txt contains a contiguous block of “[CSI Lane Config] write …” covering 0x200–0x2F4 (plus 0x250/0x254 updates).

## Instrumentation and Progress Reset

- To get periodic read-only snapshots to /opt/csi.txt, load the module with: csi_dump_enable=1.
- This branch also emits a single snapshot after s_stream(1) to ensure /opt/csi.txt is created even if the thread is disabled.
- To reset progress: rm -f /opt/csi.txt before a new run so the next snapshot starts a fresh log.

