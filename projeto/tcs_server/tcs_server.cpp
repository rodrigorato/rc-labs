#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <string.h>
#include <vector>

#ifndef PORT	
#define PORT 58014 //default connection port
#endif

#ifndef MAX_SIZE
#define MAX_SIZE 1024 //max message size
#endif

#ifndef LIST_REQUEST
#define LIST_REQUEST "ULQ"
#endif

#ifndef CONNECT_REQUEST
#define CONNECT_REQUEST "UNQ"
#endif

#ifndef NEW_SERVER
#define NEW_SERVER "SRG"
#endif

#ifndef CLOSE_SERVER
#define CLOSE_SERVER "SUN"
#endif

struct Server
{	
	char language[20];
	int port;
	char ip_addr[30];
	Server(char* language, char* ip,int p)
	{
		strcpy(this->language, language);
		port = p;
		strncpy(this->ip_addr, ip ,sizeof(this->ip_addr));
	}
};

using namespace std;

int main(int argc, char const *argv[])
{	
	int fd; // file discriptor for socket
	socklen_t addrlen;
	struct sockaddr_in serveraddr, clientaddr;
	char buffer[MAX_SIZE];		// buffer for command input
	char command[MAX_SIZE];
	vector<Server> servers;

	if ((fd = socket(AF_INET, SOCK_DGRAM,0)) ==-1)
		exit(-1);

	memset((void*)&serveraddr, (int)'\0', sizeof(serveraddr));
	serveraddr.sin_family= AF_INET;
	serveraddr.sin_addr.s_addr= htonl(INADDR_ANY);


	if(argc > 1)	//check if a custom port has been given. if not set default port
		serveraddr.sin_port = htons((u_short) atoi(argv[2]) );
	else
		serveraddr.sin_port = htons((u_short)PORT);



	if (bind(fd,(struct sockaddr*)&serveraddr,sizeof(serveraddr)) == -1) // bind socket for connection
		exit(-1);

	addrlen = sizeof(clientaddr);

	while(1)
	{
		if (recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr*) &clientaddr, &addrlen) == -1) // get message from clients
		exit(-1);


		sscanf(buffer, "%s",command);

		if (!strcmp(command,LIST_REQUEST))
			cout << "List request recieved" << endl;
		else if (!strcmp(command,CONNECT_REQUEST))
			cout << "Connection request recieved" << endl;
		else if (!strcmp(command,NEW_SERVER))
			{
				char language_buffer[20];
				char ip_buffer[30];
				int port_buffer;
				cout << buffer << endl;
				sscanf(buffer,"%s %s %s %d\n", command, language_buffer, ip_buffer, &port_buffer);
				Server s(language_buffer,ip_buffer, port_buffer);
				servers.push_back(s);
				cout << servers[servers.size()-1].language << ' ' << servers[servers.size()-1].ip_addr << ' ' << servers[servers.size()-1].port << endl;
			}
		else if (!strcmp(command,CLOSE_SERVER))
			cout << "Close server request" << endl;
	}


	return 0;
}	