#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <unistd.h>


// BLE BYTES OF INFORMATION DATA
#define BLE_RSSI		1
#define	PACKET_TYPE		14
#define TOKEN_BYTE_INIT		24
#define TOKEN_BYTE_END		31
#define TOKEN_LENGHT		7

// BLE ADVERTISING PDUs TYPES
#define ADV_IND			0x0
#define ADV_DIRECT_IND		0x1
#define ADV_NONCONN_IND		0x2
#define SCAN_REQ		0x3
#define SCAN_RSP		0x4
#define CONNECT_IND		0x5
#define ADV_SCAN_IND		0x6

//HCI MACROS
#define HCI_EVENT_FRAME		5
#define HCI_MAC_TYPE		10
#define HCI_EVENT_RESULT	0x2f
#define HCI_EVENT_BLE		0x3E
#define HCI_RSSI		21
#define MAJOR_DEVICE_CLASS	17

//MAJOR DEVICE CLASS
#define MC_COMPUTER		0x01
#define MC_PHONE		0x02
#define MC_WEREABLE		0x07


typedef struct {
  char *mac_addr;
  char *token;
  int dbm_signal;
  char *mac_type;
  char *dev_class;

} BluetoothDeviceInfo;

void free_ble_dev_info(BluetoothDeviceInfo *bt_info);
void free_hci_dev_info(BluetoothDeviceInfo *bt_info);
void set_ble_dev_info(BluetoothDeviceInfo *bt_info, const u_char *packet, bool random);
void set_hci_dev_info(BluetoothDeviceInfo *bt_info, const u_char *packet);
void set_hci_ble_info(BluetoothDeviceInfo *bt_info, const u_char *packet, int last_byte);

