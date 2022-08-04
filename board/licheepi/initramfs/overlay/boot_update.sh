#!/bin/sh

source /env.sh

BOOT_FILES="\
boot.scr,\
sun8i-v3s-licheepi-zero.dtb,\
sun8i-v3s-licheepi-zero-dock.dtb,\
zImage,\
initramfs.cpio.uboot\
"

SPL="u-boot-sunxi-with-spl.bin"

mkdir /boot_files
echo -e "Downloading boot files from ${SSH_SRV}..."
scp -P $SSH_PORT ${SSH_SRV}:${SSH_DIR}/{${BOOT_FILES},${SPL}} /boot_files

if [ $? == 0 ]; then
	echo -ne "${BLUE}Download complete. Proceed with update (y/N)? ${NC}"
	read ans
	if [ $ans == 'y' ] || [ $ans == 'Y' ]; then
		dd if=/boot_files/$SPL of=/dev/mmcblk0 bs=1024 seek=8
		sync
		rm /boot_files/$SPL
		echo -e "Mounting /dev/mmcblk0p1 at /mnt"
		mount /dev/mmcblk0p1 /mnt
		cp -f /boot_files/* /mnt
		echo -e "${GREEN}Update complete.${NC} Unmounting /dev/mmcblk0p1..."
		umount /mnt
	else
		echo -e "${RED}Update aborted.${NC}"
	fi
else
	echo -e "${RED}Download failed!${NC}"
fi

rm -rf /boot_files

