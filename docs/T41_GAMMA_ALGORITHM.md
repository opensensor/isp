# T41 exposure-driven gamma

The scalar gamma boundary uses the active 0x238-byte calibration block:
ten u32 EV knots at +0x104, ten byte strengths at +0x22e, and the RGB curve
at +0x12c. It does not contain a captured sensor curve.

`t41_gamma_strength` reproduces H20250310a `tisp_gamma_interp_by_ev`
(0x43490), including unsigned 32-bit interpolation wrap. `t41_gamma_curve`
reproduces `tisp_gamma_strength_transform` (0x433ac): strength plus its
high bit scales the calibration curve against a linear 32-step ramp, rounded
to Q8, with the final endpoint fixed at 4095. Invalid sizes, knot order,
strengths and non-12-bit samples fail before output changes.

The existing three-RGB-bank writer preserves adjacent-point packing and
commit order. The stream-lifetime-locked tone worker now updates gamma from
integration times realized linear gain. Previously only initialization and
explicit diagnostic/control paths updated it. Unchanged strength skips LUT
transformation and MMIO; frozen gamma is not overwritten.

Reproduction:

```sh
make -C tests check
sh tools/build_t41_gamma_oracle.sh CROSS_PREFIX STOCK_OBJECT OUTPUT_DIR
qemu-mipsel OUTPUT_DIR/gamma-oracle-check
```

The test-only OEM oracle is SHA-locked and uses private userspace arrays,
synthetic calibration and a register recorder; no live MMIO, sensor bin or
production dependency on extracted OEM instructions. Ten thousand cases
cover wide/narrow EV intervals, exact knots, forced upper selection, unchanged
strength, freeze, all 129 curve points and all 390 register transactions.
QEMU and physical T41 both report zero mismatches. Host ASan/UBSan passes.

The one-shot T41NQ/OS04D10 boot reports gamma_error=0 and follows live EV.
In the tested daylight scene calibration selects strength=255, so no visual
improvement is claimed for this change. Three TCP and three UDP H.264/AAC
reconnects pass. This boundary does not establish whole-ISP or whole-AWB parity.
