# T41 calibrated AWB comparison, 2026-09-07

## Follow-up: algorithm recovery, not register replay

The original stack is **not a generic completed T41 tuning implementation**.
It contains OS04D10/scene-specific bring-up register replays. A register bank
matching stock on one scene does not establish that its generating algorithm
has been recovered. Reading the active sensor calibration as algorithm input
is different from embedding a captured output bank in the driver.

This follow-up replaces the AWB setup replay with checked register packing
and gain interpolation. The old generated writers lost the second argument
to `system_reg_write(0x18004, value)`, scaled several byte offsets as pointer
offsets, discarded packed LWL/LWR threshold loads, and lost the integer part
of gain in the first threshold interpolation. Sources: H20250310a
`tisp_awb_set_hardware_param` 0x2f9c4, `set_regional_threshold` 0x2f744,
`set_lum_th_freq` 0x2f8d0. The legacy `t41_stock_awb_stats_profile` selector
now controls a calibration-driven refresh, not a captured six-word array.
The DMA ring is not reset during that post-stream refresh.

The portable AWB writer helper takes calibration bytes and log2 gain as
inputs. Tests use asymmetric synthetic geometry and ascending/descending
gain tables, including unaligned input, malformed fields and high-gain
clamping. Independently feeding the captured stock parameter block and
gain=1555 Q16 reproduces all eleven stock runtime thresholds; those captured
values are not compiled into the implementation.

The bounded AWB estimator now keeps odd/neutral records separate from the
even/global estimator records and applies Q3 luminance-class weights to both
RGB and pixel counts. It interpolates the calibration's reciprocal-temperature
mesh and EV-dependent CT selection prior. This remains a bounded estimator,
not a recovered OEM clustering/history implementation. The CT tests include
two different illuminant/EV calibrations; there is no OS04D10 gain correction
in the new shared math. The current adapter still requires full-statistics
mode; unsupported compressed layouts fail validation.

TMO initialization had a separate base-pointer bug: three late accesses were
relative to the whole parameter object rather than its TMO block at +9272.
This mis-seeded runtime controls and clamped an unrelated parameter at +480.
The corrected base follows `tisp_tmo_init` 0x6b170/0x6b290/0x6b2ac/0x6b2c4.
Mode-0 EV curve interpolation now validates all calibration knots and uses
the final curve above the highest knot, instead of the penultimate curve.
Mode 1 is explicitly unsupported, not silently treated as mode 0.

The base correction alone does **not** repair adaptive local TMO. Stock's
3750-word local map is scene-dependent; open's cold map is zero because its
unsafe FPGA/statistics worker is suppressed. A diagnostic uniform-coordinate
map avoided washout, but it was a substitute for that missing algorithm.
The proposed automatic fallback and its helper were removed from this patch.
The captured TMO replay is disabled by default. TMO stays bypassed after
stream reset, so shadow/highlight recovery remains unfinished.

Remaining sensor-specific scaffolding includes the captured CCM path,
BCSH day/low-light banks, DMSC static gain profiles, DPC/CDNS/MDNS/spatial/LCE/
ADR profiles, fixed initial AWB seeds, and OS04D10 raw-gain interpretation in
the safe AE adapter. These are not covered by the new generic-math tests and
must not be advertised as multi-sensor tuning support. Replace each at its
calibration/statistics-to-register boundary; do not add more scene captures
as production profiles.

The one-shot candidate booted in forced day mode, with R/B settling at
2116/2278 Q10 in this changing daylight scene. Full host tests and AWB/TMO
helper ASan/UBSan tests pass. Three TCP and three UDP H.264/AAC reconnects
pass. A 60-second TCP decode observed 1498 video packets, no non-increasing
PTS and no decoder/timestamp warning. This is not a color-chart or long-run
stability certification. Device validation remains T41NQ/OS04D10 only.

A fresh nearby stock capture gave paper R/G=0.963 and B/G=1.133, versus
open's 0.878 and 1.178. This is closer than the preceding open comparison,
but residual cyan remains and illumination was not controlled. The bark ROI
luma was 52.66 stock versus 11.63 open: the tone deficit is still substantial.
Do not present the AWB correction as an image-quality-parity result.

The reproduced intermittent FFmpeg null-sink timestamp warning was output
time-base quantization: monotonic 90 kHz input timestamps rounded into the
same tick at the inferred 12/301 output time base. With
`-fps_mode:v passthrough -enc_time_base:v demux`, both the 30-second repeat
and the 60-second candidate run had no warning. Do not rewrite capture/RTP
timestamps to conceal this harness artifact. Gaps up to 10807/90000 seconds
were still observed in the candidate; their cause remains unresolved.

A subsequent persistent-stage reconnect also reproduced an **audio** warning
(`stream 1`, PTS 15360 -> 15269 at 16 kHz), and a debug repeat showed
15360 -> 15350. This is distinct from video output-time-base quantization.
Raw RTP was monotonic in a separate capture; independently rebased audio and
video origins disagree when their RTCP reports synchronize the tracks.
Raptor's [per-client timeline fix](https://github.com/matteius/raptor/commit/227a5bf)
corrects this separately using a shared capture origin. Six TCP debug runs
and three TCP plus three UDP reconnects passed without backward PTS or
timestamp warnings; an RTP-Info/RTCP probe found both origins within 3 ms.
Do not describe the video harness change alone as fixing the audio defect.

The sections below describe the preceding comparison, before this follow-up.

Hardware: T41NQ, OS04D10, 2560x1440/25, Linux 4.4.94. Genuine H20250310a
stock ISP and stock IMP were compared with the open V4L2 stack. Both used
the installed sensor module and identical sensor IQ binary, forced optical
day mode, disabled anti-flicker, and 4 Mbps H.264. No firmware was flashed.
The stock path required IVDC/direct mode and one encoder stream buffer to
fit the existing 20 MiB reserved-memory pool. This is an ISP-stack comparison,
not a comparison with an entire vendor firmware image.

## AWB defect and correction

The open security controller averaged all AWB records and multiplied the
result by fixed red/blue biases. The resulting B gain was about 3186 Q10,
versus stock's roughly 2340. Applying stock gains manually removed most of
the purple/blue cast while leaving the shadow-detail deficit unchanged.
Readable CCM coefficients matched stock, so they were not changed.

The new read-only `OPEN_AWB_TARGET` control uses a bounded neutral-mesh
estimator. The existing `OPEN_AWB_SCENE` raw whole-scene-ratio ABI is unchanged.
OpenIMP uses the new estimate without applying its legacy fixed biases again,
holds the last gains on `ENODATA`, and retains legacy behavior only when the
kernel does not implement the new control. Unknown open-extension controls
and unsupported directions now fail explicitly.

Source evidence: H20250310a `tisp_awb_long_par_update` at 0x30b70,
`tisp_awb_long_alogrithm` at 0x31288, `tisp_awb_ct_detect` at 0x1b260,
`tisp_awb_set_gain` at 0x2fd58. File AWB offset +3352 maps to runtime +2424.

| AWB-relative offset | Data used |
| --- | --- |
| 0x30, 0x34 | Sensor ratio-calibration coefficients, Q10 |
| 0x38, 0x74 | Fifteen R/G and B/G axis knots, Q8 |
| 0x4ec | 15x15 neutral-selection weight mesh |
| 0xc6e, 0xc72 | Active statistics grid dimensions |
| 0xd46 | Four configured luminance-class weights |
| 0x1200 | Spatial zone weights |

The four DMA banks are temporal buffers. Each zone has two records for each
of four luminance classes. The adapter combines configured classes, excludes
unused allocation tails, then supplies RGB sums to the shared checked mesh
math. It rejects out-of-model colors instead of clamping them onto a neutral
edge. Minimum accepted samples/pixels are required. The shared math has no
OS04D10 gain constants and allocates no memory.

This is deliberately **not a complete OEM AWB reconstruction**. CT estimation,
clustering/history, EV-dependent class selection, and CT-dependent additive
gain corrections remain unrecovered. In particular, the nearby +0xcde/+0xce0
words were not verified as active stock gain biases and are not used. An
early diagnostic candidate used them; it is not the final build. Other SoCs
can reuse the math after providing their own validated statistics/calibration
adapters. Only this T41/OS04D10 configuration has device validation.

Final open gains settled near R=1947/B=2407. Stock in the nearby comparison
was approximately R=2096/B=2340. The large blue error is substantially reduced,
but a smaller cyan/green error remains. Do not claim stock color parity.

## Tone processing remains unresolved

Open's TMO is deliberately bypassed after stream start. Gamma EV refresh alone
made no visible change. Both full TMO EV refresh and the older curve-only
stock replay washed out the current scene. The captured curve, spatial map,
runtime controls and live exposure do not form a valid matched state after
the open stream transition. Both diagnostics were reset by reboot; the final
build leaves TMO bypassed. No new static scene curve was installed.

The final backlit-scene comparison still shows crushed bark/leaf shadows and
more clipped highlights in open. Repair the complete TMO/LCE initialization
and statistics-to-runtime update boundary before enabling that block. Do not
mask the mismatch with sensor-specific RGB or contrast register overrides.

## Validation and limits

Shared math tests cover invalid/nonmonotonic calibration, out-of-model and
zero-weight samples, interpolation boundaries, insufficient evidence, and
maximum accumulation without overflow; ASan/UBSan pass. ABI and full ISP host
tests pass. OpenIMP tests cover legacy no-op ioctl replies, unsupported
controls, malformed targets, and hold-last-gains behavior.

Native H.264/AAC decoding and three TCP plus three UDP reconnect sessions pass
on the final candidate. No kernel fault was recorded. CPU measurements are
workload-dependent (roughly 9-11% total across two cores during these short
runs), not a controlled efficiency A/B or a long-term leak test. Night,
artificial illumination, color-chart accuracy and other sensors are untested.

Captures, SHA256 manifests, exact experiment configurations and logs are in
the companion `t41-stack-20260907/iq/` work directory. `stock-final/frame.png`
and `open-final/frame.png` are the final nearby-in-time pair. The read-only
ROI script measures fixed paper/bark patches, not calibrated Delta-E.

Two additional observations are not fixed here: Raptor acknowledges pipeline
startup after a stock frame-source allocation failure because it ignores
`fs_enable_channel`'s return; stock and open startup/attachment logging must
be distinguished from steady-state decoder faults. Stock AWB-global values
are R/G and B/G statistics, not the applied gain registers; preserve that
telemetry distinction.
