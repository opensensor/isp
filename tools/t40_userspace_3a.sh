#!/bin/sh
# Userspace 3A loop for the T40 recovered driver. Runs ON the camera.
# Samples the NV12 output frame via /dev/mem (phys_memdump) and drives:
#  - exposure: ae_sensor_apply_force_packed = (again_idx<<16)|integration
#  - white balance: awb_manual_rgain/bgain (applied by frame-done on change)
# Both interfaces are 0644 module params; this cannot crash the kernel.
P=/sys/module/tx_isp_t40_recovered/parameters
PM=/tmp/phys_memdump
Y_PHYS=${Y_PHYS:-0x6a3e500}
UV_PHYS=${UV_PHYS:-0x6c3c500}
TARGET=${TARGET:-110}
HYST=${HYST:-14}
MAX_IT=${MAX_IT:-1919}
MIN_IT=${MIN_IT:-64}
MAX_AGAIN=${MAX_AGAIN:-25}
PERIOD=${PERIOD:-2}

it=600
again=0
rgain=$(cat $P/awb_grayworld_last_rgain 2>/dev/null || echo 1280)
bgain=$(cat $P/awb_grayworld_last_bgain 2>/dev/null || echo 1480)

# busybox od has no -t; use -d (unsigned 16-bit LE words) and split bytes in awk
mean_of() {  # phys len -> byte mean
	$PM "$1" "$2" 2>/dev/null | od -v -d | awk '
		{ for (i = 2; i <= NF; i += 8) { s += $i % 256 + int($i / 256); n += 2 } }
		END { if (n) printf "%d", s / n; else print -1 }'
}

uv_means() {  # phys len -> "umean vmean" (NV12: word = U | V<<8)
	$PM "$1" "$2" 2>/dev/null | od -v -d | awk '
		{ for (i = 2; i <= NF; i += 8) { u += $i % 256; v += int($i / 256); n++ } }
		END { if (n) printf "%d %d", u / n, v / n; else print "-1 -1" }'
}

i=0
while true; do
	sleep "$PERIOD"
	luma=$(mean_of $Y_PHYS 0x40000)
	[ "$luma" -lt 0 ] && continue
	# AE ladder
	if [ $((luma + HYST)) -lt "$TARGET" ]; then
		if [ "$it" -lt "$MAX_IT" ]; then
			it=$((it + it / 4 + 8)); [ "$it" -gt "$MAX_IT" ] && it=$MAX_IT
		elif [ "$again" -lt "$MAX_AGAIN" ]; then
			again=$((again + 1))
		fi
		echo $(( (again << 16) | it )) > $P/ae_sensor_apply_force_packed
	elif [ "$luma" -gt $((TARGET + HYST)) ]; then
		if [ "$again" -gt 0 ]; then
			again=$((again - 1))
		elif [ "$it" -gt "$MIN_IT" ]; then
			it=$((it - it / 4)); [ "$it" -lt "$MIN_IT" ] && it=$MIN_IT
		fi
		echo $(( (again << 16) | it )) > $P/ae_sensor_apply_force_packed
	fi
	# AWB trim every other tick
	i=$((i + 1))
	if [ $((i % 2)) -eq 0 ]; then
		set -- $(uv_means $UV_PHYS 0x20000)
		u=$1; v=$2
		[ "$u" -lt 0 ] && continue
		ch=0
		if [ "$v" -gt 136 ] && [ "$rgain" -gt 512 ]; then
			rgain=$((rgain - rgain / 32)); ch=1
		elif [ "$v" -lt 120 ] && [ "$rgain" -lt 6144 ]; then
			rgain=$((rgain + rgain / 32)); ch=1
		fi
		if [ "$u" -gt 136 ] && [ "$bgain" -gt 512 ]; then
			bgain=$((bgain - bgain / 32)); ch=1
		elif [ "$u" -lt 120 ] && [ "$bgain" -lt 6144 ]; then
			bgain=$((bgain + bgain / 32)); ch=1
		fi
		if [ "$ch" -eq 1 ]; then
			echo "$rgain" > $P/awb_manual_rgain
			echo "$bgain" > $P/awb_manual_bgain
		fi
		echo "3a: luma=$luma u=$u v=$v it=$it again=$again rgain=$rgain bgain=$bgain"
	fi
done
