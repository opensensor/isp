#!/usr/bin/env bash
set -euo pipefail

IP="${THINGINO_IP:-192.168.50.242}"
USER="${THINGINO_USER:-root}"
PASS="${THINGINO_PASS:-}"
ROOT="${ROOT:-/home/matteius/output/wyze_cam3pro_nor_t40xp_gc4653_rtl8192fs}"
SOC="${SOC:-t40}"
QBUF_PHYS_FALLBACK="${QBUF_PHYS_FALLBACK:-0x6bab300}"
QBUF_LEN_FALLBACK="${QBUF_LEN_FALLBACK:-0x2fd000}"
LOG="${1:-logs/$(date +%Y%m%d-%H%M%S)-t40-safe-qbuf-dump-242}"

if [[ "$IP" != "192.168.50.242" ]]; then
	echo "refusing non-target IP: $IP" >&2
	exit 2
fi
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

"${SSH[@]}" sh -s >"$LOG/load-safe.log" 2>&1 <<'EOS'
set -x
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
	core_bayer_reg8_value=0x10008 \
	tisp_main_init_reg88_override=0xffffffff \
	enable_tisp_main_init_color_inits=0 \
	tisp_main_init_color_init_mask=0 \
	force_tisp_main_init_yuv_input_csc_version=0 \
	framechan_neutral_uv_on_done=0
PARAM=/sys/module/tx_isp_t40_recovered/parameters
echo 0x7fdfe8ff > "$PARAM/tisp_main_init_top40_value"
echo 2 > "$PARAM/tisp_main_init_csc_version_value"
insmod /lib/modules/4.4.94/ingenic/sensor_gc4653_t40.ko
/etc/init.d/S31raptor start
sleep 12
chmod +x /tmp/phys_memdump
cat /proc/interrupts | grep -E '(^ *3[89]:|tx|isp|vic)' || true
cat /proc/interrupts > /tmp/t40-interrupts-after.txt
dmesg | grep -E 'framechan0 (repaired )?qbuf|VIC frame MDMA qbuf ring|irq frame-done' | tail -120 > /tmp/t40-qbuf-lines.txt
EOS

"${SCP[@]}" "$USER@$IP:/tmp/t40-interrupts-after.txt" "$LOG/interrupts-after.txt"
"${SCP[@]}" "$USER@$IP:/tmp/t40-qbuf-lines.txt" "$LOG/qbuf-lines.txt"

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
"${SSH[@]}" "/tmp/phys_memdump '$phys' '$len' /tmp/qbuf-ch0.bin" \
	>"$LOG/dump-qbuf.log" 2>&1
"${SCP[@]}" "$USER@$IP:/tmp/qbuf-ch0.bin" "$LOG/qbuf-ch0.bin"

python3 tools/nv12_probe.py "$LOG/qbuf-ch0.bin" \
	--width 1920 --height 1080 --out-dir "$LOG/qbuf-renders" \
	>"$LOG/nv12-probe.log"

timeout 25 ffmpeg -hide_banner -loglevel info -y -rtsp_transport tcp \
	-i "rtsp://thingino:thingino@$IP/ch0" -frames:v 1 \
	"$LOG/rtsp-frame.jpg" >"$LOG/ffmpeg.log" 2>&1 || true

printf 'log=%s\n' "$LOG"
sed -n '1,20p' "$LOG/qbuf-dump.txt"
sed -n '1,20p' "$LOG/nv12-probe.log"
