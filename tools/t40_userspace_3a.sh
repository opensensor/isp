#!/bin/sh
# Userspace 3A loop for the T40 recovered driver. Runs ON the camera.
# Proportional AE on the NV12 Y plane + gray-world WB trim on the UV plane,
# sampled via /dev/mem (phys_memdump). Drives the recovered module's 0644
# params only - cannot crash the kernel.
P=/sys/module/tx_isp_t40_recovered/parameters
PM=/tmp/phys_memdump
Y_PHYS=${Y_PHYS:-0x6a3e500}
UV_PHYS=${UV_PHYS:-0x6c3c500}
TARGET=${TARGET:-110}
DEADBAND_PCT=${DEADBAND_PCT:-8}
MAX_IT=${MAX_IT:-1919}
MIN_IT=${MIN_IT:-64}
MAX_AGAIN=${MAX_AGAIN:-25}
PERIOD=${PERIOD:-1}

# resume from the last applied exposure so a restart doesn't slam the sensor
prev=$(cat $P/ae_sensor_apply_force_packed 2>/dev/null || echo 0)
if [ "$prev" -gt 0 ]; then
	it=$(( prev & 0xffff ))
	again=$(( prev >> 16 ))
else
	it=600
	again=0
fi
luma_ema=-1
rgain=$(cat $P/awb_grayworld_last_rgain 2>/dev/null || echo 1280)
bgain=$(cat $P/awb_grayworld_last_bgain 2>/dev/null || echo 1480)

# busybox od has no -t/-A; use -d (unsigned 16-bit LE words), skip addr column
mean_of() {  # phys len -> byte mean
	$PM "$1" "$2" 2>/dev/null | od -v -d | awk '
		{ for (i = 2; i <= NF; i += 4) { s += $i % 256 + int($i / 256); n += 2 } }
		END { if (n) printf "%d", s / n; else print -1 }'
}

uv_means() {  # phys len -> "umean vmean" (NV12: word = U | V<<8)
	$PM "$1" "$2" 2>/dev/null | od -v -d | awk '
		{ for (i = 2; i <= NF; i += 4) { u += $i % 256; v += int($i / 256); n++ } }
		END { if (n) printf "%d %d", u / n, v / n; else print "-1 -1" }'
}

apply_expo() {
	echo $(( (again << 16) | it )) > $P/ae_sensor_apply_force_packed
}

i=0
while true; do
	sleep "$PERIOD"
	luma=$(mean_of $Y_PHYS 0x40000)
	[ "$luma" -lt 0 ] && continue
	[ "$luma" -lt 1 ] && luma=1
	# EMA over two ticks to stop noise-driven hunting
	if [ "$luma_ema" -lt 0 ]; then luma_ema=$luma; fi
	luma_ema=$(( (luma_ema + luma) / 2 ))

	# proportional AE: scale exposure by target/luma, slew-limited per tick
	err_pct=$(( (TARGET - luma_ema) * 100 / TARGET ))
	abs_err=${err_pct#-}
	if [ "$abs_err" -gt "$DEADBAND_PCT" ]; then
		# scale factor in percent, clamped to +/-15% per tick for smoothness
		scale=$(( luma_ema > 0 ? TARGET * 100 / luma_ema : 200 ))
		[ "$scale" -gt 115 ] && scale=115
		[ "$scale" -lt 87 ] && scale=87
		if [ "$scale" -gt 100 ]; then
			if [ "$it" -lt "$MAX_IT" ]; then
				it=$(( it * scale / 100 + 1 ))
				[ "$it" -gt "$MAX_IT" ] && it=$MAX_IT
			elif [ "$again" -lt "$MAX_AGAIN" ] && [ "$scale" -ge 110 ]; then
				again=$(( again + 1 ))
			fi
			apply_expo
		else
			if [ "$again" -gt 0 ] && [ "$scale" -le 90 ]; then
				# gross overexposure: shed gain fast
				if [ "$scale" -le 75 ] && [ "$again" -ge 3 ]; then
					again=$(( again - 3 ))
				else
					again=$(( again - 1 ))
				fi
			elif [ "$it" -gt "$MIN_IT" ]; then
				it=$(( it * scale / 100 ))
				[ "$it" -lt "$MIN_IT" ] && it=$MIN_IT
			fi
			apply_expo
		fi
	fi

	# AWB trim every other tick; EMA the UV means (the fixed-phys sample
	# alternates ring buffers and jitters) and use a tight neutral window
	i=$((i + 1))
	if [ $((i % 2)) -eq 0 ]; then
		set -- $(uv_means $UV_PHYS 0x20000)
		u=$1; v=$2
		[ "$u" -lt 0 ] && continue
		if [ "${u_ema:--1}" -lt 0 ]; then u_ema=$u; v_ema=$v; fi
		u_ema=$(( (u_ema * 3 + u) / 4 ))
		v_ema=$(( (v_ema * 3 + v) / 4 ))
		ch=0
		if [ "$v_ema" -gt 133 ] && [ "$rgain" -gt 512 ]; then
			rgain=$((rgain - rgain / 48)); ch=1
		elif [ "$v_ema" -lt 119 ] && [ "$rgain" -lt 6144 ]; then
			rgain=$((rgain + rgain / 48)); ch=1
		fi
		if [ "$u_ema" -gt 133 ] && [ "$bgain" -gt 512 ]; then
			bgain=$((bgain - bgain / 48)); ch=1
		elif [ "$u_ema" -lt 119 ] && [ "$bgain" -lt 6144 ]; then
			bgain=$((bgain + bgain / 48)); ch=1
		fi
		if [ "$ch" -eq 1 ]; then
			echo "$rgain" > $P/awb_manual_rgain
			echo "$bgain" > $P/awb_manual_bgain
		fi
		echo "3a: luma=$luma ema=$luma_ema u=$u_ema v=$v_ema it=$it again=$again rgain=$rgain bgain=$bgain"
	fi
done
