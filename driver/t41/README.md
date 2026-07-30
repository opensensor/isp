# Ingenic T41 ISP Recovery

This directory contains the recovered T41 TX-ISP driver baseline for the Wyze
Cam v4 T41NQ/OS04D10 Linux 4.4.94 target. It was generated from the OEM
`tx-isp-t41.ko` whose SHA-256 is
`2426eea9c3c8268373eccc3c3753424f0429ff02044e4ddd4e86c6caa40a6338`.

The source was salvaged from the June 2026 `tx-isp-t41-v1` reconstruction
workspace. All 1,510 function candidates were emitted and 1,508 compiled in
isolation, but only 157 were normalized exact matches. A later manual cleanup
made the whole driver build. It now has staged hardware coverage through the
OS04D10 sensor and Raptor consumer, but it remains recovery-grade code rather
than a production-equivalent replacement.

## Build

Build against the matching Thingino Wyze Cam v4 output:

```sh
ROOT=/home/matteius/thingino-firmware-opensensor/output/master/wyze_cam4_t41nq_os04d10_atbm6062s-4.4.94-uclibc \
KDIR="$ROOT/build/linux-2aca1252ac4a304172b870777365f42bfb100674" \
SOC=t41 ./build_local.sh
```

Expected artifact:

- `driver/t41/tx-isp-t41.ko`

The source remains named `tx_isp_t41_recovered.c`, but Kbuild emits the
canonical module name expected by current T41 sensor modules
(`depends: tx-isp-t41`).

The module is linked from four logical objects:

- `tx_isp_t41_recovered.c` — recovered core, pipeline, hardware, and tuning
- `tx_isp_t41_daynight.c` — T41 adapter for the shared day/night state machine
- `tx_isp_t41_math.c` — T41 ABI wrappers around shared math primitives
- `tx_isp_t41_sinfo.c` — T41 layout adapter around the shared sensor registry

## Baseline risk

The recovered module is a bring-up artifact, not a production-ready driver.
The current linked-binary audit finds 18 stub functions, 91 collapsed
functions, 386 shorter functions, and 41 OEM-only symbols. Critical deficits
remain in core control/ioctl dispatch and tuning paths.

Completed static repairs restore all 31 OEM exports and replace the recovered
IRQ wrappers that previously requested IRQ 0 with null handlers and disabled
IRQ 0 regardless of their arguments. The linked audit now classifies the IRQ
enable/disable helpers as shorter rather than stubs, and the request/free paths
as similar in instruction count.

The T41 module now reuses the common day/night state machine,
interpolation/fixed-point helpers, and the T23/T31/T41 typed sensor-registry
implementation. Full sensor/Raptor smoke tests pass, both MSCA streams run,
and ISP interrupts advance. The temporary static day AWB baseline is
`R=0x380, B=0x880`; both shadow banks retain those values across forced
day/night transitions. This replaces the earlier red/blue-heavy defaults that
produced a magenta day image. One registry limitation remains: the
recovered T41 runtime currently leaves the staged shared registry at
`/proc/jz/sensor/count=0`, while the persistent installed driver reports one
sensor, so metadata parity is not yet claimed.

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

The first level-1 run then showed that the graph was incomplete: IVDC probe
rejected zero input/output pad counts and graph creation reported that subdev
index 4 was missing. The common subdevice initializer was only 65 instructions
against the OEM's 420 and skipped IRQ, MMIO, clock, and pad setup. Its restored
implementation is now 306 instructions, the clock initializer is 132 against
OEM 136, and the module initializer is 48 against OEM 51. This lowers the
linked audit's collapsed count from 92 to 91.

Activating the recovered pad setup exposed two additional dormant teardown
chains at level 0. Frame-source probe used the address of its pad-pointer field
as the pad array, freed its live channel array on the success path, and used a
scaled loop index. The repaired probe is 204 instructions against OEM 206.
The VB2 queue cancel/free chain also unlocked a null spinlock and used scaled
list offsets; queue cancel now has exact OEM instruction-count parity and
queue free is within two instructions. Finally, child teardown is bounded by
the six platform entries actually registered instead of walking ten unused
slots. The resulting level-0 run at
`logs/20260720-2015-t41-level0-platform-table-dump-117` inserts and unloads
cleanly with no kernel fatal signature, clearing the updated initializer for a
fresh level-1 graph test.

The fresh level-1 run at
`logs/20260720-2019-t41-level1-restored-graph-117` inserts cleanly and no longer
reports the missing IVDC subdevice. Core, VIC, and IVDC IRQs 39, 38, and 21 are
registered, and `/dev/misc-ivdc` is now present alongside `/dev/tx-isp`.

Static review held level 2 because the recovered private-memory initializer
aliased unrelated IVDC and frame-counter globals and both mutex wrappers used a
null lock. The restored allocator uses the OEM 20-entry block pool, the
reserved ISP base/size, and a real mutex; it includes split and coalescing
paths for later core use. `isp_mem_init` is now 47 instructions against OEM
40, `find_new_buffer` is 32 against OEM 37, and `isp_free_buffer` remains 67
against OEM 70. The level-2 run at
`logs/20260720-2025-t41-level2-private-memory-117` inserts cleanly with no
kernel fatal signature, clearing private-memory setup for level-3 static
review.

Level-3 static review then found that core probe passed the clock-name table as
its subdevice operations and initialized the core spinlock and mutex at
word-scaled offsets beyond the 880-byte allocation. The corrected shallow path
uses `core_subdev_ops` and byte offsets 276 and 280. The level-0 regression at
`logs/20260720-core-offset-fix-level0-117` inserts and unloads cleanly with no
kernel fatal signature. The still-guarded level-3 channel and tuning path
requires separate reconstruction before it is enabled on hardware.

The tuning object on that guarded path also initialized its lock and mutex at
word-scaled offsets, installed two unrelated recovered constants instead of
the tuning file operations and event callback, omitted the ISP debug-node open
callback, and freed no object during deinitialization. Those fields now match
the OEM byte layout and ownership flow. `isp_core_tuning_init` is classified
similar at 53 instructions against OEM 60, while `isp_core_tuning_deinit` has
exact 18-instruction count parity. This repair is compile- and audit-verified;
hardware coverage remains coupled to the pending core-channel restoration.

Core probe now builds its 232-byte channel records and embedded normal/IR
callback tables with OEM byte offsets and named function relocations. It also
restores channel dimensions and capability flags, pad event ownership, the
tuning/debug node hooks, and failure cleanup. The linked probe is classified
similar at 289 instructions against OEM 321. The insertion-only level-3 run at
`logs/20260720-level3-core-channels-117` registers AISP, creates `/dev/isp-m0`,
keeps core/VIC/IVDC IRQs 39/38/21 active, and reports no kernel warning or fatal
signature. Live unload and a sensor/Raptor consumer remain gated on repair of
the shorter recovered TISP deinitializer and review of the stream-init path.

Stream-init review found that several OEM firmware objects had been recovered
as single words even though the code indexes or clears them as structures and
per-channel arrays. The active storage now has the OEM symbol-table extents,
including 504-byte TISP parameters, 2,656-byte scaler state, 78-byte scaler
channel state, 7,744-byte event state, two-entry parameter/day/night tables,
and the corresponding callback, histogram, sensor-control, lock, completion,
and scratch objects. This prevents `tisp_init` from overwriting neighboring
globals before the stream path is enabled. The level-3 regression at
`logs/20260720-level3-tisp-storage-117` again registers cleanly with no warning
or fatal signature.

Hardware smoke tests must use a one-shot boot stage, capture kernel and
userspace logs, and reboot after each experiment. Do not install this baseline
into the persistent module tree or hot-unload an active camera pipeline.

`tools/open_tx_isp_boot_once_init.sh` is the fail-safe init hook used for full
consumer tests. It removes and syncs its armed marker before inserting the
staged module, so a watchdog or power-cycle loads the untouched persistent
module on the next boot.

Use the staged smoke harness after boot-time stock loading has been disabled:

```sh
T41_BRINGUP_LEVEL=-1 tools/t41_smoke_cycle.sh
```

The levels are intentionally incremental: `-1` exports only, `0` performs a
shallow platform probe, `1` adds the device graph, `2` adds ISP memory setup,
and `3` enables the recovered core/tuning path. The legacy harness refuses to
run if any `tx_isp*` module is already loaded and reboots the camera after
every experiment; use the one-shot init hook when the stock boot sequence
cannot safely unload its active module.
