#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>
#include <pthread.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <sys/prctl.h>
#include <signal.h>
#include "device_info/bt_device_info.h"

pcap_t *handle;
pthread_t ubertooth_id;
pid_t ubertooth_pid;

int packet_processor();
void print_packet_info(BluetoothDeviceInfo *bt_dev_info);
void ubertooth_btle();

int main(int argc, char *argv[])
{
	char errbuf [PCAP_ERRBUF_SIZE];
	int pcap_return = 0; 
	FILE *capture;

	capture = fopen("/tmp/pipe", "w+");
	if(capture == NULL){
		printf("Couldn't open file\n");
		exit(1);
	}

	ubertooth_pid = fork();
	if(ubertooth_pid == 0){
		setpgid(getpid(),getpid()); //Move the process to another group process
		ubertooth_btle();
	}
	sleep(1);
	handle = pcap_fopen_offline(capture, errbuf);
	if(handle == NULL){
	       	printf("Error opening file: %s\n",errbuf);
		exit(1);
	}

	//pcap_return = pcap_loop(handle, -1, packet_processor, NULL);
	
	pcap_return = packet_processor();
	printf("\nClosing...\n");
	kill(-ubertooth_pid,SIGKILL); //Use negative pid to kill all process of the group
	
	fclose(capture);

	return(0);
}

int packet_processor()
{
	const u_char * packet_data;
	struct pcap_pkthdr * packet_header;
	int pcap_next_res = 0;
	BluetoothDeviceInfo bt_dev_info;

	pcap_next_res =  pcap_next_ex(handle, &packet_header, &packet_data);
	while(pcap_next_res >= 0 || pcap_next_res == PCAP_ERROR_BREAK){
		printf("MAC %02X:%02X:%02X:%02X:%02X:%02X\n",packet_data[21],packet_data[20],packet_data[19],packet_data[18],packet_data[17],packet_data[16]);
		pcap_next_res =  pcap_next_ex(handle, &packet_header, &packet_data);
	}

	printf("Error processing packet: %s",pcap_statustostr(pcap_next_res));
		

	/*
	set_dev_info(&bt_dev_info, packet);
	print_packet_info(&bt_dev_info);
	*/

	return pcap_next_res;
}

void print_packet_info(BluetoothDeviceInfo *bt_dev_info)
{
	printf("MAC: %s\n", get_dev_addr(bt_dev_info));
	printf("RSSI: %d dBm\n",get_dev_rssi(bt_dev_info));
}

void ubertooth_btle(){
	int index;
	char ubertooth_command[100];

	while(1){
		index = 37;
		for(index = 37; index <= 39; index ++){
			sprintf(ubertooth_command, "ubertooth-btle -f -A %i -q /tmp/pipe > /dev/null 2>&1",index);
			system(ubertooth_command);
		}	
	}
}
