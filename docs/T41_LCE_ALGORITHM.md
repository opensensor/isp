# T41 histogram-driven local contrast

LCE no longer replays an OS04D10 scene curve. The native implementation takes
the loaded 0x15c-byte calibration block, input dimensions, log2-Q16 gain and
45 current 32-bin histograms. It produces 32 overlapping-neighborhood curves,
each with 16 eight-bit entries. No sensor identity or captured output is an
algorithm input. The 0x5934-byte workspace is allocated once.

Geometry derives four cell areas and nine overlapping-neighborhood areas
from a 9x5 grid. Reciprocals retain the OEM's integer truncation and precision
selection. Even the OEM's special 1920x1080 case equals this generic formula;
there is no resolution-specific branch in the native implementation.

The processing stages are:

1. Normalize global and overlapping local histograms to Q20. Smooth with
   calibrated five-tap weights, redistribute rounding residue, and select
   histogram peaks/medians or configured/mean-luminance centers.
2. Apply calibrated head/tail cuts, piecewise histogram limits and contrast
   redistribution. Bright/dark cumulative corrections and WDR light locking
   preserve their separate fixed-point divisions and signed intermediates.
3. Compare current and historical histograms, converge centers/curves at
   calibrated rates, combine global/local curves, then quantize and pack
   the 132-write curve-RAM transaction.

Ten eleven-knot gain tables feed the shared global/local algorithm. WDR
selection is a **16-bit** field; the decompiler's byte interpretation was
incorrect. Invalid shapes, centers and zero divisors are rejected rather
than reproducing OEM faults. Histogram normalization overflow leaves the
last committed curve intact in the driver.

The histogram-clear handshake now finishes bank 0 at **0x501c0**, not the
incorrect 0x50000. Both histogram banks are initialized. The old compatibility
writes at 0x1d000 are removed: that is the short-exposure DPC aperture, not
LCE. The native LCE path writes only its geometry, curve and statistics ports.

The IRQ reads/acknowledges completed histogram banks under a statistics
spinlock. The drained frame worker takes a private snapshot and runs the
algorithm under an LCE mutex; it does not run histogram processing in IRQ
context or allocate per frame. Calibration replacement, gain changes,
WDR/day-night refresh, initialization and teardown use checked ownership.
The historical `t41_stock_lce_ram_profile` parameter now selects the native
path. Read-only `t41_lce_error`, `t41_lce_gain` and `t41_lce_frames` expose
progress. Missing statistics return EAGAIN without uploading an empty map.

Reproduction:

```sh
make -C tests check
sh tools/build_t41_lce_oracle.sh CROSS_PREFIX STOCK_OBJECT OUTPUT_DIR
qemu-mipsel OUTPUT_DIR/lce-oracle-check
```

The SHA-locked OEM userspace oracle matches 10,000 synthetic geometry,
calibration, smoothing, contrast, light-lock, convergence and register-writer
cases, plus 100 sequences of 100 complete frames, on QEMU and physical T41.
Full-workspace comparisons include held histograms and all reset modes.
Intentional over-range normalization and extreme histogram tests produce
OEM diagnostic messages; those are counted separately from mismatches.
Production has no OEM executable fragments. Host bounds/canary tests also
pass ASan/UBSan over 12,000 complete processing calls.

The one-shot T41 build passes three TCP and three UDP reconnects, a 120-second
H.264/AAC decode with no warnings, and a sweep to maximum sensor gain with
all tuning-block errors zero; automatic exposure is restored afterward.
LCE frame processing advances normally. An outdoor
snapshot is not a controlled color/noise reference. Optional OEM diagnostic
overlays/show-data formatting and physical WDR/day-night transitions are not
established by these forced-day tests. Full AE/AWB and ADR parity remain
separate work.
