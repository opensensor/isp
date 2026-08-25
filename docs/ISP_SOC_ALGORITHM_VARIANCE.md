# ISP SoC Algorithm Variance: T23, T30, T31, T40, and T41

## Scope

This document compares algorithmic behavior in the device-proven T23, T30,
T31, T40, and T41 work. It deliberately separates:

- a capability or data format imposed by silicon;
- the algorithm shipped in an OEM module;
- the algorithm currently safe and active in the open driver; and
- a prediction about image quality or performance.

Those are not interchangeable. A missing or unsafe recovered function is not
evidence that the hardware cannot support the feature.

**T20 is deferred.** It is not included in matrices, unification decisions, or
performance rankings until its live statistics, sensor control, output path,
and representative image behavior have been proven to the same standard as
the five generations above. T21 recovery also remains outside this
device-proven algorithm comparison.

## Confidence Labels

- **PROVEN** - observed on a live device, or covered by a direct hardware and
  consumer test recorded in this repository.
- **SOURCE** - established by OEM/recovered source, binary analysis, or a
  host-tested implementation, but not sufficient by itself to claim live OEM
  parity.
- **HYPOTHESIS** - a reasoned performance or design prediction that needs a
  controlled device comparison.

Absence from recovered source is never a hardware finding. A true hardware
limit needs register, DMA, interrupt, timing, or output evidence.

## Device Evidence Inventory

The comparison is not based on one sensor per generation. The August 25, 2026
inventory includes the following live open-driver targets:

| SoC / board class | Sensor | Live evidence relevant to this document |
|---|---|---|
| T23N / Cinnado D1 | SC2336 | Streaming, 15 x 15 AE/AWB statistics, bounded AE, calibrated AWB, gain fanout, day/night/auto, and dual MSCA output |
| T30X / Wyze Video Doorbell v1 | SC4236 | Open ISP output frames, live firmware IRQ/statistics path, tuning-derived AE/color work, balanced exposure allocation, and active anti-flicker investigation |
| T31L camera | SC2336 | Main/sub streaming, exposure writes, day/night/auto, gain-dependent tuning, MDNS, frame ABI, and stock/open register comparisons |
| T31X / Wyze Cam v3 | GC2053 | Main/sub streaming at 30 fps, live AE recovery, exact CLM-bank comparison, AWB-zone diagnosis, and stock/open image oracle |
| T31X / Wyze Video Doorbell v2 | SC301IOT | Fully open ISP/OpenIMP streaming, sensor-derived driver configuration, live histogram/zone AE, daylight stock/open comparison, and the highlight-retention regression/fix |
| T40XP / Wyze Cam 3 Pro | GC4653 | Streaming, statistics and fallback metering, two-level AE, candidate AWB, gain-driven DNS/MDNS work, and stock/open hardware-state comparisons |
| T41NQ / Wyze Cam v4 | OS04D10 | Dual-stream output, exact histogram AE, packed AWB, anti-flicker exposure planning, gain fanout, scaler recovery, V4L2, and day/night tuning |

The SC301IOT row represents three separate doorbells exercised in the fleet
archive, not repeated captures from a single unit. Sensor diversity is also
material: T31 behavior has been exercised with SC2336, GC2053, and SC301IOT.
Image-quality depth is not identical across those targets, so later claims
still name the sensor and the exact behavior that was proven.

This inventory was checked against the sibling lab archive at
`~/re-framework/logs` and `~/re-framework/artifacts`, including
`t23-device-20260716-*`, `t30-vdb1/20260825-cycle-*`,
`20260816-vdb2-fleet-*`, `20260606-*-t40-*`, and `20260806-*-t41-*`.
Those archives supplement the tracked per-driver READMEs; they are not a
substitute for moving durable conclusions into this repository.

## Common Architecture

All five generations follow the same fundamental control pattern:

```text
sensor pixels -> CSI/VIC -> ISP pixel hardware -> output DMA
                                |
                                +-> statistics hardware -> DMA/IRQ
                                                           |
                                                           v
                                                CPU software algorithm
                                                   |              |
                                                   v              v
                                              sensor I2C     ISP registers
```

The ISP performs pixel-rate transforms and accumulates compact statistics in
hardware. AE/AWB/AF policy is software. DMA transports pixels or statistics;
it does not decide exposure, white balance, denoise strength, or tone curves.

## Executive Comparison

| SoC | Statistics and hardware contract | Active open control policy | Principal strength | Principal risk | Confidence |
|---|---|---|---|---|---|
| T23 | 15 x 15 AE/AWB zones; four-bank statistics; T23-specific input selector and one-shot/rearm behavior | Bounded discrete AE ladder every 32 snapshots; calibrated SC2336 AWB model and history | Auditable, stable, sensor-verified decisions | Coarse AE steps and slow cadence can trade motion/noise optimality for safety | PROVEN on T23/SC2336 |
| T30 | Apical-style 256-bin compressed histogram; firmware-state-machine ABI | Tuning-target histogram AE, balanced integration/gain partition, requested 50/60 Hz quantization; calibrated simple AWB path | Rich dynamic range in compact histogram and flexible exposure partition | Current open control is still being aligned to stock shutter/flicker behavior; recovered full FSMs are not all trusted | PROVEN streaming/statistics; SOURCE for OEM parity |
| T31 | Linear 21-bit 256-bin histogram plus 15 x 15 RGB zone statistics; AE0 and WDR AE1 | Full Tiziano zone/hist metering, EV convergence, scene-tail weighting, anti-flicker allocation, and gain fanout | Most complete recovered adaptive loop and tuning interaction | Stateful control is sensitive to encoding, calibration, and delayed sensor effects | PROVEN across SC2336, GC2053, and SC301IOT, with sensor-specific parity depth |
| T40 | Packed AWB groups and ISP statistics exist, but the recovered stream can lose statistics after restart | Pragmatic userspace two-level AE and bright-neutral candidate AWB; gain-driven DNS refresh | Fine integration trim reduces visible gain-step breathing; candidate AWB resists dominant scene colors | Statistics lifetime defect and userspace/output sampling add latency and fragility | PROVEN on T40/GC4653, with known stats-death |
| T41 | Exact 256-bin histogram in rotating pages; packed AWB group records; extended TMO/LCE/IVDC/output contracts | Bounded safe histogram AE with shared anti-flicker planner; calibrated fixed AWB by default, guarded adaptive AWB optional | Bounded control on higher-resolution path; richer tone/scaler hardware | Full recovered vendor 3A is unsafe; current adaptive fanout is intentionally partial | PROVEN on T41/OS04D10 |

This table describes the current open path, not the maximum potential of the
silicon. T40 and T41 in particular run conservative replacement algorithms
because recovery safety, not hardware capability, is the limiting factor.

## Variance by Algorithm

### 1. AE statistics and metering

**T23 - PROVEN.** The active source-derived setup supplies a 15 x 15 AE grid
with verified DMA format and cadence. Its open controller derives a luma value
from that zone path. The T23 selector at `0xb004` is material: applying the
T31-derived selector leaves T23 statistics empty. That selector and the DMA
contract belong in a T23 hardware adapter, not in generic AE policy.

**T30 - PROVEN format and live path; SOURCE for full OEM policy parity.** T30 exposes a 256-bin Apical
histogram whose 32-bit words encode a 12-bit mantissa and 4-bit exponent. The
open decoder reconstructs the approximate count before calculating a weighted
mean and highlight-tail response. Compression is a hardware/ABI format;
highlight weighting is software policy.

**T31 - PROVEN.** AE0 provides a linear 256-bin histogram with 21-bit counts
and a separate 15 x 15 R/G/B zone stream. `ae0_weight_mean2()` combines zone
weights and scene ratios; the histogram tail modifies the solver input. The
statistics are hardware, while both operations are MIPS software despite the
historical `Tiziano_ae0_fpga()` name.

The driver and output path have been exercised on SC2336, GC2053, and
SC301IOT. The current full histogram/zone convergence and highlight-retention
claim is strongest on SC301IOT; GC2053 additionally exposed a GIB/AWB-zone
interaction, while SC2336 supplied extensive mode, exposure, MDNS, and
stock/open register evidence. These are complementary hardware tests, not one
blanket image-parity claim.

**T40 - PROVEN with limitation.** The hardware can deliver statistics, and
the packed AWB format has 16-byte records on 128-byte group strides. The open
recovery can lose the statistics path after a second stream-on, so the working
agent can meter sampled NV12 output instead. This is a current driver/lifecycle
failure, not proof of a T40 hardware limitation.

**T41 - PROVEN.** Safe AE drains one of four histogram pages selected by
status register `0x19050`. The 256 bins are exact linear counts in the active
path. Safe AWB must consume all eight 16-byte records in each 128-byte zone
group; sampling record zero alone creates ring-phase-dependent color flashes.

**Design consequence.** A common AE algorithm must consume a canonical
statistics object produced by generation-specific decoders. It must not read
MMIO or assume a DMA layout directly.

### 2. Exposure convergence and allocation

Metering answers "how bright is the image?" Convergence decides how quickly to
change total exposure. Allocation decides how much of that exposure comes from
integration time, sensor analog gain, sensor digital gain, and ISP digital
gain. Identical still-frame brightness does not imply equivalent AE.

**T23 - PROVEN.** The active controller chooses from a sorted SC2336 exposure
ladder: short unity-gain integration, the verified boot and maximum integration
states, then nine verified analog-gain states. It targets tuning luma 60 with
deadband 5, updates every 32 statistics snapshots, and limits movement to four
rungs. Sensor I2C runs in process context. Gain-dependent GIB, DMSC, DNS, and
sharpen updates occur only on analog-gain changes.

**T30 - SOURCE plus current live discovery.** The open controller computes a
histogram mean, applies the tuning exposure-correction curve, maintains total
exposure in log2 space, and then uses the OEM-style balanced partition:
integration and gain ceilings alternate around 10 ms/2x, 30 ms/4x,
60 ms/6x, and 100 ms/8x before consuming sensor limits. The 50/60 Hz request
now reaches the exposure planner. A local stock/open comparison showed the
important remaining behavior gap: stock held roughly 1/50-second integration
and added sensor gain, while open reached similar brightness with longer
integration and less gain. The latter can look fine in a still yet blur motion
and flicker badly. Do not claim parity until live integration/gain traces under
controlled 50 and 60 Hz light agree with the intended policy.

**T31 - PROVEN.** `ae0_tune2()` maintains EV-domain convergence state, tuning
targets, a luma FIFO, sensor-clamped limits, and anti-flicker nodes. It
distributes integration, analog gain, and ISP digital gain and accounts for
sensor application delays. Its scene and histogram-tail controls are
calibration-sensitive: a generic scalar setter that encoded zero as an active
level caused the rear-door sky to wash out after initially correct frames.

**T40 - PROVEN.** The userspace agent uses mains-safe integration rungs of
about 400 lines (one 120 Hz period for the tested 25 fps/1920-line timing),
with analog gain between rungs. A second level micro-trims integration within
plus or minus 80 lines in 16-line steps, keeps the offset within 20% of one
mains period, changes gain/rung only after fine range saturation, waits four
ticks, and uses a 5% deadband. This directly addresses visible GC4653 gain-step
breathing while accepting a small potential residual-flicker error.

**T41 - PROVEN.** The bounded safe controller computes a Q8 histogram mean,
updates every three histogram frames, uses proportional total exposure with a
deadband and bounded step, then calls the shared sensor-aware anti-flicker
planner. For the tested OS04D10 timing, 60 Hz nodes are derived from the
44,225-lines/second rate (369-line half-cycle base), not hard-coded as a
portable sensor constant.

**HYPOTHESIS.** For moving subjects, a flicker-safe shutter ceiling plus sensor
gain should generally outperform integration-priority AE, at the cost of more
noise. For static low-light scenes, longer integration should generally reduce
noise. The best policy is therefore selectable by scene/consumer, not one
universal split. Validate with motion MTF, noise, highlight clipping, and
banding measurements rather than visual stills alone.

### 3. AWB and scene selection

**T23 - PROVEN.** The active SC2336 branch uses calibrated zone ratios, a
tuning selection mesh, indoor illuminant distance tables, history, live-EV
weight selection, inverse-temperature interpolation, and OEM gain conversion.
It outperforms the older diagnostic gray-world fallback on the tested scene.

**T30 - SOURCE.** The open simple path uses calibration WB gains and
sensor/tuning coefficients. The recovered Apical AWB FSM exposes a more
elaborate architecture, but not every recovered writer is safe or complete.
Do not infer an AWB silicon deficit from those software gaps.

**T31 - PROVEN.** Hardware supplies zone color measurements; CPU algorithms
perform cluster/illuminant selection, color-temperature estimation, temporal
control, and gain/CT fanout. The tuning bank carries both statistics setup and
software model data.

**T40 - PROVEN.** Whole-frame gray world failed in a scene dominated by green
walls. The working agent divides the image into 12 samples, rejects clipped,
dark, or chromatically distant blocks, and uses the two brightest near-neutral
candidates. This improves scene robustness but can fail when no representative
neutral object exists.

**T41 - PROVEN.** The guarded adaptive controller aggregates every packed
record, rejects partially replaced groups with a 12.5% consistency gate, and
requires a persistent change over 32 samples. It remains opt-in because a
calibrated fixed day/low-light policy is more reliable on current scenes. This
is an open-algorithm maturity decision, not a hardware limitation.

**HYPOTHESIS.** T23's calibrated illuminant model should be more accurate than
gray world on colored scenes when its sensor tuning is correct. T40's bright
neutral candidate method may adapt better across untuned sensors, but has no
answer for a scene without neutral candidates. A common framework should allow
both selectors and share only decoding, rejection, temporal filtering, and
gain application.

### 4. Gain-, EV-, and color-temperature-dependent fanout

AE and AWB do more than write the sensor. They drive noise reduction,
demosaic, black/green correction, sharpening, tone mapping, gamma, CCM, and
BCSH interpolation.

- **T23 - PROVEN:** the active branch refreshes a reviewed GIB/DMSC/DNS/
  sharpen subset on gain changes and has dynamic BCSH CT/EV updates.
- **T30 - SOURCE:** `sensor_update_black()` and Apical FSM state consume total
  gain/exposure; more recovered fanout remains to be validated.
- **T31 - PROVEN:** the recovered Tiziano path has the broadest active gain,
  EV, and CT fanout of these open drivers.
- **T40 - PROVEN:** the userspace agent publishes `dns_gain_ev` so YDNS/YSP
  track gain; MDNS and other literal paths require their own validated state.
- **T41 - PROVEN for subset:** the safe path intentionally limits fanout to a
  selected GIB/DMSC/LSC/TMO/YDNS/SDNS mask plus separately controlled
  EV/gamma/TMO updates.

**Design consequence.** Common code should publish canonical `total_gain`,
`exposure_value`, and `color_temperature` events. Each SoC adapter should own
the safe consumer list, ordering, fixed-point ABI, and register transactions.
Blindly enabling every recovered consumer is unsafe.

### 5. Scaling, output, and dynamic-range blocks

These differences are adjacent to 3A because they affect the image being
metered and the image delivered to the consumer.

- **T23/T41 - SOURCE and device-tested output:** both use the shared checked
  polyphase coefficient generator with generation-local table/register
  adapters. T41 required the missing 0.75x Lanczos curve for sharp
  2560x1440-to-1920x1080 output. T23 retains its recovered four-tap correction.
- **T31 - PROVEN:** normal processed output is MSCA. VIC MDMA is an independent
  raw/debug path, not the frame-channel writer.
- **T40 - PROVEN:** processed output is also MSCA; enabling the legacy raw VIC
  MDMA ring can race/corrupt processed delivery. T40 has recovered LCE/defog/
  ADR/MDNS work with differing validation levels.
- **T41 - SOURCE/PROVEN subset:** the generation has extended TMO, LCE, IVDC,
  multi-channel, and output state. The richer block inventory raises potential
  dynamic-range and scaling quality, but also increases memory, sequencing,
  and recovery risk.

Some fixed-point behavior is an ABI compatibility constraint rather than a
silicon limit. For example, a legacy 32-bit intermediate wrap may need to be
preserved for OEM-equivalent coefficient generation even though a wider CPU
implementation is mathematically cleaner.

## What Can Be Unified?

The correct target is one framework with selectable policies, not one giant
algorithm full of SoC conditionals.

```text
generation hardware adapter
  -> canonical statistics
  -> selectable metering and target controller
  -> selectable exposure allocator
  -> sensor capability/timing adapter
  -> checked sensor application
  -> canonical gain/EV/CT events
  -> generation-local safe block fanout
```

### Software that should be common

1. **Sensor capability model** - minimum/maximum integration, line rate,
   gain domains/codes, application delays, frame rate, and bus operations.
2. **Statistics API** - canonical histogram and zone objects with frame ID,
   valid-population flags, color channels, and completeness information.
3. **Metering policies** - average/weighted mean, highlight-tail protection,
   ROI, candidate selection, clipping rejection, and confidence scoring.
4. **Convergence shell** - targets, deadbands, asymmetric slew, settle delays,
   and state reset on stream/mode changes.
5. **Exposure planners** - integration-priority, balanced, motion-priority,
   and flicker-safe node allocation selected by policy.
6. **Checked application** - translate continuous requests through the
   sensor's allocation callbacks and apply in process context.
7. **Adaptive event model** - total gain, EV, CT, day/night, WDR, and stream
   generation events with ordered consumers.
8. **Tests and traces** - replay recorded statistics and assert requested
   integration/gain, not just output brightness.

### Hardware contracts that stay in adapters

- statistics register maps, packing, bank/ring selection, DMA sizes, cache
  synchronization, IRQ bits, rearm rules, and cadence;
- top-bypass polarity, shadow-bank/latch behavior, reset and write ordering;
- ISP instance/channel count, WDR exposure paths, available TMO/LCE/IVDC and
  output engines;
- scaler tap count, phase count, coefficient width, hidden RAM, and commit
  sequence;
- sensor line timing, rolling-shutter limits, gain code maps, group hold, and
  application delays;
- fixed-point overflow behavior where it is part of the OEM ABI or a hardware
  register format.

### Software differences worth preserving as policies

- T23's discrete verified ladder is safer during recovery than an unbounded
  continuous solver.
- T30's balanced partition is a useful general motion/noise compromise.
- T31's tuning-rich zone/histogram solver is valuable where full calibration
  exists.
- T40's fine integration trim and neutral-candidate AWB solve concrete scene
  and gain-step problems.
- T41's bounded safe controller and conservative fanout are appropriate while
  the larger recovered state machine remains unsafe.

Unification should make those choices explicit and testable, not erase them.

## Performance Hypotheses and Testable Predictions

These are hypotheses, not rankings of the silicon:

| Question | Likely result | Required test |
|---|---|---|
| Best motion under mains lighting | T30 balanced/motion-biased or T41 shared flicker planner should beat long-integration allocation | Controlled moving target at equal output luma, 50 and 60 Hz |
| Lowest static low-light noise | A policy allowed to extend integration before gain should win if motion is absent | Fixed scene, equal luma, temporal/spatial noise and hot-pixel measures |
| Best calibrated mixed-light AWB | T23/T31 model-based selection should beat plain gray world | Color chart plus dominant colored backgrounds across CCT |
| Best untuned colored-scene robustness | T40 candidate selection may beat whole-frame gray world if neutral candidates exist | Scenes with/without neutral patches and colored occupancy sweeps |
| Fastest light-step response | T31 once-per-frame loop should react faster than T23's 32-snapshot or T40's low-rate userspace loop | Repeatable lux step, overshoot and settling-frame trace |
| Highest dynamic-range potential | T41's richer TMO/LCE path may lead once safely recovered and calibrated | Same sensor-class HDR chart, ghosting, local contrast, and highlight retention |

CPU use is expected to be modest for histogram/zone algorithms because
hardware has already reduced the frame. Userspace full-frame sampling, as in
the T40 fallback, has more bandwidth and latency cost. Actual CPU time must be
measured because recovered logging, cache maintenance, and excessive register
writes can dominate the arithmetic.

## Validation Gate for Cross-SoC Changes

A common algorithm is ready for a generation only when it demonstrates:

1. stable statistics across start, stop, and restart;
2. correct frame/bank identity and no partial DMA consumption;
3. sensor-derived timing and gain limits with no sensor-specific literals in
   generic code;
4. 50 and 60 Hz integration/gain traces under steady and changing light;
5. controlled light-step convergence without oscillation or runaway;
6. motion, noise, clipping, and color measurements at equal output luma;
7. day/night and, where supported, WDR transitions;
8. safe gain/EV/CT fanout and output on every active channel.

For T20, this entire gate remains pending; the document makes no claim about
its relative algorithm quality or hardware ceiling.

## Source Map

- T23 active algorithm and device evidence:
  [`../driver/t23/README.md`](../driver/t23/README.md) and
  `driver/t23/tx_isp_t23_core.c`
- T30 recovered architecture and active simple 3A:
  [`../driver/t30/README.md`](../driver/t30/README.md) and
  `driver/t30/tx_isp_t30_recovered.c`
- T31 boundary and AE flow:
  [`T31_ISP_ARCHITECTURE.md`](T31_ISP_ARCHITECTURE.md) and
  `driver/t31/tx_isp_tuning.c`
- T40 rolling live evidence:
  [`T40_TUNING_HURDLES.md`](T40_TUNING_HURDLES.md) and
  [`../driver/t40/README.md`](../driver/t40/README.md)
- T41 safe algorithms and device evidence:
  [`../driver/t41/README.md`](../driver/t41/README.md) and
  `driver/t41/tx_isp_t41_recovered.c`
- Shared exposure and scaler design:
  [`SHARED_DRIVER_LIBRARY.md`](SHARED_DRIVER_LIBRARY.md)
