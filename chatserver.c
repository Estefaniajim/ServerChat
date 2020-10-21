#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <sstream>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void main()
{
	// Initialze winsock
	// Creamos el winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);
	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		cerr << "Can't Initialize winsock! Quitting" << endl;
		return;
	}
	// Create a socket
	// Creamos un nuevo socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cerr << "No podemos agregar mas usuarios" << endl;
		return;
	}
	// Bind the ip address and port to a socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(listening, (sockaddr*)&hint, sizeof(hint));
	listen(listening, SOMAXCONN);
	// Master file descriptor set
	fd_set master;
	FD_ZERO(&master);
	// First socket
	// Primer socket
	FD_SET(listening, &master);
	bool running = true; 
	while (running)
	{
		//Copy of the master list
		//Copia de la lsita de master
		fd_set copy = master;

		// Check the user that is writing in the server
		//	Checamos que usuario esta escribiendo en el server
		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		// Loop through all the current users
		// Iteramos por todos los usuarios que tenemos activos
		for (int i = 0; i < socketCount; i++)
		{
			SOCKET sock = copy.fd_array[i];
			if (sock == listening)
			{
				// We accept a new user
				SOCKET user = accept(listening, nullptr, nullptr);
				//We addd the user to the list of users
				FD_SET(user, &master);
				// We display a message to the new user
				string bienvenida = "Bienvenido al chat!\r\n";
				send(client, bienvenida.c_str(), bienvenida.size() + 1, 0);
			}
			else
			{
				char buf[4096];
				ZeroMemory(buf, 4096);
				
				// Server Receive message
				//Servidor recibe un nuevo mensaje
				int bytesIn = recv(sock, buf, 4096, 0);
				if (bytesIn <= 0)
				{
					// Drop the client
					// Le decimos bye al usuario
					string despedida = "Bye\r\n";
					send(client, despedida.c_str(), despedida.size() + 1, 0);
					closesocket(sock);
					FD_CLR(sock, &master);
				}
				else
				{
					// Check to see if it's a command.
					// Checamos que el mensaje no es el comando de cerrar
					if (buf[0] == '\\')
					{
						// If it is command quit
						// Checamos si es el comando para cerrar
						string cmd = string(buf, bytesIn);
						if (cmd == "\\quit")
						{
							running = false;
							break;
						}
						// Unknown command
						// Si es un comando random lo ignoramos
						continue;
					}

					// Send message to other clients
					// Mandamos los mensajes a los otros clientes
					for (int i = 0; i < master.fd_count; i++)
					{
						SOCKET outSock = master.fd_array[i];
						if (outSock != listening && outSock != sock)
						{
							ostringstream ss;
							ss << "SOCKET #" << sock << ": " << buf << "\r\n";
							string strOut = ss.str();

							send(outSock, strOut.c_str(), strOut.size() + 1, 0);
						}
					}
				}
			}
		}
	}

	// Remove the listening socket from the master file descriptor set and close it
	//Removemos el threat
	FD_CLR(listening, &master);
	closesocket(listening);
	
	// Message to let users that the server is shutting down
	// Avisamos a los usuarios que el server se esta apagando
	string msg = "Bye\r\n";

	while (master.fd_count > 0)
	{
		// Get the socket number
		// Agarramos el threat que fue se va a cerrar
		SOCKET sock = master.fd_array[0];
		// We send the message
		// Mandamos el mensage a los users
		send(sock, msg.c_str(), msg.size() + 1, 0);

		// Remove it from the master file list and close the socket
		// Quitamos el threat de la lista de masters y lo cerramos
		FD_CLR(sock, &master);
		closesocket(sock);
	}

	// Cleanup winsock
	// Limpiamos el winsock
	WSACleanup();

	system("pause");
}