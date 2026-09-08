# T41 lens-shading algorithm recovery

The native kernel adapter owns calibration and history, receives live CT/gain
from the drained frame worker and replaces the captured CT/gain seeds. Device
validation is still pending for this adapter; arithmetic tests alone do not
establish image-quality parity.

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
`0x5f..0x67`; ring planes use `0x53..0x5e`. The replaced kernel mesh path
incorrectly used ring exponents. Other corrected differences are the 23-bit
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

The expanded oracle also covers 2,000 mesh flips (including odd dimensions,
padded columns, ring mode and IR), 10,000 geometry diagnostics, forty cold
starts, and 320 linear/WDR, flip and day/night replacements. No compatible
input mismatches were found in QEMU and on the physical T41. Nineteen oversized geometry products
are rejected before the OEM's 16-bit narrowing can wrap; they are reported
separately rather than counted as parity matches. Host sanitizers cover cold
starts, flipped calibration refreshes and double-flip restoration as well.

Cold initialization uses the OEM's universal 5000 K / unity-gain state,
not this sensor's measured running state. Mirroring transforms each of nine
calibrated mesh planes and the optical-center coordinates, retaining the
padding column. Day/night refresh starts from replacement calibration and
reapplies the saved orientation. These are algorithm/lifecycle tests, not
physical sensor WDR validation.

The setter oracle adds 1,600 parameter-part transfers, including flipped
tables. OEM parts 1..3 only stage data; part 4 completes the transfer and
reapplies orientation. Controls (part 0) are a separate immediate operation.
The native adapter preserves that transaction boundary while allowing frame
updates against the last committed calibration. Bad pointers or geometry do
not replace the active map. A host harness includes the actual kernel adapter
with private memory and MMIO recording, checking ownership, allocation/restore
failure unwinding, staged transfers, CT/gain hysteresis, complete RAM commits,
PM save/restore and stop-before-free ordering. No LSC RAM writes occur in the
hard IRQ path. Repeated unchanged inputs avoid copying/interpolating state.

Still required: stock/open device comparisons. Calibration tables remain
legitimate sensor inputs; a measured scene's CT/gain is not a default.
