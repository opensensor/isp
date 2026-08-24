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
`audit/binary_audit.*` is the authoritative post-repair audit of the final
linked module. Using the repository's standard thresholds, it reports:

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

## Validation boundary

This is recovery-grade code. It has a clean static build against the matching
vendor kernel and toolchain, but it has not been loaded on a T30 camera. Probe,
streaming, sensor ABI, interrupt behavior, image quality, and OEM behavioral
parity remain unvalidated until T30 hardware is available. The binary audit
also shows substantial code and BSS expansion from the deliberately
unoptimized recovery build, so future refactoring should reduce recovered
state and replace raw-layout accesses with reviewed types.
