# Ingenic T30 ISP Recovery

This directory contains the first rebuildable T30 TX-ISP recovery baseline for
the Jooan F2T T30X/SC4236 Linux 3.10.14 target. It was reconstructed from the
OEM `tx-isp-t30.ko` with SHA-256
`129193bea06400df5511b8d8cec527b2857be88f03a3c8e5e9efc3bcf11b96b4`.
The Binary Ninja whole-binary export used by the recovery has SHA-256
`887f3dea6843144369dbb57375903b531a50a42a2c9dc8ad166dd0cec2521051`.

The automated run emitted all 846 candidates. Of those, 836 compiled in
isolation and 44 were normalized matches. The generated whole-driver seed did
not initially compile. Manual type, storage, linkage, dispatcher, and API
repairs now produce the canonical `tx-isp-t30.ko` module without compiler or
modpost errors.

## Build

Build against a compatible Thingino T30 Linux 3.10.14 output:

```sh
ROOT=/path/to/thingino/output/jooan_f2t_t30x_sc4236_eth-3.10.14-uclibc
KDIR="$ROOT/build/linux-<revision>"
CROSS="$ROOT/per-package/ingenic-sdk/host/bin/mipsel-linux-"

make -C "$KDIR" M="$(pwd)/driver/t30" \
  ARCH=mips CROSS_COMPILE="$CROSS" modules
```

Expected artifact:

- `driver/t30/tx-isp-t30.ko`

## Recovery audit

`audit/reconstruction_audit.*` records the automated run before manual repair.
It identified 10 stub candidates and 11 collapsed candidates in isolation.
`audit/binary_audit.*` is the authoritative post-repair audit of the recovery
baseline before the shared-library refactor. At that checkpoint, using the
repository's standard thresholds, it reports:

- 847 OEM and 858 recovered function symbols
- 824 direct or explicitly mapped matches
- 0 stubs
- 0 collapsed functions
- 23 OEM-only and 34 recovered-only symbols

`audit/replacements.json` documents the one-to-many setter comparison. The
recovered build keeps the gamma, system-table, and AE-weight handlers as called
functions while the OEM compiler folds their instructions into
`apical_isp_core_ops_s_ctrl`. Instruction-count parity is only a structural
triage signal; it is not a claim of semantic or byte-for-byte equivalence.

The repair pass also:

- restored the NCU and ISP-core pad-event jump-table routes
- restored all reachable IRQ cases in `ispcore_interrupt_service_routine`
- reconstructed the collapsed firmware command processor
- corrected the CMOS integration-time guard and HDR-table addressing
- replaced the decompiler's `.text` address-building inline assembly with
  direct source-backed sensor-event dispatch
- removed the two unexpected modpost dependencies introduced by model output

## Shared-library refactor

The T30 module now links thin adapters for behaviorally common subsystems:

- `tx_isp_t30_sinfo.c` supplies the T30 sensor-object offsets to the checked
  shared registry and owns the six exports consumed by matching sensor modules.
- `tx_isp_t30_subdev.c` supplies the legacy pad layout and graph-table offset
  to the shared endpoint and remote-event resolvers.
- `tx_isp_t30_math.c` preserves the SDK's public/private Apical symbol names
  while delegating log2, exp2, roots, fixed-point arithmetic, and modulation
  to host-tested common helpers.
- `tx_isp_t30_frame.c` supplies the T30 queue/state offsets to the shared
  68-byte frame-buffer ABI and state-flag policy.
- `tx_isp_t30_exports.c` owns the reviewed OEM sensor-module export surface
  instead of scattering export policy through the recovered source.

The recovered registry candidates remain under a build-time guard for audit
provenance, but they are no longer linked. This removes their malformed lock,
procfs, count, and slot logic, reduces linked BSS by roughly 49 KiB, and makes
registry allocation failure unwind the platform driver and device. The remote
event entry point now preserves and forwards its pad, event, and data arguments
instead of calling the recovered one-argument stub. Link teardown now uses the
shared endpoint detach primitive, preserves the link-state word, updates the
one-byte pad states, and returns the OEM success value.

The math extraction uses the GPL T30 SDK source where it is available and the
matching OEM object for the remaining Apical routines. It fixes the recovered
`sqrt32`/`sqrt16` candidate tests and the zero-returning `line_offset`, while
mapping identical log2, exp2, multiply, and divide algorithms onto the generic
math library. Pair/scaled/equidistant modulation is now one common pure
implementation. The original recovered bodies remain behind a build guard for
audit provenance but are not linked.

The frame adapter restores the SDK's ERROR-to-DONE fallthrough. The recovered
body lost the ERROR bit carried in a MIPS branch delay slot, so error buffers
were reported only as done. Raw T30 frame-event values now use the common
get/set/stream/queue/completion names; T30-only free-buffer and set-banks
events remain explicitly generation-qualified.

The resulting module exports the same 27-entry ABI as the OEM T30 module. The
matching SC4236 module has no unresolved `private_*`, `tx_isp_*`, log2, or exp2
dependency after comparison with that export table. The initial recovery
artifact exported only the four sensor-registry entry points.

The recovered state layout no longer allocates address-named 16 KiB objects.
The OEM symbol map proves that the `g_abs_50c*` references are fields inside
the single 12,240-byte `__fw` object and that the `data_4cc*` references are
bytes inside the 60-byte `stab` object. Mapping those references back to their
real backing objects reduces linked BSS from 362,608 bytes to 34,720 bytes.
That is 44 bytes above the OEM module's combined 34,676-byte `.bss + .sbss`
footprint. `check_storage.sh` rejects new synthetic 16 KiB objects,
address-named static storage, and inline address-building assembly.

The Apical MMIO functions now live in `tx_isp_t30_io.c` and follow the SDK's
`system_io.c` raw-access implementation. The shared fixed-region allocator
replaces the recovered probe-time null dereference with the SDK's twenty-node,
4 KiB-aligned first-fit policy. Its pure core is host-tested for validation,
alignment, splitting, exhaustion, descriptor reuse, invalid frees, and
adjacent-block coalescing; the T30 adapter owns the kernel mutex and reserved
memory lookup.

## Validation boundary

This is recovery-grade code. It has a clean static build against the matching
vendor kernel and toolchain, but it has not been loaded on a T30 camera. Probe,
streaming, sensor ABI, interrupt behavior, image quality, and OEM behavioral
parity remain unvalidated until T30 hardware is available. The deliberately
unoptimized recovery build still has substantial text expansion, so future
refactoring should continue replacing raw-layout accesses with reviewed types
and coherent SDK-backed subsystems.
