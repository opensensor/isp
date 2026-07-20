#!/usr/bin/env bash
set -euo pipefail

# Safe, staged T41 module smoke cycle. Every experiment is followed by a
# reboot; the recovered module is only uploaded to /tmp.

IP="${THINGINO_IP:-192.168.50.117}"
USER="${THINGINO_USER:-root}"
PASS="${THINGINO_PASS:-}"
LEVEL="${T41_BRINGUP_LEVEL:--1}"
START_RAPTOR="${T41_START_RAPTOR:-0}"
SENSOR_MODULE="${T41_SENSOR_MODULE:-}"
SMOKE_SECS="${T41_SMOKE_SECS:-5}"
REMOTE_MODULE=/tmp/tx_isp_t41_recovered.ko
LOCAL_MODULE="${T41_MODULE:-driver/t41/tx_isp_t41_recovered.ko}"
LOG="${1:-logs/$(date +%Y%m%d-%H%M%S)-t41-level${LEVEL}-117}"

if [[ "$IP" != "192.168.50.117" ]]; then
	echo "refusing non-target IP: $IP" >&2
	exit 2
fi
if [[ ! "$LEVEL" =~ ^-?[0-9]+$ ]] || (( LEVEL < -1 || LEVEL > 3 )); then
	echo "T41_BRINGUP_LEVEL must be -1, 0, 1, 2, or 3" >&2
	exit 2
fi
if [[ "$START_RAPTOR" != "0" && "$START_RAPTOR" != "1" ]]; then
	echo "T41_START_RAPTOR must be 0 or 1" >&2
	exit 2
fi
if (( LEVEL < 3 )) && [[ "$START_RAPTOR" == "1" ]]; then
	echo "refusing to start Raptor below bring-up level 3" >&2
	exit 2
fi
if [[ ! "$SMOKE_SECS" =~ ^[0-9]+$ ]]; then
	echo "T41_SMOKE_SECS must be a non-negative integer" >&2
	exit 2
fi
if [[ ! -f "$LOCAL_MODULE" ]]; then
	echo "module not found: $LOCAL_MODULE" >&2
	exit 2
fi

SSH_OPTS=(-T -o LogLevel=ERROR -o StrictHostKeyChecking=no
	-o UserKnownHostsFile=/dev/null -o ConnectTimeout=5
	-o ConnectionAttempts=1)
SCP_OPTS=(-O -q -o LogLevel=ERROR -o StrictHostKeyChecking=no
	-o UserKnownHostsFile=/dev/null -o ConnectTimeout=5
	-o ConnectionAttempts=1)
if [[ -n "$PASS" ]]; then
	export SSHPASS="$PASS"
	SSH=(sshpass -e ssh "${SSH_OPTS[@]}" "$USER@$IP")
	SCP=(sshpass -e scp "${SCP_OPTS[@]}")
else
	SSH=(ssh "${SSH_OPTS[@]}" "$USER@$IP")
	SCP=(scp "${SCP_OPTS[@]}")
fi

mkdir -p "$LOG"

# Do not mutate a device that still has an ISP module loaded. The boot setup
# must suppress the stock module before this harness is used.
"${SSH[@]}" sh -s >"$LOG/preflight.txt" <<'EOS'
set -eu
date
uname -a
cat /proc/modules
if awk '{print $1}' /proc/modules | grep -Eq '^tx_isp'; then
	echo "refusing: a TX-ISP module is already loaded" >&2
	exit 42
fi
EOS

"${SCP[@]}" "$LOCAL_MODULE" "$USER@$IP:$REMOTE_MODULE"
experiment_started=1
reboot_device() {
	if [[ "${experiment_started:-0}" == "1" ]]; then
		timeout 12 "${SSH[@]}" 'sync; reboot -f' >/dev/null 2>&1 || true
	fi
}
trap reboot_device EXIT

remote_sensor_module="${SENSOR_MODULE:-__none__}"
set +e
timeout 90 "${SSH[@]}" sh -s -- "$LEVEL" "$START_RAPTOR" \
	"$remote_sensor_module" "$SMOKE_SECS" <<'EOS'
set -u
level="$1"
start_raptor="$2"
sensor_module="$3"
smoke_secs="$4"
if [ "$sensor_module" = "__none__" ]; then
	sensor_module=
fi

capture_state() {
	label="$1"
	{
		date
		uptime
		cat /proc/modules
		cat /proc/interrupts
		ps
		ls -l /dev/tx* /dev/isp* /dev/framechan* 2>/dev/null || true
		find /proc -maxdepth 3 -type f -path '*tx*isp*' -print 2>/dev/null || true
	} >"/tmp/t41-${label}-state.txt" 2>&1
	dmesg >"/tmp/t41-${label}-dmesg.txt" 2>/dev/null || true
	logread >"/tmp/t41-${label}-logread.txt" 2>/dev/null || true
	if command -v logcat >/dev/null 2>&1; then
		logcat -d >"/tmp/t41-${label}-logcat.txt" 2>/dev/null || true
	else
		: >"/tmp/t41-${label}-logcat.txt"
	fi
}

capture_state before
dmesg -c >/tmp/t41-dmesg-before-clear.txt 2>/dev/null || true
set +e
insmod /tmp/tx_isp_t41_recovered.ko tx_isp_bringup_level="$level" \
	>/tmp/t41-insmod.txt 2>&1
insmod_status=$?
set -e
printf '%s\n' "$insmod_status" >/tmp/t41-insmod-status.txt
sleep "$smoke_secs"
capture_state after-insmod

if [ "$insmod_status" -eq 0 ] && [ "$start_raptor" = "1" ]; then
	if [ -n "$sensor_module" ]; then
		insmod "$sensor_module" >/tmp/t41-sensor-insmod.txt 2>&1 || true
	else
		: >/tmp/t41-sensor-insmod.txt
	fi
	/etc/init.d/S31raptor start >/tmp/t41-raptor-start.txt 2>&1 || true
	sleep "$smoke_secs"
	capture_state after-consumer
else
	: >/tmp/t41-sensor-insmod.txt
	: >/tmp/t41-raptor-start.txt
	capture_state after-consumer
fi

# Only attempt live unload for the two intentionally shallow stages. More
# complete graph/memory/tuning experiments return to a clean state by reboot.
if [ "$insmod_status" -eq 0 ] && [ "$level" -le 0 ]; then
	set +e
	rmmod tx_isp_t41_recovered >/tmp/t41-rmmod.txt 2>&1
	printf '%s\n' "$?" >/tmp/t41-rmmod-status.txt
	set -e
else
	: >/tmp/t41-rmmod.txt
	printf '%s\n' skipped >/tmp/t41-rmmod-status.txt
fi
capture_state after-rmmod
exit 0
EOS
remote_status=$?
set -e

"${SCP[@]}" "$USER@$IP:/tmp/t41-*.txt" "$LOG/" 2>/dev/null || true

rg -n -i 'kernel panic|oops|BUG:|unable to handle|segfault|watchdog|fatal exception' \
	"$LOG"/*dmesg.txt >"$LOG/kernel-fatal-signatures.txt" || true

insmod_status="$(tr -d '\r\n' <"$LOG/t41-insmod-status.txt" 2>/dev/null || printf missing)"
rmmod_status="$(tr -d '\r\n' <"$LOG/t41-rmmod-status.txt" 2>/dev/null || printf missing)"
fatal_count="$(wc -l <"$LOG/kernel-fatal-signatures.txt")"
printf 'log=%s\n' "$LOG"
printf 'level=%s insmod_status=%s rmmod_status=%s kernel_fatal_signatures=%s\n' \
	"$LEVEL" "$insmod_status" "$rmmod_status" "$fatal_count"
printf 'remote_status=%s\n' "$remote_status"

if [[ "$remote_status" != "0" || "$insmod_status" != "0" || "$fatal_count" != "0" ]]; then
	exit 1
fi
