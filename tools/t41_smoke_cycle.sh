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
live_log_pid=
stop_live_log() {
	if [[ -n "${live_log_pid:-}" ]]; then
		kill "$live_log_pid" >/dev/null 2>&1 || true
		wait "$live_log_pid" >/dev/null 2>&1 || true
		live_log_pid=
	fi
}
reboot_device() {
	if [[ "${experiment_started:-0}" == "1" ]]; then
		timeout 12 "${SSH[@]}" 'sync; reboot -f' >/dev/null 2>&1 || true
	fi
}
cleanup() {
	stop_live_log
	reboot_device
}
trap cleanup EXIT

# BusyBox dmesg on the target has no follow mode. Polling the tail preserves a
# host-side breadcrumb if the kernel locks before the normal /tmp logs can be
# retrieved. This is read-only and intentionally does not use dmesg -c.
timeout 120 "${SSH[@]}" \
	'while :; do date; dmesg | tail -n 1200; echo ===t41-live-sample===; sleep 1; done' \
	>"$LOG/t41-live-dmesg.txt" 2>&1 &
live_log_pid=$!
sleep 1

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
	proc_diag="/tmp/t41-${label}-process.txt"
	{
		date
		uptime
		cat /proc/modules
		cat /proc/interrupts
		ps
		ls -l /dev/tx* /dev/isp* /dev/aisp* /dev/framechan* /dev/misc-* 2>/dev/null || true
		find /proc/isp -maxdepth 2 -print 2>/dev/null || true
		find /proc -maxdepth 3 -type f -path '*tx*isp*' -print 2>/dev/null || true
	} >"/tmp/t41-${label}-state.txt" 2>&1
	: >"$proc_diag"
	for pid in $(pidof rvd 2>/dev/null); do
		{
			echo "pid=$pid"
			cat "/proc/$pid/status" 2>/dev/null || true
			cat "/proc/$pid/stack" 2>/dev/null || true
			i=0
			while [ "$i" -lt 256 ] && kill -0 "$pid" 2>/dev/null; do
				cat "/proc/$pid/syscall" 2>/dev/null || true
				i=$((i + 1))
			done
		} >>"$proc_diag" 2>&1
	done
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
stop_live_log

set +e
timeout 30 "${SSH[@]}" 'cd /tmp && tar -cf - t41-*.txt' \
	>"$LOG/t41-logs.tar" 2>"$LOG/t41-log-copy.txt"
log_copy_status=$?
set -e
if [[ "$log_copy_status" == "0" ]]; then
	tar -xf "$LOG/t41-logs.tar" -C "$LOG"
fi

rg -n -i 'kernel panic|kernel bug detected|oops|BUG:|unable to handle|unhandled kernel unaligned access|segfault|watchdog|fatal exception' \
	"$LOG"/*dmesg.txt >"$LOG/kernel-fatal-signatures.txt" || true

insmod_status="$(tr -d '\r\n' <"$LOG/t41-insmod-status.txt" 2>/dev/null || printf missing)"
rmmod_status="$(tr -d '\r\n' <"$LOG/t41-rmmod-status.txt" 2>/dev/null || printf missing)"
fatal_count="$(wc -l <"$LOG/kernel-fatal-signatures.txt")"
printf 'log=%s\n' "$LOG"
printf 'level=%s insmod_status=%s rmmod_status=%s kernel_fatal_signatures=%s\n' \
	"$LEVEL" "$insmod_status" "$rmmod_status" "$fatal_count"
printf 'remote_status=%s\n' "$remote_status"
printf 'log_copy_status=%s\n' "$log_copy_status"

if [[ "$remote_status" != "0" || "$log_copy_status" != "0" || \
	"$insmod_status" != "0" || "$fatal_count" != "0" ]]; then
	exit 1
fi
