# Ingenic T23 ISP Recovery

This directory is the T23 bring-up workspace for matching the OEM
`tx-isp-t23.ko` behavior on Linux 3.10.14 targets.

The initial `tx_isp_t23_recovered.c` file is the kbuild-compatible recovered
whole-driver seed from `tx-isp-t23-v1`, plus the local compatibility shims
needed to load the SC2336 T23 sensor module and start the Raptor pipeline.

Current smoke-test status:

- `sensor_sc2336_t23.ko` loads against the recovered module.
- `/dev/tx-isp`, `/dev/isp-m0`, `/dev/misc-ivdc`, and `/dev/framechan0..3`
  are present.
- `/proc/jz/sensor/sensor0` reports the SC2336 metadata expected by Raptor.
- Raptor starts and publishes stable H.264 streams on both MSCA channels.
- The recovered child-platform path has symmetric remove handlers for its
  allocated subdevices, IRQs, MMIO mappings, clocks, pads, and channel state;
  the module can be unloaded after the sensor and consumers are stopped.
- The source-derived SC2336 GIB, LSC, DMSC, Gamma, static AWB, BCSH, and CLM
  startup images produce a clean, artifact-free image with working ISP/VIC
  interrupts. The exact LSC image programs all 651 OEM mesh nodes. The CLM
  image follows the T23 startup CT of 5000 K and programs both OEM LUT banks.
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
