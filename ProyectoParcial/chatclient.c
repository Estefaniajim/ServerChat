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

int flag = 0;
int connectionfd = 0;

void receive_handler() {
	char message[MESSAGE_LEN] = "";
	while(1) {
		int receive = recv(connectionfd, message, MESSAGE_LEN, 0);
		printf("%s", message);
		if(strcmp(message, "Adiós desde el server\n") == 0) {
			flag = 1;
		} else if(receive > 0) {
			printf("> %s", message);
  			fflush(stdout);
		} else if(receive == 0) {
			break;
		}
		bzero(message, MESSAGE_LEN);
	}
}

void send_handler() {
	char message[MESSAGE_LEN] = "";
	while(1) {
		fgets(message, MESSAGE_LEN, stdin);
		send(connectionfd, message, strlen(message), 0);
		if(strcmp(message, "bye\n") == 0) {
			break;
		}
		bzero(message, MESSAGE_LEN);
	}
	flag = 1;
}

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
	
	connectionfd = socket(AF_INET, SOCK_STREAM, 0);
	if(connect(connectionfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		printf("Connection error!!\n");
		return EXIT_FAILURE;
	};
	
	printf("> Bienvenido al chat\n");
	
	send(connectionfd, name, strlen(name), 0);
	
	pthread_t send_thread;
	pthread_create(&send_thread, NULL, (void *)send_handler, NULL);
	pthread_t receive_thread;
	pthread_create(&receive_thread, NULL, (void *)receive_handler, NULL);
	
	while(1) {
		if(flag) {
			break;
		}
		
	}
	
	close(connectionfd);
	return EXIT_SUCCESS;
}