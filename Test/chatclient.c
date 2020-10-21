#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define MESSAGE_LEN 100
#define NAME_LEN 50

// Comando para compilar: gcc chatclient.c -o chatclient -pthread
int main(int argc, char **argv) {
	if(argc != 4) {
		printf("Uso: %s <serverIP> <port> <nickname>\n", argv[0]);
		return EXIT_FAILURE;
	}
	
	int port = atoi(argv[2]);
	char name[NAME_LEN] = "";
	strcpy(name, argv[3]);
	
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	//TODO: Usar la IP de argv[1]
	inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
	
	int connectionfd = socket(AF_INET, SOCK_STREAM, 0);
	if(connect(connectionfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		printf("Connection error!!\n");
		return EXIT_FAILURE;
	};
	
	printf("Bienvenido al chat\n");
	
	char message[MESSAGE_LEN] = "";
	send(connectionfd, name, strlen(name), 0);
	while(1) {
		//TODO: Dividir entrada y salida en dos hilos
		// if((read(connectionfd, message, MESSAGE_LEN) > 0)) {
		// 	printf("Incoming message: %s", message);
		// }
		// bzero(message, MESSAGE_LEN);
		printf("> ");
		fgets(message, MESSAGE_LEN, stdin);
		send(connectionfd, message, strlen(message), 0);
	}
}