#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <signal.h>

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
				clients[i] = NULL;
				break;
			}
		}
	}
}

void broadcast_message(char *s, client_t* client) {	
	for(int i = 0; i < MAX_CLIENTS; i++) {
		if(clients[i]) {
			if(clients[i] != client) {
				write(clients[i]->sockfd, s, strlen(s));
			}
		}
	}
}

void *handle_client(void *arg) {
	int flag = 0;
	char full_message[MESSAGE_LEN+NAME_LEN];
	client_t *client = (client_t*)arg;
	
	if(read(client->sockfd, message, MESSAGE_LEN) > 0) {
		printf("%s se ha unido\n", message);
		strcpy(client->name, message);
		strcpy(full_message, client->name);
		strcat(full_message, " se ha unido\n");
		broadcast_message(full_message, client);
	} else {
		printf("Error con el nickname!\n");
		flag = 1;
	};
	
	bzero(message, MESSAGE_LEN);
	bzero(full_message, MESSAGE_LEN+NAME_LEN);
	
	while(1) {
		if(flag) {
			break;
		}
		
		int receive = recv(client->sockfd, message, MESSAGE_LEN, 0);
		if(strcmp(message, "bye\n") == 0) {
			printf("%s -> %s", client->name, message);
			strcpy(full_message, client->name);
			strcat(full_message, ": ");
			strcat(full_message, message);
			broadcast_message(full_message, client);
			flag = 1;
		} else if(receive > 0) {
			printf("%s -> %s", client->name, message);
			strcpy(full_message, client->name);
			strcat(full_message, ": ");
			strcat(full_message, message);
			broadcast_message(full_message, client);
		} else {
			flag = 1;
		}
		bzero(message, MESSAGE_LEN);
	}
	printf("%s se ha ido\n", client->name);
	strcpy(full_message, client->name);
	strcat(full_message, " se ha ido\n");
	broadcast_message(full_message, client);
	close(client->sockfd);
	remove_client(client);
	free(client);
	pthread_detach(pthread_self());
}

void signal_handler(int signum) {
	bzero(message, MESSAGE_LEN);
	strcpy(message, "Adiós desde el server");
	for(int i = 0; i < MAX_CLIENTS; i++) {
		if(clients[i]) {
			write(clients[i]->sockfd, message, strlen(message));
			close(clients[i]->sockfd);
			clients[i] = NULL;
		}
	}
	pid_t pid = getpid();
	kill(pid, 9);
}

// Comando de compilaciรณn: gcc chatserver.c -o chatserver -pthread
int main(int argc, char **argv) {
	
	signal(2, signal_handler);
	
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