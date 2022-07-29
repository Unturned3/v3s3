#!/bin/sh

source /env.sh

echo -e "Mounting /dev/mmcblk0p2 at /mnt"
mount /dev/mmcblk0p2 /mnt

echo -e "Downloading rootfs.tar from ${SSH_SRV}..."
scp -P $SSH_PORT ${SSH_SRV}:${SSH_DIR}/rootfs.tar /mnt

if [ $? == 0 ]; then
	echo -ne "${BLUE}Download complete. Proceed with update (y/N)? ${NC}"
	read ans
	if [ $ans == 'y' ] || [ $ans == 'Y' ]; then
		tar -C /mnt -xf /mnt/rootfs.tar
		echo -e "${GREEN}Update complete.${NC}"
	else
		echo -e "${RED}Update aborted.${NC}"
	fi
	rm -f /mnt/rootfs.tar
else
	echo -e "${RED}Download failed!${NC}"
fi

echo -e "Unmounting /dev/mmcblk0p2"
umount /mnt
