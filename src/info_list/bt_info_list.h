#include <bson.h>
#include <semaphore.h>
#include <string.h>

typedef struct{
  bson_t ** list;
  size_t used;
  size_t size;
  sem_t list_sem;
}BtInfoList;

void insert_in_list(char *detected_mac, char *host_mac, int dbm_signal);
void free_probe_list();
void set_list_pointer();

