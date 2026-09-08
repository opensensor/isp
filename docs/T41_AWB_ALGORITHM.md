# T41 AWB algorithm recovery

The recovered automatic estimator, frame owner and public controls are now
persistent on the test camera (kernel `fe620b59`, OpenIMP `851486f`). This is
not a claim of full image-quality, WDR or every-sensor parity.

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
| `tisp_awb_ct_detect` | `0x1b210` | `t41_awb_detect`, cluster and refinement helpers |

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

Distance-LUT initialization is a compact mathematical equivalent of the
**universal algorithm table**, not a sensor calibration extraction:
`min(256, round(319 * exp(-16*d/2025)))` for distances 0..513. The production
initializer uses a Q24 recurrence, with no floating point, allocation or
stored 514-entry coefficient array. The exponential representation is
inferred from the universal reference table and verified exhaustively against
all 514 independent entries on QEMU and the T41. Its Q24 decay multiplier
is `round(2^24 * exp(-16/2025)) = 16645178`. The explicit quantized tail is
retained separately: distance 614, for example, has weight 3 rather than
the exponential's rounded value 2. Host sanitizer checks cover short buffers,
canaries and tail boundaries; the full earlier 10,000-case oracle still passes.

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
the arithmetic boundary; the live owner and control policy are described below.

The full detector has three calibrated modes: base zone weighting, cluster
reweighting, and ranked-illuminant selection. It consumes the supplied ratio
planes, count/saturation weights, spatial weights, inclusion/exclusion
centres, CT priors and distance LUT. It builds a 14x14 histogram on the
15-knot axes, preserves first-scanned ties, then selects twelve occupied
cells. Mode 1 seeds five points per cell, iterates centroids, merges nearby
ones and reweights zones by calibrated support. Mode 2 uses cell means,
merges/ranks twelve clusters and selects/blends up to six illuminants using
calibrated temperature and count curves. Optional variance refinement and
gray-world fallback retain their separate control paths.

Details that affect parity include the upper/upper zone prior's **unrounded**
integer reciprocal (final CT publication rounds), dense ratio/count inputs
versus 15-stride scratch and some spatial-weight passes, fixed Q6 cluster
report rounding, and separate original support counts versus compacted
centroids during selection. Mode-2 special exclusions must not inherit the
mode-1 hard-radius rule from retained cluster scratch. The standalone
detector preserves supplied scratch between calls; its lifecycle is not
assumed to clear it every frame. These are generic format/arithmetic rules,
not corrections for a particular sensor or scene.

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

The detector oracle includes the actual OEM detector and all of its math
callees, **not a synthetic detector callback**. An initial 10,000 cold-state
run passes, followed by 10,000 frames with scratch retained in 500 twenty-frame
sequences. The final warm-state run matches every parameter/state byte,
ratio, failure flag and cluster report on QEMU and the physical T41: zero
mismatches or unexpected accesses. It includes 2,803/1,487/2,616 successful
results in modes 0/1/2, 3,094 failed results and 1,154 nonempty cluster reports.
Precision, axes, maps, dimensions, modes, exclusions, refinement and blending
policies vary. The 10,000-frame native-only host build passes ASan/UBSan,
calibration-write checks and malformed length/alignment/axis rejection. The
earlier 10,000-case statistics/CT/gray oracle still passes after sharing the
surface interpolation helper. Full host suite passes.

```sh
bash tools/build_t41_awb_detect_oracle.sh CROSS_PREFIX STOCK_OBJECT OUTPUT_DIR
qemu-mipsel OUTPUT_DIR/awb-detect-oracle-check
cc -std=c99 -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined \
  -fno-sanitize-recover=all tests/tx_isp_t41_awb_detect_test.c -o /tmp/awb-detect-test
/tmp/awb-detect-test
```

The combined algorithm oracle then connects DMA parsing, prior/special-region
preparation, the real detector, long history/convergence and gain publication.
It compares 10,000 frames across 100 independent 100-frame sequences, with
selected/normal DMA modes, retained state, all estimator modes, gain modes
0..9, freeze/unfreeze, EV changes and face-region warmup. QEMU and the physical
T41 both report zero mismatches in complete parameter/state/report buffers,
cluster reports and ordered special-region/WB writes (including both WB
banks and their triggers). The native-only connected pipeline also passes
10,000 frames under ASan/UBSan and calibration-write checks. This includes
gain conversion and manual/freeze behavior, not just the earlier detector
callback seam. It does not exercise the live IRQ/worker ownership adapter,
the mode-setting API, cold allocation or calibration replacement.

```sh
bash tools/build_t41_awb_frame_oracle.sh CROSS_PREFIX STOCK_OBJECT OUTPUT_DIR
qemu-mipsel OUTPUT_DIR/awb-frame-oracle-check
cc -std=c99 -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined \
  -fno-sanitize-recover=all tests/tx_isp_t41_awb_frame_test.c -o /tmp/awb-frame-test
/tmp/awb-frame-test
```

## Initialization and controls

`tx_isp_t41_awb_control.h` provides privately owned parameter/state/report
storage, mathematical distance-kernel initialization, exact cold defaults,
geometry refresh, EV-weight interpolation and control transitions. The
parameter block is **0x12e4 bytes**, not the complete 0x1700-byte tuning
readback: the latter includes a 0x41a-byte report and two padding bytes.
Native readback zeros the padding that OEM leaves uninitialized.

The real OEM initialization and control instructions match 100 cold starts
and 10,000 control transitions on QEMU and the physical T41, comparing every
parameter, state, report and control byte plus getter outputs. The external
allocator, hardware writer, gain writer and callback registration are mocked
in this **control-policy oracle**; it is not a live lifecycle test. The actual
gain writer and algorithm connections are covered by the preceding frame
oracle. Native control tests pass ASan/UBSan, short-buffer/mode/geometry
rejection and padding checks. Full host suite passes.

Covered transitions include all ten gain modes, freeze, manual CT, spatial
weights, statistics location, CT offsets, convergence settings, EV changes,
parameter replacement, both refresh flags and day/night refresh. Mode zero
retains manual control gains; day/night refresh preserves the distinct
reset-inhibit flag. Manual-mode preset constants are the OEM public API
defaults, not sensor-specific auto-WB coefficients. Invalid hardware geometry
and nonmonotonic EV knots are rejected. Setters run against an owned candidate;
late validation failure requires discarding it before any MMIO/publication.

```sh
sh tools/build_t41_awb_control_oracle.sh CROSS_PREFIX STOCK_OBJECT OUTPUT_DIR
qemu-mipsel OUTPUT_DIR/awb-control-oracle-check
cc -std=c99 -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined \
  -fno-sanitize-recover=all tests/tx_isp_t41_awb_control_test.c -o /tmp/awb-control-test
/tmp/awb-control-test
```

## Still required

The adapter in `tx_isp_t41_awb_runtime.inc` is connected in the candidate
kernel. Its actual C also runs in a host harness with mocked IRQ,
workqueue, allocation and register access. ASan/UBSan tests cover private
calibration, 100 complete frames, freeze/manual controls, calibration refresh,
failed-candidate rollback, allocation unwinding, duplicate/late IRQs,
stop-before-free and all 55 power-management register pairs. No sensor
calibration bytes are modified. Layout changes are rejected with `EBUSY`
while streaming; the parent must stop hardware before releasing DMA memory.

The shared `t41_awb_frame` helper used by the adapter passes the complete
10,000-frame OEM oracle again on QEMU and the T41, plus native ASan/UBSan.
The new `t41_awb_hardware` helper separately matches 10,000 complete OEM
geometry/threshold transactions, including ordered triggers, freeze and ready
flags, randomized gain tables and all parameter/state bytes on both targets.
Unlike the control-policy oracle, that test executes the actual OEM hardware
writer and interpolation; only MMIO itself is redirected into checked memory.

```sh
sh tools/build_t41_awb_hardware_oracle.sh CROSS_PREFIX STOCK_OBJECT OUTPUT_DIR
qemu-mipsel OUTPUT_DIR/awb-hardware-oracle-check
cc -std=c99 -O1 -g -no-pie -Wall -Wextra -Werror -fsanitize=address,undefined \
  -fno-sanitize-recover=all tests/tx_isp_t41_awb_runtime_test.c -o /tmp/awb-runtime-test
/tmp/awb-runtime-test
```

The candidate exposes the read-only `OPEN_AWB_OWNER` handshake (`0x08ff0006`,
token `0x41574201`). OpenIMP enables kernel auto-WB only on that exact response;
legacy unknown-control acknowledgements cannot accidentally select it. With
the native owner, the security daemon no longer scans statistics or publishes
manual gains and its default status-only feedback interval is 1000 ms instead
of 40 ms. Explicit nondefault intervals and old-kernel fallback are retained.
OpenIMP policy tests cover readiness failures, fallback and zero competing
statistics/gain calls; the T41 test suite passes ASan/UBSan.

The adapter serializes legacy writer/control entry points too. The eight-byte
Open manual extension retains exact Q10 hardware gains by freezing only WB
publication while estimation continues. Start-gain updates are stored as an
owned day-bank override, restored only on day refresh, never written into the
installed calibration. Host tests cover manual/auto handoff, read-only
observation, day/night start override and legacy writer dispatch.

The outer public attribute/control dispatcher now routes the 76-byte AWB
attribute, 225-byte weights and 2700-byte zone report to the same owner.
The old generated dispatcher silently acknowledged these requests. Mode,
optional start-gain publication, freeze/CT and API bookkeeping retain OEM
ordering; the whole candidate commits only after validation. Statistics use
a checked heap buffer, not a large kernel-stack object. Unsupported directions,
invalid pointers/modes and premature access return errors. Public wrappers
match another 10,000 transitions in the control oracle on QEMU and physical
T41, including complete getter and day-start readbacks. Gain arithmetic is
the independently verified shared writer; the control oracle mocks that seam.
Host adapter tests cover late failure without MMIO/state/day-override changes.
`tests/t41_awb_device_check.c` passes on the live candidate: public manual
mode/gains, freeze, weights, zone reports, invalid modes/directions/pointers
and restoration of the original attributes/weights. The kernel and library
cross-builds, complete host suite and adapter/control sanitizers pass.

The preceding `f15bee01` kernel with OpenIMP `851486f` passed a one-shot boot
on T41NQ/OS04D10, followed by forced `ric mode day`, six TCP/UDP H.264/AAC
reconnects and a 120-second decode (2968 video frames, no decoder/timestamp/
timeout warnings). Native AWB rejected no frames, and all scalar-block error
counters remained zero. Exact-Q10 manual gains held while estimation continued;
auto resumed and the security profile was restored. Sensor calibration, Neo
audio and saved Raptor configuration remained unchanged. The unarmed next
boot returned to the persistent stack before the stock one-shot was applied.

In the stationary indoor comparison, native/stock hardware red gains were
1484/1484 and blue gains 3344/3336. Paper ROI R/G was 0.9612/0.9702, B/G
1.0688/1.0874 and luma 217.03/215.02; bark luma 67.29/66.47. These are
uncontrolled scene observations, not chart-based correction coefficients.
Short CPU samples still favor stock, and delivered cadence is below the
configured 25 fps. Neither efficiency nor whole-ISP parity is claimed.

Revision `fe620b59` passed the public ioctl smoke, six TCP/UDP reconnects,
an exposure sweep to realized log2-Q16 gain 259142 and a clean 120-second
H.264/AAC decode (2998 frames). AWB rejected no frames; all checked scalar
errors were zero. Security policy was restored. Promotion changed only the
ISP module, libimp and tuning daemon, before S10 bind mounts. All 20 persistent
hashes and the four-entry previous-stack rollback manifest verified on reboot,
followed by forced day and another six passing reconnects. The module SHA256
is `802943b7a138a64078838d6694dce9082997df061ad086f498d122272a6001a2`.

A 60-second RTP probe before the exposure sweep measured 24.723 media fps,
15 roughly two-frame timestamp intervals, continuous packet sequences and no
backward timestamps. A separate 20-second probe after the sweep measured
24.986 media fps and no long intervals. This exposure-dependent difference
is under investigation, not a fixed-cadence claim. A 900-second promoted
stream test is running; its result is not yet established here.

`tisp_params_copy` copies AWB
from day-bank `0xd18` to active `0x978`; thus the OEM start setter's day-bank
`0xd24..0xd30` addresses correspond to active AWB targets/current gains. This
is a packed-bank mapping, not a different auto-WB algorithm or sensor hack.
Existing gain packing and CT-offset history are separately validated in
`T41_AE_GIB_AWB_GAIN.md`; estimator coverage is the connected oracle above.
