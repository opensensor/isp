# T21 first-pass compile and collapse audit

Date: 2026-08-25

## Build result

- `tx_isp_t21_recovered.ko` builds and links against the T21 3.10.14 Thingino
  kernel tree.
- Output: `src/tx_isp_t21_recovered.kbuild/tx_isp_t21_recovered.ko`
- Module ABI: ELF32 little-endian MIPS, o32, MIPS32 R1.
- Module vermagic: `3.10.14__isvp_turkey_1.0__ preempt mod_unload MIPS32_R1 32BIT`.
- MODPOST has one warning: `get_isp_priv_mem` is unresolved.  This is an OEM
  platform/allocator hook, not a generated compiler helper.
- The earlier accidental `__divdi3` dependency is gone.
- This is a compile milestone, not a runtime-ready declaration.  The recovery
  still requires `-fpermissive` because many untouched decompiler-generated
  pointer/integer prototypes remain noisy, and no camera smoke test was run.

## Binary audit result

The assembly audit compares named functions in the OEM and recovered modules.
Instruction counts exclude relocation records.

| Metric | First audit | After repair |
|---|---:|---:|
| Matched functions | 612 | 612 |
| Stub findings | 11 | 3 |
| Collapsed findings | 36 | 29 |
| Matched instruction ratio | 0.719 | 0.771 |

Two of the three remaining stub findings are deliberate tail-call aliases:

- `tisp_math_exp2` tail-calls `private_math_exp2`.
- `tisp_sdns_intp_reg_refresh` tail-calls `tisp_sdns_all_reg_refresh`.

The only remaining literal empty stub is `Tiziano_Awb_Ct_Detect` (OEM: 1,439
instructions).  It is a full AWB clustering/color-temperature algorithm and
needs a dedicated algorithm-recovery pass.

## Repairs made

- Recovered whole-driver kernel scaffolding, duplicate symbol names, ioctl
  signatures, indirect-call arities, and malformed register/print/wait calls.
- Recovered `tx_isp_probe` registration and unwind behavior from OEM assembly.
- Restored allocations which were emitted as `private_kmalloc(...); ptr = 0;`
  across VIN, VIC, frame-source, subdevice pads, parameter operation, AE/ROI
  buffers, and `tisp_init` working buffers.
- Recovered the ISP private-memory free-list path (`isp_mem_init`,
  `find_new_buffer`, and `isp_malloc_buffer`).
- Recovered sensor gain/integration allocators and the exact 33-entry exp2 LUT.
- Restored volatile ISP MMIO access and removed the generated 64-bit division
  dependency from ADR/GIB interpolation.
- Recovered the primary ioctl dispatch path, sensor ioctl fan-out, and buffer
  control cases.
- Recovered `subdev_sensor_ops_set_input` from OEM MIPS, including list lookup,
  pipeline stop/start callbacks, locking, and selected-sensor dimensions.
- Recovered the main `apical_isp_core_ops_s_ctrl` command dispatcher for direct
  ISP controls and its ROI/weight helper routes.  Complex exposure/WB/AF payload
  cases remain for a later pass.
- Restored `ispcore_core_ops_init` state validation, reset/lock flow, TISP
  initialization, firmware thread start/stop, and error returns.  Its OEM format
  decision tree is still materially more detailed than the recovered form.
- Prevented compiler inference from erasing `tisp_adr_process`; its algorithm
  call and MMIO writeback sequence are now present.
- Restored VIC IRQ enable/disable state handling and the probe allocation which
  had allowed those paths to collapse.

## Remaining collapse queue

High-priority algorithm bodies (real loss, not size-only noise):

1. `Tiziano_Awb_Ct_Detect`
2. `Tiziano_adr_fpga`
3. `ae_tune2`
4. `Tiziano_defog_fpga`
5. `tiziano_defog_algorithm`
6. `tisp_ae_process_impl`
7. `tiziano_adr_algorithm`
8. `Tiziano_ae_fpga`

Partially recovered dispatch/state machines:

- `tx_isp_unlocked_ioctl`
- `apical_isp_core_ops_s_ctrl`
- `isp_vic_cmd_set`
- `ispcore_core_ops_init`

Compact parameter-array accessors remain flagged because the recovered switch
trees have far fewer branches than OEM.  They need command-by-command table
validation; instruction ratio alone cannot distinguish compact C from missing
cases.  See `binary-audit/binary_audit.md` for the complete list and counts.

## Reproduction

Build:

```sh
KDIR=/path/to/thingino/output/t21-camera/build/linux-<revision>
CROSS=/path/to/thingino/output/t21-camera/per-package/ingenic-sdk/host/bin/mipsel-linux-

make -C "$KDIR" \
  M="$(pwd)/driver/t21" \
  ARCH=mips \
  CROSS_COMPILE="$CROSS" \
  -j2
```

Audit:

```sh
regtrace audit-binary \
  --oem tx-isp-t21.ko \
  --recovered driver/t21/tx-isp-t21.ko \
  --objdump "${CROSS}objdump" \
  --out driver/t21/audit
```
