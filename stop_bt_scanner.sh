#!/bin/bash

BT_CHILD_PID=$(pidof -s /usr/src/bluetooth_sniffer/objs/bt_scanner)
BT_PID=$(ps -o ppid= $BT_CHILD_PID)

kill -10 $BT_PID; wait $BT_PID;
