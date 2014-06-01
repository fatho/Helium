#!/bin/bash
# ensure that all write caches are emptied in case hd0.img is mounted
sync
qemu-system-x86_64 -smp 2 -m 128 hd0.img