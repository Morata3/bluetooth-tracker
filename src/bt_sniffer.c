#include <stdio.h>
#include <pcap.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include "device_info/bt_device_info.h"

pcap_t *handle;

void hci_packet_processor(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);
void print_packet_info(BluetoothDeviceInfo *bt_dev_info);

int main(int argc, char *argv[])
{
	char errbuf [100];
	char *dev = "bluetooth0";
	int packet_limit = 1;
	int timeout = 10000;
	const u_char *packet;
	struct pcap_pkthdr packet_header;

	printf("Opening device %s for sniffing...\n", dev);
	handle = pcap_open_live(dev, BUFSIZ, packet_limit, timeout, errbuf);
	if (handle == NULL) {
		fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
		return(2);
	}
	printf("Done\n");
	printf("Link-Layer Header Type %i\n\n", pcap_datalink(handle));

	pcap_loop(handle, 0, hci_packet_processor, NULL);
	
	return(0);
}

void hci_packet_processor(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
	const u_char hci_type_event = 0x04;
	const u_char event_inquiry_result = 0x2f;
	int buffer_lenght = header-> caplen;
		
	BluetoothDeviceInfo bt_dev_info;

	if(packet[HCI_PKT_TYPE] == hci_type_event && packet[HCI_EVT_CODE] == event_inquiry_result){
		set_dev_info(&bt_dev_info, packet);
		print_packet_info(&bt_dev_info);
	}	

	return;
}

void print_packet_info(BluetoothDeviceInfo *bt_dev_info)
{
	printf("MAC: %s\n", get_dev_addr(bt_dev_info));
	printf("RSSI: %d dBm\n",get_dev_rssi(bt_dev_info));
}

