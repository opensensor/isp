# Ingenic T23 ISP Recovery

This directory is the T23 bring-up workspace for matching the OEM
`tx-isp-t23.ko` behavior on Linux 3.10.14 targets.

The recovered whole-driver seed from `tx-isp-t23-v1` now lives in
`tx_isp_t23_core.c`, with separate math, sensor-registry, mode-profile,
callback-plan, register-profile, frame-layout, scaler, and tuning-ABI adapter
objects. Kbuild links all nine into the existing
`tx_isp_t23_recovered.ko` module identity expected by this target. The math
adapter delegates to the cross-SoC primitives, the registry adapter delegates
slot/procfs ownership to the common typed implementation, and the profile
adapter supplies the shared top-bypass flag merge. The T23 mode adapter owns
the SoC masks and exact 17-block refresh order used by day/night, custom-mode,
and tuning-bin transitions. The scaler adapter owns the recovered T23 sinc
table and four-tap correction order while delegating checked fixed-point
interpolation and normalization to the common T23/T41 generator.

The tuning-ABI adapter supplies checked shared libimp envelopes, command
descriptors, and expression/EV response packers. The frame-layout adapter
supplies overflow-checked NV12 geometry and the shared T23/T31 MDNS auxiliary
layout. Its private frame-channel arrays use the shared 68-byte frame-buffer
wire ABI and named word/flag constants. Its recovered 112-byte frame-image
format is compile-time checked against the shared format ABI. T23 still owns
dispatch, queue state,
alignment policy, register writes,
sensor-derived values, and its 12-byte allocation ABI's page padding.

Current smoke-test status:

- `sensor_sc2336_t23.ko` loads against the recovered module.
- `/dev/tx-isp`, `/dev/isp-m0`, `/dev/misc-ivdc`, and `/dev/framechan0..3`
  are present.
- `/proc/jz/sensor/sensor0` reports the SC2336 metadata expected by Raptor.
- Raptor starts and publishes stable H.264 streams on both MSCA channels.
- The nine-object module passes a full one-shot device boot with advancing
  ISP/VIC interrupts and is followed by a verified persistent-driver reboot.
- Both enabled MSCA channels now generate their polyphase curves through the
  shared scaler library. Exact host regressions cover the T23 unity and
  one-third curves, and a clean device boot published stable 1920x1080 and
  640x360 streams at 25 fps with no fatal kernel, Raptor, or Android log
  entries. The tested open module remains active with SHA-256
  `d1b53dbe46435b1bc5bbb90d29a4b24185c368a79323b97b2432e839c8893848`.
- The math adapter now also owns `fix_point_div_32`, replacing the recovered
  pointer-arithmetic body with the common OEM-compatible wrapped divider.
  Hardware validation decoded 148 main frames in six seconds, 74 substream
  frames in three, and 124 main frames in the five-second post-transition
  check. Night/auto/day transitions passed, ISP/VIC interrupts advanced, and
  `dmesg`, `logread`, and `logcat` contained no new driver fault. The tested
  open module remains active with SHA-256
  `522d1324c640ab16ee1f5653407d275e06914cf4c1d255fa2b3cf4edd750a7f1`.
- T23's active `fix_point_mult2_32` entry point now lives in the math adapter
  and uses the common generation-compatible split multiplier. Unlike the
  T31/T41 full-range helper, this primitive deliberately preserves T23's
  32-bit fractional-product wrap before shifting. The device cycle decoded
  149 frames in six seconds, recorded 148 in six, and decoded 123 in the
  five-second post-transition check. Night/auto/day, image appearance, and
  ISP/VIC interrupts remained healthy; no ISP, Raptor, or kernel fault
  appeared in `dmesg`, `logread`, or `logcat`. The tested open module remains
  active with SHA-256
  `31418bfe7de79751d18e64ff46309fc0674cb7e6e9f39c7cca2055ea8b2b9f7e`.
- The common library and T23 math adapter now own the 32-bit fixed-point
  add/subtract entry points as well. This restores the recovered add function,
  which computed a value but returned zero, while preserving T23's unsigned
  underflow diagnostic in the adapter. The entry points currently have no live
  callers. Hardware validation decoded 149 frames in six seconds, recorded
  123 in five, and decoded 124 in the five-second final check; mode transitions
  and ISP/VIC logs remained clean. The active module SHA-256 is
  `7ec01426f8701170cd13feddba543c52e90cdb424122f47dc8e9949a8c07187c`.
- The same adapter now owns the SDK-declared 64-bit add/subtract entry points.
  Their recovered six-word declarations were the MIPS O32 expansion of
  `(pointpos, u64, u64)`; the adapter uses that authoritative C ABI, retains
  T23's local underflow diagnostic, and delegates wraparound arithmetic to the
  common library. These entry points currently have no live callers. A
  one-shot boot decoded 149 frames in six seconds, passed night/auto/day,
  advanced both ISP interrupt lines, and produced no driver fault in `dmesg`,
  `logread`, or `logcat`. The active module SHA-256 is
  `7ca7b50296bc689ec011ee77fa05efeb09d262eaeeacb555661b44e2456524ac`.
- The SDK's unsuffixed add/subtract pair has the same `(pointpos, u64, u64)`
  contract and now lives beside the `_64` pair in the adapter. There are no
  live callers; retaining both names is required by the vendor ABI. The
  one-shot boot passed night/auto/day, decoded 123 frames in the five-second
  final check, advanced both ISP interrupt lines, and logged no driver fault.
  The active module SHA-256 is
  `086c5467f5024c47ec04c945cd124bf6882b78be96e825c3c65c2215566291b8`.
- The stock T23 and T41 modules implement identical signed 64-bit fixed-point
  rounding semantics. T23's adapter now exports the official `(s64, s32)` ABI
  and delegates to the common helper, replacing a recovered body that invoked
  the arithmetic-shift helper four times and lost the high return word. T23
  currently has no live caller. The one-shot boot decoded 149 frames in six
  seconds, passed night/auto/day, advanced both ISP interrupt lines, and logged
  no driver fault. The active module SHA-256 is
  `3b95aabf4e5014f53ac4fa003553a3cdf86873bd727542723fb064564f5b5669`.
- The unsuffixed and `_64` two-/three-operand multiply bodies remain in the
  recovered core. An attempted adapter extraction built and inserted cleanly
  but made `rvd` exit during startup on two consecutive fail-safe boots.
  Rebuilding the committed local boundary reproduced the accepted
  `3b95aabf...` module byte-for-byte, restored Raptor control calls, passed a
  forced day transition, and decoded 124 main frames in five seconds. Treat
  this boundary as behavior/code-generation sensitive until its recovered
  call sites have typed signatures.
- The shared MDNS layout preserves the full 1080p `0x477e70` used size and
  T23's `0x478000` page-aligned allocation. Two July 30 validation boots
  decoded 127 frames in six seconds and 112 in five, passed night/auto/day,
  and left the open module active with zero ISP interrupt errors.
- The shared NV12 DMA plan now validates QBUF length, complete address range,
  and Y/UV placement before programming MSCA. Its two validation boots decoded
  125 main frames in six seconds, 73 substream frames in three, and 99 main
  frames in the four-second final check without rejecting a live buffer.
  The validated module SHA-256 is
  `0cd4c917624be506cfd35d233ea0aba11f9491486bbd981514de5c3993a4acf4`.
- The recovered child-platform path has symmetric remove handlers for its
  allocated subdevices, IRQs, MMIO mappings, clocks, pads, and channel state;
  the module can be unloaded after the sensor and consumers are stopped.
- The source-derived SC2336 GIB, LSC, DMSC, Gamma, static AWB, BCSH, and CLM
  startup images produce a clean, artifact-free image with working ISP/VIC
  interrupts. The exact LSC image programs all 651 OEM mesh nodes. The CLM
  image follows the T23 startup CT of 5000 K and programs both OEM LUT banks.
- Gamma runtime state now uses the OEM 129-entry `u16` table shape and an
  actual current-table pointer instead of a 16-byte array treated as a
  pointer. Linear and WDR payloads are exactly `0x102` bytes, the SC2336
  register image seeds both software tables, WDR selection switches the live
  table, and API updates refresh ADR's inverse-gamma tone-map data. Gamma init
  is 21 versus 24 OEM instructions and ADR gamma refresh is 145 versus 119,
  reducing the audit to 33 hard stubs and 77 collapses. A default device cycle
  decoded 261 measured RTSP frames at `YAVG/UAVG/VAVG/SATAVG`
  `114.9/128.4/125.7/6.4`, reached `12058/1048` ISP/VIC interrupts without
  faults, and rebooted module-clean.
- Both private and public 64-bit fixed-point log2 paths now retain their full
  normalization and iterative fractional-bit algorithms. The private routine
  follows the OEM helper-based leading-one path instead of collapsing to a
  tail wrapper; it is 69 versus 96 OEM instructions while the public routine
  remains 93 versus 129. This reduces the audit to 32 hard stubs and 77
  collapses. A default device cycle decoded 262 measured RTSP frames at
  `YAVG/UAVG/VAVG/SATAVG` `106.8/129.6/123.8/8.7`, reached `14774/1281`
  ISP/VIC interrupts without faults, and rebooted module-clean.
- The active LSC LUT writer now retains its CT-region interpolation, packed
  12-bit mesh blending, gain-strength scaling, and clamping in the matched
  function instead of having GCC outline that work into recovered-only helper
  symbols. Its emitted body is 337 versus 325 OEM instructions, reducing the
  audit to 32 hard stubs and 76 collapses without changing the register image.
  A default device cycle decoded 261 measured RTSP frames at
  `YAVG/UAVG/VAVG/SATAVG` `112.5/129.0/124.5/8.1`, reached `13920/1211`
  ISP/VIC interrupts without faults, and rebooted module-clean.
- Internal TISP stream-on now copies the OEM 156-byte channel descriptor
  fields, updates the shared sensor-info tail, and registers the total-gain,
  analog-gain, exposure, color-temperature, and IR event callbacks. The body
  is 80 versus 71 OEM instructions, replacing a two-instruction hard stub and
  reducing the audit to 31 hard stubs and 76 collapses. A default device cycle
  decoded 261 measured RTSP frames at `YAVG/UAVG/VAVG/SATAVG`
  `110.6/129.1/124.4/8.1`, reached `13673/1188` ISP/VIC interrupts without
  faults, and rebooted module-clean.
- Core startup derives the OEM `0xb5742209` non-WDR top-bypass mask from the
  deployed SC2336 tuning blob, with the same value as its read-failure
  fallback. The recovered block overrides produce final mask `0xb5742a89`.
  A 2026-07-16 no-argument run passed 256 RTSP frames and moved frame
  `UAVG/VAVG` from `122.8/128.2` to `128.2/127.3`, closely matching the OEM
  reference `127.9/126.8` while removing the green/purple spatial cast.
- Public module-control set/get now preserve the upper 13 top-bypass bits,
  replace the OEM lower 19-bit control field, and round-trip the MDNS luma
  filter state through bit 31. Both functions are within `0.92x..1.03x` of
  their OEM instruction counts. A 256-frame device regression retained live
  register `0xb5742a89`; the audit improved from 51 stubs/91 collapses to
  49/90.
- The CLM color-lookup runtime now implements the T23 HLIL five-region CT
  selector, 200 K transition margins, 32-unit update hysteresis, and exact Q12
  interpolation for all 1,050 hue/saturation entries. The register commit is
  bounded to the four 420-word OEM banks. Default CT5000 and diagnostic CT4200
  interpolation cycles each passed 256-frame RTSP tests; live AWB moved the
  latter from region 3 to region 4 with one table update. All three CLM profiles
  in this SC2336 tuning blob are zero, so this recovery is behaviorally neutral
  and does not address the remaining green/yellow cast.
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
  programming. The OEM 16-channel ratio scaler and complete linear/WDR table
  selector are recovered; their assembly sizes are 785/703 and 221/231 versus
  OEM. Both the default linear bank and diagnostic `source_sdns_wdr=1` bank
  passed 256-frame RTSP cycles with active ISP interrupts and no kernel errors.
  The latter changes only SDNS tuning tables; it does not enable sensor or core
  WDR mode.
- The T23 ADR front end now loads all 44 tuning regions from the exact
  `0x13f7c..0x14b44` SC2336 payload, reconstructs the 5x5 geometry, and writes
  the static tone-map register image. Parked and explicitly active cycles both
  passed 256-frame RTSP tests. Static activation increased contrast and
  highlight clipping without correcting the remaining color cast, so
  `source_adr_internal_enable=0` is the default. The large recovered
  `tiziano_adr_algorithm` and `Tiziano_adr_fpga` bodies remain unsuitable for
  dynamic use; `source_adr_dynamic=0` prevents their event registration.
- The T23 defog front end loads its exact `0x13728..0x13f7c` SC2336 payload,
  programs the OEM 10x18 block boundaries, and recovers the 3x3 max filter,
  weighted spatial filter, 32-bin LUT reducer, strength interpolation, and
  arbitrary-resolution radial geometry builder. Default parked, dynamic
  geometry, and static-active cycles each passed 256-frame RTSP tests. Static
  activation did not materially change channel means or the severe highlight
  clipping, so `source_defog_internal_enable=0` remains the default. The large
  statistics-driven `tisp_defog_soft_process` is still a hard stub and is not
  registered on the source event path.
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
- Optional `source_ae_hlil` adds a bounded process-context exposure controller.
  Its sorted ladder covers short unity-gain integration times through the
  sensor's real 1195-line boot setting and maximum integration, then the nine
  verified analog-gain states. Every `source_ae_hlil_interval` snapshots it
  selects the nearest proportional exposure for the tuning-derived luma target
  of 60, with a default deadband of 5 and a four-rung slew limit. Sensor I2C
  never runs in IRQ context, and gain-dependent GIB/DMSC/DNS/sharpen tuning is
  reapplied only when analog gain changes. Read-only counters expose runs,
  updates, dropped schedules, last luma, current state, raw Q10 EV, and status.
  This is the proven active branch, not the full recovered OEM AE solver, and
  requires `source_ae_stats_init=1`.
  The 2026-07-16 default device smoke converged from 1195 to 600, 256, 128, and
  96 integration lines while measured AE luma fell from 216 to 61. The stream
  passed 256 RTSP frames; frame `YAVG` fell from the prior clipped 222.1 to
  116.8 and `YHIGH` from 255 to 188.
- The T23 integer and 64-bit fixed-point log2 family now follows the HLIL
  normalization and fractional-bit iteration instead of returning zero. This
  restores the gain-conversion helper used by sensor analog/digital gain and
  EV reporting; the private 64-bit entry intentionally forwards to the same
  recovered implementation. The 32-bit core compiles to 63 instructions
  versus 65 OEM, and its fixed-point wrapper is 14 versus 14. A default device
  cycle decoded 257 measured RTSP frames at `YAVG/UAVG/VAVG`
  `109.6/128.0/126.3`, retained active ISP/VIC interrupts with `ERR 0`, and
  completed without kernel errors. The binary audit moved from 46 stubs and
  87 collapses to 45 and 86.
- The active BCSH RGB/YUV transforms now implement the T23 HLIL sign/magnitude
  expansion, both fixed-point matrix products, hue interpolation, and signed
  chroma rotation. `tiziano_bcsh_Tccm_RGBYUV` now compiles to 453 instructions
  versus 559 OEM, and `tiziano_bcsh_Toffset_RGBYUV` to 107 versus 154; both
  moved out of the collapsed audit class, reducing the total to 84. The
  `/dev/isp-m0` shim also dispatches the OEM 8-byte basic and 16-byte extended
  set/get ioctl layouts instead of returning success without touching the
  driver. Brightness, contrast, saturation, sharpness, and hue now initialize
  and round-trip at 128 through `libimp`; a live `112/144/160/140/64` test
  visibly exercised the recovered matrix path before restoring neutral state.
  A no-argument device cycle decoded 258 measured RTSP frames at
  `YAVG/UAVG/VAVG` `105.2/129.3/126.1`, retained active ISP/VIC interrupts,
  completed without kernel errors, and rebooted cleanly.
- Highlight depression and backlight compensation now preserve the OEM
  44-byte AE scene layout instead of sharing an optimized two-instruction
  stub. Their setter/getter paths update the correct words, set the OEM AE
  refresh flags, and are dispatched explicitly for controls `0x0800002a` and
  `0x08000037`. Highlight round-tripped `0 -> 32 -> 0` through `libimp`;
  direct OEM-layout ioctls confirmed backlight `0 -> 3 -> 0` (Raptor does not
  include the backlight getter in its aggregate query). A restored no-argument
  cycle decoded 256 RTSP frames at `YAVG/UAVG/VAVG`
  `108.2/129.5/125.8`, retained `30432/2698` ISP/VIC interrupts without new
  errors, and rebooted cleanly. The binary audit now reports 44 hard stubs and
  84 collapses.
- The AE1 static interrupt handler now follows the T23 bank-select, 4 KiB DMA
  synchronization, AE statistics decode, and event-flag sequence. It compiles
  to 33 instructions versus 36 OEM with all three calls and ten relocations
  retained, reducing the binary audit to 43 hard stubs. A no-argument linear
  mode regression decoded 256 RTSP frames at `YAVG/UAVG/VAVG`
  `108.3/129.3/125.8`, retained `13680/1185` ISP/VIC interrupts without new
  errors, and rebooted cleanly.
- AE ROI and zone weighting now preserve the OEM two-argument ABI, 225-word
  inverse-ROI transform, day/night parameter-bank copies, 900-byte get/set
  contract, refresh flags, and AE trigger. This removes a latent null access
  in the generated zone setter, which consumed the context in `a0` instead of
  the weight pointer in `a1`. The ROI transform compiles to 76 instructions
  versus 77 OEM and the zone path to 39 versus 41. AWB day/night refresh also
  restores the OEM first-frame marker, parameter reload, and hardware reload.
  A default device cycle decoded 258 measured RTSP frames at
  `YAVG/UAVG/VAVG` `106.2/129.1/124.7`, retained active ISP/VIC interrupts
  without new errors, and rebooted cleanly.
- The shared T23 tuning object now has the OEM `0x28944`-byte extent, so its
  `0x15844`-byte active bank at offset `0x13100` no longer overwrites adjacent
  globals. Day/night, custom-mode, and binary switching copy into that bank,
  derive the top-bypass bits from 32 consecutive active-bank words, and run
  the OEM refresh sequence. Their recovered sizes are 160/180, 150/161, and
  151/183 instructions respectively; tuning disable is an exact 15/15. The
  audit now reports 42 hard stubs and 84 collapses. A default device cycle
  decoded 260 measured RTSP frames at `YAVG/UAVG/VAVG/SATAVG`
  `105.0/128.1/125.9/6.7`, retained `26075/2282` ISP/VIC interrupts without
  new faults, and rebooted cleanly.
- MSCA parameter updates now recalculate each enabled channel, pack both OEM
  scaler registers, regenerate/write the channel curves, and commit the
  shadow configuration. The public scaler-level control restores automatic
  versus fixed-level mode, the `0..128` bound, per-channel level fields, and
  global curve-mode bits. They compile to 141/130 and 91/131 instructions,
  reducing the audit to 40 hard stubs with 84 collapses. A default device
  cycle decoded 257 measured RTSP frames at `YAVG/UAVG/VAVG/SATAVG`
  `105.7/128.3/125.9/6.5`, retained `15305/1328` ISP/VIC interrupts without
  new faults, and rebooted cleanly.
- The MSCA global output-window writer now derives the active bounds from all
  three enabled channel geometries, falls back to the sensor dimensions when
  no channel is active, writes the T23 `0xd090/0xd094` extent registers, and
  reruns scaling before the shadow commit. It compiles to 113 instructions
  versus 143 OEM and moves out of the collapsed class, reducing the audit to
  40 hard stubs and 83 collapses. A default device cycle decoded 262 measured
  RTSP frames at `YAVG/UAVG/VAVG/SATAVG` `112.4/129.4/124.9/7.0`, retained
  `19594/1698` ISP/VIC interrupts without new faults, and rebooted cleanly.
- The T23 HLDC path now loads its exact 72-byte SC2336 tuning block from file
  offset `0x14b44`, packs all eleven `0x9000..0x9028` parameter registers,
  and commits shadow register `0x9044` from the direct source startup path.
  The recovered strength interpolation, validity check, radial fixed-point
  calculation, attribute controls, and parameter-array controls replace the
  generated hard stubs and unsafe pointer bodies. Every live HLDC register
  matched the OEM module in a same-device comparison. A default recovered
  cycle decoded 264 RTSP frames at `YAVG/UAVG/VAVG/SATAVG`
  `109.6/128.9/126.1/6.5`, retained `37615/3261` ISP/VIC interrupts without
  new faults, and rebooted cleanly. The binary audit now reports 39 hard stubs
  and 83 collapses.
- Sharpening initialization now selects all nine OEM linear/WDR interpolation
  tables through live pointer state instead of nulling the threshold table and
  consulting an unrelated global. Gain refresh consumes those selected tables,
  and the first full refresh retains its requested Q16 gain. The initializer
  now compiles to 75 instructions versus 85 OEM and the WDR selector to 59
  versus 67, moving both out of the collapsed class. A default device cycle
  decoded 265 measured RTSP frames at `YAVG/UAVG/VAVG/SATAVG`
  `107.1/128.3/125.6/6.6`, retained `17829/1551` ISP/VIC interrupts without
  new faults, produced a normal detailed color frame, and rebooted cleanly.
  The binary audit now reports 39 hard stubs and 81 collapses.
- `tisp_set_fps` now implements the SC2336 timing contract directly because
  the deployed T23 sensor module has no functional FPS ioctl slot. It reads
  HTS over the captured sensor I2C client, derives VTS from the 81 MHz pixel
  clock, writes `0x320e/0x320f`, and synchronizes all four integration-limit
  fields. `source_sensor_fps=0x190001` exercised 25/1 fps and reported the
  expected `HTS=2250`, `VTS=1440`, and `max_it=1436`; the stream passed 256
  frames at `UAVG/VAVG` `128.5/126.5`. The recovered API is `1.28x` OEM size
  and reduces the audit from 48 to 47 hard stubs without adding a collapse.
- An optional `source_awb_hlil` workqueue implements the active SC2336 branch
  of the T23 AWB algorithm: calibrated zone ratios, tuning-mesh weighting,
  indoor light-source distance-LUT weighting, distance refinement, history,
  live-EV RGBG-weight selection, inverse-temperature interpolation, and OEM
  gain conversion. The T23 HLIL call frame supplies `_rgbg_weight[_ot]` as the
  zone-selection mesh and `_color_temp_mesh` as the final CT mesh; keeping
  those roles distinct corrected the collapsed port's roughly 61,000 K output
  to a stable 4,400 K result in the same 256-frame no-argument run.
  Public WB controls now implement the T23 mode table, auto-gain reporting,
  relative-manual mode, start-gain accessors, and a freeze state honored by
  the live workqueue. A second 256-frame no-argument run retained neutral
  `UAVG/VAVG` of `128.0/127.1`. The binary audit moved `tisp_g_wb_mode` from a
  hard stub to the OEM instruction count and `tisp_s_wb_mode` out of the
  collapsed class, reducing totals from 52 stubs/93 collapses to 51/91.
  The earlier Q12 gray-world loop remains available only as a diagnostic
  fallback.
- Dynamic BCSH color-temperature and exposure updates are enabled by default.
  Source startup now initializes the complete SC2336 BCSH tuning state before
  restoring the exact 29-register startup image, so later AWB/AE events no
  longer fall back to the compiled identity matrix. The recovered four-point
  CT sorter replaces generated byte-offset arithmetic that caused an unaligned
  kernel access, and the seven-region interpolator implements the T23 200 K
  plateaus, forced refresh, and redundant-plateau suppression from the HLIL.
  Its assembly size is 223 instructions versus 272 OEM and is no longer
  classified as collapsed. A no-argument device run followed live AWB into CT
  region 3, decoded 257 RTSP frames at `UAVG/VAVG` `128.0/125.4`, retained
  active ISP/VIC interrupts with `ERR 0`, and produced no kernel faults.
  `source_bcsh_trace=1` provides a bounded four-update matrix trace for tuning
  diagnostics.
- The exact T23 CCM startup path applies the tuning blob's EV-derived
  saturation transform instead of writing the raw daylight matrix. It is
  stable and less extreme than the raw matrix, but the best verified startup
  keeps the top-level CCM bypassed. A 2026-07-16 no-CCM run retained adaptive
  AE/AWB, passed 256 RTSP frames, and removed much of the recovered path's
  exaggerated saturation and purple cast. `source_ccm_tuning_init=1` remains
  available for diagnostics while the pre-CCM spatial color error is repaired.
  The shared `cm_control` now implements the T23/T31 sign-decomposed 3x3
  fixed-point saturation multiply instead of byte-stepping through globals
  and consuming an uninitialized matrix. The active CCM diagnostic exercised
  two runtime CT updates and decoded 258 frames at
  `YAVG/UAVG/VAVG/SATAVG` `106.0/128.2/125.0/9.3`; the expected saturation
  increase confirms the matrix was committed. The no-argument regression
  retained the preferred bypassed image for 258 frames at
  `107.5/128.5/125.4/6.7`. Both runs had active ISP/VIC interrupts, no new
  faults, and clean reboots. `cm_control` now compiles to 135 instructions
  versus 229 OEM and moves out of the collapsed class, reducing the audit to
  39 hard stubs and 80 collapses.
- The VIN sensor-command interface now exposes the reconstructed file
  operations at `/proc/jz/isp/vin`, including bounded user-buffer handling,
  read-only sensor-register access, sensor-register writes, and complete
  procfs init/unwind/exit ownership. The open callback has the exact OEM
  instruction count, the show callback is 29 instructions versus 28 OEM, and
  the command parser is 236 versus 308 with 13 of 20 OEM calls retained. This
  removes `video_input_cmd_set` from the hard-stub class and reduces the audit
  to 38 hard stubs. On device, read-only commands returned the SC2336 ID bytes
  `0xcb/0x3a` from registers `0x3107/0x3108` before streaming.
- The full GIB de-IR table path now uses the OEM 33-word RGB bank dimensions,
  32-register channel packers, four-threshold region selector, linear
  interpolation, transition hysteresis, and update gate. Tuning is loaded
  from the SC2336 file's exact `0x2ab4..0x314c` range; the active-bank fallback
  observes the recovered `0x13100` tuning-object offset. This also replaces
  fourteen mis-typed 16 KiB globals and the four-byte blue table/header that
  received 132-byte/48-byte copies, reducing recovered BSS by 229,376 bytes.
  The parameter refresh is exactly 142/142 instructions, the interpolator is
  25/23, the LUT packer is 82/85, and the region selector is 143/164 versus
  OEM. Diagnostic `source_gib_ir_value=64` loaded IR points `5/50/51/128`,
  selected region 1, and decoded 259 measured frames at
  `YAVG/UAVG/VAVG/SATAVG` `108.0/129.3/125.1/6.8`. The no-argument regression
  decoded 259 frames at `109.8/129.3/125.0/7.2`, retained `17360/1507`
  ISP/VIC interrupts without faults, and rebooted cleanly. The audit now
  reports 38 hard stubs and 79 collapses.
- The T23 event subsystem now preserves the OEM two-channel shape: each
  channel has 80 reusable 48-byte records, separate pending/free lists, a
  20-jiffy completion wait, and ten callback slots. Push and process paths
  protect only list transitions, so algorithm callbacks run outside the
  queue lock as in the T23 binary. The recovered callback ABI also retains
  the OEM channel and duplicated event argument before the eight payload
  words. AWB, ADR, and defog producers now enqueue initialized records and
  register their actual interrupt/event functions instead of passing stack
  addresses or tuning-data offsets as callbacks. `tisp_event_process` is 145
  instructions versus 129 OEM, replacing its two-instruction hard stub and
  reducing the audit to 37 hard stubs and 79 collapses. A no-argument device
  cycle read the SC2336 `0xcb/0x3a` ID, decoded 260 RTSP frames, reached
  `18303/1590` ISP/VIC interrupts without kernel or Raptor faults, and rebooted
  module-clean.
- The custom-AE ioctl path now retains the OEM control snapshot and histogram
  metadata layout, normalizes long/short integration and gain through the
  sensor callbacks, computes the compensating ISP gains, and emits events
  4/5/6 only when analog gain, total gain, or exposure changes. Its init and
  handle functions audit at 86/99 and 240/322 OEM instructions instead of
  leaving the handle as a two-instruction stub. The shared AE register writer
  now performs both halves of the OEM transaction: it opens the `0xa000`,
  `0xa800`, or `0x1070` write gate and then writes the requested register.
  Hardware setup consequently commits the missing `0xa028/0xa828` terminal
  words. This reduces the audit to 36 hard stubs and 79 collapses. A default
  device cycle decoded 262 measured frames at `YAVG/UAVG/VAVG/SATAVG`
  `107.8/129.2/124.5/7.3`, reached `12917/1120` ISP/VIC interrupts without
  current-cycle faults, and rebooted module-clean.
- The ADR image interpolation helpers now use the T23/T31 bounded lookup
  algorithms instead of hard stubs. `subsection_map` finds the nearest entries
  in the 512-word response table, interpolates the selected point through the
  129-node gamma curve, and applies the requested fixed-point blend;
  `subsection_up` reconstructs all seven rounded interior breakpoints between
  the fixed 0 and `0xfff` endpoints. The AWB cluster ioctl wrapper also
  preserves all seven o32 stack arguments when forwarding its eleven-argument
  call. Their recovered/OEM instruction counts are `150/155`, `60/61`, and
  `55/26`, reducing the audit to 33 hard stubs and 79 collapses. A default
  device cycle decoded 261 measured RTSP frames at `YAVG/UAVG/VAVG/SATAVG`
  `111.6/128.9/124.9/6.9`, reached `13101/1138` ISP/VIC interrupts without
  faults, and rebooted module-clean.
- `source_lsc_ct` selects a generated SC2336 lens-shading image. The default is
  the OEM 5000 K startup; 3300 K is an exact A-to-T interpolation retained for
  indoor color diagnostics. The recovered transform primitives now use the
  exact 31-row by 42-node padded mesh, reverse its 252-byte packed rows through
  the OEM-sized `0x190` persistent scratch buffer, and exchange packed 12-bit
  mirror lanes without corrupting the adjacent coefficient. The public
  horizontal/vertical flip wrapper also forwards all five arguments. A
  256-frame device cycle retained neutral `UAVG/VAVG` of `128.2/127.1`; the
  binary audit moved the row transform out of the hard-stub class, reducing
  totals from 49 stubs/90 collapses to 48/90. Its recovered instruction count
  is `0.98x` OEM and the wrapper has the exact OEM instruction count. The full
  mirror/flip routine now seeds aligned mutable A/T/D tables, applies the OEM
  state-change transforms across all 1,953 words, and keeps that orientation
  through later CT and gain refreshes. It is `0.92x` the OEM instruction count
  and reduces the remaining collapse total from 90 to 89. Default and forced
  `source_lsc_initial_flip=1 source_lsc_initial_mirror=1` cycles each passed
  256 RTSP frames; the forced path retained neutral `UAVG/VAVG` of
  `127.9/126.6` without new shading or packed-coefficient artifacts. The exact
  T23 mesh-size allowlist, padded-stride check, and LUT-capacity check now
  guard that transform path, while day/night refresh resets the mutable tables
  and orientation state before forcing an update. The validator moved from
  `0.24x` to `0.69x` OEM size and the refresh is `1.12x`; a forced validator
  cycle passed 256 frames at `UAVG/VAVG` `128.5/126.7` and reduced the audit
  total from 89 to 88 collapses.
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
