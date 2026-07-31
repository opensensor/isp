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
- `tx_isp_t31_subdev_resolver.c` supplies T31's typed graph and established
  raw pad-direction mapping to the common name/type/index resolver.
- The shared frame-buffer ABI asserts T31's 68-byte MIPS32 `v4l2_buffer`
  contract and names its persistent flag mask; T31 retains queue ownership and
  its generation-specific state meanings.
- `frame_image_format` embeds the shared fixed 48-byte pixel descriptor instead
  of Ingenic's GCC-5.4-only `v4l2_pix_format` extension. This restores the
  private format from 96 to the OEM/libimp 112 bytes on modern compilers.
- The outer frame dispatcher and ISP/VIC handoffs use the shared proven event
  IDs and legacy-`V` ioctl envelopes; generation-specific events above
  buffer-done remain local.
- Compile-time checks pin the typed five-word active-link record to the common
  20-byte MIPS32 layout; the known extra callback in T31's pad tail remains
  explicit and is not folded into the shared recovered prefix.
- Target assertions also pin the declared 8-byte endpoint, 20-byte graph-link
  configuration, and 8-byte configuration-set envelope to the common wire ABI.
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
- the OEM one-buffer pre-dequeue path now schedules the configured 24 ms
  frame-start lead event, so both RTSP streams deliver the configured 25 fps
  instead of waiting through a userspace round trip and losing every other
  frame
- SC2336 gain-stage hysteresis keeps the accepted MDNS ratio `0x80` at normal
  gain and selects the device-validated `0xa0` profile in the `0x8xx`
  high-gain stage; `sc2336_mdns_auto=0` disables this local policy
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
- the compiler-independent 112-byte frame format restores the substream from
  76 frames in six seconds to 147 in six seconds while the main stream remains
  full-rate; a second boot decoded 124/122 main/sub frames in five seconds
- the validated frame-channel open module used SHA-256
  `e07d82117a4e37f8d23a97fac6d09acddcce7d551e85ada478e89a6363e25ac1`;
  forced day mode, ISP interrupts, `dmesg`, `logread`, and `logcat` all passed
- the deterministic sequential build uses the shared frame-channel events and
  ioctl envelopes; Raptor reports both streams at 25 fps and saved a fresh
  302,532-byte 1920x1080 JPEG while external monitoring occupied the RTSP
  server's four client slots
- the frame-channel dispatcher now uses the compiler-independent common
  20-byte request-buffer structure and common legacy stream command names;
  its memory-pressure limits and allocation state remain local
- the graph endpoint search now uses the common checked subdevice resolver
  while retaining T31's known raw input/output slot mapping; its one-shot boot
  bound the SC2336 once, applied day mode, advanced ISP/VIC interrupts, and
  decoded 149 1920x1080 frames in six seconds with clean `dmesg`, `logread`,
  and `logcat`; the active module SHA-256 is
  `bb95aba4587061b4d364fd3b3893bbf8d8ec0471252deed97dc996e569d48673`

The shared NV12 DMA plan now validates QBUF allocation length, complete
address range, and Y/UV placement before the local tracking and MSCA handoff.
The validation retained exact 3,133,440/353,280-byte pools without rejecting a
live buffer. The repaired pre-dequeue path decoded 501 main frames in 20
seconds and 250 substream frames in 10 seconds. The validated module SHA-256
for that earlier DMA-binding cycle was
`8a1e71f3f0479bbc450d5a98f7af910b572b1a5b0b331b03518a4ba466d5d731`.

The common math library now exposes and host-tests the split 64-bit two- and
three-operand Q-format multipliers used by AE tuning. T31 intentionally retains
its local inline compatibility body: routing that body through one additional
header wrapper changed register allocation inside the large recovered AE
function and produced visibly grainier walls despite mathematical equivalence.
Restoring the size-neutral boundary reproduces the accepted module and its
entire loadable text byte-for-byte. The pre-dequeue/MDNS work leaves that
fragile tuning object untouched and changes only the frame-source/core and
outer sensor-work objects.

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

- The pre-dequeue implementation is intentionally limited to the one-buffer
  channel contract exercised by this consumer. Multi-buffer ownership still
  needs a separate device-backed queue test before it can share this path.
- The raw histogram DMA layout still needs recovery; the per-zone luma
  fallback is intentionally narrow.
- Raptor's exposure summary still reports zero WB statistic gains even though
  the main WB ioctl returns live nonzero red/blue gains.
- The same-light July 30 wall capture measured mean flat-region frame change
  `0.01512` with the high-gain profile versus stock `0.01478`; continue
  checking motion/detail tradeoffs as more sensors acquire automatic profiles.
- More tuning internals should move into logical files, but extractions must
  retain OEM callback order and be tested on-device.
- T31's native 64-bit multiply shim can move to the common header only after
  the resulting recovered AE machine code is proven byte-identical or the
  surrounding function has been made free of code-generation-sensitive
  recovered constructs.

## Build and Check

```bash
SOC=t31 ./build_local.sh
driver/t31/verify_no_divdi3.sh driver/t31/tx-isp-t31.ko
make -C tests check
git diff --check
```

Load experiments through the one-shot boot hook.  Do not live-unload the ISP
stack while the sensor and Ingenic userspace hold its objects.
