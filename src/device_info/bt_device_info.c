#include "bt_device_info.h"

void free_dev_info(BluetoothDeviceInfo *bt_info){
	free(bt_info->mac_addr);
}

void set_dev_info(BluetoothDeviceInfo *bt_info, int header_lenght, const u_char *packet, bool random){
	bt_info->mac_addr = malloc (sizeof(char) * 18);
	int8_t rssi_dbm = packet[RSSI];
	int rssi_int = (int)rssi_dbm;
	int token_lenght = (header_lenght - PACKET_HEADER_LENGHT - CRC_LENGHT) * 2;
	bt_info->token = malloc(sizeof(char) * header_lenght * 2);
	int index;
	int printed_byte = 0;
	
	bt_info->dbm_signal = rssi_int;
	bt_info->random = random;
	snprintf(bt_info->mac_addr, 18, "%02X:%02X:%02X:%02X:%02X:%02X", packet[21],packet[20],packet[19],packet[18],packet[17],packet[16]);
		
	for(index = PACKET_HEADER_LENGHT; index < (header_lenght - CRC_LENGHT); index ++){
		printed_byte += snprintf((bt_info->token + printed_byte), (token_lenght - printed_byte) +1, "%02X", packet[index]);
	}

}


