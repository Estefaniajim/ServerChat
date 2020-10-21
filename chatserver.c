#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

// Comando de compilación gcc chatserver.c -o chatserver -pthread

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
	
	bind(listenfd,(const struct sockaddr *)&server_addr, sizeof(server_addr));
	listen(listenfd, 5);
	
	printf("Interacciones de la sala:\n");
	
	while(1) {
		connectionfd = accept(listenfd, (struct sockaddr*)&client_addr, sizeof(client_addr));
	}
	
 	return EXIT_SUCCESS;
}
