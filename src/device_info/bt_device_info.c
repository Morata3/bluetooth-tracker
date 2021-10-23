#include "bt_device_info.h"

void free_dev_info(BluetoothDeviceInfo *bt_info){
	free(bt_info->mac_addr);
}

void set_dev_info(BluetoothDeviceInfo *bt_info, const u_char *packet, bool random){
	bt_info->mac_addr = malloc (sizeof(char) * 18);
	int8_t rssi_dbm = packet[RSSI];
	int rssi_int = (int)rssi_dbm;
	
	bt_info->dbm_signal = rssi_int;
	bt_info->random = random;
	snprintf(bt_info->mac_addr, 18, "%02X:%02X:%02X:%02X:%02X:%02X", packet[21],packet[20],packet[19],packet[18],packet[17],packet[16]);
}


