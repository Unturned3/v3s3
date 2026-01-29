#!/bin/sh

EVX=$(sed -n '/lradc/,/^$/p' /proc/bus/input/devices | grep -o 'event[0-9]\+' | head -n1)
EVDEV=/dev/input/$EVX
GST=/root/gst.sh
GST_PID=/tmp/gst.pid

BRIGHT=1
BL=/sys/class/backlight/backlight/brightness

start_gst() {
	sh "$GST" >/dev/null 2>&1 & echo $! >"$GST_PID";
	echo "Starting GST"
}
stop_gst()  {
	[ -f "$GST_PID" ] && kill "$(cat "$GST_PID")" 2>/dev/null; rm -f "$GST_PID";
	echo "Stopping GST"
}
running()   {
	[ -f "$GST_PID" ] && kill -0 "$(cat "$GST_PID")" 2>/dev/null;
}

evtest "$EVDEV" | while read -r line; do
  case "$line" in
    *KEY_OK*value\ 0*)
      BRIGHT=$((BRIGHT + 1))
      [ "$BRIGHT" -gt 6 ] && BRIGHT=1
      echo "$BRIGHT" > "$BL"
      ;;
    *KEY_SELECT*value\ 0*)
      if running; then stop_gst; else start_gst; fi
      ;;
    *KEY_VOLUMEUP*value\ 0*)
      stop_gst
      poweroff
      ;;
  esac
done
