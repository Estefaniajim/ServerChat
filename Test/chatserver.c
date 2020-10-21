#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>

#define MAX_CLIENTS 5

typedef struct {
	struct sockaddr_in address;
	int sockfd;
	int id;
} client_t;

char message[100] = "";

void *handle_client(void *arg) {
	printf("Cliente se ha conectado!!\n");
	client_t *client = (client_t*)arg;
	while(recv(client->sockfd, message, 100, 0) > 0) {
		if(strcmp(message, "bye") == 0) {
			break;
		} else {
			printf("Mensaje recibido: %s", message);
		}
		bzero(message, 100);
	}
	printf("Se acabó\n");
	close(client->sockfd);
	free(client);
	pthread_detach(pthread_self());
}

// Comando de compilaciรณn: gcc chatserver.c -o chatserver -pthread
int main(int argc, char **argv) {
	if(argc != 2) {
		printf("Uso: %s <port>\n", argv[0]);
		return EXIT_FAILURE;
	}
	
	int port = atoi(argv[1]);
	
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int listenerfd = 0, connectionfd = 0;
	pthread_t thread;
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	listenerfd = socket(AF_INET, SOCK_STREAM, 0);
	
	bind(listenerfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	
	listen(listenerfd, MAX_CLIENTS);
	
	printf("Interacciones del servidor:\n");
	
	while(1) {
		socklen_t client_len = sizeof(client_addr);
		connectionfd = accept(listenerfd, (struct sockaddr*)&client_addr, &client_len);
		
		client_t *client = (client_t*)malloc(sizeof(client_t));
		client->address = client_addr;
		client->sockfd = connectionfd;
		
		pthread_create(&thread, NULL, &handle_client, (void*)client);
	}
}