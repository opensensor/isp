# T41 calibration-driven defect-pixel correction

The DPC boundary consumes the active 0x5a2-byte calibration block and a
0x6e-byte runtime state. It replaces the captured OS04D10 threshold/mask
overrides; no captured sensor coefficients remain in this implementation.

`tx_isp_t41_dpc.h` implements both eleven-knot gain interpolators and all
three H20250310a register writers. It preserves byte/halfword widths,
unaligned little-endian fields, unsigned subtraction, the reversed second
threshold pair, mode-dependent neighbor masks, IR extension and transaction
order. Long and short banks emit 28 and 19 writes; static controls emit 62
or 71 writes depending on IR mode. The old override's 0x7098 write is removed:
none of these OEM writers owns that address.

The kernel validates calibration/runtime pointers and propagates errors.
The stream-lifetime-locked tone worker updates the long bank when realized
sensor gain changes, exposing `t41_dpc_gain` and `t41_dpc_error`. The legacy
`t41_stock_dpc_profile` name remains compatible but selects a calibration
refresh, not a captured profile.

Reproduction:

```sh
make -C tests check
sh tools/build_t41_dpc_oracle.sh CROSS_PREFIX STOCK_OBJECT OUTPUT_DIR
qemu-mipsel OUTPUT_DIR/dpc-oracle-check
```

The SHA-locked OEM oracle runs only in userspace against private synthetic
arrays and a register recorder. Ten thousand randomized cases compare both
interpolators and every transaction from all three writers, including all
byte-valued modes, unsigned threshold underflow and both IR settings. QEMU
and the physical T41 report zero mismatches. Host tests include malformed
sizes, output bounds and unaligned arrays; ASan/UBSan passes.

The one-shot OS04D10 camera test boots with no DPC/gamma/sensor-limit error,
passes three TCP and three UDP H.264/AAC reconnects, and follows sensor gain
from unity to its reported maximum (log2 Q16 259142) and back to automatic
unity gain. This tests the scalar DPC boundary, not a complete WDR pipeline
or parity of the remaining ISP blocks.
