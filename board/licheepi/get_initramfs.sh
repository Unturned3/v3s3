#!/bin/sh

IP=$1	# path to outputs/images directory
EP=$BR2_EXTERNAL_V3S3_PATH

echo "Copying initramfs.cpio.uboot from ${EP}/board/licheepi/initramfs to ${IP}"
cp ${EP}/board/licheepi/initramfs/initramfs.cpio.uboot ${IP}/../images

