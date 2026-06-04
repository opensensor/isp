#!/bin/bash
# Local compile baseline for the open T31 ISP driver.
#
# Builds driver/tx-isp-t31.ko out-of-tree against a thingino buildroot output.
# No T31 output is built locally, so we compile against the T20L 3.10.14 tree:
# same Ingenic kernel version + same mipsel uclibc toolchain, so it exercises
# the identical 3.10 kernel API surface. The resulting .ko is a *compile
# baseline*, not a loadable T31 artifact.
#
# Two gcc-15 diagnostics are relaxed because the source predates gcc-14's
# promotion of these to hard errors (older Ingenic SDK toolchains only warned):
#   - implicit-function-declaration  (e.g. get_driver_common_interfaces, an
#                                      external SDK symbol resolved at load time)
#   - int-conversion
# These are pre-existing latent issues, independent of file naming.
set -e

TH="${TH:-/mnt/data/hardware/thingino-firmware}"
ROOT="${ROOT:-$TH/output/master/xiaomi_xiaofang_t20l_jxf23_rtl8189ftv-3.10.14-uclibc}"
KDIR="${KDIR:-$ROOT/build/linux-45a11a3318ee823a83536db737a8e1136ed766fd}"

[ -x "$ROOT/host/bin/mipsel-linux-gcc" ] || { echo "toolchain missing: $ROOT/host/bin"; exit 1; }
[ -f "$KDIR/Module.symvers" ]           || { echo "kernel tree not built: $KDIR"; exit 1; }

export PATH="$ROOT/host/bin:$PATH"

make -C "$KDIR" \
     M="$(cd "$(dirname "$0")" && pwd)/driver" \
     DIR=. \
     ARCH=mips \
     CROSS_COMPILE=mipsel-linux- \
     KCFLAGS="-Wno-error=implicit-function-declaration -Wno-error=int-conversion -Wno-error=implicit-int" \
     "${@:-modules}"
