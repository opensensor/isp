#!/bin/sh
#
# Verify the recovered T41 module's known streaming BSS-tail contract.
#
# Usage:
#   tools/check_t41_bss_contract.sh [driver/t41/tx-isp-t41.ko]
#
# READELF and NM may override the host tools when required.
set -eu

module=${1:-driver/t41/tx-isp-t41.ko}
readelf_tool=${READELF:-readelf}
nm_tool=${NM:-nm}

[ -r "$module" ] || {
	echo "T41 layout check: module not found: $module" >&2
	exit 2
}

section_size()
{
	"$readelf_tool" -SW "$module" |
		awk -v wanted="$1" '$2 == wanted { print $6; exit }'
}

symbol_pair()
{
	"$nm_tool" -n -S "$module" |
		awk -v wanted="$1" '$4 == wanted { print $1 " " $2; exit }'
}

check_equal()
{
	label=$1
	actual=$2
	expected=$3

	if [ "$actual" != "$expected" ]; then
		echo "T41 layout check: $label is $actual, expected $expected" >&2
		exit 1
	fi
}

check_equal ".data size" "$(section_size .data)" "004560"
check_equal ".bss size" "$(section_size .bss)" "01a520"
check_equal "g_abs_77740 address/size" \
	"$(symbol_pair g_abs_77740)" "0000048c 00000108"
check_equal "g_abs_77bf0 address/size" \
	"$(symbol_pair g_abs_77bf0)" "0000003c 00000202"
check_equal "tx_isp_sinfo_stats address/size" \
	"$(symbol_pair tx_isp_sinfo_stats)" "00004530 00000028"
check_equal "__pow2_lut address/size" \
	"$(symbol_pair __pow2_lut)" "00019e6c 00000400"
check_equal "tx_isp_sinfo_bss_layout address/size" \
	"$(symbol_pair tx_isp_sinfo_bss_layout)" "0001a290 00000284"

echo "T41 BSS contract: ok ($module)"
