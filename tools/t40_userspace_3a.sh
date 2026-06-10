#!/bin/sh
# Userspace 3A loop for the T40 recovered driver. Runs ON the camera.
# Proportional AE on the NV12 Y plane + gray-world WB trim on the UV plane,
# sampled via /dev/mem (phys_memdump). Drives the recovered module's 0644
# params only - cannot crash the kernel.
P=/sys/module/tx_isp_t40_recovered/parameters
PM=/tmp/phys_memdump
Y_PHYS=${Y_PHYS:-0x6a3e500}
UV_PHYS=${UV_PHYS:-0x6c3c500}
TARGET=${TARGET:-105}
DEADBAND_PCT=${DEADBAND_PCT:-5}
MAX_IT=${MAX_IT:-1919}
MIN_IT=${MIN_IT:-64}
MAX_AGAIN=${MAX_AGAIN:-25}
PERIOD=${PERIOD:-1}
# Anti-flicker: quantize integration time to multiples of the mains light
# period (120Hz flicker; ~20.8us/line at 25fps/1920 total lines -> ~400
# lines per period). Arbitrary IT values produce rolling brightness bands.
FLICKER_STEP=${FLICKER_STEP:-400}

# resume from the last applied exposure so a restart doesn't slam the sensor
prev=$(cat $P/ae_sensor_apply_force_packed 2>/dev/null || echo 0)
if [ "$prev" -gt 0 ]; then
	it=$(( prev & 0xffff ))
	again=$(( prev >> 16 ))
else
	it=800
	again=0
fi
rung=$(( (it + FLICKER_STEP / 2) / FLICKER_STEP * FLICKER_STEP ))
[ "$rung" -lt "$FLICKER_STEP" ] && rung=$FLICKER_STEP
fine=0
settle=0
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
	it=$(( rung + fine ))
	[ "$it" -lt "$MIN_IT" ] && it=$MIN_IT
	[ "$it" -gt "$MAX_IT" ] && it=$MAX_IT
	echo $(( (again << 16) | it )) > $P/ae_sensor_apply_force_packed
	# tell the kernel the gain EV so YDNS/YSP strength tracks it
	# (~0.158 log2 per GC4653 analog gain index -> ~10362 in 16.16)
	echo $(( again * 24576 )) > $P/dns_gain_ev 2>/dev/null
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
		# Two-level control: integration micro-trim (+/-80 lines around the
		# flicker rung, 16-line steps ~ 1% luma each, negligible residual
		# banding at <=20% of a period) does fine corrections; analog gain
		# (12-14%/step) and whole rungs do coarse moves. After any coarse
		# move, hold for settle ticks so the EMA catches up (no pumping).
		max_rung=$(( MAX_IT / FLICKER_STEP * FLICKER_STEP ))
		if [ "${settle:-0}" -gt 0 ]; then
			settle=$(( settle - 1 ))
		elif [ "$err_pct" -gt 0 ]; then
			if [ "$fine" -lt 80 ]; then
				fine=$(( fine + 16 ))
			elif [ "$rung" -lt "$max_rung" ]; then
				rung=$(( rung + FLICKER_STEP )); fine=-80; settle=4
			elif [ "$again" -lt "$MAX_AGAIN" ]; then
				again=$(( again + 1 )); fine=-80; settle=4
			fi
			apply_expo
		else
			if [ "$fine" -gt -80 ]; then
				fine=$(( fine - 16 ))
			elif [ "$again" -gt 0 ]; then
				again=$(( again - 1 )); fine=80; settle=4
			elif [ "$rung" -gt "$FLICKER_STEP" ]; then
				rung=$(( rung - FLICKER_STEP )); fine=80; settle=4
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
