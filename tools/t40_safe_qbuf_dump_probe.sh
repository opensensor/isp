#!/usr/bin/env bash
set -euo pipefail

IP="${THINGINO_IP:-192.168.50.242}"
USER="${THINGINO_USER:-root}"
PASS="${THINGINO_PASS:-}"
ROOT="${ROOT:-/home/matteius/output/wyze_cam3pro_nor_t40xp_gc4653_rtl8192fs}"
SOC="${SOC:-t40}"
QBUF_PHYS_FALLBACK="${QBUF_PHYS_FALLBACK:-0x6ea8300}"
QBUF_EXTRA_PHYS="${QBUF_EXTRA_PHYS:-0x6bab300 0x6ea8300}"
QBUF_LEN_FALLBACK="${QBUF_LEN_FALLBACK:-0x2fd000}"
SMOKE_SLEEP_SECS="${SMOKE_SLEEP_SECS:-12}"
STOP_RAPTOR_TIMEOUT_SECS="${STOP_RAPTOR_TIMEOUT_SECS:-20}"
SKIP_QBUF_DUMP="${SKIP_QBUF_DUMP:-0}"
SKIP_RTSP="${SKIP_RTSP:-0}"
FRAMECHAN_NEUTRAL_UV_ON_DONE="${FRAMECHAN_NEUTRAL_UV_ON_DONE:-0}"
TISP_MAIN_INIT_TOP40_VALUE="${TISP_MAIN_INIT_TOP40_VALUE:-0x7fdfeeff}"
TISP_MAIN_INIT_CSC_VERSION_VALUE="${TISP_MAIN_INIT_CSC_VERSION_VALUE:-2}"
ENABLE_TISP_MAIN_INIT_COLOR_INITS="${ENABLE_TISP_MAIN_INIT_COLOR_INITS:-0}"
TISP_MAIN_INIT_COLOR_INIT_MASK="${TISP_MAIN_INIT_COLOR_INIT_MASK:-0}"
CSI_SETTLE_OVERRIDE="${CSI_SETTLE_OVERRIDE:-0x1b}"
CORE_BAYER_REG8_VALUE="${CORE_BAYER_REG8_VALUE:-0x10002}"
# Init-latched host-side stock overrides for the diamond/grid artifact. Mask:
# 0x1=MIPI PHY +0x1d4 stock 0x22, 0x2=VIC +0x1e0/+0x1e8/+0x1ec stock 0,
# 0x4=CSI0 +0x128..+0x1bc stock bank. Default off.
T40_STOCK_HOST_INIT_MASK="${T40_STOCK_HOST_INIT_MASK:-0}"
# Stream-order experiment for the diamond/grid artifact. Sequence tracing showed
# recovered OEM remote streamon touching VIC before CSI, while stock brings CSI0
# and VIC up together before later MIPI changes. Set PRE_CSI=1 to direct-start
# CSI before the remote streamon; STAGE_LIMIT maps to csi_direct_stage_limit for
# only that pre-call (3 = CSI ctrl/status only, no MIPI); DELAY waits after it.
OEM_EVENT_PRE_CSI_STREAM="${OEM_EVENT_PRE_CSI_STREAM:-0}"
OEM_EVENT_PRE_CSI_STAGE_LIMIT="${OEM_EVENT_PRE_CSI_STAGE_LIMIT:-0}"
OEM_EVENT_PRE_CSI_DELAY_MS="${OEM_EVENT_PRE_CSI_DELAY_MS:-0}"
# VIC MDMA qbuf-ring output geometry. Defaults of 0 keep the driver's built-in
# behavior (stride=width=0x780, fmt=7). Stock/OEM VIC stride is 0xF00 (width<<1);
# sweep STRIDE_OVERRIDE/CTRL_VALUE/UV_OFFSET together to test the OEM 2-byte/px
# format against the 64-row luma banding (see docs/T40_TUNING_HURDLES.md).
VIC_MDMA_QBUF_RING_STRIDE_OVERRIDE="${VIC_MDMA_QBUF_RING_STRIDE_OVERRIDE:-0}"
VIC_MDMA_QBUF_RING_CTRL_VALUE="${VIC_MDMA_QBUF_RING_CTRL_VALUE:-0}"
VIC_MDMA_QBUF_RING_UV_OFFSET_OVERRIDE="${VIC_MDMA_QBUF_RING_UV_OFFSET_OVERRIDE:-0}"
# Output-engine selection. Default 1 forces the VIC-MDMA qbuf ring (legacy
# behavior, produces the 64-row banded raw-VIC capture). Set to 0 to use the
# OEM-style ISP MSCA channel FIFO + frame-done IRQ path instead (the profile
# wires it up automatically when the ring is not forced). ADDR_SOURCE picks how
# frame-done finds the completed buffer: 0=MSCA FIFO read (OEM), 1=qbuf record,
# 2=MSCA read reg 0x16174. See docs/T40_TUNING_HURDLES.md.
T40_PROFILE_FORCE_VIC_MDMA_QBUF_RING="${T40_PROFILE_FORCE_VIC_MDMA_QBUF_RING:-1}"
T40_PROFILE_NO_DIRECT_ADDR_SOURCE="${T40_PROFILE_NO_DIRECT_ADDR_SOURCE:-0}"
T40_PROFILE_DIRECT_VIC_FEED="${T40_PROFILE_DIRECT_VIC_FEED:-0}"
ADR_LINEAR_MODE="${ADR_LINEAR_MODE:-0}"
SENSOR_FULL_WIDTH_OVERRIDE="${SENSOR_FULL_WIDTH_OVERRIDE:-0}"
SENSOR_FULL_HEIGHT_OVERRIDE="${SENSOR_FULL_HEIGHT_OVERRIDE:-0}"
# Auto-exposure (AE) event callbacks. The bring-up profile starts the tisp event
# thread but does NOT register the AE gain/exposure update callbacks (events
# 6/7/10 = tgain/again update) unless these are on. Without them the GC4653
# exposure/gain over I2C is never driven and the frame stays dark. Set both to 1
# to register the AE callbacks. See docs/T40_TUNING_HURDLES.md.
ENABLE_TISP_STREAM_EVENT_INIT="${ENABLE_TISP_STREAM_EVENT_INIT:-0}"
ENABLE_TISP_STREAM_EVENT_CBS="${ENABLE_TISP_STREAM_EVENT_CBS:-0}"
# ISP 3A software loop bring-up. BLOCK_INIT allocates/registers AE/ADR block
# state; BLOCK_INIT_AE additionally calls AE init (ADR-only when 0). STATS_FANOUT
# dispatches ISP stats IRQ bits into the recovered callbacks.
ENABLE_ISP_3A_DIAG="${ENABLE_ISP_3A_DIAG:-0}"
ENABLE_ISP_STATS_FANOUT="${ENABLE_ISP_STATS_FANOUT:-0}"
ISP_STATS_FANOUT_ADR_STATUS0_MASK="${ISP_STATS_FANOUT_ADR_STATUS0_MASK:-0}"
ENABLE_ADR_PROCESS_WORK="${ENABLE_ADR_PROCESS_WORK:-0}"
ENABLE_ISP_BLOCK_INIT="${ENABLE_ISP_BLOCK_INIT:-0}"
ENABLE_ISP_BLOCK_INIT_AE="${ENABLE_ISP_BLOCK_INIT_AE:-0}"
ISP_BLOCK_INIT_STAGE_LIMIT="${ISP_BLOCK_INIT_STAGE_LIMIT:-0}"
ADR_MAIN_INIT_STAGE_LIMIT="${ADR_MAIN_INIT_STAGE_LIMIT:-0}"
# ADR grid/register push gate. With BLOCK_INIT=1, default 1 mirrors OEM and
# pushes the recomputed ADR grid/register table; set 0 to allocate/register the
# ADR loop but leave hardware ADR registers untouched during bisection.
ENABLE_ADR_REG_WRITES="${ENABLE_ADR_REG_WRITES:-1}"
# Sensor exposure apply bridge. ENABLE_AE_SENSOR_APPLY wires the recovered
# driver's staged AE value to the GC4653 TX_ISP_EVENT_SENSOR_EXPO ioctl from
# frame-done work. AE_SENSOR_APPLY_FORCE_PACKED is a test override:
# packed=(again_index<<16)|integration_time. GC4653 T40 has 26 analog-gain
# LUT entries, so the default max safe index is 25 (e.g. 0x00190760).
# MSCA re-arm guard: stop the scaler overwriting a qbuf while the consumer is
# still reading it (the diagonal ghost/tearing). Default off keeps the known
# MSCA path; set ENABLE_MSCA_REARM_GUARD=1 to forbid two consecutive writes to
# the same buffer (MAX_SKIPS bounds the escape so a held buffer can't freeze).
ENABLE_MSCA_REARM_GUARD="${ENABLE_MSCA_REARM_GUARD:-0}"
MSCA_REARM_GUARD_MAX_SKIPS="${MSCA_REARM_GUARD_MAX_SKIPS:-3}"
# Delay (ms, capped at 20) before frame-done reads the completed-buffer addr and
# delivers it. Tests whether delivery races the MSCA DDR write-complete.
IRQ_FRAME_DONE_DELAY_MS="${IRQ_FRAME_DONE_DELAY_MS:-0}"
ENABLE_AE_SENSOR_APPLY="${ENABLE_AE_SENSOR_APPLY:-0}"
AE_SENSOR_APPLY_FORCE_PACKED="${AE_SENSOR_APPLY_FORCE_PACKED:-0}"
AE_SENSOR_APPLY_CLEAR_DIRTY="${AE_SENSOR_APPLY_CLEAR_DIRTY:-1}"
AE_SENSOR_APPLY_MAX_AGAIN_INDEX="${AE_SENSOR_APPLY_MAX_AGAIN_INDEX:-25}"
AE_SENSOR_APPLY_LOG_SKIPS="${AE_SENSOR_APPLY_LOG_SKIPS:-0}"
LOG="${1:-logs/$(date +%Y%m%d-%H%M%S)-t40-safe-qbuf-dump-242}"

if [[ "$IP" != "192.168.50.242" ]]; then
	echo "refusing non-target IP: $IP" >&2
	exit 2
fi
if [[ "$FRAMECHAN_NEUTRAL_UV_ON_DONE" != "0" &&
	"$FRAMECHAN_NEUTRAL_UV_ON_DONE" != "1" ]]; then
	echo "FRAMECHAN_NEUTRAL_UV_ON_DONE must be 0 or 1" >&2
	exit 2
fi
if [[ "$ENABLE_TISP_MAIN_INIT_COLOR_INITS" != "0" &&
	"$ENABLE_TISP_MAIN_INIT_COLOR_INITS" != "1" ]]; then
	echo "ENABLE_TISP_MAIN_INIT_COLOR_INITS must be 0 or 1" >&2
	exit 2
fi
for numeric in TISP_MAIN_INIT_TOP40_VALUE TISP_MAIN_INIT_CSC_VERSION_VALUE \
	SMOKE_SLEEP_SECS STOP_RAPTOR_TIMEOUT_SECS SKIP_QBUF_DUMP SKIP_RTSP \
	TISP_MAIN_INIT_COLOR_INIT_MASK CSI_SETTLE_OVERRIDE \
	T40_STOCK_HOST_INIT_MASK OEM_EVENT_PRE_CSI_STREAM \
	OEM_EVENT_PRE_CSI_STAGE_LIMIT OEM_EVENT_PRE_CSI_DELAY_MS \
	CORE_BAYER_REG8_VALUE VIC_MDMA_QBUF_RING_STRIDE_OVERRIDE \
		VIC_MDMA_QBUF_RING_CTRL_VALUE VIC_MDMA_QBUF_RING_UV_OFFSET_OVERRIDE \
		T40_PROFILE_FORCE_VIC_MDMA_QBUF_RING T40_PROFILE_NO_DIRECT_ADDR_SOURCE \
		T40_PROFILE_DIRECT_VIC_FEED \
		ADR_LINEAR_MODE \
		SENSOR_FULL_WIDTH_OVERRIDE SENSOR_FULL_HEIGHT_OVERRIDE \
		ENABLE_TISP_STREAM_EVENT_INIT ENABLE_TISP_STREAM_EVENT_CBS \
	ENABLE_ISP_3A_DIAG ENABLE_ISP_STATS_FANOUT \
	ISP_STATS_FANOUT_ADR_STATUS0_MASK \
	ENABLE_ADR_PROCESS_WORK \
	ENABLE_ISP_BLOCK_INIT ENABLE_ISP_BLOCK_INIT_AE \
	ISP_BLOCK_INIT_STAGE_LIMIT ADR_MAIN_INIT_STAGE_LIMIT \
	ENABLE_ADR_REG_WRITES \
	ENABLE_MSCA_REARM_GUARD MSCA_REARM_GUARD_MAX_SKIPS IRQ_FRAME_DONE_DELAY_MS \
	ENABLE_AE_SENSOR_APPLY \
	AE_SENSOR_APPLY_FORCE_PACKED \
	AE_SENSOR_APPLY_CLEAR_DIRTY AE_SENSOR_APPLY_MAX_AGAIN_INDEX \
	AE_SENSOR_APPLY_LOG_SKIPS; do
	if [[ ! "${!numeric}" =~ ^(0x[0-9a-fA-F]+|[0-9]+)$ ]]; then
		echo "$numeric must be decimal or hex" >&2
		exit 2
	fi
done
if [[ -n "$PASS" ]]; then
	SSH=(sshpass -p "$PASS" ssh -T -o LogLevel=ERROR \
		-o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null \
		-o ConnectTimeout=5 "$USER@$IP")
	SCP=(sshpass -p "$PASS" scp -O -q -o LogLevel=ERROR \
		-o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null)
else
	SSH=(ssh -T -o LogLevel=ERROR -o StrictHostKeyChecking=no \
		-o UserKnownHostsFile=/dev/null -o ConnectTimeout=5 "$USER@$IP")
	SCP=(scp -O -q -o LogLevel=ERROR -o StrictHostKeyChecking=no \
		-o UserKnownHostsFile=/dev/null)
fi

mkdir -p "$LOG"

ROOT="$ROOT" SOC="$SOC" ./build_local.sh
"$ROOT/host/bin/mipsel-linux-gcc" -Os -Wall -Wextra -static \
	-o tools/phys_memdump.mipsel tools/phys_memdump.c

"${SCP[@]}" driver/t40/tx_isp_t40_recovered.ko \
	"$USER@$IP:/tmp/tx_isp_t40_recovered.ko"
"${SCP[@]}" tools/phys_memdump.mipsel "$USER@$IP:/tmp/phys_memdump"

"${SSH[@]}" \
	"SMOKE_SLEEP_SECS=$SMOKE_SLEEP_SECS" \
	"STOP_RAPTOR_TIMEOUT_SECS=$STOP_RAPTOR_TIMEOUT_SECS" \
	"FRAMECHAN_NEUTRAL_UV_ON_DONE=$FRAMECHAN_NEUTRAL_UV_ON_DONE" \
	"TISP_MAIN_INIT_TOP40_VALUE=$TISP_MAIN_INIT_TOP40_VALUE" \
	"TISP_MAIN_INIT_CSC_VERSION_VALUE=$TISP_MAIN_INIT_CSC_VERSION_VALUE" \
	"ENABLE_TISP_MAIN_INIT_COLOR_INITS=$ENABLE_TISP_MAIN_INIT_COLOR_INITS" \
	"TISP_MAIN_INIT_COLOR_INIT_MASK=$TISP_MAIN_INIT_COLOR_INIT_MASK" \
	"CSI_SETTLE_OVERRIDE=$CSI_SETTLE_OVERRIDE" \
	"T40_STOCK_HOST_INIT_MASK=$T40_STOCK_HOST_INIT_MASK" \
	"OEM_EVENT_PRE_CSI_STREAM=$OEM_EVENT_PRE_CSI_STREAM" \
	"OEM_EVENT_PRE_CSI_STAGE_LIMIT=$OEM_EVENT_PRE_CSI_STAGE_LIMIT" \
	"OEM_EVENT_PRE_CSI_DELAY_MS=$OEM_EVENT_PRE_CSI_DELAY_MS" \
	"CORE_BAYER_REG8_VALUE=$CORE_BAYER_REG8_VALUE" \
	"VIC_MDMA_QBUF_RING_STRIDE_OVERRIDE=$VIC_MDMA_QBUF_RING_STRIDE_OVERRIDE" \
	"VIC_MDMA_QBUF_RING_CTRL_VALUE=$VIC_MDMA_QBUF_RING_CTRL_VALUE" \
	"VIC_MDMA_QBUF_RING_UV_OFFSET_OVERRIDE=$VIC_MDMA_QBUF_RING_UV_OFFSET_OVERRIDE" \
	"T40_PROFILE_FORCE_VIC_MDMA_QBUF_RING=$T40_PROFILE_FORCE_VIC_MDMA_QBUF_RING" \
	"T40_PROFILE_NO_DIRECT_ADDR_SOURCE=$T40_PROFILE_NO_DIRECT_ADDR_SOURCE" \
	"T40_PROFILE_DIRECT_VIC_FEED=$T40_PROFILE_DIRECT_VIC_FEED" \
	"ADR_LINEAR_MODE=$ADR_LINEAR_MODE" \
	"SENSOR_FULL_WIDTH_OVERRIDE=$SENSOR_FULL_WIDTH_OVERRIDE" \
	"SENSOR_FULL_HEIGHT_OVERRIDE=$SENSOR_FULL_HEIGHT_OVERRIDE" \
	"ENABLE_TISP_STREAM_EVENT_INIT=$ENABLE_TISP_STREAM_EVENT_INIT" \
	"ENABLE_TISP_STREAM_EVENT_CBS=$ENABLE_TISP_STREAM_EVENT_CBS" \
	"ENABLE_ISP_3A_DIAG=$ENABLE_ISP_3A_DIAG" \
	"ENABLE_ISP_STATS_FANOUT=$ENABLE_ISP_STATS_FANOUT" \
	"ISP_STATS_FANOUT_ADR_STATUS0_MASK=$ISP_STATS_FANOUT_ADR_STATUS0_MASK" \
	"ENABLE_ADR_PROCESS_WORK=$ENABLE_ADR_PROCESS_WORK" \
	"ENABLE_ISP_BLOCK_INIT=$ENABLE_ISP_BLOCK_INIT" \
	"ENABLE_ISP_BLOCK_INIT_AE=$ENABLE_ISP_BLOCK_INIT_AE" \
	"ISP_BLOCK_INIT_STAGE_LIMIT=$ISP_BLOCK_INIT_STAGE_LIMIT" \
	"ADR_MAIN_INIT_STAGE_LIMIT=$ADR_MAIN_INIT_STAGE_LIMIT" \
	"ENABLE_ADR_REG_WRITES=$ENABLE_ADR_REG_WRITES" \
	"ENABLE_MSCA_REARM_GUARD=$ENABLE_MSCA_REARM_GUARD" \
	"MSCA_REARM_GUARD_MAX_SKIPS=$MSCA_REARM_GUARD_MAX_SKIPS" \
	"IRQ_FRAME_DONE_DELAY_MS=$IRQ_FRAME_DONE_DELAY_MS" \
	"ENABLE_AE_SENSOR_APPLY=$ENABLE_AE_SENSOR_APPLY" \
	"AE_SENSOR_APPLY_FORCE_PACKED=$AE_SENSOR_APPLY_FORCE_PACKED" \
	"AE_SENSOR_APPLY_CLEAR_DIRTY=$AE_SENSOR_APPLY_CLEAR_DIRTY" \
	"AE_SENSOR_APPLY_MAX_AGAIN_INDEX=$AE_SENSOR_APPLY_MAX_AGAIN_INDEX" \
	"AE_SENSOR_APPLY_LOG_SKIPS=$AE_SENSOR_APPLY_LOG_SKIPS" \
	sh -s >"$LOG/load-safe.log" 2>&1 <<'EOS'
set -x
: "${FRAMECHAN_NEUTRAL_UV_ON_DONE:=0}"
: "${SMOKE_SLEEP_SECS:=12}"
: "${STOP_RAPTOR_TIMEOUT_SECS:=20}"
: "${TISP_MAIN_INIT_TOP40_VALUE:=0x7fdfeeff}"
: "${TISP_MAIN_INIT_CSC_VERSION_VALUE:=2}"
: "${ENABLE_TISP_MAIN_INIT_COLOR_INITS:=0}"
: "${TISP_MAIN_INIT_COLOR_INIT_MASK:=0}"
: "${CSI_SETTLE_OVERRIDE:=0x1b}"
: "${T40_STOCK_HOST_INIT_MASK:=0}"
: "${OEM_EVENT_PRE_CSI_STREAM:=0}"
: "${OEM_EVENT_PRE_CSI_STAGE_LIMIT:=0}"
: "${OEM_EVENT_PRE_CSI_DELAY_MS:=0}"
: "${CORE_BAYER_REG8_VALUE:=0x10002}"
: "${VIC_MDMA_QBUF_RING_STRIDE_OVERRIDE:=0}"
: "${VIC_MDMA_QBUF_RING_CTRL_VALUE:=0}"
: "${VIC_MDMA_QBUF_RING_UV_OFFSET_OVERRIDE:=0}"
: "${T40_PROFILE_FORCE_VIC_MDMA_QBUF_RING:=1}"
: "${T40_PROFILE_NO_DIRECT_ADDR_SOURCE:=0}"
: "${T40_PROFILE_DIRECT_VIC_FEED:=0}"
: "${ADR_LINEAR_MODE:=0}"
: "${SENSOR_FULL_WIDTH_OVERRIDE:=0}"
: "${SENSOR_FULL_HEIGHT_OVERRIDE:=0}"
: "${ENABLE_TISP_STREAM_EVENT_INIT:=0}"
: "${ENABLE_TISP_STREAM_EVENT_CBS:=0}"
: "${ENABLE_ISP_3A_DIAG:=0}"
: "${ENABLE_ISP_STATS_FANOUT:=0}"
: "${ISP_STATS_FANOUT_ADR_STATUS0_MASK:=0}"
: "${ENABLE_ADR_PROCESS_WORK:=0}"
: "${ENABLE_ISP_BLOCK_INIT:=0}"
: "${ENABLE_ISP_BLOCK_INIT_AE:=0}"
: "${ISP_BLOCK_INIT_STAGE_LIMIT:=0}"
: "${ADR_MAIN_INIT_STAGE_LIMIT:=0}"
: "${ENABLE_ADR_REG_WRITES:=1}"
: "${ENABLE_MSCA_REARM_GUARD:=0}"
: "${MSCA_REARM_GUARD_MAX_SKIPS:=3}"
: "${IRQ_FRAME_DONE_DELAY_MS:=0}"
: "${ENABLE_AE_SENSOR_APPLY:=0}"
: "${AE_SENSOR_APPLY_FORCE_PACKED:=0}"
: "${AE_SENSOR_APPLY_CLEAR_DIRTY:=1}"
: "${AE_SENSOR_APPLY_MAX_AGAIN_INDEX:=25}"
: "${AE_SENSOR_APPLY_LOG_SKIPS:=0}"
raptor_stop_pid=
/etc/init.d/S31raptor stop &
raptor_stop_pid=$!
raptor_stop_elapsed=0
while kill -0 "$raptor_stop_pid" 2>/dev/null; do
	if [ "$raptor_stop_elapsed" -ge "$STOP_RAPTOR_TIMEOUT_SECS" ]; then
		echo "S31raptor stop timed out after ${STOP_RAPTOR_TIMEOUT_SECS}s; killing stop helper" >&2
		kill -9 "$raptor_stop_pid" 2>/dev/null || true
		break
	fi
	sleep 1
	raptor_stop_elapsed=$((raptor_stop_elapsed + 1))
done
wait "$raptor_stop_pid" 2>/dev/null || true
killall -9 rvd rad rod rsd rhd ric rwd 2>/dev/null || true
rm -f /var/run/rss/*.pid /var/run/rss/*.sock \
	/dev/shm/rss_ring_* /dev/shm/rss_osd_* 2>/dev/null || true
sleep 1
rmmod sensor_gc4653_t40 2>/tmp/rmmod-sensor.err || true
cat /tmp/rmmod-sensor.err || true
rmmod tx_isp_t40_recovered 2>/tmp/rmmod-recovered.err || true
cat /tmp/rmmod-recovered.err || true
rmmod tx_isp_t40 2>/tmp/rmmod-stock.err || true
cat /tmp/rmmod-stock.err || true
dmesg -c > /tmp/t40-dmesg-before-load.txt 2>/dev/null || true
insmod /tmp/tx_isp_t40_recovered.ko \
	t40_bringup_profile=1 \
	t40_profile_direct_vic_feed="$T40_PROFILE_DIRECT_VIC_FEED" \
	t40_profile_no_direct_irq_defaults=1 \
	t40_profile_isp_irq_passthrough=1 \
	t40_profile_force_vic_mdma_qbuf_ring="$T40_PROFILE_FORCE_VIC_MDMA_QBUF_RING" \
	t40_profile_no_direct_addr_source="$T40_PROFILE_NO_DIRECT_ADDR_SOURCE" \
	adr_linear_mode="$ADR_LINEAR_MODE" \
	sensor_full_width_override="$SENSOR_FULL_WIDTH_OVERRIDE" \
	sensor_full_height_override="$SENSOR_FULL_HEIGHT_OVERRIDE" \
	enable_tisp_stream_event_init="$ENABLE_TISP_STREAM_EVENT_INIT" \
	enable_tisp_stream_event_cbs="$ENABLE_TISP_STREAM_EVENT_CBS" \
	enable_isp_3a_diag="$ENABLE_ISP_3A_DIAG" \
	enable_isp_stats_fanout="$ENABLE_ISP_STATS_FANOUT" \
	isp_stats_fanout_adr_status0_mask="$ISP_STATS_FANOUT_ADR_STATUS0_MASK" \
	enable_adr_process_work="$ENABLE_ADR_PROCESS_WORK" \
	enable_isp_block_init="$ENABLE_ISP_BLOCK_INIT" \
	enable_isp_block_init_ae="$ENABLE_ISP_BLOCK_INIT_AE" \
	isp_block_init_stage_limit="$ISP_BLOCK_INIT_STAGE_LIMIT" \
	adr_main_init_stage_limit="$ADR_MAIN_INIT_STAGE_LIMIT" \
	enable_adr_reg_writes="$ENABLE_ADR_REG_WRITES" \
	enable_msca_rearm_guard="$ENABLE_MSCA_REARM_GUARD" \
	msca_rearm_guard_max_skips="$MSCA_REARM_GUARD_MAX_SKIPS" \
	irq_frame_done_delay_ms="$IRQ_FRAME_DONE_DELAY_MS" \
	enable_ae_sensor_apply="$ENABLE_AE_SENSOR_APPLY" \
	ae_sensor_apply_force_packed="$AE_SENSOR_APPLY_FORCE_PACKED" \
	ae_sensor_apply_clear_dirty="$AE_SENSOR_APPLY_CLEAR_DIRTY" \
	ae_sensor_apply_max_again_index="$AE_SENSOR_APPLY_MAX_AGAIN_INDEX" \
	ae_sensor_apply_log_skips="$AE_SENSOR_APPLY_LOG_SKIPS" \
	force_core_bayer_reg8_value=1 \
	core_bayer_reg8_value="$CORE_BAYER_REG8_VALUE" \
	tisp_main_init_reg88_override=0xffffffff \
	enable_tisp_main_init_color_inits="$ENABLE_TISP_MAIN_INIT_COLOR_INITS" \
	tisp_main_init_color_init_mask="$TISP_MAIN_INIT_COLOR_INIT_MASK" \
	force_tisp_main_init_yuv_input_csc_version=0 \
	csi_settle_override="$CSI_SETTLE_OVERRIDE" \
	t40_stock_host_init_mask="$T40_STOCK_HOST_INIT_MASK" \
	oem_event_pre_csi_stream="$OEM_EVENT_PRE_CSI_STREAM" \
	oem_event_pre_csi_stage_limit="$OEM_EVENT_PRE_CSI_STAGE_LIMIT" \
	oem_event_pre_csi_delay_ms="$OEM_EVENT_PRE_CSI_DELAY_MS" \
	vic_mdma_qbuf_ring_stride_override="$VIC_MDMA_QBUF_RING_STRIDE_OVERRIDE" \
	vic_mdma_qbuf_ring_ctrl_value="$VIC_MDMA_QBUF_RING_CTRL_VALUE" \
	vic_mdma_qbuf_ring_uv_offset_override="$VIC_MDMA_QBUF_RING_UV_OFFSET_OVERRIDE" \
	framechan_neutral_uv_on_done="$FRAMECHAN_NEUTRAL_UV_ON_DONE"
PARAM=/sys/module/tx_isp_t40_recovered/parameters
echo "$TISP_MAIN_INIT_TOP40_VALUE" > "$PARAM/tisp_main_init_top40_value"
echo "$TISP_MAIN_INIT_CSC_VERSION_VALUE" > "$PARAM/tisp_main_init_csc_version_value"
insmod /lib/modules/4.4.94/ingenic/sensor_gc4653_t40.ko
/etc/init.d/S31raptor start
dmesg | grep -E 'isp-block-init|3a-diag|ADR|adr|irq_func_cb|stats fanout|OEM event pre-CSI|CSI direct stage-limit|CSI direct start|direct VIC streamon|OEM event streamon|core-event streamon|CSI direct|settle|phy complete|vic_start_diag' > /tmp/t40-start-immediate-lines.txt 2>/dev/null || true
dmesg | tail -260 > /tmp/t40-dmesg-start-immediate.txt
hold_secs=$((SMOKE_SLEEP_SECS))
if [ "$hold_secs" -gt 4 ]; then
	sleep 4
	cat /proc/interrupts > /tmp/t40-interrupts-start.txt
	dmesg | grep -E 'isp-block-init|3a-diag|ADR|adr|irq_func_cb|stats fanout|OEM event pre-CSI|CSI direct stage-limit|CSI direct start|direct VIC streamon|OEM event streamon|core-event streamon|CSI direct|settle|phy complete|vic_start_diag' > /tmp/t40-start-lines.txt 2>/dev/null || true
	dmesg | tail -220 > /tmp/t40-dmesg-start.txt
	sleep $((hold_secs - 4))
else
	sleep "$hold_secs"
	cat /proc/interrupts > /tmp/t40-interrupts-start.txt
	dmesg | grep -E 'isp-block-init|3a-diag|ADR|adr|irq_func_cb|stats fanout|OEM event pre-CSI|CSI direct stage-limit|CSI direct start|direct VIC streamon|OEM event streamon|core-event streamon|CSI direct|settle|phy complete|vic_start_diag' > /tmp/t40-start-lines.txt 2>/dev/null || true
	dmesg | tail -220 > /tmp/t40-dmesg-start.txt
fi
chmod +x /tmp/phys_memdump
cat /proc/interrupts | grep -E '(^ *3[89]:|tx|isp|vic)' || true
cat /proc/interrupts > /tmp/t40-interrupts-after.txt
{
	echo "# GC4653 AE regs $(date)"
	read_reg16() {
		local reg="$1"
		local hi lo
		hi=$(( (reg >> 8) & 255 ))
		lo=$(( reg & 255 ))
		i2ctransfer -f -y 1 w2@0x29 \
			"$(printf '0x%02x' "$hi")" "$(printf '0x%02x' "$lo")" \
			r1 2>/dev/null || printf 'ERR'
	}
	for pass in 1 2 3 4 5; do
		printf 'pass=%s ' "$pass"
		for reg in 0x0202 0x0203 0x0205 0x0218 0x0219 0x0340 0x0341; do
			printf '%s=' "$reg"
			read_reg16 "$reg"
			printf ' '
		done
		printf '\n'
		sleep 2
	done
} > /tmp/t40-sensor-ae-regs.txt 2>&1 || true
set +x
if command -v devmem >/dev/null 2>&1; then
	{
		echo "# T40 recovered CSI/VIC snapshot $(date)"
		echo "# csi0 regs0 phys 0x10054000"
		for off in 0x04 0x08 0x0c 0x10 0x28 0x2c 0x80 0x100; do
			addr=$((0x10054000 + off))
			hex="$(printf '0x%08x' "$addr")"
			printf '%s ' "$hex"
			devmem "$hex" 32 || true
		done
		echo "# csi1/w01 regs1 phys 0x10023000"
		for off in 0x04 0x08 0x0c 0x10 0x14 0x28 0x2c 0x40 0x44 0x48 0x80 0x100 0x160 0x1e0 0x260; do
			addr=$((0x10023000 + off))
			hex="$(printf '0x%08x' "$addr")"
			printf '%s ' "$hex"
			devmem "$hex" 32 || true
		done
		echo "# mipi phy phys 0x10022000"
		for off in 0x00 0x80 0x128 0x160 0x1a8 0x1e0 0x228 0x260 0x2a8 0x2e0 0x328 0x360 0x3a8; do
			addr=$((0x10022000 + off))
			hex="$(printf '0x%08x' "$addr")"
			printf '%s ' "$hex"
			devmem "$hex" 32 || true
		done
		echo "# vic primary phys 0x13380000"
		for off in 0x00 0x04 0x0c 0x14 0x100 0x104 0x108 0x10c 0x110 0x1e0 0x1e4 0x1e8 0x1ec 0x300 0x304 0x308 0x310 0x314 0x318 0x31c 0x320 0x324 0x328 0x32c 0x330 0x334 0x338 0x33c 0x340 0x344 0x348 0x34c 0x350 0x370 0x380 0x3a8; do
			addr=$((0x13380000 + off))
			hex="$(printf '0x%08x' "$addr")"
			printf '%s ' "$hex"
			devmem "$hex" 32 || true
		done
		# ISP-core MSCA channel-output geometry phys 0x13300000.
		# OEM reference (docs/T40_TUNING_HURDLES.md): 0x13316100=0x07800438
		# (ch0 out 1920x1080), 0x13316180=0x780 (Y stride), 0x13316198=0x780
		# (UV stride). Diffing these against the recovered driver localizes the
		# 4-strip (272-line) diagonal shear in the MSCA FIFO path.
		echo "# isp-core msca window phys 0x13300000 (window regs)"
		for off in 0x16064 0x16080 0x16084 0x160a0 0x160a4 0x160a8 0x160ac 0x160b0 0x160b4; do
			addr=$((0x13300000 + off))
			hex="$(printf '0x%08x' "$addr")"
			printf '%s ' "$hex"
			devmem "$hex" 32 || true
		done
		for ch in 0 1 2 3; do
			bank=$((ch * 0x100))
			echo "# isp-core msca ch$ch geometry (out-size/stride)"
			for off in 0x16100 0x16104 0x16128 0x1612c 0x16168 0x16180 0x16198; do
				addr=$((0x13300000 + off + bank))
				hex="$(printf '0x%08x' "$addr")"
				printf '%s ' "$hex"
				devmem "$hex" 32 || true
			done
		done
	} > /tmp/t40-csi-vic-regs.txt 2>&1
else
	echo "devmem not found" > /tmp/t40-csi-vic-regs.txt
fi
set -x
dmesg | grep -E 'framechan0 (repaired )?qbuf|VIC frame MDMA qbuf ring|irq frame-done|frame.?done|msca|MSCA|fifo|FIFO|addr_fifo|bring-up profile|stock-host override|OEM event pre-CSI|isp-block-init|ADR|adr|3a-diag|stats fanout|irq_func_cb|TISP stream event|rearm-guard|AE sensor apply|tgain|again|event setup|sensor_ioctl|CSI direct|csi_|settle|phy complete|vic_start_diag' | tail -380 > /tmp/t40-qbuf-lines.txt
dmesg | tail -260 > /tmp/t40-dmesg-tail.txt
dmesg > /tmp/t40-dmesg-full.txt
EOS

"${SCP[@]}" "$USER@$IP:/tmp/t40-interrupts-after.txt" "$LOG/interrupts-after.txt"
"${SCP[@]}" "$USER@$IP:/tmp/t40-interrupts-start.txt" "$LOG/interrupts-start.txt"
"${SCP[@]}" "$USER@$IP:/tmp/t40-start-immediate-lines.txt" "$LOG/start-immediate-lines.txt"
"${SCP[@]}" "$USER@$IP:/tmp/t40-dmesg-start-immediate.txt" "$LOG/dmesg-start-immediate.txt"
"${SCP[@]}" "$USER@$IP:/tmp/t40-start-lines.txt" "$LOG/start-lines.txt"
"${SCP[@]}" "$USER@$IP:/tmp/t40-dmesg-start.txt" "$LOG/dmesg-start.txt"
"${SCP[@]}" "$USER@$IP:/tmp/t40-qbuf-lines.txt" "$LOG/qbuf-lines.txt"
"${SCP[@]}" "$USER@$IP:/tmp/t40-csi-vic-regs.txt" "$LOG/csi-vic-regs.txt"
"${SCP[@]}" "$USER@$IP:/tmp/t40-dmesg-tail.txt" "$LOG/dmesg-tail.txt"
"${SCP[@]}" "$USER@$IP:/tmp/t40-dmesg-full.txt" "$LOG/dmesg-full.txt"
"${SCP[@]}" "$USER@$IP:/tmp/t40-dmesg-before-load.txt" "$LOG/dmesg-before-load.txt"
"${SCP[@]}" "$USER@$IP:/tmp/t40-sensor-ae-regs.txt" "$LOG/sensor-ae-regs.txt"

qline="$(grep -m1 'framechan0 repaired qbuf' "$LOG/qbuf-lines.txt" || true)"
if [[ -z "$qline" ]]; then
	qline="$(grep -m1 'framechan0 qbuf' "$LOG/qbuf-lines.txt" || true)"
fi
if [[ -z "$qline" ]]; then
	qline="$(grep -m1 'VIC frame MDMA qbuf ring' "$LOG/qbuf-lines.txt" || true)"
fi
phys="$(sed -n 's/.*qphys=0x\([0-9a-fA-F][0-9a-fA-F]*\).*/0x\1/p' <<<"$qline")"
if [[ -z "$phys" ]]; then
	phys="$(sed -n 's/.*phys=0x\([0-9a-fA-F][0-9a-fA-F]*\).*/0x\1/p' <<<"$qline")"
fi
if [[ -z "$phys" ]]; then
	phys="$(sed -n 's/.*y0=0x\([0-9a-fA-F][0-9a-fA-F]*\).*/0x\1/p' <<<"$qline")"
fi
len="$(sed -n 's/.*qlen=0x\([0-9a-fA-F][0-9a-fA-F]*\).*/0x\1/p' <<<"$qline")"
if [[ -z "$len" ]]; then
	len="$(sed -n 's/.*len=0x\([0-9a-fA-F][0-9a-fA-F]*\).*/0x\1/p' <<<"$qline")"
fi
len="${len:-$QBUF_LEN_FALLBACK}"

if [[ -z "$phys" ]]; then
	phys="$QBUF_PHYS_FALLBACK"
	printf 'warning: using qbuf phys fallback %s; no parseable dmesg qbuf line\n' \
		"$phys" | tee "$LOG/qbuf-fallback.txt" >&2
fi

printf 'phys=%s len=%s\n' "$phys" "$len" | tee "$LOG/qbuf-dump.txt"

if [[ "$SKIP_QBUF_DUMP" == "1" ]]; then
	printf 'skipping qbuf dump (SKIP_QBUF_DUMP=1)\n' | tee -a "$LOG/qbuf-dump.txt"
else
	name=qbuf-ch0
	"${SSH[@]}" "/tmp/phys_memdump '$phys' '$len' '/tmp/$name.bin'" \
		>"$LOG/dump-$name.log" 2>&1
	"${SCP[@]}" "$USER@$IP:/tmp/$name.bin" "$LOG/$name.bin"
	python3 tools/nv12_probe.py "$LOG/$name.bin" \
		--width 1920 --height 1080 --out-dir "$LOG/$name-renders" \
		>"$LOG/nv12-probe-$name.log"
	cp "$LOG/dump-qbuf-ch0.log" "$LOG/dump-qbuf.log"
	cp "$LOG/nv12-probe-qbuf-ch0.log" "$LOG/nv12-probe.log"
	ln -sfn qbuf-ch0-renders "$LOG/qbuf-renders"

	for extra_phys in $QBUF_EXTRA_PHYS; do
		[[ "$extra_phys" == "$phys" ]] && continue
		name="qbuf-${extra_phys//[^A-Za-z0-9]/_}"
		printf 'extra_phys=%s len=%s name=%s\n' \
			"$extra_phys" "$len" "$name" | tee -a "$LOG/qbuf-dump.txt"
		"${SSH[@]}" "/tmp/phys_memdump '$extra_phys' '$len' '/tmp/$name.bin'" \
			>"$LOG/dump-$name.log" 2>&1
		"${SCP[@]}" "$USER@$IP:/tmp/$name.bin" "$LOG/$name.bin"
		python3 tools/nv12_probe.py "$LOG/$name.bin" \
			--width 1920 --height 1080 --out-dir "$LOG/$name-renders" \
			>"$LOG/nv12-probe-$name.log"
	done
fi

if [[ "$SKIP_RTSP" == "1" ]]; then
	printf 'skipping RTSP snapshot (SKIP_RTSP=1)\n' >"$LOG/ffmpeg.log"
else
	timeout 25 ffmpeg -hide_banner -loglevel info -y -rtsp_transport tcp \
		-i "rtsp://thingino:thingino@$IP/ch0" -frames:v 1 \
		"$LOG/rtsp-frame.jpg" >"$LOG/ffmpeg.log" 2>&1 || true
fi

printf 'log=%s\n' "$LOG"
sed -n '1,20p' "$LOG/qbuf-dump.txt"
if [[ -f "$LOG/nv12-probe.log" ]]; then
	sed -n '1,20p' "$LOG/nv12-probe.log"
fi
