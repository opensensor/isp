# T10 Binary-Audit and Collapse Repairs

## Inputs

- OEM module:
  `tx-isp-t10.ko`
- OEM SHA-256:
  `39b463c62eacae924451a49cc58374ed76740448e24864b4454b5c3c4864a975`
- Binary export SHA-256:
  `c2e1cf25ba6ab05bcc8c995e4496be9539544678f93cc5080894ad2f4f165c6b`
- Recovery result:
  740 candidates, 735 isolated compiles, 57 normalized exact matches

## Source partition evidence

The T10 and T20 SDK targets resolve to the same `isp/t20` tree and the same
Apical 3.12.0 firmware archive. Their OEM modules expose the same 742
function-name set. T10 is therefore compiled from the reviewed shared sources
under `CONFIG_SOC_T10`; prebuilt T20 objects are never reused.

The five non-compiling model candidates are covered as follows:

| Model candidate | Integrated owner |
|---|---|
| `register_tx_isp_vic_device` | reviewed SDK VIC translation unit |
| `register_tx_isp_core_device` | reviewed SDK core translation unit |
| `tx_isp_release_subdevs` | reviewed SDK device translation unit |
| `tx_isp_probe` | reviewed SDK device translation unit |
| `apical_isp_stab_s_attr.isra.0` | recovered unsuffixed tuning implementation |

## Repairs found by the integrated audit

The initial shared-source T10 build linked successfully but reported fourteen
collapses: `apical_api_init_idx_array` and thirteen
`*_request_interrupt` entry points.

- `apical_api_init_idx_array` now emits every entry of the OEM 140-element
  map and returns the table address.
- The shared FSM request helper is always-inlined so every OEM entry point
  contains the required interrupt-disable, pending-mask update, and conditional
  re-enable lifecycle.
- Reconciling optimized OEM names exposed a fifteenth collapse,
  `apical_program_interrupt_event.part.0`. Its explicit 16-way mapping to
  registers `0x80` through `0x9c` is now retained.

The replacement map also records compiler-created `.isra`, `.part`, and
`.constprop` names, plus SDK helpers that the OEM compiler inlined. The
`tx_isp_init` replacement records that the OEM aliases it to
`init_module`, whereas the open source keeps platform registration as a
separate rollback-capable helper.

## Final structural result

- OEM functions: 742
- recovered functions: 894
- direct matches: 708
- documented replacement matches: 34
- missing replacement groups: 0
- OEM-only functions: 0
- literal stubs: 0
- collapsed findings: 0
- shorter findings: 0

This is not a hardware pass. Runtime validation remains gated on a
non-persistent T10 device smoke cycle.
