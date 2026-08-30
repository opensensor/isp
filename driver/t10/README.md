# Ingenic T10 ISP Recovery

This directory contains the first integrated T10 TX-ISP recovery baseline.
It was reconstructed from the OEM module used with the Thingino
`noname_t10l_jxh42_mt7601` profile and builds against the regular Thingino
Linux 3.10.14 T10 kernel.

## Provenance

- OEM module SHA-256:
  `39b463c62eacae924451a49cc58374ed76740448e24864b4454b5c3c4864a975`
- Binary Ninja export SHA-256:
  `c2e1cf25ba6ab05bcc8c995e4496be9539544678f93cc5080894ad2f4f165c6b`
- Automated recovery: 740 candidates, 735 isolated compiles, 57
  relocation-normalized exact matches, and no fallback stubs.

The generated whole-driver source did not compile as one translation unit.
The integrated module replaces its platform layer with tracked SDK source,
uses reviewed T20/T10 shared firmware and common adapters, and clean-builds
without modifying the Ingenic SDK submodule.

## T10/T20 reuse boundary

The tracked Ingenic SDK deliberately defines `3.10.14/isp/t10` as a symlink
to `t20`. Both targets link the same Apical 3.12.0
`libt20-firmware.a`, and their OEM modules expose the same 742 function
names and the same initialized/uninitialized data sizes.

T10 therefore compiles thin wrapper translation units around the reviewed
T20/T10 sources. The T10 kernel configuration still selects the SoC-specific
MMIO, IRQ, clock, and platform behavior; no T20 object file is reused across
kernel builds. This avoids a duplicate million-line firmware fork while
preserving a separately auditable `tx-isp-t10.ko`.

Sensor registration uses the shared registry adapter, and fixed-point
primitives use the common math core. Camera- and sensor-specific image policy
does not live here: AE, AWB, denoise, lens shading, Iridix, and related tuning
remain calibration/IQ-bin responsibilities.

## One-off repairs

The first integrated audit exposed behavior that a name-only comparison hid:

- the 140-entry API-to-firmware calibration map had been lowered to a short
  `memcpy`, obscuring a truncated-map failure and returning the wrong value;
- thirteen firmware interrupt-request entry points had been outlined through
  one helper and appeared collapsed against the OEM critical sections; and
- the 16-way interrupt-event register map had been reduced to arithmetic,
  hiding a 63-instruction implementation behind a 145-instruction OEM symbol.

The shared source now emits the complete calibration map, returns its address,
inlines the disable/update/enable critical section into every request entry
point, and preserves all 16 explicit event routes.

The five model candidates that failed isolated compilation are not linked:
`register_tx_isp_vic_device`, `register_tx_isp_core_device`,
`tx_isp_release_subdevs`, and `tx_isp_probe` come from the tracked/reviewed
SDK path; `apical_isp_stab_s_attr.isra.0` is accounted for by the recovered
unsuffixed implementation.

## Build

```sh
ROOT=/path/to/thingino/output/t10-camera-3.10.14-uclibc
KDIR="$ROOT/build/linux-<revision>"
CROSS="$ROOT/per-package/ingenic-sdk/host/bin/mipsel-linux-"

make -C "$KDIR" M="$(pwd)/driver/t10" clean
make -C "$KDIR" M="$(pwd)/driver/t10" \
  ARCH=mips CROSS_COMPILE="$CROSS" modules
```

Expected artifact: `driver/t10/tx-isp-t10.ko`.

The August 30, 2026 clean-build checkpoint is ELF32 little-endian MIPS32 R1,
has Linux 3.10.14 vermagic, and has SHA-256
`e38c95053f9f1e722a1ad58f77f296ed49498bfce82a39f989556bf7bce59b0b`.
The artifact is intentionally not checked in.

## Binary audit

```sh
regtrace audit-binary \
  --oem /path/to/oem/tx-isp-t10.ko \
  --recovered driver/t10/tx-isp-t10.ko \
  --objdump "${CROSS}objdump" \
  --replacement-map driver/t10/audit/replacements.json \
  --out driver/t10/audit
```

The current audit accounts for all 742 OEM functions: 708 direct matches and
34 documented compiler-inlining/name replacements. It reports zero missing
replacement groups, OEM-only functions, stubs, collapsed functions, and short
functions. The 132 recovered-only functions are source-level helpers and
split dispatchers introduced by the open implementation.

The audit is structural. It does not prove probe, sensor binding, IRQ cadence,
DMA correctness, image quality, userspace ABI behavior, or unload safety.

## Hardware validation boundary

No T10 device claim is made yet. The first device gate must be surgical and
non-persistent: preserve the stock module, load the open module for one boot,
exercise stock `libimp.so` first, capture bounded UART/kernel logs and frames,
then reboot to restore stock. OpenIMP follows only after the stock-userspace
path works. Full images and OTA are out of scope for this stage.
