# T40 Tuning Hurdles Study

Date: 2026-06-08

Target device: `192.168.50.242` only.

## Baseline

Current data-path checkpoint:

- Commit: `70e58c2f Restore T40 RTSP real frame data`
- Evidence run: `logs/20260608-130100-t40-profile-defaults-retest-242`
- Representative frame: `frame-90s.jpg`
- Result: real scene geometry is visible over RTSP, but color is badly false
  colored and not a lifelike image.

The current failure is no longer the earlier flat-green/no-image failure. Both
interrupt lines are active, VIC MDMA qbuf rearm is running, and the RTSP
pipeline carries real sensor content. Treat the next problem as ISP color-path
bring-up, not stream bring-up.

## What Is Proven

- CSI/VIC/MSCA/RTSP are capable of carrying changing sensor data.
- The current no-direct T40 profile needs the VIC MDMA qbuf ring and IRQ rearm
  path to keep buffers moving.
- Frame delivery is gated on active-buffer/content checks so stale or empty
  buffers are less likely to masquerade as success.
- The T40 tuning blob loads from `/usr/share/sensor/gc4653-t40.bin` and is
  204800 bytes.
- The T40 tuning bank size in this blob is `0x18ff4`; do not reuse T31
  `0x137f0` offsets without proving the T40 layout.

## Current T40 Image-Path Risk

The working bring-up profile enables several image-path pieces because they
were needed to get a live stream far enough to test:

- `enable_tisp_main_init_scalar_regs`
- `enable_tisp_main_init_csc_version`
- `enable_tisp_main_init_tiziano`
- `enable_tisp_stream_regs_after_video`
- `enable_core_bayer_reg8`
- `enable_core_top_sel_reg0c`
- `enable_tisp_top_regs_final_reapply`
- profile default `tisp_main_init_top40_value = 0x7fd9004f`

Those are now suspects. They may be partly correct, partly stale, or too broad
for a minimal valid-color baseline. The next experiments should hold the
streaming machinery constant and vary only ISP color/tuning state.

Active-stream register reads are also a risk. An always-on proc snapshot of
the DMSC/BCSH/GIB windows wedged the camera while Raptor was running and
forced a recovery reboot. The driver now exposes this only through the
default-off `enable_t40_color_reg_snapshot` parameter. Do not enable it in the
normal RTSP profile until the individual register windows are isolated.

## 2026-06-08 Live Color Experiments

All runs below kept the no-direct RTSP/IRQ path fixed on `192.168.50.242` and
kept `enable_t40_color_reg_snapshot=0`.

Top-bypass probes:

- `0x7fd9006f` set the suspected GIB bypass bit. The proc evidence showed
  `top40_param=0x7fd9006f` and live `r40=0x7fd9006f`; the RTSP frame remained
  false-colored.
- `0x7fd9007f` added the adjacent LSC-style bypass bit. The proc evidence
  showed `top40_param=0x7fd9007f` and live `r40=0x7fd9007f`; the RTSP frame
  remained false-colored.

CFA/Bayer probes:

- The descriptor path was overriding `core_bayer_reg8_value`, so the driver now
  has `force_core_bayer_reg8_value` to force the descriptor Bayer word and the
  first-frame `reg8` write together.
- `0x10008` with derived `reg88=0x10c`: active IRQs and RTSP frame, still
  false-colored.
- `0x10009` with derived `reg88=0x10d`: active IRQs and RTSP frame, still
  false-colored.
- `0x1000a` with derived `reg88=0x10e`: no active ISP IRQ counts and no usable
  RTSP video parameters; treat as a bad phase for this profile.
- `0x1000b` with derived `reg88=0x10f`: active IRQs and RTSP frame, different
  color balance, still false-colored.
- `0x10002` with derived `reg88=0x102`: active IRQs and RTSP frame, different
  color balance, still false-colored.

Conclusion: GIB/LSC bypass and CFA phase are not sufficient by themselves.
The next likely hurdle is DMSC output/refresh and downstream BCSH/CSC/CCM
state, with a minimal top-bypass profile used to keep enhancement blocks out
of the way.

Bayer phase was revisited after the minimal DMSC+BCSH/CSI-settle baseline
because the stock T40 register-diff reference has `core+0x88=0x102` while the
safe qbuf probe was forcing derived `reg88=0x10c`. Re-test:

- `logs/20260608-192914-t40-stock-bayer-10002-1b-242` used
  `CORE_BAYER_REG8_VALUE=0x10002`, top `0x7fdfeeff`, and CSI settle `0x1b`.
  Live readback after the run showed `core+0x88=0x102`.
- The RTSP-matching qbuf `0x6ea8300` became slightly more neutral than the
  previous `0x10008` baseline: RTSP channel spread dropped from about `14.4`
  to `10.5`. This is not a lifelike/color-complete image yet, but it is the
  better stock-aligned minimal baseline.
- `CORE_BAYER_REG8_VALUE` is now exposed by `tools/t40_safe_qbuf_dump_probe.sh`
  and defaults to `0x10002`.

CSC and YUV-input CSC probes:

- The first `csc-version-sweep` log was invalid as a CSC sweep because the
  bring-up profile reset `tisp_main_init_csc_version_value` back to `2` during
  module init. Do not use `logs/20260608-151739-csc-version-sweep-r40-7fdfe8ff-242`
  as evidence for versions `0`, `1`, or `3`.
- The corrected sweep echoed the CSC version after `insmod` and before the
  sensor load: `logs/20260608-153126-actual-csc-version-sweep-r40-7fdfe8ff-242`.
  Versions `0`, `1`, and `2` produced similar washed false color; version `3`
  regressed to the older saturated false-color class. CSC version alone is not
  the missing color fix.
- OEM `tisp_main_init` only calls the YUV-input CSC latch when the mode field
  is `3`; the live GC4653 path reports mode `1`. Forcing that path anyway
  (`logs/20260608-152957-yuv-input-csc-force-r40-7fdfe8ff-242`) regressed to a
  flat green-class frame. Keep `force_tisp_main_init_yuv_input_csc_version=0`.

Direct recovered color-init replay:

- A broad direct replay of recovered DMSC, CCM, Gamma, and BCSH init calls with
  mask `0x8700` reached the first color-init call and oopsed before the normal
  tisp tail, MSCA OEM, event init, and Tiziano init could run:
  `logs/20260608-153902-color-inits-8700-r40-7fdfe8ff-csc2-242`.
- The pre-restore fault bundle is
  `logs/20260608-154238-pre-restore-color-inits-oops-242`; proc showed
  `tail_count=0`, `msca_oem_count=0`, `event_init_count=0`,
  `tiziano_count=0`, zero IRQ 38/39 counts, and empty qbuf samples.
- The driver keeps `enable_tisp_main_init_color_inits` default-off and now
  leaves `tisp_main_init_color_init_mask` at `0` by default. Any future replay
  must explicitly select one block at a time and should prefer manual register
  mirroring over broad direct calls into recovered functions.

Current safe restore after the failed color-init probe:

- `logs/20260608-155233-force-reboot-safe-csc2-242`
- Forced reboot verified by uptime reset, then loaded the rebuilt recovered
  module with `enable_tisp_main_init_color_inits=0`,
  `force_tisp_main_init_yuv_input_csc_version=0`, CSC version `2`,
  top value `0x7fdfe8ff`, forced Bayer `0x10008`, and `reg88=0x10c`.
- Evidence: IRQ 38 and 39 were active, `tail_count=1`,
  `msca_oem_count=1`, `event_init_count=1`, `tiziano_count=1`, qbuf/output
  samples were nonzero and changing, and RTSP produced
  `frame-safe-csc2.jpg` at 1920x1080. The frame is still false-colored.

## 2026-06-08 Post-RTSP Color Probes

The earlier proc `core_color_path_snapshot` used T31-era windows (`0x4800`
DMSC and `0x8024` BCSH). Direct physical reads on T40 showed the live windows
are different:

- DMSC main: `0xa000` and nearby offsets.
- BCSH main: `0x11000-0x11070`; H matrix is `0x11024-0x11038`.
- AWB gains: `0x4004/0x4008/0x400c/0x4010` and
  `0x5004/0x5008/0x500c/0x5010`.
- CSC: `0xd010-0xd030`.
- GIB remains in the `0x1030-0x1070` neighborhood.

The driver proc snapshot has been updated to print those T40 windows when
`enable_t40_color_reg_snapshot=1`; it remains default-off.

Manual BCSH tests:

- H-matrix-only OEM base write:
  `logs/20260608-161114-capture-after-bcsh-oem-base-242`.
- Full clip/offset plus H-matrix write:
  `logs/20260608-161540-live-bcsh-full-oem-242`.
- Result: the scene brightened and moved away from the darkest false-color
  class, but it remained non-lifelike. BCSH is involved, but not sufficient.

Top-gate matrix after the BCSH write:

- `logs/20260608-161722-live-top-color-matrix-after-bcsh-242`
- Re-enabling LSC produced strong geometric/striping artifacts.
- Re-enabling GIB changed the palette but did not normalize color.
- Bypassing CCM/Gamma/BCSH made the frame more synthetic/magenta.
- Keep LSC bypassed and do not treat downstream bypass as a valid baseline.

Top-gate matrix after raw-qbuf validation:

- `logs/20260608-184023-t40-top-dmsc-only-242`, top `0x7fdffeff`
  (DMSC only active): both IRQs stayed live, but qbuf/RTSP regressed to strong
  magenta/blue false color.
- `logs/20260608-184231-t40-top-dmsc-ccm-242`, top `0x7fdffcff`
  (DMSC+CCM active): also a strong false-color regression.
- `logs/20260608-184336-t40-top-dmsc-ccm-bcsh-nogamma-242`, top
  `0x7fdfecff` (DMSC+CCM+BCSH active, Gamma bypassed): recognizable mostly
  neutral scene, still with green/pink tint and severe line/geometry artifacts.
- `logs/20260608-184604-t40-top-dmsc-bcsh-242`, top `0x7fdfeeff`
  (DMSC+BCSH active, CCM/Gamma bypassed): matched the best no-Gamma result
  without needing CCM. This is the current minimal top-register baseline.
- Conclusion: DMSC alone is not enough; CLM/BCSH is needed to avoid the
  magenta failure. Gamma is harmful in the current path, and CCM is not needed
  for the best minimal baseline observed so far.

BCSH saturation-like clamp:

- `logs/20260608-162119-live-bcsh-saturation-clamp-242`
- Clamping `0x1106c/0x11070` from `0x1fff` to `0x0`, `0x100`, `0x200`,
  `0x400`, and `0x800` barely changed the frame. Those words are not the
  dominant false-color control.

Neutral-UV buffer test:

- Utility: `tools/phys_memfill.c`
- Slow fill did not hold against the live ISP writer:
  `logs/20260608-162801-live-neutral-uv-fill-242`.
- Fast fill held one observed UV plane fully neutral and partially affected
  the other:
  `logs/20260608-162937-live-neutral-uv-fill-fast-242`.
- Result: the RTSP frame became dark/tearing/false-colored rather than clean
  grayscale. This is not conclusive proof of correct UV addressing; it shows
  live buffer ownership/timing is part of the problem and a driver-side
  neutral-UV control would be a better diagnostic than userspace `/dev/mem`
  racing.
- A default-off driver diagnostic now exposes
  `framechan_neutral_uv_on_done`, `framechan_neutral_uv_channel_mask`, and
  `framechan_neutral_uv_value`. It fills the completed UV plane only from the
  deferred frame-done work item, and direct IRQ calls record a skip instead of
  mapping/writing frame memory. Runtime validation is still pending because
  `192.168.50.242` dropped off the network while stopping Raptor before the
  diagnostic module was inserted:
  `logs/20260608-163949-driver-neutral-uv-smoke-242`.
- Runtime validation after a clean Tasmota power cycle
  (`192.168.50.103` controls the camera power for `192.168.50.242`) reached the
  diagnostic path:
  `logs/20260608-182925-t40-neutral-uv-qbuf-242`. Buffer `0x6ea8300` rendered
  grayscale when interpreted as either NV12 or NV21, proving that the direct
  qbuf UV plane was neutralized. The RTSP frame stayed false-colored, so the
  encoder is still reading a different/stale buffer sequence or the neutral
  fill timing is not aligned with the buffer the encoder consumes.

Raw qbuf dump workflow for the next live pass:

- Device helper: `tools/phys_memdump.c`.
- Host renderer: `tools/nv12_probe.py`.
- Combined safe-profile probe script: `tools/t40_safe_qbuf_dump_probe.sh`.
  It intentionally avoids full `/proc/tx_isp_t40_recovered` reads while
  streaming; the 2026-06-08 18:10 run wedged at that proc read even though IRQ
  38/39 had come online.
- For the known safe 1920x1080 qbuf layout, dump `0x2fd000` bytes starting at
  the active qbuf physical address. The Y plane is `1920 * 1088 = 0x1fe000`
  bytes, so UV begins at `phys + 0x1fe000`.
- The repeated safe-profile allocations on 2026-06-08 used ch0 buffers around
  `0x6bab300` and `0x6ea8300` with length `0x2fd000`. Later captures showed
  `0x6ea8300` is the better primary fallback; `0x6bab300` can be stale or
  partially black. The script dumps both when no qbuf line is available from
  `dmesg`.
- Render the dump as NV12, NV21, and neutral chroma:
  `python3 tools/nv12_probe.py qbuf.bin --width 1920 --height 1080`.
- If the neutral render is sane but NV12/NV21 are not, the fault is chroma
  generation/order. If the neutral render is still spatially/color-broken,
  keep looking before the YUV output stage.

Raw qbuf dump result:

- Evidence: `logs/20260608-182321-t40-safe-qbuf-dump-242`.
- Both IRQs were live after safe-profile load: IRQ 38 and IRQ 39 were active.
- The static `/tmp/phys_memdump` path successfully dumped ch0 qbufs without a
  proc read. Build it static; the camera userland is uClibc and cannot execute
  a musl-linked helper.
- Buffer `0x6ea8300` rendered as neutral chroma is a coherent grayscale scene.
  Rendering the same bytes as NV12 or NV21 remains strongly false-colored.
- Conclusion: this is not just NV12/NV21 byte order. The Y/luma output is
  carrying a plausible scene, and the next target is UV/chroma generation or
  downstream color-to-YUV programming before the qbuf reaches RTSP.
- Updated dual-buffer baseline after another Tasmota power cycle:
  `logs/20260608-183454-t40-safe-qbuf-dual-242`. IRQ 38 and IRQ 39 were live,
  `framechan_neutral_uv_on_done=0`, and the qbuf/RTSP captures matched as real
  scene data with bad chroma. This is the current best non-neutral baseline for
  color work.
- Newer minimal-top baseline:
  `logs/20260608-184604-t40-top-dmsc-bcsh-242` with top `0x7fdfeeff`.
  This keeps DMSC and CLM/BCSH active while bypassing CCM and Gamma; it is
  closer to a neutral/lifelike scene than the earlier all-color baseline, but
  still has green tint and severe line/geometry artifacts.

CSI/PHY timing checkpoint:

- Earlier real-frame proof was
  `logs/20260608-182321-t40-safe-qbuf-dump-242/qbuf-6ea8300-renders/qbuf-6ea8300-gray.jpg`.
  That file is coherent grayscale scene data from the qbuf Y plane. The NV12
  and NV21 renders from the same dump are false-colored, so this is not a
  simple NV12/NV21 byte-order problem.
- The old/default-profile evidence used `csi_settle_override=0x1b`; the
  current T40 bring-up profile had drifted to `0x10`.
- Re-testing the minimal DMSC+BCSH top baseline with
  `CSI_SETTLE_OVERRIDE=0x1b` in
  `logs/20260608-190839-t40-csi-settle-1b-242` kept both IRQs live and moved
  qbuf/RTSP channel means slightly closer together. The image was still not
  lifelike; the horizontal/diagonal line artifacts remained.
- Validation after another Tasmota power cycle:
  `logs/20260608-192131-t40-csi-snapshot-dual-1b-242`. This run captured both
  ch0 qbufs (`0x6bab300` and `0x6ea8300`) plus RTSP. Buffer `0x6ea8300`
  matched the RTSP frame much more closely; `0x6bab300` can show a stale or
  worse half-frame class. Keep dumping both known qbufs until buffer ownership
  is resolved.
- Corrected live `devmem` readback on 2026-06-08 showed the recovered T40 CSI
  constants are `0x10054000` for CSI reg0, `0x10023000` for CSI reg1/W01, and
  `0x10022000` for the MIPI PHY. The requested `0x1b` was present at PHY
  settle offsets `0x160/0x1e0/0x260/0x2e0/0x360`, and W01 phase read back
  `0x630`. Do not use the T31-era `0x10021000` PHY base for this T40 test.
- The stock T40 register-diff reference uses `0x13380000` as the primary VIC
  physical base. A live read after the dual-qbuf validation showed
  `0x13380304=0x07800438`, `0x13380310/314=0x780`, and Y/UV banks alternating
  between `0x6bab300/0x6ea8300` and `0x6da9300/0x70a6300`. The earlier
  `0x133e0000` T31-style VIC base reads as zero on this target and should not
  be used for T40 MDMA evidence.
- Conclusion: CSI settle was a real discrepancy and `0x1b` is now the default
  probe/profile value, but it is not the whole fix. The remaining geometry
  artifacts still point at CSI/VIC timing, crop, packed-width, or stride state
  that must be compared against OEM-good readbacks.

## 2026-06-08 Offline qbuf forensics: VIC MDMA stride/format is the dominant defect

> CORRECTION (superseded by the live OEM capture below): the 0xF00 VIC stride
> is the VIC->ISP *internal* raw line stride (1920 * 2 bytes), not the DDR/NV12
> qbuf stride. The real NV12 output stride is 0x780 and is correct. The true
> root cause is the output *engine*, not this stride -- see the next section.


Re-analysis of the raw qbuf dumps already on disk (no live camera needed,
`tools/qbuf_forensics.py`) reframes the problem. Two findings:

1. The committed "best" stock-Bayer baseline is not better color -- it is a
   near-monochrome frame. In `logs/20260608-192914-t40-stock-bayer-10002-1b-242`
   the RTSP-matching buffer `0x6ea8300` has Y std 40 (real scene) but U/V std of
   only ~2.6/3.0. Chroma collapsed to near-zero. The low RTSP "channel spread"
   (10.5) that looked like progress is desaturation, not correction. Every
   color-bearing config (DMSC-only, +CCM, +BCSH) instead shows structured chroma
   with std 40-67, a persistent +dV (red/magenta) DC bias, and chroma
   anti-correlated with luma -- a demosaic/CFA-phase or CbCr-sign signature.
   Do not optimize for channel spread; it rewards killing color.

2. The dominant *luma* defect is a strong periodic vertical band (FFT period
   ~64/128 rows, strength ~68x) plus a horizontal-shear sideband, visible as a
   diagonal "venetian-blind" tearing lattice over the whole frame. It is present
   in every recovered-driver dump regardless of color config, so it is a fixed
   capture-geometry artifact, not a per-frame race. The OEM stock RTSP frame
   (`logs/20260608-034423-t40-stock-reg-diff-reference/rtsp-ch0-frame.jpg`) has
   no such banding.

Register evidence pins the cause. Comparing the live VIC snapshot to the stock
register diff reference (`.../stock-devmem-regs.txt`):

- Stock OEM: `vic+0x310 = vic+0x314 = 0x00000F00` (Y/UV stride = 3840 = width*2),
  and `vic+0x18 = 0x00000F00`.
- Recovered: `0x13380310 = 0x13380314 = 0x00000780` (stride = 1920 = width).

The recovered qbuf-ring path (`regtrace_vic_mdma_internal_program` /
`enable_vic_mdma_qbuf_ring`) defaults `stride = width` with the ctrl fmt nibble
= `7` (1 byte/px). The OEM stride 0xF00 implies a non-7, 2-byte/px VIC MDMA
output format. fmt=7 is the prime suspect for the 64-row tiled banding. This is
upstream of the entire color path codex was tuning, so color cannot be trusted
until the luma geometry matches OEM.

Next experiments (knobs now exposed in `tools/t40_safe_qbuf_dump_probe.sh`:
`VIC_MDMA_QBUF_RING_STRIDE_OVERRIDE`, `VIC_MDMA_QBUF_RING_CTRL_VALUE`,
`VIC_MDMA_QBUF_RING_UV_OFFSET_OVERRIDE`; all default 0 = current behavior):

1. First, a read-only capture: re-run the stock OEM driver and snapshot
   `vic+0x300` (ctrl) and `0x310/0x314` *during streaming* (the existing stock
   reference caught stride but `0x300=0` pre-stream). That gives the exact OEM
   fmt nibble to replicate.
2. Then sweep our fmt+stride together to match OEM. Caution: raising stride to
   0xF00 with the current fmt=7 8bpp layout doubles the Y-plane footprint
   (3840*1088 > the 0x2fd000 qbuf) and will overrun the buffer, so stride must
   move with a matching fmt/`UV_OFFSET_OVERRIDE` (and possibly a larger qbuf),
   not alone.
3. Re-run `tools/qbuf_forensics.py` on each dump; success is band strength
   dropping toward ~1x with chroma std staying nonzero.

## 2026-06-08 Live OEM capture: root cause is the wrong output engine

Reboot to a clean boot, then load the genuine stock driver
(`/lib/modules/4.4.94/ingenic/tx-isp-t40.ko`, note hyphens) + sensor + Raptor
and read the VIC and ISP-core channel registers *during streaming*. Evidence:
`logs/20260608-195228-t40-oem-live-ctrl-stream-242/` (`oem-load-stream.log`,
`oem-core-channel-regs.log`, `oem-stock-frame.jpg`). The fresh stock frame is
pristine: sharp geometry, no banding, no tearing (neutral grayscale because the
camera was in IR/night mode, RGB spread 0.0).

VIC MDMA is NOT the OEM output path. While stock is streaming:

- `vic+0x300` (MDMA ctrl) = 0, sampled repeatedly. Our recovered driver instead
  writes `0x80030027` here (`(qbuf_count<<16)|0x80000020|fmt7`).
- `vic+0x308/0x30c` = 0 and ALL buffer-base slots `vic+0x318..0x338` = 0.
- Only the size/stride config is set: `vic+0x304=0x07800438` (1920x1080),
  `vic+0x18 = vic+0x310 = vic+0x314 = 0xF00` (the VIC->ISP internal raw line
  stride = 1920*2, a 16-bit intermediate -- not a DDR qbuf stride).

The OEM writes NV12 via the ISP-core MSCA channel engine. While streaming:

- `core+0x16100 = 0x07800438` (ch0 out 1920x1080), `core+0x16180 = 0x780`
  (Y stride 1920), `core+0x16198 = 0x780` (UV stride).
- `core+0x16170 = 0x800101xx` (Y FIFO ctrl, enable bit set, live-changing),
  `core+0x16174 = 0x0764B70x` (Y FIFO frame addr + status, live-updating),
  `core+0x16188 = 0x800110xx` (UV FIFO ctrl), `core+0x16084 = 0x0A0005A0`,
  `core+0x160a4/ac/b4 = 0x0A000000`.
- `/proc/jz/isp` confirms: ch0 "scaler width: 1920" with `queue addr:
  0x0764b700 / 0x0734e700`; ch1 "scaler width: 640" (the substream). Those qbuf
  addresses are in the SAME rmem pool (0x06-0x07M) our recovered qbuf ring uses
  (`0x6bab300/0x6ea8300/...`).

Conclusion. Sensor -> CSI -> VIC (raw 16-bit, internal stride 0xF00) -> ISP core
(demosaic/CCM/BCSH/...) -> **ISP MSCA channel FIFO** writes clean NV12 to the
framechan qbufs (stride 0x780). The recovered bring-up profile bypasses that
final write engine: `t40_profile_force_vic_mdma_qbuf_ring=1` arms the VIC MDMA
ring (`vic+0x300/0x310/0x318`, fmt=7) to dump VIC-stage data straight to the
qbuf. Reading VIC-stage 16-bit/stride-3840 bytes as 8-bit/stride-1920 NV12 is
exactly the ~64-row banded shear + false chroma we see. This is why no
CFA/CSC/BCSH/CCM color tuning ever fixed it -- the ISP core's processed output
was never the thing reaching the qbuf.

Fix direction (real driver work, not a knob sweep). The MSCA channel-output
path already exists in the recovered driver but is gated/unused in this profile:
`REGTRACE_T40_MSCA_Y_FIFO_CTRL_REG 0x16170`, `..._READ_REG 0x16174`,
`..._UV_FIFO_CTRL_REG 0x16188`, the `0x16100/0x160a4/0x16180` programming, and
the `enable_t40_msca_shadow_fifo_program` / `enable_t40_msca_*` family. The work
is to drive the MSCA Y/UV output FIFO into the framechan qbufs the way stock
does (mirror `core+0x16170/0x16174/0x16180/0x16188/0x16198` + per-frame addr
rearm) and stop forcing the VIC MDMA ring -- then re-validate luma geometry with
`tools/qbuf_forensics.py` (target: band strength ~1x) before touching color
again. The `VIC_MDMA_QBUF_RING_*` knobs added earlier remain useful only for
characterizing the wrong-engine artifact, not for matching OEM.

Device left on the clean stock driver after this capture; re-run
`tools/t40_safe_qbuf_dump_probe.sh` to reload the recovered module.

## 2026-06-08 MSCA-FIFO bring-up WORKS: banding gone, new bottleneck is AE

Switching the recovered driver from the forced VIC-MDMA qbuf ring to the
OEM-style ISP MSCA channel FIFO path is achieved with a **single parameter** --
no driver code change. The MSCA FIFO + frame-done completion path was already
coded but gated behind `t40_profile_force_vic_mdma_qbuf_ring`. Loading with that
forced to 0 lets `regtrace_apply_t40_bringup_profile()` wire up the frame-done
IRQ + MSCA FIFO read path automatically.

Evidence: `logs/20260608-201105-t40-msca-fifo-ringoff-242` (loaded via
`T40_PROFILE_FORCE_VIC_MDMA_QBUF_RING=0 tools/t40_safe_qbuf_dump_probe.sh`).
dmesg shows the full OEM-style cycle at ~25 fps:

- `Tzn_Msca_addr_fifo_write group=0 ch=0 raw=0x6bab300/0x6da9300 ... tag=1`
  (push qbuf Y/UV addrs into the MSCA FIFO),
- `Tzn_Msca_addr_fifo_read group=0 ch=0 y=0x6ea8300 uv=0x70a6300` (read the
  completed-frame addr back from FIFO reg 0x16174),
- `framechan0 irq frame-done local source=0x4 y=0x6ea8300 idx=1` (frame-done
  marks the buffer done),
- `framechan0 repaired qbuf source=oem-qbuf` (buffer recycled).

Results:

- The RTSP frame is CLEAN: recognizable scene geometry matching the OEM frame,
  **no 64-row banding, no diagonal tearing**. The VIC-MDMA raw-capture artifact
  is gone because we now read the ISP-core-processed channel output, not the
  raw VIC stage.
- Frames are live (sequential RTSP frames differ by md5), streaming is stable
  (IRQ 38/39 keep incrementing).
- RTSP serves frames (no more "connection refused" that the bare no-ring path
  used to give -- the historical "buffers don't move" failure is resolved by
  the now-complete frame-done path).

New bottleneck -- exposure/AE, not geometry:

- The frame is very dark (RTSP brightness ~2-17/255) and decays toward black
  over the first minute of streaming, with a faint green tint. Because the MSCA
  path captures the ISP-core *processed* output, near-zero AE gain yields a dark
  frame -- the old VIC-ring captures only looked bright (Y mean ~127) because
  they grabbed *raw* VIC data that bypassed AE. So this is the sensor
  exposure/gain (AE) loop not being driven up, not a pipeline defect. (Ambient
  light was also lower than the 19:57 stock capture, but the time-correlated
  decay points at AE.)
- Next work is the AE/gain path: confirm the tisp event-4 (`tisp_tgain_update`)
  loop is closing onto the GC4653 exposure/again over I2C, and that AE stats are
  being read. Only after exposure is usable should CFA/CSC/BCSH color tuning
  resume -- and now it is meaningful because the ISP-core output finally reaches
  the qbuf.

Knobs added: `T40_PROFILE_FORCE_VIC_MDMA_QBUF_RING` (default 1 = legacy ring;
set 0 for the OEM MSCA path) and `T40_PROFILE_NO_DIRECT_ADDR_SOURCE`
(0=MSCA FIFO read, 1=qbuf record, 2=MSCA read reg) in
`tools/t40_safe_qbuf_dump_probe.sh`. The probe default is left at the legacy
ring for now so existing baselines reproduce; flip to 0 for OEM-path work.

NOTE: before loading the recovered module, ensure stock `tx_isp_t40` is fully
unloaded -- it owns the `isp_printf` symbol and a leftover instance makes the
recovered insmod fail with "invalid module format"/"duplicate symbol". A clean
reboot (no ISP module auto-loads) is the reliable reset.

## 2026-06-08 AE/exposure investigation: the AE->sensor I2C bridge is the gap

With the MSCA pipeline clean, the dark frame is an AE problem. Findings (live +
code):

Live proof the lever works:
- GC4653 is at I2C `1-0029`. Its exposure/gain registers are STATIC across
  repeated reads under the recovered driver: exposure `0x0202/0x0203 = 0x05d0`
  (1488 of VTS `0x0340/41 = 0x0780` = 1920), analog gain `0x0205 = 0xc0`. A
  working AE in a near-black scene would be maxing exposure and cranking gain.
- Manually writing exposure `0x0760` + again `0xff` + a digital-gain bump over
  I2C raised RTSP brightness ~9x (2 -> 18.7) and produced a clearly visible
  (noisy, untinted-by-AWB) scene: `logs/20260608-203632-t40-ae-manual-i2c-242`.
  Critically, the recovered driver never overwrote those manual writes, proving
  it issues NO sensor exposure/gain writes at all.

Code path (driver/t40/tx_isp_t40_recovered.c):
- The bring-up profile starts the tisp event thread (`enable_tisp_event_threads`
  default on) but does NOT register the AE gain/exposure update callbacks. Those
  (events 6/7/10 = long/short tgain + long again, table at ~line 9342) only
  register when `enable_tisp_stream_event_init=1` AND
  `enable_tisp_stream_event_cbs=1` (both default off).
- Enabling both (`logs/20260608-204340-t40-msca-ae-events-242`) was NOT
  sufficient: sensor regs stayed frozen, frame stayed dark. So callback
  registration alone does not close the loop.
- The AE->sensor bridge is `tiziano_sync_sensor_attr` (65739) ->
  `tisp_ae_sensor_trig` (70041) -> sensor ioctl. But `tisp_ae_sensor_trig` only
  sets a "needs-sync" flag byte; the real I2C write is a separate
  `regtrace_sensor_call_sensor_ioctl(REGTRACE_TX_ISP_EVENT_SENSOR_SET_*, ...)`
  (7959/8067). `tisp_main_long_again_update` (58656) is a stub returning 0.
  There is no module_param that forces the sync/trigger to run, and no
  sensor_ioctl exposure/gain call is seen in dmesg.

Conclusion: the AE algorithm result is never pushed to the GC4653 over I2C. The
remaining work is to wire the AE->sensor sync: (1) confirm the AE algorithm
actually ticks and computes gain from ISP stats, (2) ensure the computed
exposure/again reach `sensor_ioctl` (un-stub `tisp_main_long_again_update` and/or
drive `tiziano_sync_sensor_attr` + consume the `tisp_ae_sensor_trig` flag from
the event loop). Until then, a userspace I2C AE shim can brighten the image as a
bridge, since manual sensor gain works. AWB is also off (manual-gain frame had a
yellow/green tint) and will need the ct/awb events similarly wired.

Knobs added: `ENABLE_TISP_STREAM_EVENT_INIT`, `ENABLE_TISP_STREAM_EVENT_CBS`
(both default 0) in `tools/t40_safe_qbuf_dump_probe.sh`.

## 2026-06-08 In-driver AE plan: decoded GC4653 sensor ioctl ABI

Groundwork for wiring the AE->sensor push inside the recovered driver.

What the recovered driver is missing: the "sensor-apply" stage. `sensor_set_
integration_time` (42313) and `sensor_set_analog_gain` (42451) only STAGE values
into ISP-core shadow memory (`g_ispcore` + offsets 176/288/792/796/816/820 with
dirty flags) -- they never touch I2C. The only `regtrace_sensor_call_sensor_
ioctl` call in the whole driver is `SENSOR_SET_INPUT` (8067). There is NO call
that pushes exposure/gain to the sensor.

Decoded sensor ABI (from the on-device `sensor_gc4653_t40.ko`, not stripped;
full disasm saved to `scratch/gc4653_ioctl_abi.txt`):

- `sensor_sensor_ops_ioctl(sd, cmd, arg)` decodes `cmd` as
  `idx = cmd - 0x2000006`, valid when `idx < 17` (so cmd in
  `0x2000006 .. 0x2000016`), then dispatches via a 17-entry jump table.
- The exposure/integration case reads the value at `*(arg+4)` and writes GC4653
  registers `0x0203` (low) and `0x0202` (high) via `sensor_write`, then applies
  analog gain from `sensor_again_lut`. So the combined-exposure event near
  `cmd=0x2000006` is the apply the ISP must call; payload is a struct whose
  word at offset 4 carries the packed value.
- CONFIRMED from the jump table: index 0 (`cmd = 0x2000006`) is the EXPO case.
  Payload is a struct whose word at offset +4 is packed
  `(again_index << 16) | integration_time` (integration is the low 16 bits ->
  regs 0x0202/0x0203; again_index selects the `sensor_again_lut` row). So the
  apply call is `regtrace_sensor_call_sensor_ioctl(0x2000006, &payload)` with
  `payload[4] = (again<<16)|inttime`. (`SENSOR_SET_INPUT=0x2000004` is a
  different, lower event and unrelated to exposure.)

Wiring plan (next):
1. Verify the AE algorithm actually ticks and computes a total gain/exposure
   (find the AE process/`tisp_ae_process` tick driven by the event thread or the
   frame-done work) and where it deposits the result.
2. Add a sensor-apply step: read the staged exposure/again (the ISP shadow slots
   that `sensor_set_integration_time`/`_analog_gain` write) and call
   `regtrace_sensor_call_sensor_ioctl(<EXPO cmd>, &payload)` once per frame
   (from the frame-done work or the event loop). Un-stub
   `tisp_main_long_again_update` (58656) if it is on the path.
3. Gate it behind a new `enable_ae_sensor_apply` knob (default off) so it can be
   toggled, and verify live: GC4653 `0x0202/0x0203/0x0205` should start MOVING
   and RTSP brightness should track the scene.

Reminder: manual I2C exposure/gain already produces a good image, so this is a
wiring task, not a sensor-capability question. Keep the Tasmota power-cycle
(`192.168.50.103`) handy; reload requires stock `tx_isp_t40` fully unloaded.

## T31 Lessons To Reuse Carefully

The T31 tuning history has the same class of visual failure: severe
green/magenta false color with real scene data.

Historical anchors:

- `f37c28868810f760002b00e0a00707569b64bbdb` is documented as the last known
  "crisp image" checkpoint.
- The later regression was tied to expanding enabled processing blocks.
- Under the then-current masked logic, the effective crisp-image block set was
  `0xDD04`.

Important caution: `0xDD04` is a T31 experimental anchor, not automatically a
valid T40 bypass value. It tells us which classes of blocks to question first:
early raw/color blocks, especially CFA/DMSC/GIB/LSC/BCSH.

## High-Probability Hurdles

### 1. CFA/Bayer phase

Wrong CFA phase can create exactly this kind of stable false color while scene
geometry remains recognizable. The live register to audit is ISP register
`0x8`, written by the Bayer/mbus path. The first sweep should capture the four
possible Bayer phases with all other variables fixed.

### 2. Top bypass policy

The top bypass register at `0x0c` controls major ISP blocks:

- bit 5: GIB
- bit 6: LSC LUT gate
- bit 8: DMSC
- bit 9: CCM
- bit 10: Gamma
- bit 12: CLM/BCSH
- bit 16: MDNS
- bit 17: YDNS
- bit 18: RDNS

The current T40 top value `0x7fd9004f` is broad. The next baseline should be a
minimal block policy that keeps only the blocks required to convert the sensor
stream into sane NV12/YUV, then enables one class of tuning at a time.

### 3. DMSC output mode

T31 notes call out DMSC output ownership around register `0x4800` and gain
refresh ordering from event 4 (`tisp_tgain_update`). For T40, dump `0x4800`
plus the known DMSC neighborhood before changing it. A plausible path is:

1. fixed CFA phase
2. DMSC active with minimal/no downstream enhancement
3. DMSC gain refresh confirmed or parked

### 4. BCSH / CSC / CCM

BCSH H-matrix registers live at `0x11024-0x11038` on this T40 path. Identity
matrix values are not a valid RGB-to-YUV conversion and can produce bad color.

The T40 profile currently forces a CSC version path, and the corrected CSC
sweep proves that CSC version alone is not sufficient. That is not proof that
BCSH/CCM tables are valid or that the H-matrix is being recomputed. Dump this
range before trusting any color-correction stage.

### 5. GIB and LSC

T31 history repeatedly points at GIB/LSC as high-risk false-color blocks. Keep
GIB and LSC bypassed until the minimal DMSC/CSC path can produce a plausible
scene. Then test:

- GIB alone
- LSC alone
- GIB plus LSC

Do not assume the pair is safe just because each block initializes.

### 6. Event-driven refresh

T31 event 4 refreshes gain-dependent blocks: GIB, GB/BLC, DMSC, sharpen, SDNS,
DPC, LSC, YDNS, RDNS, and MDNS. Event 7 affects BCSH/downstream EV tracking,
and event 10 affects AWB.

For T40, distinguish between:

- static register init needed for a valid first frame
- per-frame tuning refresh needed for stable color
- synthetic fallback refresh that writes plausible-looking but wrong tables

The `image crisp` checkpoint removed a synthetic AWB/LSC fallback tick from the
ISR path. Avoid reintroducing that class of "help" until OEM evidence proves it.

## Immediate Experiment Order

1. Add or use a T40 register snapshot for:
   - `0x8`, `0x0c`
   - `0xa000` and nearby DMSC registers
   - `0x11024-0x11038` BCSH H-matrix range
   - `0x1030-0x1070` GIB range
2. Keep broad DMSC/BCSH/GIB snapshots default-off during normal RTSP runs.
   Audit one register window at a time, preferably before stream start or in a
   bounded smoke run.
3. Capture the current bad-color baseline with safe registers in the same log.
4. Treat the initial Bayer sweep as done for `0x10008-0x1000b` plus
   `0x10002`; do not repeat unless other path state changes.
5. Capture the small T40 CSI/MIPI/VIC register snapshot from
   `tools/t40_safe_qbuf_dump_probe.sh` for each run. Compare the
   `0x10054000`, `0x10023000`, `0x10022000`, and `0x13380000` blocks against
   OEM-good evidence before changing more ISP tuning state.
6. Build a minimal top-bypass profile:
   - keep stream/stat plumbing alive
   - keep DMSC active if required for output format
   - bypass GIB, LSC, denoise, sharpen, defog, ADR/WDR, and other enhancement
     blocks initially
7. Re-enable blocks one at a time from the minimal baseline:
   - DMSC refresh
   - Gamma/CSC/BCSH
   - LSC
   - GIB
   - denoise family
8. Do not broad-call recovered T40 color init helpers. Isolate one block per
   boot, or mirror the exact OEM register writes manually, and capture
   pre/post proc evidence before starting Raptor.
9. Only after a plausible lifelike frame appears, map T40 tuning-blob offsets
   for the active blocks and replace guesses with blob-backed values.

## Success Criteria

A passing frame is not just a large JPEG or changing frame. It must show:

- recognizable scene geometry
- plausible neutral colors
- no stable green/magenta blobs
- stable output after at least 90 seconds of RTSP capture
- matching register/proc evidence for the tested tuning state
