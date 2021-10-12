#include "bt_info_list.h"

BtInfoList bt_info_list;
void (*publish_function) (char*);

// FALTAN OS SEMAFOROS

void initialize_list(size_t initial_size){

	bson_t **info_list_pointer = malloc(initial_size *sizeof(bson_t));
	if(info_list_pointer != NULL){
		printf("Succesfully assigned %u bytes of memory\n", initial_size * sizeof(bson_t *));
		bt_info_list.list = info_list_pointer;
		bt_info_list.used = 0;
		bt_info_list.size = initial_size;
	}
	else printf("Couldn't assing so much memory\n");

}

char * get_list_message(){

	
}

void set_publish_function(void (*bt_publish_function)(char *)){
	publish_function = bt_publish_function;
}

void publish_list(){
	size_t current_size = bt_info_list.size;
	char *bt_info_message = get_list_message();
	publish_function(bt_info_message);
	free(bt_info_message);
	initialize_list(current_size);
}

void set_list_pointer(){

	if(bt_info_list.used == bt_info_list.size){
		printf("Assigning more memory to list\n");
		bson_t **list_pointer = realloc(bt_info_list.list, 10*bt_info_list.size * sizeof(bson_t *));
		if(list_pointer == NULL){
			printf("Couldn't assingn more memory. Publishing and restarting list\n");
			publish_list(bt_info_list);
		}
		else{
			bt_info_list.list = list_pointer;
			bt_info_list.size *= 10;
			printf("Memory assigned to list succesfully\n");
		}
	}
}



void insert_in_list(char *detected_mac, char *host_mac, int dbm_signal){
	
	bson_t *bt_info = bson_new();

	BSON_APPEND_UTF8(bt_info, "mac", detected_mac);
	BSON_APPEND_UTF8(bt_info, "routerID", host_mac);
	BSON_APPEND_INT32(bt_info, "signal", dbm_signal);
	BSON_APPEND_INT64(bt_info, "timestamp", (uint64_t)time(NULL) * 1000);

	bt_info_list.list[bt_info_list.used++] = bt_info;
}

void free_probe_list(){
	int index = 0;
	for(index = 0; index < bt_info_list.used; index++) bson_destroy(bt_info_list.list[index]);
	free(bt_info_list.list);
	bt_info_list.list = NULL;
	bt_info_list.used = bt_info_list.size = 0;
}
