# Ingenic T20 ISP Recovery

This directory contains the T20 TX-ISP recovery baseline reconstructed from
the OEM `tx-isp-t20.ko` used by the Thingino
`wyze_cam2_t20x_jxf23_rtl8189ftv` profile. It follows the same upstream SDK,
binary-audit, shared-adapter, and hardware-validation split used for the T21
and T30 recoveries.

## Provenance

- OEM module SHA-256:
  `c41ffded823ebd51d8ef4c2227342a1dfdf1cb82e312e5bfc9fb637d01f30298`
- Binary Ninja export SHA-256:
  `28e6b8f01e57180e4bd7fb40ea18fca1b889c73fd0906add4cbda2e45c1c8a83`
- Automated recovery: 740 candidates, 731 isolated compiles, and 53
  relocation-normalized exact matches.

The generated whole-driver source initially failed its aggregate compile.
The integrated source now clean-builds and links, but no device-load or stream
claim is made until the non-persistent hardware smoke cycle is complete.

## Source partition

The module deliberately does not rebuild all 740 generated functions:

- 27 translation units come directly from the tracked Ingenic T20 SDK;
- the sensor registry uses `driver/common/tx_isp_sinfo.c` through a T20 layout
  adapter;
- five relocation-normalized T20/T30 fixed-point matches use the shared math
  core; and
- 477 archive-backed firmware functions remain in the recovered T20 unit.

The OEM `stab` object and the 12,240-byte `__fw` object are the canonical
firmware storage. Generated absolute-address aliases were collapsed into
those objects. This reduced module BSS from 529,696 bytes to 38,128 bytes;
the OEM module uses 34,016 bytes.

`extract_firmware.py` is a baseline extractor, not a source generator for the
reviewed final unit. It refuses to overwrite the checked-in repaired source
unless `--force` is given; intentional regeneration must reapply and audit the
one-off repairs recorded under `audit/`.

## Build

Build against a compatible Thingino T20 Linux 3.10.14 output:

```sh
ROOT=/path/to/thingino/output/t20-camera-3.10.14-uclibc
KDIR="$ROOT/build/linux-<revision>"
CROSS="$ROOT/per-package/ingenic-sdk/host/bin/mipsel-linux-"

make -C "$KDIR" M="$(pwd)/driver/t20" \
  ARCH=mips CROSS_COMPILE="$CROSS" modules
```

Expected artifact: `driver/t20/tx-isp-t20.ko`.

The August 26, 2026 clean-build checkpoint is an ELF32 little-endian MIPS32 R1
module with the expected Linux 3.10.14 vermagic. Its unstripped artifact is
781,068 bytes with SHA-256
`abd9fdd4447df6dc20999cb83ac7213b3cd870075b35e93f4d4afecbd38d2c79`.

## Binary audit

```sh
regtrace audit-binary \
  --oem /path/to/oem/tx-isp-t20.ko \
  --recovered driver/t20/tx-isp-t20.ko \
  --objdump "${CROSS}objdump" \
  --replacement-map driver/t20/audit/replacements.json \
  --out driver/t20/audit
```

The current audit matches 721 of 742 OEM functions: 711 by name and 10 by a
documented replacement grouping. There are zero stub and zero collapsed
findings. Replacement groups account for OEM compiler inlining across SDK
dispatcher/helper boundaries; they do not hide missing symbols. The only real
first-pass collapse, `cmos_fsm_process_interrupt`, was restored from 13 to 640
instructions against the common Apical state/interrupt contract.

## Validation boundary

The Ingenic T20 and T30 OEM modules share 618 named functions; 133 of those
have identical relocation-normalized instruction streams in the current
reference set. Shared code is accepted only where SDK source, normalized
object code, or both prove the ABI and behavior. T20-only V4L2 topology,
register programming, calibration, and firmware policy remain local.

The next gate is a one-boot device cycle: preserve the stock module, load this
module non-persistently, bind the JXF23 sensor, stream a bounded capture, record
`dmesg`/`logread` plus frame hashes, then reboot to restore the stock state.
