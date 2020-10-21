#include <stdio.h>
#include <sys/socket.h> 
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>


// Comando para compilar: gcc chatclient.c -o chatclient	
int main(int argc, char **argv) {
	if(argc != 4) {
		printf("Uso: %s <serverIP> <port> <nickName>\n", argv[0]);
		return EXIT_FAILURE;
	}
	
	int port = atoi(argv[2]);
	
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
	
	char message[100] = "";
	while(1) {
		printf("> ");
		fgets(message, 100, stdin);
		send(connectionfd, message, strlen(message), 0);
	}
}