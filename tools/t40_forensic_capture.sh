#!/usr/bin/env bash
set -euo pipefail

# Capture a settled T40 candidate with the same evidence set used for the
# same-session stock oracle, score it, and leave the camera at a clean boot.

IP="${THINGINO_IP:-192.168.50.242}"
USER="${THINGINO_USER:-root}"
PASS="${THINGINO_PASS:-}"
LOG="${1:?usage: t40_forensic_capture.sh LOG_DIR}"
RTSP_PATH="${RTSP_PATH:-ch0}"
FRAME_COUNT="${FRAME_COUNT:-90}"
CAPTURE_FRAMES="${CAPTURE_FRAMES:-1}"
REBOOT_AFTER="${REBOOT_AFTER:-1}"
STOCK_GLOB="${STOCK_GLOB:-logs/20260718-t40-forensic-stock-baseline/frames/stock-*.jpg}"

if [[ "$IP" != "192.168.50.242" ]]; then
	echo "refusing non-target IP: $IP" >&2
	exit 2
fi
if [[ -z "$PASS" ]]; then
	echo "THINGINO_PASS is required" >&2
	exit 2
fi
for numeric in FRAME_COUNT CAPTURE_FRAMES REBOOT_AFTER; do
	if [[ ! "${!numeric}" =~ ^[0-9]+$ ]]; then
		echo "$numeric must be decimal" >&2
		exit 2
	fi
done

export SSHPASS="$PASS"
SSH=(sshpass -e ssh -T -o LogLevel=ERROR -o StrictHostKeyChecking=no
	-o UserKnownHostsFile=/dev/null -o ConnectTimeout=5 -o ConnectionAttempts=1
	"$USER@$IP")
SCP=(sshpass -e scp -O -q -o LogLevel=ERROR -o StrictHostKeyChecking=no
	-o UserKnownHostsFile=/dev/null -o ConnectTimeout=5 -o ConnectionAttempts=1)

mkdir -p "$LOG/frames"
[[ -x tools/phys_memdump.mipsel ]] || {
	echo "tools/phys_memdump.mipsel is missing; build it with the T40 harness" >&2
	exit 2
}
# Upload to a side path and atomically replace the pathname.  The live 3A loop
# may be executing the old inode, in which case SCP directly onto it fails with
# ETXTBSY; rename is safe and the next 3A sample opens the new copy.
"${SCP[@]}" tools/phys_memdump.mipsel "$USER@$IP:/tmp/phys_memdump.forensic"
"${SSH[@]}" 'chmod +x /tmp/phys_memdump.forensic; mv -f /tmp/phys_memdump.forensic /tmp/phys_memdump'

if [[ "$CAPTURE_FRAMES" == "1" ]]; then
	timeout 20 ffmpeg -nostdin -hide_banner -loglevel info -y \
		-rtsp_transport tcp \
		-i "rtsp://thingino:thingino@$IP:554/$RTSP_PATH" -an \
		-vsync 0 -q:v 2 -frames:v "$FRAME_COUNT" \
		"$LOG/frames/candidate-%03d.jpg" >"$LOG/ffmpeg-burst.log" 2>&1
fi

"${SSH[@]}" sh -s <<'EOS'
set -eu
{
	echo "# sensor $(date)"
	for spec in \
		0202:02:02 0203:02:03 0205:02:05 \
		02b3:02:b3 02b4:02:b4 02b8:02:b8 02b9:02:b9 \
		0515:05:15 0519:05:19 02d9:02:d9 020e:02:0e 020f:02:0f \
		0218:02:18 0219:02:19 0340:03:40 0341:03:41; do
		reg=${spec%%:*}
		tail=${spec#*:}
		hi=${tail%%:*}
		lo=${tail#*:}
		printf '0x%s ' "$reg"
		i2ctransfer -f -y 1 w2@0x29 "0x$hi" "0x$lo" r1 2>/dev/null || printf ERR
		printf '\n'
	done
	echo "# interrupts"
	cat /proc/interrupts
	echo "# params"
	for p in ae_sensor_apply_force_packed awb_manual_rgain awb_manual_bgain \
		dns_gain_ev ysp_gain_ev ysp_gain_ev_now lsc_lit_gain lsc_lit_ct lsc_lit_ct_now ccm_ct ccm_ct_now \
		ccm_ev ccm_ev_now \
		enable_blc_lit blc_lit_gain blc_lit_gain_now \
		enable_gib_lit gib_lit_gain gib_lit_gain_now gib_lit_final_gain \
		enable_dmsc_static enable_dmsc_lit dmsc_lit_gain dmsc_lit_gain_now \
		enable_bcsh bcsh_saturation bcsh_ev bcsh_ct \
		enable_clm clm_ct clm_ct_now clm_ct_region clm_stage_reached frame_3a_completed_seq \
		frame_3a_completed_y_phys frame_3a_completed_uv_phys; do
		printf '%s=' "$p"
		cat "/sys/module/tx_isp_t40_recovered/parameters/$p" 2>/dev/null || printf ERR
	done
} > /tmp/t40-forensic-state.txt
/tmp/phys_memdump 0x13300000 0x20000 /tmp/t40-forensic-core-0x20000.bin
sleep 2
/tmp/phys_memdump 0x13300000 0x20000 /tmp/t40-forensic-core-0x20000-t2.bin
dmesg > /tmp/t40-forensic-dmesg.txt
logread > /tmp/t40-forensic-logread.txt
cat /proc/interrupts > /tmp/t40-forensic-interrupts.txt
EOS

for name in state.txt core-0x20000.bin core-0x20000-t2.bin dmesg.txt \
	logread.txt interrupts.txt; do
	"${SCP[@]}" "$USER@$IP:/tmp/t40-forensic-$name" "$LOG/candidate-$name"
done
"${SCP[@]}" "$USER@$IP:/tmp/3a.log" "$LOG/candidate-3a.log" 2>/dev/null || true

python3 tools/t40_frame_forensics.py \
	--stock "$STOCK_GLOB" \
	--candidate "$LOG/frames/candidate-*.jpg" \
	--output "$LOG/forensics-vs-stock.json"

rg -n -i 'kernel panic|oops|BUG:|unable to handle|segfault|machine check|watchdog|fatal exception' \
	"$LOG/candidate-dmesg.txt" >"$LOG/kernel-fatal-signatures.txt" || true
# Keep application-level fatal messages for diagnosis, but do not count old
# Raptor startup/ONVIF failures as kernel safety-gate failures.
rg -n -i '\[FATAL\]|fatal' "$LOG/candidate-logread.txt" \
	>"$LOG/service-fatal-signatures.txt" || true

if [[ "$REBOOT_AFTER" == "1" ]]; then
	timeout 12 "${SSH[@]}" 'sync; reboot -f' || true
fi

echo "log=$LOG"
echo "frames=$(find "$LOG/frames" -maxdepth 1 -name 'candidate-*.jpg' | wc -l)"
echo "kernel_fatal_signatures=$(wc -l < "$LOG/kernel-fatal-signatures.txt")"
echo "service_fatal_signatures=$(wc -l < "$LOG/service-fatal-signatures.txt")"
