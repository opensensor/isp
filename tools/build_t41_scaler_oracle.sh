#!/bin/sh
set -eu
[ "$#" = 3 ] || { echo "usage: $0 CROSS_PREFIX STOCK_OBJECT OUTPUT_DIR" >&2; exit 2; }
cross=$1
stock=$2
output=$3
repo=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
mkdir -p "$output"
python3 "$repo/tools/gen_t41_ccm_oracle.py" "$stock" scaler > "$output/reference.S"
"${cross}gcc" -O2 -Wall -Wextra -Werror -fno-builtin -mno-abicalls -fno-pic \
    -c "$repo/tests/t41_tmo_oracle_support.c" -o "$output/support.o"
"${cross}gcc" -O2 -Wall -Wextra -Werror -fno-builtin -mno-abicalls -fno-pic \
    -c "$repo/tests/t41_scaler_oracle_support.c" -o "$output/scaler-support.o"
"${cross}gcc" -O2 -Wall -Wextra -Werror -static \
    "$repo/tests/t41_scaler_oracle_check.c" "$output/reference.S" \
    "$repo/driver/common/tx_isp_scaler.c" "$repo/driver/t41/tx_isp_t41_scaler.c" \
    "$output/support.o" "$output/scaler-support.o" -o "$output/scaler-oracle-check"
