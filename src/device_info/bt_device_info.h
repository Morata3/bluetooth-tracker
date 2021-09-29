#include <string.h>
#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdint.h> //for uintX_t

#define HCI_PKT_TYPE 	4 /* byte 0 + 4 bytes for direction field */
#define HCI_EVT_CODE 	5
#define HCI_RSSI 	21

// EXTENDED INQUIRY RESPONSE DATA TYPES
#define TYPE_DEV_ID	0x10
#define TYPE_DEV_NAME	0x09
#define TYPE_MANUFACT	0xFF


typedef struct {
  char *mac_addr;
  char *dev_name;
  int8_t dbm_signal;

} BluetoothDeviceInfo;


void set_dev_info(BluetoothDeviceInfo *bt_info, const u_char *packet);
char * get_dev_addr(BluetoothDeviceInfo *bt_info);
int8_t get_dev_rssi(BluetoothDeviceInfo *bt_info);

