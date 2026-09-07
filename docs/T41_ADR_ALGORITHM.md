# T41 ADR recovery checkpoint

This checkpoint is **offline, not a replacement for the live ADR profile**.
`tx_isp_t41_adr.h` contains portable algorithms and checked buffer interfaces;
the complete frame composition and calibration lifecycle now have independent
OEM tests, but kernel DMA/IRQ/worker integration remains to be completed.
The camera still runs ISP `e6f3128b` (native LCE/MDNS).

Recovered and compared with H20250310a:

- All four register writers: geometry, spatial weights, controls, and the
  194-word curve payload with its five handshake writes. The busy flag is
  read at 0x5040c; curve RAM is not a direct register-window alias.
- DMA unpacking: 24 row-major hardware records become a column-major 6x4
  mesh. Each has twenty 18-bit histogram bins, two 12-bit values and two
  29-bit sums. Global 512-bin data uses three 21-bit bins per 64-bit record.
- Histogram normalization, fourteen-bin local regrouping, global cumulative
  histogram, tile means and global means. Four output arrays are at 0x25c0,
  0x2620, 0x2680 and 0x26e0; confusing these offsets silently changes tuning.
- Geometry and integrated neighboring radial classes. All 31 radial limits
  are generated as `round(8192 * ln(31 / (i + 0.5)))`. Grid coordinates,
  squared-radius scaling, sample counts, rounded neighboring means and
  empty-class propagation depend on dimensions, not sensor identity.
- Gaussian metering weights. Five OEM coordinate arrays reduce to arithmetic
  sequences on a 6x4 grid. Sigma/center/mode clamps, Q10/Q16 operations and
  middle-tile normalization preserve OEM rounding and cache behavior.
- All exposure-table modes and their packed workspace copies. Eleven signed
  64-bit knots select signed 16-bit values, including descending ordinates,
  wrapping products and the OEM's positive half-interval rounding bias.
- Scalar interpolation, curve resampling, gamma lookup, Gaussian local
  strength, recursive shadow quantiles, rational fourth-order curves and
  both in-place slope filters. The two interpolators have distinct narrowing
  and right-ordinate clamps; they are not interchangeable.
- Full frame composition: global dark metering, local strength, three curve
  methods and weighted combinations, both filters, bounded temporal steps,
  gradient limits, bright-scene bypass and optional face-curve maxima.
  History is saved before gradient/bypass/face processing, not after it.
- The parameter-update-only CTC remap: gamma, calibrated piecewise mapping,
  inverse gamma, then hardware-knot resampling. The decompiler omitted this
  branch; direct instruction execution exposed it. Gamma's final abscissa
  is 4095, not 4096.
- Cold initialization, cached-geometry reuse, and day/night or linear/WDR
  parameter replacement. Mode replacement resets visible curves but retains
  temporal history. This is an arithmetic/lifecycle test, not physical WDR
  sensor validation.

The 128 dark-meter weights are generated as
`round(255 * exp(-i*i / (2*50*50)))`. Hardware coefficient banks are
`round(256 * (x/8)^power)` at `x = 0..8,16,32,64`, with powers selected by
the calibrated mode. Neither table is copied from a sensor or live output.
The tile-invariant rational curve is computed once per frame instead of
24 times as in OEM; tile strength blending remains independent.

The rational curve consumes the driver's existing universal 256-entry
curvature-coordinate LUT. This is OEM algorithm data, not a sensor bin or a
captured live curve. Its generating formula has not yet been established;
unlike radial thresholds and metering coordinates, this checkpoint does
not claim to have eliminated that mathematical LUT.

The current sizes are calibration 0xa60, output state 0x320, auxiliary work
0x474 and statistics 0x2748. The exact DMA read footprint is 0xaa0 within an
owned 4-KiB page. Length, channel and dangerous-divisor checks prevent
out-of-bounds writes and reproducing raw MIPS divide-by-zero behavior.
The geometry caller supplies 1 KiB of initialization scratch; there are no
allocator calls inside the portable helpers.

Reproduction:

```sh
make -C tests check
sh tools/build_t41_adr_oracle.sh CROSS_PREFIX STOCK_OBJECT OUTPUT_DIR
qemu-mipsel OUTPUT_DIR/adr-oracle-check
```

The stock object must have SHA256
`572ff4553c1a033290ec67d2d9fc384701fbc235c2e335c7e5604100966fb2ee`.
OEM instructions and reference arrays are confined to a userspace test with
private storage and an MMIO recorder. No test instruction is linked into
production. QEMU and physical T41 each pass 10,000 randomized combined cases
and 105 integrated spatial grids (including 720p, 1080p, 1440p and 2160p),
with zero valid-input mismatches. One extreme local-strength case is rejected
because its wrapped exponential becomes a zero divisor; 1,429 deliberate
busy-curve diagnostics are expected.

An additional 10,000 full-frame/history attempts yield 9,942 valid cases
with no mismatches across all parameter bytes, output bytes, history, knots
and quantiles. Forty generated cases reach an OEM zero divisor; the test
shim records this and excludes their outputs from parity comparison. Native
code rejects them. Eighteen other cases would index outside the 512-bin CDF;
native code rejects these before calling the OEM path. Invalid cases are
counted separately, never reported as matches. The suites also cover twenty
cold starts, each followed by ten calibration/mode replacements.

The standalone host test covers 2,000 primitive cases, initialization and
refresh, and 10,000 frame/history cases under ASan/UBSan with canaries.
Rejected frame candidates must not be committed by the eventual kernel
worker; the last successfully written map must remain active.

Still required before deployment: buffer/IRQ/worker integration and live
stock/open comparisons and stream testing. Arithmetic parity alone is not OEM
image-quality parity. Full OEM AE/AWB also remain separate work.
