# Cross-SoC Driver Reuse Plan

## Scope

The active refactor covers the working T23, T31, and T41 TX-ISP drivers. T31
already has core, CSI, VIC, VIN, frame-source, tuning, and support translation
units. T23 and T41 still have large recovered core translation units, but both
modules now link separate shared-library adapter objects. This gives later
extractions stable module boundaries without rewriting the recovered pipeline
all at once.

T40 remains a working recovered baseline, but it is intentionally outside this
round of cleanup.

The goal is one implementation where behavior is genuinely common, with small
per-SoC adapters for recovered object layouts, register maps, capabilities,
tables, and sequencing. Matching symbol names are only candidates for reuse;
OEM evidence and working-device behavior remain authoritative.

## Shared Code Landed

### Arithmetic primitives

`driver/include/tx_isp/tx_isp_math.h` contains kernel-independent primitives
for:

- signed and unsigned Q16 interpolation
- typed 8-bit and 16-bit table interpolation
- unsigned 32-bit fixed-point add/subtract with natural wraparound
- two- and three-operand unsigned 32-bit and 64-bit fixed-point multiplication
- T23-compatible split multiplication with wrapped 32-bit partial products
- OEM-compatible wrapped and full-range unsigned 32-bit fixed-point division

The per-SoC wrappers preserve their existing ABI and endpoint policy:

- T23 uses signed interpolation with its eight-step table endpoint and
  delegates its add/subtract and generation-specific wrapped multiply/divide
  entry points.
- T31 delegates its 32-bit fixed-point multiply wrappers to the common helpers.
  Its native 64-bit algorithm is available and tested in the common header,
  but the recovered AE translation unit retains an identical local inline body
  until an extra wrapper no longer changes its machine-code register allocation.
- T41 uses the unsigned 8/16/32-bit interpolation variants with its ten-step
  endpoint and delegates its fixed-point add/subtract, multiply, and
  wrapped-divider wrappers.

`tests/tx_isp_math_test.c` covers boundary behavior, OEM rounding, wrapped
32-bit add/subtract and products, T23's split-product equivalence over Q1-Q31,
Q0-Q63 64-bit products, typed tables, and randomized equivalence checks.

### Day/night transition shell

`driver/common/tx_isp_daynight.c` owns mode validation, pending-state
publication, normal versus fill-only transitions, CSC writes, and optional
shadow commit. `driver/include/tx_isp/tx_isp_daynight.h` defines a callback-
and-register configuration rather than selecting a SoC at runtime.

T31 and T41 provide thin adapters with their own mode identifiers, register
maps, frame-drop handling, tuning notifications, and commit behavior. T23
retains its larger recovered refresh sequence until that ordering can be
represented safely.

### Ordered register profiles and masks

`driver/common/tx_isp_reg_profile.c` applies ordered register/value lists with
an optional final commit. T31 uses it for the device-derived SC2336 DMSC
correction profiles that repair the previously solarized/false-color output
after day/night refresh.

The same unit provides the exact recovered flag-to-register merge used by
T23's day/night, custom-mode, and tuning-bin top-bypass updates. The common
operation replaces three local copies without changing transition order or
tuning values.

`tests/tx_isp_reg_profile_test.c` covers write order, commit placement,
validation failures, mask replacement, non-boolean recovered semantics, and
the 32-bit count limit.

### Ordered callback plans

`driver/common/tx_isp_callback_plan.c` validates an entire callback list before
executing it in declaration order. T23 declares its 17-block mode refresh as a
plan in `tx_isp_t23_mode.c`; T31 declares its 18-block day/night refresh plan
next to the private tuning callbacks it adapts.

This keeps the algorithms and exact order local while sharing validation and
dispatch. Pre-validation is important for hardware plans: a malformed entry
cannot leave the ISP half-programmed. `tests/tx_isp_callback_plan_test.c`
covers ordering, empty plans, malformed plans, and validation atomicity.

### Proprietary tuning wire ABI

`driver/common/tx_isp_tuning_abi.c` and
`driver/include/tx_isp/tx_isp_tuning_abi.h` define the byte-stable libimp
control envelopes, expression/EV/WB reply layouts, sparse EV offsets, pure
response packers, and validated per-SoC command descriptors.

This is deliberately not one cross-SoC ioctl switch. T23, T31, and T41 retain
their own dispatch and hardware collectors, while sharing layout checks and
scalar-versus-userspace-pointer metadata. T41's moved frame-rate/running-mode
IDs remain generation-qualified. `tests/tx_isp_tuning_abi_test.c` covers every
wire size/offset, descriptor direction and duplicate validation, sparse-input
bounds, narrowing semantics, and the three reply packers.

### Checked NV12 and MDNS layouts

`driver/common/tx_isp_frame_layout.c` owns checked single-plane NV12 geometry:
aligned Y stride/height, the private ABI's aggregate 12-bpp line size, Y-plane
size, UV offset, and complete sizeimage. Each adapter passes its proven
alignment policy, so the helper shares arithmetic without pretending
T23/T31/T41 alignment registers are identical.

T23 uses it for MSCA format reporting and QBUF UV placement, T31 core uses it
for frame-channel normalization, T31 outer format handling uses it to preserve
the OEM aggregate-line ABI without multiplying the total size twice, and T41
uses one result shape for both set-format and QBUF.

The same unit owns the checked T23/T31 MDNS auxiliary layout: NV12 working
image, four compressed-reference banks, two UV banks, and the final tiny
image. Its explicit memory-option policy reproduces the recovered bank
aliasing without moving MMIO writes or allocation ownership into common code.
T23 retains page padding in its 12-byte allocation ABI; T31 retains its exact
used-size eight-byte ABI. `tests/tx_isp_frame_layout_test.c` covers the active
1080p/360p geometries, both MDNS memory policies, exact offsets and sizes,
differing alignment policies, invalid alignment, and every 32-bit overflow
boundary.

The same interface now binds NV12 geometry to a supplied DMA buffer. It
validates allocation length and the complete 32-bit address range before
publishing Y/UV addresses. All three adapters use this plan before their local
QBUF hardware handoff. Their queue objects, locks, late-link replay, rotation,
and completion semantics remain intentionally separate.

### Sensor registry

`driver/common/tx_isp_sinfo.c` owns the sensor-registry lifecycle, exported
sensor-driver/bind functions, slot ownership, and `/proc/jz/sensor` files.
`driver/include/tx_isp/tx_isp_sinfo.h` owns the shared interface.

The recovered sensor objects do not share a C layout, so each consumer supplies
one typed `struct tx_isp_sinfo_config`:

- T23 supplies fixed SC2336 metadata plus lifecycle callbacks that retain its
  recovered sensor-client creation and cached-state side effects.
- T31 supplies the recovered client, attribute, dimensions, fps, and adapter
  byte offsets.
- T41 supplies its larger recovered layout and enables the extended sensor
  attributes.

This is source sharing rather than a second runtime module: each TX-ISP module
continues to own and export the ABI expected by its matching sensor module.

T23 and T31 have full live coverage with their SC2336 sensors, including name,
chip ID, I2C address, 1920x1080 dimensions, 25 fps, active state, and advancing
ISP interrupts.

T41 now has full live shared-registry coverage with the OS04D10 sensor.
`/proc/jz/sensor/count` reports one; the driver-add and sensor-bind counters
each report one success; and `sensor0` reports `os04d10`, chip `0x530444`,
address `0x3c`, native 2560x1440 geometry, 25 fps, and active state.

### Checked ABI and recovery support

`driver/include/tx_isp/tx_isp_subdev_abi.h` records reviewed physical pad and
generation-specific subdevice offsets. T31 compile-time assertions cover the
confirmed named fields and common pad prefix.

`driver/include/tx_isp/tx_isp_recovered_kernel.h` holds the already-reviewed
kernel-tree compatibility prelude used by recovered sources. Keep larger
freestanding recovery-tool declarations local until their signatures are
verified.

### Canonical T41 artifact

T41 Kbuild now emits `driver/t41/tx-isp-t41.ko`, the canonical dependency name
used by current T41 sensor modules, while retaining
`tx_isp_t41_recovered.c` as the source filename.

T41 is now a multi-object module with six explicit boundaries:

- `tx_isp_t41_recovered.c` owns the recovered pipeline, hardware, and tuning
  implementation.
- `tx_isp_t41_daynight.c` adapts T41 registers and callbacks to the common
  transition state machine.
- `tx_isp_t41_math.c` preserves the recovered math entry-point ABI and
  delegates its algorithms to `tx_isp_math.h`.
- `tx_isp_t41_sinfo.c` supplies the T41 object-layout adapter for the common
  sensor registry.
- `tx_isp_t41_tuning_abi.c` links the shared proprietary control ABI.
- `tx_isp_t41_frame_layout.c` links checked frame-channel NV12 geometry and
  vendor aggregate-line reporting.

### Multi-object T23 artifact

T23 preserves the deployed `tx_isp_t23_recovered` module identity while linking
eight logical objects:

- `tx_isp_t23_core.c` owns the recovered pipeline, hardware, tuning, and the
  T23-specific sensor lifecycle callbacks.
- `tx_isp_t23_math.c` preserves the recovered interpolation entry-point ABI
  and delegates its algorithm to `tx_isp_math.h`.
- `tx_isp_t23_sinfo.c` supplies static metadata and lifecycle callbacks for the
  common sensor registry.
- `tx_isp_t23_mode.c` owns the T23 bypass masks and one authoritative
  declarative 17-block mode-refresh sequence.
- `tx_isp_t23_callback_plan.c` links the common validated callback runner.
- `tx_isp_t23_reg_profile.c` links the shared ordered-profile and register-mask
  implementation.
- `tx_isp_t23_tuning_abi.c` links the shared proprietary control ABI.
- `tx_isp_t23_frame_layout.c` links checked frame-channel NV12 and MDNS
  geometry.

## Device Validation

Every staged module was loaded through the one-shot fail-safe hook, exercised
through the real Raptor consumer, and checked with stream captures plus
`dmesg`, `logread`, and `logcat`. Final device boots were re-armed with the
tested open build so the current work remains active for inspection.

| SoC | Staged coverage | Result |
|---|---|---|
| T23 | eight-object module, shared math/registry/register-mask/callback-plan/tuning-ABI/frame/MDNS layout plus mode adapter, SC2336 | pass; exact `0x477e70` MDNS use/`0x478000` allocation, day/night/auto, and full-rate RTSP clean |
| T31 | shared math/registry/day-night/profiles/callback-plan/tuning-ABI/frame/MDNS layout, SC2336 | pass; corrected 3,133,440-byte pool geometry, unchanged `0x2f8740` memory-optimized MDNS allocation, and expected half-rate RTSP clean |
| T41 | six-object module, shared math/registry/day-night/tuning-ABI/frame-layout, OS04D10 | transport/ABI pass; complete registry parity, correct 3,133,440-byte main pool, and full-rate RTSP; mixed-light color and anti-flicker tuning remain open |

The one-shot loader in `tools/open_tx_isp_boot_once_init.sh` consumes and syncs
its marker before `insmod`. A crash therefore cannot repeatedly load the staged
module: the next watchdog or power-cycle returns to the persistent driver.
Live unloading is not a safe test strategy for these camera pipelines. The
latest detailed matrix is in `docs/SHARED_DRIVER_LIBRARY.md`.

The latest validation also covers the proprietary tuning ioctl ABI:
WB/highlight/backlight readback, hue, AE compensation, total gain, expression
line timing, exposure microseconds, and AE luma on T31, plus T41's typed
frame-rate and running-mode startup controls. The ioctl command set mixes
scalar and pointer payloads; that distinction is now explicit in common
descriptors instead of inferred from numeric command ranges.

## Target Layout

```text
driver/
├── include/tx_isp/       # SoC-independent reviewed interfaces/primitives
├── common/               # Shared kernel implementation
├── t23/                  # T23 registers, resources, tables, quirks
├── t31/                  # Split T31 implementation and adapters
├── t40/                  # Deferred recovered baseline
└── t41/                  # T41 registers, resources, tables, quirks
```

A common helper should accept explicit generation data or a small typed
callback surface. It should not discover the SoC at runtime or grow a web of
`#ifdef Txx` branches.

## Keep Per-SoC Until Proven Otherwise

- physical addresses and register offsets
- IRQ masks, acknowledge order, and error recovery
- clock/reset names and enable ordering
- packed or offset-addressed recovered structures
- stream-start and sensor handoff quirks
- tuning tables and calibration payloads
- block presence, bank count, and interpolation endpoint count
- code still represented by unresolved recovery fragments

T31's file boundaries are a useful decomposition guide, not automatically the
canonical behavior. T23 and T41 may expose older or newer semantics that need
to shape the eventual common interface.

## Safe Extraction Rules

1. Compare parameter types, call sites, return values, side effects, and object
   layout across every intended consumer.
2. Preserve the existing exported or per-driver entry point as a thin wrapper
   during the first move.
3. Express generation differences as explicit data, offsets, or callbacks.
4. Add host tests for pure code and build all three active modules for every
   shared change.
5. Run a full consumer smoke test on each affected SoC before declaring a
   kernel-facing extraction complete.
6. Restore and verify the persistent module after each staged experiment.
7. Do not mix a structural extraction with tuning-value or register-sequence
   changes.

## Next Candidates

1. Extend validated callback plans to other repeated initialization and
   teardown sequences whose ordering is already device-proven.
2. Review fixed-point divide and log/exp helpers shared by T31 and T41, starting
   with host-testable functions that cannot touch kernel or ISP state.
3. Reconcile the known T31 pad-direction and 0x24/0x28 stride discrepancy in a
   standalone, device-tested change.
4. Extend the checked ABI from pads to links, events, and sensor attributes.
5. Split the next low-risk T23/T41 subsystem, such as proc diagnostics or frame
   completion, behind a reviewed interface.
6. Move event/state-machine shells only after IRQ decode and acknowledge
   ordering remain explicit per-SoC behavior.
7. Model the next layer of buffer ownership and queue-lifecycle state behind
   typed adapters,
   starting with read-only comparisons of the T23/T31/T41 consumer contracts.
