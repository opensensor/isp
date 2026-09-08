# T41 AWB algorithm recovery

This is a partial algorithm boundary, **not full AWB parity**. The live driver
still uses its bounded neutral-mesh estimator. The portable primitives below
are not connected to its frame lifecycle until clustering, history and
calibration replacement are recovered and checked together.

The reference is H20250310a `tx-isp-t41.ko`, SHA256
`572ff4553c1a033290ec67d2d9fc384701fbc235c2e335c7e5604100966fb2ee`.
Addresses here are ELF `.text` offsets, not the HLIL export's +0x50 labels.
Production headers are ordinary C algorithms. No OEM instructions, live MMIO
snapshots, sensor-bin coefficients or scene-derived corrections are linked
into them. Calibration and statistics are explicit caller inputs.

## Recovered boundaries

| OEM function | ELF offset | Portable operation |
| --- | --- | --- |
| `tisp_awb_get_statistics` | `0x30540` | `t41_awb_statistics` |
| `tisp_awb_sat_weight` | `0x3106c` | `t41_awb_saturation_weights` |
| `tisp_awb_long_par_update` | `0x30b20` | `t41_awb_prior_prepare` |
| `tisp_awb_spec_calculate` | `0x31fb4` | `t41_awb_special_prepare` |

AWB parameters start at active calibration +`0x978`; runtime state is `0xf54c`
bytes. This is a format boundary shared by T41 sensor calibrations, not a
special case for OS04D10. The runtime owner must privately copy calibration
before calling parameter-mutating algorithms.

Statistics have four luminance-class pairs per zone, with global and neutral
selections in even and odd records respectively. A 16-byte record contains
four 22-bit sums and a 14-bit count. Normal mode consumes 128 bytes per zone;
selected-class mode consumes 32, retaining other classes' previous state.
Each class has five 225-word planes, with fixed 15-zone row stride even when
the configured width is smaller. Global planes start at `0x7f94`, neutral at
`0x3944`; class stride is `0x1194`. Padding and unrelated history are untouched.
The four hardware DMA banks are temporal banks, not extra color phases.

Saturation weighting uses the count gate at parameter +0, precision at
`0xcd2`, and mode at `0xd56` (both **u16**, not byte controls). RGB is adjusted
by supplied previous gains, with OEM low-u32 multiply then right shift.
Mode 1 computes minimum/maximum; other modes use minimum/mean. Empty, gated
or zero-minimum zones get weight 1. A wrapped zero mean divisor is rejected
before modifying any output. Both paths keep OEM shifts/truncations.

The CT prior uses EV thresholds `0x1c/0x20`, three four-knot tables at
`0xb0/0xc0/0xd0`, enables/floors at `0xe0..0xec`, and precision at `0xcd4`.
Transition span is one third of the EV-threshold difference. Knot interpolation
uses low-u32 products and unsigned division; floor interpolation uses signed
division. Disabled day/night priors select/relax the calibrated tables as the
OEM does. Invalid enable values and unsafe EV ranges are rejected. The helper
also builds the complete 45-pointer estimator view using native-sized
pointers; it does not serialize host pointers into the 32-bit hardware ABI.

Special-illuminant regions use ten mode bytes at `0x11f6`, with immediate
handling for slots 0 and 5..9. Region zero waits for the `0x34de/0x34df`
two-frame readiness gate before consuming a caller snapshot of RGB statistics
`0x180ac/b0/b4`. Green zero produces zero ratios, not a divide fault. Mode 1
generates ordered writes for the shared region-pair registers; other nonzero
modes prepare calibrated software ratios. Register writes are returned as
address/value pairs: the helper itself performs no MMIO. Neighbouring regions
share packed words, so whole-word writes and order are significant.

## Validation and reproduction

10,000 randomized combined cases match all changed parameter/state bytes,
all 45 normalized pointers, every ordered register write and the conditional
RGB reads on QEMU: zero mismatches or unexpected accesses. Inputs cover both
DMA modes, all four selected classes, changing dimensions, saturation modes,
all day/night-enable combinations, prior interpolation and special-region
warmup/software/hardware modes. No sensor bin is an oracle input.

The host suite tests 2,000 randomized frames with ASan/UBSan, padding and
canaries, explicit malformed lengths/dimensions, and rejection of a wrapped
zero saturation divisor without partial output. Full `make -C tests check`
also passes. Physical T41 oracle execution is pending.

```sh
bash tools/build_t41_awb_stats_oracle.sh CROSS_PREFIX STOCK_OBJECT OUTPUT_DIR
qemu-mipsel OUTPUT_DIR/awb-stats-oracle-check
make -C tests check
cc -std=c99 -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined \
  -fno-sanitize-recover=all tests/tx_isp_t41_awb_stats_test.c -o /tmp/awb-stats-test
/tmp/awb-stats-test
```

The oracle generator relocates selected OEM functions into a private
userspace fixture. Only the test executable contains reference instructions.
It has no ISP access; its register interface is checked in memory.

## Still required

Full CT clustering/gray-world fallback, class aggregation and ratio history,
convergence/freeze/manual policy, and owned frame-safe runtime with lifecycle
and calibration replacement tests. Existing gain packing and CT-offset
history are separately validated in `T41_AE_GIB_AWB_GAIN.md`; that does not
establish correctness of the estimator feeding them.
