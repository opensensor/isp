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
- `tisp_main_init_top40_value = 0x7fd9004f`

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

BCSH H-matrix registers live at `0x8024-0x8038` in the T31 map. Identity
matrix values are not a valid RGB-to-YUV conversion and can produce bad color.

The T40 profile currently forces a CSC version path, but that is not proof that
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
   - `0x4800` and nearby DMSC registers
   - `0x8024-0x8038` BCSH H-matrix range
   - `0x1030-0x1070` GIB range
2. Keep broad DMSC/BCSH/GIB snapshots default-off during normal RTSP runs.
   Audit one register window at a time, preferably before stream start or in a
   bounded smoke run.
3. Capture the current bad-color baseline with safe registers in the same log.
4. Treat the initial Bayer sweep as done for `0x10008-0x1000b` plus
   `0x10002`; do not repeat unless other path state changes.
5. Build a minimal top-bypass profile:
   - keep stream/stat plumbing alive
   - keep DMSC active if required for output format
   - bypass GIB, LSC, denoise, sharpen, defog, ADR/WDR, and other enhancement
     blocks initially
6. Re-enable blocks one at a time from the minimal baseline:
   - DMSC refresh
   - Gamma/CSC/BCSH
   - LSC
   - GIB
   - denoise family
7. Only after a plausible lifelike frame appears, map T40 tuning-blob offsets
   for the active blocks and replace guesses with blob-backed values.

## Success Criteria

A passing frame is not just a large JPEG or changing frame. It must show:

- recognizable scene geometry
- plausible neutral colors
- no stable green/magenta blobs
- stable output after at least 90 seconds of RTSP capture
- matching register/proc evidence for the tested tuning state
