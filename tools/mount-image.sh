#!/bin/bash

if (( $# != 2 )); then
    echo "Usage: mount-image.sh <file> <mountpoint>"
    exit -1
fi

IMGFILE=$1
MOUNTPOINT=$2

if [ ! -f "$IMGFILE" ]; then
    echo "$IMGFILE does not exist"
    exit -1
fi
if [ ! -d "$MOUNTPOINT" ]; then
    echo "$MOUNTPOINT does not exist"
    exit -1
fi

sudo mount -o rw,loop,offset=1048576 "$IMGFILE" "$MOUNTPOINT"