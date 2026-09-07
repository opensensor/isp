# T41 calibration-driven luminance denoise

The YDNS boundary consumes a 0xf5-byte calibration block and log2-Q16 gain.
It interpolates 22 byte-valued fields using the common eleven-knot gain
interpolator and emits eight register transactions. Packed table loads are
explicitly unaligned-safe. Neither the interpolator nor writer contains
sensor names, captured register values or an OEM executable dependency.

The kernel initialization, gain-change, day/night refresh and calibration
replacement paths now call this checked boundary. They no longer dereference
the incorrectly recovered `ivdc_threshold_line` object pointer. TOP bit 14
follows calibration after initialization and stream reset. The captured
seven-word YDNS override and its duplicate trigger are removed; normal gain
fanout includes YDNS. Failed updates are not cached as successful gain changes.

Reproduction:

```sh
make -C tests check
sh tools/build_t41_ydns_oracle.sh CROSS_PREFIX STOCK_OBJECT OUTPUT_DIR
qemu-mipsel OUTPUT_DIR/ydns-oracle-check
```

The SHA-locked OEM object is used only by the test oracle. Ten thousand
synthetic calibration/state cases compare every state byte and transaction,
including independent writer inputs, both channel apertures, exact and
fractional knots, and high-gain clamping. QEMU and physical T41 report zero
mismatches. Host bounds and unaligned-buffer tests pass with ASan/UBSan.

The existing kernel allocation supports channel 0 only; the portable writer
covers both channels. This scalar-block result does not establish parity for
temporal denoise, AE/AWB convergence or the whole image pipeline.
