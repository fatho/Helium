#!/bin/bash
# ensure that all write caches are emptied in case hd0.img is mounted
sync
bochs -q