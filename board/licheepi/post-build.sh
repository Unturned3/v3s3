#!/bin/bash

IP=$1	# path to outputs/target directory
EP=$BR2_EXTERNAL_V3S3_PATH

source ${BR2_CONFIG} &> /dev/null

if [[ -v BR2_EXTERNAL_ENABLE_INITRAMFS ]]; then
	echo "Copying initramfs.cpio.uboot from ${EP}/board/licheepi/initramfs to ${IP}"
	cp ${EP}/board/licheepi/initramfs/initramfs.cpio.uboot ${IP}/../images
else
	touch ${IP}/../images/initramfs.cpio.uboot
fi

if [[ -e $TARGET_DIR/etc/network/if-pre-up.d/wait_iface ]]; then
	echo "Renaming '$TARGET_DIR/etc/network/if-pre-up.d/wait_iface' script"
	mv $TARGET_DIR/etc/network/if-pre-up.d/wait_iface $TARGET_DIR/etc/network/if-pre-up.d/001-wait_iface
fi

