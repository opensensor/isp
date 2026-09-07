# T41 calibration-driven CCM

`tx_isp_t41_ccm.h` implements the H20250310a CCM calculation from the
148-byte sensor calibration block, current neutral temperature and EV.
It contains no sensor name, tuning-bin sample, captured matrix or register
bank. The CSC coefficients are the driver's standard Q16 luminance
coefficients, not sensor calibration.

The six temperature boundaries select four signed Q10 matrices, with
alternating constant and interpolated intervals. Nine EV knots select Q8
saturation. CT interpolation rounds signed values away from zero; the
fixed-shift saturation calculation rounds ties toward positive infinity.
Each product is rounded separately, preserving the OEM's 32-bit product
truncation. The CSC complements are independently quantized, which matters
for the third CSC family's green diagonal. Output is clipped to signed
14-bit fields and packed into the eight writer-owned words at 0xb004..0xb020.
The captured writes to 0xb024/0xb028 were removed with the captured matrix.

Invalid inputs leave the helper's output unchanged. The checked domain
requires strictly increasing CT/EV knots, signed 14-bit matrix entries,
Q8 saturation <=1024, and nonnegative custom Q16 luminance coefficients
<=65536. Values outside that domain are rejected, not clamped to a sensor
preset. Manual CCM overrides remain unsupported.

## Independent reference test

```sh
bash tools/build_t41_ccm_oracle.sh /path/to/mipsel-linux- \
    /path/to/wrapped-stock/tx-isp-t41.ko /tmp/ccm-oracle
qemu-mipsel /tmp/ccm-oracle/ccm-oracle-check
```

The generator checks the exact vendor object's SHA256 and relocates only
five bounded reference routines and their leaf dependencies into a
userspace-only test. Register writes terminate in a private array. There
are no ISP ioctls, kernel pointers, sensor inputs or tuning-bin inputs.
Generated vendor instructions are neither committed nor linked into the
open driver. The link warnings for the isolated non-PIC leaf support are
expected. CCM has no MXU instructions and can also run under MIPS emulation.

10,000 synthetic CT/EV/CSC cases, including wide EV intervals: zero differences in selected matrices,
saturation, transformed matrices and packed words; zero unexpected register
destinations. Strict host tests cover unaligned calibration, plateau and
transition boundaries, high-EV clamping and atomic rejection. They also
pass ASan/UBSan.

## Live integration and limits

The active CCM bank is now computed from calibration. The shared neutral
AWB estimator publishes temperature through the sensor's reciprocal-CT
mesh; the frame worker supplies current EV. This is a bounded neutral
estimator, not a claim that the entire OEM AWB clustering/history algorithm
has been recovered. The CSC getter's broken table-pointer arithmetic was
replaced with an explicit seven-table selector. Matrix programming and
object destruction are serialized; unchanged matrices avoid repeated MMIO.

On the T41NQ/OS04D10 one-shot build, automatic TMO plus CCM passed three TCP
and three UDP H.264/AAC reconnects. Relative to the preceding automatic-TMO
build, paper R/G was 0.876 vs 0.845, B/G 1.124 vs 1.158; bark luma was 61.55
vs 60.55. These are changing-daylight observations, not color-chart parity.
The [BCSH follow-up](T41_BCSH_ALGORITHM.md) removes its captured banks and
repairs the exposure adapter's unconditional CCM enable: calibration can
apply color correction in BCSH while bypassing pre-tone-map CCM. Other
spatial profiles are still present. Full
day/night and WDR lifecycle parity is not established by this daytime test.
