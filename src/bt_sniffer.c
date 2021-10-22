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
pid_t ubertooth_pid, parent_pid;
FILE *capture;
char *devmac;
static char *FILENAME = "/tmp/pipe";

void packet_processor(u_char *args, const struct pcap_pkthdr *header, const u_char *buffer);
void print_packet_info(BluetoothDeviceInfo *bt_dev_info);
void ubertooth_btle(pid_t parent_pid);
void disconnect(int s);
void send_data(int s);
void getMAC();
bool is_random(const u_char header);

int main(int argc, char *argv[])
{
	char errbuf [PCAP_ERRBUF_SIZE];
	struct sigaction sigint;
	struct sigaction sigsend;
	int file_d;

	sigint.sa_handler = disconnect;
	sigemptyset(&sigint.sa_mask);
	sigint.sa_flags = 0;

	sigsend.sa_handler = send_data;
	sigemptyset(&sigsend.sa_mask);
	sigsend.sa_flags = 0;

	sigaction(SIGINT, &sigint, NULL);
	sigaction(SIGALRM, &sigsend, NULL);

	// Connect to MQTT
	pcap_connect();
	set_publish_function(pcap_publish_message);
	getMAC();
	init_list(100);

	// Create pipe for captures
	unlink(FILENAME);
	if(mkfifo(FILENAME,0750) == -1){
		printf("Error creating pipe\n");
		exit(1);
	}

	if((file_d = open(FILENAME, O_RDWR)) == -1 ){
		printf("Error opening pipe\n");
		exit(1);
	}

	if((capture = fdopen(file_d, "w+")) == NULL){
		printf("Couldn't open file\n");
		exit(1);
	}

	// Run ubertooth command
	parent_pid = getpid();
	ubertooth_pid = fork();
	if(ubertooth_pid == 0){
		setpgid(getpid(),getpid()); //Move the process to another group process
		ubertooth_btle(parent_pid);
	}

	//Open pcap file
	handle = pcap_fopen_offline(capture, errbuf);
	if(handle == NULL){
	       	printf("Error opening file: %s\n",errbuf);
		exit(1);
	}

	// Sniff loop
	int loop_ret;
	do{
		loop_ret = pcap_loop(handle, 0, packet_processor, NULL);
	}while(loop_ret != PCAP_ERROR_BREAK);

	return(0);
}

void packet_processor(u_char *args, const struct pcap_pkthdr *header, const u_char *packet_data)
{
	bool random;
	const u_char packet_header = packet_data[PACKET_HEADER];
	
	BluetoothDeviceInfo bt_dev_info;

	random = is_random(packet_header);
	set_dev_info(&bt_dev_info, packet_data, random);

	if(check_device_in_list(bt_dev_info.mac_addr) == 0){
		set_list_pointer();
		insert_in_list(bt_dev_info.mac_addr, devmac, bt_dev_info.dbm_signal, bt_dev_info.random);
		print_packet_info(&bt_dev_info);
	}
	free_dev_info(&bt_dev_info);
	
	return;
}

void print_packet_info(BluetoothDeviceInfo *bt_dev_info)
{
	printf("MAC: %s\n", get_dev_addr(bt_dev_info));
	printf("RSSI: %d dBm\n",get_dev_rssi(bt_dev_info));
}

void ubertooth_btle(pid_t parent_pid){
	int index;
	char ubertooth_command[100];
	
	while(1){
		for(index = 37; index <= 39; index ++){
			//sprintf(ubertooth_command, "ubertooth-btle -n -A %i -q /tmp/pipe > /dev/null 2>&1",index);
			sprintf(ubertooth_command, "timeout 5 ubertooth-btle -n -A %i -q %s > /dev/null 2>&1",index, FILENAME);
			printf("Changing channel to %i\n", index);
			system(ubertooth_command);
		}
		kill(parent_pid, SIGALRM);
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

bool is_random(const u_char header){
	if( (header >= 0x40 && header < 0x80) || header >= 0xC0) return 1;
	else return 0;
}
