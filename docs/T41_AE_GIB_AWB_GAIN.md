# T41 calibrated metering, black normalization and WB gain writer

These replacements are ordinary C algorithms driven by the active sensor
calibration and completed-frame statistics. No sensor name, sampled tuning
bin, captured gain pair or captured register bank is compiled into them.
The test-only OEM oracle is not part of the driver.

## AE statistics and target

`tx_isp_t41_ae.h` decodes the packed 16-byte zone records into six 15-stride
planes. Grid and allocation bounds are checked before output is changed.
Spatial/foreground weights, dark/bright thresholds and highlight weighting
come from the 0x910-byte AE block. Metering modes 0/1 implement OEM's integer
weighted mean, including its staged fixed-point truncations and histogram
highlight compensation. ROI blending and OEM convergence are not implemented.

The 15 EV knots and targets are copied from calibration, with the optional
calibration target adjustment. Exposure loses its fractional bits before
target interpolation, exactly as OEM does. Equal/reversed knots, unsupported
precision and zero denominators are rejected instead of substituted with a
scene-specific target. Target-table multiplication intentionally retains
OEM u64/u32/u16 truncation and signed division where applicable.

For H20250310a's normal parameter-copy case, runtime +0x68 receives 0x910
bytes from file-body +0xc8 (file +0x108 including its 64-byte header).
That is a module-specific mapping, not a universal offset adjustment.

The AE event thread copies the completed DMA bank into owned storage and
checks its sequence/bank again before accepting it. The IRQ only records
completion; no zone scan runs in hard IRQ. Missing/invalid data holds AE.
The histogram and zone metering share the existing statistics lock.

`OPEN_AE_TARGET` now accepts zero for calibrated automatic metering. GET
returns zero while this mode is selected; a nonzero 0x400..0xffff request
selects the explicit histogram target. The default is calibrated. Diagnostics
`t41_ae_zone_mean`, `t41_ae_calibrated_target`, `t41_ae_meter_error` and
`t41_safe_ae_effective_target_q8` expose calculation and effective target.

The bounded exposure controller still has separate limitations: conservative
step/deadband policy, no full OEM sensor/digital-gain allocation, and a legacy
OS04D10 raw-gain/timing adapter. Calibrated metering is not full OEM AE parity.

## Black-level normalization

Subtracting black level also removes part of the 12-bit range. GIB restores
that range with a rounded Q12 reciprocal, capped at twice unity, and composes
it with per-channel Q10 ISP digital gains. The range selector and packed
15-bit gains preserve OEM rounding and 32-bit product truncation.

The generated self-gain function had lost two arguments and its division.
The interpolation adapter then stored the result in `tparams[4]` rather
than `gib_info + 16`, corrupting unrelated TOP parameters. The dgain writer
also lost its fifth argument and shift-register value. Finally, disabling
the legacy flicker image profile overwrote normalization with unity.
All four faults are repaired. With black=256 and unity ISP digital gain,
the algorithm produces Q12 self-gain 4369 and Q10 channel gains 1092;
these are calculated results, not a bank copied from the camera.

## White-balance conversion and history

`tx_isp_t41_awb_gain.h` implements the gain writer, not illuminant clustering.
It combines calibration gain and RGB coefficients using OEM fixed-point
arithmetic, updates reciprocal-gain reports, handles gain modes 0..9,
selects the three CT-offset regions, and preserves the 200 K offset-history
rule. Freeze suppresses hardware writes, not history updates. Invalid inputs
leave caller state untouched. Both channels clamp independently to 14 bits;
the generated predecessor always selected maximum blue gain.

Both hardware WB banks receive the checked result. The compatibility manual
gain parameters default to zero/zero, selecting calibration instead of the
old 1800/3000 scene seed. The bounded neutral-mesh estimator remains distinct
from full OEM AWB clustering/history; its automatic CT-additive handoff is
not implemented by this initial writer recovery.

## Reproducible arithmetic checks

```sh
sh tools/build_t41_ae_oracle.sh CROSS_PREFIX STOCK_MODULE /tmp/ae-oracle
sh tools/build_t41_gib_oracle.sh CROSS_PREFIX STOCK_MODULE /tmp/gib-oracle
sh tools/build_t41_awb_gain_oracle.sh CROSS_PREFIX STOCK_MODULE /tmp/wb-oracle
```

Run the resulting static MIPS binaries with QEMU or directly on a T41.
The generator requires the exact wrapped H20250310a module SHA256
`572ff4553c1a033290ec67d2d9fc384701fbc235c2e335c7e5604100966fb2ee`.
It relocates reference instructions into private userspace objects; register
writes terminate in arrays. Generated instructions are not committed or
linked into production. AE's unrelated flicker/BV side effects are stubbed;
ROI blending is excluded from the comparison.

Each suite passed 10,000 synthetic cases on QEMU and physical T41 with zero
mismatches. AE varies dimensions, DMA bits, target knots, both mean modes,
weights and overflow boundaries. GIB varies black levels, IR selection and
full-width gain products. WB varies calibration, modes, freeze, CT history
and register clamps, comparing all parameter/runtime/report bytes. Host
boundary/error tests and ASan/UBSan also pass.

## Live verification and comparison caveat

The one-shot combination boots T41NQ/OS04D10 with `ric mode day`, unchanged
sensor calibration and Neo audio. Security policy reports `ae_target=0`;
calibrated mean/target converge near 74/73 in the tested daylight scene.
Six TCP/UDP H.264/AAC reconnect/decode tests pass.

A separate HAL defect invalidated earlier color comparisons: V4L2 exposed
an entire Annex-B access unit as one IDR descriptor, hiding SPS from Raptor's
VUI correction. Splitting actual NAL descriptors changes the open stream's
declaration from limited BT.709 to full-range BT.601, matching its ISP CSC
and the stock stream. Decoding one identical compressed frame correctly
raises bark luma from 35.06 to 46.29 without changing ISP tuning. Correct
metadata before interpreting remaining color differences. Sunlit live
photographs are not controlled color-chart or Delta-E measurements.
