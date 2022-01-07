#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "pcap_publisher.h"

struct mosquitto *mosq;
void connect_callback(struct mosquitto *mosq, void *obj, int result);
void publish_callback(struct mosquitto *mosq, void *obj, int mid);
void disconnect_callback(struct mosquitto *mosq, void *obj, int rc);

void connect_callback(struct mosquitto *mosq, void *obj, int result) {
	switch (result) {
		case 0:
			printf("Conexión establecida con éxito\n");
			break;
		case 1:
			printf("Conexión rexeitada (versión de protocolo inaceptable)\n");
			break;
		case 2:
			printf("Conexión rexeitada (identificación rexeitada)\n");
			break;
		case 3:
			printf("Conexión rexeitada (broker non dispoñible)\n");
			break;

	}
}

void publish_callback(struct mosquitto *mosq, void *obj, int mid){

	printf("Message sent to the broker succesfully\n");
}

void disconnect_callback(struct mosquitto *mosq, void *obj, int rc){

	printf("Disconnected from broker\n");
}

void pcap_connect(){

	int keepalive = 60;
	bool clean_session = true;
  
  	mosquitto_lib_init();
  	mosq = mosquitto_new(NULL, clean_session, NULL);
  	if(!mosq){
		printf("Error: Out of memory.\n");
		exit(1);
	}
  
	mosquitto_username_pw_set(mosq,USERNAME,PASSWORD);
	mosquitto_connect_callback_set(mosq, connect_callback);
	mosquitto_publish_callback_set(mosq, publish_callback);
	mosquitto_disconnect_callback_set(mosq, disconnect_callback);

  	int connect_return = mosquitto_connect(mosq, MQTT_HOST, MQTT_PORT, keepalive);
	printf("mosquitto_connect returned: %i\n", connect_return);
 
}

void pcap_disconnect(){

	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();

}

void pcap_publish_message(char *message){

  printf("\nIntentando a publicación\n");
  int connection_status = mosquitto_loop(mosq, -1, 1);
  if(connection_status == MOSQ_ERR_NO_CONN){
	  printf("Not connected to broker. Trying to stablish connection\n");
  	  mosquitto_connect(mosq, MQTT_HOST, MQTT_PORT, 60);
  }
  else if(connection_status == MOSQ_ERR_CONN_LOST){
	  printf("Connection lost with broker. Reconnecting\n");
	  mosquitto_reconnect(mosq);

  }

  printf("Publishing %s message\n", MQTT_TOPIC);
  int ext_code = mosquitto_publish(mosq, NULL, MQTT_TOPIC, strlen(message), message, 0, 0);
  switch(ext_code){
	  case MOSQ_ERR_SUCCESS:
  		printf("mosquitto_publish (%i): success\n", ext_code);
		break;
	  case MOSQ_ERR_INVAL:
  		printf("mosquitto_publish (%i): input parameters were invalid\n", ext_code);
		break;
	  case MOSQ_ERR_NOMEM:
  		printf("mosquitto_publish (%i): out of memory condition occurred\n", ext_code);
		break;
	  case MOSQ_ERR_NO_CONN:
  		printf("mosquitto_publish (%i): client isn’t connected to a broker\n", ext_code);
		break;
	  case MOSQ_ERR_PROTOCOL:
  		printf("mosquitto_publish (%i): protocol error communicating with the broker\n", ext_code);
		break;
	  case MOSQ_ERR_PAYLOAD_SIZE:
  		printf("mosquitto_publish (%i): payloadlen is too large\n", ext_code);
		break;
	  case MOSQ_ERR_MALFORMED_UTF8:
  		printf("mosquitto_publish (%i): topic is not valid UTF-8\n", ext_code);
		break;
  }

}


