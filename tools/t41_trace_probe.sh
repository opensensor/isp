#!/bin/sh

/tmp/t41_alloc_probe_trace &
probe_pid=$!
sleep 1
insmod /tmp/tx_isp_trace.ko interval_ms=60000
trace_status=$?
wait "$probe_pid"
probe_status=$?
if [ "$trace_status" -eq 0 ]; then
	rmmod tx_isp_trace
	cp /opt/isp-trace.txt /tmp/t41-isp-trace.txt
fi
exit "$probe_status"
