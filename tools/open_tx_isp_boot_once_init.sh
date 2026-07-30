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

	module_args=
	[ ! -r "$ARGS" ] || module_args="$(cat "$ARGS")"
	insmod "$MODULE" $module_args >"$LOG" 2>&1
	echo $? >"$STATUS"
	sync
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
