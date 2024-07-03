#include "read_sensor.h"
#include <time.h>

void reverse(char* str, int len) 
{ 
    int i = 0, j = len - 1, temp; 
    while (i < j) { 
        temp = str[i]; 
        str[i] = str[j]; 
        str[j] = temp; 
        i++; 
        j--; 
    } 
} 

int intToStr(int x, char str[], int d) 
{ 
    int i = 0; 
    while (x) { 
        str[i++] = (x % 10) + '0'; 
        x = x / 10; 
    } 
    while (i < d) 
        str[i++] = '0'; 
    reverse(str, i); 
    str[i] = '\0'; 
    return i; 
} 

void ftoa(float n, char* res, int afterpoint) 
{ 
    int ipart = (int)n; 
    float fpart = n - (float)ipart; 
    int i = intToStr(ipart, res, 0); 
     if (afterpoint != 0) { 
        res[i] = '.'; 
        fpart = fpart * pow(10, afterpoint); 
        intToStr((int)fpart, res + i + 1, afterpoint); 
    } 
} 

struct tm  getdatetime()
{
	time_t mytime;
	mytime =time(NULL);
	struct tm date = *localtime(&mytime);
	return date;
}

uint8_t crc8(uint8_t *data, uint8_t len) {

    uint8_t i;
    uint8_t j;
    uint8_t temp;
    uint8_t databyte;
    uint8_t crc = 0;
    for (i = 0; i < len; i++) {
        databyte = data[i];
        for (j = 0; j < 8; j++) {
            temp = (crc ^ databyte) & 0x01;
            crc >>= 1;
            if (temp)
                crc ^= 0x8C;

            databyte >>= 1;
        }
    }

    return crc;
}

int presence(uint8_t pin) {
    bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(pin, LOW);
    bcm2835_delayMicroseconds(480);
    bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);
    bcm2835_delayMicroseconds(70);
    uint8_t b = bcm2835_gpio_lev(pin);
    bcm2835_delayMicroseconds(410);
    return b;
}

void writeBit(uint8_t pin,int b) {  
    int delay1, delay2;
    if (b == 1) {
        delay1 = 6;
        delay2 = 64;
    } else {
        delay1 = 80;
        delay2 = 10;
    }
    bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(pin, LOW);
    bcm2835_delayMicroseconds(delay1);
    bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);
    bcm2835_delayMicroseconds(delay2);
}

uint8_t readBit(uint8_t pin) {
    bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(pin, LOW);
    bcm2835_delayMicroseconds(6);
    bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);
    bcm2835_delayMicroseconds(8);
    uint8_t b = bcm2835_gpio_lev(pin);
    bcm2835_delayMicroseconds(55);
    return b;
}

int readByte(uint8_t pin) {
    int byte = 0;
    int i;
    for (i = 0; i < 8; i++) {
        byte = byte | readBit(pin) << i;
    };
    return byte;
}

void writeByte(uint8_t pin,int byte) {
    int i;
    for (i = 0; i < 8; i++) {
        if (byte & 1) {
            writeBit(pin,1);
        } else {
            writeBit(pin,0);
        }
        byte = byte >> 1;
    }
}

int readiButton(uint8_t pin, uint8_t *data) {
    int b = presence(pin);
    if (b != 0) return 0;
    writeByte(pin, 0x33);
    int i;
    for (i = 0; i < 8; i++) {
        data[i] = readByte(pin);
    }
    uint8_t crc = crc8(data, 8);
    if (crc == 0) return 1;
    return 0;
}

int convert(uint8_t pin) {
    int i;
    writeByte(pin, 0x44);
    for (i = 0; i < 1000; i++) {
        bcm2835_delayMicroseconds(100000);
        if (readBit(pin) == 1)break;
    }
    return i;
}

float getTemperature(uint8_t pin) {
    if (presence(pin) == 1) return -1000;
    writeByte(pin, 0xCC);
    convert(pin);
    presence(pin);
    writeByte(pin, 0xCC);
    writeByte(pin, 0xBE);
    int i;
    uint8_t data[9];
    for (i = 0; i < 9; i++) {
        data[i] = readByte(pin);
    }
    uint8_t crc = crc8(data, 9);
    if(crc!=0) return -2000;
    int t1 = data[0];
    int t2 = data[1];
    int16_t temp1 = (t2 << 8 | t1);
    float temp = (float) temp1 / 16;
    return temp;
}
