#include <stdio.h>
#include <sys/socket.h> //For Sockets
#include <stdlib.h>
#include <netinet/in.h> //For the AF_INET (Address Family)

struct sockaddr_in serv; //This is our main socket variable.
int fd; //This is the socket file descriptor that will be used to identify the socket
int conn; //This is the connection file descriptor that will be used to distinguish client connections.
char message[250] = ""; //This array will store the messages that are sent by the server

int main(int argc, char**argv){
	char *ip = argv[0];
    char *hostName = argv[1];
    char c;
	
	fd = socket(AF_INET, SOCK_STREAM, 0);
	serv.sin_family = AF_INET;
	serv.sin_port = htons(8096);
	inet_pton(AF_INET,ip, &serv.sin_addr); //This binds the client to localhost
	connect(fd, (struct sockaddr *)&serv, sizeof(serv)); //This connects the client to the server.
	while(1) {
    	fgets(message, 250, stdin);
    	send(fd, message, strlen(message), 0);
    	//An extra breaking condition can be added here (to terminate the while loop)
	}
	#prueba

    return 0;

}

