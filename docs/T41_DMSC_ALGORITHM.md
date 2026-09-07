# T41 calibration-driven demosaic

`tx_isp_t41_dmsc.h` replaces both captured OS04D10 demosaic profiles and the
scene-derived gain threshold that selected them. It consumes the active
0x1076-byte calibration block, not live register values or a sensor bin
compiled into the driver.

The algorithm interpolates 306 byte/halfword fields at eleven log2 gain
knots, represented by 54 contiguous field-binding runs. Five fields receive
the OEM Q7 sharpness transform. The static writer normalizes disabled
calibration fields and emits 36 transactions. The dynamic writer applies
enable flags to runtime state and emits 125 transactions. All widths,
unaligned loads, unsigned arithmetic, masks and transaction order are
explicit. A successful refresh commits at 0xa19c; incremental updates retain
the OEM 0x100 log2-Q16 gain deadband.

The old captured writes to 0xa18c/0xa190/0xa194/0xa198 are removed. Neither
of these OEM calibration writers owns those addresses. No register's role
is inferred merely because it appeared in a live snapshot.

Kernel integration checks state/calibration pointers and propagates errors.
`t41_dmsc_error` and `t41_dmsc_gain` expose the last attempted refresh result
and successfully written gain. Sharpness changes rebuild from calibration;
they no longer replay a daylight capture afterward.

Reproduction:

```sh
make -C tests check
sh tools/build_t41_dmsc_oracle.sh CROSS_PREFIX STOCK_OBJECT OUTPUT_DIR
qemu-mipsel OUTPUT_DIR/dmsc-oracle-check
```

The SHA-locked OEM userspace oracle compares 10,000 randomized calibration
blocks, all 256 sharpness values, exact/fractional gain knots, high-gain
clamping, explicit disabled branches, padding preservation and independently
randomized runtime writer inputs. Every state byte and register transaction
matches on QEMU and physical T41. The reference has no live MMIO and is not
linked into the production module. Host unaligned/bounds tests and
ASan/UBSan pass.

The one-shot camera build boots with zero DMSC/DPC/gamma errors and responds
to automatic gain changes. This establishes these scalar demosaic boundaries,
not full OEM AWB clustering, AE convergence or the remaining noise filters.
