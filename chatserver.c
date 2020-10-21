<<<<<<< HEAD
#include <stdio.h>
#include <sys/socket.h> 
#include <stdlib.h>
#include <netinet/in.h> 

struct sockaddr_in serv; 
int fd; 
int conn; 
char message[250]; 

int main(int argc, char**argv){
	serv.sin_family = AF_INET;
	serv.sin_port = htons(8096); 
	serv.sin_addr.s_addr = INADDR_ANY;
	fd = socket(AF_INET, SOCK_STREAM, 0); 
	bind(fd, (struct sockaddr *)&serv, sizeof(serv)); 
	listen(fd,5); 

	while(conn = accept(fd, (struct sockaddr *)NULL, NULL)) {
    	int pid;
    	if((pid = fork()) == 0) {
        	while (recv(conn, message, 100, 0)>0) {
            	printf("Message Received: %s\n", message);
            	//An extra breaking condition can be added here (to terminate the child process)            
            	//message = "";
        	}
        	exit(0);
    	}
	}
}
=======
#include <iostream>
#include <WS2tcpip.h>
#pragma comment (lib, "w2_32.lib")
using namespace std;
void main()
{
	// Initialize winsock
	// Create socket
	// Bind the socket to an ip adress and port
	// Tell windsock the socket is for listening
	// Wait for a connection
	// Close listening socket
	// While loop: accept and echo message back to client
	// Close the sock
	// ShutDown winsock
	}
>>>>>>> 9a0b67b0bafe7c7580ff93d0122e41c76e736992
