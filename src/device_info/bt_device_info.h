#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <unistd.h>


// BLE BYTES OF INFORMATION DATA
#define RSSI 			1
#define	PACKET_HEADER		14

// BLE ADVERTISING PDUs TYPES
#define ADV_IND			0x0
#define ADV_DIRECT_IND		0x1
#define ADV_NONCONN_IND		0x2
#define SCAN_REQ		0x3
#define SCAN_RSP		0x4
#define CONNECT_IND		0x5
#define ADV_SCAN_IND		0x6



typedef struct {
  char *mac_addr;
  int dbm_signal;
  bool random;

} BluetoothDeviceInfo;

void free_dev_info(BluetoothDeviceInfo *bt_info);
void set_dev_info(BluetoothDeviceInfo *bt_info, const u_char *packet,bool random);

