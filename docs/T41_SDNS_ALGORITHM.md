# T41 calibration-driven spatial denoise

The portable SDNS boundary takes a 0x384-byte calibration block, log2-Q16
gain and a byte-valued strength. It interpolates 74 fields into a 0x4c-byte
state, preserving its padding byte. The Q7 strength transform is shared
with demosaic through the SoC-independent `tx_isp_tuning_ratio_u32` primitive.

The two calibration writers emit 29 static and 27 dynamic transactions. The
old dynamic path placed runtime +0x4a at bit 12 of register +0x2c; the OEM
places it at bit 18. The repaired static writer also restores unaligned
32-bit table loads, byte-addressed register offsets and the final +0x34
write. It no longer treats `ivdc_threshold_line` as an SDNS object pointer.

The captured 55-word SDNS delta is removed, including writes not owned by
either OEM calibration writer. Initialization, gain, calibration replacement,
day/night parameter refresh and strength controls use checked state access.
TOP bit 18 follows calibration after successful initialization/stream reset.
Kernel channel 1 remains unsupported by the existing state allocation;
the portable writer tests cover both channel address windows.

Reproduction:

```sh
make -C tests check
sh tools/build_t41_sdns_oracle.sh CROSS_PREFIX STOCK_OBJECT OUTPUT_DIR
qemu-mipsel OUTPUT_DIR/sdns-oracle-check
```

Ten thousand synthetic cases compare every interpolated state byte and every
transaction with the SHA-locked OEM userspace oracle, including all strength
values, exact/fractional gain knots, high-gain clamping and independent random
writer inputs. QEMU and physical T41 report zero mismatches. Unaligned and
malformed-buffer host tests pass with ASan/UBSan; demosaic's oracle still
passes after sharing the strength primitive.

The one-shot OS04D10 camera boots with zero SDNS/DMSC/DPC/gamma errors.
This proves the SDNS scalar boundary, not complete temporal denoise, AWB,
AE convergence or a full day/night/WDR pipeline.
