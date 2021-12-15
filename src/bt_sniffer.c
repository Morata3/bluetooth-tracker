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

#define TIME_TO_PUBLISH 10

pcap_t *handle;
pid_t ubertooth_pid, parent_pid;
FILE *capture;
char *devmac;
static char *FILENAME = "/tmp/pipe";

void packet_processor();
void ubertooth_btle();
void disconnect(int s);
void send_data(int s);
void send_and_stop(int s);
void getMAC();
bool is_random(const u_char header);

int main(int argc, char *argv[])
{
	char errbuf [PCAP_ERRBUF_SIZE];
	struct sigaction sigint;
	struct sigaction sigsend;
	struct sigaction sigsendstop;

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

	//TODO Connect to MQTT Logger
	//set_log_topic(devmac);
	//log_connect();

	// Create pipe for captures
	unlink(FILENAME);
	if(mkfifo(FILENAME,0750) == -1){
		printf("Error creating pipe\n");
		exit(1);
	}

	// Run ubertooth command
	ubertooth_pid = fork();
	if(ubertooth_pid == 0){
		setpgid(getpid(),getpid());
		ubertooth_btle();
	}

	if((capture = fopen(FILENAME, "r")) == NULL){
		printf("Couldn't open file\n");
		exit(1);
	}

	//Open pcap file
	handle = pcap_fopen_offline(capture, errbuf);
	if(handle == NULL){
	       	printf("Error opening file: %s\n",errbuf);
		exit(1);
	}

	// Sniff loop
	alarm(TIME_TO_PUBLISH); //Publish list after TIME_TO_PUBLISH seconds
	printf("Starting sniffer loop...\n");
	//log_publish_message("Starting sniffer...\n");
	fflush(stdout);
	int loop_ret;
	do{
		loop_ret = pcap_loop(handle, 0, packet_processor, NULL);
	}while(loop_ret != PCAP_ERROR_BREAK);

	return(0);
}

void packet_processor(u_char *args, const struct pcap_pkthdr *header, const u_char *packet_data)
{
	bool random;
	const u_char packet_header = packet_data[PACKET_TYPE];
	const u_char packet_type = packet_header & 0xF;

	if(packet_type <= ADV_NONCONN_IND){
		BluetoothDeviceInfo bt_dev_info;
		random = is_random(packet_header);
		set_ble_dev_info(&bt_dev_info, packet_data, random);

		if(check_device_in_list(bt_dev_info.mac_addr) == 0){
			set_list_pointer();
			insert_ble_in_list(bt_dev_info.mac_addr, devmac, bt_dev_info.dbm_signal, bt_dev_info.mac_type);
			//printf("MAC: %s --> TYPE: %s\n",  bt_dev_info.mac_addr, bt_dev_info.mac_type);
		}
		free_ble_dev_info(&bt_dev_info);
	}
	return;
}

void ubertooth_btle(){
	char ubertooth_command[50];
	while(1){
		sprintf(ubertooth_command, "ubertooth-btle -n -q %s -A 38 > /dev/null 2>&1", FILENAME);
		system(ubertooth_command);
	}
}

void disconnect (int s){

	printf("\nClosing...\n");
	pcap_disconnect();
	pcap_breakloop(handle);
	pcap_close(handle);
	free(devmac);
	free_info_list();
	kill(-ubertooth_pid,SIGTERM); //Use negative pid to kill all process of the group

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
	devmac = malloc(sizeof(char) * 25);
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
	strcat(devmac,":UB");
       	pclose(fp);
       	return;
	    
}

bool is_random(const u_char header){
	if( (header >= 0x40 && header < 0x80) || header >= 0xC0) return 1;
	else return 0;
}
