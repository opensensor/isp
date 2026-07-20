# Ingenic T41 ISP Recovery

This directory contains the recovered T41 TX-ISP driver baseline for the Wyze
Cam v4 T41NQ/OS04D10 Linux 4.4.94 target. It was generated from the OEM
`tx-isp-t41.ko` whose SHA-256 is
`2426eea9c3c8268373eccc3c3753424f0429ff02044e4ddd4e86c6caa40a6338`.

The source was salvaged from the June 2026 `tx-isp-t41-v1` reconstruction
workspace. All 1,510 function candidates were emitted and 1,508 compiled in
isolation, but only 157 were normalized exact matches. A later manual cleanup
made the whole driver build; it was never promoted into the open driver tree or
validated on hardware.

## Build

Build against the matching Thingino Wyze Cam v4 output:

```sh
ROOT=/home/matteius/thingino-firmware-opensensor/output/master/wyze_cam4_t41nq_os04d10_atbm6062s-4.4.94-uclibc \
KDIR="$ROOT/build/linux-2aca1252ac4a304172b870777365f42bfb100674" \
SOC=t41 ./build_local.sh
```

Expected artifact:

- `driver/t41/tx_isp_t41_recovered.ko`

## Baseline risk

The recovered module is a bring-up artifact, not a production-ready driver.
The current linked-binary audit finds 18 stub functions, 92 collapsed
functions, 385 shorter functions, and 41 OEM-only symbols. Critical deficits
include subdevice initialization, core control/ioctl dispatch, and tuning
paths.

Completed static repairs restore all 31 OEM exports and replace the recovered
IRQ wrappers that previously requested IRQ 0 with null handlers and disabled
IRQ 0 regardless of their arguments. The linked audit now classifies the IRQ
enable/disable helpers as shorter rather than stubs, and the request/free paths
as similar in instruction count. Hardware validation is still required.

The OEM-derived leading-bit helpers now return their computed positions rather
than zero, and `private_copy_from_user` once again uses the kernel's checked
copy path. This restores a prerequisite for fixed-point tuning math and for the
many ioctl paths that consume userspace structures.

The complete T41 fixed-point log helper family was restored from the local
Ingenic Linux 4.4.94 T41 SDK source. Four helpers now have exact OEM
instruction-count parity and the 64-bit log conversion is within one
instruction of OEM.

The corresponding internal TISP fixed-point log family now returns its
calculated values instead of zero. The 32-bit integer helper is within one
instruction of OEM and both exported conversion wrappers are classified as
similar by the linked audit; the 64-bit integer helper is semantically restored
but remains shorter than OEM and needs runtime coverage during tuning bring-up.

Twelve more vendor shim functions covering I2C, GPIO input, module references,
completion handling, and interruptible timeout waits were restored from that
same SDK source. Each now has OEM instruction-count parity.

The CPM reset helper now polls the OEM `0xb00000c4` register for all 500
iterations and performs both completion writes at that same address. Recovered
pointer types had shortened the poll to 125 iterations and redirected the
completion pulse to `0xb0000310`; the repaired helper is within three
instructions of OEM.

The six fixed-point add/subtract entry points now implement the OEM 32-bit and
64-bit arithmetic instead of returning zero or one input operand. All three
add helpers have exact instruction-count parity; the subtract helpers preserve
OEM unsigned wraparound and underflow diagnostics and are classified as
similar.

The 64-bit rounding, signed minimum/maximum, and three-operand 32-bit multiply
helpers now preserve their full return values. The multiplier and min/max
helpers have exact OEM instruction-count parity. A decompiler-only pair of
`muls_dp_*` calls in AWB distance calculation was also replaced with explicit
64-bit squares, eliminating two symbols that the target kernel cannot resolve.

The ISP-core pad event handler again accepts its data argument and dispatches
events `0x03000001` through `0x03000008` to the seven OEM callback slots (with
the OEM no-op sixth event). Missing callbacks return `-1`; disabled pads and
unknown events remain no-ops.

The media-bus Bayer writer now preserves the upper bits of ISP register
`0x88`, clears its low five bits, and applies the OEM `{1,3,2,0}` Bayer-order
mapping across codes `0x5200` through `0x5213`. Unsupported formats retain the
OEM diagnostic/no-op behavior.

The exports-only (`-1`) hardware smoke level inserts and removes cleanly on the
Wyze Cam v4 target. The first shallow-platform (`0`) run inserted successfully,
created `/dev/tx-isp`, and reached the recovered probe, but exposed word-scaled
decompiler offsets in `tx_isp_remove` during unload. The parent teardown now
uses the OEM byte offsets `0x0c` and `0x88`, preserves the proc pointer at
`0x138`, and guards an absent parent device before deregistration.

A second level-0 run passed that parent teardown but exposed heap corruption
when the kernel later freed module metadata. The recovered child-platform loop
had advanced 32 bytes instead of the OEM 8 bytes and called a math helper in
place of each child driver's remove callback. The repaired loop visits all 16
entries and invokes the callback at driver offset `0x04` before unregistering
each platform device. This repair requires a fresh level-0 hardware retest
before advancing to the device graph.

The next level-0 run reached the restored core remove callback and showed that
it unconditionally deinitialized TISP even though levels below 3 deliberately
skip TISP initialization. Core removal now mirrors that bring-up gate, while
still deinitializing the registered subdevice, and passes the OEM channel-buffer
and core pointers to the two recovered no-argument frees. This repair also
requires a fresh level-0 hardware retest.

The fourth level-0 run completed core teardown and reached CSI removal. It
exposed another scaled pointer (`0x40` instead of the OEM byte offset `0x10`)
and missing iounmap, resource-release, and free arguments. The CSI path and the
same statically visible VIN, IVDC, and frame-source teardown defects are now
restored from OEM disassembly. The VIC path was normalized at the same time.

The fifth level-0 run removed every driver object but detected a kernel bug
while the module's vmalloc area was released. The common subdevice destructor
had discarded its misc, heap, ioremap, and memory-region arguments, and its
clock-release helper became an infinite loop whenever clocks were present.
Those paths and the collapsed module deinitializer are restored from OEM
control flow. The smoke harness now also treats `Kernel bug detected` as a
fatal signature.

The sixth level-0 run is clean: module insertion, `/dev/tx-isp` creation,
parent/child removal, and module unload all return zero with no kernel fatal
signature. This clears the shallow-platform gate for device-graph testing.

Hardware smoke tests must stage the module under `/tmp`, unload conflicting
stock ISP modules first, capture kernel and userspace logs, and reboot after
each experiment. Do not install this baseline into the persistent module tree.

Use the staged smoke harness after boot-time stock loading has been disabled:

```sh
T41_BRINGUP_LEVEL=-1 tools/t41_smoke_cycle.sh
```

The levels are intentionally incremental: `-1` exports only, `0` performs a
shallow platform probe, `1` adds the device graph, `2` adds ISP memory setup,
and `3` enables the recovered core/tuning path. The harness refuses to run if
any `tx_isp*` module is already loaded and reboots the camera after every
experiment.
