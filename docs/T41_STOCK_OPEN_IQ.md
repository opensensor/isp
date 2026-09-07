# T41 calibrated AWB comparison, 2026-09-07

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
