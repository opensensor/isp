# Ingenic T23 ISP Recovery

This directory is the T23 bring-up workspace for matching the OEM
`tx-isp-t23.ko` behavior on Linux 3.10.14 targets.

The initial `tx_isp_t23_recovered.c` file is the kbuild-compatible recovered
whole-driver seed from `tx-isp-t23-v1`, plus the local compatibility shims
needed to load the SC2336 T23 sensor module and start the Raptor pipeline.

Current smoke-test status:

- `sensor_sc2336_t23.ko` loads against the recovered module.
- `/dev/tx-isp`, `/dev/isp-m0`, `/dev/misc-ivdc`, and `/dev/framechan0..3`
  are present.
- `/proc/jz/sensor/sensor0` reports the SC2336 metadata expected by Raptor.
- Raptor starts and publishes stable H.264 streams on both MSCA channels.
- The recovered child-platform path has symmetric remove handlers for its
  allocated subdevices, IRQs, MMIO mappings, clocks, pads, and channel state;
  the module can be unloaded after the sensor and consumers are stopped.
- The source-derived SC2336 GIB, LSC, DMSC, Gamma, static AWB, BCSH, and CLM
  startup images produce a clean, artifact-free image with working ISP/VIC
  interrupts. The exact LSC image programs all 651 OEM mesh nodes. The CLM
  image follows the T23 startup CT of 5000 K and programs both OEM LUT banks.
- The full T23 MDNS temporal/spatial denoise path is enabled by default. Its
  473 tuning fields and 377 gain interpolations come from the T23 binary and
  SC2336 tuning blob, while the register packers are shared with the readable
  T31 implementation. `TX_ISP_SET_BUF` programs all OEM MDNS DMA planes and
  the stream-on path restores them after the core reset. Runtime total-gain
  changes refresh MDNS from process context with the OEM `0x100` hysteresis.
  A no-argument load has passed 256-frame RTSP tests with both luma and chroma
  filters active; `source_park_uninitialized_mdns=1` remains a diagnostic
  bypass.
- The T23 SDNS spatial denoise path is enabled by default. Its 123 tuning
  fields occupy the exact contiguous `0xbf18..0xd1d0` SC2336 payload that ends
  where MDNS begins. The register writers use the readable T31 packing with
  the T23 Gaussian-Y constant, and replace generated bodies that performed
  unaligned or out-of-bounds loads. Startup selects the non-WDR tables,
  commits the complete `0x8800..0x8b4c` image, and clears top-bypass bit 15.
  Runtime total-gain changes refresh the interpolation subset with OEM
  `0x100` hysteresis. Parked, explicitly active, and no-argument active device
  cycles passed full Raptor startup and RTSP checks; `source_sdns_internal_enable=0`
  retains a diagnostic top-level bypass while still validating register
  programming.
- The source-derived AWB statistics setup produces valid 15x15-zone data in
  all four DMA banks when top-bypass bit 25 is cleared. The T23 tuning blob's
  input selector (`0xb004` bit 16 set) is required; the T31-derived selector-0
  override leaves every T23 AWB DMA bank empty.
- Optional `source_ae_stats_init` programs the exact T23 AE0 15x15 statistics
  geometry and thresholds. The DMA format and event cadence are verified.
  `source_ae_force_packed` is a zero-default bring-up control that sends one
  packed integration/gain value through the real sensor-ops ioctl after
  stream-on. For the verified SC2336 gain codes, packed `0x0080059c` is unity
  gain at maximum integration and
  packed `0x00c0059c` and `0x0880059c` select 1.5x and 2x respectively. The
  matching OEM GIB and DMSC total-gain state is inferred automatically; a
  nonzero `source_total_gain_q16` remains available as an explicit override.
- Optional `source_ae_hlil` adds a bounded process-context exposure controller
  over those three verified states. Every `source_ae_hlil_interval` snapshots
  it moves one rung toward the tuning-derived luma target of 60, with a default
  deadband of 5. Sensor I2C never runs in IRQ context, and each transition
  reapplies the base DMSC image before its matching gain delta. Read-only
  counters expose runs, updates, dropped schedules, last luma, current state,
  raw Q10 EV, and status. This is the proven active branch, not the full
  recovered OEM AE solver, and requires `source_ae_stats_init=1`.
- An optional `source_awb_hlil` workqueue implements the active SC2336 branch
  of the T23 AWB algorithm: calibrated zone ratios, tuning-mesh weighting,
  indoor light-source distance-LUT weighting, distance refinement, history,
  live-EV CT-mesh selection, and OEM gain conversion. The current scene has no
  zones inside either calibrated light-source radius, so the added OEM stage
  is intentionally neutral there. The loop is stable, and the earlier Q12
  gray-world loop remains available only as a diagnostic fallback.
- The exact T23 CCM startup path applies the tuning blob's EV-derived
  saturation transform instead of writing the raw daylight matrix. It is
  stable and less extreme than the raw matrix, but the best verified startup
  still keeps the top-level CCM bypassed.
- `source_lsc_ct` selects a generated SC2336 lens-shading image. The default is
  the OEM 5000 K startup; 3300 K is an exact A-to-T interpolation retained for
  indoor color diagnostics.
- Static initialization has reached the same visual plateau as the T31/T40
  recovery work. Further bring-up uses `tx-isp-t23-hlil.txt` as the behavioral
  specification for the dynamic 3A event path; T31/T40 source and history are
  used only to recover names and intent where the T23 HLIL is ambiguous.

Build from a compatible Thingino T23 3.10.14 kernel tree with:

```sh
make -C <kernel-src> M=$(pwd)/driver/t23 ARCH=mips CROSS_COMPILE=<mipsel-prefix> modules
```

Expected artifact:

- `driver/t23/tx_isp_t23_recovered.ko`
