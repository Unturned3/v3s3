#!/bin/sh

# This works! But is still a little slow/laggy.
exec /usr/bin/gst-launch-1.0 -v \
  v4l2src device=/dev/video0 io-mode=mmap ! \
  video/x-raw,format=YUY2,width=640,height=512,framerate=30/1 ! \
  videoscale ! video/x-raw,width=800,height=480 ! \
  videoconvert ! video/x-raw,format=BGRx ! \
  kmssink connector-id=49 plane-id=43 force-modesetting=true sync=false
