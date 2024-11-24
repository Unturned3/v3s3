#!/bin/sh

. /etc/ansi-colors.sh

echo -ne "\n"
date
echo -ne "\n"
echo -ne $BLUE
uname -a
echo -ne "\n"
echo -ne $GREEN
free -h
echo -ne "\n"
echo -ne $NC
