#!/bin/sh

echo -e "Mounting /dev/mmcblk0p1 at /newroot"
mount /dev/mmcblk0p1 /newroot

echo -e "Downloading initramfs.cpio.uboot from ${SSH_SRV}..."
scp ${SSH_SRV}:${SSH_DIR}/initramfs.cpio.uboot /

if [ $? == 0 ]; then
	echo -ne "${BLUE}Download complete. Proceed with update (y/N)? ${NC}"
	read ans
	if [ $ans == 'y' ] || [ $ans == 'Y' ]; then
		rm -f /newroot/initramfs.cpio.uboot
		cp /initramfs.cpio.uboot /newroot
		echo -e "${GREEN}Update complete.${NC}"
	else
		echo -e "${RED}Update aborted.${NC}"
	fi
	rm -f /initramfs.cpio.uboot
else
	echo -e "${RED}Download failed!${NC}"
fi

echo -e "Unmounting /dev/mmcblk0p1"
umount /newroot
