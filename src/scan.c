#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>


int main(int argc, char **argv)
{
    inquiry_info *ii = NULL;
    int max_rsp, num_rsp;
    int dev_id, sock, len, flags;
    int i;
    char addr[19] = { 0 };
    char name[248] = { 0 };
    bdaddr_t bdaddr;
		
    struct hci_conn_info_req *cr;
    unsigned int ptype;
    int to;
    uint16_t handle, accuracy;
    uint8_t which, rssi, role;
    uint32_t clock;
   	
    which = 0x01; // 0 for local clock, 1 for piconet clock
    role = 0x01; // 1 master, 0 slave
    ptype = HCI_DM1 | HCI_DM3 | HCI_DM5 | HCI_DH1 | HCI_DH3 | HCI_DH5;
    len  = 8;
    max_rsp = 255;
    flags = IREQ_CACHE_FLUSH;

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
  
    ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));

    //Requesting info devices
    num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
    if( num_rsp < 0 ) perror("hci_inquiry");

    for (i = 0; i < num_rsp; i++) {
	bdaddr = (ii+i)->bdaddr;
        ba2str(&bdaddr, addr);
        memset(name, 0, sizeof(name));
	if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), 
            name, 0) < 0)
		strcpy(name, "[unknown]");
	
	//Connecting device to request information
/*	if (hci_create_connection(sock, &bdaddr, htobs(ptype),htobs(0x0000), role, &handle, 25000) < 0)
	       	perror("Can't create connection");
                                                                                              
	cr = malloc(sizeof(*cr) + sizeof(struct hci_conn_info));
	if (!cr) {
		perror("Can't allocate memory");
		exit(1);
	}

	bacpy(&cr->bdaddr, &bdaddr);
	cr->type = ACL_LINK;
	if (ioctl(sock, HCIGETCONNINFO, (unsigned long) cr) < 0) {
		perror("Get connection info failed");
		exit(1);
	}
	handle = htobs(cr->conn_info->handle);

	//Read clock
	if (hci_read_clock(sock, handle, which, &clock, &accuracy, 1000) < 0) {
		perror("Reading clock failed");
		exit(1);
    	}
	accuracy = btohs(accuracy);
	
	//Read signal strength
	if (hci_read_rssi(sock, htobs(cr->conn_info->handle), &rssi, 1000) < 0) {
		perror("Read RSSI failed");
		exit(1);
	}
	free(cr);
*/
     	printf("%s  %s\n", addr, name);
    	printf("Clock:    0x%4.4x\n", btohl(clock));
	printf("Accuracy: %.2f msec\n", (float) accuracy * 0.3125);
	printf("Signal strength: %d\n", rssi);
    }

    free( ii );
    close( sock );
    return 0;
}
