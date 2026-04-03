# Open-Source ISP Driver for Ingenic T31

![Ingenic ISP Logo](./ingenic_isp.webp)

## Overview

This repository contains an open-source reimplementation of the Ingenic T31 ISP kernel driver (`tx-isp-t31.ko`) used on devices such as the Wyze Cam 3 and other T31-based cameras.

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
- [`docs/IMAGE_TUNING_PRD.md`](docs/IMAGE_TUNING_PRD.md) — plan for finishing image tuning and remaining work
- [`driver/REGMAP_ADR_YDNS.md`](driver/REGMAP_ADR_YDNS.md) — ADR / YDNS register-map notes
- [`driver/TX_ISP_VIDEO_S_STREAM_VERIFIED.md`](driver/TX_ISP_VIDEO_S_STREAM_VERIFIED.md) — stream-control verification notes
- [`external/ingenic-sdk/3.10/isp/t31/OEM_TUNING_BLOB_MANIFEST.md`](external/ingenic-sdk/3.10/isp/t31/OEM_TUNING_BLOB_MANIFEST.md) — current map of recovered vs synthetic tuning data

## Repository Layout

| Path | Purpose |
|---|---|
| `driver/` | Open-source T31 ISP kernel-driver implementation |
| `driver/include/` | Shared headers and data structures |
| `external/ingenic-sdk/` | Sensor and SDK reference material |
| `docs/` | High-level project documentation and planning |
| `OEM-tx-isp-t31.ko` | OEM reference kernel module |

Important driver files:

- `driver/tx-isp-module.c` — module init/exit, platform resources, shared register helpers
- `driver/tx_isp_core.c` — core probe, memory mappings, ISR path, first-frame logic
- `driver/tx_isp_tuning.c` — tuning subsystem, per-block init, parameter handling, image pipeline control
- `driver/tx_isp_csi.c` / `driver/tx_isp_vic.c` / `driver/tx_isp_vin.c` / `driver/tx_isp_fs.c` — CSI/VIC/VIN/frame-source subdevices

## Project Goals

1. Replace the proprietary ISP kernel driver on T31 devices
2. Preserve compatibility with Ingenic's `libimp.so`
3. Match OEM register sequencing and control behavior closely
4. Recover or reconstruct enough OEM tuning content for acceptable image quality
5. Document the hardware and bring-up process so the work is maintainable

## Requirements

- **Target SoC:** Ingenic T31
- **Kernel focus:** Linux 3.10.14-based vendor trees
- **Userspace ABI target:** Ingenic `libimp.so`
- **Sensor support model:** OEM-style sensor drivers and compatible sensor integrations from the Ingenic SDK ecosystem

## Build

Typical cross-compile command for the vendor 3.10.14 kernel environment:

```bash
make -C <kernel-src> M=$(pwd)/driver modules ARCH=mips CROSS_COMPILE=mipsel-linux-gnu-
```

Expected artifact:

- `driver/tx-isp-t31.ko`

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