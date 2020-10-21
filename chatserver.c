#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>
#include <pthread.h>

#define BUFFER_SIZE 2048
#define MAX_CLIENTS 5
#define NAME_LEN 32

static int client_count = 0;
static int id = 0;

typedef struct {
	struct sockaddr_in address;
	int sockfd;
	int id;
	char name[NAME_LEN];
} client_t;

client_t *clients[MAX_CLIENTS];

pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;

void overwrite_stdout() {
	printf("\r%s", "> ");
	fflush(stdout);
}

void trim(char* arr, int length) {
	for(int i = 0; i < length; i++) {
		if(arr[i] == '\n') {
			arr[i] = '\0';
			break;
		}
	}
}

void queue_client(client_t* client) {
	pthread_mutex_lock(&client_mutex);
	
	for(int i = 0; i < MAX_CLIENTS; i++) {
		if(!clients[i]) {
			clients[i] = client;
			break;
		}
	}
	
	pthread_mutex_unlock(&client_mutex);
}

void dequeue_client(int id) {
	pthread_mutex_lock(&client_mutex);
	
	for(int i = 0; i < MAX_CLIENTS; i++) {
		if(clients[i]) {
			if(clients[i]->id == id) {
				clients[i] = NULL;
				break;
			}
		}
	}
	
	pthread_mutex_unlock(&client_mutex);
}

void send_message(char *s, int id) {
	pthread_mutex_lock(&client_mutex);
	
	for(int i = 0; i < MAX_CLIENTS; i++) {
		if(clients[i]) {
			if(clients[i]->id != id) {
				write(clients[i]->sockfd, s, strlen(s));
			}
		}
	}
	
	pthread_mutex_unlock(&client_mutex);
}

void *handle_client(void *arg) {
	char buffer[BUFFER_SIZE];
	char name[NAME_LEN];
	int leave_flag = 0;
	client_count++;
	
	client_t *client = (client_t*)arg;
	if(recv(client->sockfd, name, NAME_LEN, 0) <= 0 || strlen(name) < 2 || strlen(name) >= NAME_LEN-1) {
		printf("Nombre no válido!\n");
		leave_flag = 1;
	} else {
		strcpy(client->name, name);
		sprintf(buffer, "%s se ha unido", client->name);
		printf("%s", buffer);
		send_message(buffer, client->id);
	}
	
	bzero(buffer, BUFFER_SIZE);
	
	while(1) {
		if(leave_flag) {
			break;
		}
		
		int receive = recv(client->sockfd, buffer, BUFFER_SIZE, 0);
		if(receive > 0) {
			if(strlen(buffer) > 0) {
				send_message(buffer, client->id);
				trim(buffer, strlen(buffer));
				printf("%s -> %s\n", buffer, client->name);
			}
		} else if(receive == 0 || strcmp(buffer, "bye") == 0) {
			sprintf(buffer, "%s has left\n", client->name);
			printf("%s\n", buffer);
			send_message(buffer, client->id);
			leave_flag = 1;
		} else {
			printf("ERROR: -1\n");
			leave_flag = 1;
		}
		bzero(buffer, BUFFER_SIZE);
	}
	close(client->sockfd);
	dequeue_client(client->id);
	free(client);
	pthread_detach(pthread_self());
	
	return NULL;
}	

// Comando de compilación: gcc chatserver.c -o chatserver -pthread
int main(int argc, char **argv) {
	if(argc != 2) {
		printf("Uso: %s <port>\n", argv[0]);
		return EXIT_FAILURE;
	}
	
	int port = atoi(argv[1]);
	
	int option = 1;
	int listenfd = 0, connectionfd = 0;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	pthread_t thread;
	
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(port);
	
	signal(SIGPIPE, SIG_IGN);
	
	bind(listenfd,(const struct sockaddr *)&server_addr, sizeof(server_addr));
	listen(listenfd, MAX_CLIENTS);
	
	printf("Interacciones de la sala:\n");
	
	while(1) {
		connectionfd = accept(listenfd, (struct sockaddr*)&client_addr, (unsigned int *)sizeof(client_addr));
		
		if((client_count+1) == MAX_CLIENTS) {
			printf("Número máximo de clientes excedido! Rechazando conexión");
			close(connectionfd);
			continue;
		}
		
		client_t *client = (client_t*)malloc(sizeof(client_t));
		client->address = client_addr;
		client->sockfd = connectionfd;
		client->id = id++;
		
		queue_client(client);
		pthread_create(&thread, NULL, &handle_client, (void*)client);
		
		sleep(1);
	}
	
 	return EXIT_SUCCESS;
}
