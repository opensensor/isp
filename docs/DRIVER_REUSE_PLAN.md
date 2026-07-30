# Cross-SoC Driver Reuse Plan

## Scope

The active refactor covers the working T23, T31, and T41 TX-ISP drivers. T31
already has core, CSI, VIC, VIN, frame-source, tuning, and support translation
units. T23 and T41 still have large recovered core translation units, but both
modules now link separate math and sensor-registry adapter objects. This gives
later extractions stable module boundaries without rewriting the recovered
pipeline all at once.

T40 remains a working recovered baseline, but it is intentionally outside this
round of cleanup.

The goal is one implementation where behavior is genuinely common, with small
per-SoC adapters for recovered object layouts, register maps, capabilities,
tables, and sequencing. Matching symbol names are only candidates for reuse;
OEM evidence and working-device behavior remain authoritative.

## Shared Code Landed

### Arithmetic primitives

`driver/include/tx_isp/tx_isp_math.h` contains kernel-independent primitives
for:

- signed and unsigned Q16 interpolation
- typed 8-bit and 16-bit table interpolation
- two- and three-operand unsigned fixed-point multiplication

The per-SoC wrappers preserve their existing ABI and endpoint policy:

- T23 uses signed interpolation with its eight-step table endpoint.
- T31 delegates its fixed-point multiply wrappers to the common helpers.
- T41 uses the unsigned 8/16/32-bit interpolation variants with its ten-step
  endpoint and delegates its fixed-point multiply wrappers.

`tests/tx_isp_math_test.c` covers boundary behavior, OEM rounding, wrapped
32-bit products, typed tables, and randomized equivalence checks.

### Sensor registry

`driver/common/tx_isp_sinfo.c` owns the sensor-registry lifecycle, exported
sensor-driver/bind functions, slot ownership, and `/proc/jz/sensor` files.
`driver/include/tx_isp/tx_isp_sinfo.h` owns the shared interface.

The recovered sensor objects do not share a C layout, so each consumer supplies
one typed `struct tx_isp_sinfo_config`:

- T23 supplies fixed SC2336 metadata plus lifecycle callbacks that retain its
  recovered sensor-client creation and cached-state side effects.
- T31 supplies the recovered client, attribute, dimensions, fps, and adapter
  byte offsets.
- T41 supplies its larger recovered layout and enables the extended sensor
  attributes.

This is source sharing rather than a second runtime module: each TX-ISP module
continues to own and export the ABI expected by its matching sensor module.

T23 and T31 have full live coverage with their SC2336 sensors, including name,
chip ID, I2C address, 1920x1080 dimensions, 25 fps, active state, and advancing
ISP interrupts.

T41 has live streaming coverage with the OS04D10 sensor, but the recovered
runtime currently leaves the staged shared registry at count zero. Its
persistent installed driver reports one sensor, so this is a concrete parity
gap rather than an unsupported sensor. The sensor and Raptor pipeline load and
stream normally, but shared-registry metadata parity is not yet claimed. Do
not hide this with synthetic sensor entries; recover the missing T41
bind/lifecycle behavior first.

### Checked ABI and recovery support

`driver/include/tx_isp/tx_isp_subdev_abi.h` records reviewed physical pad and
generation-specific subdevice offsets. T31 compile-time assertions cover the
confirmed named fields and common pad prefix.

`driver/include/tx_isp/tx_isp_recovered_kernel.h` holds the already-reviewed
kernel-tree compatibility prelude used by recovered sources. Keep larger
freestanding recovery-tool declarations local until their signatures are
verified.

### Canonical T41 artifact

T41 Kbuild now emits `driver/t41/tx-isp-t41.ko`, the canonical dependency name
used by current T41 sensor modules, while retaining
`tx_isp_t41_recovered.c` as the source filename.

T41 is now a multi-object module with three explicit boundaries:

- `tx_isp_t41_recovered.c` owns the recovered pipeline, hardware, and tuning
  implementation.
- `tx_isp_t41_math.c` preserves the recovered math entry-point ABI and
  delegates its algorithms to `tx_isp_math.h`.
- `tx_isp_t41_sinfo.c` supplies the T41 object-layout adapter for the common
  sensor registry.

### Multi-object T23 artifact

T23 preserves the deployed `tx_isp_t23_recovered` module identity while linking
three logical objects:

- `tx_isp_t23_core.c` owns the recovered pipeline, hardware, tuning, and the
  T23-specific sensor lifecycle callbacks.
- `tx_isp_t23_math.c` preserves the recovered interpolation entry-point ABI
  and delegates its algorithm to `tx_isp_math.h`.
- `tx_isp_t23_sinfo.c` supplies static metadata and lifecycle callbacks for the
  common sensor registry.

## Device Validation

Every staged module was loaded for one boot only, exercised through the real
Raptor consumer, checked for advancing interrupts and kernel fatal signatures,
then replaced by the untouched persistent module on reboot.

| SoC | Staged coverage | Result |
|---|---|---|
| T23 | three-object module, shared interpolation and typed registry, SC2336, main/sub rings | pass; registry count 1 |
| T31 | shared fixed-point math and typed registry, SC2336, main/sub rings | pass; registry count 1 |
| T41 | three-object module, shared math and typed registry, OS04D10, main/sub rings | streaming pass; staged registry count remains zero |

The one-shot loader in `tools/open_tx_isp_boot_once_init.sh` consumes and syncs
its marker before `insmod`. A crash therefore cannot repeatedly load the staged
module: the next watchdog or power-cycle returns to the persistent driver.
Live unloading is not a safe test strategy for these camera pipelines.

## Target Layout

```text
driver/
├── include/tx_isp/       # SoC-independent reviewed interfaces/primitives
├── common/               # Shared kernel implementation
├── t23/                  # T23 registers, resources, tables, quirks
├── t31/                  # Split T31 implementation and adapters
├── t40/                  # Deferred recovered baseline
└── t41/                  # T41 registers, resources, tables, quirks
```

A common helper should accept explicit generation data or a small typed
callback surface. It should not discover the SoC at runtime or grow a web of
`#ifdef Txx` branches.

## Keep Per-SoC Until Proven Otherwise

- physical addresses and register offsets
- IRQ masks, acknowledge order, and error recovery
- clock/reset names and enable ordering
- packed or offset-addressed recovered structures
- stream-start and sensor handoff quirks
- tuning tables and calibration payloads
- block presence, bank count, and interpolation endpoint count
- code still represented by unresolved recovery fragments

T31's file boundaries are a useful decomposition guide, not automatically the
canonical behavior. T23 and T41 may expose older or newer semantics that need
to shape the eventual common interface.

## Safe Extraction Rules

1. Compare parameter types, call sites, return values, side effects, and object
   layout across every intended consumer.
2. Preserve the existing exported or per-driver entry point as a thin wrapper
   during the first move.
3. Express generation differences as explicit data, offsets, or callbacks.
4. Add host tests for pure code and build all three active modules for every
   shared change.
5. Run a full consumer smoke test on each affected SoC before declaring a
   kernel-facing extraction complete.
6. Restore and verify the persistent module after each staged experiment.
7. Do not mix a structural extraction with tuning-value or register-sequence
   changes.

## Next Candidates

1. Recover the missing T41 sensor-registry bind/lifecycle path and require full
   `/proc/jz/sensor` parity before extending that interface.
2. Review fixed-point divide and log/exp helpers shared by T31 and T41, starting
   with host-testable functions that cannot touch kernel or ISP state.
3. Reconcile the known T31 pad-direction and 0x24/0x28 stride discrepancy in a
   standalone, device-tested change.
4. Extend the checked ABI from pads to links, events, and sensor attributes.
5. Split the next low-risk T23/T41 subsystem, such as proc diagnostics or frame
   completion, behind a reviewed interface.
6. Move event/state-machine shells only after IRQ decode and acknowledge
   ordering remain explicit per-SoC behavior.
