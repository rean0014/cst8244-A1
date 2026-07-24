#!/bin/sh


. /proc/boot/build/options

# By default set umask so that services don't accidentally create group/world writable files.
umask 022


echo "---> Starting slogger2"
 slogger2 -U 21:21 -P /pps/slogger2
waitfor /dev/slog

echo "---> Starting PCI Services"
 pci-server --config=/proc/boot/pci_server.cfg
waitfor /dev/pci



echo "---> Starting devb"
  devb-eide cam user=20:20 blk cache=64M,auto=partition,vnode=2000,ncache=2000,commit=low 
waitfor /dev/hd0

echo "---> Mounting file systems"
mount_fs.sh

 devc-con -e 

 syslogd -f /system/etc/syslog.conf
 random -t -p -U 22:22 -s /data/var/random/rnd-seed  
waitfor /dev/random
 pipe -U 24:24
waitfor /dev/pipe
 devc-pty -U 37:37
 dumper -U 30:30 -d /data/var/dumper

on  -d -t /dev/con1 ksh -l
# Start more consoles which can be switched to using ctrl-alt-[1-4]
on  -d -t /dev/con2 ksh -l
on  -d -t /dev/con3 ksh -l
on  -d -t /dev/con4 ksh -l

echo "---> Starting Networking"
setconf _CS_HOSTNAME $OPT_HOSTNAME
 io-pkt-v6-hc -U 33:33 -d e1000  
waitfor /dev/socket
start_net.sh
 dhclient -nw --no-pid -cf /system/etc/dhclient.conf -sf /system/xbin/_dhclient-script -lf /data/var/dhclient/leases -m wm0

echo "---> Starting sshd"
if [ ! -f /data/var/ssh/ssh_host_rsa_key ]; then
    ssh-keygen -q -t rsa -N '' -f /data/var/ssh/ssh_host_rsa_key
fi
if [ ! -f /data/var/ssh/ssh_host_ed25519_key ]; then
    ssh-keygen -q -t ed25519 -N '' -f /data/var/ssh/ssh_host_ed25519_key
fi
 /system/xbin/sshd -f /system/etc/ssh/sshd_config

echo "---> Starting misc"
 qconn
 mq -U 27:27
 mqueue
 pps -U 32:32 -p /data/var/pps
 io-audio -d audiopci cap_name=defaultc,play_name=defaultp -U 35:35


# Execute the post startup script.  This is a separate script to allow for common behavior of
# both slm and script based startup.
 /proc/boot/post_startup.sh
