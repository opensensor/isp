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
# UV neutral window for the WB trim; set UV_LO=0 UV_HI=255 to freeze gains
UV_LO=${UV_LO:-121}
UV_HI=${UV_HI:-133}

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
[ "$rgain" -le 0 ] 2>/dev/null && rgain=1280
[ "$bgain" -le 0 ] 2>/dev/null && bgain=1480

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

apply_expo
echo "$rgain" > $P/awb_manual_rgain 2>/dev/null
echo "$bgain" > $P/awb_manual_bgain 2>/dev/null

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
	# alternates ring buffers and jitters) and use a tight neutral window.
	# Bright-gated gray-world: average UV only over blocks whose Y mean is
	# bright-but-not-clipped. White surfaces are the only trustworthy gray
	# references — a whole-frame mean lets a dominant colored wall pull the
	# trim and tints the true whites (the OEM AWB likewise excludes
	# saturated/dark pixels via its regional thresholds). Falls back to the
	# all-block mean when nothing qualifies (e.g. night).
	i=$((i + 1))
	if [ $((i % 2)) -eq 0 ]; then
		# Near-gray candidate selection (what the OEM AWB's regional
		# thresholds do): a gray reference block is bright-ish (>= frame
		# mean, not clipped) AND already near-neutral in chroma. A
		# dominant colored wall fails the chroma test, so it cannot pull
		# the trim and tint the true whites. Two passes: strict dev<=24,
		# relaxed dev<=48; if nothing qualifies for 10 straight ticks,
		# take one global gray-world step as a bootstrap escape hatch.
		# Among candidates (bright-ish, unclipped, roughly neutral) use
		# only the TWO BRIGHTEST blocks. Averaging every candidate lets a
		# dominant colored wall sneak in as soon as the gains make it
		# near-neutral — a feedback loop that whitewashes the wall color
		# and overshoots the true whites (green walls drove the trim to
		# purple). The ceiling/whites are reliably the brightest unclipped
		# surfaces, so brightest-2 stays locked to them.
		y1=-1; u1=0; v1=0; y2=-1; u2=0; v2=0; au=0; av=0; an=0
		ygate=$luma_ema
		[ "$ygate" -gt 220 ] && ygate=220
		for blk in "135 240" "135 960" "135 1680" \
			   "405 240" "405 960" "405 1680" \
			   "675 240" "675 960" "675 1680" \
			   "945 240" "945 960" "945 1680"; do
			set -- $blk
			r=$1; c=$2
			yb=$(mean_of $(( Y_PHYS + r * 1920 + c )) 0x1000)
			[ "$yb" -lt 0 ] && continue
			set -- $(uv_means $(( UV_PHYS + r / 2 * 1920 + c )) 0x1000)
			[ "$1" -lt 0 ] && continue
			au=$(( au + $1 )); av=$(( av + $2 )); an=$(( an + 1 ))
			[ "$yb" -lt "$ygate" ] && continue
			[ "$yb" -gt 235 ] && continue
			du=$(( $1 - 128 )); du=${du#-}
			dv=$(( $2 - 128 )); dv=${dv#-}
			[ $(( du + dv )) -gt 48 ] && continue
			if [ "$yb" -gt "$y1" ]; then
				y2=$y1; u2=$u1; v2=$v1
				y1=$yb; u1=$1; v1=$2
			elif [ "$yb" -gt "$y2" ]; then
				y2=$yb; u2=$1; v2=$2
			fi
		done
		if [ "$y2" -ge 0 ]; then
			u=$(( (u1 + u2) / 2 )); v=$(( (v1 + v2) / 2 )); bn=2; hold=0
		elif [ "$y1" -ge 0 ]; then
			u=$u1; v=$v1; bn=1; hold=0
		elif [ "$an" -gt 0 ] && [ "$(( ${hold:-0} + 1 ))" -gt 10 ]; then
			u=$(( au / an )); v=$(( av / an )); bn=0; hold=0
		else
			hold=$(( ${hold:-0} + 1 ))
			echo "3a: luma=$luma ema=$luma_ema bn=0 hold=$hold it=$it again=$again rgain=$rgain bgain=$bgain"
			continue
		fi
		if [ "${u_ema:--1}" -lt 0 ]; then u_ema=$u; v_ema=$v; fi
		u_ema=$(( (u_ema * 3 + u) / 4 ))
		v_ema=$(( (v_ema * 3 + v) / 4 ))
		ch=0
		if [ "$v_ema" -gt "$UV_HI" ] && [ "$rgain" -gt 512 ]; then
			rgain=$((rgain - rgain / 48)); ch=1
		elif [ "$v_ema" -lt "$UV_LO" ] && [ "$rgain" -lt 6144 ]; then
			rgain=$((rgain + rgain / 48)); ch=1
		fi
		if [ "$u_ema" -gt "$UV_HI" ] && [ "$bgain" -gt 512 ]; then
			bgain=$((bgain - bgain / 48)); ch=1
		elif [ "$u_ema" -lt "$UV_LO" ] && [ "$bgain" -lt 6144 ]; then
			bgain=$((bgain + bgain / 48)); ch=1
		fi
		if [ "$ch" -eq 1 ]; then
			echo "$rgain" > $P/awb_manual_rgain
			echo "$bgain" > $P/awb_manual_bgain
		fi
		echo "3a: luma=$luma ema=$luma_ema u=$u_ema v=$v_ema bn=$bn it=$it again=$again rgain=$rgain bgain=$bgain"
	fi
done
