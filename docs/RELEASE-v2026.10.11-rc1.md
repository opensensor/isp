# open-tx-isp v2026.10.11-rc1: first release candidate

**open-tx-isp** is an open replacement for Ingenic's proprietary ISP kernel driver (`tx-isp-<soc>.ko`: sensor input, VIC/CSI, ISP core and tuning, MSCA scaler/frame channels, statistics). Together with **OpenIMP** (the open `libimp`) it forms a fully open camera stack for Ingenic T-series SoCs. Release together with [OpenIMP v2026.10.11-rc1](https://github.com/Lu-Fi/openimp/releases/tag/v2026.10.11-rc1); the shared parts (soak, release gate, streamers, beyond-vendor list, upgrade notes) are described there in full. Always use matching tags.

| Read more (pinned to this tag) | |
|---|---|
| **Feature matrix** (per function and SoC) | [FEATURE_MATRIX.md](https://github.com/Lu-Fi/openimp/blob/v2026.10.11-rc1/docs/FEATURE_MATRIX.md) · [rendered HTML](https://htmlpreview.github.io/?https://github.com/Lu-Fi/openimp/blob/v2026.10.11-rc1/docs/feature-matrix.html) |
| Beyond the vendor stack (module parameters, streamer notes) | [OPENIMP_BEYOND_VENDOR.md](https://github.com/Lu-Fi/openimp/blob/v2026.10.11-rc1/docs/OPENIMP_BEYOND_VENDOR.md) |
| Full campaign log with test evidence | [OPEN_STACK_CHANGELOG.md](https://github.com/Lu-Fi/openimp/blob/v2026.10.11-rc1/docs/OPEN_STACK_CHANGELOG.md) |
| Driver changelog · README (T23 module parameters) | [CHANGELOG.md](https://github.com/Lu-Fi/open-tx-isp/blob/v2026.10.11-rc1/CHANGELOG.md) · [README.md](https://github.com/Lu-Fi/open-tx-isp/blob/v2026.10.11-rc1/README.md) |
| Per-SoC driver notes | `driver/<soc>/README.md` (e.g. [T23](https://github.com/Lu-Fi/open-tx-isp/blob/v2026.10.11-rc1/driver/t23/README.md), [T41](https://github.com/Lu-Fi/open-tx-isp/blob/v2026.10.11-rc1/driver/t41/README.md)) |

## 1. Summary

| Component | Revision |
|---|---|
| open-tx-isp | `v2026.10.11-rc1` = `191be6c5` (soaked agg-35 code `6e464ac7` + documentation only) |
| OpenIMP | `v2026.10.11-rc1` = `fe9307c` (agg-35 `687a68e4` + documentation) |
| Recommended streamer · firmware | timps **v1.10.0** (tag `4432fa8`; the soaked images used `f1565f7`, source identical) · thingino `aperto` (pin of package `open-tx-isp` to `v2026.10.11-rc1` pending as a pull request) |

Scope: all driver changes since `e92166b9` (last commit before 2026-09-29 19:00), 568 commits; about 194 of them are new since the previous `aperto` state (2026-10-06). **RC1** is exactly the soaked code; if no blocking problem is reported, it becomes the first dated release on `aperto`.

SoCs: **release** T10, T20, T21, T23, T31; **experimental** T30 (builds against a real T30 kernel; no T30 device in our lab, an external contributor reported a live gate on a T30X, see OpenIMP `docs/T30_STATUS.md`; test kit in OpenIMP `tools/testkit/t30`), T40 (vendor kernel driver used in the first OpenIMP device run; the open T40 driver was not part of this campaign), T41 (open driver runs, sub-stream and reload issues open).

**Completeness** of the vendor API per SoC (done = device-tested + host-tested + audited; details and gap lists in the [OpenIMP notes](https://github.com/Lu-Fi/openimp/releases/tag/v2026.10.11-rc1) and the [feature matrix](https://github.com/Lu-Fi/openimp/blob/v2026.10.11-rc1/docs/FEATURE_MATRIX.md#progress-per-soc)): T10 89.2 %, T20 90.5 %, T21 93.2 %, T23 93.8 %, T31 96.4 % (release SoCs: 0 gaps in functions a known streamer imports); T41 59.3 %, T40 25.4 %, T30 not audited on a device (experimental). Streamer-used gaps: T30 2, T40 7 (raptor), T41 22 (prudynt 6, raptor 19); agg-36 closes most of them.

## 2. Release evidence

- **Overnight soak of the RC1 code** (2026-10-10 18:04 to 2026-10-11 08:00, about 14 h, 10 cameras, full OTA images): **0 kernel oopses, 0 unexplained reboots, 0 streamer restarts, VA-API decode 54/54.** Previous soak (agg-32, 43.4 h): 0 unplanned reboots, 0 oopses, 0 guard trips on the cameras without interaction.
- **Release gate** (apitest, imgfx): every FAIL fixed or explained; table in the OpenIMP notes. Kernel log checked after each run: 0 oopses.
- **Reload and stop:** 10 stop/start (including `kill -9`) and 10 `rmmod`/`insmod` cycles without an oops on T20, T21, T23, T31 and T41 (T10: 5 cycles while streaming).
- **Host tests:** `make -C tests check` (about 130 test sources: front-crop decision logic, MSCA geometry and scratch, frame drop, AE gain cap, T41 MSCA shadow/start, WDR ring, tuning wire formats); the recovered T20/T10 firmware runs through a host harness at `-O0`/`-Os`/`-O2` with identical register traces. Several lifted T23 blocks (AF, AWB cluster/trend, mask block, ADR/defog: 44/44 cases) are proven identical to the stock module in a MIPS emulator.

Below, **[host]** marks items without a device test; everything else was device-tested on the SoC named.

## 3. Driver changes by SoC

**All SoCs**
- Lifecycle hardening: locking around frame-channel ioctls, STREAMOFF and last-close races, use-after-free on unload, statistics DMA stopped before its buffers are freed, all user copies checked, bounded waits.
- Sensor module pinned while the ISP is open (`rmmod` of a streaming sensor is refused); sensor registry under `/proc/jz/sensor` with orphan-slot and unload-deadlock fixes.
- `isp-m0` in the vendor format with live exposure/gain read-back on every T-series SoC.
- Unknown tuning control IDs are rejected instead of silently succeeding.
- Smaller modules than the vendor: T21 452 KB (vendor 616), T23 622 KB (857), T31 711 KB (829).
- QBUF buffers must lie in the rmem window (`qbuf_guard=0` disables); memory shortfalls are logged once with the missing amount and a concrete parameter value.

**T31 (reference)**
- Front crop is effective and guarded: a window smaller than a running channel's scaler output (the MSCA cannot upscale) is refused with -EINVAL instead of stalling every output until reboot; a main stream larger than the window drops the crop with a warning; scaler steps and latch are written, so a valid window zooms both streams; the window survives idle off/on and a flip, and is released when the ISP session closes.
- `SetFrameDrop`/`GetFrameDrop` with the stock semantics (the ISP drops frames: 29.6 fps to 14.8/7.4); the old swapped registers and the second software drop are gone.
- `O_NONBLOCK` DQBUF and poll on the frame channels; MSCA address FIFO cleared on STREAMOFF/REQBUFS; per-frame `WaitFrame`.
- Tuning gaps closed: AE ROI/hist/weights/Expr/IT max/min, `SetAeAttr`, `GetSensorAttr`, CCM, gamma, sinter/sharpness/defog idempotent, RGB coefficients, BLC/CSC/scaler level, anti-flicker table after `SetSensorFPS`, AF statistics; privacy mask like the vendor (400/400 random sequences register-identical); scene mode and colour effects.
- `rmmod` oops and a 252 KB per-cycle vmalloc leak fixed; WDR memory only in WDR mode; WDR statistics ring kept until its DMA engine is off (100 stop/start cycles and 20 reloads clean).
- Kernel stack no longer leaked through AE/AF zone getters; tuning netlink and register map bounded.

**T23**
- Release defaults are the stock-like start/stop set (`chan_stop_keep_input=1`, `msca_keep_enabled=2`, `msca_restart_skip=1`, `msca_session_release=1`, ...); STREAMOFF waits until the channel's buffers left the hardware and QBUF invalidates cache lines, so cold-start snapshots no longer fail (20 cold starts, 20 streamer restarts, all OK).
- **MSCA scratch buffer** (beyond stock): a stopped channel whose output stays enabled writes into a scratch area instead of a buffer user space has freed (`msca_scratch=1`).
- **Front crop:** geometry check refuses windows the MSCA cannot do (-EINVAL, `msca_geom_rejects`); offset windows work (origin relative to the global input window, `msca_rel_origin`); crop off unlocks the window and restores the full frame; a locked window that no longer fits is dropped instead of hanging; the lock is released at the last close. 18-case matrix green; upscaling up to 2.0 measured (`fcrop_upscale_pct`, default 0).
- AF statistics on by default (`source_af=1`, no measurable CPU cost); lifted vendor AE default; dynamic ADR and defog from the vendor module; about 45 control IDs wired (WB, CCM, DRC, defog, DPC, CSC, flip, anti-flicker, max digital gain stage, `SetSensorFPS`).
- Fixes: day-to-night oops, purple night picture, white-balance jump to about 10000 K, Bayer re-sync after flip, exposure kept across restarts, `isp_clk`/`isp_clka` now reach the hardware, two out-of-bounds writes, `.bss` 434 to 180 KB.
- AWB cluster/trend, mask block and autozoom handlers from the stock module [host/emulator only; mask has no image effect yet].

**T21** (first open bring-up)
- Stock AE (both stages), AWB with CT detection, CT-driven CCM/LSC, ADR/defog and control dispatchers lifted instruction by instruction: night flicker, colour blotches, night mono and overexposure fixed.
- Beyond vendor: brightness scales the AE target (128 = vendor path); sepia; `ae_it_max_us` acts; AWB hysteresis at dusk and night freeze (module parameters, 0 = vendor).
- Reload oops (statistics DMA into freed buffers) fixed; open/release refcounting; `.bss` 259 to 106 KB.

**T20 / T10** (T10 builds the T20 sources)
- **`isp-m0` "MAX SENSOR analog gain"** reports the cap the AE really reaches (minimum of sensor maximum and `SetMaxAgain` ceiling; T10 the stepped gain plateau, 142 instead of 144): fixes the T20 Day-in-the-dark regression together with timps v1.10.0 (confirmed on a T20 camera: Night at dusk, Day in the morning, ISP confirmed; both T20 cameras correct through the RC1 soak).
- Front crop: a window is kept while no downscaled channel is open and applied at stream on; released with the tuning session.
- Max analog gain and user max integration time honoured by the AE; AE re-armed on every streamer start (5/5 restarts converge); vertical-flip chroma overrun (pink band) and sharpness fixed; simple AWB with manual WB and presets (vendor chain selectable); Sinter/Temper strength acts; CCM/LSC readable.
- Unused 8 MiB V4L2 MMAP pool off by default (`isp_mmap_pool_kb=0`): about 8 MB more free RAM (T10 1.9 to about 9.7 MB).
- `rmmod` while streaming refused, 53 user copies checked, stop/restart oops fixed (20 clean cycles), IRQ registered after its data; T10 NVPU reference overrun, sub-stream scaler and day/night panic fixed; T10 build refuses a non-T10 kernel tree.
- Firmware unit built at `-Os` (T20 module 589 to 409 KB) [host harness; device run open].

**T41** (experimental)
- Module reload root cause fixed (10/10 cycles); AE compensation, gain/exposure caps, 2D/3D noise reduction, picture controls, flip, BCSH reach the ISP.
- Tuning controls: gamma (fixed curves, restore), CCM, CSC presets/user tables, manual exposure, module control, DPC/DRC ratios.
- Driver controls streamers call: frame drop (stock 40-byte request, all entries validated first), sensor register read/write, WDR enable/disable, AF weight.
- Channel-restart hang fix (flip update only on a real change); statistics DMA engines stopped before their buffers are freed. MSCA geometry latch stays opt-in (`t41_msca_cfg_update=0` default).

**T30** · builds against a real T30 kernel, missing `isp_printf` export fixed [build only].

## 4. Beyond the vendor driver: module parameters

| Parameter | SoC | Default | Meaning |
|---|---|---|---|
| `fcrop_upscale_pct` | T31, T23 | 0 | tolerated MSCA upscale for front crop; its presence marks a guarded driver |
| `msca_scratch` | T23 | 1 | scratch area for stopped channels (0 = first thing to try on a stop/start hang) |
| `chan_stop_drain`, `qbuf_cache_inv` | T23 | 21, 1 | STREAMOFF drain wait (x 10 ms), QBUF cache invalidate |
| `source_af` | T23 | 1 | AF statistics chain (read-only) |
| `msca_rel_origin` | T23 | Y | crop origin relative to the global input window |
| `t23_runtime_trace` | T23 | 0 | informational logging |
| `isp_mmap_pool_kb` | T20, T10 | 0 | optional V4L2 MMAP frame pool |
| `qbuf_guard` | all | on | QBUF must lie in rmem |
| AWB hysteresis / night freeze | T21 | 10 % | 0 = vendor behaviour |
| `t41_msca_cfg_update`, `t41_msca_stop_disable` | T41 | 0, 0 | opt-in geometry latch (correct up to 768x432) |

Full T23 table: README "T23 module parameters". Behaviour beyond the vendor without a switch: front-crop guards, gain-cap report, T21 brightness/sepia/`ae_it_max_us`, T10/T20 Sinter/Temper strength, checked inputs, refused `rmmod` while streaming.

## 5. Known issues and what is not in RC1

- **T41:** channel 1 colour at night differs from channel 0 (soak finding); channel 1 of 960x540 or more started mid-stream can hang the SoC with the opt-in latch (keep it at 768x432 or smaller); `rmmod tx_isp_t41` sometimes oopses after reload cycles (heap overwrite through a stale pointer, not seen in 30 stop/start cycles without reload); after a reload `AddSensor` can fail once with a busy I2C client; `WdrOutputMode` has no stock handler; `AutoZoom`, `SetMaskBlock`, `SetScalerLv` deferred.
- **T31:** a black picture of about 80 s seen once during a streamer-driven day/night probe; not understood yet. The T31 MSCA upscale limit is not established (leave `fcrop_upscale_pct` at 0).
- **Day/night** decisions (T23 dusk flapping with raptor, poorly lit places) are streamer logic; the driver reports sensor values only.
- **T20/T10:** release of the crop window at session close not verified on a device; T10 occasional Day switch with a black frame in the dark (earlier soak).
- **T23:** no real WDR; the rare single Helix encode error is provisionally closed (kernel patch 0102).
- **Not in RC1 (agg-36 and later):** T40 RC/H.265 and ISP-drawn OSD, `SwitchBin`, bufshare, default log level, IVS result ordering, further review fixes and vendor-API gap work, T41 mask/zoom.

## 6. Upgrade and compatibility

- thingino `aperto` with the open ISP stack (`BR2_PACKAGE_THINGINO_ISP_OPEN`); the module installs as `tx-isp-<soc>.ko`, SDK sensor/audio/AVPU modules stay. Flash a full image: the thingino kernel patches for `soc_vpu`/rmem (sleeping VPU wait, error interrupt, validated flush range, NVPU statistics registers for the T20 rate controller) must match. The optional boot guard (`BR2_PACKAGE_THINGINO_ISP_GUARD`, off by default) skips the ISP modules after an unstable load.
- Use the matching OpenIMP tag.
- **Changed since the previous `aperto` state:** T31 front crop refused (-EINVAL) when a running stream is larger than the window; crop windows on T31/T23/T20/T10 are released at session close (streamers re-apply); T20/T10 `isp-m0` gain cap reports the reachable value (128 instead of 158 on T20); T23 defaults now include `source_af=1` and `msca_scratch=1`; T41 accepts frame drop, sensor register, WDR enable and AF weight.

## 7. Testing, reporting, credits

- **Test:** day and night with your streamer; watch `dmesg` for oopses, `-EINVAL` crop refusals and shortfall lines; on T23 the counters under `/sys/module/tx_isp_t23/parameters/`. `make -C tests check` runs the host tests. T30 owners: see `tools/testkit/t30` in OpenIMP (no flashing, results archive).
- **Report** at [open-tx-isp issues](https://github.com/Lu-Fi/open-tx-isp/issues) (driver, ISP, memory) or [openimp issues](https://github.com/Lu-Fi/openimp/issues): SoC and sensor, both tags, streamer and version, `dmesg`, stream set, `rmem`/`ispmem`. No addresses, credentials, location names or private pictures.
- **Contribute:** changes grounded in vendor binary analysis, hardware logs or captures, emulator or host tests; state evidence, validation and open points.
- **Credits:** the original [opensensor/open-tx-isp](https://github.com/opensensor/open-tx-isp) and [OpenIMP](https://github.com/opensensor/openimp); [thingino-firmware](https://github.com/themactep/thingino-firmware), [ingenic-sdk](https://github.com/themactep/ingenic-sdk) and the Ingenic reverse-engineering community; the timps, prudynt and raptor maintainers. Licence: GPLv3.
