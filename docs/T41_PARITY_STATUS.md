# T41 daytime-path exit gate

This is the gate for moving development effort to another SoC, not a claim
that every T41 ISP mode is recovered. The physical target is T41NQ/OS04D10,
2560x1440 at 25 fps, H.264/AAC with Neo audio, forced Raptor day mode and the
unchanged installed sensor calibration. No complete firmware was flashed.

## Verified checkpoint

ISP `1c43bcb4` uses sensor-reported integration limits by default and the
checked calibration/timing-derived flicker-state adapter. The previous
369-line ceiling was a measured mains period, not a sensor maximum. In the
indoor trial, exposure changed from 369 lines at approximately 5.5x analog
gain to 1474 lines at approximately 1.31x, with metered luma near the same
calibrated target. Longer exposure trades motion sharpness for reduced
amplification; this is not a controlled noise or color-chart measurement.

The full host suite, AE ASan/UBSan tests and 10,000 synthetic AE cases against
the OEM instructions pass in QEMU and on the T41. The automatic allocator's
last argument is the luma target, not FPS; see `T41_AE_GIB_AWB_GAIN.md`.
The new ISP passed a cold boot, six TCP/UDP reconnects, 120 seconds of
H.264/AAC decode with an empty warning log, and a 60-second RTP check at
24.986465 fps without long/backwards timestamps or packet-sequence gaps.
All 17 tuning error diagnostics were zero. It is persistently staged with
the prior ISP and complete manifest retained for rollback.

OpenIMP `851486f` delegates AWB to the frame-driven native owner. The AWB,
tone/color, noise-filter and LSC documents describe their algorithm-oracle
coverage and physical limits separately. Verified C calculations consume
calibration and live statistics; OEM reference instructions are test-only.

HAL `2084274` defaults the serial V4L2 encode bridge to one output buffer,
keeping two capture buffers and explicit output-count overrides. The old
fourth buffer spilled into uncached memory and periodically stalled IDRs.
The repair passed a cold-boot 900-second decode with 22,489 frames and no
FFmpeg warnings, plus reconnect and cadence checks. RVD is persistently
staged; no Raptor configuration or Neo audio library was changed.

## Required before calling the normal daytime path finished

1. **Concurrent main/substream/JPEG output.** This is a hard exit requirement,
   not optional follow-up work. The V4L2 checkpoint now exposes main and
   substream; JPEG, OSD and IVS remain disabled on that backend. Exercise
   the shared IMP FrameSource/encoder graph first, then repair the actual
   queue, scaler, encoder and lifetime boundaries it exposes. Main and
   scaled substream must decode concurrently with audio; taking JPEGs and
   stopping/restarting either output must not starve or corrupt the other.
   Check independent output geometry/rates, timestamp continuity, memory
   ownership and queue loss. A second advertised RTSP endpoint is not proof
   that the second media pipeline works.
   The shared IMP checkpoint now has correct generated substream pixels,
   ten non-disruptive substream restarts, mixed 25/15 fps and live substream
   resizing. Reverse-direction restart and software-JPEG stress still expose
   occasional missing source frames. Standalone V4L2 now has three capture
   nodes, shared input ownership, HAL channel plumbing and shared AVPU
   submission/completion arbitration. Its resize crash is repaired, but
   occasional peer source-frame gaps during live resizing remain. Keep this
   requirement open until the selected production backend is validated.
2. **Complete live AE ownership.** The automatic allocator and convergence
   ramp are checked helpers, but `t41_safe_ae_calc_process` still uses the
   conservative three-frame, +25%/-20% controller. Recover and test the
   surrounding target/deadband/history policy, then wire sensor allocation,
   realized analog-gain compensation, ISP digital gain and total-EV/gain
   fanout as one coherent transaction. Respect the sensor's apply delay.
   Do not treat the removed default ceiling as complete AE parity.
3. **AE controls and transitions.** Test automatic/manual handoff, min/max
   limits, compensation, 50/60 Hz, off/AUTO/NORMAL and any supported FPS
   changes. Unsupported manual masks or variable-FPS policies must report
   that fact, not silently succeed. Check recovery after rejected controls.
4. **Matched IQ and production-configuration regression.** Compare stock
   and open with exposure and WB held equal, using the same calibration and
   correct full-range BT.601 decode. Examine neutral color, shadows,
   highlights, temporal noise and motion in available illumination. Test
   main/substream/JPEG combinations that the selected configuration actually
   uses; algorithm oracles do not establish multi-output hardware coverage.
5. **Final endurance and handoff.** Repeat cold boots and longer concurrent
   TCP/UDP sessions with audio, including slow/disconnecting peers. Check
   source cadence and server queue loss as well as decoder logs, and record
   CPU/memory with a matched client/configuration load. Keep exact package
   pins, reproducible evidence and a coherent rollback for the final build.

## Explicitly separate validation work

WDR, IR/ring-LSC paths, other sensor modules and optional AI/region paths
are not covered merely because the daytime stream works. The scaler path
needed by the production substream is required above, not deferred. Hardware
unavailable for those tests need not block the next SoC, provided these
limits remain explicit. Remove remaining experimental sensor-derived
profiles rather than turning captured scene coefficients into defaults.
