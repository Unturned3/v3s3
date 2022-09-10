#!/bin/sh

# script for testing DVP camera (ov5640 specifically)

#res=640x480
#res=1280x720
res=1920x1080
delay=0
skip=4
frames=1

media-ctl --set-v4l2 "\"ov5640 1-003c\":0[fmt:UYVY8_2X8/${res}]"
fswebcam --delay $delay -S $skip -F $frames -d /dev/video0 -p UYVY -r $res out.jpg

# some random ffmpeg commands, not useful

#ffmpeg -hide_banner -loglevel error -f v4l2 -r 30 -t 10 \
#-s 720x480 -i /dev/video0 -pix_fmt yuv420p -c:v h264_omx output.avi -y

#ffmpeg -hide_banner \
#-f v4l2 -framerate 30 -video_size 320x240 -pix_fmt mjpeg -i /dev/video0 \
#-f mjpeg udp://192.168.123.11:8888

#ffmpeg -hide_banner \
#-f v4l2 -framerate 30 -video_size $res -pix_fmt mjpeg -i /dev/video0 \
#-c:v copy -f mjpeg udp://192.168.123.11:8888

