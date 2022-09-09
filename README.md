
# Buildroot Package for Allwinner V3s/S3/S3L

> Support for S3/S3L coming soon (no access to hardware for testing as of now)

This repo contains a _buildroot external tree_ for the Allwinner V3s/S3/S3L SoCs. Using buildroot's external tree mechanism allows all project-specific files to be separated from the buildroot internals and easy upgrades to future buildroot releases.

The main goal is to support all available features/peripherals on these SoCs with the latest mainline software, with minimum patches/modifications. Unfortunately, some binary blobs are required for certain features (specifically, video encoding; see more below).

## General Features

- Fast 0.3 seconds boot time (from `Starting kernel...` to `/sbin/init`)
- Automatic start of `udhcpc` and SSH support via `dropbear`
- `initramfs` support, as a simple way to perform full-disk upgrades over the air. This is disabled by default; for more information, see _insert link here_
- `u-boot` env support: boot env variables can be accessed in userspace via the `fw_getenv` and `fw_setenv` commands

## Tested SoC Hardware Features

- Connectivity: SPI, I2C, UART, LRADC, SDIO, USB, Ethernet
- DVP (parallel) camera interface
- H.264 BP/MP/HP video encoding, up to 720p @ 60fps or 1080p @ 30fps (demo programs coming soon)
- Hardware cryptographic accelerator (accessible in userspace via /dev/crypto; see [cryptodev-linux](http://cryptodev-linux.org/))

In addition, available features shown in the [Sunxi Mainline Status Matrix](https://linux-sunxi.org/Linux_mainlining_effort#Status_Matrix) in the S3/V3s column should be supported as well, but I did not test them personally.

## TODO

- MIPI CSI-2
- ISP

