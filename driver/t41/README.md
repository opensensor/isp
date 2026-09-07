# Ingenic T41 ISP Recovery

Latest tuning status: [the September IQ audit](../../docs/T41_STOCK_OPEN_IQ.md)
and [scalar TMO recovery](../../docs/T41_TMO_ALGORITHM.md). The older bring-up
profiles below are historical, not claims of generic tuning support.

This directory contains the recovered T41 TX-ISP driver baseline for the Wyze
Cam v4 T41NQ/OS04D10 Linux 4.4.94 target. It was generated from the OEM
`tx-isp-t41.ko` whose SHA-256 is
`2426eea9c3c8268373eccc3c3753424f0429ff02044e4ddd4e86c6caa40a6338`.

The source was salvaged from the June 2026 `tx-isp-t41-v1` reconstruction
workspace. All 1,510 function candidates were emitted and 1,508 compiled in
isolation, but only 157 were normalized exact matches. A later manual cleanup
made the whole driver build. It now has staged hardware coverage through the
OS04D10 sensor and Raptor consumer, but it remains recovery-grade code rather
than a production-equivalent replacement.

## Build

Build against the matching Thingino Wyze Cam v4 output:

```sh
ROOT=/home/matteius/thingino-firmware-opensensor/output/master/wyze_cam4_t41nq_os04d10_atbm6062s-4.4.94-uclibc \
KDIR="$ROOT/build/linux-2aca1252ac4a304172b870777365f42bfb100674" \
SOC=t41 ./build_local.sh
```

Expected artifact:

- `driver/t41/tx-isp-t41.ko`

The source remains named `tx_isp_t41_recovered.c`, but Kbuild emits the
canonical module name expected by current T41 sensor modules
(`depends: tx-isp-t41`).

The module is linked from nine logical objects:

- `tx_isp_t41_recovered.c` — recovered core, pipeline, hardware, and tuning
- `tx_isp_t41_daynight.c` — T41 adapter for the shared day/night state machine
- `tx_isp_t41_math.c` — T41 ABI wrappers around shared math primitives
- `tx_isp_t41_sinfo.c` — T41 layout adapter around the shared sensor registry
- `tx_isp_t41_subdev.c` — extended T41 graph-layout adapter around the shared
  name/type/index pad resolver, link-state core, and checked remote-event
  route resolver
- `tx_isp_t41_tuning_abi.c` — shared tuning-envelope and command-descriptor
  implementation
- `tx_isp_t41_frame_layout.c` — shared checked NV12 geometry and vendor
  aggregate-line semantics used by set-format and QBUF; it also imports the
  shared 68-byte frame-buffer ABI, 116-byte extended frame-format ABI, and
  generation-specific flag policy
- `tx_isp_frame_channel.h` supplies the proven common event IDs and T41's
  exact private-`T` ioctl envelopes to the recovered dispatcher and IRQ paths
- `tx_isp_subdev_abi.h` names the pad/link fields used by remote event
  dispatch and frame-channel callback installation, and supplies the shared
  T23/T41 recovered link-detach prefix while retaining link state
- `tx_isp_t41_exposure.c` — T41 sensor/color adapter around the shared checked
  anti-flicker exposure planner
- `tx_isp_t41_scaler.c` — exact T41 coefficient-table adapter around the
  shared checked polyphase scaler generator

## Baseline risk

The recovered module is a bring-up artifact, not a production-ready driver.
The current linked-binary audit finds 18 stub functions, 91 collapsed
functions, 386 shorter functions, and 41 OEM-only symbols. Critical deficits
remain in core control/ioctl dispatch and tuning paths.

Completed static repairs restore all 31 OEM exports and replace the recovered
IRQ wrappers that previously requested IRQ 0 with null handlers and disabled
IRQ 0 regardless of their arguments. The linked audit now classifies the IRQ
enable/disable helpers as shorter rather than stubs, and the request/free paths
as similar in instruction count.

The T41 module now reuses the common day/night state machine,
interpolation/fixed-point helpers, checked exposure and scaler arithmetic, and
the T23/T31/T41 typed sensor-registry implementation. Full sensor/Raptor smoke
tests pass, both MSCA streams run, and ISP interrupts advance. The static AWB
safety fallback is now `R=1800, B=3000`; both shadow banks retain those values
across forced day/night transitions. It runs with the exact active OEM OS04D10
day CCM and unity GIB. The older half-GIB/custom-CCM combination remains
available only as an explicitly selected experiment. The shared
registry reports one active OS04D10 with chip `0x530444`, address `0x3c`,
native 2560x1440 geometry, and 25 fps; both driver-add and sensor-bind report
one successful lifecycle call.

The optional T41 safe-AWB path now consumes all eight packed 16-byte records
in every 128-byte DMA zone group. The earlier record-zero sampler appeared
valid in one spatial phase but changed its ratios by several times when the
hardware advanced the ring, producing red/green flashes. Runtime writes of a
negative value to `t41_safe_awb_controller` rearm the one-shot statistics
engine; disabling it cancels the worker and immediately restores the calibrated
fallback. Read-only parameters expose the selected bank, raw ratios, applied
gains, IRQ/update counts, and rejected samples. A 12.5% consistency gate rejects
partially replaced DMA groups while accepting a persistent illuminant change
after 32 samples. On the mixed daylight/incandescent test scene the controller
held `R=1438..1439, B=3819..3824` across bank transitions while rejecting the
transient groups that previously pulled either channel down by hundreds.
The aggregate controller remains opt-in: in the August 6 full-daylight scene
its current gray-world-to-gain mapping settled near `R=1127, B=1518` and made
the frame green. The fixed mixed-light stock snapshot (`1240/4624`) was also
not a safe daylight fallback: the neutral ceiling measured `U/V=186.05/79.28`.
A two-axis live sweep selected `1800/3000`, measuring `126.97/129.33` on the
same ceiling region and reducing its average saturation from `75.31` to `2.02`.
This closes the severe daylight color regression while the larger OEM AWB
model-selection path is recovered; it does not claim dynamic-AWB parity.

A later matched stock/open capture on August 6 bounded the live-profile state
more precisely. OEM AWB moved from `1484/3516` to `1476/3524` over 20 seconds;
the latter pair is now the deterministic security seed. Replaying the remaining
different GIB/CCM/BCSH writer words did not close the observed luma gap. Raising
the open safe-AE target from `14500` to `17600` did: whole-frame normalized Y
moved from `0.381` to `0.453`, versus `0.450` on stock. Two open-extension
tuning controls now switch the bounded AWB mode/gains and AE target in process
context, without stopping V4L2 capture or the encoder. Policy names and presets
remain in OpenIMP userspace; the experimental aggregate AWB controller remains
explicitly opt-in.

A matched nighttime oracle showed why that daylight seed cannot be the whole
security policy. Stock converged around `1240/4692`, lowered scene luma, and
selected a different 29-word `tisp_bcsh_write_reg()` bank. Replaying only the
gains left open too warm and saturated; replaying the writer-owned low-light
bank plus the matched `15800` AE target closed normalized YUV from stock
`109.568/116.295/138.708` to open `109.546/116.765/137.548`. A third open
tuning command now switches the complete day/low-light color-model bank live.
OpenIMP owns gain-threshold hysteresis and the sensor-specific trims; the
driver owns only the validated register transaction.

The August correctness cycle also restores the T41 1.2 tuning responses used
by RIC: running-mode GET, AE expression, the 256-bin/225-zone AE statistics
envelope, and AWB global statistics. Their packed layouts are implemented in
the common tuning-ABI unit and covered by host tests rather than being open
coded in the recovered ioctl dispatcher. The earlier AE target `17600`, profile
`1`, and the old AWB baseline measured
`Y/U/V/SAT=117.125/119.855/132.521/29.133`, versus
`118.832/119.857/132.143/25.161` on stock. After the aggregate-AWB correction,
the calibrated fallback measured `U/V=125.51/128.88` and the adaptive path
settled at `126.45/128.13`; the controlled stock oracle was `127.66/125.97`.
An in-place AE sweep selected the new `14500` default: it measured
`Y/U/V/SAT=97.54/126.96/128.42/30.57` versus the same-scene stock luma
`99.76`, retained the stock-like 737-line shutter, and reduced the flat-ceiling
standard deviation from `0.0444` at target `16000` to `0.0344`.
The open driver and OpenIMP ran together with no visible block corruption or
H.264 decode errors in repeated frames. All 15 host suites pass; the active
one-shot module SHA-256 is
`cf838ed8f4dfc612389fdee5aeb2377a16636818298b6087da278025a55e8814`.

The July 30 shared-format validation preserved the 3,133,440-byte 1080p pool,
full-rate output, and coherent geometry across two clean boots. It decoded
150 frames in six seconds and 125 in five, passed night/auto/day transitions,
and left the open module active with zero ISP interrupt errors. The validated
module SHA-256 is
`64e7103e81765b5c72a42088bb7b46a12cea1538aae21a9d3cfe4f802ffbc5da`.

The shared frame-channel contract now names the set/get-format, stream,
queue, and completion events plus every T41 private ioctl envelope. Its
REQBUFS path also consumes the common five-word count/type/memory positions
while retaining T41's local allocation, queue-free, and remote-event policy. The
deterministic sequential build's fail-safe boot reports both 1920x1080 and
640x360 streams at 25 fps and saved a fresh 379,001-byte main-stream JPEG.
ISP IRQs advanced, forced day mode applied, and kernel, Raptor, and Android
fault scans were clean. The active module SHA-256 is
`cf79410cf59b3140b4df3eb67b00d69f6fa99e6455b2181ad220cc1d5e73129a`.
The scene was darkening during a storm, so this cycle validates transport and
ABI behavior rather than brightness or noise parity.

The common graph resolver now owns the endpoint name/type/index search while
the T41 adapter retains the graph table at `0x3c`, extended pad slots, and
recovered pointer checks. Its setup path consumes the common 8-byte endpoint,
20-byte graph-link, and 8-byte record-set wire positions. Its one-shot boot bound
the OS04D10 once, applied
day mode, advanced both ISP cores with `error=0`, and decoded 150 1920x1080
frames in six seconds. `dmesg`, `logread`, and `logcat` fault scans were clean.
The tested module remains active with SHA-256
`34aaccbd070010bc704e9588182bfb9f3302a416bcd871111c53f82ae235cd1e`.

Link setup now delegates link-type/stream-state validation and the symmetric
source/sink record write to the common subdevice library. T41 retains
generation-local conflict teardown calls and their recovered ordering. The
one-shot candidate bound the OS04D10 once, accepted forced day mode, advanced
both ISP cores with `error=0`, and decoded 150 1920x1080 frames in six
seconds. `dmesg`, `logread`, and `logcat` fault scans are clean. The candidate
remains active with SHA-256
`7f385cf872d21568dc105b85ca852a4f33a5ade30c72f52451acd880dfeee9d4`.
The scene was darkening during a rainstorm, so this gate covers graph, stream,
and ABI behavior rather than comparative brightness or shadow-noise quality.

Remote-event dispatch now delegates the local-pad, active-sink, and handler
lookup to the shared checked resolver while retaining T41's pointer policy,
event diagnostics, and callback invocation. The fail-safe cycle registered
and bound the OS04D10 once, accepted forced day mode, advanced both ISP cores
with `error=0`, and decoded 151 1920x1080 frames in six seconds. Early kernel
capture shows repeated resolved remote callbacks returning zero, and the final
`dmesg`, `logread`, and `logcat` scans contain no driver faults. The candidate
remains active with SHA-256
`07166ce513a884029b90c3250976da957fd4d6439932cbc17ef67f4946a3f7ae`.
Changing storm light excluded visual-quality comparisons from this gate.

The OEM `check_state` queue/state policy is now shared with T23 while T41
retains its `0x1fc` queue and `0x224` byte-state positions. This replaces the
recovered unconditional-zero body with the OEM decision. The fail-safe boot
bound the OS04D10 once, accepted day mode, advanced both ISP cores with
`error=0`, and decoded 151 1080p frames in six seconds without driver faults.
The candidate remains active with SHA-256
`adb6e008067e4e44f909d6695a6a3f53ac87355e9d5c8766f6d1a5b3e3e84f96`.

The same cycle validates the shared NV12 DMA binding plan in the recovered
late-link queue path. It decoded 150 main frames in six seconds, 75 substream
frames in three, and 103 main frames in the four-second final check without
rejecting a live buffer or changing stream geometry. Later live tuning fixed
the pink-red error and 120 Hz recessed-light shadow cycle with the shared
exposure planner plus the T41 GIB/CCM adapter. The scaler investigation then
found that the recovered path programmed 2560x1440-to-1920x1080 ratios while
leaving ratio-specific hidden filter RAM uninitialized. The shared fixed-point
generator now reproduces the stock unity curve exactly and emits the missing
0.75x Lanczos coefficients. A clean boot logged two successful exact-stock
curve commits, held 1920x1080 at 25 fps, and restored visibly sharper detail
without changing the accepted color profile. The validated module SHA-256 is
`687a90404e6d204e2030c381df4ae1890dfefd1333244115f55ce0f448c35376`.

The shared T41 math adapter now also owns `fix_point_div_32`, backed by the
common OEM-compatible divider while preserving its wrapped 32-bit quotient and
remainder behavior. The hardware cycle held 1920x1080 at 25 fps through a
forced day transition, logged zero ISP interrupt errors, and preserved the
accepted color, clarity, and lighting profile. The validated module SHA-256 is
`27cb2801e4a0b9609b2db175acdfc39be2ab5e2b1ab63e6d956e4574793833d4`.

The common library and T41 math adapter now also own the 32-bit fixed-point
add/subtract entry points, retaining T41's underflow diagnostic locally.
These entry points currently have no live callers. A clean boot decoded 150
frames in six seconds and recorded 125 in five, preserved the accepted
red/color and sharpness profile, passed night/auto/day, and logged zero ISP
interrupt errors. The active module SHA-256 is
`8ff2f59a2c2e555d88b2a9bd7d5f08911b2e84be33a0e9e75c4af1fdafc61a2c`.

The SDK-declared 64-bit add/subtract entry points now live in the same adapter
and delegate to common unsigned wraparound primitives. The recovered six-word
forms represented the MIPS O32 expansion of `(pointpos, u64, u64)`; keeping the
official C signature in the adapter makes that ABI explicit while retaining
T41's underflow diagnostic. These entry points currently have no live callers.
A one-shot boot decoded 151 frames in six seconds, passed night/auto/day,
advanced both ISP cores with `error=0`, and preserved the accepted red/color
and sharpness profile. No driver fault appeared in `dmesg`, `logread`, or
`logcat`. The active module SHA-256 is
`11c850f8dc83fecaeda9fba627e5c1bbeb5db4f77bf851b34adbd4a8432a161f`.

The unsuffixed SDK add/subtract pair exposes the same `(pointpos, u64, u64)`
contract and now lives beside the `_64` pair in the adapter. Both symbol names
are retained for vendor ABI compatibility; neither pair currently has a live
caller. The one-shot boot passed night/auto/day, decoded 126 frames in the
five-second final check, advanced both ISP cores with `error=0`, and logged no
driver fault. The active module SHA-256 is
`c60fddd5000f96a7bfea49e67f059d5417e79e7643395cfbfc093f269186414b`.

The signed 64-bit rounding entry point now lives in the math adapter and uses
the exact behavior recovered directly from both stock modules: arithmetic
shift by `p`, then add bit `p-1`. This replaces a recovered T41 body that added
the parity bit of the already-shifted result. The helper is live throughout
gamma, CSC, color, and filter coefficient generation, so this boundary
requires same-scene image validation in addition to the normal smoke cycle.
The candidate has the stock 140-byte routine size, decoded 152 frames in six
seconds, passed night/auto/day, and advanced both ISP cores with `error=0`.
The post-boot frame retained the accepted sharpness and color profile, while
`dmesg`, `logread`, and `logcat` showed no driver fault. The active module
SHA-256 is
`1aed03add34f3e213e39a654c0389603c23eecf2e39f6d1c9455df87248fbda6`.

The two-/three-operand unsuffixed and `_64` fixed-point multiply entry points
now live in the math adapter as official `(pointpos, u64, ...)` MIPS O32
interfaces. Their common limb implementation retains the full low word of the
128-bit product after the requested Q-format shift without compiler 128-bit
support. The active candidate decoded 152 main frames in six seconds and 75
substream frames in three, passed night/auto/day, kept `rvd` responsive, and
reported zero ISP interrupt errors. Its module SHA-256 is
`03e9f68a3740c03f51caae9269fa339d8d971d2c8566b1a8463daf7ffe1f904e`.

The matching unsuffixed and `_64` divide entry points now use a common
full-domain shift/subtract divider. This replaces one recovery-grade body and
one literal jump-to-null stub without introducing `__udivdi3` or 128-bit
compiler helpers. Ten thousand randomized host cases match a `__uint128_t`
oracle. The fail-safe device boot decoded 150 main frames in six seconds, 77
substream frames in three, and 125 post-day frames in five; night/auto/day,
`rvd`, and both ISP cores remained healthy with `error=0`. The validated
module SHA-256 is
`bc1a798d0dac2909657b87f11f65938aa1af849d0a12291bf58fcc3e1e010100`.

The integer/fixed 32-bit and 64-bit log2 family now lives in the math adapter
and delegates to the same normalized-square primitives already used by T23
and T31. Randomized host coverage verifies both integer and fixed wrappers.
Because these entry points are live in gain, color, and filter calculations,
the device gate included image inspection: the accepted red/color and edge
profile remained intact. The cycle decoded 150 main frames in six seconds, 75
substream frames in three, and 125 post-day frames in five, with successful
night/auto/day and zero ISP errors. The validated module SHA-256 is
`c8c94049aaf125798f1496e3b00dcac70f08c63461c5533349b747220b00a6ba`.

The complementary `tisp_math_exp2` entry point now delegates to the common
33-node Q30 lookup/interpolation helper, which rejects invalid precision or
table ranges instead of inheriting masked MIPS shifts. The active tuning paths
use valid Q5/Q16 inputs. The device cycle decoded 150 main frames in six
seconds, 75 substream frames in three, and 125 post-day frames in five;
night/auto/day, `rvd`, image color/detail, and zero-error ISP interrupts all
held. The validated module SHA-256 is
`b55cc521890fe4f37d4b19f67d59113784ed176e40fd7ab11a4e2e95a6044741`.

The live DMSC/SDNS `tisp_ratio` strength blend now comes from the common
three-segment Q7 primitive. All 256 ratio values are host-tested across both
increasing and decreasing output ranges. T41 must keep this entry in its
original recovered text slot: moving the body shifted unresolved indirect
targets and made Raptor stop during `IMP_ISP_AddSensor`. The shared return-body
form preserves the complete accepted `.ko` byte-for-byte. With
`tx_isp_bringup_level=3` retained in the one-shot argument file, the device
again passed night/auto/day, decoded 151 main frames in six seconds and 75
substream frames in three, kept both ISP IRQs advancing with `error=0`, and
showed no driver fault in `dmesg`, `logread`, or `logcat`. The validated hash
therefore remains
`b55cc521890fe4f37d4b19f67d59113784ed176e40fd7ab11a4e2e95a6044741`.

The OEM-derived leading-bit helpers now return their computed positions rather
than zero, and `private_copy_from_user` once again uses the kernel's checked
copy path. This restores a prerequisite for fixed-point tuning math and for the
many ioctl paths that consume userspace structures.

The complete T41 fixed-point log helper family was restored from the local
Ingenic Linux 4.4.94 T41 SDK source. Four helpers now have exact OEM
instruction-count parity and the 64-bit log conversion is within one
instruction of OEM.

The corresponding internal TISP fixed-point log family now returns its
calculated values instead of zero. The 32-bit integer helper is within one
instruction of OEM and both exported conversion wrappers are classified as
similar by the linked audit; the 64-bit integer helper is semantically restored
but remains shorter than OEM and needs runtime coverage during tuning bring-up.

Twelve more vendor shim functions covering I2C, GPIO input, module references,
completion handling, and interruptible timeout waits were restored from that
same SDK source. Each now has OEM instruction-count parity.

The CPM reset helper now polls the OEM `0xb00000c4` register for all 500
iterations and performs both completion writes at that same address. Recovered
pointer types had shortened the poll to 125 iterations and redirected the
completion pulse to `0xb0000310`; the repaired helper is within three
instructions of OEM.

The six fixed-point add/subtract entry points now implement the OEM 32-bit and
64-bit arithmetic instead of returning zero or one input operand. All three
add helpers have exact instruction-count parity; the subtract helpers preserve
OEM unsigned wraparound and underflow diagnostics and are classified as
similar.

The 64-bit rounding, signed minimum/maximum, and three-operand 32-bit multiply
helpers now preserve their full return values. The multiplier and min/max
helpers have exact OEM instruction-count parity. A decompiler-only pair of
`muls_dp_*` calls in AWB distance calculation was also replaced with explicit
64-bit squares, eliminating two symbols that the target kernel cannot resolve.

The ISP-core pad event handler again accepts its data argument and dispatches
events `0x03000001` through `0x03000008` to the seven OEM callback slots (with
the OEM no-op sixth event). Missing callbacks return `-1`; disabled pads and
unknown events remain no-ops.

The media-bus Bayer writer now preserves the upper bits of ISP register
`0x88`, clears its low five bits, and applies the OEM `{1,3,2,0}` Bayer-order
mapping across codes `0x5200` through `0x5213`. Unsupported formats retain the
OEM diagnostic/no-op behavior.

The exports-only (`-1`) hardware smoke level inserts and removes cleanly on the
Wyze Cam v4 target. The first shallow-platform (`0`) run inserted successfully,
created `/dev/tx-isp`, and reached the recovered probe, but exposed word-scaled
decompiler offsets in `tx_isp_remove` during unload. The parent teardown now
uses the OEM byte offsets `0x0c` and `0x88`, preserves the proc pointer at
`0x138`, and guards an absent parent device before deregistration.

A second level-0 run passed that parent teardown but exposed heap corruption
when the kernel later freed module metadata. The recovered child-platform loop
had advanced 32 bytes instead of the OEM 8 bytes and called a math helper in
place of each child driver's remove callback. The repaired loop visits all 16
entries and invokes the callback at driver offset `0x04` before unregistering
each platform device. This repair requires a fresh level-0 hardware retest
before advancing to the device graph.

The next level-0 run reached the restored core remove callback and showed that
it unconditionally deinitialized TISP even though levels below 3 deliberately
skip TISP initialization. Core removal now mirrors that bring-up gate, while
still deinitializing the registered subdevice, and passes the OEM channel-buffer
and core pointers to the two recovered no-argument frees. This repair also
requires a fresh level-0 hardware retest.

The fourth level-0 run completed core teardown and reached CSI removal. It
exposed another scaled pointer (`0x40` instead of the OEM byte offset `0x10`)
and missing iounmap, resource-release, and free arguments. The CSI path and the
same statically visible VIN, IVDC, and frame-source teardown defects are now
restored from OEM disassembly. The VIC path was normalized at the same time.

The fifth level-0 run removed every driver object but detected a kernel bug
while the module's vmalloc area was released. The common subdevice destructor
had discarded its misc, heap, ioremap, and memory-region arguments, and its
clock-release helper became an infinite loop whenever clocks were present.
Those paths and the collapsed module deinitializer are restored from OEM
control flow. The smoke harness now also treats `Kernel bug detected` as a
fatal signature.

The sixth level-0 run is clean: module insertion, `/dev/tx-isp` creation,
parent/child removal, and module unload all return zero with no kernel fatal
signature. This clears the shallow-platform gate for device-graph testing.

The first level-1 run then showed that the graph was incomplete: IVDC probe
rejected zero input/output pad counts and graph creation reported that subdev
index 4 was missing. The common subdevice initializer was only 65 instructions
against the OEM's 420 and skipped IRQ, MMIO, clock, and pad setup. Its restored
implementation is now 306 instructions, the clock initializer is 132 against
OEM 136, and the module initializer is 48 against OEM 51. This lowers the
linked audit's collapsed count from 92 to 91.

Activating the recovered pad setup exposed two additional dormant teardown
chains at level 0. Frame-source probe used the address of its pad-pointer field
as the pad array, freed its live channel array on the success path, and used a
scaled loop index. The repaired probe is 204 instructions against OEM 206.
The VB2 queue cancel/free chain also unlocked a null spinlock and used scaled
list offsets; queue cancel now has exact OEM instruction-count parity and
queue free is within two instructions. Finally, child teardown is bounded by
the six platform entries actually registered instead of walking ten unused
slots. The resulting level-0 run at
`logs/20260720-2015-t41-level0-platform-table-dump-117` inserts and unloads
cleanly with no kernel fatal signature, clearing the updated initializer for a
fresh level-1 graph test.

The fresh level-1 run at
`logs/20260720-2019-t41-level1-restored-graph-117` inserts cleanly and no longer
reports the missing IVDC subdevice. Core, VIC, and IVDC IRQs 39, 38, and 21 are
registered, and `/dev/misc-ivdc` is now present alongside `/dev/tx-isp`.

Static review held level 2 because the recovered private-memory initializer
aliased unrelated IVDC and frame-counter globals and both mutex wrappers used a
null lock. The restored allocator uses the OEM 20-entry block pool, the
reserved ISP base/size, and a real mutex; it includes split and coalescing
paths for later core use. `isp_mem_init` is now 47 instructions against OEM
40, `find_new_buffer` is 32 against OEM 37, and `isp_free_buffer` remains 67
against OEM 70. The level-2 run at
`logs/20260720-2025-t41-level2-private-memory-117` inserts cleanly with no
kernel fatal signature, clearing private-memory setup for level-3 static
review.

Level-3 static review then found that core probe passed the clock-name table as
its subdevice operations and initialized the core spinlock and mutex at
word-scaled offsets beyond the 880-byte allocation. The corrected shallow path
uses `core_subdev_ops` and byte offsets 276 and 280. The level-0 regression at
`logs/20260720-core-offset-fix-level0-117` inserts and unloads cleanly with no
kernel fatal signature. The still-guarded level-3 channel and tuning path
requires separate reconstruction before it is enabled on hardware.

The tuning object on that guarded path also initialized its lock and mutex at
word-scaled offsets, installed two unrelated recovered constants instead of
the tuning file operations and event callback, omitted the ISP debug-node open
callback, and freed no object during deinitialization. Those fields now match
the OEM byte layout and ownership flow. `isp_core_tuning_init` is classified
similar at 53 instructions against OEM 60, while `isp_core_tuning_deinit` has
exact 18-instruction count parity. This repair is compile- and audit-verified;
hardware coverage remains coupled to the pending core-channel restoration.

Core probe now builds its 232-byte channel records and embedded normal/IR
callback tables with OEM byte offsets and named function relocations. It also
restores channel dimensions and capability flags, pad event ownership, the
tuning/debug node hooks, and failure cleanup. The linked probe is classified
similar at 289 instructions against OEM 321. The insertion-only level-3 run at
`logs/20260720-level3-core-channels-117` registers AISP, creates `/dev/isp-m0`,
keeps core/VIC/IVDC IRQs 39/38/21 active, and reports no kernel warning or fatal
signature. Live unload and a sensor/Raptor consumer remain gated on repair of
the shorter recovered TISP deinitializer and review of the stream-init path.

Stream-init review found that several OEM firmware objects had been recovered
as single words even though the code indexes or clears them as structures and
per-channel arrays. The active storage now has the OEM symbol-table extents,
including 504-byte TISP parameters, 2,656-byte scaler state, 78-byte scaler
channel state, 7,744-byte event state, two-entry parameter/day/night tables,
and the corresponding callback, histogram, sensor-control, lock, completion,
and scratch objects. This prevents `tisp_init` from overwriting neighboring
globals before the stream path is enabled. The level-3 regression at
`logs/20260720-level3-tisp-storage-117` again registers cleanly with no warning
or fatal signature.

Hardware smoke tests must use a one-shot boot stage, capture kernel and
userspace logs, and reboot after each experiment. Do not install this baseline
into the persistent module tree or hot-unload an active camera pipeline.

`tools/open_tx_isp_boot_once_init.sh` is the fail-safe init hook used for full
consumer tests. It removes and syncs its armed marker before inserting the
staged module, so a watchdog or power-cycle loads the untouched persistent
module on the next boot.

Use the staged smoke harness after boot-time stock loading has been disabled:

```sh
T41_BRINGUP_LEVEL=-1 tools/t41_smoke_cycle.sh
```

The levels are intentionally incremental: `-1` exports only, `0` performs a
shallow platform probe, `1` adds the device graph, `2` adds ISP memory setup,
and `3` enables the recovered core/tuning path. The legacy harness refuses to
run if any `tx_isp*` module is already loaded and reboots the camera after
every experiment; use the one-shot init hook when the stock boot sequence
cannot safely unload its active module.
