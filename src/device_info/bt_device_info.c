#include "bt_device_info.h"

void set_address_type(u_char byte, BluetoothDeviceInfo *bt_info);

void free_hci_dev_info(BluetoothDeviceInfo *bt_info){
	free(bt_info->mac_addr);
	free(bt_info->dev_class);
	free(bt_info->mac_type);
}

void set_hci_dev_info(BluetoothDeviceInfo *bt_info, const u_char *packet){
	bt_info->mac_addr = malloc (sizeof(char) * 18);
	bt_info->dev_class = malloc (sizeof(char)* 10);
	bt_info->mac_type = malloc (sizeof(char)* 15);
        int8_t rssi_dbm = packet[HCI_RSSI];
        int rssi_int = (int)rssi_dbm;
        
        bt_info->dbm_signal = rssi_int;
        snprintf(bt_info->mac_addr, 18, "%02X:%02X:%02X:%02X:%02X:%02X", packet[13],packet[12],packet[11],packet[10],packet[9],packet[8]);
	snprintf(bt_info->mac_type, 15, "%s", "Public");

	if(packet[MAJOR_DEVICE_CLASS] == MC_COMPUTER) snprintf(bt_info->dev_class, 10, "%s", "Computer");
	else if(packet[MAJOR_DEVICE_CLASS] == MC_PHONE) snprintf(bt_info->dev_class, 10, "%s", "Phone");
	else if(packet[MAJOR_DEVICE_CLASS] == MC_WEREABLE) snprintf(bt_info->dev_class, 10, "%s", "Wereable");
	else snprintf(bt_info->dev_class, 10, "%s", "Other");

}

void set_hci_ble_info(BluetoothDeviceInfo *bt_info, const u_char *packet, int last_byte){
	bt_info->mac_addr = malloc (sizeof(char) * 18);
	bt_info->dev_class = malloc (sizeof(char)* 10);
	bt_info->mac_type = malloc (sizeof(char)* 15);

	int8_t rssi_dbm = packet[last_byte];
	int rssi_int = (int)rssi_dbm;

	bt_info->dbm_signal = rssi_int;
        snprintf(bt_info->mac_addr, 18, "%02X:%02X:%02X:%02X:%02X:%02X", packet[16],packet[15],packet[14],packet[13],packet[12],packet[11]);
	snprintf(bt_info->dev_class, 10, "%s", "BLE");
	if(packet[HCI_MAC_TYPE] != 0x00) set_address_type(packet[16], bt_info);
	else snprintf(bt_info->mac_type, 15, "%s", "Public");
}

void set_address_type(u_char byte, BluetoothDeviceInfo *bt_info){
	unsigned char bits[8];
	unsigned char mask = 1;
	int index;

	for(index=0; index<8; index ++) bits[index] = (byte && (mask << index)) != 0;
	if(bits[7] == 0 && bits[6] == 0) snprintf(bt_info->mac_type, 15, "%s", "Non-resolvable");
	if(bits[7] == 0 && bits[6] == 1) snprintf(bt_info->mac_type, 15, "%s", "Resolvable");
	if(bits[7] == 1 && bits[6] == 0) snprintf(bt_info->mac_type, 15, "%s", "Random");
	if(bits[7] == 1 && bits[6] == 1) snprintf(bt_info->mac_type, 15, "%s", "Static");

}

