setenv bootargs console=ttyS0,115200 panic=5 rootwait rdinit=/init initrd=0x42000000 root=/dev/mmcblk0p2 earlyprintk rw quiet
load mmc 0:1 0x41000000 zImage
load mmc 0:1 0x41800000 sun8i-v3s-licheepi-zero-dock.dtb
load mmc 0:1 0x42000000 initramfs.cpio.uboot
bootz 0x41000000 0x42000000 0x41800000
