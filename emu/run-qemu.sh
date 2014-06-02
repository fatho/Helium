#!/bin/bash
# ensure that all write caches are emptied in case hd0.img is mounted
sync

NUM_CPUS=2

if (($# >= 1 )); then
	mode=$1
	shift
else
	mode="debug"
fi

case "$mode" in
	debug)
		ARGS="-s"
		;;
	release)
		ARGS=""
		;;
	*)
		echo "invalid mode!"
		exit -1
		;;
esac

qemu-system-x86_64 "$@" $ARGS -smp $NUM_CPUS -m 128 hd0.img

