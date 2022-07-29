#!/bin/sh

IP=$1	# path to outputs/images directory
#EP=$2	# path to external tree
EP=$BR2_EXTERNAL_V3S3_PATH

echo "Copying initramfs images to ${EP}/board/licheepi/initramfs"
cp $IP/rootfs.cpio.gz $EP/board/licheepi/initramfs/initramfs.cpio.gz
cp $IP/rootfs.cpio.uboot $EP/board/licheepi/initramfs/initramfs.cpio.uboot

