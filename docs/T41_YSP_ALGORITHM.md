# T41 calibration-driven sharpening

YSP consumes a 0xa78-byte calibration block, log2-Q16 gain and the saved
unscaled calibration for manual strength changes. Fourteen binding runs
interpolate 165 fields into a 0x112-byte state, preserving padding. The 32
sharpening tables have byte-valued calibration knots but **16-bit runtime
fields**; the source and destination strides are deliberately separate.

The static writer emits 13 transactions. The dynamic writer emits 77:
scalar fields, three piecewise-linear threshold decoders with rounded
reciprocals, three eight-point clamped ramps, and three packed LUT groups.
Only cumulative thresholds saturate at encoded 0xfff. All strength values
use the shared Q7 primitive with maximum 255 on the original 32x11 byte
table, not on the already-scaled output. No register snapshot is involved.

Both captured YSP arrays are removed, including the diagnostic profile that
previously wrote live statistic/result registers. Initialization now writes
the complete calibrated block and restores calibrated TOP bit 17. Frame
work applies gain changes with the OEM 0x100 hysteresis. Full refreshes also
record the applied gain so stream reset cannot leave a stale hysteresis
cache. Day/night, calibration and sharpness controls use checked objects.
An allocation failure after creating the runtime now frees that allocation.

Reproduction:

```sh
make -C tests check
sh tools/build_t41_ysp_oracle.sh CROSS_PREFIX STOCK_OBJECT OUTPUT_DIR
qemu-mipsel OUTPUT_DIR/ysp-oracle-check
```

Ten thousand synthetic calibration/state cases match the SHA-locked OEM
oracle on QEMU and physical T41 with zero mismatches. They cover all 256
strength values, exact/fractional gain knots, independent writer inputs and
every encoded threshold through 0x1000. Host tests additionally check
unaligned buffers, padding, insufficient capacities and signed ramp slopes
under ASan/UBSan. OEM code is confined to the test oracle, not the driver.

The existing kernel object allocation remains channel-0 only. These tests
prove the sharpening scalar boundary, not overall AE/AWB or WDR parity.
