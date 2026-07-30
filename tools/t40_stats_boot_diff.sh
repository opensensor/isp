#!/usr/bin/env bash
set -euo pipefail

HERE="$(cd "$(dirname "$0")/.." && pwd)"
cd "$HERE"

IP="${THINGINO_IP:-192.168.50.242}"
USER="${THINGINO_USER:-root}"
PASS="${THINGINO_PASS:-}"
POWER_IP="${TASMOTA_IP:-192.168.50.103}"
ROOT="${ROOT:-/home/matteius/output/wyze_cam3pro_nor_t40xp_gc4653_rtl8192fs}"
MAX_ATTEMPTS="${MAX_ATTEMPTS:-8}"
BOOT_WAIT_SECS="${BOOT_WAIT_SECS:-55}"
POWER_OFF_SECS="${POWER_OFF_SECS:-4}"
POWER_ON_SETTLE_SECS="${POWER_ON_SETTLE_SECS:-8}"
RUN_ROOT="${1:-logs/$(date +%Y%m%d-%H%M%S)-t40-stats-boot-diff}"

mkdir -p "$RUN_ROOT"

if [[ -n "$PASS" ]]; then
	export SSHPASS="$PASS"
	SSH=(sshpass -e ssh -T -o LogLevel=ERROR \
		-o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null \
		-o ConnectTimeout=5 -o ConnectionAttempts=1 "$USER@$IP")
else
	SSH=(ssh -T -o LogLevel=ERROR -o StrictHostKeyChecking=no \
		-o UserKnownHostsFile=/dev/null -o BatchMode=yes \
		-o ConnectTimeout=5 -o ConnectionAttempts=1 "$USER@$IP")
fi

note() {
	printf '%s %s\n' "$(date +%Y-%m-%dT%H:%M:%S%z)" "$*" | tee -a "$RUN_ROOT/run.log"
}

power_cycle() {
	local tag="$1"

	note "power-cycle $tag off"
	curl -fsS --max-time 5 "http://$POWER_IP/cm?cmnd=Power%20OFF" \
		>"$RUN_ROOT/power-$tag-off.json" 2>"$RUN_ROOT/power-$tag-off.err" || true
	sleep "$POWER_OFF_SECS"
	note "power-cycle $tag on"
	curl -fsS --max-time 5 "http://$POWER_IP/cm?cmnd=Power%20ON" \
		>"$RUN_ROOT/power-$tag-on.json" 2>"$RUN_ROOT/power-$tag-on.err" || true
	sleep "$POWER_ON_SETTLE_SECS"
}

wait_ssh() {
	local waited=0

	while (( waited < BOOT_WAIT_SECS )); do
		if timeout 8 "${SSH[@]}" 'true' >/dev/null 2>&1; then
			note "ssh reachable after ${waited}s"
			return 0
		fi
		sleep 1
		waited=$((waited + 1))
	done
	note "ssh did not become reachable within ${BOOT_WAIT_SECS}s"
	return 1
}

build_once() {
	note "building T40 module and phys_memdump"
	ROOT="$ROOT" SOC=t40 ./build_local.sh >"$RUN_ROOT/build.log" 2>&1
	"$ROOT/host/bin/mipsel-linux-gcc" -Os -Wall -Wextra -static \
		-o tools/phys_memdump.mipsel tools/phys_memdump.c \
		>>"$RUN_ROOT/build.log" 2>&1
}

classify_attempt() {
	local log="$1"
	local haystack=("$log/kmsg.log" "$log/qbuf-lines.txt" "$log/dmesg-full.txt" "$log/dmesg-start-immediate.txt" "$log/dmesg-end.txt" "$log/proc.txt" "$log/proc-post.txt" "$log/fast-core-samples.txt" "$log/wrapper.out" "$log/wrapper.err")

	if grep -E -i -q 'Unhandled kernel|kernel oops|Process rvd|Segmentation fault' "${haystack[@]}" 2>/dev/null; then
		printf 'crash'
		return
	fi
	if ! grep -E -q 'tx_isp_enable_irq: idx=0 irq=39|core-event streamon|STREAMON event' "${haystack[@]}" 2>/dev/null; then
		printf 'no_streamon'
		return
	fi
	if grep -E -q 'TRACE awb_irq|TRACE awb_gain_reg|AEIRQ .*fired|TRACE ae_main_process|TRACE event_process ch=0 event=14' "${haystack[@]}" 2>/dev/null; then
		printf 'live'
		return
	fi
	printf 'dead'
}

summarize_attempt() {
	local attempt="$1"
	local status="$2"
	local log="$3"
	local awb_irq=0
	local awb_gain=0
	local ae_irq=0
	local ae_proc=0
	local streamon=0
	local irq39_enable=0
	local crash=0
	local last_status="NA"
	local irq39="NA"

	grep -E -q 'core-event streamon|STREAMON event' "$log/kmsg.log" "$log/qbuf-lines.txt" "$log/dmesg-start-immediate.txt" "$log/dmesg-end.txt" 2>/dev/null && streamon=1
	grep -E -q 'tx_isp_enable_irq: idx=0 irq=39' "$log/kmsg.log" "$log/qbuf-lines.txt" "$log/dmesg-start-immediate.txt" "$log/dmesg-end.txt" 2>/dev/null && irq39_enable=1
	grep -E -i -q 'Unhandled kernel|kernel oops|Process rvd|Segmentation fault' "$log/kmsg.log" "$log/dmesg-start-immediate.txt" "$log/dmesg-end.txt" "$log/wrapper.out" "$log/wrapper.err" 2>/dev/null && crash=1
	grep -E -q 'TRACE awb_irq' "$log/kmsg.log" 2>/dev/null && awb_irq=1
	grep -E -q 'TRACE awb_gain_reg' "$log/kmsg.log" 2>/dev/null && awb_gain=1
	grep -E -q 'AEIRQ .*fired' "$log/kmsg.log" 2>/dev/null && ae_irq=1
	grep -E -q 'TRACE ae_main_process' "$log/kmsg.log" 2>/dev/null && ae_proc=1
	last_status="$(grep -E '3a-diag core-irq|ISP irq post-dispatch ack|core_irq ' "$log/kmsg.log" "$log/proc.txt" "$log/proc-post.txt" 2>/dev/null | tail -1 | tr '\t' ' ' || true)"
	irq39="$(grep -E '^ *39:' "$log/interrupts-after.txt" 2>/dev/null | tr '\t' ' ' || true)"

	printf '%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n' \
		"$attempt" "$status" "$streamon" "$irq39_enable" "$crash" "$awb_irq" "$awb_gain" "$ae_irq" "$ae_proc" \
		"$irq39" "$last_status" >>"$RUN_ROOT/summary.tsv"
}

run_attempt() {
	local attempt="$1"
	local log="$RUN_ROOT/attempt-$(printf '%02d' "$attempt")"

	mkdir -p "$log"
	note "attempt $attempt start: $log"
	env \
		ROOT="$ROOT" \
		SOC=t40 \
		SKIP_BUILD=1 \
		FAST_RTSP_ONLY=1 \
		FAST_STATS_SNAPSHOT=1 \
		FAST_STATS_SAMPLE_SECS="${FAST_STATS_SAMPLE_SECS:-10}" \
		FAST_STATS_SAMPLE_INTERVAL="${FAST_STATS_SAMPLE_INTERVAL:-1}" \
		REMOTE_PROBE_TIMEOUT_SECS="${REMOTE_PROBE_TIMEOUT_SECS:-300}" \
		RTSP_FIRST=1 \
		SKIP_RTSP="${SKIP_RTSP:-1}" \
		SKIP_QBUF_DUMP=1 \
		SKIP_REG_SNAPSHOT=1 \
		ENABLE_USERSPACE_3A=0 \
		ENABLE_TISP_STREAM_EVENT_INIT="${ENABLE_TISP_STREAM_EVENT_INIT:-1}" \
		ENABLE_TISP_STREAM_EVENT_CBS="${ENABLE_TISP_STREAM_EVENT_CBS:-1}" \
		ENABLE_ISP_3A_DIAG="${ENABLE_ISP_3A_DIAG:-1}" \
		ENABLE_ISP_STATS_FANOUT=1 \
		ENABLE_ISP_BLOCK_INIT=1 \
		ENABLE_ISP_BLOCK_INIT_AE="${ENABLE_ISP_BLOCK_INIT_AE:-0}" \
		ENABLE_ISP_BLOCK_INIT_AWB=1 \
		ENABLE_AWB_REG_WRITES=1 \
		ENABLE_AWB_SET_GAIN="${ENABLE_AWB_SET_GAIN:-0}" \
		AWB_MAIN_INIT_STAGE_LIMIT="${AWB_MAIN_INIT_STAGE_LIMIT:-4}" \
		ENABLE_AWB_GRAYWORLD="${ENABLE_AWB_GRAYWORLD:-0}" \
		ENABLE_AE_SOFT=0 \
		ENABLE_FRAME_3A=0 \
		ENABLE_AE_SENSOR_APPLY=1 \
		AE_SENSOR_APPLY_FORCE_PACKED="${AE_SENSOR_APPLY_FORCE_PACKED:-0}" \
		AE_SENSOR_APPLY_CLEAR_DIRTY=1 \
		AE_SENSOR_APPLY_MAX_AGAIN_INDEX=25 \
		AWB_MANUAL_RGAIN="${AWB_MANUAL_RGAIN:-2172}" \
		AWB_MANUAL_BGAIN="${AWB_MANUAL_BGAIN:-2360}" \
		DNS_GAIN_EV_INIT="${DNS_GAIN_EV_INIT:-98304}" \
		IRQ_FRAME_DONE_DELAY_MS="${IRQ_FRAME_DONE_DELAY_MS:-0}" \
		ADR_LINEAR_MODE="${ADR_LINEAR_MODE:-1}" \
		SENSOR_FULL_WIDTH_OVERRIDE="${SENSOR_FULL_WIDTH_OVERRIDE:-2560}" \
		SENSOR_FULL_HEIGHT_OVERRIDE="${SENSOR_FULL_HEIGHT_OVERRIDE:-1440}" \
		CSI_SETTLE_OVERRIDE="${CSI_SETTLE_OVERRIDE:-0x10}" \
		T40_PROFILE_FORCE_VIC_MDMA_QBUF_RING=0 \
		T40_PROFILE_NO_DIRECT_ADDR_SOURCE=0 \
		ENABLE_MSCA_REARM_GUARD="${ENABLE_MSCA_REARM_GUARD:-1}" \
		MSCA_REARM_GUARD_MAX_SKIPS="${MSCA_REARM_GUARD_MAX_SKIPS:-8}" \
		ENABLE_BCSH="${ENABLE_BCSH:-1}" \
		BCSH_MODE="${BCSH_MODE:-2}" \
		ENABLE_DMSC_STATIC="${ENABLE_DMSC_STATIC:-1}" \
		ENABLE_CLM="${ENABLE_CLM:-1}" \
		CLM_STAGE_LIMIT="${CLM_STAGE_LIMIT:-13}" \
		CLM_DEFER_TRIG="${CLM_DEFER_TRIG:-1}" \
		ENABLE_MDNS="${ENABLE_MDNS:-1}" \
		ENABLE_SDNS="${ENABLE_SDNS:-1}" \
		ENABLE_YDNS="${ENABLE_YDNS:-1}" \
		ENABLE_YSP="${ENABLE_YSP:-1}" \
		ENABLE_CCM="${ENABLE_CCM:-1}" \
		ENABLE_GIB_BLC="${ENABLE_GIB_BLC:-1}" \
		ENABLE_LSC_LIT="${ENABLE_LSC_LIT:-1}" \
		LSC_LIT_CT="${LSC_LIT_CT:-3300}" \
		ENABLE_OEM_ISR_LIT="${ENABLE_OEM_ISR_LIT:-0}" \
		OEM_ISR_LIT_CUT="${OEM_ISR_LIT_CUT:-0}" \
		./tools/t40_safe_qbuf_dump_probe.sh "$log" >"$log/wrapper.out" 2>"$log/wrapper.err" || true

	local status
	status="$(classify_attempt "$log")"
	note "attempt $attempt classified $status"
	summarize_attempt "$attempt" "$status" "$log"
	printf '%s\n' "$status" >"$log/stats-status.txt"
}

make_diffs() {
	local dead="$1"
	local live="$2"

	note "writing live-vs-dead diffs"
	diff -u "$dead/fast-core-regs.txt" "$live/fast-core-regs.txt" \
		>"$RUN_ROOT/dead-vs-live-fast-core-regs.diff" || true
	diff -u "$dead/fast-core-regs-post.txt" "$live/fast-core-regs-post.txt" \
		>"$RUN_ROOT/dead-vs-live-fast-core-regs-post.diff" || true
	diff -u "$dead/fast-core-samples.txt" "$live/fast-core-samples.txt" \
		>"$RUN_ROOT/dead-vs-live-fast-core-samples.diff" || true
	diff -u "$dead/proc.txt" "$live/proc.txt" \
		>"$RUN_ROOT/dead-vs-live-proc.diff" || true
	diff -u "$dead/proc-post.txt" "$live/proc-post.txt" \
		>"$RUN_ROOT/dead-vs-live-proc-post.diff" || true
}

printf 'attempt\tstatus\tstreamon\tirq39_enable\tcrash\tawb_irq\tawb_gain\tae_irq\tae_proc\tirq39\tlast_status\n' >"$RUN_ROOT/summary.tsv"

build_once
power_cycle initial
wait_ssh || true

live_log=""
dead_log=""
for attempt in $(seq 1 "$MAX_ATTEMPTS"); do
	if ! wait_ssh; then
		power_cycle "before-attempt-$attempt"
		if ! wait_ssh; then
			log="$RUN_ROOT/attempt-$(printf '%02d' "$attempt")"
			mkdir -p "$log"
			status="ssh_unreachable"
			note "attempt $attempt skipped: ssh unreachable"
			summarize_attempt "$attempt" "$status" "$log"
			printf '%s\n' "$status" >"$log/stats-status.txt"
			power_cycle "after-attempt-$attempt"
			wait_ssh || true
			continue
		fi
	fi

	run_attempt "$attempt"
	log="$RUN_ROOT/attempt-$(printf '%02d' "$attempt")"
	status="$(cat "$log/stats-status.txt")"
	if [[ "$status" == "live" && -z "$live_log" ]]; then
		live_log="$log"
	fi
	if [[ "$status" == "dead" && -z "$dead_log" ]]; then
		dead_log="$log"
	fi

	if [[ -n "$live_log" && -n "$dead_log" ]]; then
		make_diffs "$dead_log" "$live_log"
		note "captured both classes: live=$live_log dead=$dead_log"
		break
	fi

	power_cycle "after-attempt-$attempt"
	wait_ssh || true
done

if [[ -z "$live_log" || -z "$dead_log" ]]; then
	note "incomplete classes after $MAX_ATTEMPTS attempts: live=${live_log:-none} dead=${dead_log:-none}"
else
	power_cycle final-clean
	wait_ssh || true
fi

note "summary: $RUN_ROOT/summary.tsv"
printf 'log=%s\n' "$RUN_ROOT"
