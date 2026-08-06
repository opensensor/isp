#!/bin/sh
# Low-overhead, read-only ISP pipeline baseline for Thingino/Ingenic devices.
# POSIX sh + BusyBox userspace; no compiler or debug kernel features required.

set -u

VERSION=2
DURATION=${BENCH_DURATION:-60}
INTERVAL=${BENCH_INTERVAL:-10}
WARMUP=${BENCH_WARMUP:-15}
OUTPUT=${BENCH_OUTPUT:-}
LABEL=${BENCH_LABEL:-baseline}
MODULE_FILE=${BENCH_MODULE_FILE:-}
RING_NAME=${BENCH_RING:-main}
TRANSPORT=${BENCH_TRANSPORT:-imp-shm-ring}
PROCESSES=${BENCH_PROCESSES:-"rvd rsd rwd rad ric"}

usage()
{
	cat <<'EOF'
Usage: isp_benchmark_device.sh [options]

  -d SEC   measurement duration (default: 60)
  -i SEC   sample interval (default: 10)
  -w SEC   warm-up before measurement (default: 15)
  -o DIR   output directory (default: /tmp/isp-benchmark-<UTC time>)
  -l NAME  run label, such as open-t41 or oem-t41
  -m FILE  exact on-disk ISP module used for this run
  -r NAME  Raptor video ring name (default: main)
  -t NAME  consumer/transport label (default: imp-shm-ring)
  -p LIST  quoted process-name list
  -h        show this help

Environment variables with the BENCH_ prefix provide the same settings.
Pass -m explicitly for comparable module file size/hash results.
EOF
}

while getopts "d:i:w:o:l:m:r:t:p:h" opt; do
	case "$opt" in
		d) DURATION=$OPTARG ;;
		i) INTERVAL=$OPTARG ;;
		w) WARMUP=$OPTARG ;;
		o) OUTPUT=$OPTARG ;;
		l) LABEL=$OPTARG ;;
		m) MODULE_FILE=$OPTARG ;;
		r) RING_NAME=$OPTARG ;;
		t) TRANSPORT=$OPTARG ;;
		p) PROCESSES=$OPTARG ;;
		h) usage; exit 0 ;;
		*) usage >&2; exit 2 ;;
	esac
done

is_uint()
{
	case "$1" in
		''|*[!0-9]*) return 1 ;;
		*) return 0 ;;
	esac
}

if ! is_uint "$DURATION" || [ "$DURATION" -eq 0 ] ||
   ! is_uint "$INTERVAL" || [ "$INTERVAL" -eq 0 ] ||
   ! is_uint "$WARMUP"; then
	echo "duration and interval must be positive integers; warm-up must be an integer" >&2
	exit 2
fi

utc_stamp=$(date -u +%Y%m%dT%H%M%SZ 2>/dev/null || date +%Y%m%d-%H%M%S)
if [ -z "$OUTPUT" ]; then
	OUTPUT="/tmp/isp-benchmark-$utc_stamp"
fi
if [ -e "$OUTPUT" ]; then
	echo "refusing to overwrite existing output: $OUTPUT" >&2
	exit 2
fi
mkdir -p "$OUTPUT" || exit 1
WORK="$OUTPUT/.state"
mkdir -p "$WORK" || exit 1

metadata="$OUTPUT/metadata.tsv"
summary="$OUTPUT/summary.tsv"
samples="$OUTPUT/samples.csv"
process_samples="$OUTPUT/process_samples.csv"
irq_samples="$OUTPUT/irq_samples.csv"
files="$OUTPUT/pipeline_files.tsv"

clean_value()
{
	printf '%s' "$1" | tr '\t\r\n' '   '
}

meta()
{
	printf '%s\t' "$1" >> "$metadata"
	clean_value "$2" >> "$metadata"
	printf '\n' >> "$metadata"
}

command_capture()
{
	name=$1
	shift
	"$@" > "$OUTPUT/$name" 2>&1 || true
}

uptime_s()
{
	awk '{ print $1; exit }' /proc/uptime
}

cpu_state()
{
	awk '/^cpu / {
		total = 0
		for (i = 2; i <= NF; i++) total += $i
		idle = $5 + 0
		iowait = $6 + 0
		printf "%.0f %.0f %.0f\n", total, idle, iowait
		exit
	}' /proc/stat
}

mem_value()
{
	key=$1
	awk -v key="$key" '$1 == key ":" { print $2; found=1; exit }
		END { if (!found) print 0 }' /proc/meminfo
}

mem_csv()
{
	awk '
		$1 == "MemAvailable:" { available=$2 }
		$1 == "MemFree:" { free=$2 }
		$1 == "Cached:" { cached=$2 }
		$1 == "Shmem:" { shmem=$2 }
		$1 == "Slab:" { slab=$2 }
		$1 == "SReclaimable:" { sreclaim=$2 }
		$1 == "SUnreclaim:" { sunreclaim=$2 }
		$1 == "KernelStack:" { kstack=$2 }
		$1 == "PageTables:" { pagetables=$2 }
		END { printf "%d,%d,%d,%d,%d,%d,%d,%d,%d", available, free, cached,
			shmem, slab, sreclaim, sunreclaim, kstack, pagetables }
	' /proc/meminfo
}

frame_sequence()
{
	if [ -r "/dev/shm/rss_ring_$RING_NAME" ] && command -v hexdump >/dev/null 2>&1; then
		# Raptor ring v1-v4 begins with the little-endian 64-bit write_seq.
		# Reading the low word avoids acquiring a reader slot or copying frames.
		hexdump -s 0 -n 4 -e '1/4 "%u\n"' "/dev/shm/rss_ring_$RING_NAME" 2>/dev/null
	else
		printf 'NA\n'
	fi
}

frame_delta()
{
	previous=$1
	current=$2
	if [ "$previous" = NA ] || [ "$current" = NA ] ||
	   ! is_uint "$previous" || ! is_uint "$current"; then
		printf 'NA\n'
	else
		# Use awk rather than shell arithmetic because a 32-bit BusyBox shell
		# may treat the unsigned low word as negative above INT32_MAX.
		awk -v a="$previous" -v b="$current" 'BEGIN {
			if (b >= a) printf "%.0f\n", b-a
			else printf "%.0f\n", b + 4294967296 - a
		}'
	fi
}

find_pid()
{
	name=$1
	if command -v pidof >/dev/null 2>&1; then
		pidof "$name" 2>/dev/null | awk '{ print $1; exit }'
	else
		for p in /proc/[0-9]*; do
			[ -r "$p/comm" ] || continue
			[ "$(cat "$p/comm" 2>/dev/null)" = "$name" ] && {
				basename "$p"
				break
			}
		done
	fi
}

proc_state()
{
	pid=$1
	awk -v pid="$pid" 'BEGIN {
		stat_path = "/proc/" pid "/stat"
		status_path = "/proc/" pid "/status"
		if ((getline line < stat_path) <= 0) exit 1
		close(stat_path)
		n = split(line, a, " ")
		if (n < 24) exit 1
		ticks = a[14] + a[15]
		while ((getline line < status_path) > 0) {
			split(line, b, /[ \t]+/)
			if (b[1] == "VmRSS:") rss=b[2]
			else if (b[1] == "VmSize:") vmsize=b[2]
			else if (b[1] == "Threads:") threads=b[2]
			else if (b[1] == "voluntary_ctxt_switches:") vol=b[2]
			else if (b[1] == "nonvoluntary_ctxt_switches:") nvol=b[2]
		}
		close(status_path)
		printf "%.0f,%d,%d,%d,%.0f,%.0f\n", ticks, rss, vmsize, threads, vol, nvol
	}'
}

discover_processes()
{
	: > "$WORK/process.prev"
	for name in $PROCESSES; do
		pid=$(find_pid "$name")
		[ -n "$pid" ] || continue
		state=$(proc_state "$pid") || continue
		printf '%s,%s,%s\n' "$name" "$pid" "$state" >> "$WORK/process.prev"
	done
}

discover_irqs()
{
	awk -v cpus="$cpu_count" '
		/^[ \t]*[0-9]+:/ {
			line=tolower($0)
			class=""
			if (line ~ /isp|tx[_-]isp|[[:space:]]vic([[:space:]]|$)|[[:space:]]csi([[:space:]]|$)/)
				class="isp"
			else if (line ~ /avpu|vpu|jpeg/)
				class="codec"
			if (class == "") next
			irq=$1; sub(/:$/, "", irq)
			count=0
			for (i=2; i<=1+cpus; i++) count += $i
			label=$NF
			gsub(/,/, ";", label)
			printf "%s,%s,%s,%.0f\n", class, irq, label, count
		}' /proc/interrupts > "$WORK/irq.prev"
}

irq_count()
{
	irq=$1
	awk -v irq="$irq" -v cpus="$cpu_count" '$1 == irq ":" {
		count=0
		for (i=2; i<=1+cpus; i++) count += $i
		printf "%.0f\n", count
		exit
	}' /proc/interrupts
}

file_row()
{
	role=$1
	path=$2
	method=$3
	[ -f "$path" ] || return 0
	size=$(stat -c %s "$path" 2>/dev/null || wc -c < "$path")
	if command -v sha256sum >/dev/null 2>&1; then
		hash=$(sha256sum "$path" 2>/dev/null | awk '{ print $1 }')
	else
		hash=unavailable
	fi
	printf '%s\t%s\t%s\t%s\t%s\n' "$role" "$path" "$size" "$hash" "$method" >> "$files"
}

error_count()
{
	pattern=$1
	file=$2
	awk -v pattern="$pattern" 'BEGIN { IGNORECASE=1 }
		tolower($0) ~ pattern { count++ }
		END { print count + 0 }' "$file"
}

printf 'key\tvalue\n' > "$metadata"
printf 'role\tpath\tsize_bytes\tsha256\tselection_method\n' > "$files"
meta benchmark_version "$VERSION"
meta label "$LABEL"
meta transport "$TRANSPORT"
meta requested_duration_s "$DURATION"
meta sample_interval_s "$INTERVAL"
meta warmup_s "$WARMUP"
meta ring_name "$RING_NAME"
meta process_names "$PROCESSES"
meta started_utc "$(date -u +%Y-%m-%dT%H:%M:%SZ 2>/dev/null || date)"
meta hostname "$(hostname 2>/dev/null || echo unknown)"
meta uname "$(uname -a 2>/dev/null || echo unknown)"

cpu_count=$(awk '/^cpu[0-9]+ / { n++ } END { print n ? n : 1 }' /proc/stat)
meta logical_cpu_count "$cpu_count"
meta page_size_bytes "4096 (Ingenic kernel ABI; getconf is not required)"
meta module_file_explicit "$([ -n "$MODULE_FILE" ] && echo yes || echo no)"

command_capture cpuinfo.txt cat /proc/cpuinfo
command_capture cmdline.txt cat /proc/cmdline
command_capture mounts.txt cat /proc/mounts
command_capture loaded_modules.txt cat /proc/modules
command_capture initial_processes.txt ps w
command_capture raptor_status.txt raptorctl status
command_capture rvd_status.txt raptorctl rvd status
command_capture rwd_status.txt raptorctl rwd status
command_capture rsd_status.txt raptorctl rsd status
command_capture rvd_fps_config.txt raptorctl rvd get-fps 0
command_capture rvd_bitrate_config.txt raptorctl rvd get-bitrate 0
command_capture rvd_rc_mode.txt raptorctl rvd get-rc-mode 0
command_capture rvd_gop_config.txt raptorctl rvd get-gop 0
command_capture ring_header.txt ringdump "$RING_NAME"
command_capture rsd_clients.txt raptorctl rsd clients
command_capture rwd_clients.txt raptorctl rwd clients

{
	printf 'sensor\tfield\tvalue\n'
	for sensor_dir in /proc/jz/sensor/sensor[0-9]*; do
		[ -d "$sensor_dir" ] || continue
		sensor=$(basename "$sensor_dir")
		for field_path in "$sensor_dir"/*; do
			[ -f "$field_path" ] || continue
			field=$(basename "$field_path")
			value=$(cat "$field_path" 2>/dev/null | tr '\t\r\n' '   ')
			printf '%s\t%s\t%s\n' "$sensor" "$field" "$value"
		done
	done
} > "$OUTPUT/sensor_state.tsv"

{
	printf 'module\tloaded_bytes\tuse_count\tdependencies\tstate\taddress\n'
	awk '$1 ~ /^(tx[_-]isp|sensor_|avpu)/ { print $1 "\t" $2 "\t" $3 "\t" $4 "\t" $5 "\t" $6 }' /proc/modules
} > "$OUTPUT/loaded_module_footprint.tsv"

module_name=$(awk '$1 ~ /^tx[_-]isp/ { print $1; exit }' /proc/modules)
meta loaded_isp_module "${module_name:-not-found}"
if [ -n "$module_name" ] && [ -d "/sys/module/$module_name/parameters" ]; then
	{
		for parameter in /sys/module/"$module_name"/parameters/*; do
			[ -f "$parameter" ] || continue
			value=$(cat "$parameter" 2>/dev/null | tr '\t\r\n' '   ')
			printf '%s\t%s\n' "$(basename "$parameter")" "$value"
		done
	} > "$OUTPUT/module_parameters.tsv"
fi

if [ -n "$MODULE_FILE" ]; then
	if [ ! -f "$MODULE_FILE" ]; then
		meta module_file_error "not found: $MODULE_FILE"
	else
		file_row isp_module "$MODULE_FILE" operator-explicit
	fi
else
	meta module_file_warning "not supplied; loaded size is recorded but on-disk identity is intentionally unknown"
fi

rvd_pid=$(find_pid rvd)
if [ -n "$rvd_pid" ]; then
	rvd_exe=$(readlink -f "/proc/$rvd_pid/exe" 2>/dev/null || true)
	[ -n "$rvd_exe" ] && file_row rvd_executable "$rvd_exe" proc-exe
	if [ -r "/proc/$rvd_pid/maps" ]; then
		awk '$NF ~ /\/libimp\.so(\.|$)/ { print $NF }' "/proc/$rvd_pid/maps" |
			sort -u > "$WORK/libimp.paths"
		while IFS= read -r libimp_path; do
			[ -n "$libimp_path" ] && file_row imp_library "$libimp_path" rvd-mapped-object
		done < "$WORK/libimp.paths"
	fi
fi

{
	printf 'name\tsize_bytes\n'
	for shm in /dev/shm/rss_ring_* /dev/shm/rss_osd_*; do
		[ -f "$shm" ] || continue
		printf '%s\t%s\n' "$(basename "$shm")" "$(stat -c %s "$shm" 2>/dev/null || echo 0)"
	done
} > "$OUTPUT/shared_memory_files_start.tsv"

{
	printf 'cpu\tgovernor\tcurrent_khz\tmin_khz\tmax_khz\n'
	for cpu_dir in /sys/devices/system/cpu/cpu[0-9]*; do
		[ -d "$cpu_dir" ] || continue
		cpu=$(basename "$cpu_dir")
		base="$cpu_dir/cpufreq"
		governor=$(cat "$base/scaling_governor" 2>/dev/null || echo unavailable)
		current=$(cat "$base/scaling_cur_freq" 2>/dev/null || echo unavailable)
		minimum=$(cat "$base/scaling_min_freq" 2>/dev/null || echo unavailable)
		maximum=$(cat "$base/scaling_max_freq" 2>/dev/null || echo unavailable)
		printf '%s\t%s\t%s\t%s\t%s\n' "$cpu" "$governor" "$current" "$minimum" "$maximum"
	done
} > "$OUTPUT/cpu_frequency.tsv"

echo "ISP benchmark v$VERSION: warm-up ${WARMUP}s, sample ${DURATION}s every ${INTERVAL}s"
echo "Output: $OUTPUT"
if [ "$WARMUP" -gt 0 ]; then
	sleep "$WARMUP"
fi

dmesg > "$OUTPUT/dmesg_before.txt" 2>&1 || true
logread > "$OUTPUT/logread_before.txt" 2>&1 || true
cp /proc/interrupts "$OUTPUT/interrupts_before.txt" 2>/dev/null || cat /proc/interrupts > "$OUTPUT/interrupts_before.txt"
cp /proc/meminfo "$OUTPUT/meminfo_before.txt" 2>/dev/null || cat /proc/meminfo > "$OUTPUT/meminfo_before.txt"

printf '%s\n' 'sample,elapsed_s,interval_s,cpu_busy_pct,cpu_iowait_pct,mem_available_kib,mem_free_kib,cached_kib,shmem_kib,slab_kib,sreclaimable_kib,sunreclaim_kib,kernel_stack_kib,page_tables_kib,frame_write_seq_low32,frame_delta,delivered_fps,isp_irq_delta,isp_irq_rate,codec_irq_delta,codec_irq_rate' > "$samples"
printf '%s\n' 'sample,elapsed_s,name,pid,state,cpu_capacity_pct,cpu_one_core_pct,rss_kib,vmsize_kib,threads,voluntary_ctxt,nonvoluntary_ctxt,voluntary_ctxt_delta,nonvoluntary_ctxt_delta' > "$process_samples"
printf '%s\n' 'sample,elapsed_s,class,irq,label,count,delta,rate' > "$irq_samples"

discover_processes
discover_irqs
cpu_state > "$WORK/cpu.prev"
start_uptime=$(uptime_s)
previous_uptime=$start_uptime
start_frame=$(frame_sequence)
previous_frame=$start_frame
start_mem_available=$(mem_value MemAvailable)
start_mem_free=$(mem_value MemFree)
sample=0

while :; do
	elapsed=$(awk -v now="$(uptime_s)" -v start="$start_uptime" 'BEGIN { print now-start }')
	awk -v elapsed="$elapsed" -v duration="$DURATION" 'BEGIN { exit !(elapsed < duration) }' || break
	sleep "$INTERVAL"
	sample=$((sample + 1))
	current_uptime=$(uptime_s)
	elapsed=$(awk -v now="$current_uptime" -v start="$start_uptime" 'BEGIN { printf "%.3f", now-start }')
	actual_interval=$(awk -v now="$current_uptime" -v previous="$previous_uptime" 'BEGIN { printf "%.3f", now-previous }')

	set -- $(cat "$WORK/cpu.prev")
	previous_total=$1
	previous_idle=$2
	previous_iowait=$3
	set -- $(cpu_state)
	current_total=$1
	current_idle=$2
	current_iowait=$3
	delta_total=$((current_total - previous_total))
	delta_idle=$((current_idle - previous_idle))
	delta_iowait=$((current_iowait - previous_iowait))
	set -- $(awk -v total="$delta_total" -v idle="$delta_idle" -v iowait="$delta_iowait" 'BEGIN {
		if (total <= 0) { print "NA NA"; exit }
		printf "%.3f %.3f\n", 100*(total-idle-iowait)/total, 100*iowait/total
	}')
	cpu_busy=$1
	cpu_iowait=$2
	printf '%s %s %s\n' "$current_total" "$current_idle" "$current_iowait" > "$WORK/cpu.next"
	mv "$WORK/cpu.next" "$WORK/cpu.prev"

	current_frame=$(frame_sequence)
	delivered_delta=$(frame_delta "$previous_frame" "$current_frame")
	if [ "$delivered_delta" = NA ]; then
		delivered_fps=NA
	else
		delivered_fps=$(awk -v delta="$delivered_delta" -v interval="$actual_interval" 'BEGIN {
			if (interval > 0) printf "%.3f", delta/interval; else print "NA"
		}')
	fi

	: > "$WORK/irq.next"
	while IFS=, read -r class irq label previous_count; do
		current_count=$(irq_count "$irq")
		[ -n "$current_count" ] || current_count=$previous_count
		delta=$((current_count - previous_count))
		rate=$(awk -v delta="$delta" -v interval="$actual_interval" 'BEGIN {
			if (interval > 0) printf "%.3f", delta/interval; else print "NA"
		}')
		printf '%s,%s,%s,%s\n' "$class" "$irq" "$label" "$current_count" >> "$WORK/irq.next"
		printf '%s,%s,%s,%s,%s,%s,%s,%s\n' "$sample" "$elapsed" "$class" "$irq" "$label" "$current_count" "$delta" "$rate" >> "$irq_samples"
	done < "$WORK/irq.prev"
	# Aggregate this interval from the just-written tidy rows.
	isp_irq_delta=$(awk -F, -v s="$sample" '$1 == s && $3 == "isp" { sum += $7 } END { print sum+0 }' "$irq_samples")
	codec_irq_delta=$(awk -F, -v s="$sample" '$1 == s && $3 == "codec" { sum += $7 } END { print sum+0 }' "$irq_samples")
	isp_irq_rate=$(awk -v delta="$isp_irq_delta" -v interval="$actual_interval" 'BEGIN { if (interval>0) printf "%.3f", delta/interval; else print "NA" }')
	codec_irq_rate=$(awk -v delta="$codec_irq_delta" -v interval="$actual_interval" 'BEGIN { if (interval>0) printf "%.3f", delta/interval; else print "NA" }')
	mv "$WORK/irq.next" "$WORK/irq.prev"

	: > "$WORK/process.next"
	while IFS=, read -r name pid previous_ticks previous_rss previous_vmsize previous_threads previous_vol previous_nvol; do
		state=$(proc_state "$pid" 2>/dev/null) || state=
		if [ -z "$state" ]; then
			new_pid=$(find_pid "$name")
			if [ -n "$new_pid" ]; then
				pid=$new_pid
				state=$(proc_state "$pid" 2>/dev/null) || state=
			fi
		fi
		if [ -z "$state" ]; then
			printf '%s,%s,%s,%s,missing,NA,NA,0,0,0,0,0,0,0\n' "$sample" "$elapsed" "$name" "$pid" >> "$process_samples"
			printf '%s,%s,%s,%s,%s,%s,%s,%s\n' "$name" "$pid" "$previous_ticks" "$previous_rss" "$previous_vmsize" "$previous_threads" "$previous_vol" "$previous_nvol" >> "$WORK/process.next"
			continue
		fi
		IFS=, read -r ticks rss vmsize threads vol nvol <<EOF
$state
EOF
		if [ "$ticks" -lt "$previous_ticks" ] || [ "$pid" != "$(awk -F, -v n="$name" '$1==n {print $2; exit}' "$WORK/process.prev")" ]; then
			cpu_capacity=NA
			cpu_one_core=NA
			vol_delta=0
			nvol_delta=0
			proc_status=restarted
		else
			tick_delta=$((ticks - previous_ticks))
			set -- $(awk -v ticks="$tick_delta" -v total="$delta_total" -v cpus="$cpu_count" 'BEGIN {
				if (total <= 0) { print "NA NA"; exit }
				capacity=100*ticks/total
				printf "%.3f %.3f\n", capacity, capacity*cpus
			}')
			cpu_capacity=$1
			cpu_one_core=$2
			vol_delta=$((vol - previous_vol))
			nvol_delta=$((nvol - previous_nvol))
			proc_status=running
		fi
		printf '%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n' "$sample" "$elapsed" "$name" "$pid" "$proc_status" "$cpu_capacity" "$cpu_one_core" "$rss" "$vmsize" "$threads" "$vol" "$nvol" "$vol_delta" "$nvol_delta" >> "$process_samples"
		printf '%s,%s,%s,%s,%s,%s,%s,%s\n' "$name" "$pid" "$ticks" "$rss" "$vmsize" "$threads" "$vol" "$nvol" >> "$WORK/process.next"
	done < "$WORK/process.prev"
	mv "$WORK/process.next" "$WORK/process.prev"

	memory=$(mem_csv)
	# mem_csv is deliberately one argument containing nine comma-separated
	# fields; keep thirteen printf conversions for the thirteen arguments.
	printf '%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n' \
		"$sample" "$elapsed" "$actual_interval" "$cpu_busy" "$cpu_iowait" "$memory" \
		"$current_frame" "$delivered_delta" "$delivered_fps" "$isp_irq_delta" "$isp_irq_rate" \
		"$codec_irq_delta" "$codec_irq_rate" >> "$samples"

	previous_uptime=$current_uptime
	previous_frame=$current_frame
done

end_uptime=$(uptime_s)
actual_duration=$(awk -v end="$end_uptime" -v start="$start_uptime" 'BEGIN { printf "%.3f", end-start }')
scheduled_sleep=$((sample * INTERVAL))
collection_delay=$(awk -v actual="$actual_duration" -v scheduled="$scheduled_sleep" 'BEGIN {
	delay=actual-scheduled
	if (delay < 0) delay=0
	printf "%.3f", delay
}')
collection_delay_pct=$(awk -v delay="$collection_delay" -v actual="$actual_duration" 'BEGIN {
	if (actual > 0) printf "%.3f", 100*delay/actual; else print "0.000"
}')
end_frame=$(frame_sequence)
end_mem_available=$(mem_value MemAvailable)
end_mem_free=$(mem_value MemFree)

dmesg > "$OUTPUT/dmesg_after.txt" 2>&1 || true
logread > "$OUTPUT/logread_after.txt" 2>&1 || true
logcat > "$OUTPUT/logcat_after.txt" 2>&1 || true
cp /proc/interrupts "$OUTPUT/interrupts_after.txt" 2>/dev/null || cat /proc/interrupts > "$OUTPUT/interrupts_after.txt"
cp /proc/meminfo "$OUTPUT/meminfo_after.txt" 2>/dev/null || cat /proc/meminfo > "$OUTPUT/meminfo_after.txt"
command_capture final_processes.txt ps w
command_capture raptor_status_after.txt raptorctl status
command_capture rwd_status_after.txt raptorctl rwd status
command_capture rsd_status_after.txt raptorctl rsd status
command_capture rwd_clients_after.txt raptorctl rwd clients
command_capture rsd_clients_after.txt raptorctl rsd clients

{
	printf 'name\tsize_bytes\n'
	for shm in /dev/shm/rss_ring_* /dev/shm/rss_osd_*; do
		[ -f "$shm" ] || continue
		printf '%s\t%s\n' "$(basename "$shm")" "$(stat -c %s "$shm" 2>/dev/null || echo 0)"
	done
} > "$OUTPUT/shared_memory_files_end.tsv"

printf 'metric\tvalue\tunit\n' > "$summary"
printf 'benchmark_version\t%s\tversion\n' "$VERSION" >> "$summary"
printf 'label\t%s\ttext\n' "$LABEL" >> "$summary"
printf 'transport\t%s\ttext\n' "$TRANSPORT" >> "$summary"
printf 'samples\t%s\tcount\n' "$sample" >> "$summary"
printf 'actual_duration\t%s\tseconds\n' "$actual_duration" >> "$summary"
printf 'scheduled_sample_sleep\t%s\tseconds\n' "$scheduled_sleep" >> "$summary"
printf 'sampling_collection_and_scheduler_delay\t%s\tseconds\n' "$collection_delay" >> "$summary"
printf 'sampling_collection_and_scheduler_delay_pct\t%s\tpercent-of-actual-duration\n' "$collection_delay_pct" >> "$summary"
printf 'logical_cpus\t%s\tcount\n' "$cpu_count" >> "$summary"
printf 'mem_available_start\t%s\tKiB\n' "$start_mem_available" >> "$summary"
printf 'mem_available_end\t%s\tKiB\n' "$end_mem_available" >> "$summary"
printf 'mem_available_delta\t%s\tKiB\n' "$((end_mem_available - start_mem_available))" >> "$summary"
printf 'mem_free_start\t%s\tKiB\n' "$start_mem_free" >> "$summary"
printf 'mem_free_end\t%s\tKiB\n' "$end_mem_free" >> "$summary"

awk -F, '
	function emit(name, value, unit) { printf "%s\t%.3f\t%s\n", name, value, unit }
	# Some Thingino BusyBox awk builds omit the optional math library and do
	# not provide sqrt(). Newton iteration keeps the script portable.
	function root(value, guess, i) {
		if (value <= 0) return 0
		guess = value > 1 ? value : 1
		for (i=0; i<24; i++) guess = (guess + value/guess) / 2
		return guess
	}
	NR == 1 { next }
	{
		n++
		busy += $4; busy2 += $4*$4
		if (n==1 || $4<busy_min) busy_min=$4
		if (n==1 || $4>busy_max) busy_max=$4
		if (n==1 || $6<mem_min) mem_min=$6
		if ($17 != "NA") {
			fps_n++; fps += $17; fps2 += $17*$17
			if (fps_n==1 || $17<fps_min) fps_min=$17
			if (fps_n==1 || $17>fps_max) fps_max=$17
		}
		if ($16 != "NA") frame_total += $16
		isp_total += $18; codec_total += $20
	}
	END {
		if (!n) exit
		emit("cpu_busy_mean", busy/n, "percent-of-total-capacity")
		emit("cpu_busy_min", busy_min, "percent-of-total-capacity")
		emit("cpu_busy_max", busy_max, "percent-of-total-capacity")
		busy_mean=busy/n
		emit("cpu_busy_window_stddev", root(busy2/n-busy_mean*busy_mean), "percentage-points")
		emit("mem_available_min", mem_min, "KiB")
		emit("delivered_frames", frame_total, "frames")
		if (fps_n) {
			emit("delivered_fps_mean", fps/fps_n, "frames-per-second")
			emit("delivered_fps_min", fps_min, "frames-per-second")
			emit("delivered_fps_max", fps_max, "frames-per-second")
			fps_mean=fps/fps_n
			emit("delivered_rate_window_stddev", root(fps2/fps_n-fps_mean*fps_mean), "frames-per-second")
		}
		emit("isp_irq_delta", isp_total, "raw-interrupts")
		emit("codec_irq_delta", codec_total, "raw-interrupts")
	}' "$samples" >> "$summary"

if [ "$start_frame" != NA ] && [ "$end_frame" != NA ]; then
	total_frame_delta=$(frame_delta "$start_frame" "$end_frame")
	overall_fps=$(awk -v frames="$total_frame_delta" -v duration="$actual_duration" 'BEGIN { if (duration>0) printf "%.3f", frames/duration; else print "NA" }')
	printf 'delivered_fps_overall\t%s\tframes-per-second\n' "$overall_fps" >> "$summary"
fi

if [ -n "$module_name" ]; then
	loaded_bytes=$(awk -v module="$module_name" '$1==module {print $2; exit}' /proc/modules)
	printf 'loaded_isp_module\t%s\tname\n' "$module_name" >> "$summary"
	printf 'loaded_isp_module_bytes\t%s\tbytes\n' "${loaded_bytes:-unknown}" >> "$summary"
fi
if [ -n "$MODULE_FILE" ] && [ -f "$MODULE_FILE" ]; then
	module_bytes=$(stat -c %s "$MODULE_FILE" 2>/dev/null || wc -c < "$MODULE_FILE")
	module_sha=$(sha256sum "$MODULE_FILE" 2>/dev/null | awk '{print $1}')
	printf 'isp_module_file_bytes\t%s\tbytes\n' "$module_bytes" >> "$summary"
	printf 'isp_module_file_sha256\t%s\tsha256\n' "${module_sha:-unavailable}" >> "$summary"
fi

printf 'name\tpid\tsamples\tcpu_capacity_mean_pct\tcpu_capacity_max_pct\tcpu_one_core_mean_pct\tcpu_one_core_max_pct\trss_mean_kib\trss_max_kib\tvmsize_max_kib\tthreads_max\tvoluntary_ctxt_delta\tnonvoluntary_ctxt_delta\n' > "$OUTPUT/process_summary.tsv"
awk -F, 'NR==1 || $5=="missing" || $6=="NA" { next }
	{
		key=$3 SUBSEP $4; name[key]=$3; pid[key]=$4; n[key]++
		cap[key]+=$6; core[key]+=$7; rss[key]+=$8
		if ($6>capmax[key]) capmax[key]=$6
		if ($7>coremax[key]) coremax[key]=$7
		if ($8>rssmax[key]) rssmax[key]=$8
		if ($9>vmmax[key]) vmmax[key]=$9
		if ($10>threadmax[key]) threadmax[key]=$10
		vol[key]+=$13; nvol[key]+=$14
	}
	END { for (key in n) printf "%s\t%s\t%d\t%.3f\t%.3f\t%.3f\t%.3f\t%.1f\t%d\t%d\t%d\t%d\t%d\n",
		name[key], pid[key], n[key], cap[key]/n[key], capmax[key], core[key]/n[key],
		coremax[key], rss[key]/n[key], rssmax[key], vmmax[key], threadmax[key], vol[key], nvol[key]
	}' "$process_samples" >> "$OUTPUT/process_summary.tsv"

awk -F, '
	NR==1 || $5=="missing" || $6=="NA" { next }
	{ by_sample[$1]+=$6 }
	END {
		for (s in by_sample) {
			n++; total+=by_sample[s]
			if (n==1 || by_sample[s]<minimum) minimum=by_sample[s]
			if (n==1 || by_sample[s]>maximum) maximum=by_sample[s]
		}
		if (n) {
			printf "pipeline_process_cpu_capacity_mean\t%.3f\tpercent-of-total-capacity\n", total/n
			printf "pipeline_process_cpu_capacity_min\t%.3f\tpercent-of-total-capacity\n", minimum
			printf "pipeline_process_cpu_capacity_max\t%.3f\tpercent-of-total-capacity\n", maximum
		}
	}' "$process_samples" >> "$summary"

printf 'class\tirq\tlabel\tsamples\tdelta\tmean_rate\tmin_rate\tmax_rate\n' > "$OUTPUT/irq_summary.tsv"
awk -F, 'NR==1 { next }
	{
		key=$3 SUBSEP $4 SUBSEP $5
		class[key]=$3; irq[key]=$4; label[key]=$5; n[key]++
		delta[key]+=$7; rate[key]+=$8
		if (n[key]==1 || $8<minimum[key]) minimum[key]=$8
		if (n[key]==1 || $8>maximum[key]) maximum[key]=$8
	}
	END { for (key in n) printf "%s\t%s\t%s\t%d\t%.0f\t%.3f\t%.3f\t%.3f\n",
		class[key], irq[key], label[key], n[key], delta[key], rate[key]/n[key], minimum[key], maximum[key]
	}' "$irq_samples" >> "$OUTPUT/irq_summary.tsv"

awk -F '\t' -v duration="$actual_duration" 'NR==1 { next }
	{ delta[$1]+=$5 }
	END { for (class in delta) printf "%s_irq_rate_overall\t%.3f\traw-interrupts-per-second\n", class, delta[class]/duration }
	' "$OUTPUT/irq_summary.tsv" >> "$summary"

printf 'metric\tbefore\tafter\tdelta\n' > "$OUTPUT/error_deltas.tsv"
for error_spec in \
	'isp_overflow:isp.*overflow|overflow.*isp' \
	'kernel_fatal:oops|kernel panic|call trace|bug:' \
	'userspace_fault:segfault|segmentation fault'; do
	metric=${error_spec%%:*}
	pattern=${error_spec#*:}
	before=$(error_count "$pattern" "$OUTPUT/dmesg_before.txt")
	after=$(error_count "$pattern" "$OUTPUT/dmesg_after.txt")
	delta=$((after - before))
	printf '%s\t%s\t%s\t%s\n' "$metric" "$before" "$after" "$delta" >> "$OUTPUT/error_deltas.tsv"
	printf '%s_delta\t%s\tevents\n' "$metric" "$delta" >> "$summary"
done

meta completed_utc "$(date -u +%Y-%m-%dT%H:%M:%SZ 2>/dev/null || date)"
meta actual_duration_s "$actual_duration"
meta output_directory "$OUTPUT"
rm -rf "$WORK"

echo
echo "Benchmark complete. Summary:"
awk -F '\t' 'NR==1 {next} $1 ~ /^(cpu_busy_mean|pipeline_process_cpu_capacity_mean|delivered_fps_overall|mem_available_delta|sampling_collection_and_scheduler_delay_pct|loaded_isp_module_bytes|isp_module_file_bytes|isp_overflow_delta|kernel_fatal_delta)$/ { printf "  %-44s %s %s\n", $1, $2, $3 }' "$summary"
echo "Results: $OUTPUT"
