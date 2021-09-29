#include "bt_device_info.h"

void set_dev_info(BluetoothDeviceInfo *bt_info, const u_char *packet){
	bt_info->mac_addr = malloc (sizeof(char) * 18);
	int8_t rssi_dbm = packet[HCI_RSSI];

	bt_info->dbm_signal = rssi_dbm;
	snprintf(bt_info->mac_addr, 18, "%02X:%02X:%02X:%02X:%02X:%02X", packet[13],packet[12],packet[11],packet[10],packet[9],packet[8]);
}

char * get_dev_addr(BluetoothDeviceInfo *bt_info){
	return bt_info->mac_addr;
}

int8_t get_dev_rssi(BluetoothDeviceInfo *bt_info){
	return bt_info->dbm_signal;
}
