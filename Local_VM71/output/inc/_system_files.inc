@lib/libssl.so.2.1=usr/lib/libssl.so.2.1
@lib/libcurl.so.10=usr/lib/libcurl.so.10
@lib/libcares.so.4=usr/lib/libcares.so.4
@/xbin/setkey=usr/sbin/setkey
@[perms=555] /xbin/rununittests=__TOOL_DIR__/inputs/rununittests
@[perms=555] xbin/cleanup_tmp=__TOOL_DIR__/inputs/cleanup_tmp
@# local/snippets/system_files.custom
@# Placeholder for local list of files to add to system partition
@
@xbin/secpol=bin/secpol
@[perms=555] xbin/pinger={
@#!/bin/sh
@# Script to ping all interfaces of our host as a means of getting our own address into the arp cache
@addr_list=192.168.147.1:192.168.188.1:10.70.144.114
@# Wait for us to get an address
@while ! ifconfig | grep 'inet .*broadcast' | grep -v 0.0.0.0 >/dev/null; do
@    sleep 2
@done
@IFS=:
@for ip in $addr_list; do
@ 	ping -c 1 $ip >/dev/null 2>&1 &
@done
@}
@lib/dll/devnp-e1000.so=lib/dll/devnp-e1000.so
