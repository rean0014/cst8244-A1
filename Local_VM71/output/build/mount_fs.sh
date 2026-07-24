#!/bin/sh

echo "---> Mounting file systems"

if [ -e /data -a -e /system ]; then
	exit 0
fi



mount -t qnx6 -o sync=optional,mntperms=755  /dev/hd0t178 /system

mount -t qnx6 -o sync=optional,mntperms=755  /dev/hd0t179 /data
mount -t qnx6 /dev/hd0t177 /boot

ln -sPf /data/var/tmp /tmp
/system/xbin/cleanup_tmp
