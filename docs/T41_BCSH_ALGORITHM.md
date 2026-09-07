# T41 calibration-driven BCSH and color routing

The automatic BCSH path now computes its 29 register words from the active
360-byte calibration block, live CT/EV, and the 92-byte CSC description.
All three captured day/low-light BCSH banks and their profile selector have
been removed. No sensor name, sampled tuning-bin contents, or scene register
bank is compiled into the replacement algorithm.

## Calculation and layout

The six CT boundaries select/interpolate four signed Q10 RGB matrices.
The matrix is converted to YUV space with the CSC forward and inverse
matrices. Signed matrix products use the OEM's particular staged rounding;
RGB offsets retain its per-product signed 32-bit truncation. Eleven tables
are interpolated over nine EV knots, including the OEM's unsigned 32-bit
interpolation numerator. They control luminance/chroma thresholds, piecewise
contrast, saturation, and brightness. Invalid threshold ordering takes the
OEM's identity fallback. Register packing covers 0x11000..0x11070.

The helper implements neutral brightness/contrast/saturation/hue API values
(128 each), not manual color overrides. Automatic calibration changes remain
live at those neutral controls. Unrecovered manual setters reject requests
instead of entering unsafe generated code. Parameter access, calculation,
programming, and destruction are serialized. The writer skips unchanged
banks, and the frame worker supplies current CT/EV without allocating memory.

For the H20250310a layout, file BCSH starts at file-header +65536+32700,
whereas its active runtime block is +65536+18288. CCM is a different mapping:
file-header +65536+26468 to runtime +65536+14336. There is no universal
file-to-runtime offset for all modules.

## Independent arithmetic verification

```sh
bash tools/build_t41_bcsh_oracle.sh /path/to/mipsel-linux- \
    /path/to/wrapped-stock/tx-isp-t41.ko /tmp/bcsh-oracle
qemu-mipsel /tmp/bcsh-oracle/bcsh-oracle-check
```

The test-only generator validates the exact vendor object's SHA256 and
relocates bounded routines into a private userspace oracle. Its register
writes terminate in an array, never MMIO. Generated vendor instructions and
reference tables are neither committed nor linked into the open module.

10,000 synthetic neutral-API cases vary all CT matrices, CT/EV, narrow and
wide EV intervals, RGB offsets, thresholds and forward/inverse CSC matrices:
zero word mismatches and zero unexpected register destinations. The tests
use no sensor bin or camera statistics. Both MIPS emulation and the T41 CPU
pass. Host boundary/invalid-input tests and ASan/UBSan also pass.

The wider-EV test additionally caught and corrected a CCM interpolation
overflow discrepancy; its 10,000-case oracle now passes wide intervals too.

## Routing defect found by the live comparison

Correct matrices alone did not produce correct color. The exposure adapter
unconditionally cleared TOP bypass bit 9 when disabling its legacy image
profile. This enabled pre-tone-map CCM even when calibration bypassed it
and applied RGB correction in BCSH. The result applied color correction
twice. The adapter now restores calibration's bit 9, not a captured TOP word.

Other repaired-bank enable paths now restore their own calibration flags
instead of forcing blocks on. This includes CDNS, which was enabled in open
but bypassed in the stock daytime comparison. The shared checked TOP helper
preserves every unowned safety bit and rejects malformed flags atomically.
Programming a valid bank is not evidence that calibration enables its block.

On the T41NQ/OS04D10 one-shot candidate, all 29 readable BCSH words matched
live stock. Restoring CCM routing visibly reduced the exaggerated dark
colors. Six TCP/UDP H.264/AAC reconnect tests and a 120-second decode passed
without decoder or timestamp warnings. Daylight changed sharply between
photographs; these are not controlled color-chart or Delta-E results.

## Scope

This establishes the neutral-API BCSH calculation and fixes color routing,
not whole-ISP OEM parity. Full AWB clustering/history, manual color controls,
day/night/WDR lifecycle, and remaining spatial register replays are separate
work. Only T41NQ/OS04D10 has live validation. Other sensor calibrations are
algorithm inputs, not an assertion that all of them have been tested.
