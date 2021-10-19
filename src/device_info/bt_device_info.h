#include <string.h>
//#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
//#include <netinet/in.h>
#include <unistd.h>
#include <stdint.h> //for uintX_t

#define BTLE_RSSI 		1
#define BTLE_PACKET_HEADER	14

// EXTENDED INQUIRY RESPONSE DATA TYPES
#define TYPE_DEV_ID	0x10
#define TYPE_DEV_NAME	0x09
#define TYPE_MANUFACT	0xFF


typedef struct {
  char *mac_addr;
  int dbm_signal;
  bool random;

} BluetoothDeviceInfo;

void free_dev_info(BluetoothDeviceInfo *bt_info);
void set_dev_info(BluetoothDeviceInfo *bt_info, const u_char *packet,bool random);
char * get_dev_addr(BluetoothDeviceInfo *bt_info);
int8_t get_dev_rssi(BluetoothDeviceInfo *bt_info);

