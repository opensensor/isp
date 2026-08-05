# Cross-SoC Driver Reuse Plan

## Scope

The active refactor covers the working T23, T31, T40, and T41 TX-ISP drivers.
T31 already has core, CSI, VIC, VIN, frame-source, tuning, and support
translation units. T23, T40, and T41 still have large recovered core
translation units, but their modules now link separate shared-library adapter
objects. This gives later extractions stable module boundaries without
rewriting the recovered pipeline all at once.

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
- unsigned 32-bit and 64-bit fixed-point add/subtract with natural wraparound
- OEM-exact signed 64-bit fixed-point rounding
- two- and three-operand unsigned 32-bit and 64-bit fixed-point multiplication
- T23-compatible split multiplication with wrapped 32-bit partial products
- OEM-compatible wrapped and full-range unsigned 32-bit fixed-point division
- full-range unsigned 64-bit fixed-point division without compiler 128-bit
  arithmetic or libgcc division helpers

The per-SoC wrappers preserve their existing ABI and endpoint policy:

- T23 uses signed interpolation with its eight-step table endpoint and
  delegates its generic/32/64-bit add/subtract and generation-specific wrapped
  multiply/divide entry points.
- T31 delegates its 32-bit fixed-point multiply wrappers to the common helpers.
  Its native 64-bit algorithm is available and tested in the common header,
  but the recovered AE translation unit retains an identical local inline body
  until an extra wrapper no longer changes its machine-code register allocation.
- T41 uses the unsigned 8/16/32-bit interpolation variants with its ten-step
  endpoint and delegates its generic/32/64-bit fixed-point add/subtract,
  signed rounding, full-width multiply/divide, 32-/64-bit log2, exp2, and
  wrapped 32-bit divider wrappers.
- T23 retains its recovered unsuffixed/64-bit multiply bodies. Moving those
  symbols into the adapter reproducibly made `rvd` exit during startup on two
  clean boots, while rebuilding the committed local boundary restored Raptor
  and produced the exact accepted module hash.

`tests/tx_isp_math_test.c` covers boundary behavior, OEM interpolation
rounding, signed 64-bit fixed-point rounding, wrapped 32/64-bit add/subtract
and products, T23's split-product equivalence over Q1-Q31, Q0-Q63 64-bit
products and quotients, typed tables, and randomized equivalence checks.

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

### Frame-buffer wire ABI and state flags

`driver/include/tx_isp/tx_isp_frame_abi.h` defines the shared 17-word
MIPS32 `v4l2_buffer` wire shape used by all three active generations. Named
offsets replace raw indices in the T23 and T41 private frame-channel paths,
and the common 68-byte size is asserted against T31's kernel
`struct v4l2_buffer`.

The common layer also owns the persistent flag mask and a pure,
policy-parameterized queue/state flag builder. T31 and T41 deliberately have
different state masks; preserving those adapters avoids the unsafe assumption
that a numeric internal buffer state has one meaning across SoCs. T31 uses the
shared inline policy; T41 uses a common statement form that retains the vendor
load/write order and prevents compiler register-allocation drift across its
recovered monolith. The integration is binary-neutral on T23, T31, and T41,
providing a stable contract for later extraction of queue-copy helpers without
changing live behavior. `tests/tx_isp_frame_abi_test.c` validates sizes,
offsets, flag merging, policy priority, every known state, and single
evaluation by the T41 adapter.

### Frame-image format wire ABI

`driver/include/tx_isp/tx_isp_frame_format.h` owns the fixed private format
layout separately from both queue state and image-size arithmetic. Its
112-byte base contains the 48-byte pixel descriptor and crop/scaler/rate
controls shared by T23/T31. T41 appends its recovered flip word for a
116-byte generation-specific envelope.

T31 no longer embeds the kernel's compiler-conditional `v4l2_pix_format`.
Under the current compiler that type omitted four words and silently reduced
the private payload to 96 bytes even though `_IOWR` and the OEM userspace
contract require 112. Replacing only that nested descriptor with the shared
wire type restores the original offsets while keeping T31's normalization,
event dispatch, and hardware policy local. Two device boots delivered
full-rate 1080p and 360p streams with clean kernel/Raptor/Android logs.

T23 asserts its recovered local structure against the common size. T41 uses
the common word names in its live set/get-format path while retaining its
different field and colorspace values. `tests/tx_isp_frame_format_test.c`
provides host-side size and offset regression coverage.

### Frame-channel events and private ioctl envelopes

`driver/include/tx_isp/tx_isp_frame_channel.h` names the common remote events
from get-format through buffer-done and both generation-qualified ioctl
families. T23/T31 use the legacy `V` family with a 112-byte format; T41 uses
its private `T` family with the 116-byte flip extension. Small pure decoders
make type, number, payload size, and direction testable on the host.

The same header defines the compiler-independent 20-byte REQBUFS object and
its five named word positions. T31 uses the structure directly; T23 and T41
retain code-generation-stable arrays while replacing raw positions with those
names. The legacy stream-on/off command IDs and their four-byte scalar payload
are common, but each generation still owns stream state and hardware order.

The common event range intentionally ends at `0x03000006`. Event meanings
above buffer completion diverge between recovered generations and stay local
until device evidence proves otherwise. T23's aliases, T31's frame/core/VIC
handoffs, and T41's dispatch/IRQ paths now reference the common contract.
`tests/tx_isp_frame_channel_test.c` covers every named envelope and the proven
event sequence.

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
generation-specific subdevice offsets. T31 compile-time assertions now cover
the complete OEM 0x24-byte pad, including the private pointer and stride. It
also owns the exact
five-word, 20-byte active-link record: source, sink, reverse, flags, and state.
Host checks and T31 target assertions cover every field. T23/T41 recovered
link teardown and T41 remote-event dispatch use the common offsets without
changing any active module image.

The shared header also owns the identical T23/T41 link-detach prefix: capture
source, reverse, and sink, then clear source, sink, reverse, and flags while
retaining the link state word. Pad state transitions and the rest of link
destruction remain generation-local. A sequential T23/T31/T41 rebuild after
this extraction reproduced all three active module hashes exactly.

The former T31-only callback after the pad event pointer was a recovery error:
the allocator already used the OEM 0x24-byte stride, so the 0x28-byte C type
made the final pad's private-pointer initialization overrun the allocation.
The callback is removed, private/size assertions enforce the wire ABI, and an
explicit compatibility word after the embedded channel pad preserves the
already-established `isp_channel` tail offsets. A one-shot GC2053 boot bound
one sensor, kept Raptor and the ISP interrupts live, and decoded 358 main
frames in 12 seconds without a kernel fault.

`driver/common/tx_isp_subdev.c` now owns the name/type/index graph search used
by all four active generations. Small layout adapters retain T23's graph table
at `0x38`, the T40/T41 tables at `0x3c`, their legacy/extended subdevice
prefixes, and T31's established raw direction mapping. The common resolver
validates type, pad storage, and bounds and reports a host-tested failure
status without owning logging or link mutation.

The companion wire contract fixes an endpoint at 8 bytes, a complete
source/sink/flag link record at 20 bytes, and a record-set pointer/count
envelope at 8 bytes. Host tests cover every field; T31 target assertions pin
the SDK declarations, and T41's recovered graph path now uses the common
positions. Sequential rebuilds remained byte-identical to all three tested
resolver modules.

The same common subdevice unit now owns the generation-neutral part of link
state transitions. It validates the source/sink capability intersection,
rejects streaming pads, adds the enabled flag, initializes the five-word
active-link record, and writes symmetric source/sink pairs in recovered order.
T23 and T31 use the common initializer during pad allocation; T41 also uses
the validator and pair connector during live graph setup. Conflict teardown
callbacks and their ordering remain generation-local.

T40 now uses the common initializer during recovered pad allocation and the
validator/pair connector in its live graph-repair path. Its strict pointer
policy, diagnostic counters, and teardown remain local.

A sequential three-target build and one-shot device cycle validated this
extraction. Each device consumed its marker, registered exactly one sensor,
accepted forced day mode, advanced ISP/VIC interrupts, and decoded 149, 148,
and 150 1080p frames in six seconds on T23, T31, and T41 respectively.
`dmesg`, `logread`, and `logcat` fault scans were clean. The scene darkened
during a rainstorm, so image brightness and wall-noise comparisons were
deliberately excluded from this structural gate.

`driver/common/tx_isp_remote_event.c` now owns the generation-neutral
local-pad to active-sink to event-handler lookup shared by T23, T40, and T41.
Generation adapters retain their pointer policy, invocation, and diagnostics.
This replaces T23's recovered unconditional-success stub with the OEM
three-argument dispatch behavior while leaving T41's working call path and
logging intact. T40 keeps its event-class filter, detailed diagnostics, and
local frame-done fallback. T31's typed dispatcher has different fallback
semantics and is intentionally not a consumer.

Host failure-path tests pass, sequential T23/T41 builds left the excluded T31
artifact byte-identical, and fail-safe boots left the new T23/T41 modules
active. Both registered one sensor, accepted forced day mode, and advanced ISP
interrupts. Six-second 1080p decodes produced 149 T23, 148 untouched-control
T31, and 151 T41 frames; T41 early logs show repeated resolved remote
callbacks returning zero. Kernel, system, and Raptor logs contain no driver
faults. The scene was darkening during a storm, so the cycle validates
dispatch and transport rather than comparative image quality.

The common state unit now owns the identical value-level T23/T40/T41
`check_state` decision while adapters retain their different queue offsets and
state-field widths. Besides removing duplicate recovered policy, this restores
the T40 and T41 OEM behavior in place of unconditional-zero recovery stubs.
Host edge cases pass. Fail-safe T23/T41 boots each bound one sensor, accepted
forced day mode, advanced ISP interrupts, and decoded 149/151 1080p frames in
six seconds without driver faults. T31 was not rebuilt or rebooted and its
artifact and live module remained byte-identical.

`driver/include/tx_isp/tx_isp_recovered_kernel.h` holds the already-reviewed
kernel-tree compatibility prelude used by recovered sources. Keep larger
freestanding recovery-tool declarations local until their signatures are
verified.

### Canonical T40 artifact

T40 Kbuild emits `driver/t40/tx-isp-t40.ko`, matching the dependency name used
by the GC4653 sensor module. It is now a three-object module:

- `tx_isp_t40_recovered.c` owns the recovered hardware and tuning pipeline.
- `tx_isp_t40_sinfo.c` supplies the T40 layout adapter for the common sensor
  registry.
- `tx_isp_t40_subdev.c` supplies the extended-layout graph, remote-event,
  link-state, and readiness adapters.

The 2026-07-31 T40XP/GC4653 cycle built against the exact 4.4.94 firmware tree,
registered one sensor, accepted forced day mode, kept IRQ 38/39 active, and
delivered a valid 1920x1080 H.264 stream. FFmpeg decoded eight frames without
errors. The full recovery proc dump remains explicitly outside the health
contract because its legacy diagnostic path is unsafe.

### Canonical T41 artifact

T41 Kbuild now emits `driver/t41/tx-isp-t41.ko`, the canonical dependency name
used by current T41 sensor modules, while retaining
`tx_isp_t41_recovered.c` as the source filename.

T41 is now a multi-object module with nine explicit boundaries:

- `tx_isp_t41_recovered.c` owns the recovered pipeline, hardware, and tuning
  implementation.
- `tx_isp_t41_daynight.c` adapts T41 registers and callbacks to the common
  transition state machine.
- `tx_isp_t41_math.c` preserves the recovered math entry-point ABI and
  delegates its algorithms to `tx_isp_math.h`.
- `tx_isp_t41_sinfo.c` supplies the T41 object-layout adapter for the common
  sensor registry.
- `tx_isp_t41_subdev.c` adapts the extended T41 graph and subdevice layout to
  the common pad resolver.
- `tx_isp_t41_tuning_abi.c` links the shared proprietary control ABI.
- `tx_isp_t41_frame_layout.c` links checked frame-channel NV12 geometry and
  vendor aggregate-line reporting.
- `tx_isp_t41_exposure.c` adapts T41 sensor control to checked common exposure
  planning.
- `tx_isp_t41_scaler.c` supplies T41 coefficient tables to the common checked
  scaler generator.

### Multi-object T23 artifact

T23 uses the canonical deployed `tx_isp_t23` module identity while linking
ten logical objects:

- `tx_isp_t23_core.c` owns the recovered pipeline, hardware, tuning, and the
  T23-specific sensor lifecycle callbacks.
- `tx_isp_t23_math.c` preserves the recovered interpolation entry-point ABI
  and delegates its algorithm to `tx_isp_math.h`.
- `tx_isp_t23_sinfo.c` supplies static metadata and lifecycle callbacks for the
  common sensor registry.
- `tx_isp_t23_subdev.c` adapts the legacy T23 graph and subdevice layout to
  the common pad resolver.
- `tx_isp_t23_mode.c` owns the T23 bypass masks and one authoritative
  declarative 17-block mode-refresh sequence.
- `tx_isp_t23_callback_plan.c` links the common validated callback runner.
- `tx_isp_t23_reg_profile.c` links the shared ordered-profile and register-mask
  implementation.
- `tx_isp_t23_tuning_abi.c` links the shared proprietary control ABI.
- `tx_isp_t23_frame_layout.c` links checked frame-channel NV12 and MDNS
  geometry.
- `tx_isp_t23_scaler.c` supplies T23 coefficient tables to the common checked
  scaler generator.

## Device Validation

Every staged module was loaded through the one-shot fail-safe hook, exercised
through the real Raptor consumer, and checked with stream captures plus
kernel and service diagnostics. The persistent module is restored and
verified after each controlled experiment.

| SoC | Staged coverage | Result |
|---|---|---|
| T23 | ten-object module, shared math/registry/subdevice resolver/register-mask/callback-plan/tuning-ABI/frame/channel/MDNS layout plus mode adapter, SC2336 | pass; exact `0x477e70` MDNS use/`0x478000` allocation, day/night/auto, and full-rate RTSP clean |
| T31 | shared math/registry/subdevice resolver/day-night/profiles/callback-plan/tuning-ABI/frame/channel/MDNS layout; SC2336 and GC2053 | transport/ABI pass; corrected 0x24-byte pad allocation contract, one GC2053 registration, 1920x1080/640x360 at 30 fps, and decoder-clean RTSP. CLM banks now match the GC2053 OEM image byte-for-byte; active-GIB color parity remains open |
| T40 | three-object module, shared registry/subdevice resolver/link/event/state policy, GC4653 | pass; day mode, active IRQ 38/39, and valid 1920x1080 RTSP decode; legacy full proc dump excluded |
| T41 | nine-object module, shared math/registry/subdevice resolver/day-night/tuning-ABI/frame/channel/scaler/exposure, OS04D10 | transport/ABI pass; complete registry parity, correct 3,133,440-byte main pool, and full-rate RTSP; mixed-light tuning remains scene-dependent |

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
├── t40/                  # T40 recovered pipeline and shared adapters
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
canonical behavior. T23, T40, and T41 may expose older or newer semantics that
need to shape the eventual common interface.

## Safe Extraction Rules

1. Compare parameter types, call sites, return values, side effects, and object
   layout across every intended consumer.
2. Preserve the existing exported or per-driver entry point as a thin wrapper
   during the first move.
3. Express generation differences as explicit data, offsets, or callbacks.
4. Add host tests for pure code and build all four active modules for every
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
3. Diagnose the T31 linear-GIB active path using the GC2053 stock/open oracle;
   with otherwise identical CLM tables, GIB bypass restores AWB RGB sums and
   recognizable color while the OEM module operates correctly with GIB active.
   Visible GIB controls, the DEIR table, internal BLC, and post-top payload
   replay have now been ruled out; compare the raw-front-end clock/reset,
   input-format, and ownership sequence before changing more tuning data.
4. Extend the checked ABI from pads to links, events, and sensor attributes.
5. Split the next low-risk recovered T23/T40/T41 subsystem behind a reviewed
   interface. T40 proc diagnostics are specifically excluded until their
   unsafe full-read path is repaired.
6. Move event/state-machine shells only after IRQ decode and acknowledge
   ordering remain explicit per-SoC behavior.
7. Model the next layer of buffer ownership and queue-lifecycle state behind
   typed adapters,
   starting with read-only comparisons of the T23/T31/T40/T41 consumer
   contracts.
