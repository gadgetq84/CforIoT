#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>        
#include <sys/mman.h> 
#include <sys/time.h>      
#include <sched.h>         
#include <assert.h> 
#include <math.h> 
#define TERM RPI_GPIO_P1_07

uint8_t crc8(uint8_t *data, uint8_t len);
int presence(uint8_t pin);
void writeBit(uint8_t pin,int b);
uint8_t readBit(uint8_t pin);
int readByte(uint8_t pin);
void writeByte(uint8_t pin,int byte);
int readiButton(uint8_t pin, uint8_t *data);
int convert(uint8_t pin);
float getTemperature(uint8_t pin);
struct tm getdatetime();
void ftoa(float n, char* res, int afterpoint) ;

