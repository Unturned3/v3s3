#!/bin/sh

source /env.sh

mkdir -p /mnt/p2
mkdir -p /mnt/p3
echo -e "Mounting /dev/mmcblk0p{2,3}"
mount /dev/mmcblk0p2 /mnt/p2
mount /dev/mmcblk0p3 /mnt/p3

echo -e "Downloading rootfs.tar from ${SSH_SRV}..."
scp -P $SSH_PORT ${SSH_SRV}:${SSH_DIR}/rootfs.tar /mnt/p3

if [ $? == 0 ]; then
	echo -ne "${BLUE}Download complete. Proceed with update (y/N)? ${NC}"
	read ans
	if [ $ans == 'y' ] || [ $ans == 'Y' ]; then
		rm -rf /mnt/p2/*
		tar -C /mnt/p2 -xf /mnt/p3/rootfs.tar
		echo -e "${GREEN}Update complete.${NC}"
	else
		echo -e "${RED}Update aborted.${NC}"
	fi
	rm -f /mnt/p3/rootfs.tar
else
	echo -e "${RED}Download failed!${NC}"
fi

echo -e "Unmounting /dev/mmcblk0p{2,3}"
umount /mnt/p2
umount /mnt/p3

