#!/bin/sh


if_up -p wm0
ifconfig wm0 up
sysctl -w net.inet.icmp.bmcastecho=1
sysctl -w qnx.kern.droproot=0x1


exit 0
