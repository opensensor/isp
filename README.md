# Open-Source TX-ISP Drivers for Ingenic T23, T31, T40, and T41

![Ingenic ISP Logo](./ingenic_isp.webp)

## Overview

This repository contains open-source reimplementations of the Ingenic TX-ISP
kernel drivers for T23, T31, T40, and T41 cameras. The active cross-SoC
refactoring work targets all four device-tested drivers. T31 is organized as a
modular driver. T23, T40, and T41 retain large recovered core sources, but
their modules now have separate adapters for shared facilities where
applicable.

The project goal is **behavioral equivalence with the OEM driver**, so Ingenic's proprietary user-space library `libimp.so` can run unmodified against the open-source driver.

This is not a greenfield camera pipeline. It is a reverse-engineering and compatibility effort that combines:

- open-source kernel-driver development
- OEM binary analysis
- `libimp.so` ABI compatibility work
- image-quality tuning and calibration recovery

## Current Status

The project has moved well beyond initial bring-up.

### Working today

- kernel module architecture is in place
- major ISP subdevices exist and probe
- core MMIO mapping and IRQ ownership are understood
- stream bring-up is functional enough for live video
- tuning infrastructure and many ISP blocks are implemented
- common interpolation/fixed-point primitives are used by T23, T31, and T41
- T23, T31, and T41 share one typed sensor-registry implementation
- T31 and T41 share a configurable frame-boundary day/night state machine
- T23 and T31 share ordered register-profile and bypass-mask primitives
- T23 and T31 share validated ordered callback plans for tuning sequences
- T23, T31, and T41 share checked proprietary tuning wire layouts, response
  packers, and scalar-versus-pointer command descriptors
- T23, T31, and T41 share overflow-checked NV12 stride, private aggregate-line,
  UV-offset, and sizeimage calculation while retaining per-SoC alignment
  policy
- T23 and T31 share checked MDNS working/reference/UV/tiny-plane layout while
  retaining their distinct allocation ABIs and register ownership
- T23, T31, and T41 share checked NV12 DMA binding, including allocation
  length, complete 32-bit address-range, and Y/UV plane validation before QBUF
  reaches hardware
- the private frame-channel and future public V4L2 adapters now share an
  allocation-free queue core for buffer ownership, completion ordering,
  sequence/timestamp metadata, errors, and deterministic STREAMOFF recovery
- T23, T31, and T41 share the proven frame-channel event namespace and exact
  legacy-`V`/T41-`T` private ioctl envelopes without conflating the
  generation-specific events above buffer completion; the common contract
  also owns the fixed 20-byte request-buffer wire object and legacy stream
  command IDs
- T23, T31, T40, and T41 share checked 32-bit pad and active-link offsets,
  including the event callback slot used for remote frame-channel dispatch
- T31 applies evidence-backed SC2336 day/night DMSC correction profiles
- T23, T40, and T41 link recovered cores with logical shared-library adapter
  objects
- reverse-engineered architecture and tuning docs now exist in-tree

### Still incomplete

- image quality is **not yet OEM-equivalent**
- some tuning tables are still synthetic or only partially reconstructed
- several ISP blocks need additional parity work or better OEM-derived data
- broader sensor, WDR, flip, and exposure-range coverage remains incomplete

If you want the detailed status and finish plan, start with `docs/IMAGE_TUNING_PRD.md`.

## Key Documentation

- [`docs/T31_ISP_ARCHITECTURE.md`](docs/T31_ISP_ARCHITECTURE.md) — current hardware / driver architecture notes
- [`docs/DRIVER_REUSE_PLAN.md`](docs/DRIVER_REUSE_PLAN.md) — cross-SoC commonality map and staged reuse plan
- [`docs/SHARED_DRIVER_LIBRARY.md`](docs/SHARED_DRIVER_LIBRARY.md) — landed shared interfaces, adapters, invariants, and device matrix
- [`driver/t31/README.md`](driver/t31/README.md) — T31 file ownership, validated SC2336 state, tuning ABI, and known gaps
- [`docs/IMAGE_TUNING_PRD.md`](docs/IMAGE_TUNING_PRD.md) — plan for finishing image tuning and remaining work
- [`docs/ISP_PERFORMANCE_BENCHMARK.md`](docs/ISP_PERFORMANCE_BENCHMARK.md) — reproducible on-device CPU, memory, throughput, IRQ, and module-size baseline
- [`docs/T41_OPEN_PERFORMANCE_BASELINE_20260806.md`](docs/T41_OPEN_PERFORMANCE_BASELINE_20260806.md) — first 2.5K open-stack T41 baseline and configured-versus-delivered FPS finding
- [`docs/V4L2_CAPTURE_PATH.md`](docs/V4L2_CAPTURE_PATH.md) — additive V4L2 capture architecture, landed queue core, and adapter phases
- [`driver/t31/REGMAP_ADR_YDNS.md`](driver/t31/REGMAP_ADR_YDNS.md) — ADR / YDNS register-map notes
- [`driver/t31/TX_ISP_VIDEO_S_STREAM_VERIFIED.md`](driver/t31/TX_ISP_VIDEO_S_STREAM_VERIFIED.md) — stream-control verification notes
- [`external/ingenic-sdk/3.10/isp/t31/OEM_TUNING_BLOB_MANIFEST.md`](external/ingenic-sdk/3.10/isp/t31/OEM_TUNING_BLOB_MANIFEST.md) — current map of recovered vs synthetic tuning data

## Repository Layout

| Path | Purpose |
|---|---|
| `driver/` | Per-SoC open-source ISP kernel-driver implementations |
| `driver/include/tx_isp/` | Reviewed cross-SoC interfaces and primitives |
| `driver/common/` | Shared kernel implementation with explicit SoC adapters |
| `driver/t23/` | T23 recovered driver and tuning data |
| `driver/t31/` | T31 ISP kernel-driver implementation |
| `driver/t31/include/` | T31-local headers and data structures |
| `driver/t40/` | T40 recovered driver, shared-library adapters, and tuning data |
| `driver/t41/` | T41 recovered driver and tuning data |
| `external/ingenic-sdk/` | Sensor and SDK reference material |
| `docs/` | High-level project documentation and planning |
| `OEM-tx-isp-t31.ko` | OEM reference kernel module |

Important driver files:

- `driver/common/tx_isp_sinfo.c` — shared sensor registry and procfs lifecycle
- `driver/common/tx_isp_daynight.c` — configurable day/night transition shell
- `driver/common/tx_isp_callback_plan.c` — validated ordered callback execution
- `driver/common/tx_isp_reg_profile.c` — ordered register profiles and bypass-mask merge
- `driver/common/tx_isp_tuning_abi.c` — checked libimp envelopes, reply packers, and command descriptors
- `driver/common/tx_isp_frame_layout.c` — checked NV12 and T23/T31 MDNS geometry
- `driver/common/tx_isp_subdev.c` — checked graph endpoint resolution and
  generation-neutral pad-link validation, initialization, and connection
- `driver/common/tx_isp_remote_event.c` — checked pad-to-remote-handler route
  resolution shared by the recovered T23, T40, and T41 dispatchers
- `driver/common/tx_isp_state.c` — value-level recovered subdevice readiness
  policy with generation-local field adapters
- `driver/include/tx_isp/tx_isp_math.h` — shared fixed-point/interpolation primitives
- `driver/include/tx_isp/tx_isp_sinfo.h` — typed registry configuration and lifecycle interface
- `driver/include/tx_isp/tx_isp_subdev.h` — graph wire records, resolver
  interface, and shared link-state operations
- `driver/include/tx_isp/tx_isp_remote_event.h` — remote-event adapter,
  resolved-target, and failure-status contract
- `driver/include/tx_isp/tx_isp_state.h` — layout-independent subdevice state
  evaluation interface
- `driver/include/tx_isp/tx_isp_tuning_abi.h` — generation-aware proprietary control wire ABI
- `driver/include/tx_isp/tx_isp_frame_abi.h` — exact 32-bit frame-buffer wire layout and generation-aware state flags
- `driver/include/tx_isp/tx_isp_frame_channel.h` — shared frame-channel event IDs, generation-qualified ioctl envelopes, and ioctl decoders
- `driver/include/tx_isp/tx_isp_frame_format.h` — compiler-independent 112/116-byte frame-image format ABI
- `driver/include/tx_isp/tx_isp_frame_layout.h` — alignment-parametric NV12 and MDNS layout interface
- `driver/t23/tx_isp_t23_core.c` and adapter objects — T23 recovered core with shared math, registry, and register-profile facilities
- `driver/t31/tx_isp_module.c` — module init/exit, platform resources, shared register helpers
- `driver/t31/tx_isp_core.c` — core probe, memory mappings, ISR path, first-frame logic
- `driver/t31/tx_isp_tuning.c` — tuning subsystem, per-block init, parameter handling, image pipeline control
- `driver/t31/tx_isp_csi.c` / `driver/t31/tx_isp_vic.c` / `driver/t31/tx_isp_vin.c` / `driver/t31/tx_isp_fs.c` — CSI/VIC/VIN/frame-source subdevices
- `driver/t40/tx_isp_t40_recovered.c` and adapter objects — T40 recovered core
  with shared subdevice graph, remote-event, link-state, and readiness policy
- `driver/t41/tx_isp_t41_recovered.c` and adapter objects — T41 recovered core with shared day/night, math, and registry facilities

## Project Goals

1. Replace the proprietary TX-ISP kernel drivers on supported T23/T31/T40/T41 devices
2. Preserve compatibility with Ingenic's `libimp.so`
3. Match OEM register sequencing and control behavior closely
4. Recover or reconstruct enough OEM tuning content for acceptable image quality
5. Document the hardware and bring-up process so the work is maintainable

## Requirements

- **Active target SoCs:** Ingenic T23, T31, T40, and T41
- **Kernel focus:** Linux 3.10.14 vendor trees (T23/T31) and Linux 4.4.94 vendor trees (T40/T41)
- **Userspace ABI target:** Ingenic `libimp.so`
- **Sensor support model:** OEM-style sensor drivers and compatible sensor integrations from the Ingenic SDK ecosystem

## Build

The local build helper selects a per-SoC driver with `SOC`:

```bash
SOC=t23 ./build_local.sh
SOC=t31 ./build_local.sh
SOC=t40 ./build_local.sh
SOC=t41 ./build_local.sh
```

`ROOT`, `KDIR`, and `CROSS` can be supplied for the matching vendor kernel and
toolchain. See the comments in `build_local.sh` for details.

Host-side tests for shared, kernel-independent primitives run with:

```bash
make -C tests check
```

## Reverse-Engineering Workflow

The project works best when changes are driven by evidence, not guesswork.

Recommended workflow:

1. identify the relevant open-source code path in `driver/`
2. compare against the OEM binary behavior
3. confirm `libimp.so` expectations when ioctl or struct ABI is involved
4. make the smallest safe parity change
5. validate with logs, images, and targeted diffs

The new architecture and PRD docs capture the current high-level understanding so this work can continue systematically instead of rediscovering the same facts.

## What Makes This Hard

This project is solving several problems at once:

- hardware bring-up and clock/reset ordering
- platform/subdevice modeling
- reverse-engineering OEM register sequences
- reproducing runtime tuning behavior
- recovering missing calibration/tuning tables

Even when streaming works, image quality can still be wrong if one of the following is off:

- CFA/demosaic phase
- block enable/bypass state
- LUT programming path
- tuning table contents
- day/night or WDR bank selection

## Limitations

Current limitations are mostly in **image tuning parity**, not basic driver existence.

Known classes of remaining work include:

- early color-path parity (for example DMSC / GIB / LSC / YDNS interactions)
- OEM-calibrated table recovery for AE, CCM/BCSH/WB, ADR/WDR, and denoise banks
- mode-complete validation for day/night, WDR, and sensor flip combinations

## Contributing

Contributions are welcome, especially when they are grounded in one of these:

- OEM binary analysis
- `libimp.so` ABI validation
- concrete hardware validation logs/captures
- recovery of tuning/calibration data
- improvements to documentation and reproducibility

If you are making behavioral changes, please document:

- what OEM evidence supports the change
- which files/functions were updated
- how the change was validated
- any remaining uncertainty

## Acknowledgments

Thanks to the work and prior art from the broader Ingenic / Thingino / Wyze reverse-engineering community, especially:

- [thingino-firmware](https://github.com/themactep/thingino-firmware)
- [ingenic-sdk](https://github.com/themactep/ingenic-sdk)

## License

This project is licensed under the GNU General Public License (GPLv3).
