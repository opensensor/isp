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

## 2026-06-08 EXPO bridge verified + shear is a pre-existing MSCA defect

Two independent results from the in-driver AE work (commit "lock in verified
GC4653 EXPO bridge"):

1. **AE->sensor EXPO bridge works (verified).** A gated frame-done hook
   (`enable_ae_sensor_apply`, default off) issues `TX_ISP_EVENT_SENSOR_EXPO`
   (combined case **0x02000016**, not 0x02000006) to the GC4653 with a packed
   `(again_idx<<16)|integration`. Forcing `0x00190760` moved GC4653
   `0x0202/0x0203 -> 0x07/0x60` and took the frame black->bright
   (`logs/20260608-212400-t40-ae-force-expo-safe-cmd16-242`, RTSP mean ~255).
   Gain index is clamped to 25 (GC4653 LUT = 26 entries); index 255 walked off
   the LUT and crashed the sensor module. Non-forced runs show the staged AE
   slots are never dirtied (`dirty=0/0/0`) -> the upstream AE algorithm isn't
   producing values; that's the remaining AE gap (separate from the bridge).
   The `enable_ae_main_process_on_frame` experiment (ticking
   `tisp_ae_main_process()` from frame-done) **reboots the camera every run**
   and was removed.

2. **The diagonal shear is NOT exposure/AE and NOT a geometry-register bug.**
   With exposure now mid-tone (~140) the raw qbuf shows a periodic
   **4-strip diagonal comb: 272-line vertical period (1088 = 4x272)**, dominant
   2-D FFT sideband at (fx=1, fy=4). It was always present on the MSCA-FIFO
   path; the earlier "no tearing" 20:11 capture was fooled by a near-black frame
   (ymean 17) -- normalized by brightness the comb energy is the same in both.
   - Luma stride is correct: stride=1920 minimizes the vertical-gradient sweep.
   - **ISP-core MSCA ch0 geometry matches OEM exactly**: `0x13316100=0x07800438`
     (1920x1080), `0x13316180=0x16198=0x780` (Y/UV stride 1920). So the shear is
     not static out-size/stride misprogramming.
   - The addr-FIFO writes a single linear 1920x1088 buffer (Y then UV
     contiguous, UV-Y = 0x1fe000), so the shear is generated *inside* the MSCA
     output, not at the buffer-address level.
   - Notable: **ch0 AND ch2 are both active at full 1920x1080**
     (`0x13316300=0x07800438`, stride 0x780); ch1 is the 640x360 substream
     (`0x16200=0x02800168`), ch3 inactive. The rendered frame also *ghosts*
     (scene visible twice) -> two full-res channels + frame-done are a prime
     suspect for tearing/mixing.

   Next experiments (device, the probe now also dumps `0x13316xxx` MSCA geometry
   via devmem): (a) dump two consecutive frames -> if the comb position moves,
   it's tearing; if static, a fixed scaler-strip layout. (b) Disable/repoint ch2
   and re-check the comb. Driving the scaler/FIFO config -- not color tuning --
   is the lever.

## 2026-06-09 "Fractal regression" diagnosed: dead ISP 3A/stats loop (ADR)

The persistent diagonal "venetian-blind" artifact is NOT tearing/timing/MSCA.
Proven this session:

1. **Fixed additive overlay** — ~80% cancels in a two-frame difference; it's the
   same pattern every frame, content-independent. ~28-30 vertical-zone banding
   (~38 output-row period) measured in a smooth region.
2. **Upstream of the MSCA** — MSCA geometry/scaler/filter/format all match OEM on
   live silicon (0x161c0-cc=0x40080, 0x16028=1, 0x1602c=0x7400, single group).
   Ruled out buffer-ownership (rearm-guard alternated buffers perfectly, no
   change) and delivery-timing (12ms delay, no change).
3. **It's ADR** — Adaptive Dynamic Range, the only enabled tile-based block.
   Extracted the OEM tisp_main_init reg40 bit->block map: bit4=DPC, bit5=GIB,
   **bit7=ADR**, bit8=DMSC, bit9=CCM, bit10=gamma, bit11=defog, bit14=YDNS,
   bit15=BCSH, bit21=LCE. Live top40=0x7fdfeeff has bit7 set (ADR active),
   bit21 (LCE) clear.
4. **Root cause = the 3A/stats loop never runs.** ADR is a closed loop (T31 ref
   gtxaspec/txx-isp-c confirms): stats-DMA IRQ -> per-stat handler reads stats +
   tisp_event_push -> event thread runs *_main_process -> writes new per-tile
   gains. With the loop dead, ADR applies its init-default tile map forever.

   `enable_isp_3a_diag` instrumentation (gated, in post_dispatch_ack +
   ispcore_interrupt_service_routine + the ADR handler/process) proved the exact
   break on live silicon:
   - ISP stats interrupts **DO fire**: status0 (core+0x40028) shows bits
     3(AWB) 4(AE) 5(AE-hist) 6(AF) 8(LCE) setting; mask0=0x1ffff enables them.
   - `irq_func_cb[]` is **EMPTY** -> no stat handlers registered.
   - `ispcore_interrupt_service_routine` is **never invoked** (isp_irq_handle's
     subdev-ops pointer-walk doesn't resolve in the bring-up profile); the live
     path is regtrace_isp_irq_post_dispatch_ack, which acks status0 but does no
     `irq_func_cb` fanout.
   - The per-block software init (`tisp_adr_main_init`/`tisp_ae_main_init`, which
     vmalloc the stats DMA buffer AND call system_irq_func_set) never ran -- the
     recovered bring-up programs ADR *hardware* registers directly but skips the
     software init. `adr_main_stat_info` is therefore null.

   Same root cause as the earlier "AE never driven" finding (staged AE slots
   always clean) -- one fix addresses both AE and ADR.

Fix (option 3, in progress): run tisp_adr_main_init + tisp_ae_main_init (gated)
to allocate stats DMA + register handlers in irq_func_cb, then add the stats
fanout to post_dispatch_ack (for each set status0 stat bit, call
irq_func_cb[bit]). Event thread + cbs already enabled. ADR idx=9, AE idx=4/5.

### Update: the wall — recovered bring-up runs NO ISP software init

Step-2 wiring built: `enable_isp_stats_fanout` adds the missing `irq_func_cb[]`
fanout to post_dispatch_ack (for each set status0 stat bit 3-11, call the
registered handler; null-guarded). Correct, but it has nothing to dispatch to:

- With `enable_tisp_main_init_tiziano=1` + `enable_tisp_main_init_event_init=1` +
  `enable_isp_stats_fanout=1`, `irq_func_cb[]` is STILL empty; ADR-irq=0,
  ADR-proc=0.
- Root: `enable_tisp_main_init_tiziano` only sets the global `tiziano_enable`,
  which is read by **`tisp_deinit`** (line ~61006), NOT by an init path. The
  recovered live bring-up **never calls** `tisp_init`, `tisp_adr_main_init`, or
  `tisp_ae_main_init` (no call sites in the live path). It pokes ADR/AE hardware
  registers directly and skips the entire ISP software init.

Therefore the per-stat handlers are never registered and the stats DMA buffers
(`adr_main_stat_info`, AE buffers) are never allocated. The fanout + event
thread are ready, but the loop has no producers.

**Remaining work to finish option 3:** actually invoke the OEM ISP software init
in the recovered driver — either `tisp_init` (the full block-init sequence) or
the individual `tisp_adr_main_init` + `tisp_ae_main_init` with correct args and
prerequisites (tuning blob loaded via `enable_tiziano_param_load`, core base,
DMA). This is the integration the bring-up profile deliberately avoided because
it is complex and crash-prone; it needs careful, incremental work (high risk of
device crash per attempt). The diagnosis, reg40 bit map, fanout, and event
plumbing are all in place; this init invocation is the one missing producer.

### Update 2: option-3 attempt A — block-init call faults on missing param infra

Added gated `enable_isp_block_init`: calls the OEM `tisp_ae_main_init` +
`tisp_adr_main_init` inside regtrace_tisp_main_init (mirroring OEM args
&tisp_par_info, tuning_blob+0x5940), to register the stats handlers + vmalloc
the stats DMA. Result on device: bring-up aborts inside the call — the
regtrace_tisp_main_init shim line (printed AFTER the call) never appears, reg40
isn't applied, and streaming never comes up (dmesg has only audio-codec init,
no ISP). No Oops, but a fault/hang in the block init.

Cause: the recovered driver's tuning blob is truncated (`data_b0000[16384]`,
but OEM passes blob+0x5940 = 0x5940 > 0x4000) and the per-block param structures
the inits dereference aren't reconstructed. So the inits read past valid data.

Conclusion: option 3 (drive the 3A loop) is blocked by missing ISP param/tuning
infrastructure, not by the wiring (which is done: reg40 map, stats fanout, event
plumbing). Finishing it requires reconstructing the ISP tuning-param blob load +
the per-block param tables the inits consume -- i.e. completing the ISP software
reconstruction, a substantial effort beyond bring-up tweaks. enable_isp_block_init
is left default-off (faults when enabled).

### Update 3: option-3 attempt A — init wiring correct, init functions mis-reconstructed

Corrected the block-init arg: pass the LOADED tuning blob (tparamsN[0]+0x5940,
from /etc/sensor/gc4653-t40.bin, 200KB, bridged at lines ~63283-86) instead of
the truncated static data_b0000. The param-load IS wired (the user was right).

With the right arg, calling the inits no longer reads bad params -- but both
**decompiled init functions crash from reconstruction bugs** (uninitialized
register artifacts), because the recovered bring-up never executed them before
so the bugs were latent:

- `tisp_ae_main_init` (71524): fragment 8 writes `*(*(s2+60))=...` but s2 is
  never assigned (declared =0) -> null+0x28c deref Oops.
- `tisp_adr_main_init` (135097): null+0 deref Oops in a sub-call/divergence.
  The OEM decompiled_text for tisp_adr_main_init is clean (vmalloc+memset of
  data_a8xxx globals), so the fault is in the recovered version's divergence,
  not the OEM logic.

Knobs added (default off): enable_isp_block_init (ADR), enable_isp_block_init_ae
(AE, separate because it faults first).

**Conclusion / remaining work for A:** the recovered ISP 3A init chain
(tisp_ae_main_init, tisp_adr_main_init, and their callees) must be repaired
against the OEM decompiled_text (tx-isp-t40-whole-binary.json) / the gtxaspec
T31 reference -- fix the lost register assignments so the inits run cleanly.
Then they register the idx-4/idx-9 stats handlers + alloc stats DMA, the
already-built fanout drives the loop, and ADR gains update (banding clears).
This is a focused multi-function reconstruction-repair effort.

### Update 4: ADR grid/register repair, build-tested only

The streaming/MSCA path can work while ADR is still visibly wrong. The likely
failure is not qbuf/interrupt delivery anymore; it is stale or bad ADR hardware
state after `tisp_adr_main_init` survives far enough to allocate/register the
software loop but never correctly recomputes and pushes the OEM block grid.

OEM `tiziano_adr_base_pars(width, height, ch)` does three concrete things:

- Selects `adr_main_hard_base` / `adr_sec_hard_base`.
- Recomputes the 5x7 tile coordinates from frame size:
  `grid_x = even(width / 6)`, `grid_y = even(height / 4)`, M coords
  `{0, grid_y, 2*grid_y, 3*grid_y, height}`, N coords
  `{0, grid_x, 2*grid_x, 3*grid_x, 4*grid_x, 5*grid_x, width}`.
- Recomputes the 31-value 5x5 distance table from `adr_5x5_in2`, then the OEM
  writers push hard-base and hard-para tables to ADR registers.

The recovered `tiziano_adr_base_pars` still contains unresolved div/mflo and
table-access artifacts, and the old ADR writer functions rely on similarly
fragile decompiler output. New direct guarded helpers now do the OEM-equivalent
base-pars math and ADR register writes:

- `regtrace_tiziano_adr_base_pars()` computes the 5x7 grid and distance table.
- `regtrace_func_adr_reg_write_one/5x5/sometimes/every()` push the ADR register
  tables using the recovered hard-base/hard-para pointer tables.
- `enable_adr_reg_writes` gates the hardware write stage. Probe default is `1`
  when block-init is enabled, matching OEM; set `ENABLE_ADR_REG_WRITES=0` to
  bisect allocation/handler registration separately from register programming.

Important caveat: the direct base-pars helper deliberately does NOT call the old
`tiziano_adr_5x5_init()` yet. That function still has risky reconstruction
damage, so this patch repairs the grid/register push first. If the live image
still shows an ADR tile artifact after this, repair `tiziano_adr_5x5_init` next.

Human-inspection test recipe when it is safe to interrupt the camera:

```
T40_PROFILE_FORCE_VIC_MDMA_QBUF_RING=0 \
ENABLE_ISP_BLOCK_INIT=1 \
ENABLE_ISP_BLOCK_INIT_AE=0 \
ENABLE_ISP_STATS_FANOUT=1 \
ENABLE_TISP_STREAM_EVENT_INIT=1 \
ENABLE_TISP_STREAM_EVENT_CBS=1 \
ENABLE_ADR_REG_WRITES=1 \
SMOKE_SLEEP_SECS=120 \
tools/t40_safe_qbuf_dump_probe.sh logs/$(date +%Y%m%d-%H%M%S)-t40-adr-grid-242
```

Per user request, leave the stream running long enough for visual inspection on
future live tests. Do not churn reloads around a short automatic capture unless
the stream is already down.

### Update 5: ADR pointer-table builders repaired, build-tested only

The screenshot/user-visible failure is still the fixed diagonal ADR/fractal
artifact in the raw qbuf, not an RTSP encode problem. Live read-only check while
the stream stayed up showed:

- Current live module has `enable_isp_block_init=N`, so the new ADR-grid code is
  not loaded/running on that stream.
- `enable_msca_rearm_guard=Y` was toggled live without interrupt loss, but a
  new RTSP frame and both raw qbuf renders still showed the same wedge. The
  simple same-buffer guard is not the fix.

Deeper ADR reconstruction bug found and fixed locally: several ADR pointer-table
builders had GP/global reconstruction damage, not just `tiziano_adr_base_pars`.

- `tiziano_adr_wal_para_refresh()` was writing 142 pointers into the 0x11c-byte
  WAL allocation, which only holds 71 pointers. OEM has a 71-entry table and
  selects one of two offset tables using `data_a8958[channel]` (linear/WDR ADR
  mode). The recovered helper lost that branch and overran the allocation.
- `tiziano_adr_hard_base_refresh()` and `tiziano_adr_hard_para_refresh()` were
  treating OEM byte offsets as `int32_t *` word offsets. That points the ADR
  hardware table entries hundreds of bytes too far into `main_adr_paras`.
- `tiziano_adr_hard_para_refresh()` was also sourcing entries 0xb/0xd/0xe from
  the top-para table in recovered code; OEM sources them from WAL entries
  0x42/0x43/0x44.
- `tiziano_adr_top_para_refresh()` and `_soft_para_refresh()` each had a final
  pointer store stranded after an early return.
- `tisp_adr_linear_switch()` wrote its mode/dirty flags through a bogus
  `isp_memopt` base. It now updates a real `regtrace_adr_linear_mode[channel]`
  state, rebuilds the ADR tables, recomputes base geometry when needed, and
  pushes the ADR register tables.

The local driver now uses direct OEM-equivalent ADR table builders for WAL,
top, soft, hard-para, and hard-base. This should be the next thing to live-test
with `ENABLE_ISP_BLOCK_INIT=1`, `ENABLE_ISP_BLOCK_INIT_AE=0`, stats fanout/event
callbacks on, and a long human-inspection window. Still not live-tested as of
this note.

One more critical caller fix: block-init must call `tisp_adr_main_init(width,
height)`, not `(tisp_par_info, blob+0x5940)`. The AE init uses the latter form,
but ADR init stores its two args into the global ADR width/height slots and then
uses `tparamsN` for tuning. The live-test path now calls ADR init as
`1920x1080` unless the existing sensor-full-width/height override knobs are set.

### Update 6: diamond/green pattern localized to UV/chroma, not ADR geometry

Live tests on 2026-06-09 falsified the ADR-grid root-cause hypothesis for the
visible green/diamond artifact.

What was ruled out:

- ADR direct init, full-sensor grid, and linear mode ran cleanly:
  `ENABLE_ISP_BLOCK_INIT=1`, `ENABLE_ISP_BLOCK_INIT_AE=0`,
  `ENABLE_ADR_REG_WRITES=1`, `ADR_LINEAR_MODE=1`,
  `SENSOR_FULL_WIDTH_OVERRIDE=2560`, `SENSOR_FULL_HEIGHT_OVERRIDE=1440`.
  The ADR regs matched the full-sensor grid:
  `0x13309030=0x01680000`, `0x13309034=0x043802d0`,
  `0x13309038=0x000005a0`, `0x1330903c=0x01aa0000`,
  `0x13309040=0x04fe0354`, `0x13309044=0x085206a8`,
  `0x13309048=0x00000a00`.
  The visible green/diamond artifact remained.
- GC4653 stock-vs-recovered I2C dump (`tools/t40_dump_gc4653_i2c_regs.sh`)
  differed only in timing/exposure regs:
  `0x0202/03`, `0x0207/08`, `0x0340/41`. Forcing stock timing/exposure onto
  recovered changed brightness/exposure but did not remove the artifact.
- Forcing the remaining stock host-side CSI/MIPI/VIC static diffs after streamon
  was diagnostic only and did not remove the artifact.

Decisive localization:

- Live-enabling `framechan_neutral_uv_on_done=Y` removed the green/chroma
  diamonds from completed frames and left a coherent grayscale luma image.
- Startup neutral-UV alone only partly neutralized the image; the lower band
  stayed green. Enabling `enable_msca_rearm_guard=Y` fixed that startup race by
  skipping early same-buffer MSCA completions until the FIFO alternated buffers.
- Clean-start run with both knobs enabled produced a stable clean grayscale
  stream with healthy IRQs 38/39:
  `logs/20260609-105536-t40-neutral-uv-rearm-startup-hold`.
  Follow-up non-invasive RTSP check:
  `logs/20260609-verify-live-neutral-uv/rtsp-now.jpg`.

Current known-good inspection recipe:

```
FRAMECHAN_NEUTRAL_UV_ON_DONE=1 \
ENABLE_MSCA_REARM_GUARD=1 \
MSCA_REARM_GUARD_MAX_SKIPS=8 \
T40_PROFILE_FORCE_VIC_MDMA_QBUF_RING=0 \
CSI_SETTLE_OVERRIDE=0x10 \
ADR_LINEAR_MODE=1 \
SENSOR_FULL_WIDTH_OVERRIDE=2560 \
SENSOR_FULL_HEIGHT_OVERRIDE=1440 \
ENABLE_ISP_BLOCK_INIT=1 \
ENABLE_ISP_BLOCK_INIT_AE=0 \
ENABLE_ADR_REG_WRITES=1 \
SMOKE_SLEEP_SECS=180 \
SKIP_QBUF_DUMP=1 \
tools/t40_safe_qbuf_dump_probe.sh logs/$(date +%Y%m%d-%H%M%S)-t40-neutral-uv-rearm-242
```

Bottom line: the visible diamond/green failure is a chroma-plane problem in the
MSCA/output path (UV contents, UV address freshness, color conversion/AWB/IR
chroma), not luma geometry and not the ADR hardware grid. The neutral-UV fill is
a valid inspection workaround for the current IR/night scene, but the real color
fix is to make the MSCA UV plane correct without neutralizing it.

### Update 7: neutral UV was only half the fix; top40 bit21 removes luma diamonds

User-visible correction: the green chroma diamond was gone after neutral UV, but
the grayscale stream still had the luma diamond/diagonal texture. Raw MSCA qbuf
Y renders confirmed the pattern was already in the completed luma plane, not an
RTSP encode artifact.

Live reversible testing on the running stream:

- Clearing ADR bit7, YDNS bit14, defog bit11, or BCSH bit15 did not remove the
  luma diamond.
- Setting top40 bit21 (`0x13300040: 0x7fdfeeff -> 0x7fffeeff`) immediately
  produced stable clean luma over repeated RTSP frames:
  `logs/20260609-115023-t40-live-lce-bit21-hold/rtsp-0.jpg` ..
  `rtsp-5.jpg`.
- Readback after the hold showed `0x13300040 = 0x7fffeeff`, and IRQs 38/39
  remained healthy. The stream was intentionally left running in this state for
  human inspection.

Correct current inspection baseline:

```
TISP_MAIN_INIT_TOP40_VALUE=0x7fffeeff \
FRAMECHAN_NEUTRAL_UV_ON_DONE=1 \
ENABLE_MSCA_REARM_GUARD=1 \
MSCA_REARM_GUARD_MAX_SKIPS=8 \
T40_PROFILE_FORCE_VIC_MDMA_QBUF_RING=0 \
CSI_SETTLE_OVERRIDE=0x10 \
ADR_LINEAR_MODE=1 \
SENSOR_FULL_WIDTH_OVERRIDE=2560 \
SENSOR_FULL_HEIGHT_OVERRIDE=1440 \
ENABLE_ISP_BLOCK_INIT=1 \
ENABLE_ISP_BLOCK_INIT_AE=0 \
ENABLE_ADR_REG_WRITES=1 \
SMOKE_SLEEP_SECS=180 \
SKIP_QBUF_DUMP=1 \
tools/t40_safe_qbuf_dump_probe.sh logs/$(date +%Y%m%d-%H%M%S)-t40-lce21-neutraluv-242
```

Code default updated: the T40 bring-up profile and
`tools/t40_safe_qbuf_dump_probe.sh` now seed `tisp_main_init_top40_value` with
`0x7fffeeff`. That keeps the LCE/top40 bit21 state that removed the luma
diamonds, while preserving the module/script override for future sweeps.

Revised root-cause split:

- Luma diamond texture: top40 bit21/LCE-path state.
- Green diamond color: bad UV/chroma plane, currently suppressed by neutral UV.
- Remaining real image-quality work: restore correct chroma/color and AE/AWB;
  keep bit21 set unless a later OEM-equivalent LCE init is reconstructed.

### Update 8: AWB block-init reconstruction started for color/tuning parity

Live IRQ status on the clean grayscale stream shows the ISP is raising AWB/AE
stats (`status0=0x38`: bits 3 AWB, 4 AE, 5 AE-hist), but only ADR had a
registered stats handler (`irq_func_cb[9]`). There was no `irq_func_cb[3]`, so
AWB stats were acknowledged and dropped.

Root cause in `tisp_awb_main_init` was the same reconstruction class seen in
AE/ADR: GP-relative globals were rendered incorrectly. Two concrete AWB bugs
were repaired behind a new default-off knob:

- `tisp_awb_main_init` allocated the 60328-byte AWB inter block but copied from
  `&sclk_name`; original assembly points at the AWB initialized object. The
  repair seeds from the recovered `stMainAwbInterOri` materialized bytes and
  zero-fills the missing tail.
- The recovered init called `tisp_awb_main_process(0,14,fn)` directly. Original
  assembly registers it with `tisp_event_set_cb(0,14,fn)`, after registering
  AWB static IRQ handler index 3.
- `tisp_awb_params_refresh` now writes into allocated `stMainAwbOuter` /
  `stSecAwbOuter` instead of the wrong static scratch arrays, and its final AWB
  grid loops use the pointer fields shown by the original assembly.

New gated knobs:

```
ENABLE_ISP_BLOCK_INIT_AWB=1
ENABLE_AWB_REG_WRITES=0|1
AWB_MAIN_INIT_STAGE_LIMIT=0..4
```

Use `ENABLE_AWB_REG_WRITES=0` for the first callback-registration test. That
keeps the AWB allocation/refresh/event path live but prevents the still-recovered
AWB hardware setters from touching color registers until the handler path is
proven. `AWB_MAIN_INIT_STAGE_LIMIT` is an internal bisect gate:

- `1`: return after inter/outer allocation.
- `2`: return after `tisp_awb_params_transmit`.
- `3`: return after `tisp_awb_params_refresh`.
- `4`: return after optional hardware writes but before callback registration.
- `0`: full AWB init.

Suggested first live test for color-loop bring-up, leaving the stream up for
human inspection:

```
TISP_MAIN_INIT_TOP40_VALUE=0x7fffeeff \
FRAMECHAN_NEUTRAL_UV_ON_DONE=1 \
ENABLE_MSCA_REARM_GUARD=1 \
MSCA_REARM_GUARD_MAX_SKIPS=8 \
T40_PROFILE_FORCE_VIC_MDMA_QBUF_RING=0 \
CSI_SETTLE_OVERRIDE=0x10 \
ENABLE_TISP_STREAM_EVENT_INIT=1 \
ENABLE_TISP_STREAM_EVENT_CBS=1 \
ENABLE_ISP_STATS_FANOUT=1 \
ENABLE_ISP_BLOCK_INIT=1 \
ENABLE_ISP_BLOCK_INIT_AE=0 \
ENABLE_ISP_BLOCK_INIT_AWB=1 \
ENABLE_AWB_REG_WRITES=0 \
AWB_MAIN_INIT_STAGE_LIMIT=0 \
ENABLE_ADR_REG_WRITES=1 \
ADR_LINEAR_MODE=1 \
SENSOR_FULL_WIDTH_OVERRIDE=2560 \
SENSOR_FULL_HEIGHT_OVERRIDE=1440 \
SMOKE_SLEEP_SECS=180 \
SKIP_QBUF_DUMP=1 \
tools/t40_safe_qbuf_dump_probe.sh logs/$(date +%Y%m%d-%H%M%S)-t40-awb-blockinit-neutraluv-242
```

Expected proof of life: dmesg should show `awb-main-init repaired`,
`irq_func_cb[3]`, AWB stats fanout counts rising, IRQs 38/39 healthy, and the
neutral-UV inspection stream should remain clean. Only after that passes should
we run a real-UV/color test with `FRAMECHAN_NEUTRAL_UV_ON_DONE=0` and then
consider enabling AWB register writes. If UV is still bad, keep neutral UV as the
inspection workaround and continue with the MSCA UV-plane/AWB gain path
separately.

Note: the 2026-06-09 `t40-awb-blockinit-realuv` attempt did not reach the new
AWB code; the partial `load-safe.log` ends during `S31raptor stop`, before
`insmod`. The camera was power-cycled and restored with the known-good
neutral-UV recovered stream:
`logs/20260609-142105-t40-recover-clean-neutraluv-242`.

Follow-up: `logs/20260609-142438-t40-awb-cb-neutraluv-242` reproduced the same
failure signature while stopping the active recovered stream. The log again ends
inside `S31raptor stop` after consumer daemons stop and before any new `insmod`,
so this is not an AWB-code result. Treat it as a recovered-driver
release/stream-stop blocker: killing the active `rvd` stream can take the camera
off-LAN before the module reload starts.

Safety change: `tools/t40_safe_qbuf_dump_probe.sh` now refuses to stop an active
`rvd` stream by default. Use one of these paths instead:

- Preferred: power-cycle first, wait for the camera to return, then run the
  probe from a no-stream boot state.
- Explicit override only when deliberately testing the release path:
  `ALLOW_ACTIVE_STREAM_STOP=1`.

Current restored stream after the stop/release finding:
`logs/20260609-142753-t40-recover-clean-neutraluv-242` with neutral UV,
MSCA rearm guard, top40 bit21, IRQs healthy, and AWB disabled.

Follow-up code guard for future loads: the probe now passes
`FORCE_LOCAL_FRAME_STREAMOFF=1` by default (`force_local_frame_streamoff` module
param). This leaves streamon on the OEM remote path, but routes streamoff through
the local recovered MSCA/VIC/CSI shutdown path instead of
`TX_ISP_EVENT_FRAME_STREAMOFF`. The release crash needs a deliberate live test
later with `ALLOW_ACTIVE_STREAM_STOP=1`, but default probe behavior should no
longer arm a recovered module that uses the suspected remote streamoff callback
on `rvd` exit.

## 2026-06-09 AWB reconstruction repairs verified against OEM binary

Offline disassembly of `tx-isp-t40.ko` (`tisp_awb_params_refresh` at
`0x2fc38`, `tisp_awb_main_init` at `0x311b4`) confirmed the pointer-table
interpretation of the AWB ctx and exposed four reconstruction bugs, all fixed:

1. `stMainAwbInterOri` was materialized truncated (16384 of 60328 bytes).
   Re-extracted the full blob from OEM `.data+0x1d3d8` (no relocations inside).
   The tail carries real seed data at `+0xc605..0xc6b5` and `+0xe0f4..0xe11b`,
   and several AWB functions index past 16K (`+0x4f5c`, `+0x515c`, `+0x53b8`,
   `+0x53c0`), which previously read out of bounds.
2. `tisp_awb_params_refresh` stored the `arg1[54]` flag byte at `dest+0x5a`;
   the OEM stores it at `dest+0x5e` (`sh v0,-10(a0)` with `a0=dest+0x68`).
   The flag-clear semantics (`sb zero,0(ptr)` through `arg1[101]`) and both
   tail grid loops were verified correct as repaired.
3. `pstMainAwbOri`/`pstSecAwbOri` were declared as 4-byte scalars but OEM
   reserves 0x200 bytes each — `tisp_awb_params_transmit` fills them as
   128-entry pointer tables and would have smashed adjacent .bss. Now arrays.
4. `tisp_awb_main_init` anchored its ctx at `isp_memopt` (.bss+0); OEM anchors
   at `pstMainAwbOri` (.bss+0x3f34). Also OEM `tisp_main_init` passes sensor
   width/height words (`tisp_par_info[0]`, `tisp_par_info+4`) to
   `tisp_awb_main_init`, not the `(par_info, blob)` pair AE takes; the
   block-init gate now does the same with override fallback.

Probe guard refinement: the active-stream refusal now only triggers when the
recovered module is actually loaded (`/sys/module/tx_isp_t40_recovered`), so
fresh-boot runs over the stock driver proceed without
`ALLOW_ACTIVE_STREAM_STOP=1`.

## 2026-06-09 AWB live bring-up results (fresh-boot power-cycle workflow)

Workflow note: camera power-cycles via the Tasmota switch at 192.168.50.103
(`curl http://192.168.50.103/cm?cmnd=Power%20Off` / `On`), camera back on LAN
~20s later; run probes only from this fresh-boot state.

- Stage-4 AWB block-init (alloc + transmit + refresh, no reg writes, no
  callbacks) is stable live: `isp_block_init_awb_ret=0`, IRQs healthy
  (`logs/20260609-173701-t40-awb-stage4-neutraluv-242`).
- Full AWB init (stage 0) crashed: `tisp_awb_main_interrupt_static` was
  fragment soup that fed a garbage vaddr to `private_dma_cache_sync` in IRQ
  context, and the AWB stats vaddr (`tispinfo+36`) is only seeded when
  `enable_tisp_main_init_tispinfo_dma=1`. Handler repaired against OEM 0x31a40
  with a skip-and-log guard when the stats vaddr is unset.
- Stage-4 + `ENABLE_AWB_REG_WRITES=1` oopsed at `tisp_awb_set_regional_threshold`
  → `tisp_simple_intp_int16` (BadVA = outer+0x34c0, vmalloc guard page): the
  recovered function passed swapped interpolation args and had unmatched
  lwl/lwr fragments. Repaired against OEM 0x3053c (8 ct-interpolated thresholds
  into outer[68..82], packed-word writes to 0x18028/0x18228 banks, awb trig).
- BREAKTHROUGH: with the AWB table/blob repairs in place, real UV
  (`FRAMECHAN_NEUTRAL_UV_ON_DONE=0`) now produces a clean color image — no
  diamond/venetian-blind artifact — with the expected strong green cast of
  un-white-balanced Bayer (`logs/20260609-175033-t40-awb-stage4-realuv-nowrites-242`,
  frame: lamp renders yellow, window cyan, walls green). The remaining color
  fix is WB gains via the `tisp_awb_set_gain`/`set_hardware_param` path.

### AWB hw-write chain repairs (2026-06-09, continued)

The kmsg-streaming workflow (start `cat /proc/kmsg` over SSH before the probe
so oops reports survive the watchdog reboot) pinned the reg-writes crashes:

1. `tisp_awb_params_transmit` (recovered) wrote wrong bases/offsets into the
   upper pointer-table entries — live diagnostic showed `ctx[34] = outer+13504`
   (an inter-family offset on the outer base). Rewrote the whole function from
   a symbolic extraction of OEM 0x2f560: entries 0..38 are outer+{0,100,...,4712},
   entries 39..127 are inter+{0,1800,...,59428}; sec path (a1=1) uses the
   stSec* pair. Verified live: `ctx34=outer+3376`, `ctx117=inter+57592`,
   `*ct=0x400`.
2. `tisp_awb_set_hardware_param` (recovered) contained foreign fragment code
   calling `tisp_simple_intp_int8` with a garbage table (OEM makes no such
   call) → BadVA 0xacb. Rewrote per OEM 0x30a70: pack 10 words from outer
   params → 0x18004..0x18024 + 0x1804c (sec +0x200), gated by *ctx[100], then
   call regional_threshold + lum_th_freq.
3. `tisp_awb_set_lum_th_freq` rewritten per OEM 0x30704: mode!=1 does ct
   interpolation over the three 11-entry tables at ctx[34]+176/198/220;
   mode==1 does piecewise-linear over the 9-entry lum zone table at ctx[21]
   keyed by *ctx[116]; tail calls tisp_ae_(sec_)mean_update, writes packed
   word to 0x18038/0x18238, pulses awb trig.

## 2026-06-09 WB gains live + software gray-world AWB

Empirical discoveries (all live-verified on .242):

- top40 (reg 0x40) bits are BYPASS bits for bits 8/12-class blocks (clear =
  active, matching the 2026-06-08 matrix): **bit2 = WB gain (WBG) unit**. With
  bit2 set (bypassed) the AWB gain bank writes have zero image effect; with it
  clear they apply directly. New probe default top40: `0x7fffeefb`.
- **bit3 hard-freezes the SoC when cleared** (silent, watchdog reboot, even
  with sane AWB config). Blacklisted until its block is identified/initialized.
- WB gain registers (confirmed by image response): `0x4004`/`0x400c` = R,
  `0x4008`/`0x4010` = B (G implicit unity, matching the T31
  `Tiziano_awb_set_gain` write pattern), value = `clamp14(gain)|0x04000000`
  with 0x400 = 1.0, latched by writing 1 to `0x4000`/`0x5000`
  (`system_reg_set_awb_trig(2/3, 0)`). Second bank `0x5004..0x5010` mirrors.
- **`enable_tisp_main_init_dma_kseg0` was a live bug**: the stats DMA engines
  take pure physical bank addresses; the KSEG0 OR made the AWB stats banks
  cycle but stay zero. Profile default flipped to false.
- AWB stats record format (live-decoded, matches the OEM parser at 0x3146c):
  16-byte records on 128-byte group strides; R = w0[21:0],
  G = w1[11:0]<<10|w0[31:22], B = w2[1:0]<<20|w1[31:12],
  count = w3[5:0]<<8|w2[31:24], frame tag = w3[15:8].
- New `enable_awb_grayworld=1` mode: the repaired AWB stats IRQ handler
  parses all four banks (freshest tag wins), computes gray-world R/B gains
  with EMA smoothing and live-calibrated output-path biases
  (`awb_grayworld_rbias=1193`, `awb_grayworld_bbias=1448`, both 0644), writes
  the WBG banks, and re-arms the stats engine — a self-sustaining AWB loop at
  frame rate (~500 updates in the first verification run, stable convergence).
  The faithful Tiziano `tisp_awb_main_process` chain repair remains future
  work; gray-world bypasses it entirely.
- `tisp_ae_main_init` takes sensor width/height (OEM 0x23b34 stores a0/a1 as
  uint16 pair), same as AWB — the block-init gate now passes them.

### Software AE (ae-soft) status: controller works, sensor EXPO bridge is a no-op

`enable_ae_soft=1` (with `enable_ae_sensor_apply=1`) adds a proportional AE
controller to the gray-world stats loop: it measures G-channel per-pixel mean
from the AWB stats records and ladders GC4653 integration time (64..1919)
then analog-gain index (0..25) toward `ae_soft_target`, staging
`(again<<16)|it` through `ae_sensor_apply_force_packed`.

Live result 2026-06-09: the controller measured/stepped correctly and the
EXPO ioctl returned 0, but exposure never changed — A/B test
(again=10 vs 25 at it=0x400) moved scene luma 26.2 -> 27.3 (noise). At
it=1919/again=25 the ioctl returns -290. Conclusion: the
`TX_ISP_EVENT_SENSOR_EXPO` bridge into the GC4653 module does not reach the
sensor I2C (payload format or event routing). Direct i2ctransfer
verification is blocked: the bus times out while the sensor driver holds it.
Next session: trace the gc4653 module's EXPO ioctl handler (payload struct
layout, return -290 origin) or drive the sensor's 0x0202/0x0203 + gain LUT
registers through the driver's own I2C client. ae-soft stays default-off.

Also fixed: `stMainAeInterOri` materialized (17772 bytes from OEM
.data+0x5d3c); the recovered tisp_ae_main_init memcpy'd from the .data base
symbol instead. AE block-init remains gated off pending the same
verification pass AWB got (its init registers the full unverified AE
callback chain unconditionally).

## 2026-06-09 (late): EXPO bridge fixed, userspace 3A online

- ROOT CAUSE of the dead exposure path: the driver sent sensor event
  0x2000006, which in the GC4653 ioctl jump table is "set integration time
  only" (regs 0x200/0x201). The full expo handler (integration regs
  0x202/0x203 + the 9-register analog-gain LUT) is cmd **0x2000016**, reading
  the packed (again_idx<<16)|it word from arg+4. With the fix, gain index
  10 vs 0 swings scene luma 10x. The mystery -290 = two -145 I2C write
  failures summed (transient bus timeouts).
- The AWB stats engine input dies after rvd's second streamon (banks cycle,
  config intact, all zone counts zero; thresholds/windows ruled out by live
  permissive writes). Under investigation; the gray-world IRQ loop freezes
  with it. Workarounds in place: stats watchdog re-arm from frame-done +
  bank-select parse (the old frame-tag filter also wedged on its own).
- In-kernel frame sampling (enable_frame_3a) crashes the camera (suspected
  rmem cache-alias machine check from a second kernel mapping) — left
  default-off, DO NOT enable until reworked.
- WORKING SOLUTION: userspace 3A (`tools/t40_userspace_3a.sh`, deploy to
  camera /tmp, nohup): samples the NV12 output planes via /dev/mem
  (phys_memdump), ladders exposure through ae_sensor_apply_force_packed
  (now deduplicated - forced values only re-sent on change to avoid a 25Hz
  I2C storm) and trims WB via new awb_manual_rgain/bgain params that
  frame-done applies on change. Verified at night: luma 35 -> 103 in ~60s
  (it=1919, again=8), UV self-neutralizing, image transformed from black
  grain to a properly exposed scene.

## 2026-06-09 (final): gamma online, grain eliminated, smooth 3A

- Soft gamma shipped: `enable_soft_gamma=1` (probe default) streams a 2.2
  tone curve into the instance-0 gamma LUT units at block-init using the
  protocol decoded from OEM `tisp_gamma_real_write_lut` (start
  0x50040/60/80 = 0x101, 128 packed pairs to 0x50044/64/84, commit
  0x7f0102), and the probe's default top40 is now `0x7fffeafb` (bit10
  gamma + bit2 WBG active). Effect: the AE hits its target with a
  fraction of the exposure - converged at it=600/again=0 at night, where
  the linear pipeline needed it=1919/again=8+. Grain gone.
- The 3A agent is now proportional + slew-limited (max +/-15%/tick,
  deadband, luma EMA, gain-preferred-down, fast shed on gross
  overexposure) and resumes exposure state from
  ae_sensor_apply_force_packed on restart - no more ratcheting or reload
  slams. UV trim uses EMA'd means and a wider neutral window to stop
  ring-buffer sampling jitter from dithering the gains. The probe deploys
  and auto-starts /tmp/3a.sh after raptor.
- top40 sweep negative results (live, with temporal-noise metric): bits
  4/13/14 no visible effect without inits; bit6 brightens; **bit15 blanks
  the output to a solid green frame** (and fooled the temporal-noise
  metric with tnoise=0.08 - sanity-check content before trusting a noise
  metric). bit3 remains a hard-freeze. Real denoise needs the Tiziano
  YDNS/RDNS/SDNS inits.

## 2026-06-09 (cont): YDNS chain repaired faithfully, CCM scoped

- Full YDNS chain rewritten against OEM (params_refresh 0x60ae8, wdr_en
  0x608b8, intp 0x60674, reg_cfg 0x60530, all_reg_refresh/par_refresh/
  refresh/main_init): 465-byte param block from tparamsN+0x119ba, 20-entry
  comb pointer table selecting linear (+3..+244) vs WDR (+245..+464)
  11-byte interpolation tables, 22 working bytes pushed to the YDNS unit at
  0x10000 (regs +8/+10/+20/+24/+28/+40/+44/+48, trig +4). The recovered
  wdr_en had inverted flag logic AND wrote into fake g_data_* placeholder
  arrays while intp read the real anchors. `enable_ydns=1` (probe default)
  runs it at block-init; probe top40 default now 0x7fffaafb (bit14 also
  active). Live: regs configured with real values, stream healthy.
  A/B bit14 toggle showed no measurable tnoise change at the current
  operating point (gamma keeps analog gain at 0, tnoise already 0.41);
  expect the benefit at high-gain very-dark scenes.
- CCM scoped: tisp_ccm_main_init (0x6ffd8) chains transmit/refresh/
  ev_ct_update/lut_parameter/cm_control - all memory-to-memory; the hw
  apply point is indirect (no system_reg_write anywhere in the chain), so
  CCM needs the faithful chain repair + identifying its DMA/LUT apply
  mechanism. Parked with notes.
- AWB-stats-death after second streamon: still open; the alive window is
  only the first ~60s of a boot, so the planned register-window diff needs
  a scripted boot-race capture. Documented for next session.

## 2026-06-10: fog eliminated — OEM gamma curve + GIB black level

The "foggy" look vs stock was diagnosed as three missing pieces (NOT the
defog block, which targets atmospheric haze):

1. The synthetic 2.2 gamma lifted near-black enormously (out[1]=459/4095).
   The REAL OEM curve lives in the tuning blob at gc4653-t40.bin +0x106ec
   (halfword entries hw[2..130]): zero toe through entry 11 (anchored
   blacks) and max output 3932. soft-gamma now streams the OEM curve.
2. GIB (bit5) was bypassed, leaving the sensor black pedestal in the
   signal. New `enable_gib_blc=1` writes the four per-channel BLC offsets
   (regs 0x1030..0x103c, default 0x100) at block-init; probe top40 default
   is now `0x7fffaadb` (bit5 also active). The faithful gain-tracking GIB
   chain (tisp_gib_init 0x366c0, blob +0xd610, write_reg 0x35ff0 with 3
   mode paths) is decoded and remains for full repair (task: GIB init).
3. CCM saturation — still pending (indirect apply path).

Result: deep blacks, real contrast, stock-like tone. Remaining gaps vs
stock: sharpening (block unidentified yet) and CCM color depth.

## 2026-06-10 (cont): YSP sharpening scoped + extraction tooling

Sharpening = the YSP block (unit base (ch+152)<<9 = 0x13000 main), with the
exact same chain architecture as YDNS: params 3432B from blob +0x16d98
(main_ysp .bss+18228), 420B comb pointer table (+18220), 146B working buf
(+18212), wdr_en selects linear/WDR tables, all_reg_refresh = intp ->
noref_reg_cfg -> ref_reg_cfg -> reg_trig. Recovered fragment functions:
wdr_en, both reg_cfgs, the small glue; intp/params_refresh/reg_trig are
model_output (verify before trusting).

New tool `tools/regcfg_extract.py`: symbolic MIPS extractor for the
Tiziano reg_cfg-style functions (anchor loads, pointer arithmetic, packed
field expressions, system_reg_write emission). Extracted register maps in
docs/extracted/: noref complete (32 writes, no control flow), ref has 81
writes extracted but contains slt/movn/movz min-max clamp ladders that
need emulator-grade branch handling in the extractor before the C can be
assembled. That plus YDNS-pattern glue is the remaining sharpening work.

## 2026-06-10 (cont): sharpening online — YSP chain via literal translation

- New tool `tools/mips2c_literal.py`: literal MIPS32->C translator (per-
  instruction statements over virtual registers, goto control flow, delay-
  slot-safe condition pre-evaluation, reloc-anchored symbol mapping, call
  dispatch through tracked symbols). This is the general solution for the
  remaining fragment functions; min/max clamp ladders, loops and unaligned
  lwl/lwr pairs all translate mechanically.
- YSP chain installed: ref/noref reg_cfg + wdr_en + intp are literal
  translations; params_refresh/glue/main_init are YDNS-pattern rewrites
  (params 3432B from blob +0x16d98, unit regs at 0x13000). The model-output
  params_refresh was garbage (NULL deref + infinite loop) — replaced.
  `enable_ysp=1` probe default.
- top40 sweep with edge-energy metric: **bit20 gates the sharpening path**
  (+23% edge energy at +13% luma; visually stock-level texture/edges).
  New probe top40 default `0x7fefaadb`. bit18 blanks the output (black
  frame) — blacklisted alongside bit3 (freeze) and bit15 (green blank).
  bit19 brightens strongly (LCE/defog-class?) — left bypassed, candidate
  for later.

## 2026-06-10 (cont): CCM online — full color pipeline complete

- The entire CCM chain (transmit, refresh, reg2par, parameter_convert,
  ct_ccm_interpolation incl. its .rodata jump table -> C switch, para2reg,
  lut_parameter [the hw apply point], ev_ct_update, cm_control) installed
  as a self-contained `_lit` set via mips2c_literal; Ori blob (228B,
  .data+0x2d07c) materialized; `enable_ccm=1`, top40 default `0x7fefa8db`
  (bit9 CCM active).
- LESSON (cost two crash cycles): the literal-translation wrapper's
  virtual `mips_stack` must cover the translated function's full stack
  frame — cm_control uses a 256-byte frame and the original 288/224
  wrapper overflowed into the kernel stack (silent watchdog reboot).
  Wrappers now use 448/384. Also: shared macros (REGTRACE_LWLR) must be
  defined before the FIRST translated function in file order (an
  undefined-symbol modpost warning means a misplaced define).
- Visual result: full stock-character color — saturated greens, natural
  tungsten warmth, wood tones — on top of the sharpening/gamma/BLC stack.

Pipeline now live end-to-end: GIB BLC -> WBG (userspace auto-WB) -> DMSC ->
CCM -> OEM gamma -> BCSH -> YDNS -> YSP sharpening, with userspace AE.
Remaining vs stock: MDNS/SDNS (high-gain denoise), faithful GIB
gain-tracking, ADR loop, LSC; investigations: stats-death, daylight.

## 2026-06-10 (cont): anti-flicker AE, DNS gain tracking

- "Rolling fog" diagnosed as mains flicker banding: the 3A picked arbitrary
  integration times. The agent now moves IT only in whole flicker periods
  (FLICKER_STEP=400 lines ~= one 120Hz period at 25fps/1920 lines) and uses
  analog gain between rungs - converges to it=1600 (4 periods) + gain.
  Quantization lesson: percent-steps smaller than half a rung round back
  and deadlock; step in whole rungs.
- AE smoothness: rung ladder + gain steps at 1Hz with luma EMA and a 10%
  deadband; no more visible exposure staggering.
- Grain: YDNS/YSP strengths were stuck at their gain-1.0 interpolation
  point. New `dns_gain_ev` param (log2-gain 16.16, written by the agent on
  every exposure change) lets frame-done re-run ydns/ysp par_refresh so
  denoise/sharpen strength tracks gain like OEM tgain_update does.
- Remaining grain delta vs stock is MDNS (temporal denoise) - the next
  translation target.

## 2026-06-10 (cont): MDNS chain installed (recon state, default off)

Full MDNS chain literal-translated and installed as _lit set
(`enable_mdns`, DEFAULT OFF): eq_smp/eq_dif/reg_cfg(5.2KB)/start/trig/
intp/wdr_en/func_en/glue/main_init. Findings:
- Unit at (ch+30)<<11 = 0xF000 main; version reg +0 reads 0x20191209.
- Params 2398B from tuning blob +0x1105c (1813 nonzero bytes).
- tisp_mdns_func_en gates on get_isp_memopt() (memopt mode word) - in
  memopt modes temporal may be restricted.
- After an enable_mdns=1 init run, only sparse regs are nonzero
  (+0xc=0x40, +0x20=0x11, +0x100=0x10); needs a marker-verified run with
  kmsg + dense before/after reg diff to confirm the init executes fully,
  then identification of the temporal reference buffer & the MDNS top40
  gate (bit3 = freeze suspect) before any enable attempt.

AE/grain round (user-reported): target back to 105 (less gain), DNS
gain-EV mapping strengthened (24576/step ~ mid-table by gain 8). Visible
grain reduced; full fix remains MDNS.

## 2026-06-10 (cont): two-level AE — smooth at last

Measured GC4653 analog-gain steps: ~12-14% luma each — single steps are
visible and a deadband smaller than one step guarantees oscillation
("breathing"), which also pumps the encoder (perceived "distortion").
GIB digital gain knobs were chased and ruled out live (0x1050 no-op,
0x8000 window = sec instance, main 0x50140..0x50164 unconfigured/no-op).

The shipped design needs no new hardware: two-level control in the agent —
integration micro-trim of +/-80 lines in 16-line (~1%) steps around the
flicker rung (<=20% of a mains period off-perfect = negligible residual
banding) for fine corrections, with analog gain/rung changes only when the
fine range saturates, followed by a 4-tick settle hold. Deadband 5%.
Verified: converged at rung 1600 + fine 80, gain 7 (one step lower than
before), perfectly static readings.

## 2026-06-10: MDNS temporal denoise ONLINE — the grain fix

End state: the MDNS unit (0xF000) is configured byte-identically to the
OEM-driven hardware (verified by full-unit /dev/mem diff against a stock
boot), the camera is stable, and the live image is visibly grain-free.

What it took, in dependency order:

1. **Reference buffers.** OEM programs them from a TX_ISP set-buffer ioctl
   branch (OEM 0x131bc) that our probe path never issues: five planes from
   one rmem chunk at (ch+30)<<11 + 0x40..0x64 (addr/stride pairs), with
   the Y/UV plane sizes >>8 at +0x70/+0x74. Layout (isp_memopt=0):
   Y ref w16*H*33/32, UV ref w32*H/2*129/128, motion plane ~W/16 B/line,
   full-width half plane, quarter plane — each align1024; 2560x1440 needs
   0x7a3400. Now carved statically at phys 0x08000000 (rmem=96M@0x6000000,
   stream buffers live below 0x7000000) by regtrace_mdns_buf_setup().
   The /proc raw-dump handler's "mode 2" reads reg 0xF040 to reuse this
   buffer — that's how the register was identified.
2. **Comb pointer table.** tisp_mdns_wdr_en is the comb builder (84
   pointers into the par blob; separate linear/WDR offset sets). The
   literal translation dropped a bnez delay-slot store AND exited before
   the entire linear fill; entry [83] lives in the jr-ra delay slot
   (par+1507 linear / par+2387 WDR) and was missed by a first manual
   extraction too — tisp_mdns_intp reads all 84 and oopsed on the NULL
   at vaddr 1 (simple_intp(1, 0, NULL)). Faithfully rewritten in C.
3. **params_refresh return value.** OEM tail-calls memcpy, so it returns
   the dest pointer; treating nonzero as failure made main-init bail with
   -EFAULT right after buffer setup on every boot.
4. **Fragment-chain clobber.** tisp_main_long_tgain_update (live AE event
   path) called the fragment tisp_mdns_refresh, which reads gain state
   from a fake anchor (sclk_name-14128) and re-ran the broken fragment
   reg_cfg with width=0 — zeroing 0xF068/0xF06C and filling 0xF100+ with
   garbage after every gain move. Fragment entries are now no-ops; gain
   tracking runs from the frame-done dns_gain_ev hook via
   tisp_mdns_par_refresh_lit(0, ev, 256) (process context).
5. **Block-init timing.** The tuning blob often arrives after the
   streamon that calls isp_block_init_once, and retriggering via an rvd
   restart crashes the camera (pre-existing second-streamon fragility —
   confirmed by an ENABLE_MDNS=0 control crashing identically).
   isp_block_init_once is now retried from frame-done until the blob
   shows up.
6. **The gate is top40 bit13**, not bit3 (stock top40=0x7fd9004f has bit3
   set too — bit3 is bypassed even on stock and remains blacklisted).
   Bit13 matches the module-control bit13 the OEM buffer ioctl toggles.
   New default top40 = 0x7fef88db; ENABLE_MDNS defaults to 1.

Debug helpers that made this possible: /sys params mdns_dbg_init_ret /
mdns_dbg_reg68 (log-rotation-proof breadcrumbs), tools/phys_memwrite32.c,
and full-unit dumps in docs/extracted/mdns-unit-oem.bin + top-oem.bin
(stock top40/0x20 values, AE/AF stats DMA window at 0x6010-0x601c).

Stock-boot top40 also says bits 4/7/11/15/17/18/21 are active on OEM and
bit20 bypassed — our empirical map disagrees on 15/18/20; worth a careful
re-derivation when chasing the remaining color/LCE deltas.

## 2026-06-10 midday: daylight color — green ceiling fixed, structural delta identified

User compared daylight frames vs stock: stock renders the white ceiling
white AND the green-painted walls green; ours rendered the ceiling green.

Userspace AWB rework (tools/t40_userspace_3a.sh): whole-frame gray-world
fails on this scene (dominant green walls drag the UV mean; neutralizing
it tints the true whites). Now does OEM-style candidate selection — 12
sample blocks; candidates must be >= frame-mean luma, unclipped, and
near-neutral chroma — and uses only the TWO BRIGHTEST candidates
(averaging all candidates re-admits the walls once gains make them
near-neutral: a whitewash feedback loop that overshoots to purple). UV
window env-tunable (UV_LO/UV_HI; 0/255 freezes gains).

Structural finding (full-bank /dev/mem diffs, converged day state, ours
vs stock — docs/extracted/bank{0,1,5}-{ours,stock}-day.bin): WB gains and
the CCM matrix (0xb004-0xb014) are nearly IDENTICAL to stock, yet no WB
point renders both surfaces correctly — the character delta lives in the
blocks our shim never initializes (enable_tisp_main_init_color_inits=0,
fragment inits unsafe): BCSH unit 0x11000 sits at reset defaults (stock
has a configured matrix + hue/sat LUT), CLM unit 0xc000 (101 words
differ), LSC 0x9000 (160), LCE/defog-area 0xa000 (90). Transplanting the
stock BCSH register snapshot does NOT work (greens rotate purple — the
LUT only composes with stock's full pipeline state): added
enable_bcsh_static (default off) with the snapshot for experiments.

NEXT MILESTONE: literal-translate (mips2c) the faithful tisp_bcsh chain
(OEM tisp_bcsh_main_init 0x6a738), tisp_clm_init, and tisp_lsc_init, like
the MDNS/YSP/CCM bring-ups. Stock day-mode register ground truth for all
three is in the bank dumps. Stock day/night switch for reference boots:
`raptorctl ric mode day`.

2026-06-10 update: the faithful BCSH chain is now compiled in as
`driver/t40/tx_isp_t40_bcsh_lit.inc`, gated behind `enable_bcsh=0`
(probe env `ENABLE_BCSH`, `BCSH_MODE=2`). It is a self-contained literal
translation using the OEM T40 BCSH inter table (`.data+0x2cd44`), the
`.rodata+0x2184` hue matrix, explicit CT switch-table repair, and local
`StrenCal` helpers so none of the GP-mangled recovered BCSH functions are
called. Live validation with `ENABLE_BCSH=1` left IRQs/stream healthy and
programmed the BCSH block to the stock day register state except five
EV/CT-dependent words (`0x1104c`, `0x11050`, `0x11064`, `0x1106c`,
`0x11070`); live-writing those five stock values did not materially change
the purple/green cast. Conclusion: BCSH is no longer the blocking delta by
itself; the stock color character depends on the missing CLM/LSC/LCE stack
composing with it.

2026-06-10 update 2: the faithful CLM chain is now compiled in as
`driver/t40/tx_isp_t40_clm_lit.inc`, gated behind `enable_clm=0` (probe env
`ENABLE_CLM`). It uses OEM `stMainClmInterOri` / `stSecClmInterOri`
(`.data+0x2e354` / `.data+0x2c9c0`, 0x1994 bytes each), the decoded
`tisp_ct_clm_interpolation` 5-way jump table (`.rodata+0x1d10`), and a
small `mips2c_literal.py` fix for MIPS `seb`/`seh`. Local T40 module build
passes.

Live result: `ENABLE_BCSH=1 ENABLE_CLM=1 CLM_STAGE_LIMIT=0` reboots during
Raptor start / stream block-init before any frame capture
(`logs/20260610-clm-bcsh-lit-clean`). The camera was restored to the
known-good BCSH/CLM-off stream afterward (`logs/20260610-stable-restore-after-clm`).
The CLM entrypoint now has `clm_stage_limit` (probe `CLM_STAGE_LIMIT`):
`1` stop after allocation, `2` after params_transmit, `3` after blob refresh,
`0` full set_params/hardware writes. Next live CLM work should sweep those
stages first, then split `tisp_clm_set_params_lit` if stage 3 is stable.
