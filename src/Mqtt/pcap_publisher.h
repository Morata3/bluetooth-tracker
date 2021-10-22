#include <mosquitto.h>

#define MQTT_HOST "10.42.0.1"
#define MQTT_PORT 1883
#define MQTT_TOPIC "pcap"
//#define MQTT_TOPIC "Televes"


void pcap_publish_message(char *message);
void pcap_connect();
void pcap_disconnect();
