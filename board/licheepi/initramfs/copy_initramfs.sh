#!/bin/sh

echo "ERROR: copy_initramfs.sh not ready. Please fix relative paths!"

exit 1

echo "Copying initramfs images to board/licheepi/initramfs..."
cp output/images/rootfs.cpio.gz board/licheepi/initramfs/initramfs.cpio.gz
cp output/images/rootfs.cpio.uboot board/licheepi/initramfs/initramfs.cpio.uboot
