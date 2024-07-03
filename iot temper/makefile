all: mqtt-datasender

mqtt-datasender: read_sensor.o main.o sender.o
	gcc -o mqtt-datasender  main.o read_sensor.o sender.o -lbcm2835 -lpaho-mqtt3cs -lm

main.o: main.c
	gcc -c --std=c99  -o main.o main.c  -lbcm2835 -lpaho-mqtt3cs 

read_sensor.o: read_sensor.c
	gcc -c --std=c99  -o read_sensor.o  read_sensor.c  -lbcm2835 -lm

sender.o: sender.c
	gcc -c --std=c99  -o sender.o  sender.c -lbcm2835 -lpaho-mqtt3cs -lm
clean:
	del *.o
	del mqtt-datasender
