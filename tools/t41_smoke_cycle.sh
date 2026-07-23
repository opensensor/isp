#!/usr/bin/env bash
set -euo pipefail

# Safe, staged T41 module smoke cycle. Every experiment is followed by a
# reboot; the recovered module is only uploaded to /tmp.

IP="${THINGINO_IP:-192.168.50.244}"
USER="${THINGINO_USER:-root}"
PASS="${THINGINO_PASS:-}"
LEVEL="${T41_BRINGUP_LEVEL:--1}"
START_RAPTOR="${T41_START_RAPTOR:-0}"
LOAD_SENSOR="${T41_LOAD_SENSOR:-$START_RAPTOR}"
SENSOR_MODULE="${T41_SENSOR_MODULE:-/lib/modules/4.4.94/ingenic/sensor_os04d10_t41.ko}"
SENSOR_INITIAL_INTEGRATION="${T41_SENSOR_INITIAL_INTEGRATION:-}"
SENSOR_INITIAL_AGAIN="${T41_SENSOR_INITIAL_AGAIN:-}"
SMOKE_SECS="${T41_SMOKE_SECS:-5}"
CONSUMER_SECS="${T41_CONSUMER_SECS:-$SMOKE_SECS}"
CHECKPOINT_MS="${T41_CHECKPOINT_MS:-0}"
CHECKPOINT_START="${T41_CHECKPOINT_START:-0}"
RECOVERED_PARAMS="${T41_RECOVERED_PARAMS:-1}"
REMOTE_MODULE=/tmp/tx_isp_t41_recovered.ko
LOCAL_MODULE="${T41_MODULE:-driver/t41/tx_isp_t41_recovered.ko}"
FRAME_PROBE="${T41_FRAME_PROBE:-}"
REMOTE_FRAME_PROBE=/tmp/t41_frame_probe
IMP_TRACE="${T41_IMP_TRACE:-}"
REMOTE_IMP_TRACE=/tmp/t41_imp_trace.so
RAPTOR_CONFIG="${T41_RAPTOR_CONFIG:-}"
REMOTE_RAPTOR_CONFIG=/tmp/t41_raptor.conf
IMP_FS_PROBE="${T41_IMP_FS_PROBE:-}"
REMOTE_IMP_FS_PROBE=/tmp/t41_imp_fs_probe
KERNEL_TRACE="${T41_KERNEL_TRACE:-}"
KERNEL_TRACE_QUALITY_ONLY="${T41_KERNEL_TRACE_QUALITY_ONLY:-1}"
REMOTE_KERNEL_TRACE=/tmp/t41_kernel_trace.ko
LOG="${1:-logs/$(date +%Y%m%d-%H%M%S)-t41-level${LEVEL}-${IP##*.}}"

case "$IP" in
	192.168.50.117 | 192.168.50.244) ;;
	*)
		echo "refusing non-target IP: $IP" >&2
		exit 2
		;;
esac
if [[ ! "$LEVEL" =~ ^-?[0-9]+$ ]] || (( LEVEL < -1 || LEVEL > 3 )); then
	echo "T41_BRINGUP_LEVEL must be -1, 0, 1, 2, or 3" >&2
	exit 2
fi
if [[ "$START_RAPTOR" != "0" && "$START_RAPTOR" != "1" ]]; then
	echo "T41_START_RAPTOR must be 0 or 1" >&2
	exit 2
fi
if [[ "$LOAD_SENSOR" != "0" && "$LOAD_SENSOR" != "1" ]]; then
	echo "T41_LOAD_SENSOR must be 0 or 1" >&2
	exit 2
fi
if [[ -n "$SENSOR_INITIAL_INTEGRATION" &&
      ! "$SENSOR_INITIAL_INTEGRATION" =~ ^[0-9]+$ ]]; then
	echo "T41_SENSOR_INITIAL_INTEGRATION must be a decimal integer" >&2
	exit 2
fi
if [[ -n "$SENSOR_INITIAL_AGAIN" &&
      ! "$SENSOR_INITIAL_AGAIN" =~ ^(0[xX][0-9a-fA-F]+|[0-9]+)$ ]]; then
	echo "T41_SENSOR_INITIAL_AGAIN must be a decimal or hexadecimal integer" >&2
	exit 2
fi
if [[ "$START_RAPTOR" == "1" && "$LOAD_SENSOR" != "1" ]]; then
	echo "T41_START_RAPTOR=1 requires T41_LOAD_SENSOR=1" >&2
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
if [[ ! "$CONSUMER_SECS" =~ ^[0-9]+$ ]]; then
	echo "T41_CONSUMER_SECS must be a non-negative integer" >&2
	exit 2
fi
if [[ ! "$CHECKPOINT_MS" =~ ^[0-9]+$ ]] || (( CHECKPOINT_MS > 1000 )); then
	echo "T41_CHECKPOINT_MS must be an integer from 0 through 1000" >&2
	exit 2
fi
if [[ ! "$CHECKPOINT_START" =~ ^[0-9]+$ ]] || (( CHECKPOINT_START > 64 )); then
	echo "T41_CHECKPOINT_START must be an integer from 0 through 64" >&2
	exit 2
fi
if [[ "$RECOVERED_PARAMS" != "0" && "$RECOVERED_PARAMS" != "1" ]]; then
	echo "T41_RECOVERED_PARAMS must be 0 or 1" >&2
	exit 2
fi
if [[ "$KERNEL_TRACE_QUALITY_ONLY" != "0" &&
      "$KERNEL_TRACE_QUALITY_ONLY" != "1" ]]; then
	echo "T41_KERNEL_TRACE_QUALITY_ONLY must be 0 or 1" >&2
	exit 2
fi
if [[ ! -f "$LOCAL_MODULE" ]]; then
	echo "module not found: $LOCAL_MODULE" >&2
	exit 2
fi
if [[ -n "$FRAME_PROBE" && ! -f "$FRAME_PROBE" ]]; then
	echo "frame probe not found: $FRAME_PROBE" >&2
	exit 2
fi
if [[ -n "$IMP_TRACE" && ! -f "$IMP_TRACE" ]]; then
	echo "IMP trace preload not found: $IMP_TRACE" >&2
	exit 2
fi
if [[ -n "$RAPTOR_CONFIG" && ! -f "$RAPTOR_CONFIG" ]]; then
	echo "Raptor config not found: $RAPTOR_CONFIG" >&2
	exit 2
fi
if [[ -n "$IMP_FS_PROBE" && ! -f "$IMP_FS_PROBE" ]]; then
	echo "IMP frame-source probe not found: $IMP_FS_PROBE" >&2
	exit 2
fi
if [[ -n "$KERNEL_TRACE" && ! -f "$KERNEL_TRACE" ]]; then
	echo "kernel register trace module not found: $KERNEL_TRACE" >&2
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
LOG="$(realpath -m "$LOG")"

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
if [[ -n "$FRAME_PROBE" ]]; then
	"${SCP[@]}" "$FRAME_PROBE" "$USER@$IP:$REMOTE_FRAME_PROBE"
fi
if [[ -n "$IMP_TRACE" ]]; then
	"${SCP[@]}" "$IMP_TRACE" "$USER@$IP:$REMOTE_IMP_TRACE"
fi
if [[ -n "$RAPTOR_CONFIG" ]]; then
	"${SCP[@]}" "$RAPTOR_CONFIG" "$USER@$IP:$REMOTE_RAPTOR_CONFIG"
fi
if [[ -n "$IMP_FS_PROBE" ]]; then
	"${SCP[@]}" "$IMP_FS_PROBE" "$USER@$IP:$REMOTE_IMP_FS_PROBE"
fi
if [[ -n "$KERNEL_TRACE" ]]; then
	"${SCP[@]}" "$KERNEL_TRACE" "$USER@$IP:$REMOTE_KERNEL_TRACE"
fi
experiment_started=1
live_log_pid=
live_user_log_pid=
live_logcat_pid=
stop_live_log() {
	if [[ -n "${live_log_pid:-}" ]]; then
		kill "$live_log_pid" >/dev/null 2>&1 || true
		wait "$live_log_pid" >/dev/null 2>&1 || true
		live_log_pid=
	fi
	if [[ -n "${live_user_log_pid:-}" ]]; then
		kill "$live_user_log_pid" >/dev/null 2>&1 || true
		wait "$live_user_log_pid" >/dev/null 2>&1 || true
		live_user_log_pid=
	fi
	if [[ -n "${live_logcat_pid:-}" ]]; then
		kill "$live_logcat_pid" >/dev/null 2>&1 || true
		wait "$live_logcat_pid" >/dev/null 2>&1 || true
		live_logcat_pid=
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

# BusyBox dmesg has no follow mode, but /dev/kmsg provides an independent,
# record-oriented stream.  Unlike polling dmesg, this preserves the final
# printk even when the target resets within a few milliseconds.
timeout 120 "${SSH[@]}" 'cat /dev/kmsg' \
	>"$LOG/t41-live-dmesg.txt" 2>&1 &
live_log_pid=$!
timeout 120 "${SSH[@]}" 'logread -f' \
	>"$LOG/t41-live-logread.txt" 2>&1 &
live_user_log_pid=$!
timeout 120 "${SSH[@]}" 'exec logcat' \
	>"$LOG/t41-live-logcat.txt" 2>&1 &
live_logcat_pid=$!
sleep 1

remote_sensor_module="${SENSOR_MODULE:-__none__}"
remote_frame_probe="${FRAME_PROBE:+$REMOTE_FRAME_PROBE}"
remote_frame_probe="${remote_frame_probe:-__none__}"
remote_imp_trace="${IMP_TRACE:+$REMOTE_IMP_TRACE}"
remote_imp_trace="${remote_imp_trace:-__none__}"
remote_raptor_config="${RAPTOR_CONFIG:+$REMOTE_RAPTOR_CONFIG}"
remote_raptor_config="${remote_raptor_config:-__none__}"
remote_imp_fs_probe="${IMP_FS_PROBE:+$REMOTE_IMP_FS_PROBE}"
remote_imp_fs_probe="${remote_imp_fs_probe:-__none__}"
remote_kernel_trace="${KERNEL_TRACE:+$REMOTE_KERNEL_TRACE}"
remote_kernel_trace="${remote_kernel_trace:-__none__}"
remote_sensor_initial_integration="${SENSOR_INITIAL_INTEGRATION:-__none__}"
remote_sensor_initial_again="${SENSOR_INITIAL_AGAIN:-__none__}"
set +e
timeout 90 "${SSH[@]}" sh -s -- "$LEVEL" "$LOAD_SENSOR" \
	"$START_RAPTOR" "$remote_sensor_module" "$SMOKE_SECS" "$CHECKPOINT_MS" \
	"$CHECKPOINT_START" "$remote_frame_probe" "$remote_imp_trace" \
	"$remote_raptor_config" "$remote_imp_fs_probe" "$RECOVERED_PARAMS" \
	"$remote_kernel_trace" "$remote_sensor_initial_integration" \
	"$remote_sensor_initial_again" "$CONSUMER_SECS" \
	"$KERNEL_TRACE_QUALITY_ONLY" \
	>"$LOG/t41-remote-run.txt" 2>&1 <<'EOS'
set -u
level="$1"
load_sensor="$2"
start_raptor="$3"
sensor_module="$4"
smoke_secs="$5"
checkpoint_ms="$6"
checkpoint_start="$7"
frame_probe="$8"
imp_trace="$9"
raptor_config="${10}"
imp_fs_probe="${11}"
recovered_params="${12}"
kernel_trace="${13}"
sensor_initial_integration="${14}"
sensor_initial_again="${15}"
consumer_secs="${16}"
kernel_trace_quality_only="${17}"
if [ "$sensor_module" = "__none__" ]; then
	sensor_module=
fi
if [ "$frame_probe" = "__none__" ]; then
	frame_probe=
fi
if [ "$imp_trace" = "__none__" ]; then
	imp_trace=
fi
if [ "$raptor_config" = "__none__" ]; then
	raptor_config=
fi
if [ "$imp_fs_probe" = "__none__" ]; then
	imp_fs_probe=
fi
if [ "$kernel_trace" = "__none__" ]; then
	kernel_trace=
fi
if [ "$sensor_initial_integration" = "__none__" ]; then
	sensor_initial_integration=
fi
if [ "$sensor_initial_again" = "__none__" ]; then
	sensor_initial_again=
fi

capture_state() {
	label="$1"
	proc_diag="/tmp/t41-${label}-process.txt"
	reg_diag="/tmp/t41-${label}-regs.txt"
	# Preserve the first kernel fault before slower /proc diagnostics can block
	# on a damaged task or the allocator's cascading failure path.
	timeout 2 dmesg >"/tmp/t41-${label}-dmesg.txt" 2>/dev/null || true
	timeout 2 logread >"/tmp/t41-${label}-logread.txt" 2>/dev/null || true
	if command -v logcat >/dev/null 2>&1; then
		logcat -d >"/tmp/t41-${label}-logcat.txt" 2>/dev/null || true
	else
		: >"/tmp/t41-${label}-logcat.txt"
	fi
	{
		date
		uptime
		cat /proc/modules
		if [ -r /proc/jz/clock/clocks ]; then
			echo '# /proc/jz/clock/clocks'
			cat /proc/jz/clock/clocks
		fi
		if [ -r /proc/jz/clock/misc ]; then
			echo '# /proc/jz/clock/misc'
			cat /proc/jz/clock/misc
		fi
		timeout 2 cat /proc/interrupts || true
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
			cat "/proc/$pid/stat" 2>/dev/null || true
			cat "/proc/$pid/wchan" 2>/dev/null || true
		} >>"$proc_diag" 2>&1
	done
	: >"$reg_diag"
	if [ "$label" = "after-consumer" ] && command -v devmem >/dev/null 2>&1; then
		for range in \
			"isp-top 0x13300000 0x110" \
			"isp-stream 0x13301000 0x64" \
			"isp-awb-gain-bank0 0x13304000 0x10" \
			"isp-awb-gain-bank1 0x13305000 0x10" \
			"isp-irq 0x13340000 0xb8" \
			"vic 0x13380000 0x200" \
			"csi 0x10023000 0x30"; do
			set -- $range
			name="$1"
			base=$(( $2 ))
			end=$(( $3 ))
			printf '# %s\n' "$name" >>"$reg_diag"
			off=0
			while [ "$off" -le "$end" ]; do
				addr=$((base + off))
				printf 'off=0x%05x value=' "$off" >>"$reg_diag"
				devmem "$(printf '0x%08x' "$addr")" 32 \
					>>"$reg_diag" 2>&1 || true
				off=$((off + 4))
			done
		done
		# OS04D10 exposure oracle.  Selecting page 1 does not alter streaming
		# state; the three following byte reads expose the integration time and
		# analog gain selected by the stock or recovered AE path.
		if command -v i2cset >/dev/null 2>&1 &&
		   command -v i2cget >/dev/null 2>&1; then
			{
				echo '# os04d10-page1-exposure'
				i2cset -y 0 0x3c 0xfd 0x01 b
				printf 'reg=0x03 integration_hi='; i2cget -y 0 0x3c 0x03 b
				printf 'reg=0x04 integration_lo='; i2cget -y 0 0x3c 0x04 b
				printf 'reg=0x24 analog_gain='; i2cget -y 0 0x3c 0x24 b
			} >>"$reg_diag" 2>&1 || true
		fi
	fi
}

capture_state before
dmesg -c >/tmp/t41-dmesg-before-clear.txt 2>/dev/null || true
set +e
if [ "$recovered_params" = "1" ]; then
	insmod /tmp/tx_isp_t41_recovered.ko tx_isp_bringup_level="$level" \
		t41_checkpoint_ms="$checkpoint_ms" \
		t41_checkpoint_start="$checkpoint_start" \
		>/tmp/t41-insmod.txt 2>&1
else
	insmod /tmp/tx_isp_t41_recovered.ko >/tmp/t41-insmod.txt 2>&1
fi
insmod_status=$?
set -e
printf '%s\n' "$insmod_status" >/tmp/t41-insmod-status.txt
sleep "$smoke_secs"
capture_state after-insmod

sensor_status=skipped
if [ "$insmod_status" -eq 0 ] && [ "$load_sensor" = "1" ]; then
	set +e
	if [ -n "$sensor_module" ]; then
		if [ -n "$sensor_initial_integration" ] ||
		   [ -n "$sensor_initial_again" ]; then
			insmod "$sensor_module" \
				${sensor_initial_integration:+initial_integration="$sensor_initial_integration"} \
				${sensor_initial_again:+initial_again="$sensor_initial_again"} \
				>/tmp/t41-sensor-insmod.txt 2>&1
		else
			insmod "$sensor_module" >/tmp/t41-sensor-insmod.txt 2>&1
		fi
		sensor_status=$?
	else
		: >/tmp/t41-sensor-insmod.txt
		sensor_status=0
	fi
	set -e
else
	: >/tmp/t41-sensor-insmod.txt
fi
printf '%s\n' "$sensor_status" >/tmp/t41-sensor-insmod-status.txt
sleep "$smoke_secs"
capture_state after-sensor

if [ -n "$kernel_trace" ] && [ "$insmod_status" -eq 0 ]; then
	rm -f /tmp/isp-trace.txt /tmp/t41-kernel-trace.txt
	system_read_addr="$(awk '$3 == "system_reg_read" { print "0x" $1; exit }' /proc/kallsyms)"
	trace_memory_args=
	for trace_symbol in tmo_info gamma_info gib_info dmsc_info lsc_info ae_info awb_info statYOut msca_info; do
		trace_addr="$(awk -v symbol="$trace_symbol" \
			'$3 == symbol && $1 != "00000000" { print "0x" $1; exit }' \
			/proc/kallsyms)"
		if [ -n "$trace_addr" ]; then
			if [ "$trace_symbol" = "statYOut" ]; then
				trace_memory_args="$trace_memory_args stat_y_out_addr=$trace_addr"
			else
				trace_memory_args="$trace_memory_args ${trace_symbol}_addr=$trace_addr"
			fi
		fi
	done
	ysp_addr="$(awk '$3 == "ysp_info" && $1 != "00000000" \
		{ print "0x" $1; exit }' /proc/kallsyms)"
	if [ -z "$ysp_addr" ]; then
		# Recovered T41 aliases ysp_info to wdr_info[1].
		wdr_addr="$(awk '$3 == "wdr_info" && $1 != "00000000" \
			{ print "0x" $1; exit }' /proc/kallsyms)"
		if [ -n "$wdr_addr" ]; then
			ysp_addr="$(printf '0x%08x' "$((wdr_addr + 4))")"
		fi
	fi
	if [ -n "$ysp_addr" ]; then
		trace_memory_args="$trace_memory_args ysp_info_addr=$ysp_addr"
	fi
	mscaler_addr="$(awk '$3 == "mscaler" && $1 != "00000000" \
		{ print "0x" $1; exit }' /proc/kallsyms)"
	if [ -n "$mscaler_addr" ]; then
		trace_memory_args="$trace_memory_args mscaler_addr=$mscaler_addr"
	fi
	msca_hardpar_addr="$(awk '$3 == "mscaHardPar" && $1 != "00000000" \
		{ print "0x" $1; exit }' /proc/kallsyms)"
	if [ -z "$msca_hardpar_addr" ]; then
		msca_hardpar_addr="$(awk '$3 == "mscaHardPar_storage" && $1 != "00000000" \
			{ print "0x" $1; exit }' /proc/kallsyms)"
	fi
	if [ -n "$msca_hardpar_addr" ]; then
		trace_memory_args="$trace_memory_args msca_hardpar_addr=$msca_hardpar_addr"
	fi
	set +e
	if [ -n "$system_read_addr" ] && [ "$system_read_addr" != "0x00000000" ]; then
		insmod "$kernel_trace" snapshot_only=1 \
			quality_only="$kernel_trace_quality_only" interval_ms=5000 \
			system_reg_read_addr="$system_read_addr" $trace_memory_args \
			>/tmp/t41-kernel-trace-insmod.txt 2>&1
	else
		printf 'system_reg_read address unavailable\n' \
			>/tmp/t41-kernel-trace-insmod.txt
		false
	fi
	printf '%s\n' "$?" >/tmp/t41-kernel-trace-insmod-status.txt
	set -e
else
	: >/tmp/t41-kernel-trace-insmod.txt
	printf '%s\n' skipped >/tmp/t41-kernel-trace-insmod-status.txt
fi

if [ "$insmod_status" -eq 0 ] && [ "$sensor_status" = "0" ] && \
	[ -n "$imp_fs_probe" ]; then
	chmod 700 "$imp_fs_probe"
	set +e
	if [ -n "$imp_trace" ]; then
		LD_PRELOAD="$imp_trace" timeout 30 "$imp_fs_probe" \
			>/tmp/t41-imp-fs-probe.txt 2>&1
	else
		timeout 30 "$imp_fs_probe" >/tmp/t41-imp-fs-probe.txt 2>&1
	fi
	printf '%s\n' "$?" >/tmp/t41-imp-fs-probe-status.txt
	set -e
	: >/tmp/t41-raptor-start.txt
	: >/tmp/t41-frame-probe.txt
	capture_state after-consumer
elif [ "$insmod_status" -eq 0 ] && [ "$sensor_status" = "0" ] && \
	[ "$start_raptor" = "1" ]; then
	# The init script waits for each daemon to report ready.  Run it in the
	# background so a driver-stalled rvd cannot prevent state capture.
	if [ -n "$raptor_config" ] && [ -n "$imp_trace" ]; then
		LD_PRELOAD="$imp_trace" /usr/bin/rvd -f -d -c "$raptor_config" \
			>/tmp/t41-raptor-start.txt 2>&1 &
	elif [ -n "$raptor_config" ]; then
		/usr/bin/rvd -f -d -c "$raptor_config" \
			>/tmp/t41-raptor-start.txt 2>&1 &
	elif [ -n "$imp_trace" ]; then
		LD_PRELOAD="$imp_trace" sh /etc/init.d/S31raptor start \
			>/tmp/t41-raptor-start.txt 2>&1 &
	else
		sh /etc/init.d/S31raptor start >/tmp/t41-raptor-start.txt 2>&1 &
	fi
	raptor_start_pid=$!
	sleep "$consumer_secs"
	capture_state after-consumer
	if [ -n "$frame_probe" ]; then
		chmod 700 "$frame_probe"
		timeout 5 "$frame_probe" /dev/framechan0 getset \
			>/tmp/t41-frame-probe.txt 2>&1 || true
	else
		: >/tmp/t41-frame-probe.txt
	fi
	if kill -0 "$raptor_start_pid" 2>/dev/null; then
		printf 'raptor init still running: pid=%s\n' "$raptor_start_pid" \
			>>/tmp/t41-raptor-start.txt
		kill "$raptor_start_pid" 2>/dev/null || true
		# rvd does not reliably honor SIGTERM once its encoder poll has
		# stalled.  Do not let a completed smoke test sit until the outer
		# 90-second SSH timeout; retain a short graceful window, then reap it.
		for _stop_try in 1 2 3 4 5 6 7 8 9 10; do
			kill -0 "$raptor_start_pid" 2>/dev/null || break
			usleep 100000
		done
		if kill -0 "$raptor_start_pid" 2>/dev/null; then
			kill -9 "$raptor_start_pid" 2>/dev/null || true
		fi
		wait "$raptor_start_pid" 2>/dev/null || true
	fi
else
	: >/tmp/t41-raptor-start.txt
	: >/tmp/t41-frame-probe.txt
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
if [ -n "$kernel_trace" ]; then
	sleep 1
	rmmod tx_isp_trace >/tmp/t41-kernel-trace-rmmod.txt 2>&1 || true
	cp /tmp/isp-trace.txt /tmp/t41-kernel-trace.txt 2>/dev/null || true
else
	: >/tmp/t41-kernel-trace-rmmod.txt
	: >/tmp/t41-kernel-trace.txt
fi
capture_state after-rmmod
exit 0
EOS
remote_status=$?
set -e
stop_live_log

set +e
timeout 30 "${SSH[@]}" 'cd /tmp && set -- t41-*.txt; [ ! -f t41-frame.nv12 ] || set -- "$@" t41-frame.nv12; tar -cf - "$@"' \
	>"$LOG/t41-logs.tar" 2>"$LOG/t41-log-copy.txt"
log_copy_status=$?
set -e
if [[ "$log_copy_status" == "0" ]]; then
	tar -xf "$LOG/t41-logs.tar" -C "$LOG"
fi

rg -n -i 'kernel panic|kernel bug detected|oops|BUG:|unable to handle|unhandled kernel unaligned access|segfault|watchdog|fatal exception' \
	"$LOG"/*dmesg.txt >"$LOG/kernel-fatal-signatures.txt" || true

insmod_status="$(tr -d '\r\n' <"$LOG/t41-insmod-status.txt" 2>/dev/null || printf missing)"
sensor_status="$(tr -d '\r\n' <"$LOG/t41-sensor-insmod-status.txt" 2>/dev/null || printf missing)"
rmmod_status="$(tr -d '\r\n' <"$LOG/t41-rmmod-status.txt" 2>/dev/null || printf missing)"
fatal_count="$(wc -l <"$LOG/kernel-fatal-signatures.txt")"
printf 'log=%s\n' "$LOG"
printf 'level=%s insmod_status=%s sensor_status=%s rmmod_status=%s kernel_fatal_signatures=%s\n' \
	"$LEVEL" "$insmod_status" "$sensor_status" "$rmmod_status" "$fatal_count"
printf 'remote_status=%s\n' "$remote_status"
printf 'log_copy_status=%s\n' "$log_copy_status"

if [[ "$remote_status" != "0" || "$log_copy_status" != "0" || \
	"$insmod_status" != "0" || "$fatal_count" != "0" ]]; then
	exit 1
fi
