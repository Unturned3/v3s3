#!/bin/sh

echo -e "Mounting /dev/mmcblk0p1 at /newroot"
mount /dev/mmcblk0p1 /newroot

echo -e "Downloading boot.tar from ${SSH_SRV}..."
scp ${SSH_SRV}:${SSH_DIR}/boot.tar /

if [ $? == 0 ]; then
	echo -ne "${BLUE}Download complete. Proceed with update (y/N)? ${NC}"
	read ans
	if [ $ans == 'y' ] || [ $ans == 'Y' ]; then
		tar -C /newroot -xf /boot.tar
		echo -e "${GREEN}Update complete.${NC}"
	else
		echo -e "${RED}Update aborted.${NC}"
	fi
	rm -f /boot.tar
else
	echo -e "${RED}Download failed!${NC}"
fi

echo -e "Unmounting /dev/mmcblk0p1"
umount /newroot
