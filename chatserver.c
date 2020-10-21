#include <stdio.h>
#include <sys/socket.h> //For Sockets
#include <stdlib.h>
#include <netinet/in.h> //For the AF_INET (Address Family)

struct sockaddr_in serv; //This is our main socket variable.
int fd; //This is the socket file descriptor that will be used to identify the socket
int conn; //This is the connection file descriptor that will be used to distinguish client connections.
char message[250]; //This array will store the messages that are sent by the server

int main(int argc, char**argv){
	char *ip = argv[0];
	char *port = argv[1];
	char *name = argv[2];
	
    struct sockaddr_in serverAddress;
 
    serv.sin_family = AF_INET;
	serv.sin_port = htons(8096); 
	serv.sin_addr.s_addr = INADDR_ANY;
	fd = socket(AF_INET, SOCK_STREAM, 0); 
	bind(fd, (struct sockaddr *)&serv, sizeof(serv)); 
	listen(fd,5); 

	while(conn = accept(fd, (struct sockaddr *)NULL, NULL)) {
    	int pid;
    	if((pid = fork()) == 0) {
        	while (recv(conn, message, 250, 0)>0) {
            printf(name,":", message);           
            //message = null;
			}
        }
        exit(0);
    }
}