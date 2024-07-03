#include <MQTTClient.h>
#include <MQTTClientPersistence.h>
#include "pubsub_opts.h"
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


#if defined(_WIN32)
#define sleep Sleep
#else
#include <sys/time.h>
#endif


volatile int toStop = 0;


struct pubsub_opts opts =
{
	0, 0, 0, 0, "\n", 100,  	/* debug/app options */
	NULL, NULL, 1, 0, 0, /* message options */
	MQTTVERSION_DEFAULT, "/test/temp", "paho-cs-sub", 0, 0, "IoT", "123456Zz!", "192.168.0.104", "1883", NULL, 10, /* MQTT options */
	NULL, NULL, 0, 0, /* will options */
	0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* TLS options */
	0, {NULL, NULL}, /* MQTT V5 options */
	NULL, NULL, /* HTTP and HTTPS proxies */
};

void showHelp()
{
	printf(" This program subscriber mqtt protocol\n\n");
	printf("  -H show help information\n");
	printf("  -h (server ip address):port\n");
  printf("  -l username \n");
  printf("  -p password \n");
  printf("  -t topic \n");
  printf("  -c clientid \n");
  
  
}


int myconnect(MQTTClient client)
{
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	MQTTClient_SSLOptions ssl_opts = MQTTClient_SSLOptions_initializer;
	MQTTClient_willOptions will_opts = MQTTClient_willOptions_initializer;
	int rc = 0;

	if (opts.verbose)
		printf("Connecting\n");

	if (opts.MQTTVersion == MQTTVERSION_5)
	{
		MQTTClient_connectOptions conn_opts5 = MQTTClient_connectOptions_initializer5;
		conn_opts = conn_opts5;
	}

	conn_opts.keepAliveInterval = opts.keepalive;
	conn_opts.username = opts.username;
	conn_opts.password = opts.password;
	conn_opts.MQTTVersion = opts.MQTTVersion;
	conn_opts.httpProxy = opts.http_proxy;
	conn_opts.httpsProxy = opts.https_proxy;

	if (opts.will_topic) 	/* will options */
	{
		will_opts.message = opts.will_payload;
		will_opts.topicName = opts.will_topic;
		will_opts.qos = opts.will_qos;
		will_opts.retained = opts.will_retain;
		conn_opts.will = &will_opts;
	}

	if (opts.connection && (strncmp(opts.connection, "ssl://", 6) == 0 ||
			strncmp(opts.connection, "wss://", 6) == 0))
	{
		if (opts.insecure)
			ssl_opts.verify = 0;
		else
			ssl_opts.verify = 1;
		ssl_opts.CApath = opts.capath;
		ssl_opts.keyStore = opts.cert;
		ssl_opts.trustStore = opts.cafile;
		ssl_opts.privateKey = opts.key;
		ssl_opts.privateKeyPassword = opts.keypass;
		ssl_opts.enabledCipherSuites = opts.ciphers;
		conn_opts.ssl = &ssl_opts;
	}

	if (opts.MQTTVersion == MQTTVERSION_5)
	{
		MQTTProperties props = MQTTProperties_initializer;
		MQTTProperties willProps = MQTTProperties_initializer;
		MQTTResponse response = MQTTResponse_initializer;

		conn_opts.cleanstart = 1;
		response = MQTTClient_connect5(client, &conn_opts, &props, &willProps);
		rc = response.reasonCode;
		MQTTResponse_free(response);
	}
	else
	{
		conn_opts.cleansession = 1;
		rc = MQTTClient_connect(client, &conn_opts);
	}

	if (opts.verbose && rc == MQTTCLIENT_SUCCESS)
		fprintf(stderr, "Connected\n");
	else if (rc != MQTTCLIENT_SUCCESS && !opts.quiet)
		fprintf(stderr, "Connect failed return code: %s\n", MQTTClient_strerror(rc));

	return rc;
}


void cfinish(int sig)
{
	signal(SIGINT, NULL);
	toStop = 1;
}


void trace_callback(enum MQTTCLIENT_TRACE_LEVELS level, char* message)
{
	fprintf(stderr, "Trace : %d, %s\n", level, message);
}


int main(int argc, char** argv)
{
	  int rez_param = 0;
  int help = 0;
  char address [256] ={0};
  char username[256] = {0};
  char pass[256] = {0};
  char topic[256] = {0};
  char clientid[256] = {0};
  
while ((rez_param = getopt(argc,argv,"Hh:l:p:t:c:"))!=-1)
	{
		switch(rez_param)
		{
			case 'H': 
				help =1; 
				
				break;
			case 'h': 
				if(strlen(optarg)<257)
					strcpy(address,optarg); 
				else
					{
						printf("maximum size 256 characters \n",optarg);
						return 1;
					}
				break;
			case 'l': 
				if(strlen(optarg)<257)
					strcpy(username,optarg); 
				else
					{
						printf("maximum size 256 characters \n",optarg);
						return 1;
					}
				break;
			case 'p': 
				if(strlen(optarg)<257)
					strcpy(pass,optarg); 
				else
					{
						printf("maximum size 256 characters \n",optarg);
						return 1;
					}
				break;
			case 't': 
				if(strlen(optarg)<257)
					strcpy(topic,optarg); 
				else
					{
						printf("maximum size 256 characters \n",optarg);
						return 1;
					}
				break;
			case 'c': 
				if(strlen(optarg)<257)
					strcpy(clientid,optarg); 
				else
					{
						printf("maximum size 256 characters \n",optarg);
						return 1;
					}
				break;
			case '?': 
				printf("Error argument found, use -H!!!\n"); 
				return 1;
				break;
		}
	}
  
	if(help==1)
	{
	  showHelp();
	  return 0;
	}

	if(username[0]=='\0')
	{
		printf("empty username, use default username %s\n",opts.username);
	  opts.username="IoT";
	}else
	{
		opts.username=username;
	}

	if(pass[0]=='\0')
	{
		printf("empty password!\n");
		return 1;
	}else
	{
		opts.password=pass;
	}
	if(topic[0]=='\0')
		printf("empty topic, use default topic %s\n",opts.topic);
	else
		opts.topic=topic;
		
	if(clientid[0]=='\0')
		opts.clientid="ExampleClientPub";
	else
		opts.clientid=clientid;

	if(address[0]=='\0')
	{
		printf("empty address, use default address %s\n","192.168.0.104:1883");
		opts.connection="192.168.0.104:1883";
	}
	else
		opts.connection=address;

	if(opts.connection)
		printf("%s\n",opts.connection);
	if(opts.username)
		printf("%s\n",opts.username);
	if(opts.topic)
		printf("%s\n",opts.topic);
	if(opts.clientid)
		printf("%s\n",opts.clientid);
	
	
	FILE *file;
    file = fopen("temp.csv","a");
    if(file==NULL)
    {
		printf("error open file");
		return 0;
	}
	MQTTClient client;
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	MQTTClient_createOptions createOpts = MQTTClient_createOptions_initializer;
	int rc = 0;
	char* url;
	const char* version = NULL;
#if !defined(_WIN32)
    struct sigaction sa;
#endif
	const char* program_name = "sub";
	MQTTClient_nameValue* infos = MQTTClient_getVersionInfo();

/*
	if (argc < 2)
		usage(&opts, (pubsub_opts_nameValue*)infos, program_name);

	if (getopts(argc, argv, &opts) != 0)
		usage(&opts, (pubsub_opts_nameValue*)infos, program_name);
*/
	if (strchr(opts.topic, '#') || strchr(opts.topic, '+'))
		opts.verbose = 1;

	if (opts.connection)
		url = opts.connection;
	else
	{
		url = malloc(100);
		sprintf(url, "%s:%s", opts.host, opts.port);
	}
	if (opts.verbose)
		printf("URL is %s\n", url);

	if (opts.tracelevel > 0)
	{
		MQTTClient_setTraceCallback(trace_callback);
		MQTTClient_setTraceLevel(opts.tracelevel);
	}

	if (opts.MQTTVersion >= MQTTVERSION_5)
		createOpts.MQTTVersion = MQTTVERSION_5;
	rc = MQTTClient_createWithOptions(&client, url, opts.clientid, MQTTCLIENT_PERSISTENCE_NONE,
			NULL, &createOpts);
	if (rc != MQTTCLIENT_SUCCESS)
	{
		if (!opts.quiet)
			fprintf(stderr, "Failed to create client, return code: %s\n", MQTTClient_strerror(rc));
		exit(EXIT_FAILURE);
	}


#if defined(_WIN32)
	signal(SIGINT, cfinish);
	signal(SIGTERM, cfinish);
#else
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = cfinish;
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
#endif

	if (myconnect(client) != MQTTCLIENT_SUCCESS)
		goto exit;

	if (opts.MQTTVersion >= MQTTVERSION_5)
	{
		MQTTResponse response = MQTTClient_subscribe5(client, opts.topic, opts.qos, NULL, NULL);
		rc = response.reasonCode;
		MQTTResponse_free(response);
	}
	else
		rc = MQTTClient_subscribe(client, opts.topic, opts.qos);
	if (rc != MQTTCLIENT_SUCCESS && rc != opts.qos)
	{
		if (!opts.quiet)
			fprintf(stderr, "Error %d subscribing to topic %s\n", rc, opts.topic);
		goto exit;
	}

	while (!toStop)
	{
		time_t mytime = time (NULL);
		struct tm *now = localtime(&mytime);
		char* topicName = NULL;
		int topicLen;
		MQTTClient_message* message = NULL;

		rc = MQTTClient_receive(client, &topicName, &topicLen, &message, 1000);
		if (rc == MQTTCLIENT_DISCONNECTED)
			myconnect(client);
		else if (message)
		{
			size_t delimlen = 0;

			if (opts.verbose)
				printf("%s\t", topicName);
			if (opts.delimiter)
				delimlen = strlen(opts.delimiter);
			if (opts.delimiter == NULL || (message->payloadlen > delimlen &&
				strncmp(opts.delimiter, &((char*)message->payload)[message->payloadlen - delimlen], delimlen) == 0))
				printf("%.*s", message->payloadlen, (char*)message->payload);
			else
				printf("%.*s%s", message->payloadlen, (char*)message->payload, opts.delimiter);
//			if (message->struct_version == 1 && opts.verbose)
//				logProperties(&message->properties);
			char str[255]={0};
			char time[20];
			char date[20];
			
			strftime(date,sizeof(str),"%D",now);
			strftime(time,sizeof(str),"%T",now);

			strcat(str,(char*)message->payload);
			//printf("%s\n",str);
			printf("%s\n",time);
			printf("%s\n",date);
			
			fseek(file, 0, SEEK_END);
			fprintf(file,"Temp: %s Time: %s Date: %s\n",str,time,date);

			fflush(stdout);
			MQTTClient_freeMessage(&message);
			MQTTClient_free(topicName);
		}
	}

exit:
	fclose(file);
	MQTTClient_disconnect(client, 0);

	MQTTClient_destroy(&client);

	return EXIT_SUCCESS;
}
