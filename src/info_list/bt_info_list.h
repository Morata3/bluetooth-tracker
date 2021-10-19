#include <bson.h>
#include <semaphore.h>
#include <string.h>

typedef struct{
  bson_t ** list;
  size_t used;
  size_t size;
  sem_t list_sem;
}BtInfoList;

void init_list(size_t initial_size);
void insert_in_list(char *detected_mac, char *host_mac, int dbm_signal, bool random);
void free_info_list();
void set_list_pointer();
char * get_list_message();
void set_publish_function(void (*bt_publish_function)(char*));
int check_device_in_list(char *detected_mac);

