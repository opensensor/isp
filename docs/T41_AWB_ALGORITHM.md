# T41 AWB algorithm recovery

This is a partial algorithm boundary, **not full AWB parity**. The live driver
still uses its bounded neutral-mesh estimator. The portable primitives below
are not connected to its frame lifecycle until clustering, history and
calibration replacement are recovered and checked together.

The reference is H20250310a `tx-isp-t41.ko`, SHA256
`572ff4553c1a033290ec67d2d9fc384701fbc235c2e335c7e5604100966fb2ee`.
Addresses here are ELF `.text` offsets, not the HLIL export's +0x50 labels.
Production headers are ordinary C algorithms. No OEM instructions, live MMIO
snapshots, sensor-bin coefficients or scene-derived corrections are linked
into them. Calibration and statistics are explicit caller inputs.

## Recovered boundaries

| OEM function | ELF offset | Portable operation |
| --- | --- | --- |
| `tisp_awb_get_statistics` | `0x30540` | `t41_awb_statistics` |
| `tisp_awb_sat_weight` | `0x3106c` | `t41_awb_saturation_weights` |
| `tisp_awb_long_par_update` | `0x30b20` | `t41_awb_prior_prepare` |
| `tisp_awb_spec_calculate` | `0x31fb4` | `t41_awb_special_prepare` |
| `ISPAWBInterpolation1/2`, `Tiziano_Awb_Ct_Cal` | `0x1a570`, `0x1a624`, `0x1a774` | `t41_awb_ct_lerp`, `t41_awb_ct_calculate` |
| `Tiziano_Awb_Ct_Detect_GrayWorld_mode` | `0x1aaac` | `t41_awb_grayworld_mode` |
| `tisp_awb_long_alogrithm` | `0x31238` | `t41_awb_long` (explicit detector callback) |

AWB parameters start at active calibration +`0x978`; runtime state is `0xf54c`
bytes. This is a format boundary shared by T41 sensor calibrations, not a
special case for OS04D10. The runtime owner must privately copy calibration
before calling parameter-mutating algorithms.

Statistics have four luminance-class pairs per zone, with global and neutral
selections in even and odd records respectively. A 16-byte record contains
four 22-bit sums and a 14-bit count. Normal mode consumes 128 bytes per zone;
selected-class mode consumes 32, retaining other classes' previous state.
Each class has five 225-word planes, with fixed 15-zone row stride even when
the configured width is smaller. Global planes start at `0x7f94`, neutral at
`0x3944`; class stride is `0x1194`. Padding and unrelated history are untouched.
The four hardware DMA banks are temporal banks, not extra color phases.

Saturation weighting uses the count gate at parameter +0, precision at
`0xcd2`, and mode at `0xd56` (both **u16**, not byte controls). RGB is adjusted
by supplied previous gains, with OEM low-u32 multiply then right shift.
Mode 1 computes minimum/maximum; other modes use minimum/mean. Empty, gated
or zero-minimum zones get weight 1. A wrapped zero mean divisor is rejected
before modifying any output. Both paths keep OEM shifts/truncations.

The CT prior uses EV thresholds `0x1c/0x20`, three four-knot tables at
`0xb0/0xc0/0xd0`, enables/floors at `0xe0..0xec`, and precision at `0xcd4`.
Transition span is one third of the EV-threshold difference. Knot interpolation
uses low-u32 products and unsigned division; floor interpolation uses signed
division. Disabled day/night priors select/relax the calibrated tables as the
OEM does. Invalid enable values and unsafe EV ranges are rejected. The helper
also builds the complete 45-pointer estimator view using native-sized
pointers; it does not serialize host pointers into the 32-bit hardware ABI.

Special-illuminant regions use ten mode bytes at `0x11f6`, with immediate
handling for slots 0 and 5..9. Region zero waits for the `0x34de/0x34df`
two-frame readiness gate before consuming a caller snapshot of RGB statistics
`0x180ac/b0/b4`. Green zero produces zero ratios, not a divide fault. Mode 1
generates ordered writes for the shared region-pair registers; other nonzero
modes prepare calibrated software ratios. Register writes are returned as
address/value pairs: the helper itself performs no MMIO. Neighbouring regions
share packed words, so whole-word writes and order are significant.

Temperature calculation clips calibrated R/G and B/G to their 15-knot axes,
then interpolates the reciprocal-temperature surface at `0x870`. The first
pass shifts table values into the fractional domain; the second does not
shift them again. Both preserve the OEM intermediate fixed-point multiply/
divide truncations. A zero interpolated reciprocal returns 5000 K. The last
axis endpoints are evaluated directly, avoiding unnecessary one-past-axis/
surface accesses in the OEM while preserving their valid-input result.
Malformed axes and zero interpolation divisors are rejected. This is the
ratio-to-temperature calculation, not the cluster selection producing ratios.

Gray-world fallback tries the neutral selection, then the global selection
only if the first remains failed. Spatial calibration weights are multiplied
by four for ratios inside both calibrated axes. An optional second pass
weights squared distance from the first mean using the caller-owned generic
distance LUT and its quantized tail. Its input is four dense zone planes;
distance scratch retains the hardware row stride of 15. Empty first-pass
support returns unity and 5000 K. Empty refinement marks failure but retains
the first mean and its calculated temperature, matching the OEM's final
publication rather than its overwritten intermediate assignment. This is
fallback behavior, not the main cluster detector.

The long-frame pipeline combines four luminance classes with calibrated Q3
weights, optionally overriding one class from runtime special-region state.
It prepares calibrated R/G and B/G ratios, raw report ratios and global RGB
means. The global ratio mean is accumulated **before** count gating; this
ordering matters to the settled-state change detector. Saturation weights,
the 16-bit wrapped change comparison and event-flag mask retain OEM behavior.

An explicit detector callback supplies ratios and temperature. Cold history
fills all 15 entries and applies one-frame convergence. Subsequent frames
average the last 1..15 entries with their absolute history-position weights,
then apply the calibrated start/restart thresholds and signed gain slew.
Stable adjacent ratios and completed gains allow the detector to be skipped
on following unchanged frames. Malformed dimensions, zero calibration factors,
zero inverse-gain divisors and an invalid transition counter are rejected.
The entry point takes aligned, privately owned working state; callers must
discard that working state on error, not publish partial changes. This is
not yet the live ownership adapter or freeze/manual/day-night policy.

## Validation and reproduction

10,000 randomized combined cases match all changed parameter/state bytes,
all 45 normalized pointers, every ordered register write and the conditional
RGB reads on QEMU and the physical T41: zero mismatches or unexpected accesses.
The extended run also checks 10,000 temperature calculations, including
clipping, exact upper endpoints, zero surfaces and changing precision.
It also checks both gray-world selection passes, optional distance refinement,
zero support, arbitrary distance LUT entries and unchanged-output skip paths.
Inputs cover both
DMA modes, all four selected classes, changing dimensions, saturation modes,
all day/night-enable combinations, prior interpolation and special-region
warmup/software/hardware modes. No sensor bin is an oracle input.

The host suite tests 2,000 randomized frames with ASan/UBSan, padding and
canaries, explicit malformed lengths/dimensions, and rejection of a wrapped
zero saturation divisor without partial output. Full `make -C tests check`
also passes. The sanitizer suite includes constant/zero temperature surfaces,
upper endpoints and malformed-axis rejection without changing the output.

```sh
bash tools/build_t41_awb_stats_oracle.sh CROSS_PREFIX STOCK_OBJECT OUTPUT_DIR
qemu-mipsel OUTPUT_DIR/awb-stats-oracle-check
make -C tests check
cc -std=c99 -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined \
  -fno-sanitize-recover=all tests/tx_isp_t41_awb_stats_test.c -o /tmp/awb-stats-test
/tmp/awb-stats-test
```

The oracle generator relocates selected OEM functions into a private
userspace fixture. Only the test executable contains reference instructions.
It has no ISP access; its register interface is checked in memory.

The separate long-pipeline oracle compares 20,000 frames in 200 independent
100-frame sequences, including 9,500 settled detector skips: all parameter,
state and report bytes and callback counts match on QEMU and the physical
T41, with zero unexpected accesses. It deliberately replaces the OEM cluster
detector with a synthetic callback and supplies identical callback outputs
to native C. This proves the surrounding pipeline, **not cluster detection**.
Calibration, counts, RGB sums, dimensions, precision, history lengths, class
overrides, saturation modes, failed detections and event flags vary. No sensor
bin or scene data is used. A separate 2,000-frame ASan/UBSan suite verifies
history/slew examples, padding, canaries and malformed-input rejection.

```sh
bash tools/build_t41_awb_long_oracle.sh CROSS_PREFIX STOCK_OBJECT OUTPUT_DIR
qemu-mipsel OUTPUT_DIR/awb-long-oracle-check
cc -std=c99 -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined \
  -fno-sanitize-recover=all tests/tx_isp_t41_awb_long_test.c -o /tmp/awb-long-test
/tmp/awb-long-test
```

## Still required

Full CT clustering, freeze/manual/day-night policy, and owned frame-safe runtime with lifecycle
and calibration replacement tests. Existing gain packing and CT-offset
history are separately validated in `T41_AE_GIB_AWB_GAIN.md`; that does not
establish correctness of the estimator feeding them.
