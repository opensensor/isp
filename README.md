# Open-Source TX-ISP Drivers for Ingenic T23, T31, and T40

![Ingenic ISP Logo](./ingenic_isp.webp)

## Overview

This repository contains open-source reimplementations of the Ingenic TX-ISP
kernel drivers for T23, T31, and T40 cameras. T31 is organized as a modular
driver; T23 and T40 are working recovered drivers that are being progressively
split into reviewed, reusable subsystems.

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
- reverse-engineered architecture and tuning docs now exist in-tree

### Still incomplete

- image quality is **not yet OEM-equivalent**
- the current main visible issue is false-color / green-magenta blob artifacts
- some tuning tables are still synthetic or only partially reconstructed
- several ISP blocks need additional parity work or better OEM-derived data

If you want the detailed status and finish plan, start with `docs/IMAGE_TUNING_PRD.md`.

## Key Documentation

- [`docs/T31_ISP_ARCHITECTURE.md`](docs/T31_ISP_ARCHITECTURE.md) — current hardware / driver architecture notes
- [`docs/DRIVER_REUSE_PLAN.md`](docs/DRIVER_REUSE_PLAN.md) — cross-SoC commonality map and staged reuse plan
- [`docs/IMAGE_TUNING_PRD.md`](docs/IMAGE_TUNING_PRD.md) — plan for finishing image tuning and remaining work
- [`driver/t31/REGMAP_ADR_YDNS.md`](driver/t31/REGMAP_ADR_YDNS.md) — ADR / YDNS register-map notes
- [`driver/t31/TX_ISP_VIDEO_S_STREAM_VERIFIED.md`](driver/t31/TX_ISP_VIDEO_S_STREAM_VERIFIED.md) — stream-control verification notes
- [`external/ingenic-sdk/3.10/isp/t31/OEM_TUNING_BLOB_MANIFEST.md`](external/ingenic-sdk/3.10/isp/t31/OEM_TUNING_BLOB_MANIFEST.md) — current map of recovered vs synthetic tuning data

## Repository Layout

| Path | Purpose |
|---|---|
| `driver/` | Per-SoC open-source ISP kernel-driver implementations |
| `driver/include/tx_isp/` | Reviewed cross-SoC interfaces and primitives |
| `driver/t23/` | T23 recovered driver and tuning data |
| `driver/t31/` | T31 ISP kernel-driver implementation |
| `driver/t31/include/` | T31-local headers and data structures |
| `driver/t40/` | T40 recovered driver and tuning data |
| `external/ingenic-sdk/` | Sensor and SDK reference material |
| `docs/` | High-level project documentation and planning |
| `OEM-tx-isp-t31.ko` | OEM reference kernel module |

Important driver files:

- `driver/t31/tx_isp_module.c` — module init/exit, platform resources, shared register helpers
- `driver/t31/tx_isp_core.c` — core probe, memory mappings, ISR path, first-frame logic
- `driver/t31/tx_isp_tuning.c` — tuning subsystem, per-block init, parameter handling, image pipeline control
- `driver/t31/tx_isp_csi.c` / `driver/t31/tx_isp_vic.c` / `driver/t31/tx_isp_vin.c` / `driver/t31/tx_isp_fs.c` — CSI/VIC/VIN/frame-source subdevices

## Project Goals

1. Replace the proprietary TX-ISP kernel drivers on supported T23/T31/T40 devices
2. Preserve compatibility with Ingenic's `libimp.so`
3. Match OEM register sequencing and control behavior closely
4. Recover or reconstruct enough OEM tuning content for acceptable image quality
5. Document the hardware and bring-up process so the work is maintainable

## Requirements

- **Target SoCs:** Ingenic T23, T31, and T40
- **Kernel focus:** Linux 3.10.14 vendor trees (T23/T31) and Linux 4.4.94 vendor trees (T40)
- **Userspace ABI target:** Ingenic `libimp.so`
- **Sensor support model:** OEM-style sensor drivers and compatible sensor integrations from the Ingenic SDK ecosystem

## Build

The local build helper selects a per-SoC driver with `SOC`:

```bash
SOC=t23 ./build_local.sh
SOC=t31 ./build_local.sh
SOC=t40 ./build_local.sh
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
