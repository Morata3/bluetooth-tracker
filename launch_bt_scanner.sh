#!/bin/bash

echo 2 > /proc/sys/vm/overcommit_memory
/usr/src/bluetooth_sniffer/objs/bt_scanner &

