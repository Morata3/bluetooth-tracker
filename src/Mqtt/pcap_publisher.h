#include <mosquitto.h>

#define MQTT_HOST "10.42.0.1"
#define MQTT_PORT 1883
#define MQTT_PORT_LOG 9001
#define MQTT_TOPIC "bluetooth"
//#define MQTT_TOPIC "Televes"

#define USERNAME "aimars" 
#define PASSWORD "aimars2021"


void pcap_publish_message(char *message);
void pcap_connect();
void pcap_disconnect();

void log_publish_message(char *message);
void set_log_topic(char *new_topic);
void log_connect();
void log_disconnect();
