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
The integrated source now clean-builds, links, loads non-persistently on a
T20X device, and streams through stock `libimp.so`, OpenIMP, and direct V4L2
capture.

## Source partition

The module deliberately does not rebuild all 740 generated functions:

- 19 unmodified translation units come directly from the tracked Ingenic T20
  SDK;
- eight repaired SDK-derived translation units and their two modified private
  headers are versioned under `sdk/source/`, so the open T20 implementation is
  owned and reviewable in this repository rather than hidden in SDK submodule
  changes;
- the sensor registry uses `driver/common/tx_isp_sinfo.c` through a T20 layout
  adapter;
- five relocation-normalized T20/T30 fixed-point matches use the shared math
  core; and
- 477 archive-backed firmware functions remain in the recovered T20 unit.

`sdk/tx-isp-core-local.h` is an ABI invariant, not a convenience include. It
ensures every mixed local/unchanged SDK translation unit sees the same repaired
`tx_isp_core_device` layout before the legacy SDK header can claim its include
guard. Mixing those definitions moves the day/night work item and later fields
by 16 bytes; device testing showed that split as DMA capture stopping after 25
frames.

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

The August 29, 2026 device-tested checkpoint is an ELF32 little-endian MIPS32
R1 module with the expected Linux 3.10.14 vermagic. Rebuild locally before
comparing an artifact hash; the module is intentionally not checked in.

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

The shared T10 integration also made three audit-sensitive paths explicit:
the complete 140-entry API-to-firmware map, the per-FSM interrupt critical
sections, and all 16 interrupt-event register routes. Both T10 and T20
clean-build with those common repairs, and the refreshed T20 audit retains
zero stub and zero collapsed findings.

## Device validation

The Ingenic T20 and T30 OEM modules share 618 named functions; 133 of those
have identical relocation-normalized instruction streams in the current
reference set. Shared code is accepted only where SDK source, normalized
object code, or both prove the ABI and behavior. T20-only V4L2 topology,
register programming, calibration, and firmware policy remain local.

The same reconstructed driver passed three surgical, RAM-only device gates on
August 29, 2026:

1. stock `libimp.so` produced a clean processed frame;
2. OpenIMP produced clean 640x360 H.264 at 25 fps; and
3. Raptor captured the ISP V4L2 nodes directly and produced clean 640x360
   H.264 at 25 fps.

The direct-V4L2 and OpenIMP reference frames measured SSIM 0.985870 in the
tested scene. Every cycle preserved the stock module and sensor module, staged
the open components under `/tmp`, captured bounded logs and frames, and
rebooted to restore the stock stack.

The subsequent full-resolution Raptor gate used the repository-owned T20
driver and the generic two-buffer V4L2 handoff. It captured 120/120 H.264 High
frames at 1920x1080 and 25.2 fps over IPv6 RTSP/TCP, passed strict decode with
zero source or publish drops and zero ISP IRQ errors, and sustained 3.06 Mbps
against a 3 Mbps request. The driver reserves an 8 MiB sensor-neutral MMAP
pool so two page-aligned 1080p NV12 buffers can overlap capture and encoding;
the vendor 4 MiB default fits only one such frame.

This is evidence from one Wyze Cam V2 for the T20X/JXF23 linear-daylight path,
not a universal sensor or mode claim. Night/IR, WDR, additional sensors, and
long-duration streaming remain open validation work. Runtime image policy is
sensor-neutral: active IQ calibration tables own AE targets, AWB/denoise
response, lens shading, and Iridix behavior; sensor-specific tuning does not
belong in this driver.

Several early-initialization trace messages remain intentionally present. A
build differing from the three-gate artifact only by removal of those messages
rebooted before the userspace-ready marker, so their timing effect must be
isolated with IRQ/state instrumentation before they can be removed safely.
