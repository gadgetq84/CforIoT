#include "sender.h"

int initmqttconection(char *address,char *clientid, MQTTClient *client,MQTTClient_connectOptions *conn_opts,MQTTClient_deliveryToken *token,char *username,char *pass)
{  
    int rc;
       
    if ((rc = MQTTClient_create(client, address, clientid,MQTTCLIENT_PERSISTENCE_NONE,NULL))!= MQTTCLIENT_SUCCESS)
	{
		 printf("Failed to create client, return code %d\n", rc);
         exit(EXIT_FAILURE);
	}
    
    
	conn_opts->keepAliveInterval = 20;
    conn_opts->cleansession = 1;
    conn_opts->username = username;
    conn_opts->password = pass;
    
    if ((rc = MQTTClient_connect(*client, conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(-1);
    }
      rc = MQTTClient_waitForCompletion(client, *token, TIMEOUT);
    return rc;
	
}
int mqttsend(char *topic, MQTTClient_message *pubmsg,MQTTClient *client,MQTTClient_deliveryToken *token)
{
	int rc;
    MQTTClient_publishMessage(*client, topic, pubmsg, token);
        printf("Waiting for up to %d seconds for publication of %s\n"
            "on topic %s \n",
            (int)(TIMEOUT/1000), pubmsg->payload , topic);
    rc = MQTTClient_waitForCompletion(*client, *token, TIMEOUT);
    printf("Message with delivery token %d delivered\n", *token);
    return rc;
}

int mqttdisconect(MQTTClient *client)
{
	MQTTClient_disconnect(*client, 10000);
    MQTTClient_destroy(client);
	return 0;
}
//int main(int argc, char* argv[])
//{
    //MQTTClient client;
    //MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    //MQTTClient_message pubmsg = MQTTClient_message_initializer;
    //MQTTClient_deliveryToken token;
    //int rc;

    //MQTTClient_create(&client, ADDRESS, CLIENTID,
        //MQTTCLIENT_PERSISTENCE_NONE, NULL);
    //conn_opts.keepAliveInterval = 20;
    //conn_opts.cleansession = 1;
    //conn_opts.username = USERNAME;
    //conn_opts.password = PASS;

    //if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    //{
        //printf("Failed to connect, return code %d\n", rc);
        //exit(-1);
    //}
    //pubmsg.payload = PAYLOAD;
    //pubmsg.payloadlen = strlen(PAYLOAD);
    //pubmsg.qos = QOS;
    //pubmsg.retained = 0;
    //MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
    //printf("Waiting for up to %d seconds for publication of %s\n"
            //"on topic %s for client with ClientID: %s\n",
            //(int)(TIMEOUT/1000), PAYLOAD, TOPIC, CLIENTID);
    //rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    //printf("Message with delivery token %d delivered\n", token);
    //MQTTClient_disconnect(client, 10000);
    //MQTTClient_destroy(&client);
    //return rc;
//}
