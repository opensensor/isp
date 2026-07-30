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
| Checked NV12 and MDNS layouts | `tx_isp_frame_layout.h` | `common/tx_isp_frame_layout.c` | T23, T31, T41 |

The common implementation is linked into each TX-ISP module rather than
loaded as another kernel module. This preserves the exported symbol and
dependency ABI expected by the matching sensor drivers and userspace.

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

`tests/tx_isp_frame_abi_test.c` checks every important wire offset, total size,
prefix size, persistent/queue flag merging, each recovered state transition,
overlapping-policy priority, and out-of-range states.

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
- Extract common frame/channel bookkeeping after the T23/T41 recovered object
  offsets have typed checks.
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
