wdt dev watchdog@1c20ca0
wdt start 10000

if test "${kern_quiet}" = "1"; then
	setenv bootargs "${bootargs} quiet"
fi

if test "${earlyprintk}" = "1"; then
	setenv bootargs "${bootargs} earlyprintk"
fi

load mmc 0:1 0x41000000 zImage
load mmc 0:1 0x41800000 sun8i-v3s-tau.dtb

wdt reset
bootz 0x41000000 - 0x41800000

