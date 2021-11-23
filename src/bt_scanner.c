#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pcap.h>
#include <pthread.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <sys/prctl.h>
#include <signal.h>
#include "device_info/bt_device_info.h"
#include "info_list/bt_info_list.h"
#include "Mqtt/pcap_publisher.h"

#define TIME_TO_PUBLISH 60

pcap_t *handle;
pid_t scanner_pid;
FILE *capture;
char *devmac;

void packet_processor(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);
void disconnect(int s);
void send_data(int s);
void send_and_stop(int s);
void getMAC();
void scanner();

int main(int argc, char *argv[])
{
	char errbuf [PCAP_ERRBUF_SIZE];
	struct sigaction sigint;
	struct sigaction sigsend;
	struct sigaction sigsendstop;

	char *device = "bluetooth0";

	sigint.sa_handler = disconnect;
	sigemptyset(&sigint.sa_mask);
	sigint.sa_flags = 0;

	sigsend.sa_handler = send_data;
	sigemptyset(&sigsend.sa_mask);
	sigsend.sa_flags = 0;
	
	sigsendstop.sa_handler = send_and_stop;
	sigemptyset(&sigsendstop.sa_mask);
	sigsendstop.sa_flags = 0;

	sigaction(SIGINT, &sigint, NULL);
	sigaction(SIGALRM, &sigsend, NULL);
	sigaction(SIGUSR1, &sigsendstop, NULL);

	
	// Connect to MQTT
	pcap_connect();
	set_publish_function(pcap_publish_message);
	getMAC();
	init_list(100);
	
	/*
	// Connecto to MQTT Logger
	set_log_topic(devmac);
	log_connect();
    	*/

	scanner_pid = fork();
	if(scanner_pid == 0){
		while(1){
			scanner();
		}
	}

	// Open device for live capture
	handle = pcap_open_live(device, BUFSIZ, 0, 5000, errbuf);
	if (handle == NULL) {
		fprintf(stderr, "Could not open device %s: %s\n", device, errbuf);
	 	return 2;
	}

	// Sniff loop
	alarm(TIME_TO_PUBLISH);
     	pcap_loop(handle, 0, packet_processor, NULL);

	return(0);
}

void packet_processor(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
	int packet_lenght = header->caplen;

	if(packet_lenght > 5 && (packet[HCI_EVENT_FRAME] == HCI_EVENT_RESULT || packet[HCI_EVENT_FRAME] == HCI_EVENT_BLE) ){
		BluetoothDeviceInfo bt_dev_info;
		int last_byte = packet_lenght - 1;
		if(packet[HCI_EVENT_FRAME] == HCI_EVENT_RESULT) set_hci_dev_info(&bt_dev_info, packet);
		else if(packet[HCI_EVENT_FRAME] == HCI_EVENT_BLE) set_hci_ble_info(&bt_dev_info, packet, last_byte);
		else return;

		if(check_device_in_list(bt_dev_info.mac_addr) == 0){
			set_list_pointer();
			insert_hci_in_list(bt_dev_info.mac_addr, devmac, bt_dev_info.dbm_signal,bt_dev_info.mac_type, bt_dev_info.dev_class);
			printf("MAC: %s -> TYPE: %s\n", bt_dev_info.mac_addr, bt_dev_info.mac_type);
		}
		free_hci_dev_info(&bt_dev_info);
	}

	return;
}

void scanner(){

	inquiry_info *ii = NULL;
	int max_rsp, sock, num_rsp, btle;
     	int dev_id, flags;
	int len = 8;
    	max_rsp = 255;
   	flags = IREQ_CACHE_FLUSH;
	uint8_t own_type = LE_PUBLIC_ADDRESS;
	uint8_t scan_type = 0x01;
	uint8_t filter_policy = 0x00;
	uint16_t interval = htobs(0x0010);
	uint16_t window = htobs(0x0010);
	uint8_t filter_dup = 0x01;

	// Device detection
    	dev_id = hci_get_route(NULL);
    	if(dev_id < 0){
		fprintf(stderr, "Not connected.\n");
		exit(1);
    	}

    	sock = hci_open_dev(dev_id);
    	if (sock < 0) {
		perror("HCI device open failed");
		exit(1);
    	}

	system("hciconfig hci0 down");
	system("hciconfig hci0 up");
	btle = hci_le_set_scan_parameters(sock, scan_type, interval, window,
						own_type, filter_policy, 10000);
	if (btle < 0) {
		perror("Set scan parameters failed");
		exit(1);
	}
										 
	btle = hci_le_set_scan_enable(sock, 0x01, filter_dup, 10000);
	if (btle < 0) {
		perror("Enable scan failed");
		exit(1);
	}
    	

	ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));

    	//Requesting info devices
    	num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
    	if( num_rsp < 0 ) perror("hci_inquiry");
		
	btle = hci_le_set_scan_enable(sock, 0x00, filter_dup, 10000);
	if (btle < 0) {
		perror("Disable scan failed");
		exit(1);        
	}

	free(ii);
	hci_close_dev(sock);
}

void disconnect (int s){

	printf("\nClosing...\n");
	pcap_disconnect();
	pcap_breakloop(handle);
	pcap_close(handle);
	free(devmac);
	free_info_list();
	kill(scanner_pid,SIGTERM);

}

void send_data(int s){
	printf("\n\n Enviando lista \n\n");
	publish_list_if_needed();
	alarm(TIME_TO_PUBLISH); //Restart alarm
}

void send_and_stop(int s){
	printf("\n System rebooting...\n");
	send_data(0);
	disconnect(0);
}

void getMAC(){
	FILE *fp;
	char hci_info[50];
	devmac = malloc(sizeof(char) * 18);
	char *ret;
   
	memset(devmac, '\0', 18);
	fp = popen("/bin/hciconfig", "r");
       	if (fp == NULL) {
	      	printf("Failed to run command\n" );
	      	exit(1);
       	}

       	while (fgets(hci_info, sizeof(hci_info), fp) != NULL) {
		ret = strstr(hci_info, "Address");
		if(ret) strncpy(devmac, hci_info + 13, 17);
       	}
	
       	pclose(fp);
       	return;
	    
}

