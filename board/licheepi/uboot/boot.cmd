
setenv bootargs "${bootargs} console=ttyS0,115200 panic=5 rootwait root=/dev/mmcblk0p2 earlyprintk rw"

if test "$quiet" = "1"; then
	setenv bootargs "${bootargs} quiet"
fi

load mmc 0:1 0x41000000 zImage
load mmc 0:1 0x41800000 sun8i-v3s-licheepi-zero-dock.dtb

if test "$initramfs" = "1"; then
	setenv bootargs "${bootargs} rdinit=/init initrd=0x42000000"
	load mmc 0:1 0x42000000 initramfs.cpio.uboot
	bootz 0x41000000 0x42000000 0x41800000
else
	bootz 0x41000000 - 0x41800000
fi

