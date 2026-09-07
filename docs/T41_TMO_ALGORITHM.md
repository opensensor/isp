# T41 local TMO algorithm recovery

## Current frame integration

The safe IRQ path now schedules a process-context worker for completed TMO
statistics. The worker owns the existing sum/count/map/history allocations;
there is no per-frame heap allocation or hard-IRQ histogram scan. Sequence
and bank checks reject a snapshot if DMA completion advances during the
copy. AE publishes integration and log gain as a pair. Curve shadow, runtime,
detail strength, map and temporal history are updated under one mutex, and
TMO bypass is cleared only after the first nonempty map has been uploaded.

Stream stop closes the scheduling gate under the same IRQ lock before
draining work; destruction then frees the objects. Parameter refresh resets
history and waits for another completed frame. Live diagnostic replays are
rejected while the worker owns TMO. Unrecovered manual face/curve writers
are rejected. The IRQ lock is explicitly placed in `.data`, outside the
generated driver's aliased legacy BSS. An initial one-shot candidate with
the lock in BSS failed to return on the network; no boot log established
the cause. The storage-corrected candidate booted normally.

The corrected one-shot camera run processed more than 8,600 frames with no
rejected snapshots and passed three TCP and three UDP reconnects. A short
CPU sample showed 85.7% total idle; this is not a long-run performance gate.
The camera was then rebooted back to the unchanged persistent baseline.
CCM integration is documented in [T41_CCM_ALGORITHM.md](T41_CCM_ALGORITHM.md).
The earlier diagnostic-only results below are retained as recovery history.
Full day/night, WDR, kernel-selector mode 2 and EV-curve mode 1 remain separate
work; they are not certified by the daytime frame test.

The local map is computed from **current statistics**, not a sensor-name
profile, captured register bank, or saved `statYOut`. The scalar implementation
is `driver/t41/tx_isp_t41_tmo_map.h`. It implements kernel-selection modes 0
(per-tile calibration indices) and 1 (uniform calibration indices). Both
produce a scene-dependent local map. Mode 2's statistics-driven **kernel
selector** is still unsupported; this is different from the EV curve mode
at parameter offset `0x55c`, whose mode 1 is also still unsupported.

## Evidence and arithmetic

Reference: H20250310a `tisp_tmo_fpga`, unwrapped `.text` 0x69380..0x69fa0;
the matching wrapped object has the function at 0x69330..0x69f50. The missing
MXU operations were decoded using Ingenic's *XBurst ISA MXU3 Programming
Manual 1.0*: ADD/MUL, SLL/SRL, REPIW, BSHLI, LAO/LAD and SAO. The register
fields refer to full vectors, not independent arithmetic quarters.

1. Input is 375 tiles (25 by 15), each with ten interleaved u16 residual-sum
   and count pairs. Output has ten planes of 375 u32 coefficients.
2. Reconstruct the luminance sum as `(sum * 512 + count * 819 * bin) >> shift`.
   `shift` comes from the geometry calculation, not a sensor-specific constant.
3. Smooth counts and reconstructed sums with a calibration-selected spatial
   Gaussian, skipping neighbours outside the grid. Preserve the MXU u32
   product/accumulator truncations and the radius-dependent pre-shift.
4. Zero-pad two bins at either end and apply a five-tap range Gaussian.
   The count denominator is shifted by `shift + 5`, with zero replaced by
   one. Preserve the separate numerator shifts and final 15-bit extraction;
   algebraically merging them changes quantization.
5. Optionally clamp to `round(bin * 4095 / 10)`, then clamp to 4095. On later
   frames blend `(previous * 15 + current) >> 4`. The first frame is not
   blended with the cold zero workspace.

`tools/gen_t41_tmo_kernels.py` generates the sensor-independent Q8 Gaussian
weights from mathematical widths, with no binary or frame input. Symmetry
reduces storage from 966 coefficients to 210 bytes. Its output matches all
891 `kernelSStatic` and 75 `kernelRStatic` coefficients in the driver's
read-only data. These are general filter kernels, **not sensor calibration**.
The selected indices still come from each sensor's calibration.

## Tests

`make -C tests check` covers zero padding, nonuniform spatial response,
first/subsequent frames, bounds and atomic rejection. The portable helper
also passes ASan/UBSan with strict warnings enabled.

For an independent hardware oracle, supply the exact wrapped OEM object:

```sh
bash tools/build_t41_tmo_oracle.sh /path/to/mipsel-linux- \
    /path/to/stock/tx-isp-t41.ko /tmp/t41-tmo-oracle
```

The generator verifies SHA256
`572ff4553c1a033290ec67d2d9fc384701fbc235c2e335c7e5604100966fb2ee`.
The resulting `tmo-oracle-check` is a **userspace-only test**, with private
memory and non-PIC memcpy/memset/shift leaf dependencies. It has no ISP
access, MMIO, kernel-pointer reads, sensor input or vendor tuning-bin input.
Generated vendor assembly is never linked into the open driver or checked
into this repository. The mixed-ABI link warning is expected for the isolated
non-PIC leaf dependencies; their callable ABI is intentional.

On T41: 660 synthetic frames, all eleven spatial kernels, all fifteen range
kernels, radii 0..4, shifts 0..9, full u16 sum/count range, varying per-tile
indices and history: **2,475,000 output coefficients, zero mismatches**.
Combined reference + scalar test cost was 2.910 CPU seconds. That is a test
measurement, not a live ISP CPU-utilization claim.

## Earlier guarded diagnostic path

The production driver includes only the scalar algorithm and generated
Gaussian kernels. `t41_tmo_map_trigger` is an explicit process-context
diagnostic: a positive value supplies EV, processes a nonempty current DMA
bank, uploads the resulting map and curve, then clears TMO bypass. Zero
restores bypass. Unsupported layouts/modes are rejected. A mutex serializes
this diagnostic against itself and TMO object destruction; it does not certify
the unrecovered day/night, EV-event and frame-worker lifecycle. Keep ordinary
EV fan-out and other manual TMO writers inactive during this experiment.

Automatic processing remains disabled. Do not install a timer or boot-time
one-shot map as a substitute for the missing frame/lifecycle integration.
The new code also corrects the lost value argument in the default-register
writer at 0x1e040 (reference 0x6a5f8).

On the stationary OS04D10 camera, repeated diagnostic updates raised bark
ROI luma from 20.61 (bypassed) to 65.63; nearby stock measured 64.24. Open
paper R/G remained 0.866 versus stock 0.938; bark color was also too saturated.
Illumination was not controlled, so this is evidence of shadow recovery, not
color-chart parity. The existing captured CCM/BCSH/spatial profiles and the
bounded AWB estimator still need their own algorithmic repairs.

The final writer-corrected module repeated 40 live updates alongside three
TCP and three UDP reconnects: all passed without decoder/timestamp warnings
or kernel faults. Its bark luma was 69.16, paper R/G 0.928 and B/G 1.086;
nearby stock was 64.24, 0.938 and 1.137 respectively. Bark R/G was still
1.932 versus stock 1.581. These changing-daylight samples do not establish
that white balance is solved. Raptor's prior shared-origin timestamp fix,
neo audio libraries, sensor module and installed IQ bin were retained.
