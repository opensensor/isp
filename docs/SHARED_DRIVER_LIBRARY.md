# Shared TX-ISP Driver Library

## Purpose

The active T23, T31, and T41 drivers share behavior only where device evidence
shows that the contract is genuinely common. Shared code owns algorithms and
small state-machine shells; each SoC adapter continues to own addresses,
object layouts, tuning data, callbacks, and ordering differences.

T40 is intentionally outside this refactor.

## Current Library

| Unit | Interface | Implementation | Consumers |
|---|---|---|---|
| Fixed-point and interpolation math | `tx_isp_math.h` | header-only pure functions | T23, T31, T41 |
| Sensor registry | `tx_isp_sinfo.h` | `common/tx_isp_sinfo.c` | T23, T31, T41 |
| Day/night transition shell | `tx_isp_daynight.h` | `common/tx_isp_daynight.c` | T31, T41 |
| Ordered register profiles | `tx_isp_reg_profile.h` | `common/tx_isp_reg_profile.c` | T23, T31 |
| Ordered callback plans | `tx_isp_callback_plan.h` | `common/tx_isp_callback_plan.c` | T23, T31 |
| Proprietary tuning wire ABI | `tx_isp_tuning_abi.h` | `common/tx_isp_tuning_abi.c` | T23, T31, T41 |
| Frame-buffer wire ABI and state flags | `tx_isp_frame_abi.h` | header-only pure functions | T23, T31, T41 |
| Frame-channel events and ioctls | `tx_isp_frame_channel.h` | header-only descriptors and decoders | T23, T31, T41 |
| Frame-image format wire ABI | `tx_isp_frame_format.h` | compiler-independent wire types | T23, T31, T41 |
| Checked NV12 and MDNS layouts | `tx_isp_frame_layout.h` | `common/tx_isp_frame_layout.c` | T23, T31, T41 |
| Subdevice pad/link ABI | `tx_isp_subdev_abi.h` | header-only offsets, assertions, and detach helpers | T23, T31, T41 |
| Subdevice graph resolver | `tx_isp_subdev.h` | `common/tx_isp_subdev.c` | T23, T31, T41 |
| Remote pad event resolver | `tx_isp_remote_event.h` | `common/tx_isp_remote_event.c` | T23, T41 |
| Recovered subdevice readiness | `tx_isp_state.h` | `common/tx_isp_state.c` | T23, T41 |

The common implementation is linked into each TX-ISP module rather than
loaded as another kernel module. This preserves the exported symbol and
dependency ABI expected by the matching sensor drivers and userspace.

### Subdevice pad and link ABI

`tx_isp_subdev_abi.h` names the common 32-bit pad prefix and its embedded
five-word active-link record. The link fields are source, sink, reverse,
flags, and state at offsets `0x00` through `0x10`, with a total size of
`0x14`; the pad event function remains at offset `0x1c`.

The identical recovered T23/T41 detach prefix now uses a shared operation that
returns the source, reverse link, and sink before clearing the first four link
words. It deliberately preserves the state word and leaves pad state and
generation-specific destruction policy to each driver. Host tests lock down
the read order, cleared fields, and retained state.

T31 asserts every typed link field at target compile time. T23 and T41 use the
same names in recovered link teardown, T23 uses the pad names during pad
creation, and T41 uses them for frame-channel event callback installation and
remote dispatch. Host tests assert every link and pad offset. The change is
binary-neutral on all three active modules.

The shared prefix deliberately stops before the unresolved pad tail. T31 has
an extra `event_callback` member after `event`, so its `priv` position and
full pad size differ from the recovered 0x20/0x24 layout used elsewhere.

### Subdevice graph resolution

`tx_isp_subdev_resolve_pad()` owns the common graph search: validate the OEM
output/input type, find the named subdevice, validate pad storage and index,
then apply the configured stride. It returns a precise status for invalid
records, unknown names or types, missing pad arrays, and range failures.

The same interface defines the compiler-independent graph wire objects: an
8-byte endpoint containing a 32-bit name pointer plus type/index bytes, a
20-byte source/sink/flag link record, and an 8-byte record-pointer/count set.
Host tests assert every offset and size; T31 additionally asserts its declared
vendor structures against the contract at target compile time. T23/T41
adapters and T41's recovered link setup use the named offsets.

The adapters keep every physical difference visible. T23 reads 16 subdevice
pointers at graph offset `0x38` and uses the legacy `0xc8` through `0xd0`
pad slots. T41 reads its 16 pointers at `0x3c` and uses the extended `0x100`
through `0x108` slots. T31 uses its typed graph and retains its established
raw input/output mapping, including the known reversal relative to the
declared structure. Pointer validation also remains adapter policy.

Host tests exercise valid input/output resolution and every failure status.
One-shot device boots then exercised real graph creation on all three SoCs.
Forced day mode passed, sensor bind/add remained one-for-one, ISP/VIC
interrupts advanced, and six-second 1920x1080 decodes produced 148 T23, 149
T31, and 150 T41 frames. Post-stream `dmesg`, `logread`, and `logcat` fault
scans were clean. The live module hashes are respectively
`865278fb52172ff948b4a4e2bc92c27680344c12b7b57dfcc9f1c94743fc4047`,
`bb95aba4587061b4d364fd3b3893bbf8d8ec0471252deed97dc996e569d48673`,
and `34aaccbd070010bc704e9588182bfb9f3302a416bcd871111c53f82ae235cd1e`.
Ambient brightness and noise were not compared because storm lighting was
changing.

Replacing the remaining raw graph-record positions with this wire contract
rebuilt to the exact three live hashes above, so it required no additional
device cycle.

### Remote pad event routing

`tx_isp_resolve_remote_event()` owns the common recovered route from a local
pad through its active-link sink to the remote pad event handler. The pure
resolver clears its output first, validates the local pad, remote pad, and
handler through generation policy, and distinguishes invalid input, an
unlinked pad, and a missing handler. Callback invocation, event logging, and
the kernel-address policy remain in each SoC adapter.

T23 and T41 share the same recovered pad/link positions and use small ABI
accessors around the common resolver. This restores T23's OEM three-argument
remote-event behavior in place of its former unconditional-success stub.
T41 preserves its existing event diagnostics and call ordering. T31 retains
its structurally different typed dispatch/fallback path and does not link this
unit.

Host tests cover invalid inputs, missing accessors, unlinked and invalid
remote pads, absent and invalid handlers, and successful callback invocation.
Sequential T23/T41 builds left the excluded T31 artifact byte-identical.
Fail-safe T23/T41 boots then registered one sensor apiece, accepted forced day
mode, advanced all active ISP interrupt lines, and decoded 149 and 151
1920x1080 frames in six seconds. The untouched T31 control decoded 148 frames.
T41's early kernel capture recorded repeated real remote routes returning
zero; final `dmesg`, `logread`, and `logcat` scans contained no driver faults.
The active T23, T31, and T41 module hashes are respectively
`d8f71fd58f2fde522b1cc61f8f29386bbf3c7869923a66003c9034b406801c58`,
`322ace762900c81d22e55f527572708f6ae92d6ca144725d52fb58413dd37b4a`,
and `07166ce513a884029b90c3250976da957fd4d6439932cbc17ef67f4946a3f7ae`.
Outdoor light was changing rapidly during a rainstorm, so this gate makes no
brightness, noise, or image-detail comparison.

### Recovered subdevice readiness

The OEM T23 and T41 `check_state` functions apply the same policy: reject a
null object; accept an object whose queue head is not self-linked; otherwise
invert bit zero of its state field. `tx_isp_subdev_state_ready()` owns that
value-level policy. The adapters retain T23's `0x1f8` queue/`0x20c` word-state
layout and T41's `0x1fc` queue/`0x224` byte-state layout.

This extraction also restores T41 behavior that the recovered source had
collapsed to an unconditional-zero stub. Host tests cover null, linked,
self-linked, and unrelated state-bit combinations. Sequential T23/T41 builds
left T31's artifact unchanged, and fail-safe device boots registered one
sensor each, accepted forced day mode, advanced ISP interrupts, and decoded
149 and 151 1080p frames in six seconds. Final kernel, system, and Raptor logs
contained no driver faults. The active T23 and T41 module hashes are
`86970c12687c268c795a53c91db734716662eefe2eaca4b41e6a45edf28ce61b`
and
`adb6e008067e4e44f909d6695a6a3f53ac87355e9d5c8766f6d1a5b3e3e84f96`.
T31 remained active at
`322ace762900c81d22e55f527572708f6ae92d6ca144725d52fb58413dd37b4a`.
Changing storm light again excluded visual-quality comparisons.

## Adapter Boundaries

### Day/night

The common day/night state machine validates modes, stages the requested bank,
publishes pending state in the required order, and applies the frame-boundary
fill or normal transition. Its configuration supplies SoC callbacks for:

- the actual MMIO write
- mode-specific tuning preparation
- userspace notification
- frame-drop preparation
- CSC and optional shadow-commit registers

T31 and T41 use different register maps, mode identifiers, tuning callbacks,
and commit behavior. Those remain in `driver/t31/tx_isp_daynight.c` and
`driver/t41/tx_isp_t41_daynight.c`.

T23 keeps its recovered transition orchestration for now. It refreshes a
larger ordered list of tuning blocks directly, so forcing it through the
T31/T41 adapter before that sequencing is understood would be unsafe.

### Ordered profiles

`tx_isp_reg_profile_apply()` writes a profile in declaration order and can
perform exactly one final shadow-commit write. Sensor- and block-specific
tables stay local to their owner.

T31 currently uses this primitive for the SC2336 day/night DMSC correction in
`tx_isp_dmsc_profile.c`. The profile is reapplied after the generic DMSC
refresh only when the existing SC2336 OEM-profile gate is active.

`tx_isp_reg_flags_merge()` preserves the recovered shift-and-merge semantics
used to construct ISP top-bypass masks. T23's dedicated
`tx_isp_t23_mode.c` adapter now combines that common primitive with the T23
masks and one authoritative 17-block refresh order. Day/night, custom-mode,
and tuning-bin transitions call the adapter instead of maintaining three
copies.

T31 likewise keeps one authoritative 18-block OEM refresh plan for normal
day/night and custom-bank transitions. Its individual block functions remain
local to the T31 tuning translation unit and are exposed to the generic plan
through typed local adapters.

### Callback plans

`tx_isp_callback_plan_run()` first validates every entry, then invokes the
callbacks in declaration order. A malformed plan therefore returns `-EINVAL`
without executing a partial hardware sequence.

The runner is deliberately small: error handling inside a tuning step remains
an SoC policy, while the plan guarantees shape and ordering. T23 and T31 use
it for their distinct 17- and 18-block refresh sequences.

### Frame-buffer ABI

`tx_isp_frame_abi.h` names the exact 17-word, 68-byte MIPS32 frame-buffer
contract used by the private T23, T31, and T41 frame-channel ioctls. It owns
the stable word offsets, the 52-byte copied prefix, persistent flag mask, and
V4L2 queued/done/error values without depending on the build host's pointer or
`timeval` sizes.

Buffer-state values are not assumed to be universal. The common pure flag
builder takes explicit state masks, with small T31 and T41 policy wrappers for
their recovered meanings. Queue lists, locks, DMA handoff, late-link replay,
and completion ordering remain generation-local. The active adapters now use
the shared word and flag names in place of numeric offsets while preserving
all three validated module images byte for byte.

T31 calls its shared inline policy directly. T41 uses the equivalent common
statement adapter because an ordinary inline call lets GCC reschedule the
state load across the recovered flag block, perturbing register allocation
throughout the large queue functions. The adapter retains the vendor operation
order and keeps the validated T41 image byte-exact without duplicating policy
in the recovered monolith.

`tests/tx_isp_frame_abi_test.c` checks every important wire offset, total size,
prefix size, persistent/queue flag merging, each recovered state transition,
overlapping-policy priority, out-of-range states, and single evaluation by the
layout-preserving T41 adapter.

### Frame-channel event and ioctl contracts

`tx_isp_frame_channel.h` owns the device-proven remote event sequence from
get/set format through stream on/off, queue buffer, and buffer completion
(`0x03000001` through `0x03000006`). T23/T31 retain their V4L2 `V` private
ioctl family and 112-byte format, while T41 retains its private `T` family and
116-byte extended format. Set/get format, buffer requests, query/queue/dequeue,
stream control, completion wait, bank/count, and alignment commands are named
without manufacturing a false common command number across generations.

Events `0x03000007` through `0x03000009` remain local. Recovered T31 and T41
code assigns different queue-free, request, and VIC meanings in that range,
so the shared namespace deliberately stops at the last contract supported by
all three devices.

T23's frame-channel compatibility aliases, T31's outer dispatcher and
ISP/VIC event handoff, and T41's recovered dispatcher/IRQ handoff now consume
the common names. Pure ioctl decoders expose command number, type, payload
size, and direction for host validation without depending on a kernel's
`_IOC` definitions. `tests/tx_isp_frame_channel_test.c` checks the contiguous
event contract plus every legacy and T41 envelope.

The header also owns the fixed five-word, 20-byte request-buffer envelope,
named count/type/memory/capabilities/reserved positions, and the legacy
stream-on/off commands. T31 consumes the shared structure directly. T23 and
T41 use the named positions in their existing arrays to retain recovered
instruction layout. Allocation limits, queue release, stream transitions, and
generation-divergent remote events remain in the adapters. Sequential builds
produce the same validated module hashes, so this extraction is binary-neutral
on all three active SoCs.

### Frame-image format ABI

`tx_isp_frame_format.h` models the private frame-channel image format without
using the build kernel's conditional `struct v4l2_pix_format`. T23 and T31 use
the 112-byte base: a fixed 48-byte pixel descriptor followed by crop, scaler,
rate, and front-crop fields. T41 uses the same base plus its recovered
four-byte flip-enable extension, for 116 bytes total.

This boundary exposed a real T31 compiler dependency. Ingenic's 3.10
`videodev2.h` includes the final flags/encoding/quantization/transfer words
only when `GCC_VERSION == 50400`. A modern cross-compiler therefore made the
open driver's private format 96 bytes while its ioctl still advertised the
OEM 112-byte payload. T31 now embeds the shared fixed pixel descriptor, so
copy size and every crop/scaler offset match libimp regardless of compiler.

The T31 device passed two clean boots with the corrected layout. Main and
substream decoded 148/147 frames in six seconds, then 124/122 in five seconds;
the old module's repeated substream samples were 76 frames in six seconds.
T23 and T41 rebuild byte-identically because their layouts were already
explicit. T41's differing field/colorspace values and flip extension remain
generation-qualified rather than normalized away.

`tests/tx_isp_frame_format_test.c` asserts both complete sizes, all structural
boundaries, enable-byte padding, named word offsets, and the T41 extension.

### Sensor registry

The sensor registry owns slots, bind/unbind lifecycle, exported registry entry
points, and procfs representation. Each SoC supplies a typed configuration
that describes its recovered object layout and lifecycle side effects.

T31 additionally makes the userspace `AddSensor` path idempotent. If `rvd`
exits without its normal IMP teardown, a replacement producer reuses the
existing I2C client and sensor subdevice instead of trying to create a second
client at the same address and failing with `-EBUSY`. This recovery ownership
is still T31-local until the T23 and T41 client lifecycles have matching device
evidence.

### Math

Math primitives are kernel-independent and do not know the active SoC. Thin
per-SoC wrappers preserve legacy entry-point names and generation-specific
table endpoints. The library keeps T23's wrapped 32-bit split multiplier
separate from the full-range T31/T41 primitive because its fractional partial
product wraps before the Q-format shift. T23 and T41 share wraparound
add/subtract arithmetic but retain their different underflow diagnostics in
their adapters. T41 now routes all four native two-/three-operand multiply
symbols and both native divide symbols through full-range primitives. The
64-bit divider uses bounded shift/subtract arithmetic so kernel builds do not
acquire compiler-runtime division dependencies. T41's integer/fixed 32-/64-bit
log2 entry points use the shared normalized-square implementation, and its
exp2 entry point uses the shared bounded 33-node Q30 interpolation. Its live
DMSC/SDNS strength fanout also uses the shared three-segment Q7 ratio blend,
with the recovered low-byte branch ABI made explicit in the common primitive.
The T41 entry remains in its original text slot because moving it shifted
still-recovered indirect call targets; its macro form produces the exact
accepted load image while sharing one tested algorithm. T31 retains
size-neutral local copies in its recovered AE object, and T23 retains its
recovered multiply bodies after an adapter extraction reproducibly terminated
`rvd` during startup.

### Proprietary tuning ABI

The common tuning ABI unit owns only byte-stable contracts:

- the 8-byte T23/T31 scalar control, 12-byte T31 extended control, and
  generation-specific 16-byte T23/T41 envelopes
- exact 12-byte expression, 24-byte EV, and 8-byte WB replies
- the sparse 0x80-byte EV offsets used to construct an expression reply
- validated command descriptors with direction, inline-versus-userspace-
  pointer payload kind, and fixed size where known

Dispatch, hardware collection, and object offsets remain local. T23 uses the
common descriptors and response packers in its safe userspace bridge. T31
uses them for explicit get-control routing and no longer copies the first
eight bytes of a larger internal structure as an implicit wire contract. T41
uses a typed startup envelope and a two-entry generation-specific descriptor
table for frame-rate and running-mode controls.

The descriptor lookup intentionally accepts per-SoC tables. Identical numeric
ranges are not assumed to have identical payloads, and T41's moved control IDs
are named separately.

### Frame layout

`tx_isp_nv12_layout_build()` computes aligned Y-plane stride, the vendor
private ABI's aggregate 12-bpp line size, aligned height, Y-plane size, UV
offset, and complete single-plane NV12 size with checked 64-bit intermediates.
It rejects zero dimensions, non-power-of-two alignment, and every result that
cannot be represented by the vendor ABI's 32-bit fields. The two line sizes
are intentionally distinct: the aggregate value describes the complete NV12
line to libimp, while the Y stride locates the UV plane in DMA memory.

`tx_isp_nv12_buffer_build()` binds that geometry to one physical buffer. It
rejects an undersized userspace allocation and any image whose final byte
would exceed the 32-bit DMA address space, then publishes typed Y and UV
addresses atomically. T23, T31, and T41 call it before their local QBUF paths
write MSCA state. Queue ownership, locking, replay, and completion remain
per-SoC behavior.

Alignment remains policy supplied by the adapter:

- T23 supplies width alignment 1 and height alignment 16 for its MSCA format
  and UV offset.
- T31 supplies width alignment 1 and height alignment 16 for both core and
  outer frame-channel formats. Its outer private ABI now reports the OEM
  aggregate line size and applies the NV12 plane multiplier exactly once.
- T41 supplies width alignment 32 and height alignment 16 for both set-format
  and QBUF, so those paths cannot disagree about the UV base or total size.

`tx_isp_mdns_layout_build()` models the independently aligned T23/T31 MDNS
allocation: one NV12 working image, four 64-byte-aligned reference banks, two
UV banks, and the final tiny image. It also models the `isp_memopt=1` aliasing
policy in which the auxiliary banks share the start of the working image.
T23 and T31 still own allocation, register programming, logging, and ABI
policy. T23 page-aligns the size returned by its 12-byte `GET_BUF` contract;
T31 returns the exact used size through its eight-byte contract.

## Rules for New Shared Code

1. Share a contract, not merely a matching recovered symbol name.
2. Pass registers, tables, layout offsets, and side effects explicitly.
3. Keep IRQ acknowledgement, reset order, and stream handoff per-SoC until
   hardware evidence proves equivalence.
4. Preserve existing driver entry points as thin adapters during extraction.
5. Add host tests for pure behavior.
6. Build every consumer and smoke-test every affected device.
7. Keep structural refactors separate from new tuning-value experiments.

## Validation Matrix

The July 30, 2026 device cycle exercised the real Raptor consumer, both RTSP
streams, forced day/night transitions, automatic mode, and post-run
`dmesg`, `logread`, and `logcat`.

| SoC / sensor | Main stream | Sub stream | Mode coverage | Result |
|---|---:|---:|---|---|
| T23 / SC2336 | 1920×1080 @ 25 fps | 640×360 @ 25 fps | day, night, auto | clean geometry and color; no fatal or memory faults |
| T31 / SC2336 | 1920×1080 @ 25 fps | 640×360 @ 25 fps | day, night, auto | one-buffer pre-dequeue restored; high-gain MDNS profile matches stock flat-wall stability |
| T41 / OS04D10 | 1920×1080 @ 25 fps | 640×360 @ 25 fps | day, night, auto | balanced `0x380/0x880` day AWB and dual-stream fanout |

The tested stock reserved-memory settings are sufficient: 22 MiB on T23/T31
and 30 MiB on T41. No bootloader environment change is currently required.

The T31 consumer uses one frame-source buffer and configures
`isp_ch0_pre_dequeue_time=24`. Stock schedules an early DQBUF event from the
frame-start interrupt so userspace can return that buffer before the following
frame. The open worker previously ignored that event and therefore delivered
only about 12.5 fps. Restoring the one-buffer pre-dequeue contract produced
501 main frames in 20 seconds and 250 substream frames in 10 seconds. A
same-light stock oracle produced 191 frames in eight seconds.

At the storm-light gain point, the SC2336-specific automatic MDNS profile
selects ratio 160 only in the sensor's `0x8xx` high-gain stage and returns to
128 below the hysteresis band. With that profile, mean flat-wall decoded luma
change was `0.01512`, compared with stock `0.01478`. The policy remains
T31/SC2336-local and can be disabled with `sc2336_mdns_auto=0`.
Later wall observations were made while storm light was falling and are not
treated as same-scene regression evidence.

The latest T31 cycle also corrected three proprietary tuning-ABI hazards:

- a sparse EV structure write that previously overran the kernel stack
- scalar routing that swallowed pointer-valued WB/highlight/backlight queries
- an incorrectly packed expression structure and frame-duration/line-duration
  mix-up that forced exposure and AE-luma telemetry to zero

The resulting T31 module survived 100 consecutive ISP/exposure query pairs,
day/night cycling, and RTSP decoding with no `dmesg`, `logread`, or `logcat`
ioctl/fault signature.

The frame-channel contract extraction was rebuilt sequentially because
`build_local.sh` changes shared top-level build selection and therefore cannot
produce authoritative artifacts in parallel. The deterministic T23, T31, and
T41 modules all passed fail-safe boots with loader status zero and consumed
one-shot markers. Raptor reports both configured channels at 25 fps on every
device; T31 and T41 additionally saved fresh 302,532- and 379,001-byte
1920x1080 JPEGs while an external monitor occupied the RTSP servers' client
slots. ISP interrupts advanced, day mode applied, and `dmesg`, `logread`, and
`logcat` contained no driver fault signature. The tested open builds remain
active. Ambient brightness and noise were not compared because a rainstorm
changed the scene during validation.

The subsequent common tuning-ABI extraction was rebuilt and reboot-tested on
all three active devices. T23 decoded 124 main-stream frames in six seconds,
T31 decoded 126 in ten seconds, and T41 decoded 201 in eight seconds. T31
again completed 100 ISP/exposure query pairs, while T41 completed 100 typed
frame-rate queries at 25/1. All three passed day/night/auto/day transitions
and ended in day mode with loader status zero, the one-shot marker consumed,
Raptor running, and their staged open module active.

The checked frame-layout extraction then passed a second clean reboot on all
three targets. T23 decoded 123 frames in five seconds, T31 decoded 88 in seven
seconds at its inherited half cadence, and T41 decoded 151 in six seconds.
T41's frame remained visually unchanged after sharing its set-format and QBUF
UV-offset calculation. No kernel or ioctl fault signature appeared in the
final `dmesg`, `logread`, or bounded `logcat` captures.

The subsequent MDNS and private-format extraction corrected a concrete T31
pool mismatch. Before the change, the outer frame channel reported 4,700,160
bytes while the consumer allocated 3,133,440 bytes; both now report 3,133,440
bytes, matching the OEM's `2880 * align16(1080)` contract. The active
`isp_memopt=1` MDNS allocation remains exactly `0x2f8740`. T23 preserves its
full-layout `0x477e70` used size and `0x478000` page-aligned allocation.

Two clean boots per device covered the new module and a final re-armed
inspection state. T23 decoded 127 frames in six seconds and then 112 in five;
T31 decoded 125 in ten seconds and then 89 in seven at its inherited cadence;
T41 decoded 150 in six seconds and then 125 in five. All three passed
night/auto/day transitions, retained coherent geometry and stream continuity,
reported zero ISP interrupt errors, and had clean `dmesg`, `logread`, and
bounded `logcat` captures. A later human-subject T41 check exposed two tuning
defects that those transport smokes could not establish: red rendered pink
and a short exposure under 120 Hz recessed LEDs produced a five-frame moving
shadow cycle.

The checked DMA-binding extraction then passed the same two-boot cycle on all
three devices. T23 decoded 125 main frames in six seconds, 73 substream frames
in three, and 99 main frames in the four-second final check. T31 decoded
120/10 seconds, 65/5 seconds on the substream, and 88/7 seconds after the clean
reboot at its inherited cadence. T41 decoded 150/6 seconds, 75/3 seconds on
the substream, and 103/4 seconds after the clean reboot. No active buffer was
rejected; geometry, stream continuity, and the T31
3,133,440/353,280-byte pool contracts were unchanged. Every device passed
night/auto/day and had clean `dmesg`, `logread`, and bounded `logcat`
captures. Color fidelity is tracked separately from these DMA-layout results.

## Next Extraction Targets

- Extend callback plans to repeated, device-proven initialization and teardown
  sequences while preserving exact per-SoC order.
- Extract common frame/channel bookkeeping now that command/event contracts
  and wire layouts are explicit, while keeping the T23/T41 recovered object
  layouts local until their offsets have typed checks.
- Expand ordered profiles to other evidence-backed, sensor-specific tuning
  corrections.
- Continue moving the generation-specific 64-bit divide and remaining
  fixed-point interpolation entry points behind host tests.
- Reconnect T31's host-tested common 64-bit multiplier only through a
  size-neutral boundary; the first additional inline layer changed recovered
  AE code generation and visibly increased wall grain.
- Extend tuning descriptors only when the matching userspace payload size and
  direction are proven; keep SoC dispatch and collectors local.
- Extract typed buffer ownership and queue-lifecycle bookkeeping only after
  matching the now-device-validated one-buffer T31 pre-dequeue semantics and
  the T23/T41 paths.
