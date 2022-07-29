#!/bin/sh

source /env.sh

echo -e "Mounting /dev/mmcblk0p1 at /mnt"
mount /dev/mmcblk0p1 /mnt

echo -e "Downloading initramfs.cpio.uboot from ${SSH_SRV}..."
scp -P $SSH_PORT ${SSH_SRV}:${SSH_DIR}/initramfs.cpio.uboot /

if [ $? == 0 ]; then
	echo -ne "${BLUE}Download complete. Proceed with update (y/N)? ${NC}"
	read ans
	if [ $ans == 'y' ] || [ $ans == 'Y' ]; then
		rm -f /mnt/initramfs.cpio.uboot
		cp /initramfs.cpio.uboot /mnt
		echo -e "${GREEN}Update complete.${NC}"
	else
		echo -e "${RED}Update aborted.${NC}"
	fi
	rm -f /initramfs.cpio.uboot
else
	echo -e "${RED}Download failed!${NC}"
fi

echo -e "Unmounting /dev/mmcblk0p1"
umount /mnt
