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
ALLOW_ACTIVE_STREAM_STOP="${ALLOW_ACTIVE_STREAM_STOP:-0}"
SKIP_BUILD="${SKIP_BUILD:-0}"
SKIP_QBUF_DUMP="${SKIP_QBUF_DUMP:-0}"
SKIP_RTSP="${SKIP_RTSP:-0}"
SKIP_REG_SNAPSHOT="${SKIP_REG_SNAPSHOT:-0}"
RTSP_FIRST="${RTSP_FIRST:-0}"
RTSP_TIMEOUT_SECS="${RTSP_TIMEOUT_SECS:-25}"
FAST_RTSP_ONLY="${FAST_RTSP_ONLY:-0}"
FAST_STATS_SNAPSHOT="${FAST_STATS_SNAPSHOT:-0}"
FAST_STATS_SAMPLE_SECS="${FAST_STATS_SAMPLE_SECS:-6}"
FAST_STATS_SAMPLE_INTERVAL="${FAST_STATS_SAMPLE_INTERVAL:-1}"
REMOTE_PROBE_TIMEOUT_SECS="${REMOTE_PROBE_TIMEOUT_SECS:-300}"
PRESTREAM_EXPO_IT="${PRESTREAM_EXPO_IT:-0}"
AWB_MANUAL_RGAIN="${AWB_MANUAL_RGAIN:-0}"
AWB_MANUAL_BGAIN="${AWB_MANUAL_BGAIN:-0}"
DNS_GAIN_EV_INIT="${DNS_GAIN_EV_INIT:-0}"
YSP_GAIN_EV_INIT="${YSP_GAIN_EV_INIT:-0}"
RTSP_PATH="${RTSP_PATH:-ch0}"
FRAMECHAN_NEUTRAL_UV_ON_DONE="${FRAMECHAN_NEUTRAL_UV_ON_DONE:-0}"
# Keep the LCE/top40 bit21 set. With 0x7fdfeeff the recovered MSCA output keeps
# a luma diamond pattern even after UV is neutralized; 0x7fffeeff removes it.
TISP_MAIN_INIT_TOP40_VALUE="${TISP_MAIN_INIT_TOP40_VALUE:-0x7fef88db}"
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
# Default 0 = OEM MSCA FIFO path. The legacy forced VIC-MDMA ring (1) races
# the MSCA output for the same framechan buffers once the ISP core path is
# live: every 4th encoded frame catches the raw VIC write instead of the
# processed frame (full-frame wrap-contour psychedelic flash, root-caused
# 2026-06-10 in logs/20260610-t40-msca-ringoff-fix).
T40_PROFILE_FORCE_VIC_MDMA_QBUF_RING="${T40_PROFILE_FORCE_VIC_MDMA_QBUF_RING:-0}"
T40_PROFILE_NO_DIRECT_ADDR_SOURCE="${T40_PROFILE_NO_DIRECT_ADDR_SOURCE:-0}"
T40_PROFILE_DIRECT_VIC_FEED="${T40_PROFILE_DIRECT_VIC_FEED:-0}"
FORCE_LOCAL_FRAME_STREAMOFF="${FORCE_LOCAL_FRAME_STREAMOFF:-1}"
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
ENABLE_ISP_BLOCK_INIT_AWB="${ENABLE_ISP_BLOCK_INIT_AWB:-0}"
ENABLE_AWB_REG_WRITES="${ENABLE_AWB_REG_WRITES:-1}"
ENABLE_AWB_SET_GAIN="${ENABLE_AWB_SET_GAIN:-1}"
ENABLE_AWB_GRAYWORLD="${ENABLE_AWB_GRAYWORLD:-0}"
ENABLE_AE_SOFT="${ENABLE_AE_SOFT:-0}"
ENABLE_FRAME_3A="${ENABLE_FRAME_3A:-0}"
ENABLE_SOFT_GAMMA="${ENABLE_SOFT_GAMMA:-1}"
ENABLE_YDNS="${ENABLE_YDNS:-1}"
# Legacy non-stock 0x1030 pedestal experiment; keep off for quality profiles.
ENABLE_GIB_BLC="${ENABLE_GIB_BLC:-0}"
ENABLE_BLC_LIT="${ENABLE_BLC_LIT:-0}"
BLC_LIT_GAIN="${BLC_LIT_GAIN:-212541}"
ENABLE_GIB_LIT="${ENABLE_GIB_LIT:-0}"
GIB_LIT_GAIN="${GIB_LIT_GAIN:-212541}"
GIB_LIT_FINAL_GAIN="${GIB_LIT_FINAL_GAIN:-1261}"
ENABLE_YSP="${ENABLE_YSP:-1}"
ENABLE_CCM="${ENABLE_CCM:-1}"
CCM_CT_TRACK="${CCM_CT_TRACK:-1}"
CCM_CT="${CCM_CT:-5000}"
CCM_EV="${CCM_EV:-400}"
ENABLE_BCSH="${ENABLE_BCSH:-0}"
BCSH_SATURATION="${BCSH_SATURATION:-0}"
BCSH_EV="${BCSH_EV:-400}"
BCSH_CT="${BCSH_CT:-6000}"
ENABLE_DMSC_STATIC="${ENABLE_DMSC_STATIC:-0}"
ENABLE_DMSC_LIT="${ENABLE_DMSC_LIT:-0}"
DMSC_LIT_GAIN="${DMSC_LIT_GAIN:-212541}"
DMSC_STOCK_CURRENT_OVERLAY="${DMSC_STOCK_CURRENT_OVERLAY:-0}"
DMSC_STOCK_HIGH_GAIN_OVERLAY="${DMSC_STOCK_HIGH_GAIN_OVERLAY:-0}"
DMSC_STOCK_OVERLAY_MASK="${DMSC_STOCK_OVERLAY_MASK:-15}"
BCSH_MODE="${BCSH_MODE:-2}"
ENABLE_CLM="${ENABLE_CLM:-0}"
CLM_STAGE_LIMIT="${CLM_STAGE_LIMIT:-0}"
CLM_DEFER_TRIG="${CLM_DEFER_TRIG:-0}"
CLM_CT="${CLM_CT:-5000}"
ENABLE_MDNS="${ENABLE_MDNS:-1}"
ENABLE_SDNS="${ENABLE_SDNS:-0}"
# MDNS temporal-reference buffer. The old driver default (0x8000000) sits
# 32MB into rmem, which rvd maps whole and carves its encoder buffers from —
# the ISP would scribble over rvd allocations there. Park it in nmem; the
# NNA is idle on the bench.
MDNS_BUF_PHYS="${MDNS_BUF_PHYS:-0xc000000}"
# LSC literal chain (tx_isp_t40_lsc_lit.inc): faithful OEM lens-shading init
# from the tparamsN tuning blob. LSC_LIT_CT picks the static color-temperature
# interp point (OEM refines it from AWB at runtime); LSC_LIT_GAIN is the 16.16
# log-gain for the global gain tables.
ENABLE_LSC_LIT="${ENABLE_LSC_LIT:-0}"
LSC_LIT_CT="${LSC_LIT_CT:-5000}"
LSC_LIT_GAIN="${LSC_LIT_GAIN:-0}"
LSC_LIT_CT_TRACK="${LSC_LIT_CT_TRACK:-1}"
LSC_LIT_CT_WARM_X="${LSC_LIT_CT_WARM_X:-1074}"
LSC_LIT_CT_WARM_CT="${LSC_LIT_CT_WARM_CT:-3300}"
LSC_LIT_CT_COOL_X="${LSC_LIT_CT_COOL_X:-730}"
LSC_LIT_CT_COOL_CT="${LSC_LIT_CT_COOL_CT:-6500}"
ENABLE_USERSPACE_3A="${ENABLE_USERSPACE_3A:-1}"
USERSPACE_3A_SETTLE_SECS="${USERSPACE_3A_SETTLE_SECS:-12}"
USERSPACE_3A_TARGET="${USERSPACE_3A_TARGET:-105}"
USERSPACE_3A_DEADBAND_PCT="${USERSPACE_3A_DEADBAND_PCT:-5}"
USERSPACE_3A_PERIOD="${USERSPACE_3A_PERIOD:-1}"
USERSPACE_3A_UV_LO="${USERSPACE_3A_UV_LO:-121}"
USERSPACE_3A_UV_HI="${USERSPACE_3A_UV_HI:-133}"
USERSPACE_3A_AWB_STEP_DIV="${USERSPACE_3A_AWB_STEP_DIV:-48}"
ISP_BLOCK_INIT_STAGE_LIMIT="${ISP_BLOCK_INIT_STAGE_LIMIT:-0}"
AWB_MAIN_INIT_STAGE_LIMIT="${AWB_MAIN_INIT_STAGE_LIMIT:-0}"
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
ENABLE_OEM_ISR_LIT="${ENABLE_OEM_ISR_LIT:-0}"
OEM_ISR_LIT_CUT="${OEM_ISR_LIT_CUT:-0}"
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
	SMOKE_SLEEP_SECS STOP_RAPTOR_TIMEOUT_SECS ALLOW_ACTIVE_STREAM_STOP \
	SKIP_BUILD SKIP_QBUF_DUMP SKIP_RTSP SKIP_REG_SNAPSHOT RTSP_FIRST FAST_RTSP_ONLY \
	FAST_STATS_SNAPSHOT FAST_STATS_SAMPLE_SECS FAST_STATS_SAMPLE_INTERVAL \
	REMOTE_PROBE_TIMEOUT_SECS RTSP_TIMEOUT_SECS PRESTREAM_EXPO_IT \
	AWB_MANUAL_RGAIN AWB_MANUAL_BGAIN DNS_GAIN_EV_INIT YSP_GAIN_EV_INIT \
	TISP_MAIN_INIT_COLOR_INIT_MASK CSI_SETTLE_OVERRIDE \
	T40_STOCK_HOST_INIT_MASK OEM_EVENT_PRE_CSI_STREAM \
	OEM_EVENT_PRE_CSI_STAGE_LIMIT OEM_EVENT_PRE_CSI_DELAY_MS \
	CORE_BAYER_REG8_VALUE VIC_MDMA_QBUF_RING_STRIDE_OVERRIDE \
		VIC_MDMA_QBUF_RING_CTRL_VALUE VIC_MDMA_QBUF_RING_UV_OFFSET_OVERRIDE \
		T40_PROFILE_FORCE_VIC_MDMA_QBUF_RING T40_PROFILE_NO_DIRECT_ADDR_SOURCE \
		T40_PROFILE_DIRECT_VIC_FEED \
		FORCE_LOCAL_FRAME_STREAMOFF \
		ADR_LINEAR_MODE \
		SENSOR_FULL_WIDTH_OVERRIDE SENSOR_FULL_HEIGHT_OVERRIDE \
		ENABLE_TISP_STREAM_EVENT_INIT ENABLE_TISP_STREAM_EVENT_CBS \
	ENABLE_ISP_3A_DIAG ENABLE_ISP_STATS_FANOUT \
	ISP_STATS_FANOUT_ADR_STATUS0_MASK \
	ENABLE_ADR_PROCESS_WORK \
	ENABLE_ISP_BLOCK_INIT ENABLE_ISP_BLOCK_INIT_AE ENABLE_ISP_BLOCK_INIT_AWB \
	ENABLE_AWB_REG_WRITES ENABLE_AWB_SET_GAIN ENABLE_AWB_GRAYWORLD ENABLE_AE_SOFT ENABLE_FRAME_3A \
	ENABLE_SOFT_GAMMA ENABLE_YDNS ENABLE_GIB_BLC \
	ENABLE_BLC_LIT BLC_LIT_GAIN ENABLE_GIB_LIT GIB_LIT_GAIN GIB_LIT_FINAL_GAIN \
	ENABLE_YSP ENABLE_CCM CCM_CT_TRACK CCM_CT CCM_EV ENABLE_BCSH BCSH_SATURATION BCSH_EV BCSH_CT ENABLE_DMSC_STATIC ENABLE_DMSC_LIT DMSC_LIT_GAIN DMSC_STOCK_CURRENT_OVERLAY DMSC_STOCK_HIGH_GAIN_OVERLAY DMSC_STOCK_OVERLAY_MASK BCSH_MODE ENABLE_CLM CLM_STAGE_LIMIT CLM_DEFER_TRIG CLM_CT ENABLE_MDNS ENABLE_SDNS \
	ENABLE_LSC_LIT LSC_LIT_CT LSC_LIT_GAIN \
	LSC_LIT_CT_TRACK LSC_LIT_CT_WARM_X LSC_LIT_CT_WARM_CT \
	LSC_LIT_CT_COOL_X LSC_LIT_CT_COOL_CT \
	ENABLE_USERSPACE_3A USERSPACE_3A_SETTLE_SECS \
	USERSPACE_3A_TARGET USERSPACE_3A_DEADBAND_PCT USERSPACE_3A_PERIOD \
	USERSPACE_3A_UV_LO USERSPACE_3A_UV_HI \
	USERSPACE_3A_AWB_STEP_DIV \
	ISP_BLOCK_INIT_STAGE_LIMIT AWB_MAIN_INIT_STAGE_LIMIT ADR_MAIN_INIT_STAGE_LIMIT \
	ENABLE_ADR_REG_WRITES \
	ENABLE_MSCA_REARM_GUARD MSCA_REARM_GUARD_MAX_SKIPS IRQ_FRAME_DONE_DELAY_MS \
	ENABLE_AE_SENSOR_APPLY \
	AE_SENSOR_APPLY_FORCE_PACKED \
	AE_SENSOR_APPLY_CLEAR_DIRTY AE_SENSOR_APPLY_MAX_AGAIN_INDEX \
	AE_SENSOR_APPLY_LOG_SKIPS ENABLE_OEM_ISR_LIT OEM_ISR_LIT_CUT; do
	if [[ ! "${!numeric}" =~ ^(0x[0-9a-fA-F]+|[0-9]+)$ ]]; then
		echo "$numeric must be decimal or hex" >&2
		exit 2
	fi
done
if [[ -n "$PASS" ]]; then
	export SSHPASS="$PASS"
	SSH=(sshpass -e ssh -T -o LogLevel=ERROR \
		-o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null \
		-o ConnectTimeout=5 -o ConnectionAttempts=1 "$USER@$IP")
	SCP=(sshpass -e scp -O -q -o LogLevel=ERROR \
		-o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null \
		-o ConnectTimeout=5 -o ConnectionAttempts=1)
else
	SSH=(ssh -T -o LogLevel=ERROR -o StrictHostKeyChecking=no \
		-o UserKnownHostsFile=/dev/null -o BatchMode=yes \
		-o ConnectTimeout=5 -o ConnectionAttempts=1 "$USER@$IP")
	SCP=(scp -O -q -o LogLevel=ERROR -o StrictHostKeyChecking=no \
		-o UserKnownHostsFile=/dev/null -o BatchMode=yes \
		-o ConnectTimeout=5 -o ConnectionAttempts=1)
fi

mkdir -p "$LOG"

scp_fetch_optional() {
	local remote="$1"
	local dest="$2"

	if ! timeout 20 "${SCP[@]}" "$USER@$IP:$remote" "$dest"; then
		printf 'warning: failed to fetch %s -> %s\n' "$remote" "$dest" \
			>>"$LOG/scp-warnings.txt"
		: >"$dest"
	fi
}

if [[ "$SKIP_BUILD" == "0" ]]; then
	ROOT="$ROOT" SOC="$SOC" ./build_local.sh
	"$ROOT/host/bin/mipsel-linux-gcc" -Os -Wall -Wextra -static \
		-o tools/phys_memdump.mipsel tools/phys_memdump.c
else
	[[ -f driver/t40/tx_isp_t40_recovered.ko ]] || {
		echo "SKIP_BUILD=1 but driver/t40/tx_isp_t40_recovered.ko is missing" >&2
		exit 2
	}
	[[ -x tools/phys_memdump.mipsel ]] || {
		echo "SKIP_BUILD=1 but tools/phys_memdump.mipsel is missing" >&2
		exit 2
	}
fi

"${SSH[@]}" 'pkill -f "[3]a.sh" 2>/dev/null || true; pkill -f /tmp/phys_memdump 2>/dev/null || true' || true
"${SCP[@]}" driver/t40/tx_isp_t40_recovered.ko \
	"$USER@$IP:/tmp/tx_isp_t40_recovered.ko"
"${SCP[@]}" tools/phys_memdump.mipsel "$USER@$IP:/tmp/phys_memdump"
"${SCP[@]}" tools/t40_userspace_3a.sh "$USER@$IP:/tmp/3a.sh"

timeout "$REMOTE_PROBE_TIMEOUT_SECS" "${SSH[@]}" \
	"SMOKE_SLEEP_SECS=$SMOKE_SLEEP_SECS" \
	"STOP_RAPTOR_TIMEOUT_SECS=$STOP_RAPTOR_TIMEOUT_SECS" \
	"ALLOW_ACTIVE_STREAM_STOP=$ALLOW_ACTIVE_STREAM_STOP" \
	"SKIP_REG_SNAPSHOT=$SKIP_REG_SNAPSHOT" \
	"FAST_RTSP_ONLY=$FAST_RTSP_ONLY" \
	"FAST_STATS_SNAPSHOT=$FAST_STATS_SNAPSHOT" \
	"FAST_STATS_SAMPLE_SECS=$FAST_STATS_SAMPLE_SECS" \
	"FAST_STATS_SAMPLE_INTERVAL=$FAST_STATS_SAMPLE_INTERVAL" \
	"PRESTREAM_EXPO_IT=$PRESTREAM_EXPO_IT" \
	"AWB_MANUAL_RGAIN=$AWB_MANUAL_RGAIN" \
	"AWB_MANUAL_BGAIN=$AWB_MANUAL_BGAIN" \
	"DNS_GAIN_EV_INIT=$DNS_GAIN_EV_INIT" \
	"YSP_GAIN_EV_INIT=$YSP_GAIN_EV_INIT" \
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
	"FORCE_LOCAL_FRAME_STREAMOFF=$FORCE_LOCAL_FRAME_STREAMOFF" \
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
	"ENABLE_ISP_BLOCK_INIT_AWB=$ENABLE_ISP_BLOCK_INIT_AWB" \
	"ENABLE_AWB_REG_WRITES=$ENABLE_AWB_REG_WRITES" \
	"ENABLE_AWB_SET_GAIN=$ENABLE_AWB_SET_GAIN" \
	"ENABLE_AWB_GRAYWORLD=$ENABLE_AWB_GRAYWORLD" \
	"ENABLE_AE_SOFT=$ENABLE_AE_SOFT" \
	"ENABLE_FRAME_3A=$ENABLE_FRAME_3A" \
	"ENABLE_SOFT_GAMMA=$ENABLE_SOFT_GAMMA" \
	"ENABLE_YDNS=$ENABLE_YDNS" \
	"ENABLE_GIB_BLC=$ENABLE_GIB_BLC" \
	"ENABLE_BLC_LIT=$ENABLE_BLC_LIT" \
	"BLC_LIT_GAIN=$BLC_LIT_GAIN" \
	"ENABLE_GIB_LIT=$ENABLE_GIB_LIT" \
	"GIB_LIT_GAIN=$GIB_LIT_GAIN" \
	"GIB_LIT_FINAL_GAIN=$GIB_LIT_FINAL_GAIN" \
	"ENABLE_YSP=$ENABLE_YSP" \
	"ENABLE_CCM=$ENABLE_CCM" \
	"CCM_CT_TRACK=$CCM_CT_TRACK" \
	"CCM_CT=$CCM_CT" \
	"CCM_EV=$CCM_EV" \
	"ENABLE_BCSH=$ENABLE_BCSH" \
	"BCSH_SATURATION=$BCSH_SATURATION" \
	"BCSH_EV=$BCSH_EV" \
	"BCSH_CT=$BCSH_CT" \
	"ENABLE_DMSC_STATIC=$ENABLE_DMSC_STATIC" \
	"ENABLE_DMSC_LIT=$ENABLE_DMSC_LIT" \
	"DMSC_LIT_GAIN=$DMSC_LIT_GAIN" \
	"DMSC_STOCK_CURRENT_OVERLAY=$DMSC_STOCK_CURRENT_OVERLAY" \
	"DMSC_STOCK_HIGH_GAIN_OVERLAY=$DMSC_STOCK_HIGH_GAIN_OVERLAY" \
	"DMSC_STOCK_OVERLAY_MASK=$DMSC_STOCK_OVERLAY_MASK" \
	"BCSH_MODE=$BCSH_MODE" \
	"ENABLE_CLM=$ENABLE_CLM" \
	"CLM_STAGE_LIMIT=$CLM_STAGE_LIMIT" \
	"CLM_DEFER_TRIG=$CLM_DEFER_TRIG" \
	"CLM_CT=$CLM_CT" \
	"ENABLE_MDNS=$ENABLE_MDNS" \
	"ENABLE_SDNS=$ENABLE_SDNS" \
	"MDNS_BUF_PHYS=$MDNS_BUF_PHYS" \
	"ENABLE_LSC_LIT=$ENABLE_LSC_LIT" \
	"LSC_LIT_CT=$LSC_LIT_CT" \
	"LSC_LIT_GAIN=$LSC_LIT_GAIN" \
	"LSC_LIT_CT_TRACK=$LSC_LIT_CT_TRACK" \
	"LSC_LIT_CT_WARM_X=$LSC_LIT_CT_WARM_X" \
	"LSC_LIT_CT_WARM_CT=$LSC_LIT_CT_WARM_CT" \
	"LSC_LIT_CT_COOL_X=$LSC_LIT_CT_COOL_X" \
	"LSC_LIT_CT_COOL_CT=$LSC_LIT_CT_COOL_CT" \
	"ISP_BLOCK_INIT_STAGE_LIMIT=$ISP_BLOCK_INIT_STAGE_LIMIT" \
	"AWB_MAIN_INIT_STAGE_LIMIT=$AWB_MAIN_INIT_STAGE_LIMIT" \
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
	"ENABLE_OEM_ISR_LIT=$ENABLE_OEM_ISR_LIT" \
	"OEM_ISR_LIT_CUT=$OEM_ISR_LIT_CUT" \
	sh -s >"$LOG/load-safe.log" 2>&1 <<'EOS'
set -x
: "${FRAMECHAN_NEUTRAL_UV_ON_DONE:=0}"
: "${SMOKE_SLEEP_SECS:=12}"
: "${STOP_RAPTOR_TIMEOUT_SECS:=20}"
: "${ALLOW_ACTIVE_STREAM_STOP:=0}"
: "${SKIP_REG_SNAPSHOT:=0}"
: "${FAST_RTSP_ONLY:=0}"
: "${FAST_STATS_SNAPSHOT:=0}"
: "${FAST_STATS_SAMPLE_SECS:=6}"
: "${FAST_STATS_SAMPLE_INTERVAL:=1}"
: "${PRESTREAM_EXPO_IT:=0}"
: "${AWB_MANUAL_RGAIN:=0}"
: "${AWB_MANUAL_BGAIN:=0}"
: "${DNS_GAIN_EV_INIT:=0}"
: "${YSP_GAIN_EV_INIT:=0}"
: "${TISP_MAIN_INIT_TOP40_VALUE:=0x7fffeeff}"
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
: "${T40_PROFILE_FORCE_VIC_MDMA_QBUF_RING:=0}"
: "${T40_PROFILE_NO_DIRECT_ADDR_SOURCE:=0}"
: "${T40_PROFILE_DIRECT_VIC_FEED:=0}"
: "${FORCE_LOCAL_FRAME_STREAMOFF:=1}"
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
: "${ENABLE_ISP_BLOCK_INIT_AWB:=0}"
: "${ENABLE_AWB_REG_WRITES:=1}"
: "${ENABLE_AWB_SET_GAIN:=1}"
: "${ENABLE_AWB_GRAYWORLD:=0}"
: "${ENABLE_AE_SOFT:=0}"
: "${ENABLE_FRAME_3A:=0}"
: "${ENABLE_SOFT_GAMMA:=1}"
: "${ENABLE_YDNS:=1}"
: "${ENABLE_GIB_BLC:=0}"
: "${ENABLE_BLC_LIT:=0}"
: "${BLC_LIT_GAIN:=212541}"
: "${ENABLE_GIB_LIT:=0}"
: "${GIB_LIT_GAIN:=212541}"
: "${GIB_LIT_FINAL_GAIN:=1261}"
: "${ENABLE_YSP:=1}"
: "${ENABLE_CCM:=1}"
: "${CCM_CT_TRACK:=1}"
: "${CCM_CT:=5000}"
: "${CCM_EV:=400}"
: "${ENABLE_BCSH:=0}"
: "${BCSH_SATURATION:=0}"
: "${BCSH_EV:=400}"
: "${BCSH_CT:=6000}"
: "${ENABLE_DMSC_STATIC:=0}"
: "${BCSH_MODE:=2}"
: "${ENABLE_CLM:=0}"
: "${CLM_STAGE_LIMIT:=0}"
: "${CLM_DEFER_TRIG:=0}"
: "${CLM_CT:=5000}"
: "${ENABLE_MDNS:=1}"
: "${ENABLE_SDNS:=0}"
: "${MDNS_BUF_PHYS:=0xc000000}"
: "${ENABLE_LSC_LIT:=0}"
: "${LSC_LIT_CT:=5000}"
: "${LSC_LIT_GAIN:=0}"
: "${LSC_LIT_CT_TRACK:=1}"
: "${LSC_LIT_CT_WARM_X:=1074}"
: "${LSC_LIT_CT_WARM_CT:=3300}"
: "${LSC_LIT_CT_COOL_X:=730}"
: "${LSC_LIT_CT_COOL_CT:=6500}"
: "${ISP_BLOCK_INIT_STAGE_LIMIT:=0}"
: "${AWB_MAIN_INIT_STAGE_LIMIT:=0}"
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
: "${ENABLE_OEM_ISR_LIT:=0}"
: "${OEM_ISR_LIT_CUT:=0}"
raptor_stop_pid=
# The stream-stop crash is specific to a stream served by the recovered
# module; stopping the stock-driver stream on a fresh boot is routine.
if [ "$ALLOW_ACTIVE_STREAM_STOP" != "1" ] && pidof rvd >/dev/null 2>&1 \
	&& [ -d /sys/module/tx_isp_t40_recovered ]; then
	echo "refusing to stop rvd stream on active recovered module; set ALLOW_ACTIVE_STREAM_STOP=1 or power-cycle first" >&2
	exit 3
fi
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
# A raptor daemon respawned here (e.g. the boot-time S31raptor start still in
# flight) grabs /dev/tx-isp the moment the recovered module appears — before
# the sensor module is in — fails sensor registration, and wedges the whole
# bring-up (then rmmod of the wedged module oopses). Re-kill until the box is
# quiet; bail out rather than insmod into a race.
raptor_quiet=0
for _i in 1 2 3 4 5 6 7 8 9 10; do
	if ps | grep -qE "[r]vd|[r]ic"; then
		killall -9 rvd rad rod rsd rhd ric rwd 2>/dev/null || true
		sleep 2
	else
		raptor_quiet=1
		break
	fi
done
if [ "$raptor_quiet" != "1" ]; then
	echo "raptor daemons keep respawning; refusing to insmod into the race" >&2
	exit 4
fi
rmmod sensor_gc4653_t40 2>/tmp/rmmod-sensor.err || true
cat /tmp/rmmod-sensor.err || true
rmmod tx_isp_t40_recovered 2>/tmp/rmmod-recovered.err || true
cat /tmp/rmmod-recovered.err || true
rmmod tx_isp_t40 2>/tmp/rmmod-stock.err || true
cat /tmp/rmmod-stock.err || true
dmesg -c > /tmp/t40-dmesg-before-load.txt 2>/dev/null || true
# Detached kmsg capture: catches oops/epc in streamon worker threads that
# dmesg's ring buffer rotates away under event spam (see T40_TUNING_HURDLES).
rm -f /tmp/k.log
( cat /proc/kmsg > /tmp/k.log 2>/dev/null & echo $! > /tmp/kmsg.pid )
insmod /tmp/tx_isp_t40_recovered.ko \
	t40_bringup_profile=1 \
	t40_profile_direct_vic_feed="$T40_PROFILE_DIRECT_VIC_FEED" \
	t40_profile_no_direct_irq_defaults=1 \
	t40_profile_isp_irq_passthrough=1 \
	enable_oem_isr_lit="${ENABLE_OEM_ISR_LIT:-0}" \
	oem_isr_lit_cut="${OEM_ISR_LIT_CUT:-0}" \
	t40_profile_force_vic_mdma_qbuf_ring="$T40_PROFILE_FORCE_VIC_MDMA_QBUF_RING" \
	t40_profile_no_direct_addr_source="$T40_PROFILE_NO_DIRECT_ADDR_SOURCE" \
	force_local_frame_streamoff="$FORCE_LOCAL_FRAME_STREAMOFF" \
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
	enable_isp_block_init_awb="$ENABLE_ISP_BLOCK_INIT_AWB" \
	enable_awb_reg_writes="$ENABLE_AWB_REG_WRITES" \
	enable_awb_set_gain="$ENABLE_AWB_SET_GAIN" \
	enable_awb_grayworld="$ENABLE_AWB_GRAYWORLD" \
	enable_ae_soft="$ENABLE_AE_SOFT" \
	enable_frame_3a="$ENABLE_FRAME_3A" \
	enable_soft_gamma="$ENABLE_SOFT_GAMMA" \
	enable_ydns="$ENABLE_YDNS" \
	enable_gib_blc="$ENABLE_GIB_BLC" \
	enable_blc_lit="$ENABLE_BLC_LIT" \
	blc_lit_gain="$BLC_LIT_GAIN" \
	enable_gib_lit="$ENABLE_GIB_LIT" \
	gib_lit_gain="$GIB_LIT_GAIN" \
	gib_lit_final_gain="$GIB_LIT_FINAL_GAIN" \
	enable_ysp="$ENABLE_YSP" \
	enable_ccm="$ENABLE_CCM" \
	ccm_ct_track="$CCM_CT_TRACK" \
	ccm_ct="$CCM_CT" \
	ccm_ev="$CCM_EV" \
	enable_bcsh="$ENABLE_BCSH" \
	bcsh_saturation="$BCSH_SATURATION" \
	bcsh_ev="$BCSH_EV" \
	bcsh_ct="$BCSH_CT" \
	enable_dmsc_static="$ENABLE_DMSC_STATIC" \
	enable_dmsc_lit="$ENABLE_DMSC_LIT" \
	dmsc_lit_gain="$DMSC_LIT_GAIN" \
	dmsc_stock_current_overlay="$DMSC_STOCK_CURRENT_OVERLAY" \
	dmsc_stock_high_gain_overlay="$DMSC_STOCK_HIGH_GAIN_OVERLAY" \
	dmsc_stock_overlay_mask="$DMSC_STOCK_OVERLAY_MASK" \
	bcsh_mode="$BCSH_MODE" \
	enable_clm="$ENABLE_CLM" \
	clm_stage_limit="$CLM_STAGE_LIMIT" \
	clm_defer_trig="$CLM_DEFER_TRIG" \
	clm_ct="$CLM_CT" \
	enable_mdns="$ENABLE_MDNS" \
	enable_sdns="$ENABLE_SDNS" \
	mdns_buf_phys="$MDNS_BUF_PHYS" \
	enable_lsc_lit="$ENABLE_LSC_LIT" \
	lsc_lit_ct="$LSC_LIT_CT" \
	lsc_lit_gain="$LSC_LIT_GAIN" \
	lsc_lit_ct_track="$LSC_LIT_CT_TRACK" \
	lsc_lit_ct_warm_x="$LSC_LIT_CT_WARM_X" \
	lsc_lit_ct_warm_ct="$LSC_LIT_CT_WARM_CT" \
	lsc_lit_ct_cool_x="$LSC_LIT_CT_COOL_X" \
	lsc_lit_ct_cool_ct="$LSC_LIT_CT_COOL_CT" \
	isp_block_init_stage_limit="$ISP_BLOCK_INIT_STAGE_LIMIT" \
	awb_main_init_stage_limit="$AWB_MAIN_INIT_STAGE_LIMIT" \
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
	awb_manual_rgain="$AWB_MANUAL_RGAIN" \
	awb_manual_bgain="$AWB_MANUAL_BGAIN" \
	dns_gain_ev="$DNS_GAIN_EV_INIT" \
	ysp_gain_ev="$YSP_GAIN_EV_INIT" \
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
if [ "$PRESTREAM_EXPO_IT" != "0" ]; then
	{
		hi=$(( (PRESTREAM_EXPO_IT >> 8) & 255 ))
		lo=$(( PRESTREAM_EXPO_IT & 255 ))
		printf 'prestream_expo_it=%s hi=0x%02x lo=0x%02x\n' \
			"$PRESTREAM_EXPO_IT" "$hi" "$lo"
		i2ctransfer -f -y 1 w3@0x29 0x02 0x02 "$(printf '0x%02x' "$hi")"
		i2ctransfer -f -y 1 w3@0x29 0x02 0x03 "$(printf '0x%02x' "$lo")"
		printf 'readback_0202='
		i2ctransfer -f -y 1 w2@0x29 0x02 0x02 r1 2>/dev/null || true
		printf ' readback_0203='
		i2ctransfer -f -y 1 w2@0x29 0x02 0x03 r1 2>/dev/null || true
		printf '\n'
	} > /tmp/t40-prestream-expo.txt 2>&1 || true
else
	echo "prestream exposure clamp disabled" > /tmp/t40-prestream-expo.txt
fi

capture_t40_fast_stats() {
	set +x
	cat /proc/interrupts > /tmp/t40-interrupts-after.txt 2>/dev/null || true
	echo "skipped fast proc dump: /proc/tx_isp_t40_recovered can block while streaming" > /tmp/t40-proc.txt
	dmesg > /tmp/t40-dmesg-full.txt 2>/dev/null || true
	dmesg | grep -E 'framechan0 (repaired )?qbuf|irq frame-done|frame.?done|msca|MSCA|fifo|FIFO|addr_fifo|bring-up profile|isp-block-init|awb|AWB|AEIRQ|AETRIG|AWBGAIN|3a-diag|stats fanout|irq_func_cb|TISP stream event|event_process|ae_main_process|sensor_ioctl|core-event streamon' | tail -500 > /tmp/t40-qbuf-lines.txt 2>/dev/null || true

	if ! command -v devmem >/dev/null 2>&1; then
		echo "devmem not found" > /tmp/t40-fast-core-regs.txt
		echo "devmem not found" > /tmp/t40-fast-core-regs-post.txt
		echo "devmem not found" > /tmp/t40-fast-core-samples.txt
		return
	fi

	dump_core_reg_range() {
		local label="$1"
		local start="$2"
		local end="$3"
		local off addr hex

		echo "# $label"
		off="$start"
		while [ "$off" -le "$end" ]; do
			addr=$((0x13300000 + off))
			hex="$(printf '0x%08x' "$addr")"
			printf 'off=0x%05x addr=%s value=' "$off" "$hex"
			devmem "$hex" 32 2>/dev/null || true
			off=$((off + 4))
		done
	}

	dump_t40_core_regs() {
		local phase="$1"

		echo "# T40 ISP core register snapshot phase=$phase $(date)"
		dump_core_reg_range "core-top" $((0x00000)) $((0x00110))
		dump_core_reg_range "core-pipe" $((0x00800)) $((0x00840))
		dump_core_reg_range "tisp-stream" $((0x01000)) $((0x01064))
		dump_core_reg_range "awb-wb" $((0x04000)) $((0x05010))
		dump_core_reg_range "dmsc" $((0x0a000)) $((0x0a27c))
		dump_core_reg_range "csc" $((0x0d000)) $((0x0d040))
		dump_core_reg_range "stats-awb" $((0x18000)) $((0x18080))
		dump_core_reg_range "stats-ae" $((0x19000)) $((0x19080))
		dump_core_reg_range "msca" $((0x16000)) $((0x16400))
		dump_core_reg_range "t40-gate" $((0x17000)) $((0x17040))
		dump_core_reg_range "irq-route" $((0x40000)) $((0x400b8))
		dump_core_reg_range "legacy-route-9a" $((0x09a00)) $((0x09ad0))
		dump_core_reg_range "legacy-route-b0" $((0x0b000)) $((0x0b030))
	}

	dump_t40_core_regs pre > /tmp/t40-fast-core-regs.txt 2>&1

	{
		i=0
		while [ "$i" -lt "$FAST_STATS_SAMPLE_SECS" ]; do
			echo "# sample $i $(date)"
			for off in 0x18050 0x18054 0x18058 0x19000 0x19004 0x19050 0x19054 0x19058 \
				0x40020 0x40024 0x40028 0x40030 0x40034 0x40038 0x40060 0x40064 0x40068 \
				0x400a0 0x400a4 0x400a8; do
				addr=$((0x13300000 + off))
				hex="$(printf '0x%08x' "$addr")"
				printf 'off=%s addr=%s value=' "$off" "$hex"
				devmem "$hex" 32 2>/dev/null || true
			done
			cat /proc/interrupts | grep -E '(^ *3[89]:|tx|isp|vic)' || true
			sleep "$FAST_STATS_SAMPLE_INTERVAL"
			i=$((i + FAST_STATS_SAMPLE_INTERVAL))
		done
	} > /tmp/t40-fast-core-samples.txt 2>&1

	dump_t40_core_regs post > /tmp/t40-fast-core-regs-post.txt 2>&1
	echo "skipped fast proc dump: /proc/tx_isp_t40_recovered can block while streaming" > /tmp/t40-proc-post.txt
	dmesg > /tmp/t40-dmesg-full.txt 2>/dev/null || true
	dmesg | grep -E 'framechan0 (repaired )?qbuf|irq frame-done|frame.?done|msca|MSCA|fifo|FIFO|addr_fifo|bring-up profile|isp-block-init|awb|AWB|AEIRQ|AETRIG|AWBGAIN|3a-diag|stats fanout|irq_func_cb|TISP stream event|event_process|ae_main_process|sensor_ioctl|core-event streamon' | tail -700 > /tmp/t40-qbuf-lines.txt 2>/dev/null || true
}

/etc/init.d/S31raptor start
if [ "$FAST_RTSP_ONLY" = "1" ]; then
	{
		date
		uptime
		ps | grep "[r]vd" || true
	} > /tmp/t40-fast-start.txt 2>&1
	if [ "$FAST_STATS_SNAPSHOT" = "1" ]; then
		capture_t40_fast_stats
	else
		echo "skipped fast stats snapshot (FAST_STATS_SNAPSHOT=0)" > /tmp/t40-proc.txt
		echo "skipped fast stats snapshot (FAST_STATS_SNAPSHOT=0)" > /tmp/t40-proc-post.txt
		echo "skipped fast stats snapshot (FAST_STATS_SNAPSHOT=0)" > /tmp/t40-fast-core-regs.txt
		echo "skipped fast stats snapshot (FAST_STATS_SNAPSHOT=0)" > /tmp/t40-fast-core-regs-post.txt
		echo "skipped fast stats snapshot (FAST_STATS_SNAPSHOT=0)" > /tmp/t40-fast-core-samples.txt
	fi
	exit 0
fi
dmesg | grep -E 'isp-block-init|lsc-lit|awb|AWB|clm|CLM|bcsh|BCSH|ccm|CCM|mdns|MDNS|ydns|YDNS|ysp|YSP|gib|GIB|3a-diag|ADR|adr|irq_func_cb|stats fanout|OEM event pre-CSI|CSI direct stage-limit|CSI direct start|direct VIC streamon|OEM event streamon|core-event streamon|CSI direct|settle|phy complete|vic_start_diag' > /tmp/t40-start-immediate-lines.txt 2>/dev/null || true
dmesg | tail -260 > /tmp/t40-dmesg-start-immediate.txt
hold_secs=$((SMOKE_SLEEP_SECS))
if [ "$hold_secs" -gt 4 ]; then
	sleep 4
	cat /proc/interrupts > /tmp/t40-interrupts-start.txt
	dmesg | grep -E 'isp-block-init|lsc-lit|awb|AWB|clm|CLM|bcsh|BCSH|ccm|CCM|mdns|MDNS|ydns|YDNS|ysp|YSP|gib|GIB|3a-diag|ADR|adr|irq_func_cb|stats fanout|OEM event pre-CSI|CSI direct stage-limit|CSI direct start|direct VIC streamon|OEM event streamon|core-event streamon|CSI direct|settle|phy complete|vic_start_diag' > /tmp/t40-start-lines.txt 2>/dev/null || true
	dmesg | tail -220 > /tmp/t40-dmesg-start.txt
	sleep $((hold_secs - 4))
else
	sleep "$hold_secs"
	cat /proc/interrupts > /tmp/t40-interrupts-start.txt
	dmesg | grep -E 'isp-block-init|lsc-lit|awb|AWB|clm|CLM|bcsh|BCSH|ccm|CCM|mdns|MDNS|ydns|YDNS|ysp|YSP|gib|GIB|3a-diag|ADR|adr|irq_func_cb|stats fanout|OEM event pre-CSI|CSI direct stage-limit|CSI direct start|direct VIC streamon|OEM event streamon|core-event streamon|CSI direct|settle|phy complete|vic_start_diag' > /tmp/t40-start-lines.txt 2>/dev/null || true
	dmesg | tail -220 > /tmp/t40-dmesg-start.txt
fi
chmod +x /tmp/phys_memdump
cat /proc/interrupts | grep -E '(^ *3[89]:|tx|isp|vic)' || true
cat /proc/interrupts > /tmp/t40-interrupts-after.txt
cat /proc/tx_isp_t40_recovered > /tmp/t40-proc.txt 2>/dev/null || true
if [ "$SKIP_REG_SNAPSHOT" = "1" ]; then
	echo "skipped active sensor/devmem snapshot (SKIP_REG_SNAPSHOT=1)" > /tmp/t40-sensor-ae-regs.txt
	echo "skipped active sensor/devmem snapshot (SKIP_REG_SNAPSHOT=1)" > /tmp/t40-csi-vic-regs.txt
else
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
		# Capture the complete GC4653 combined-exposure footprint.  The EXPO
		# ioctl selects a ten-register analog/digital-gain LUT row; sampling
		# only 0x02b3/0x02b4 can make two different rows look deceptively alike.
		for reg in 0x0202 0x0203 0x0205 \
			0x02b3 0x02b4 0x02b8 0x02b9 0x0515 0x0519 0x02d9 0x020e 0x020f \
			0x0218 0x0219 0x0340 0x0341; do
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
fi
set -x
dmesg | grep -E 'framechan0 (repaired )?qbuf|lsc-lit|VIC frame MDMA qbuf ring|irq frame-done|frame.?done|msca|MSCA|fifo|FIFO|addr_fifo|bring-up profile|stock-host override|OEM event pre-CSI|isp-block-init|awb|AWB|clm|CLM|bcsh|BCSH|ccm|CCM|mdns|MDNS|ydns|YDNS|ysp|YSP|gib|GIB|ADR|adr|3a-diag|stats fanout|irq_func_cb|TISP stream event|rearm-guard|AE sensor apply|tgain|again|event setup|sensor_ioctl|CSI direct|csi_|settle|phy complete|vic_start_diag' | tail -380 > /tmp/t40-qbuf-lines.txt
dmesg | tail -260 > /tmp/t40-dmesg-tail.txt
dmesg > /tmp/t40-dmesg-full.txt
EOS

capture_rtsp_frame() {
	timeout "$RTSP_TIMEOUT_SECS" ffmpeg -nostdin -hide_banner -loglevel info -y \
		-rtsp_transport tcp \
		-i "rtsp://thingino:thingino@$IP:554/$RTSP_PATH" -frames:v 1 \
		"$LOG/rtsp-frame.jpg" >"$LOG/ffmpeg.log" 2>&1 || true
}

start_userspace_3a() {
	"${SSH[@]}" "sh -c 'TARGET=$USERSPACE_3A_TARGET DEADBAND_PCT=$USERSPACE_3A_DEADBAND_PCT PERIOD=$USERSPACE_3A_PERIOD UV_LO=$USERSPACE_3A_UV_LO UV_HI=$USERSPACE_3A_UV_HI AWB_STEP_DIV=$USERSPACE_3A_AWB_STEP_DIV DYNAMIC_PHYS=1 nohup sh /tmp/3a.sh > /tmp/3a.log 2>&1 < /dev/null &' ; echo 3a-agent-started; sleep 1; ps | grep '[3]a.sh' || true"
}

if [[ "$FAST_RTSP_ONLY" == "1" && "$ENABLE_USERSPACE_3A" == "1" ]]; then
	{
		echo "# starting dynamic-buffer userspace 3A before fast RTSP capture"
		start_userspace_3a
	} >"$LOG/3a-start.log" 2>&1 || true
	if [[ "$USERSPACE_3A_SETTLE_SECS" != "0" ]]; then
		sleep "$USERSPACE_3A_SETTLE_SECS"
	fi
elif [[ "$FAST_RTSP_ONLY" == "1" ]]; then
	echo "userspace 3A disabled (ENABLE_USERSPACE_3A=0)" >"$LOG/3a-start.log"
fi

if [[ "$RTSP_FIRST" == "1" || "$FAST_RTSP_ONLY" == "1" ]]; then
	if [[ "$SKIP_RTSP" == "1" ]]; then
		printf 'skipping RTSP snapshot (SKIP_RTSP=1)\n' >"$LOG/ffmpeg.log"
	else
		capture_rtsp_frame
	fi
fi

if [[ "$FAST_RTSP_ONLY" == "1" ]]; then
	scp_fetch_optional /tmp/t40-fast-start.txt "$LOG/fast-start.txt"
	scp_fetch_optional /tmp/t40-prestream-expo.txt "$LOG/prestream-expo.txt"
	scp_fetch_optional /tmp/t40-proc.txt "$LOG/proc.txt"
	scp_fetch_optional /tmp/t40-proc-post.txt "$LOG/proc-post.txt"
	scp_fetch_optional /tmp/t40-fast-core-regs.txt "$LOG/fast-core-regs.txt"
	scp_fetch_optional /tmp/t40-fast-core-regs-post.txt "$LOG/fast-core-regs-post.txt"
	scp_fetch_optional /tmp/t40-fast-core-samples.txt "$LOG/fast-core-samples.txt"
	scp_fetch_optional /tmp/t40-interrupts-after.txt "$LOG/interrupts-after.txt"
	scp_fetch_optional /tmp/t40-qbuf-lines.txt "$LOG/qbuf-lines.txt"
	"${SSH[@]}" 'kill "$(cat /tmp/kmsg.pid 2>/dev/null)" 2>/dev/null; true' || true
	scp_fetch_optional /tmp/k.log "$LOG/kmsg.log"
	scp_fetch_optional /tmp/t40-dmesg-full.txt "$LOG/dmesg-full.txt"
	scp_fetch_optional /tmp/3a.log "$LOG/3a.log"
	scp_fetch_optional /tmp/t40-forensic-state.txt "$LOG/forensic-state.txt"
	printf 'log=%s\n' "$LOG"
	sed -n '1,40p' "$LOG/ffmpeg.log"
	exit 0
fi

scp_fetch_optional /tmp/t40-interrupts-after.txt "$LOG/interrupts-after.txt"
scp_fetch_optional /tmp/t40-interrupts-start.txt "$LOG/interrupts-start.txt"
scp_fetch_optional /tmp/t40-start-immediate-lines.txt "$LOG/start-immediate-lines.txt"
scp_fetch_optional /tmp/t40-dmesg-start-immediate.txt "$LOG/dmesg-start-immediate.txt"
scp_fetch_optional /tmp/t40-start-lines.txt "$LOG/start-lines.txt"
scp_fetch_optional /tmp/t40-dmesg-start.txt "$LOG/dmesg-start.txt"
scp_fetch_optional /tmp/t40-qbuf-lines.txt "$LOG/qbuf-lines.txt"
scp_fetch_optional /tmp/t40-proc.txt "$LOG/proc.txt"
scp_fetch_optional /tmp/t40-csi-vic-regs.txt "$LOG/csi-vic-regs.txt"
scp_fetch_optional /tmp/t40-dmesg-tail.txt "$LOG/dmesg-tail.txt"
scp_fetch_optional /tmp/t40-dmesg-full.txt "$LOG/dmesg-full.txt"
scp_fetch_optional /tmp/t40-dmesg-before-load.txt "$LOG/dmesg-before-load.txt"
"${SSH[@]}" 'kill "$(cat /tmp/kmsg.pid 2>/dev/null)" 2>/dev/null; true' || true
scp_fetch_optional /tmp/k.log "$LOG/kmsg.log"
scp_fetch_optional /tmp/t40-sensor-ae-regs.txt "$LOG/sensor-ae-regs.txt"
scp_fetch_optional /tmp/t40-prestream-expo.txt "$LOG/prestream-expo.txt"

if [[ "$ENABLE_USERSPACE_3A" == "1" ]]; then
	{
		echo "# starting userspace 3A before qbuf/RTSP capture"
		start_userspace_3a
	} >"$LOG/3a-start.log" 2>&1 || true
	if [[ "$USERSPACE_3A_SETTLE_SECS" != "0" ]]; then
		sleep "$USERSPACE_3A_SETTLE_SECS"
	fi
else
	echo "userspace 3A disabled (ENABLE_USERSPACE_3A=0)" >"$LOG/3a-start.log"
fi

qline="$(grep -h 'framechan0 repaired qbuf' "$LOG/qbuf-lines.txt" \
	"$LOG/start-immediate-lines.txt" "$LOG/dmesg-start-immediate.txt" \
	2>/dev/null | head -n1 || true)"
if [[ -z "$qline" ]]; then
	qline="$(grep -h 'framechan0 qbuf' "$LOG/qbuf-lines.txt" \
		"$LOG/start-immediate-lines.txt" "$LOG/dmesg-start-immediate.txt" \
		2>/dev/null | head -n1 || true)"
fi
if [[ -z "$qline" ]]; then
	qline="$(grep -h 'VIC frame MDMA qbuf ring' "$LOG/qbuf-lines.txt" \
		"$LOG/start-immediate-lines.txt" "$LOG/dmesg-start-immediate.txt" \
		2>/dev/null | head -n1 || true)"
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

if [[ "$RTSP_FIRST" != "1" ]]; then
	if [[ "$SKIP_RTSP" == "1" ]]; then
		printf 'skipping RTSP snapshot (SKIP_RTSP=1)\n' >"$LOG/ffmpeg.log"
	else
		capture_rtsp_frame
	fi
fi

"${SSH[@]}" 'P=/sys/module/tx_isp_t40_recovered/parameters; \
	echo "# 3A process"; ps | grep "[3]a.sh" || true; \
	echo "# 3A params"; \
	for p in ae_sensor_apply_force_packed dns_gain_ev ysp_gain_ev ysp_gain_ev_now dmsc_lit_gain dmsc_lit_gain_now awb_manual_rgain awb_manual_bgain awb_grayworld_last_rgain awb_grayworld_last_bgain lsc_lit_ct lsc_lit_ct_now isp_block_init_ran isp_block_init_count isp_block_init_skip_no_blob isp_block_init_adr_ret isp_block_init_clm_ret clm_stage_limit clm_defer_trig clm_ct clm_ct_now clm_ct_region clm_stage_reached clm_last_ret; do \
		printf "%s=" "$p"; cat "$P/$p" 2>/dev/null || echo NA; \
	done; \
	echo "# 3A log tail"; tail -80 /tmp/3a.log 2>/dev/null || true' \
	>"$LOG/3a-status.txt" 2>&1 || true
"${SCP[@]}" "$USER@$IP:/tmp/3a.log" "$LOG/3a.log" 2>/dev/null || true

printf 'log=%s\n' "$LOG"
sed -n '1,20p' "$LOG/qbuf-dump.txt"
if [[ -f "$LOG/nv12-probe.log" ]]; then
	sed -n '1,20p' "$LOG/nv12-probe.log"
fi
