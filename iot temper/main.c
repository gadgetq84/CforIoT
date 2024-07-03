#include <stdio.h>
#include "read_sensor.h"
#include "sender.h"
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
void showHelp()
{
	printf(" This program takes temperature readings and sends them to the server via mqtt protocol\n\n");
	printf("  -H show help information\n");
	printf("  -h (server ip address):port\n");
  printf("  -l username \n");
  printf("  -p password \n");
  printf("  -t topic \n");
  printf("  -c clientid \n");
  
  
}

int main(int argc, char **argv) {
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
  printf("empty username, use default username IoT!\n");
  strcpy(username,"IoT");
}
if(pass[0]=='\0')
{
    printf("empty password!\n");
    return 1;
}
if(topic[0]=='\0')
  strcpy(topic,"/test/temp");
if(clientid[0]=='\0')
  strcpy(clientid,"ExampleClientPub");
if(address[0]=='\0')
  strcpy(address,"192.168.0.104:1883");
 
printf("%s\n",address);
printf("%s\n",username);
printf("%s\n",topic);
printf("%s\n",clientid);

    const struct sched_param priority = {1};
    sched_setscheduler(0, SCHED_FIFO, &priority);
    mlockall(MCL_CURRENT | MCL_FUTURE);

    if (!bcm2835_init())
        return 1;
    bcm2835_gpio_fsel(TERM, BCM2835_GPIO_FSEL_INPT);
    double DELAY =2.0;
    float curtemp=0;
    struct tm curtime;
    
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int initmqtt =  initmqttconection(address,clientid, &client, &conn_opts,&token,username,pass);
    char res[20]; 
    
while (1) {
    clock_t begin = clock();
    //bcm2835_delay(5000);
    system("clear");
    if(presence(TERM)==0)
    {
      
        curtemp=getTemperature(TERM);
        ftoa(curtemp, res, 4); 
        curtime = getdatetime();
        if(curtemp!=-2000 && curtemp!=-1000)
            {
              pubmsg.payload = res;
              pubmsg.payloadlen = strlen(res);
              pubmsg.qos = QOS;
              pubmsg.retained = 0;
              
               mqttsend(topic,&pubmsg,&client,&token);
//                printf("%f\n",curtemp);
            }
      //uint8_t code[8];
      //int i;
      //int p = readiButton(TERM, code);
      //if (p == 1) 
        //{
        //for (i = 0; i < 8; i++) {
          //printf("%hhX ", code[i]);
        //}
        //printf("\n\r");
        //fflush(stdout);
        //}

    }
while ((double)(clock() - begin)/CLOCKS_PER_SEC<DELAY)
		{}    
}
mqttdisconect(&client);
}
