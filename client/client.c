/*!
 * Simple chat program (client side).cpp - http://github.com/hassanyf
 * Version - 2.0.1
 *
 * Copyright (c) 2016 Hassan M. Yousuf
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>

int main()
{
    /* ---------- ИНИЦИАЛИЗАЦИЯ ПЕРЕМЕННЫХ КЛИЕНТА---------- */
    
    /*  
       1. client - файл-дескриптор, хранящий значение сокета,
       которые вернула система при подключении к серверу.
       2. portNum нужен для хранения номера порта, на котором происходит соединение.
       3. isExit - булевая переменная, признак завершения программы.
       4. Клиент считывает сообщение из сокета соединения в буффер обмена для приема/отправки данных к/от сервера
       5. sockaddr_in - структура, содержащая интернет адрес, с которым будет установлено соединение.
       Эта структура уже определена в netinet/in.h, поэтому нет необходимости
       заново ее задавать.
        DEFINITION:   
        struct sockaddr_in
        {
          short   sin_family;
          u_short sin_port;
          struct  in_addr sin_addr;
          char    sin_zero[8];
        };

        6. serv_addr будет содержать адрес сервера
    */

    int client;
    int portNum = 5500; // Номер порта один для сервера и клиента!
    bool isExit = false;
    int bufsize = 1024;
    char buffer[bufsize];
    char* ip = "127.0.0.1";

    struct sockaddr_in server_addr;

    /*
        Socket() функция создает новый сокет.
        На вход получает 3 аргумента,
            a. AF_INET: доменный адрес сокета.
            b. SOCK_STREAM: тип сокета. Потоковый сокет, в котором сообщение 
            читается последовательным потоком(TCP).
            c. Третий это аргумент протокола: всегда должен быть 0. 
            Операционная система выберет самый подходящий протокол.
            Функция возвращает файл-дескриптор целое число - соединение, которое используется для всех ссылок 
            на этот сокет. Если произошла ошибка соединения, то возвращается -1.
    */
    
    /* ---------- УСТАНОВКА СОКЕТ ПОДКЛЮЧЕНИЯ ----------*/
    /* --------------- socket() функция ------------------*/

    client = socket(AF_INET, SOCK_STREAM, 0);

    printf("CLIENT\n");

    if (client < 0) 
    {
        printf("Error establishing socket...\n");
        exit(1);
    }

    printf("=> Socket client has been created...\n");
    
    /* 
        Переменная serv_addr - это структура sockaddr_in. 
        sin_family содержит код для адресной семьи. 
        Всегда должна быть установлена AF_INET.
        htons() переводит номер порта из порядка байтов хоста 
        в номер порта в порядке байтов сети.
    */

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portNum);

	// эта функция возвращает 1, если IP-адрес действительный
    // и 0, если недействительный
    // inet_pton переводит IP-адрес в пакеты
    // inet_ntoa переводит обратно из пакета в IP-адрес
    
    inet_pton(AF_INET, ip, &server_addr.sin_addr);

    /* ---------- СОЕДИНЕНИЕ СОКЕТА ---------- */
    /* ---------------- connect() ---------------- */

    if (connect(client,(struct sockaddr *)&server_addr, sizeof(server_addr)) == 0)
        printf("=> Connection to the server %s with port number: %d\n",inet_ntoa(server_addr.sin_addr),portNum);

    if (connect(client,(struct sockaddr *)&server_addr, sizeof(server_addr)) == 0)
        printf("=> Connection to the server port number: %d \n",portNum);

    /* 
        Функция connect вызывается клиентом для установки соединения с сервером. 
        Ей нужны 3 аргумента: файловый дескриптор сокета, адрес хоста, к которому
        клиент хочет подключится (включая номер порта), и  размер адреса.
        Функция возвращает 0 при успехе и 1 при неудаче.
        Примечание: клиенту нужно знать номер порта сервера, 
        а не свой номер порта.
    */

    printf("=> Awaiting confirmation from the server...\n");
    recv(client, buffer, bufsize, 0);
    printf("=> Connection confirmed, you are good to go...\n");

    printf("\n\n=> Enter # to end the connection\n");

    // При достижении соединения, клиент может послать сообщение первым.
    
    do {
		printf("Client: ");
        char c;
        int cntr = 0;
        while((c=getchar())!='.')
        {
            buffer[cntr++]=c;
           
        }
        buffer[cntr++]=0;
        if(buffer[0] == '#')
        {
            isExit = true;
        }
        send(client,buffer, bufsize,0);
        send(client,".",2,0);
        
        printf("Server: ");
        do {
            recv(client, buffer, bufsize, 0);
            printf("%s ",buffer);
            if (buffer[0] == '#') 
            {
                isExit = true;
            }
        } while (buffer[0] != '.' &&  buffer[0] != '#');
        printf("\n");

    } while (!isExit);

    /* ---------------- ЗАВЕРШЕНИЕ СОЕДИНЕНИЯ ------------- */
    /* ----------------- close() --------------- */

    /* 
        Как только сервер нажмет # для завершения соединения,
        цикл завершится и закроет серверное сокет соединение
        и клиентсткое соединение.
    */

    printf("\n=> Connection terminated.\n\nGoodbye...\n");

    close(client);
    return 0;
}

