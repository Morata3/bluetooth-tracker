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

pcap_t *handle;
pid_t ubertooth_pid;
FILE *capture;
char *devmac;
bool breakloop;

void packet_processor();
void print_packet_info(BluetoothDeviceInfo *bt_dev_info);
void ubertooth_btle();
void disconnect(int s);
void send_data(int s);
void getMAC();
bool is_random(const u_char random);

int main(int argc, char *argv[])
{
	char errbuf [PCAP_ERRBUF_SIZE];
	struct sigaction sigint;
	struct sigaction sigsend;

	sigint.sa_handler = disconnect;
	sigemptyset(&sigint.sa_mask);
	sigint.sa_flags = 0;

	sigsend.sa_handler = send_data;
	sigemptyset(&sigsend.sa_mask);
	sigsend.sa_flags = 0;

	sigaction(SIGINT, &sigint, NULL);
	sigaction(SIGKILL, &sigsend, NULL);

	// Connect to MQTT
	pcap_connect();
	set_publish_function(pcap_publish_message);
	getMAC();
	init_list(100);

	// Open file to save captures
	capture = fopen("/tmp/pipe", "w+");
	if(capture == NULL){
		printf("Couldn't open file\n");
		exit(1);
	}

	// Run ubertooth command
	ubertooth_pid = fork();
	if(ubertooth_pid == 0){
		setpgid(getpid(),getpid()); //Move the process to another group process
		ubertooth_btle();
	}
	sleep(1);

	//Open pcap file
	handle = pcap_fopen_offline(capture, errbuf);
	if(handle == NULL){
	       	printf("Error opening file: %s\n",errbuf);
		exit(1);
	}

	breakloop = false;

	// Sniff loop
	packet_processor();

	return(0);
}

void packet_processor()
{
	const u_char * packet_data;
	struct pcap_pkthdr * packet_header;
	int pcap_next_ret = 0;
	bool random;
	BluetoothDeviceInfo bt_dev_info;

	pcap_next_ret =  pcap_next_ex(handle, &packet_header, &packet_data);
	while(pcap_next_ret >= 0 || pcap_next_ret == PCAP_ERROR_BREAK){
		random = is_random(packet_data[BTLE_PACKET_HEADER]);
		set_dev_info(&bt_dev_info, packet_data, random);

		if(check_device_in_list(bt_dev_info.mac_addr) == 0 && !breakloop){
			set_list_pointer();
			insert_in_list(bt_dev_info.mac_addr, devmac, bt_dev_info.dbm_signal, bt_dev_info.random);
			print_packet_info(&bt_dev_info);
		}
		free_dev_info(&bt_dev_info);
		
		pcap_next_ret =  pcap_next_ex(handle, &packet_header, &packet_data);
	}	
	printf("Packet processor interrupted: %s\n",pcap_statustostr(pcap_next_ret));

	return;
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
			sprintf(ubertooth_command, "timeout 5 ubertooth-btle -f -A %i -q /tmp/pipe > /dev/null 2>&1",index);
			printf("Changing channel to %i\n", index);
			//sprintf(ubertooth_command, "ubertooth-btle -f -A %i -q /tmp/pipe > /dev/null 2>&1",index);
			system(ubertooth_command);
		}	
	}
}

void disconnect (int s){

	printf("\nClosing...\n");
	breakloop = true;
	fclose(capture);
	free(devmac);
	free_info_list();
	kill(-ubertooth_pid,SIGTERM); //Use negative pid to kill all process of the group
	system("/bin/rm /tmp/pipe");
	//get_list_message();
}

void send_data(int s){
	printf("\n\n Enviando lista \n\n");

}

void getMAC(){
	FILE *fp;
	char hci_info[50];
	devmac = malloc(sizeof(char) * 18);
	char *ret;

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

bool is_random(const u_char random){
	if( (random >= 0x40 && random < 0x80) || random >= 0xC0) return 1;
	else return 0;
}
