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
FRAMECHAN_NEUTRAL_UV_ON_DONE="${FRAMECHAN_NEUTRAL_UV_ON_DONE:-0}"
TISP_MAIN_INIT_TOP40_VALUE="${TISP_MAIN_INIT_TOP40_VALUE:-0x7fdfeeff}"
TISP_MAIN_INIT_CSC_VERSION_VALUE="${TISP_MAIN_INIT_CSC_VERSION_VALUE:-2}"
ENABLE_TISP_MAIN_INIT_COLOR_INITS="${ENABLE_TISP_MAIN_INIT_COLOR_INITS:-0}"
TISP_MAIN_INIT_COLOR_INIT_MASK="${TISP_MAIN_INIT_COLOR_INIT_MASK:-0}"
CSI_SETTLE_OVERRIDE="${CSI_SETTLE_OVERRIDE:-0x1b}"
CORE_BAYER_REG8_VALUE="${CORE_BAYER_REG8_VALUE:-0x10002}"
# VIC MDMA qbuf-ring output geometry. Defaults of 0 keep the driver's built-in
# behavior (stride=width=0x780, fmt=7). Stock/OEM VIC stride is 0xF00 (width<<1);
# sweep STRIDE_OVERRIDE/CTRL_VALUE/UV_OFFSET together to test the OEM 2-byte/px
# format against the 64-row luma banding (see docs/T40_TUNING_HURDLES.md).
VIC_MDMA_QBUF_RING_STRIDE_OVERRIDE="${VIC_MDMA_QBUF_RING_STRIDE_OVERRIDE:-0}"
VIC_MDMA_QBUF_RING_CTRL_VALUE="${VIC_MDMA_QBUF_RING_CTRL_VALUE:-0}"
VIC_MDMA_QBUF_RING_UV_OFFSET_OVERRIDE="${VIC_MDMA_QBUF_RING_UV_OFFSET_OVERRIDE:-0}"
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
	TISP_MAIN_INIT_COLOR_INIT_MASK CSI_SETTLE_OVERRIDE \
	CORE_BAYER_REG8_VALUE VIC_MDMA_QBUF_RING_STRIDE_OVERRIDE \
	VIC_MDMA_QBUF_RING_CTRL_VALUE VIC_MDMA_QBUF_RING_UV_OFFSET_OVERRIDE; do
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
	"FRAMECHAN_NEUTRAL_UV_ON_DONE=$FRAMECHAN_NEUTRAL_UV_ON_DONE" \
	"TISP_MAIN_INIT_TOP40_VALUE=$TISP_MAIN_INIT_TOP40_VALUE" \
	"TISP_MAIN_INIT_CSC_VERSION_VALUE=$TISP_MAIN_INIT_CSC_VERSION_VALUE" \
	"ENABLE_TISP_MAIN_INIT_COLOR_INITS=$ENABLE_TISP_MAIN_INIT_COLOR_INITS" \
	"TISP_MAIN_INIT_COLOR_INIT_MASK=$TISP_MAIN_INIT_COLOR_INIT_MASK" \
	"CSI_SETTLE_OVERRIDE=$CSI_SETTLE_OVERRIDE" \
	"CORE_BAYER_REG8_VALUE=$CORE_BAYER_REG8_VALUE" \
	"VIC_MDMA_QBUF_RING_STRIDE_OVERRIDE=$VIC_MDMA_QBUF_RING_STRIDE_OVERRIDE" \
	"VIC_MDMA_QBUF_RING_CTRL_VALUE=$VIC_MDMA_QBUF_RING_CTRL_VALUE" \
	"VIC_MDMA_QBUF_RING_UV_OFFSET_OVERRIDE=$VIC_MDMA_QBUF_RING_UV_OFFSET_OVERRIDE" \
	sh -s >"$LOG/load-safe.log" 2>&1 <<'EOS'
set -x
: "${FRAMECHAN_NEUTRAL_UV_ON_DONE:=0}"
: "${TISP_MAIN_INIT_TOP40_VALUE:=0x7fdfeeff}"
: "${TISP_MAIN_INIT_CSC_VERSION_VALUE:=2}"
: "${ENABLE_TISP_MAIN_INIT_COLOR_INITS:=0}"
: "${TISP_MAIN_INIT_COLOR_INIT_MASK:=0}"
: "${CSI_SETTLE_OVERRIDE:=0x1b}"
: "${CORE_BAYER_REG8_VALUE:=0x10002}"
: "${VIC_MDMA_QBUF_RING_STRIDE_OVERRIDE:=0}"
: "${VIC_MDMA_QBUF_RING_CTRL_VALUE:=0}"
: "${VIC_MDMA_QBUF_RING_UV_OFFSET_OVERRIDE:=0}"
/etc/init.d/S31raptor stop || true
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
insmod /tmp/tx_isp_t40_recovered.ko \
	t40_bringup_profile=1 \
	t40_profile_direct_vic_feed=0 \
	t40_profile_no_direct_irq_defaults=1 \
	t40_profile_isp_irq_passthrough=1 \
	t40_profile_force_vic_mdma_qbuf_ring=1 \
	force_core_bayer_reg8_value=1 \
	core_bayer_reg8_value="$CORE_BAYER_REG8_VALUE" \
	tisp_main_init_reg88_override=0xffffffff \
	enable_tisp_main_init_color_inits="$ENABLE_TISP_MAIN_INIT_COLOR_INITS" \
	tisp_main_init_color_init_mask="$TISP_MAIN_INIT_COLOR_INIT_MASK" \
	force_tisp_main_init_yuv_input_csc_version=0 \
	csi_settle_override="$CSI_SETTLE_OVERRIDE" \
	vic_mdma_qbuf_ring_stride_override="$VIC_MDMA_QBUF_RING_STRIDE_OVERRIDE" \
	vic_mdma_qbuf_ring_ctrl_value="$VIC_MDMA_QBUF_RING_CTRL_VALUE" \
	vic_mdma_qbuf_ring_uv_offset_override="$VIC_MDMA_QBUF_RING_UV_OFFSET_OVERRIDE" \
	framechan_neutral_uv_on_done="$FRAMECHAN_NEUTRAL_UV_ON_DONE"
PARAM=/sys/module/tx_isp_t40_recovered/parameters
echo "$TISP_MAIN_INIT_TOP40_VALUE" > "$PARAM/tisp_main_init_top40_value"
echo "$TISP_MAIN_INIT_CSC_VERSION_VALUE" > "$PARAM/tisp_main_init_csc_version_value"
insmod /lib/modules/4.4.94/ingenic/sensor_gc4653_t40.ko
/etc/init.d/S31raptor start
sleep 12
chmod +x /tmp/phys_memdump
cat /proc/interrupts | grep -E '(^ *3[89]:|tx|isp|vic)' || true
cat /proc/interrupts > /tmp/t40-interrupts-after.txt
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
	} > /tmp/t40-csi-vic-regs.txt 2>&1
else
	echo "devmem not found" > /tmp/t40-csi-vic-regs.txt
fi
set -x
dmesg | grep -E 'framechan0 (repaired )?qbuf|VIC frame MDMA qbuf ring|irq frame-done|CSI direct|csi_|settle|phy complete|vic_start_diag' | tail -180 > /tmp/t40-qbuf-lines.txt
dmesg | tail -260 > /tmp/t40-dmesg-tail.txt
EOS

"${SCP[@]}" "$USER@$IP:/tmp/t40-interrupts-after.txt" "$LOG/interrupts-after.txt"
"${SCP[@]}" "$USER@$IP:/tmp/t40-qbuf-lines.txt" "$LOG/qbuf-lines.txt"
"${SCP[@]}" "$USER@$IP:/tmp/t40-csi-vic-regs.txt" "$LOG/csi-vic-regs.txt"
"${SCP[@]}" "$USER@$IP:/tmp/t40-dmesg-tail.txt" "$LOG/dmesg-tail.txt"

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

dump_qbuf() {
	local dump_phys="$1"
	local name="$2"

	"${SSH[@]}" "/tmp/phys_memdump '$dump_phys' '$len' '/tmp/$name.bin'" \
		>"$LOG/dump-$name.log" 2>&1
	"${SCP[@]}" "$USER@$IP:/tmp/$name.bin" "$LOG/$name.bin"
	python3 tools/nv12_probe.py "$LOG/$name.bin" \
		--width 1920 --height 1080 --out-dir "$LOG/$name-renders" \
		>"$LOG/nv12-probe-$name.log"
}

dump_qbuf "$phys" qbuf-ch0
cp "$LOG/dump-qbuf-ch0.log" "$LOG/dump-qbuf.log"
cp "$LOG/nv12-probe-qbuf-ch0.log" "$LOG/nv12-probe.log"
ln -sfn qbuf-ch0-renders "$LOG/qbuf-renders"

for extra_phys in $QBUF_EXTRA_PHYS; do
	[[ "$extra_phys" == "$phys" ]] && continue
	extra_name="qbuf-${extra_phys//[^A-Za-z0-9]/_}"
	printf 'extra_phys=%s len=%s name=%s\n' \
		"$extra_phys" "$len" "$extra_name" | tee -a "$LOG/qbuf-dump.txt"
	dump_qbuf "$extra_phys" "$extra_name"
done

timeout 25 ffmpeg -hide_banner -loglevel info -y -rtsp_transport tcp \
	-i "rtsp://thingino:thingino@$IP/ch0" -frames:v 1 \
	"$LOG/rtsp-frame.jpg" >"$LOG/ffmpeg.log" 2>&1 || true

printf 'log=%s\n' "$LOG"
sed -n '1,20p' "$LOG/qbuf-dump.txt"
sed -n '1,20p' "$LOG/nv12-probe.log"
