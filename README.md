<p align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="docs/assets/banner-dark.svg">
    <img src="docs/assets/banner.svg" alt="Open Ingenic - open source ISP driver &amp; libimp for Ingenic SoCs" width="560">
  </picture>
</p>

<h1 align="center">open-tx-isp</h1>

<p align="center">

[![license](https://img.shields.io/badge/license-GPLv3-blue)](#license)
[![SoCs](https://img.shields.io/badge/SoC-T10%20%C2%B7%20T20%20%C2%B7%20T21%20%C2%B7%20T23%20%C2%B7%20T30%20%C2%B7%20T31%20%C2%B7%20T40%20%C2%B7%20T41-3e63dd)](#status)
[![status](https://img.shields.io/badge/open%20stack-device%20tested-30a46c)](#status)
[![branch next](https://img.shields.io/badge/branch-next-e5484d)](https://github.com/Lu-Fi/open-tx-isp/tree/next)
[![thingino](https://img.shields.io/badge/thingino-integrated-orange)](https://github.com/themactep/thingino-firmware)
[![platform](https://img.shields.io/badge/platform-MIPS%20%C2%B7%20Linux%203.10%20%26%204.4-lightgrey)](#build)
[![last commit](https://img.shields.io/github/last-commit/Lu-Fi/open-tx-isp/next)](https://github.com/Lu-Fi/open-tx-isp/commits/next)
[![PRs welcome](https://img.shields.io/badge/PRs-welcome-brightgreen)](https://github.com/Lu-Fi/open-tx-isp/pulls)

</p>

Open reimplementation of Ingenic's closed TX-ISP kernel driver (`tx-isp-<soc>.ko`) for
T10, T20, T21, T23, T30, T31, T40 and T41. The goal is behavioural equivalence with the
OEM driver: same ioctl and `libimp` ABI, same register sequencing, same image behaviour,
with cleaner unload/reload, checked inputs and less memory.

It works with Ingenic's unmodified `libimp.so` and with the open replacement
[OpenIMP](https://github.com/Lu-Fi/openimp). Open stack = open-tx-isp + OpenIMP + a streamer
([timps](https://github.com/Lu-Fi/timps)). This fork tracks
[opensensor/open-tx-isp](https://github.com/opensensor/open-tx-isp).

This is a reverse-engineering and compatibility effort, not a greenfield pipeline: OEM binary
analysis, `libimp` ABI work and recovery of tuning data.

## Status

**State: RC1 v2026.10.11-rc1: openimp claude/rc1 (agg-35 687a68e4 + docs), open-tx-isp 6e464ac7.** Release SoCs: T10, T20, T21, T23, T31; T30, T40 and T41 are experimental. The agg-35 soak of 2026-10-10/11 on 10 cameras was clean (0 oopses, 0 unexplained reboots, 0 streamer restarts); recommended streamer: timps v1.10.0. Release notes and known issues: [CHANGELOG.md](CHANGELOG.md). The per-SoC table below dates from 2026-10-04. "Fully open" = open driver, OpenIMP and streamer run from a flashed image.

| SoC | Status |
|---|---|
| T10 | Fully open; day/night, reload (5 cycles) and boot guard verified. Image controls partly documented. Module 731 KB. |
| T20 | Fully open; 1 h 44 min soak without errors, 10x stop/start and reload without an oops, `rmmod` during streaming refused. Module 736 KB. |
| T21 | First open bring-up, now fully open; AE/ADR/defog/AWB lifted from the vendor module. Module 452 KB (vendor 616). |
| T23 | Fully open (native encoder in OpenIMP); module 622 KB (vendor 857); vendor AE default. Frequent Helix frame drops fixed (residual interrupt, kernel patch merged upstream); still open: a rare single Helix encode error (errno 5), no real WDR. |
| T30 | Builds against a real T30 kernel; earlier bring-up on hardware. Not exercised in the latest campaign. |
| T31 | Reference SoC; SC2336, GC2053, SC301IOT; 2 h 53 min soak without errors; module 711 KB (vendor 829). |
| T40 | Device-tested earlier (T40XP/GC4653); statistics restart stability is a known limitation. Not in the latest campaign. |
| T41 | Fully open from a flashed image (H.264, H.265); reload verified (10/10); module 80 KB smaller than before. Open: flip, night column noise. |

Per feature and SoC:
[FEATURE_MATRIX](https://github.com/Lu-Fi/openimp/blob/next/docs/FEATURE_MATRIX.md).
History: [CHANGELOG.md](CHANGELOG.md).

## Better than the vendor driver

- Reload and stop: 10 stop/start and rmmod/insmod cycles without an oops on T20/T21/T23/T31/T41 (T10: 5 cycles).
- T20/T10 give back about 8 MB RAM (unused 8 MiB V4L2 frame pool off by default); on a T20 the open stack measured 10.5 % streamer CPU against 25.2 % with the vendor stack, snapshots 0.20 s against 0.45 s.
- Smaller modules than the vendor on T21, T23 and T31.
- Checked inputs (all user copies, QBUF window, bounded waits), sensor module pinned while streaming.
- T21 controls and noise reduction act (the vendor ignores them), T10/T20 noise-reduction strength acts.
- Shortfall logging with a concrete parameter value when reserved memory is too small.

Details and streamer integration notes:
[OPENIMP_BEYOND_VENDOR](https://github.com/Lu-Fi/openimp/blob/next/docs/OPENIMP_BEYOND_VENDOR.md).

## Build

Out-of-tree kernel modules, built against a Thingino buildroot output (toolchain and vendor
kernel tree, Linux 3.10.14 for T10-T31, 4.4.94 for T40/T41). The helper picks the SoC with `SOC`:

```sh
SOC=t31 ./build_local.sh          # t10 t20 t21 t23 t30 t31 t40 t41
TH=/path/to/thingino-firmware SOC=t23 ./build_local.sh
```

`TH`, `ROOT`, `KDIR`, `CROSS` and `ARCH` can be overridden (see the comments in `build_local.sh`).
It builds `driver/<soc>/` into `tx-isp-<soc>.ko` plus the diagnostic `driver/tx_isp_trace.ko`; when no
module for the SoC was built locally the result is a compile baseline only. The T20 driver expects the
`external/ingenic-sdk` submodule (`git submodule update --init`). Host tests for kernel-independent
primitives: `make -C tests check`.

## Integration in Thingino

The packages `open-tx-isp` (this driver) and `openimp` (userspace) are in the upstream
[thingino-firmware](https://github.com/themactep/thingino-firmware) branch `aperto`
([#1756](https://github.com/themactep/thingino-firmware/pull/1756)), selected with
`BR2_PACKAGE_THINGINO_ISP_OPEN` (menu "ISP stack") and pinned by commit SHA to the `next` branches
of the Lu-Fi forks. The module is installed as `tx-isp-<soc>.ko` and replaces the proprietary one;
SDK sensor, audio and AVPU modules stay. The kernel VPU/rmem stability patches
([#1748](https://github.com/themactep/thingino-firmware/pull/1748),
[#1752](https://github.com/themactep/thingino-firmware/pull/1752)) are merged there. The optional
boot guard `BR2_PACKAGE_THINGINO_ISP_GUARD` ([#1749](https://github.com/themactep/thingino-firmware/pull/1749),
default off; `isp_open=auto|manual|off`) skips the ISP/sensor modules after an unstable load so a bad
driver cannot boot-loop the camera. Once the first date tag exists on `aperto`, thingino's `aperto` branch will pin that tag instead of a SHA.

## Branches and releases

- `main`: fork default branch, not the tested stack.
- `next`: tested integration branch; everything on it was flashed and checked on cameras.
- `aperto`: release branch; fast-forward only from `next` after a clean soak (planned, not created yet; the first tag follows after the 24 h soak that started 2026-10-04). It carries the date tags, and thingino's `aperto` branch pins the tag.
- Tags `vYYYY.MM.DD` on `aperto` (planned).
- Work happens on `claude/<topic>` branches, merged into `next` after device tests.

## Documentation

- [Wiki](https://github.com/Lu-Fi/openimp/wiki) (one wiki for both repositories): module parameters, memory (rmem, ispmem, MMAP pool), troubleshooting, install and boot guard, release scheme.
- [`docs/T31_ISP_ARCHITECTURE.md`](docs/T31_ISP_ARCHITECTURE.md): hardware and driver architecture
- [`docs/ISP_SOC_ALGORITHM_VARIANCE.md`](docs/ISP_SOC_ALGORITHM_VARIANCE.md): algorithm differences between SoCs
- [`docs/DRIVER_REUSE_PLAN.md`](docs/DRIVER_REUSE_PLAN.md), [`docs/SHARED_DRIVER_LIBRARY.md`](docs/SHARED_DRIVER_LIBRARY.md): shared code
- [`docs/IMAGE_TUNING_PRD.md`](docs/IMAGE_TUNING_PRD.md): image tuning plan
- [`docs/ISP_PERFORMANCE_BENCHMARK.md`](docs/ISP_PERFORMANCE_BENCHMARK.md): on-device CPU/memory baseline
- [`docs/V4L2_CAPTURE_PATH.md`](docs/V4L2_CAPTURE_PATH.md): V4L2 capture architecture
- [`docs/INTERRUPT_DEBUG_GUIDE.md`](docs/INTERRUPT_DEBUG_GUIDE.md): interrupt debugging
- [`docs/re/`](docs/re): reverse-engineering dumps (T31 vendor module HLIL)
- Per SoC: `driver/t10/README.md`, `driver/t20/README.md`, `driver/t21/README.md` (+ `COMPARATIVE_ANALYSIS.md`), `driver/t23/README.md`, `driver/t30/README.md`, `driver/t31/README.md`, `driver/t40/README.md`, `driver/t41/README.md`

Layout: `driver/<soc>/` per-SoC driver, `driver/common/` and `driver/include/tx_isp/` shared code and
interfaces, `docs/` notes, `tests/` host tests and oracle checks, `tools/` on-device probes and generators,
`sensor-src/` sensor sources used by the T10/T20 builds.

## Reporting problems

Open an issue at [Lu-Fi/open-tx-isp](https://github.com/Lu-Fi/open-tx-isp/issues) (kernel driver, ISP, memory) or [Lu-Fi/openimp](https://github.com/Lu-Fi/openimp/issues) when unsure. Please include the SoC and sensor, the revisions of open-tx-isp, OpenIMP and the streamer, `dmesg` (including any shortfall line such as `set ispmem >= N KB`), the streamer log, the stream set and the `rmem`/`ispmem` values. Do not post addresses, credentials or location names. Details: [Troubleshooting](https://github.com/Lu-Fi/openimp/wiki/Troubleshooting#reporting-a-problem).

## Contributing

Contributions are welcome when grounded in OEM binary analysis, `libimp` ABI validation, hardware
logs or captures, tuning/calibration recovery or documentation. For behavioural changes state the OEM
evidence, the files changed, how it was validated and what remains uncertain.

## Acknowledgments

Prior art from the Ingenic / Thingino reverse-engineering community, especially
[thingino-firmware](https://github.com/themactep/thingino-firmware),
[ingenic-sdk](https://github.com/themactep/ingenic-sdk) and
[OpenIMP](https://github.com/opensensor/openimp), and the upstream
[opensensor/open-tx-isp](https://github.com/opensensor/open-tx-isp).

## License

This project is licensed under the GNU General Public License (GPLv3).

---

<sub>Not affiliated with or endorsed by Ingenic Semiconductor. "Ingenic" is used only to name the SoCs this project supports.</sub>
