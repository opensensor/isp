# T31 TX-ISP Driver

## Organization

The T31 implementation is the most decomposed active driver and is the
reference layout for future T23/T41 extractions.  The split is behavioral, not
just cosmetic:

- `tx_isp_module.c` owns module lifecycle, platform resources, frame-channel
  ioctls, and buffer bookkeeping.
- `tx_isp_core.c` owns ISP-core probe, MMIO, core IRQ handling, and MSCA frame
  completion.
- `tx_isp_csi.c`, `tx_isp_vic.c`, `tx_isp_vin.c`, and `tx_isp_fs.c` own their
  corresponding subdevices.
- `tx_isp_tuning.c` owns the recovered Tiziano tuning implementation and the
  proprietary `libimp.so` control ABI.
- `tx_isp_daynight.c`, `tx_isp_callback_plan.c`, and
  `tx_isp_reg_profile.c` adapt T31 behavior to shared library units.
- `tx_isp_tuning_abi.c` links checked shared libimp envelopes, command
  descriptors, and response packers.
- `tx_isp_frame_layout.c` links checked NV12 and MDNS auxiliary geometry while
  T31 retains its alignment, register, allocation, and memory-option policy.
- `tx_isp_t31_exposure.c` adapts the shared exposure library to the T31
  deflicker LUT's fixed 120-word, repeated-tail ABI.
- `tx_isp_sinfo.c` supplies the T31 adapter for the shared sensor registry.
- `tx_isp_fixpt.c`, `tx_isp_ae_zone.c`, and `tx_isp_frame_done.c` isolate
  arithmetic, AE-zone, and frame-sync behavior from the large tuning unit.

Register addresses, recovered object layouts, IRQ acknowledgement, tuning
tables, and sensor-specific profiles remain T31-local.

## Current SC2336 Runtime

The July 30, 2026 device cycle validated the open module on the real T31
SC2336 camera through the stock Ingenic userspace and Raptor:

- main/sub RTSP streams initialize and remain responsive
- forced night and day transitions produce top masks `0xB574224D` and
  `0xB5742209`
- lens shading is enabled by default (`force_bypass_lsc=0`)
- day output has coherent color, geometry, lens shading, and tonal continuity
- DMSC register `0x4800` is sourced only from the tuning blob's output/debug
  selector; Bayer routing is not synthesized into this register
- the SC2336 linear-mode MDNS top word matches the same-camera stock oracle
  (`0x7814=0x00f01100`)
- duplicate sensor exposure tuples are suppressed after the first successful
  write, avoiding redundant 25 Hz SC2336 timing-register transactions
- a synchronized stock/open capture at approximately 1200 integration lines
  and sensor gain `0xc4` matched the functional DMSC, sharpen, MDNS/YDNS,
  RDNS, SDNS, and BCSH register programming
- 100 consecutive `get-isp` plus `get-exposure` query pairs complete without
  an ioctl failure, kernel fault, or producer restart
- the main pool now reports the OEM-private `bytesperline=2880` and
  `sizeimage=3133440` consistently, eliminating the former 4,700,160 versus
  3,133,440 producer/consumer mismatch
- the active `isp_memopt=1` MDNS allocation remains exactly `0x2f8740`
- the tested reserved-memory command line is
  `rmem=22M@0x2a00000`; no bootloader environment change is required

The shared NV12 DMA plan now validates QBUF allocation length, complete
address range, and Y/UV placement before the local tracking and MSCA handoff.
The validation retained exact 3,133,440/353,280-byte pools and the inherited
half-rate cadence without rejecting a live buffer. The validated module
SHA-256 is
`2b7a97c16d709fb2c9e0671f347093e7ea8be1054065741ee5b848f0306ce556`.

The DMSC output selector deserves particular care.  Its low bits select
normal or diagnostic DMSC outputs; they are not a CFA index.  Rewriting them
from the sensor media-bus Bayer order produced a monochrome edge/noise image.
The preserved compatibility entry point is therefore a no-op, while
`tisp_dmsc_out_opt_cfg()` writes the tuning-selected word exactly like the
shipping implementation.

## Tuning ABI Corrections

The T31 tuning ioctl carries either a scalar or a userspace pointer in the
same eight-byte control payload.  Keep command routing explicit.  A former
`cmd >= 0x8000023` shortcut interpreted pointer-valued WB, highlight, and
backlight queries as scalars.

The wire envelopes, fixed response layouts, sparse-EV extraction, and command
payload descriptors now live in `tx_isp_tuning_abi.h`; T31 retains only the
command table, hardware collectors, and dispatch policy.

`tisp_g_ev_attr` is an OEM-shaped sparse 0x80-byte structure.  In particular:

- current exposure lines are word 0
- exposure microseconds are word 2
- minimum/maximum integration lines are halfwords at byte offsets
  `0x6c`/`0x6e`
- one-line duration in microseconds is the halfword at byte offset `0x7c`

Do not turn these byte offsets into `uint32_t` indices.  The old
`ev_buffer[0x37]` write landed at byte `0xdc`, beyond both the OEM object and
the caller's former eight-word stack buffer.

`apical_isp_expr_g_ctrl` must return the exact 12-byte
`IMPISPExpr.g_attr` layout:

```text
u32 mode
u16 integration_time
u16 integration_time_min
u16 integration_time_max
u16 one_line_expr_in_us
```

The recovered histogram path currently reports all samples in bin zero on
this device.  `tisp_ae_g_luma` therefore uses the already-valid per-zone AE
weighted mean only when the histogram-derived value is zero.  The live ioctl
then reports scene-responsive exposure and luma instead of zeros.

## Known Gaps

- Raptor configures 25 fps, but this one-buffer frame-source pipeline delivers
  about 12.5 fps.  The stock T31 evidence advances about 408 frames per 30
  seconds (roughly 13.6 fps) with the same one-buffer behavior, so this is not
  currently classified as an open-driver regression.  Changing kernel
  completion cadence without a multi-buffer consumer test would risk active
  buffer overwrite.
- The raw histogram DMA layout still needs recovery; the per-zone luma
  fallback is intentionally narrow.
- Raptor's exposure summary still reports zero WB statistic gains even though
  the main WB ioctl returns live nonzero red/blue gains.
- In the matched-exposure July 30 wall capture, accepted open output retained
  roughly 1.5-1.7 times the stock frame-to-frame luma change in flat regions.
  The image was visually accepted, but this remains a useful MDNS/LSC tuning
  parity target.
- More tuning internals should move into logical files, but extractions must
  retain OEM callback order and be tested on-device.

## Build and Check

```bash
SOC=t31 ./build_local.sh
driver/t31/verify_no_divdi3.sh driver/t31/tx-isp-t31.ko
make -C tests check
git diff --check
```

Load experiments through the one-shot boot hook.  Do not live-unload the ISP
stack while the sensor and Ingenic userspace hold its objects.
