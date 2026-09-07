# T41 calibration-driven chroma denoise

CDNS consumes a 0x161-byte calibration block and log2-Q16 gain. Thirty-two
byte-valued fields use the common eleven-knot interpolator. The writer emits
twelve transactions at `(channel + 0x380) << 7`. Its final threshold pair is
signed: clamp the upper endpoint to at least the lower endpoint, then use
`4080 / (upper - lower)` when distinct, or zero when equal. Casting the
endpoints as unsigned before comparison is not equivalent.

The captured three-word profile is removed. Initialization, frame gain
changes, day/night refresh and calibration get/set use checked state rather
than the damaged generated `ivdc_threshold_line` pointer. The OEM gain
hysteresis comes from `gain_thres`. Successful full refreshes update the
applied-gain cache too. TOP bit 19 follows calibration; writing a bank does
not override a sensor's decision to bypass it.

Reproduction:

```sh
make -C tests check
sh tools/build_t41_cdns_oracle.sh CROSS_PREFIX STOCK_OBJECT OUTPUT_DIR
qemu-mipsel OUTPUT_DIR/cdns-oracle-check
```

QEMU and physical T41 pass 10,000 synthetic interpolation/register cases
and all 65,536 signed threshold pairs with zero OEM mismatches. Host tests
also check every pair, unaligned buffers, canaries and invalid dimensions
under ASan/UBSan. The OEM executable is used only as a test oracle. The
kernel's existing allocation remains channel-0 only; portable writer tests
cover both apertures. Full temporal-denoise/AE/AWB parity is separate work.
