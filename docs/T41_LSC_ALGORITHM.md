# T41 lens-shading algorithm recovery

This is an offline checkpoint, not a deployed LSC replacement. The existing
kernel path still needs its captured CT/gain seed removed and its lifecycle
and live CT/gain fanout replaced. Do not claim image-quality parity from these
arithmetic tests.

`tx_isp_t41_lsc.h` implements temperature selection/history, gain selection
and register packing for the T41 calibration format. It supports both mesh
and radial-ring layouts, including the optional IR ring. Inputs are sensor
calibration, dimensions/layout and live temperature/gain; no captured live
curves or sensor identity participate.

Four calibrated temperature breakpoints define three constant-table zones
and two blending zones. Blending aligns each plane's binary exponent, then
uses Q12 linear interpolation with OEM unsigned wrap and halfword narrowing.
The temperature update threshold is inclusive; the gain threshold is strict.
Constant-table zones suppress redundant temperature updates.

The format has two distinct exponent groups: mesh planes use bytes
`0x5f..0x67`; ring planes use `0x53..0x5e`. The existing kernel mesh path
incorrectly uses ring exponents. Other exposed differences are the 23-bit
mask on register `0x3018`, the lack of a gain-register write for CT-only
updates, and complete hardware-bank packing even when the interpolation
count is smaller. Unused scratch must retain history in that last case.

Mesh interpolation consumes up to 2,304 samples in each of three planes.
Rings consume 577 samples per RGB plane and optionally IR. Each ring hardware
entry combines adjacent samples, so 576 entries are produced; optional IR
entries pair IR with green. Register words pack six 12-bit values into 72
bits. Calibration occupies `0xd880` bytes, state/history `0x6c38` bytes.

Reproduce against H20250310a, SHA256
`572ff4553c1a033290ec67d2d9fc384701fbc235c2e335c7e5604100966fb2ee`:

```sh
make -C tests check
sh tools/build_t41_lsc_oracle.sh CROSS_PREFIX STOCK_OBJECT OUTPUT_DIR
qemu-mipsel OUTPUT_DIR/lsc-oracle-check
```

QEMU and the physical T41 each pass 10,000 synthetic cases spanning all five
CT zones, threshold boundaries, history sequences, both layouts, IR on/off,
both gain-table banks and all four register-writer modes: zero mismatches
and unexpected writes. The independent host test passes another 10,000 cases
with ASan/UBSan, short-buffer rejection and canaries. The oracle has private
state and a register recorder; it does not access ISP hardware or import a
sensor bin. OEM instructions are confined to this test executable and are
never linked into the driver.

Still required: geometry validation, cold/mode/flip lifecycle proof, native
kernel ownership/fanout, and stock/open device comparisons. Calibration tables
remain legitimate sensor inputs; a measured scene's CT/gain is not a default.
