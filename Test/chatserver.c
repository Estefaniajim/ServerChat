#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>

#define MAX_CLIENTS 5
#define MESSAGE_LEN 100
#define NAME_LEN 50

typedef struct {
	struct sockaddr_in address;
	int sockfd;
	int id;
	char name[NAME_LEN];
} client_t;

client_t *clients[MAX_CLIENTS];
char message[MESSAGE_LEN] = "";

void add_client(client_t* client) {	
	for(int i = 0; i < MAX_CLIENTS; i++) {
		if(!clients[i]) {
			clients[i] = client;
			break;
		}
	}
}

void remove_client(client_t* client) {
	for(int i = 0; i < MAX_CLIENTS; i++) {
		if(clients[i]) {
			if(clients[i] == client) {
				printf("Usuario %s removido\n", client->name);
				clients[i] = NULL;
				break;
			}
		}
	}
}

void broadcast_message(char *s, client_t* client) {	
	for(int i = 0; i < MAX_CLIENTS; i++) {
		if(clients[i]) {
			printf("TEST: %s\n", clients[i]->name);
			if(clients[i] != client) {
				printf("Sending message to %s\n", clients[i]->name);
				write(clients[i]->sockfd, s, strlen(s));
			}
		}
	}
}

void *handle_client(void *arg) {
	int flag = 0;
	client_t *client = (client_t*)arg;
	
	if(read(client->sockfd, message, MESSAGE_LEN) > 0) {
		printf("%s se ha unido\n", message);
		strcpy(client->name, message);
	} else {
		printf("Error con el nickname!\n");
		flag = 1;
	};
	bzero(message, MESSAGE_LEN);
	
	while(1) {
		if(flag) {
			break;
		}
		int receive = recv(client->sockfd, message, MESSAGE_LEN, 0);
		if(receive > 0) {
			printf("%s -> %s", client->name, message);
			broadcast_message(message, client);
		} else if(strcmp(message, "bye") == 0) {
			printf("%s -> %s", client->name, message);
			broadcast_message(message, client);
			flag = 1;
		} else {
			flag = 1;
		}
		bzero(message, MESSAGE_LEN);
	}
	printf("%s se ha ido\n", client->name);
	close(client->sockfd);
	remove_client(client);
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
		
		add_client(client);
		pthread_create(&thread, NULL, &handle_client, (void*)client);
	}
}