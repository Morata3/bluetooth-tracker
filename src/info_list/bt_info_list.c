#include "bt_info_list.h"

BtInfoList bt_info_list;
void (*publish_function) (char*);

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
	
	int index;
	size_t message_size = 0;
	int printed_chars = 0;
	char *list_message;
	list_message = malloc(sizeof(char));
	list_message[printed_chars] = '{';
	printed_chars ++;
	message_size ++;
	for(index = bt_info_list.used -1; index > -1; index --){
		size_t bt_info_size;
		char *bt_info_string = bson_as_relaxed_extended_json(bt_info_list.list[index], &bt_info_size);

		message_size += bt_info_size +12; //Add 12 for "bt_info:" string
		list_message = realloc(list_message, message_size);

		if(index == 0) printed_chars += snprintf((list_message + printed_chars), (message_size - printed_chars) +1, "\"bt_info\":%s}", bt_info_string);
		else printed_chars += snprintf((list_message + printed_chars), (message_size - printed_chars) +1, "\"bt_info\":%s,", bt_info_string);
		bson_free(bt_info_string);
		bson_destroy(bt_info_list.list[index]);
		bt_info_list.size -= 1;
		bt_info_list.list = realloc(bt_info_list.list, bt_info_list.size * sizeof(bson_t*));
	}

	printf("Done. Message : %s\n", list_message);
	return list_message;
	
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

void publish_list_if_needed(){
	
	sem_wait(&(bt_info_list.list_sem));

	if(bt_info_list.used > 0){
		printf("Captured %i devices. Publishing...\n",bt_info_list.used);
		publish_list();
	}
	else printf("List is empty. Publish not necessary\n\n");
	sem_post(&(bt_info_list.list_sem));
}

void set_list_pointer(){
	
	sem_wait(&(bt_info_list.list_sem));

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

	sem_post(&(bt_info_list.list_sem));
}

void init_list(size_t initial_size){
	sem_init(&(bt_info_list.list_sem), 0, 1);
	sem_wait(&(bt_info_list.list_sem));
	initialize_list(initial_size);
	sem_post(&(bt_info_list.list_sem));
}

void insert_in_list(char *detected_mac, char *host_mac, int dbm_signal, bool random){
	
	sem_wait(&(bt_info_list.list_sem));

	bson_t *bt_info = bson_new();

	BSON_APPEND_UTF8(bt_info, "mac", detected_mac);
	BSON_APPEND_UTF8(bt_info, "routerID", host_mac);
	BSON_APPEND_INT32(bt_info, "signal", dbm_signal);
	BSON_APPEND_BOOL(bt_info, "random", random);
	BSON_APPEND_INT64(bt_info, "timestamp", (uint64_t)time(NULL) * 1000);

	bt_info_list.list[bt_info_list.used++] = bt_info;
	sem_post(&(bt_info_list.list_sem));
}

void free_info_list(){
	
	sem_wait(&(bt_info_list.list_sem));

	int index = 0;
	for(index = 0; index < bt_info_list.used; index++) bson_destroy(bt_info_list.list[index]);
	free(bt_info_list.list);
	bt_info_list.list = NULL;
	bt_info_list.used = bt_info_list.size = 0;
	printf("List release\n");

	sem_post(&(bt_info_list.list_sem));
}

int check_device_in_list(char *detected_mac){

	int index;
	bson_iter_t list_iterator;
	const bson_value_t *list_mac_value;
	char *mac_value;

	sem_wait(&(bt_info_list.list_sem));
	for(index = 0; index < bt_info_list.used; index ++){
		bson_t *info_in_list = bt_info_list.list[index];
		if(bson_iter_init_find(&list_iterator, info_in_list, "mac") && BSON_ITER_HOLDS_UTF8 (&list_iterator)){
			list_mac_value = bson_iter_value(&list_iterator);
			mac_value = list_mac_value->value.v_utf8.str;
		}

		if(strcmp(detected_mac, mac_value) == 0){
			sem_post(&(bt_info_list.list_sem));
			return -1;
		}
	}		
	sem_post(&(bt_info_list.list_sem));
	return 0;
	
}




