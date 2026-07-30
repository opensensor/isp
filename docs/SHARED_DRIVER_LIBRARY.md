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
table endpoints.

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
| T31 / SC2336 | 1920×1080, 25 fps configured | 640×360, 25 fps configured | day, night, auto | LSC/DMSC color and shading corrected; tuning readbacks and exposure telemetry stable |
| T41 / OS04D10 | 1920×1080 @ 25 fps | 640×360 @ 25 fps | day, night, auto | balanced `0x380/0x880` day AWB and dual-stream fanout |

The tested stock reserved-memory settings are sufficient: 22 MiB on T23/T31
and 30 MiB on T41. No bootloader environment change is currently required.

The T31 consumer currently delivers about 12.5 frames per second despite its
25-fps configuration. Historical stock evidence on the same T31/SC2336
pipeline advances about 408 frames per 30 seconds (roughly 13.6 fps) and also
uses a one-buffer frame-source pool. This is tracked as inherited consumer
cadence rather than a new open-driver regression; changing frame-completion
cadence requires a multi-buffer ownership test first.

The latest T31 cycle also corrected three proprietary tuning-ABI hazards:

- a sparse EV structure write that previously overran the kernel stack
- scalar routing that swallowed pointer-valued WB/highlight/backlight queries
- an incorrectly packed expression structure and frame-duration/line-duration
  mix-up that forced exposure and AE-luma telemetry to zero

The resulting T31 module survived 100 consecutive ISP/exposure query pairs,
day/night cycling, and RTSP decoding with no `dmesg`, `logread`, or `logcat`
ioctl/fault signature.

## Next Extraction Targets

- Extend callback plans to repeated, device-proven initialization and teardown
  sequences while preserving exact per-SoC order.
- Extract common frame/channel bookkeeping after the T23/T41 recovered object
  offsets have typed checks.
- Expand ordered profiles to other evidence-backed, sensor-specific tuning
  corrections.
- Continue moving pure fixed-point divide/log helpers behind host tests.
- Recover the remaining T41 sensor-registry bind/lifecycle parity.
- Define and host-test a shared scalar-versus-pointer tuning-command descriptor
  before sharing ioctl dispatch across SoCs.
