# Ingenic T21 ISP Recovery

This directory is the upstream integration point for the first rebuildable
T21 TX-ISP recovery. It was reconstructed from the OEM `tx-isp-t21.ko` with
SHA-256 `ee1090c05c02c6c87da56074255ef73a5cd011f4ec928ef7886c0f7cfaf53456`.
The imported whole-driver source and its audit history now live here; further
T21 work should be made in this directory rather than in the reconstruction
workspace.

## Build

Build against a compatible Thingino T21 Linux 3.10.14 output:

```sh
ROOT=/path/to/thingino/output/t21-camera-3.10.14-uclibc
KDIR="$ROOT/build/linux-<revision>"
CROSS="$ROOT/per-package/ingenic-sdk/host/bin/mipsel-linux-"

make -C "$KDIR" M="$(pwd)/driver/t21" \
  ARCH=mips CROSS_COMPILE="$CROSS" modules
```

Expected artifact: `driver/t21/tx-isp-t21.ko`.

The current module compiles and completes MODPOST against the vendor kernel.
It still emits many recovery-grade type and prototype warnings; those are a
repair queue, not harmless noise. No T21 camera load or stream claim is made
at this checkpoint.

## Shared math boundary

`tx_isp_t21_math.c` supplies 18 T21-facing entry points backed by the common,
host-tested math library. The boundary includes private/public log2 and exp2,
signed eight-step interpolation, and the 32-bit add/subtract, wrapped
multiply, three-way multiply, and OEM-compatible divider functions.

This extraction is backed by the stock binaries rather than symbol-name
similarity: 30 of 31 selected T21 math/AWB helpers have relocation-normalized
instruction signatures identical to T31. The one exception is
`fix_point_intp`, which remains local. T23 has a distinct code-generation and
arithmetic profile, so its existing adapter policy remains separate.

The recovered bodies stay behind `TX_ISP_T21_SHARED_MATH` for provenance, but
are not linked in the normal module. This also replaces the recovered
`fix_point_add_32` and `fix_point_mult3_32` fragment bodies, both of which lost
their computed return values or call state.

## Repairs made during integration

- Restored the OEM local/global split between the tiny IRQ-line callbacks and
  the lock-protected public `tx_isp_enable_irq`/`tx_isp_disable_irq` pair.
  The public functions now invoke the callback slots at `0x84`/`0x88` under
  the lock at `0x80`, matching T21 and T31 disassembly. Both functions moved
  out of the collapsed audit class.
- Restored the driver-interface table in `pfaces` and routed
  `private_get_isp_priv_mem` through its OEM slot at `0x164`. The integrated
  module therefore has no reconstruction-created MODPOST dependency on a
  nonexistent standalone `get_isp_priv_mem` symbol.
- Restored the stock 30-symbol module export surface in a separate adapter.
  The recovered module's `__ksymtab` and `__ksymtab_strings` sizes now match
  the OEM module exactly.
- Rebuilt `tisp_init` around its real global parameter buffers and 80-byte
  runtime state instead of the decompiler-created 255 KiB stack frame. The
  repaired path restores the sensor callback, Bayer/top-register selection,
  AE initialization, working-buffer bookkeeping, and the OEM custom-effect
  copies without writing beyond `tispinfo`.
- Rebuilt `ispcore_core_ops_init` from the T21 stock disassembly. Its exact
  76-byte TISP sensor record, 10-entry output-format table, media-bus/Bayer
  conversion, channel layout, state transitions, and firmware-thread failure
  handling are now explicit C. The audit moved it from `collapsed` at 0.311
  of OEM instructions to `shorter` at 0.671.
- Corrected the recovered `JZ_Isp_Ae_Reg2par` word/byte indexing that could
  otherwise overwrite its caller's stack, plus the brightness/contrast custom
  effect byte updates and the stream-state scalar type.
- Restored ioctl `0x800456d0`, the OEM video-link graph setup command. The
  driver now walks the three stock 20-byte link descriptors, resolves each
  source/sink pad, validates link capability and active state, replaces stale
  links, and writes the reciprocal pad linkage before recording config zero.
  The matching destroy path now dereferences the link-set header correctly.
- Corrected the top-level ioctl scratch allocation from 0x50 words to the OEM
  0x50-byte payload. Its MIPS stack frame is now 152 bytes versus 168 OEM,
  instead of 392 bytes.

## Audit and validation boundary

`audit/binary_audit.*` is the authoritative current module audit.
`audit/collapse_repair_notes.md` records the earlier standalone repair pass.
Instruction-count parity is structural triage, not proof of semantics.

The current audit has 612 directly matched functions, 4 syntactic stub
findings, 25 collapsed findings, and a 0.787 matched-instruction ratio. Three
of the four stub findings are intentional tail-call aliases; the only true
empty body is `Tiziano_Awb_Ct_Detect`.

The highest-priority remaining findings are the T21-specific AWB, ADR, AE,
defog, ioctl, and parameter-copy paths. In particular,
`Tiziano_Awb_Ct_Detect` is still a true empty body. The stock T21, T23, and
T31 versions are 1,439, 1,845, and 2,156 instructions respectively, so the
T31 implementation must not be copied wholesale. The T21 HLIL export is the
authoritative source for that repair.

The built-in parameter images currently materialize only the recovered first
16 KiB of each OEM 0x15380-byte day/night buffer. Their remaining bytes are
zero until `tiziano_load_parameters()` loads the sensor parameter file. A
probe-only smoke test is appropriate once lab access is available, but a
stream test must confirm that parameter load succeeded before enabling the
firmware loop.

See `COMPARATIVE_ANALYSIS.md` for the cross-generation measurements and the
next extraction candidates.
