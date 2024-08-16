#!/bin/sh

if [[ $# != 3 ]]; then
	echo "Usage: $0 interface ssid passphrase"
	exit 1
fi

modprobe r8723bs
ip link set wlan0 up
wpa_supplicant -B -i "$1" -c <(wpa_passphrase "$2" "$3")
sleep 1
udhcpc -i $1
