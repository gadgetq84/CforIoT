#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <MQTTClient.h>
#define QOS         1
#define TIMEOUT     10000L
int initmqttconection(char *address,char *clientid, MQTTClient *client,MQTTClient_connectOptions *conn_opts,MQTTClient_deliveryToken *token,char *username,char *pass);
int mqttsend(char *topic, MQTTClient_message *pubmsg,MQTTClient *client,MQTTClient_deliveryToken *token);
int mqttdisconect(MQTTClient *client);

