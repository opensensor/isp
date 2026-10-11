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
[![branch aperto](https://img.shields.io/badge/branch-aperto-e5484d)](https://github.com/opensensor/open-tx-isp/tree/aperto)
[![thingino](https://img.shields.io/badge/thingino-integrated-orange)](https://github.com/themactep/thingino-firmware)
[![platform](https://img.shields.io/badge/platform-MIPS%20%C2%B7%20Linux%203.10%20%26%204.4-lightgrey)](#build)
[![last commit](https://img.shields.io/github/last-commit/opensensor/open-tx-isp/aperto)](https://github.com/opensensor/open-tx-isp/commits/aperto)
[![PRs welcome](https://img.shields.io/badge/PRs-welcome-brightgreen)](https://github.com/opensensor/open-tx-isp/pulls)

</p>

Open reimplementation of Ingenic's closed TX-ISP kernel driver (`tx-isp-<soc>.ko`) for
T10, T20, T21, T23, T30, T31, T40 and T41. The goal is behavioural equivalence with the
OEM driver: same ioctl and `libimp` ABI, same register sequencing, same image behaviour,
with cleaner unload/reload, checked inputs and less memory.

It works with Ingenic's unmodified `libimp.so` and with the open replacement
[OpenIMP](https://github.com/opensensor/openimp). Open stack = open-tx-isp + OpenIMP + a streamer
([timps](https://github.com/Lu-Fi/timps)). The `aperto` branch carries the released open stack on top of the original `main` line; see
[Branches and releases](#branches-and-releases).

This is a reverse-engineering and compatibility effort, not a greenfield pipeline: OEM binary
analysis, `libimp` ABI work and recovery of tuning data.

## Status

**State: RC1 v2026.10.11-rc1: openimp fe9307c (agg-35 687a68e4 + docs), open-tx-isp 191be6c5 (agg-35 6e464ac7 + docs).** Release SoCs: T10, T20, T21, T23, T31; T30, T40 and T41 are experimental. The agg-35 soak of 2026-10-10/11 on 10 cameras was clean (0 oopses, 0 unexplained reboots, 0 application restarts). Release notes and known issues: [CHANGELOG.md](CHANGELOG.md). The per-SoC table below dates from 2026-10-04. "Fully open" = open driver, OpenIMP and streamer run from a flashed image.

Earlier state: the release candidate (`claude/agg-28` on top of `next`, 2026-10-06 evening). "Fully open" = open driver, OpenIMP and streamer run from a flashed image. Release plan: `next` goes to `aperto` (fast-forward) after the long soak and is tagged `vYYYY.MM.DD`; the first release covers T10 (not re-tested on the open stack today), T20, T21, T23 and T31; T41 is not part of it.

| SoC | Status |
|---|---|
| T10 | Fully open; day/night, reload (5 cycles) and boot guard verified. Image controls partly documented. Module 731 KB. **Not re-tested** on 2026-10-06 (shares the T20 firmware base). Front crop window kept while no downscaled channel is open and released with the tuning session (agg-34, device-tested). |
| T20 | Fully open; 1 h 44 min soak without errors, 10x stop/start and reload without an oops, `rmmod` during streaming refused. Module 736 KB. apitest of the release candidate: 212 PASS / 0 FAIL on two cameras. Same front-crop fix as T10 (agg-34); `isp-m0` reports the reachable analog gain cap (smaller of the sensor maximum and the `SetMaxAgain` ceiling), which the streamer needs for day/night (agg-34, host-tested). |
| T21 | First open bring-up, now fully open; AE/ADR/defog/AWB lifted from the vendor module. Module 452 KB (vendor 616). `SetBrightness` acts and sepia works (beyond vendor). apitest of the release candidate: 228 PASS / 0 FAIL on two cameras. |
| T23 | Fully open (native encoder in OpenIMP); module 622 KB (vendor 857); vendor AE default. Frequent Helix frame drops fixed (residual interrupt, kernel patch merged upstream); stock-like release defaults, MSCA scratch buffer, AF statistics on by default (see "T23 module parameters"). Open: **a FrameSource crop change stops the pipeline (under investigation, release blocker)**, a rare single Helix encode error (errno 5), no real WDR. Front crop (agg-34, device-tested): crop off unlocks the window, a window that does not fit is dropped instead of hanging the MSCA (the 2026-10-06 crop hang), the lock is released with the ISP session, module parameter `fcrop_upscale_pct`. |
| T30 | Builds against a real T30 kernel; earlier bring-up on hardware. Not exercised in the latest campaign. |
| T31 | Reference SoC; SC2336, GC2053, SC301IOT; 2 h 53 min soak without errors; module 711 KB (vendor 829). `SetFrameDrop` with the stock semantics. H.264 stall after JPEG teardown fixed (OpenIMP agg-29). **Front crop (agg-34):** the crash from a window the MSCA would have to upscale is fixed (the kernel refuses it with -EINVAL, module parameter `fcrop_upscale_pct`, default 0); the picture effect of a valid window is **experimental, not proven**. |
| T40 | Device-tested earlier (T40XP/GC4653); statistics restart stability is a known limitation. Not in the latest campaign. |
| T41 | **Not part of the first release (experimental).** Fully open from a flashed image (H.264, H.265); reload verified (10/10); module 80 KB smaller than before. Open: MSCA channel 1 scaling registers are staged (fix in branch `claude/t41-ch1-fix`, not merged), an output restart can hang the SoC, 38 tuning IDs missing, flip, night column noise. |

**Completeness** of the vendor API at RC1 (done = device-tested + host-tested + audited): T10 89.2 %, T20 90.5 %, T21 93.2 %, T23 93.8 %, T31 96.4 % (release), T41 59.3 %, T40 25.4 %, T30 not device-audited (experimental). Table: [OpenIMP README, Completeness](https://github.com/opensensor/openimp/blob/aperto/README.md#completeness). Release notes: [docs/RELEASE-v2026.10.11-rc1.md](docs/RELEASE-v2026.10.11-rc1.md).

Per feature and SoC:
[FEATURE_MATRIX](https://github.com/opensensor/openimp/blob/aperto/docs/FEATURE_MATRIX.md).
History: [CHANGELOG.md](CHANGELOG.md).

## T23 module parameters (release candidate defaults)

Defaults of `tx-isp-t23.ko` in the release candidate. The switches are `0644` module parameters (changeable at run time through `/sys/module/tx_isp_t23/parameters/`) except where noted; the stock-like set is the one that ran 7 h overnight plus many streamer restarts. Parameters marked "debug" exist for bisecting and are not needed in normal operation.

| Parameter | Default | Meaning |
|---|---|---|
| `chan_stop_keep_input` | 1 | Keep the input (CSI, sensor, VIC, TISP core) running after the last frame channel STREAMOFF while the ISP is streaming, as stock does; an on-demand wake then restarts no hardware. 0 stops the input with the last channel (debug; hangs together with `msca_fifo_rearm=1`) |
| `msca_keep_enabled` | 2 | What STREAMOFF does with the MSCA output enable bit: 0 clear, 1 keep when the input stops too, 2 always keep (stock style) |
| `msca_fifo_rearm` | 0 | 1 clears and refills a stopped channel's MSCA address FIFO at STREAMON (debug; stock never does) |
| `msca_flip_skip_noop` | 1 | No MSCA update request for unchanged mirror/flip bits |
| `msca_restart_skip` | 1 | No MSCA reload on STREAMON of a still-enabled output with unchanged geometry |
| `msca_session_release` | 1 | An ISP STREAMON with the input stopped switches off MSCA outputs left enabled and clears their FIFOs |
| `msca_scratch` | 1 | STREAMOFF parks a kept MSCA output on a scratch area at the tail of the ISP buffer (900 KiB at 720p), QBUFs of a parked channel are held until STREAMON, the input waits until the scratch address was consumed. Read-only counters `msca_scratch_parks`, `_frames`, `_deferred`, `_skips`, `_settle_timeouts`, `_settle_ticks_max`. 0 = off (first thing to try if a stop/start hangs) |
| `chan_stop_drain` | 21 | STREAMOFF waits up to N x 10 ms until the channel's queued buffers have left the hardware (stock: 21); 0 = no wait. Counters `chan_drain_waits`, `chan_drain_timeouts`, `chan_drain_ticks_max` |
| `qbuf_cache_inv` | 1 | QBUF invalidates the buffer's cache lines before its addresses reach the FIFO (as stock); 0 = off (debug) |
| `source_af` | 1 (read-only) | AF statistics chain of the stock module: AF block, AF interrupt and the AF getters/setters. 0 = AF off and the controls unrouted. No measurable CPU or interrupt cost |
| `source_ae_oem` | 1 (read-only) | Lifted vendor AE (default since 2026-10-03) |
| `crumbs` | 0 | Hang step markers in a reserved page (1 = rmem page after the MDNS buffer, 2 = page at `crumb_addr`); only for hang analysis |
| `t23_runtime_trace` | 0 | 1 enables informational driver logging |
| `fcrop_upscale_pct` | 0 | Percentage by which the front-crop geometry check tolerates an MSCA upscale (0 = no upscaling allowed). Measured on T23: upscaling up to 2.0 works, at 2.2 the MSCA stalled briefly and the crop was dropped without a reboot. The same parameter exists in `tx-isp-t31.ko` (writable under `/sys/module/tx_isp_t31/parameters/`); the T31 limit was not established, leave it at 0 |
| `isp_clk`, `isp_clka` | 153000000, 416000000 (load time only) | ISP core and AXI clocks in Hz; since the candidate they really reach the hardware (before, the activation path read the wrong clock-table slot) |

## Better than the vendor driver

- Reload and stop: 10 stop/start and rmmod/insmod cycles without an oops on T20/T21/T23/T31/T41 (T10: 5 cycles).
- T20/T10 give back about 8 MB RAM (unused 8 MiB V4L2 frame pool off by default); on a T20 the open stack measured 10.5 % streamer CPU against 25.2 % with the vendor stack, snapshots 0.20 s against 0.45 s.
- Smaller modules than the vendor on T21, T23 and T31.
- Checked inputs (all user copies, QBUF window, bounded waits), sensor module pinned while streaming.
- T21 controls and noise reduction act (the vendor ignores them), T10/T20 noise-reduction strength acts.
- Shortfall logging with a concrete parameter value when reserved memory is too small.

Details and streamer integration notes:
[OPENIMP_BEYOND_VENDOR](https://github.com/opensensor/openimp/blob/aperto/docs/OPENIMP_BEYOND_VENDOR.md).

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
`BR2_PACKAGE_THINGINO_ISP_OPEN` (menu "ISP stack") and pinned by commit SHA to the `aperto` branches
of the opensensor repositories. The module is installed as `tx-isp-<soc>.ko` and replaces the proprietary one;
SDK sensor, audio and AVPU modules stay. The kernel VPU/rmem stability patches
([#1748](https://github.com/themactep/thingino-firmware/pull/1748),
[#1752](https://github.com/themactep/thingino-firmware/pull/1752)) are merged there. The optional
boot guard `BR2_PACKAGE_THINGINO_ISP_GUARD` ([#1749](https://github.com/themactep/thingino-firmware/pull/1749),
default off; `isp_open=auto|manual|off`) skips the ISP/sensor modules after an unstable load so a bad
driver cannot boot-loop the camera. Once the first date tag exists on `aperto`, thingino's `aperto` branch will pin that tag instead of a SHA.

## Branches and releases

- `main`: the original author's line (opensensor). It is left untouched and is a strict ancestor of `aperto`.
- `aperto`: the open stack release line: OpenIMP + open-tx-isp, as used by the thingino `aperto` branch. Fast-forward only; every commit was flashed and checked on cameras. Releases are tagged `vYYYY.MM.DD` on this branch, and thingino pins a tag instead of a SHA.
- Development and the device-test campaign happen in the [Lu-Fi forks](https://github.com/Lu-Fi/open-tx-isp) (branch `next` = integration, `claude/<topic>` = topic branches); tested work reaches `aperto` from `next` after a clean soak.
- Companion repository: [opensensor/openimp](https://github.com/opensensor/openimp) (branch `aperto`). Both repositories are released together; use matching tags.

## Documentation

- [Wiki](https://github.com/opensensor/openimp/wiki) (one wiki for both repositories): module parameters, memory (rmem, ispmem, MMAP pool), troubleshooting, install and boot guard, release scheme.
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

Open an issue at [opensensor/open-tx-isp](https://github.com/opensensor/open-tx-isp/issues) (kernel driver, ISP, memory) or [opensensor/openimp](https://github.com/opensensor/openimp/issues) when unsure. Please include the SoC and sensor, the revisions of open-tx-isp, OpenIMP and the streamer, `dmesg` (including any shortfall line such as `set ispmem >= N KB`), the streamer log, the stream set and the `rmem`/`ispmem` values. Do not post addresses, credentials or location names. Details: [Troubleshooting](https://github.com/opensensor/openimp/wiki/Troubleshooting#reporting-a-problem).

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
