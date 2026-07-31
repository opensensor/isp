#!/bin/sh
#
# One-shot, fail-safe boot loader for staged TX-ISP smoke modules.
#
# Install this as /etc/init.d/S10open-tx-isp-smoke and place these files in
# /opt/open-tx-isp-smoke:
#   armed       - marker enabling exactly one staged insertion
#   module.ko   - module to insert before the normal S11 module loader
#   args        - optional insmod arguments
#
# The marker is removed and synced before insmod. If the staged module crashes
# the kernel, a watchdog/power-cycle boots the persistent module on the next
# attempt. If insmod returns an error, S11 can load the persistent module in
# the same boot.

STATE_DIR=/opt/open-tx-isp-smoke
MARKER=$STATE_DIR/armed
MODULE=$STATE_DIR/module.ko
ARGS=$STATE_DIR/args
STATUS=$STATE_DIR/last-status
LOG=$STATE_DIR/last-insmod.log
WATCH_MARKER=$STATE_DIR/watch-registry
WATCH_LOG=$STATE_DIR/registry-watch.log
# These must survive a watchdog reboot if the staged module faults.
BOOT_DMESG_PREFIX=$STATE_DIR/boot-dmesg
BOOT_KMSG_LOG=$STATE_DIR/boot-kmsg.log

watch_registry()
{
	iteration=0
	while [ "$iteration" -lt 45 ]; do
		# Preserve the earliest driver/consumer messages before a verbose
		# recovery build wraps the small kernel log ring.  Keep the snapshots
		# in tmpfs and bound them to the startup window.
		if [ "$iteration" -lt 12 ]; then
			dmesg >"$BOOT_DMESG_PREFIX-$iteration.log"
		fi
		echo "sample=$iteration uptime=$(cut -d' ' -f1 /proc/uptime)"
		if [ -r /proc/jz/sensor/count ]; then
			echo -n "count="
			cat /proc/jz/sensor/count
			[ ! -r /proc/jz/sensor/events ] ||
				cat /proc/jz/sensor/events
			echo -n "entries="
			ls -1 /proc/jz/sensor 2>/dev/null
		else
			echo "count=missing"
		fi
		dmesg | grep 'tx-isp-sinfo:' | tail -n 16
		iteration=$((iteration + 1))
		sleep 1
	done
}

start()
{
	[ -e "$MARKER" ] || return 0

	rm -f "$MARKER"
	sync

	if [ ! -r "$MODULE" ]; then
		echo "module missing: $MODULE" >"$LOG"
		echo 2 >"$STATUS"
		sync
		return 0
	fi

	rm -f "$BOOT_KMSG_LOG" "$BOOT_DMESG_PREFIX"-*.log
	module_args=
	[ ! -r "$ARGS" ] || module_args="$(cat "$ARGS")"
	insmod "$MODULE" $module_args >"$LOG" 2>&1
	echo $? >"$STATUS"
	sync

	if [ "$(cat "$STATUS")" = 0 ] && [ -e "$WATCH_MARKER" ]; then
		# The T41 recovery build can wrap the kernel ring in less than one
		# second during ISP startup.  Stream a bounded copy from /dev/kmsg so
		# the messages between snapshots are not lost.
		timeout 20 cat /dev/kmsg >"$BOOT_KMSG_LOG" 2>&1 &
		watch_registry >"$WATCH_LOG" 2>&1 &
	fi
}

case "$1" in
	start)
		start
		;;
	stop)
		;;
	restart)
		start
		;;
	*)
		echo "Usage: $0 {start|stop|restart}"
		exit 1
		;;
esac

exit 0
