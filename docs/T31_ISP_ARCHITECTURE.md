# Ingenic T31 ISP Architecture

## Purpose

This document defines the hardware/software boundary of the T31 camera path and
the architecture of the open `tx-isp-t31.ko` implementation. It is a durable
reference, not a bring-up log.

The short answer to "is AE hardware?" is: **the ISP hardware measures the
image; software decides the exposure**. T31 hardware accumulates AE statistics
and transports them through DMA. MIPS software meters those statistics, runs
the convergence and anti-flicker policy, and asks the sensor to apply
integration time and analog gain. The ISP also applies any requested digital
gain and other image-processing updates in hardware.

## Evidence Rules

Reverse-engineered names are useful labels, not proof of a physical block.
Use evidence in this order:

1. live register, DMA, IRQ, sensor-bus, and image observations;
2. OEM binary behavior and call/relocation structure;
3. recovered source and disassembly;
4. names, strings, and comments.

The OEM module remains the primary compatibility reference for register
values, sequencing, ioctl contracts, and error behavior. Live silicon wins
when a recovered name or decompilation implies a different boundary. In
particular, `Tiziano_ae0_fpga()` is ordinary kernel code executing on the MIPS
CPU; its historical name does not establish the presence of an FPGA.

## Physical Data Path and Control Loop

```text
Pixel path:

external sensor -> D-PHY/CSI -> VIC/VIN -> Tiziano ISP -> MSCA DMA -> frame channel
                                      \
                                       +-> VIC MDMA (independent raw/debug path)

Control path:

ISP statistics engines -> statistics DMA -> IRQ/event -> MIPS software 3A
        ^                                         |          |
        |                                         |          +-> sensor I2C
        +--------------- ISP register updates <---+
```

The subdevice graph used by the driver represents software ownership, routing,
and event delivery. It is not a floorplan of the silicon. For example, a graph
edge involving VIC and the ISP does not mean that processed frame-channel
buffers are written by VIC MDMA. Normal processed output is written by MSCA;
VIC MDMA is a separate capture path.

## What Is Hardware and What Is Software?

| Component | Execution domain | Responsibility |
|---|---|---|
| Image sensor | External hardware | Pixel integration, rolling readout, sensor analog gain, and often sensor digital gain |
| D-PHY / CSI-2 receiver | SoC hardware | Electrical receive, lane recovery, packet decode, and error reporting |
| VIC / VIN | SoC hardware plus kernel control | Input timing, Bayer stream admission, routing, and the independent VIC MDMA path |
| Tiziano pixel pipeline | ISP hardware | DPC, LSC, green balance, demosaic, color/tone processing, denoise, sharpening, and related per-pixel math |
| AE/AWB/AF statistics engines | ISP hardware | Histograms, zone accumulations, thresholds, counters, and focus metrics |
| MSCA, VIC MDMA, statistics DMA | SoC hardware | Move pixels or statistics; these engines do not choose exposure or white balance policy |
| IRQ/event plumbing | Hardware plus kernel driver | Report completion and schedule the appropriate consumer |
| Tiziano adaptive algorithms | MIPS kernel software | AE/AWB decisions, target interpolation, scene weighting, anti-flicker allocation, and gain/EV/CT fanout |
| Sensor driver | MIPS kernel software controlling external hardware | Validate sensor limits and translate requested integration/gain into sensor register writes |
| `libimp.so` / OpenIMP | MIPS user space | Configure streams, modes, tuning controls, and consume frames |

It is therefore directionally useful to call CSI, VIC, and ISP the main imaging
hardware blocks, but not literally complete. The SoC also contains D-PHY,
MSCA, DMA, interrupt, clock, and bus hardware. More importantly, "ISP" itself
contains both the per-pixel processing blocks and the metering blocks used by
software 3A.

## Driver Decomposition

| File | Primary role |
|---|---|
| `driver/t31/tx_isp_module.c` | Module lifecycle, platform devices, shared register helpers, IRQ adoption/dispatch |
| `driver/t31/tx_isp_core.c` | Core probe, MMIO mapping, ISP ISR, and first-frame bring-up |
| `driver/t31/tx_isp_csi.c` | CSI device and stream control |
| `driver/t31/tx_isp_vic.c` | VIC input and independent MDMA handling |
| `driver/t31/tx_isp_vin.c` | VIN-facing subdevice |
| `driver/t31/tx_isp_fs.c` | Processed frame-source channels presented to user space |
| `driver/t31/tx_isp_tuning.c` | ISP block programming, tuning ABI, statistics consumers, and software 3A |
| `driver/t31/tx_isp_subdev*.c` | Software graph, pad, and link management |
| `driver/t31/include/` | T31-local ABI, state, and register definitions |

The `tiziano_*` prefix does not identify an execution domain. Some functions
write hardware registers or LUTs; others are algorithms running on the CPU.
Read the data flow and side effects before classifying a function.

## Hardware Resources

| Resource | Address / IRQ | Role |
|---|---:|---|
| ISP core MMIO | `0x13300000` | Main ISP blocks, statistics engines, LUT windows, and MSCA controls |
| CSI window | `0x10022000` | CSI / D-PHY-facing controls |
| VIC window | `0x133e0000` | VIC input and MDMA controls |
| Primary IRQ | `37` | `isp-m0`, ISP events |
| Secondary IRQ | `38` | `isp-w02`, VIC events |

`tx_isp_init_memory_mappings()` maps the ISP core through `0x90000` so that
the high register/LUT windows used by the OEM sequence are covered.

## Bring-Up and Processed Output

The important ordering is:

1. `tx_isp_init()` allocates the shared device, registers platform devices and
   drivers, creates the tuning node, and builds the software graph.
2. `tx_isp_core_probe()` establishes mappings before tuning state can touch
   hardware.
3. `ispcore_core_ops_init()` activates the core and calls `tisp_init()` after
   the VIC input path is ready.
4. On the first frame, `mbus_to_bayer_write()` derives the CFA phase from the
   live sensor format and flip state, and `tisp_top_sel()` releases top-level
   processing.
5. MSCA writes processed output buffers and signals frame-channel completion.

Wrong CFA phase corrupts demosaic output. Wrong clock/reset/IRQ order can fail
silently. Enabling VIC MDMA is not required for normal processed output and
must not be used as a substitute for MSCA.

## ISP Pixel Processing and Statistics

### Per-pixel hardware

The major hardware-controlled families include DPC, GIB/GB, LSC, DMSC, CCM,
gamma, BCSH/CLM, sharpening, SDNS/MDNS/RDNS/YDNS, defog, ADR, and WDR-related
processing. Software loads their register banks and LUTs; hardware applies the
math at pixel rate.

The top bypass value is composed in
`tisp_compute_top_bypass_from_params()` and written to ISP register `0xc`.
Bypass polarity, shadow-bank selection, and write order are hardware-specific
contracts, even when the policy that selects a bank is software.

### Metering hardware

T31 AE0 hardware produces two complementary streams:

- a 256-bin primary histogram, stored as linear 21-bit counts in the first
  `0x400` bytes of a rotating `0x800`-byte histogram bank;
- a 15 x 15 zone-statistics grid containing channel sums and threshold counts,
  transported in a rotating `0x1000`-byte static-statistics bank.

AWB, AF, ADR, DPC, and WDR-related paths have their own statistics or event
contracts. Having a hardware statistics block does not imply that its adaptive
control algorithm is hardware.

## AE0 Per-Frame Flow

```text
AE0 static-stat IRQ
  -> select completed static DMA bank
  -> synchronize 0x1000 bytes
  -> ae0_interrupt_static() decodes/caches 15 x 15 zone data

AE0 histogram IRQ
  -> select completed histogram DMA bank
  -> synchronize 0x800 bytes
  -> ae0_interrupt_hist() decodes 256 histogram bins
  -> push AE event 1

AE event 1, once per frame
  -> tisp_ae0_process()
  -> Tiziano_ae0_fpga()                  [MIPS software]
     -> ae0_weight_mean2()               [MIPS metering/scene weighting]
     -> ae0_tune2()                      [MIPS convergence/allocation]
  -> request integration + analog gain  [sensor event / I2C]
  -> update ISP digital gain and gain-dependent blocks
  -> wait for the sensor-declared application delays
```

The software controller combines zone weighting with histogram-tail behavior,
uses tuning-derived targets and convergence state, constrains the result to
sensor-published integration/gain limits, and applies anti-flicker exposure
nodes. AE1 is the secondary statistics/control path used for WDR modes.

This answers a common misconception: the CPU does not normally inspect every
output pixel to run AE. Hardware reduces the frame to compact statistics; CPU
software performs the slower policy calculation once per statistics event.

## The Washed-Sky AE Failure

The rear-door SC301IOT test exposed a software policy bug, not missing AE
hardware. The first frame retained the calibrated scene behavior and rendered
the blue sky correctly. A later generic tuning write converted a zero
highlight/backlight request into an active level, disabling the calibrated
bright-tail response. AE then converged toward foreground brightness and
washed out the sky.

The permanent rule is:

- zero means no generic override and preserves the active sensor/tuning-bank
  calibration;
- an explicit non-zero request is translated through the OEM scalar level
  encoding;
- sensor timing, limits, delays, Bayer phase, and bus type come from the active
  sensor attributes rather than driver literals.

This is why a generic control plane must distinguish "no override" from an
encoded hardware or OEM algorithm level.

## Tuning and Runtime Switching

The active day, night, and WDR parameter banks are a recovered control plane.
They contain both calibration data loaded into hardware and thresholds/LUTs
consumed by CPU algorithms. A tuning blob is not firmware executed by the ISP.

Mode changes are ordered transitions rather than a single enable bit. They can
select new hardware banks and cause gain-, EV-, or color-temperature-dependent
software fanout. ABI-compatible structure layout and ioctl sizes must remain
stable for `libimp.so`, but algorithm internals can be made common where the
hardware contracts have adapters.

## Device Coverage and Limits of the Claim

The open T31 driver is not a one-sensor experiment:

- **T31L / SC2336:** main/sub streaming, exposure and gain-dependent tuning,
  day/night/auto, MDNS, frame ABI, and stock/open register comparisons;
- **T31X / GC2053:** main/sub streaming at 30 fps, live AE recovery, exact CLM
  bank comparison, and a GIB/AWB-zone failure isolated with a stock oracle;
- **T31X / SC301IOT:** fully open driver/OpenIMP streaming, sensor-derived ISP
  configuration, live histogram/zone AE, and stock/open daylight comparison.
  Three separate Wyze Video Doorbell v2 units have been exercised in the fleet
  archive.

The detailed histogram-tail regression in this document was reproduced and
fixed on SC301IOT. The other two sensors independently prove substantial
transport, mode, exposure, tuning, and image behavior, but do not turn that
SC301IOT result into universal image-quality parity. Night/IR, WDR, extreme
exposure, and both 50/60 Hz regimes still need sensor-specific controlled
comparisons. Each claim must name its sensor and be backed by attributes,
tuning bank, live statistics, sensor writes, and output frames.

## Related Documents

- [`ISP_SOC_ALGORITHM_VARIANCE.md`](ISP_SOC_ALGORITHM_VARIANCE.md) - proven
  cross-generation algorithm differences, unification boundaries, and
  performance hypotheses
- [`ISP_TUNING_MATH.md`](ISP_TUNING_MATH.md) - fixed-point and tuning math
- [`TX_ISP_PIPELINE_ARCHITECTURE.md`](TX_ISP_PIPELINE_ARCHITECTURE.md) -
  detailed T31 register, interrupt, and buffer reference
- [`../driver/t31/REGMAP_ADR_YDNS.md`](../driver/t31/REGMAP_ADR_YDNS.md) -
  ADR/YDNS register notes
- [`../driver/t31/TX_ISP_VIDEO_S_STREAM_VERIFIED.md`](../driver/t31/TX_ISP_VIDEO_S_STREAM_VERIFIED.md) -
  stream-control verification
