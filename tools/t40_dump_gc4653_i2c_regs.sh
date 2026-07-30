#!/usr/bin/env bash
set -euo pipefail

IP="${THINGINO_IP:-192.168.50.242}"
USER="${THINGINO_USER:-root}"
PASS="${THINGINO_PASS:-}"
BUS="${GC4653_I2C_BUS:-1}"
ADDR="${GC4653_I2C_ADDR:-0x29}"
LOG="${1:-logs/$(date +%Y%m%d-%H%M%S)-t40-gc4653-i2c-regs}"

# Keep this focused on mode/timing/exposure banks. These are enough to catch
# sensor-side differences without spending minutes walking sparse address space.
RANGES="${GC4653_REG_RANGES:-\
0x0100-0x0120 \
0x0200-0x023f \
0x0300-0x037f \
0x3000-0x31ff \
0x3200-0x33ff \
0x3400-0x36ff \
0x3800-0x39ff}"
RANGES_CSV="${RANGES// /,}"

if [[ "$IP" != "192.168.50.242" ]]; then
	echo "refusing non-target IP: $IP" >&2
	exit 2
fi

if [[ -n "$PASS" ]]; then
	SSH=(sshpass -p "$PASS" ssh -T -o LogLevel=ERROR \
		-o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null \
		-o ConnectTimeout=5 "$USER@$IP")
else
	SSH=(ssh -T -o LogLevel=ERROR -o StrictHostKeyChecking=no \
		-o UserKnownHostsFile=/dev/null -o ConnectTimeout=5 "$USER@$IP")
fi

mkdir -p "$LOG"

"${SSH[@]}" \
	"BUS=$BUS" \
	"ADDR=$ADDR" \
	"RANGES_CSV=$RANGES_CSV" \
	sh -s >"$LOG/gc4653-regs.txt" 2>"$LOG/dump.err" <<'EOS'
set -u
RANGES="$(printf '%s' "$RANGES_CSV" | tr ',' ' ')"

echo "# GC4653 I2C register dump $(date)"
echo "# bus=$BUS addr=$ADDR"
echo "# ranges=$RANGES"
echo "# format: reg value"

read_reg16() {
	reg="$1"
	hi=$(( (reg >> 8) & 255 ))
	lo=$(( reg & 255 ))
	if out="$(i2ctransfer -f -y "$BUS" "w2@$ADDR" \
		"$(printf '0x%02x' "$hi")" "$(printf '0x%02x' "$lo")" \
		r1 2>/dev/null)"; then
		printf '0x%04x %s\n' "$reg" "$out"
	else
		printf '0x%04x ERR\n' "$reg"
	fi
}

for range in $RANGES; do
	start="${range%-*}"
	end="${range#*-}"
	start_dec=$((start))
	end_dec=$((end))
	printf '# range %s\n' "$range"
	reg="$start_dec"
	while [ "$reg" -le "$end_dec" ]; do
		read_reg16 "$reg"
		reg=$((reg + 1))
	done
done
EOS

{
	echo "log=$LOG"
	echo "regs=$LOG/gc4653-regs.txt"
	echo "stderr=$LOG/dump.err"
	grep -vc '^#' "$LOG/gc4653-regs.txt" | sed 's/^/reg_count=/'
	err_count="$(grep -c ' ERR$' "$LOG/gc4653-regs.txt" || true)"
	echo "err_count=$err_count"
} | tee "$LOG/summary.txt"
