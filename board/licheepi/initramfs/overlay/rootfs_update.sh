#!/bin/sh

echo -e "Mounting /dev/mmcblk0p2 at /newroot"
mount /dev/mmcblk0p2 /newroot

echo -e "Downloading rootfs.tar from ${SSH_SRV}..."
scp ${SSH_SRV}:${SSH_DIR}/rootfs.tar /newroot

if [ $? == 0 ]; then
	echo -ne "${BLUE}Download complete. Proceed with update (y/N)? ${NC}"
	read ans
	if [ $ans == 'y' ] || [ $ans == 'Y' ]; then
		tar -C /newroot -xf /newroot/rootfs.tar
		echo -e "${GREEN}Update complete.${NC}"
	else
		echo -e "${RED}Update aborted.${NC}"
	fi
	rm -f /newroot/rootfs.tar
else
	echo -e "${RED}Download failed!${NC}"
fi

echo -e "Unmounting /dev/mmcblk0p2"
umount /newroot
