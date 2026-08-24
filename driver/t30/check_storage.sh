#!/bin/sh
set -eu

script_dir=$(CDPATH='' cd -- "$(dirname -- "$0")" && pwd)
source_file="$script_dir/tx_isp_t30_recovered.c"

if grep -En 'static .*(data|g_abs)_[[:xdigit:]]+' "$source_file"; then
	echo "T30 recovery contains synthetic address-named storage" >&2
	exit 1
fi

if grep -En '\[16384\]' "$source_file"; then
	echo "T30 recovery contains a synthetic 16 KiB object" >&2
	exit 1
fi

if grep -En 'asm[[:space:]]+volatile' "$source_file"; then
	echo "T30 recovery contains inline address-building assembly" >&2
	exit 1
fi

echo "T30 recovered storage audit passed"
