# T21 / T23 / T31 Comparative Analysis

## Reference set

This pass used the following stock modules:

| SoC | SHA-256 |
|---|---|
| T21 | `ee1090c05c02c6c87da56074255ef73a5cd011f4ec928ef7886c0f7cfaf53456` |
| T23 | `0310eac7e014dba9747d52d9f960bd67e15669845400bcd37eaeea1a7585d76c` |
| T31 | `b858e6ee1c83d228330f053b932041ad3cdb90efd3dbe4869572bd5445669ddf` |

Function signatures were computed from MIPS instruction words with relocated
immediates normalized to relocation type and target. Non-relocated words,
branch topology, register allocation, and relocation order remain part of the
signature. This is deliberately stricter than comparing names or instruction
counts.

## Whole-module overlap

| Comparison | Common function names | Exact normalized signatures |
|---|---:|---:|
| T21 vs T23 | 527 | 72 |
| T21 vs T31 | 561 | 206 |

T31 is the materially closer implementation family. The exact T21/T31 set
includes the complete fixed-point add/subtract/multiply/divide family,
private and public log2/exp2 paths, signed interpolation, AWB interpolation,
`func_zone_ct_weight`, IRQ helpers, several sensor/platform wrappers, and a
large group of tuning refresh functions.

T23's exact set is dominated by kernel/platform wrappers and a smaller group
of simple controls. Its math bodies are not instruction-identical to T21 and
use generation-specific policy already represented by the T23 adapter.

## Decisions from this pass

### Extracted now

- 18 general math ABI entry points now delegate through
  `tx_isp_t21_math.c` to `driver/include/tx_isp/tx_isp_math.h`.
- T21 keeps its signed table policy and eight-step endpoint in the adapter.
- The recovered unsuffixed and `_64` multiply/divide bodies remain local.
  Their o32 prototypes and generated call sites need typing before changing a
  code-generation-sensitive boundary.

### Repaired locally

- `tx_isp_enable_irq` and `tx_isp_disable_irq` are identical in the T21, T23,
  and T31 stock modules. The T21 recovery had conflated their local IRQ-line
  names with the global lock-protected callbacks. The symbol ownership and
  callback offsets are now restored without introducing a shared object-layout
  abstraction.
- `private_get_isp_priv_mem` again dispatches through the common-interface
  table slot used by the stock T21 module.

### Deliberately not shared

- `Tiziano_Awb_Ct_Detect`: stock sizes are 1,439 instructions on T21, 1,845 on
  T23, and 2,156 on T31. All three normalized signatures differ. Recover from
  the T21 HLIL and use T31 only as a control-flow/data-structure guide.
- Large ADR, AE, defog, ioctl, and core-init findings have different stock
  sizes or signatures. Register layout and sequencing stay generation-local
  until field-level equivalence is proven. The T21 core-init path was therefore
  repaired from its own stock disassembly rather than copied from T31.
- Parameter-array handlers often share a protocol shape but have different
  sizes because block tables and payload layouts differ. A future common
  bounds/copy shell should take explicit per-block descriptors rather than
  reuse a sibling body.

## Next evidence-driven work

1. Repair the startup-facing `isp_vic_cmd_set` and `tx_isp_unlocked_ioctl`
   dispatch paths from the T21 stock control flow, then run a module/probe-only
   camera smoke test.
2. Reconstruct `Tiziano_Awb_Ct_Detect` from the T21 HLIL, keeping its 19-argument
   ABI and T21 global work arrays; use T31 only to name the phases before the
   first streaming test.
3. Type the o32 64-bit fixed-point call sites, then evaluate moving the exact
   T21/T31 unsuffixed and `_64` arithmetic to a native-ABI adapter.
4. Split parameter-array access into a common validated copy engine plus
   generation-local descriptor tables.
5. Repair the large ADR/AE/defog paths in descending audit severity and rerun
   the binary audit after every coherent subsystem.
6. Keep initial hardware work non-persistent: module load, probe, diagnostics,
   and reboot first. Gate the firmware stream on successful sensor-parameter
   loading and repair of the remaining true AWB stub.
