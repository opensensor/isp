#!/bin/bash
# Local compile baseline for the open Ingenic ISP driver.
#
# Builds the per-SoC driver from driver/<SOC>/ (default t31 -> tx-isp-t31.ko)
# plus the shared diagnostic driver/tx_isp_trace.ko, out-of-tree against a
# thingino buildroot output. If no matching SoC output is built locally, any
# Ingenic 3.10.14 output works: same kernel version + same mipsel uclibc
# toolchain exercise the identical 3.10 kernel API surface. The resulting .ko
# is a *compile baseline*, not necessarily a loadable artifact for that SoC.
#
# Two gcc-15 diagnostics are relaxed because the source predates gcc-14's
# promotion of these to hard errors (older Ingenic SDK toolchains only warned):
#   - implicit-function-declaration  (e.g. get_driver_common_interfaces, an
#                                      external SDK symbol resolved at load time)
#   - int-conversion
# These are pre-existing latent issues, independent of file naming.
#
# Parameters (all overridable via environment):
#   TH      buildroot checkout            (default: ../thingino-firmware)
#   ROOT    a single output/<...> dir     (default: first output with a toolchain)
#   KDIR    kernel build tree             (default: $ROOT/build/linux-*)
#   CROSS   cross-compile prefix          (default: mipsel-linux-)
#   ARCH    target arch                   (default: mips)
#   SOC     per-SoC driver subdir         (default: t31 -> driver/t31/)
#
# Examples:
#   ./build_local.sh                       # autodetect everything, build modules
#   SOC=t20 ./build_local.sh               # build driver/t20/ instead
#   ROOT=/path/to/output/t31_foo ./build_local.sh
#   TH=~/src/thingino-firmware ./build_local.sh clean
set -euo pipefail

HERE="$(cd "$(dirname "$0")" && pwd)"

TH="${TH:-$(cd "$HERE/.." && pwd)/thingino-firmware}"
CROSS="${CROSS:-mipsel-linux-}"
ARCH="${ARCH:-mips}"

# ROOT: an output/<config> dir containing host/bin + build/linux-*.
# If unset, pick the first one that ships a usable cross-gcc.
if [ -z "${ROOT:-}" ]; then
	[ -d "$TH/output" ] || { echo "buildroot output not found under: $TH/output (set TH=)"; exit 1; }
	ROOT="$(find "$TH/output" -maxdepth 6 -path '*/host/bin/'"${CROSS}gcc" \
	         -printf '%h\n' 2>/dev/null | sed 's#/host/bin$##' | sort | head -1)"
	[ -n "$ROOT" ] || { echo "no ${CROSS}gcc toolchain found under $TH/output (set ROOT=)"; exit 1; }
fi

# KDIR: the built kernel tree inside ROOT.
if [ -z "${KDIR:-}" ]; then
	KDIR="$(find "$ROOT/build" -maxdepth 1 -type d -name 'linux-*' 2>/dev/null | sort | head -1)"
fi

[ -x "$ROOT/host/bin/${CROSS}gcc" ] || { echo "toolchain missing: $ROOT/host/bin/${CROSS}gcc"; exit 1; }
[ -n "${KDIR:-}" ] && [ -f "$KDIR/Module.symvers" ] || { echo "kernel tree not built (no Module.symvers): ${KDIR:-<unset>}"; exit 1; }

echo "ROOT=$ROOT"
echo "KDIR=$KDIR"

export PATH="$ROOT/host/bin:$PATH"

KCFLAGS="-Wno-error=implicit-function-declaration -Wno-error=int-conversion -Wno-error=implicit-int"
GOAL="${1:-modules}"

# Per-SoC ISP driver (driver/<SOC>/, default t31) -> tx-isp-<soc>.ko
make -C "$KDIR" M="$HERE/driver/${SOC:-t31}" DIR=. \
     ARCH="$ARCH" CROSS_COMPILE="$CROSS" KCFLAGS="$KCFLAGS" "$GOAL"

# Shared SoC-agnostic diagnostics (driver/) -> tx_isp_trace.ko
make -C "$KDIR" M="$HERE/driver" DIR=. \
     ARCH="$ARCH" CROSS_COMPILE="$CROSS" KCFLAGS="$KCFLAGS" "$GOAL"
