#!/bin/sh

# script for updating kernel
# for development purposes

# replace with values appropriate for your setup
port=22
host=richard@192.168.1.151

mkdir -p /boot
mount /dev/mmcblk0p1 /boot
scp -P $port ${host}:'~/outbu/*.dtb' /boot
umount /boot
rmdir /boot

