#!/bin/bash

if (( $# != 3 )); then
    echo "Usage: make-image.sh <sizeKB> <file> <mountpoint>"
    exit -1
fi

SIZE=$1
IMGFILE=$2
MOUNTPOINT=$3

if [ -f "$IMGFILE" ]; then
    echo "$IMGFILE already exists. To prevent accidental data loss, please remove it manually"
    exit -1
fi

if [ ! -d "$MOUNTPOINT" ]; then
    echo "$MOUNTPOINT does not exist"
    exit -1
fi

if (( $SIZE <= 10240 )); then
    echo "at least 10 MiB are required"
    exit -1
fi

echo "creating empty image file"

dd if=/dev/zero of="$IMGFILE" bs=1024 count=$SIZE

echo "creating partition..."

fdisk "$IMGFILE" <<EOF
n
p
1
2048

w
EOF
sudo sync

echo "initializing loop devices"

sudo losetup /dev/loop0 "$IMGFILE"
sudo losetup /dev/loop1 "$IMGFILE" -o 1048576


echo "initializing ext2 file system"

sudo mke2fs /dev/loop1

echo "mounting"

sudo mount /dev/loop1 "$MOUNTPOINT"

echo "installing grub"
sudo grub-install --root-directory="$MOUNTPOINT" --no-floppy --modules="normal part_msdos ext2 multiboot" /dev/loop0

echo "unmounting"
sudo umount "$MOUNTPOINT"

sudo losetup -d /dev/loop0
sudo losetup -d /dev/loop1

echo "success!"
