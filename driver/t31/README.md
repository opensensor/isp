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
- Pad allocation delegates the common five-word active-link initializer while
  retaining T31's raw pad-slot mapping. The complete pad type is asserted as
  the OEM 0x24-byte wire object.
- The shared frame-buffer ABI asserts T31's 68-byte MIPS32 `v4l2_buffer`
  contract, names its persistent flag mask, and defines the recovered
  FREE/QUEUED/ACTIVE/DONE slot states; T31 retains queue ownership.
- `frame_image_format` embeds the shared fixed 48-byte pixel descriptor instead
  of Ingenic's GCC-5.4-only `v4l2_pix_format` extension. This restores the
  private format from 96 to the OEM/libimp 112 bytes on modern compilers.
- The outer frame dispatcher and ISP/VIC handoffs use the shared proven event
  IDs and legacy-`V` ioctl envelopes; generation-specific events above
  buffer-done remain local.
- Compile-time checks pin the typed five-word active-link record to the common
  20-byte MIPS32 layout and the pad private pointer/stride to 0x20/0x24. The
  former extra callback was removed because it made the C type wider than the
  allocator's OEM stride.
- Target assertions also pin the declared 8-byte endpoint, 20-byte graph-link
  configuration, and 8-byte configuration-set envelope to the common wire ABI.
- `tx_isp_t31_exposure.c` adapts the shared exposure library to the T31
  deflicker LUT's fixed 120-word, repeated-tail ABI.
- `tx_isp_sinfo.c` supplies the T31 adapter for the shared sensor registry.
- `tx_isp_fixpt.c`, `tx_isp_ae_zone.c`, and `tx_isp_frame_done.c` isolate
  arithmetic, AE-zone, and frame-sync behavior from the large tuning unit.

Register addresses, recovered object layouts, IRQ acknowledgement, tuning
tables, and sensor-specific profiles remain T31-local.

## Current SC301IOT Runtime

The August 13-25, 2026 Wyze Video Doorbell v2 work added a third live T31
sensor family and exercised three separate SC301IOT units:

- the open driver and OpenIMP deliver stable processed main/sub streams on the
  2048x1536 sensor mode, including fleet runs on three physical doorbells
- CSI lanes, Bayer code/phase, dimensions, integration/gain limits, apply
  delays, and exposure capabilities are derived from the bound sensor rather
  than GC2053 or SC2336 literals
- the AE0 static and histogram DMA paths feed the recovered per-frame Tiziano
  software controller; a same-scene stock/open daylight comparison reached
  near-stock color and exposure on the validated doorbell profile
- a live rear-door test exposed a delayed highlight-retention failure: the
  first frame held a blue sky and later frames washed it out because a generic
  zero scene-control request was encoded as an active OEM level
- zero/no-override now preserves the active tuning bank's calibrated
  highlight/backlight behavior, while explicit non-zero levels retain the OEM
  encoding; the fix is generic policy, not an SC301IOT special case

This proves the generic sensor-attribute path and the SC301IOT daylight AE
loop. It does not claim night/IR, WDR, or every 50/60 Hz lighting condition is
already at OEM parity.

## Current GC2053 Runtime

The August 5, 2026 Wyze Cam v3 cycle added a second T31 sensor oracle:

- the corrected 0x24-byte pad ABI consumed its one-shot marker, registered one
  GC2053 at address `0x37`, kept Raptor and ISP/W02/AVPU interrupts advancing,
  and decoded 358 1920x1080 frames in 12 seconds without a driver fault
- the recovered CLM bank assignment is H/H at `0x60000/0x68000` and S/S at
  `0x70000/0x78000`; this matches both the T31 OEM HLIL and the T23 recovered
  implementation
- loading H, S, and shift from tuning-block offsets `0xfb84`, `0xff9e`, and
  `0x107d4` reproduces all four stock GC2053 CLM banks byte-for-byte
- with exact CLM data, AE recovers to the stock operating range and both
  1920x1080 and 640x360 streams remain decoder-clean at 30 fps
- image parity is not complete: with GIB active, AWB DMA has zero RGB sums and
  the frame is severely magenta; bypassing only GIB restores RGB sums in at
  least 219 of 225 zones and recognizable color. The default remains
  OEM-aligned with GIB active so this remaining defect is visible rather than
  silently hidden
- every candidate was loaded through the one-shot fail-safe path, and the
  camera was power-cycled back to the persistent 831,776-byte stock module
  after each experiment

## Current SC2336 Runtime

The July 30-31, 2026 device cycles validated the open module on the real T31
SC2336 camera through the stock Ingenic userspace and Raptor:

- main/sub RTSP streams initialize and remain responsive
- forced night and day transitions produce top masks `0xB574224D` and
  `0xB5742209`
- lens shading is enabled by default (`force_bypass_lsc=0`)
- day output has coherent color, geometry, lens shading, and tonal continuity
- DMSC register `0x4800` is sourced only from the tuning blob's output/debug
  selector; Bayer routing is not synthesized into this register
- reduced-memory MDNS follows the recovered OEM policy for every sensor by
  disabling ASS, BGM, and PSN when their reference planes are absent; the
  SC2336 top word remains the same-camera stock oracle
  (`0x7814=0x00f01100`)
- duplicate sensor exposure tuples are suppressed after the first successful
  write, avoiding redundant 25 Hz SC2336 timing-register transactions
- the main frame source uses two VBM buffers and rotates QUEUED/ACTIVE/DONE
  ownership before waking DQBUF; userspace receives only DMA-complete frames
  while both RTSP streams retain the configured 25 fps
- frame timestamps are captured at DMA completion instead of at userspace
  dequeue, eliminating scheduler-dependent PTS/DTS jitter and multi-second
  delivery gaps
- MDNS strength remains under the standard tuning control instead of applying
  a sensor-named gain-stage override
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
- the common pad-link state candidate consumed its one-shot marker, bound the
  SC2336 once, accepted forced day mode, advanced ISP/VIC interrupts, and
  decoded 148 1920x1080 frames in six seconds; all three fault scans are clean
  and the responsive candidate remains active with SHA-256
  `322ace762900c81d22e55f527572708f6ae92d6ca144725d52fb58413dd37b4a`
- the final two-buffer module consumed its fail-safe marker, loaded with status
  zero, accepted forced day mode, and returned 300 monotonic main-stream
  packets in 12 seconds with 39.155-41.644 ms DTS spacing; the persistent open
  module SHA-256 is
  `14908c65dd40bb76f538a4a92a0a27803ea89fd8a9a0cdb8c0975516eaeb85a7`

The shared NV12 DMA plan now validates QBUF allocation length, complete
address range, and Y/UV placement before the local tracking and MSCA handoff.
The validation retained exact 3,133,440/353,280-byte pools without rejecting a
live buffer. An earlier one-buffer pre-dequeue experiment decoded 501 main
frames in 20 seconds and 250 substream frames in 10 seconds, but could expose
an ACTIVE buffer before DMA completion; it has been replaced by the safe
two-buffer rotation. The validated module SHA-256 for that earlier cycle was
`8a1e71f3f0479bbc450d5a98f7af910b572b1a5b0b331b03518a4ba466d5d731`.

The common math library now exposes and host-tests the split 64-bit two- and
three-operand Q-format multipliers used by AE tuning. T31 intentionally retains
its local inline compatibility body: routing that body through one additional
header wrapper changed register allocation inside the large recovered AE
function and produced visibly grainier walls despite mathematical equivalence.
Restoring the size-neutral boundary reproduces the accepted tuning object
byte-for-byte. The frame-queue/MDNS work leaves that fragile object untouched
and changes only frame-source/core and outer sensor-work objects.

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

The histogram interrupt invalidates the selected DMA bank before unpacking
it, matching the OEM driver.  A live T31L/SC2336 probe reported all 518,400
pixels across 241 nonzero bins and a frame-responsive weighted mean.
`tisp_ae_g_luma` retains the per-zone AE weighted mean as a defensive fallback
only when the histogram-derived value is zero.

## Known Gaps

- T23/T41 still need their own device-backed ownership tests before the full
  T31 queue lifecycle can move behind a common implementation rather than only
  sharing slot-state names.
- Raptor's exposure summary still reports zero WB statistic gains even though
  the main WB ioctl returns live nonzero red/blue gains.
- Motion-region denoise parity still needs a synchronized stock/open capture;
  static same-light register and flat-region measurements are not sufficient
  to validate the MDNS motion classifier.
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
