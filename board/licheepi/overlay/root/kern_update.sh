#!/bin/sh

mkdir -p /boot
mount /dev/mmcblk0p1 /boot
scp -P 2222 richard@192.168.123.11:~/outbu/zImage /boot
umount /boot
rmdir /boot

