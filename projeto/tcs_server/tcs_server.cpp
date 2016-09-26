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

//macros for commands and errors to facilitate code reading
#ifndef LIST_REQUEST
#define LIST_REQUEST "ULQ"	
#endif

#ifndef LIST_RESPONSE
#define LIST_RESPONSE "ULR"
#endif

#ifndef CONNECT_REQUEST
#define CONNECT_REQUEST "UNQ"
#endif

#ifndef NEW_SERVER
#define NEW_SERVER "SRG"
#endif

#ifndef NEW_SERVER_OK
#define NEW_SERVER_OK "SRR OK"
#endif

#ifndef NEW_SERVER_NOK
#define NEW_SERVER_NOK "SRR NOK"
#endif

#ifndef CLOSE_SERVER
#define CLOSE_SERVER "SUN"
#endif

#ifndef CLOSE_SERVER_OK
#define CLOSE_SERVER_OK "SUN OK"
#endif

#ifndef CLOSE_SERVER_NOK
#define CLOSE_SERVER_NOK "SUN NOK"
#endif

#ifndef NO_SERVERS_ERROR
#define NO_SERVERS_ERROR "ULR EOF"
#endif


struct Server // struct to hold data about currently active servers
{	
	char language[20];
	int port;
	char ip_addr[30];
	Server(char* language, char* ip,int p)
	{
		strncpy(this->language, language,sizeof(this->language));
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
	vector<Server> servers;		// vector to hold server data

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

	addrlen = sizeof(clientaddr); // get size of client address

	while(1)
	{
		if (recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr*) &clientaddr, &addrlen) == -1) // get message from clients
		exit(-1);


		sscanf(buffer, "%s",command); // determine the received command

		if (!strcmp(command,LIST_REQUEST))		// list command processing
			{
				if (servers.size() == 0) // There are no servers available
				{
					if(sendto(fd, NO_SERVERS_ERROR, sizeof(NO_SERVERS_ERROR), 0, (struct sockaddr*) &clientaddr, addrlen) == -1) // send no available servers error
					exit(1);
				}
				else
				{
					if(sendto(fd, LIST_RESPONSE, sizeof(LIST_RESPONSE), 0, (struct sockaddr*) &clientaddr, addrlen) == -1) //send initial response
					exit(1);

					if(sendto(fd, (const char*) servers.size(), sizeof(servers.size()), 0, (struct sockaddr*) &clientaddr, addrlen) == -1) // send number of avaulabel servers
					exit(1);

					for (unsigned i = 0; i < servers.size(); ++i) //send the language of all available servers
						{
							if(sendto(fd, servers[i].language, sizeof(servers[i].language), 0, (struct sockaddr*) &clientaddr, addrlen) == -1) 
								exit(1);
						}

				}
				
			}
		else if (!strcmp(command,CONNECT_REQUEST)) // translator connection request
			cout << "Connection request recieved" << endl;
		
		else if (!strcmp(command,NEW_SERVER)) // add new avalable translation server
			{
				char language_buffer[20];
				char ip_buffer[30];
				int port_buffer;
				sscanf(buffer,"%s %s %s %d\n", command, language_buffer, ip_buffer, &port_buffer); //get info from new server
				Server s(language_buffer,ip_buffer, port_buffer); // create new server and add to server vector
				servers.push_back(s);
				cout << "Added: " <<  servers[servers.size()-1].language << ' ' << servers[servers.size()-1].ip_addr << ' ' << servers[servers.size()-1].port 
				<< ' ' << servers.size() << endl;
				if(sendto(fd, NEW_SERVER_OK, sizeof(NEW_SERVER_OK), 0, (struct sockaddr*) &clientaddr, addrlen) == -1) //send confirmation message to translation server
					exit(1); 
			}
		
		else if (!strcmp(command,CLOSE_SERVER)) // remove server from available list
			{
				char language_buffer[20];
				char ip_buffer[30];
				int port_buffer;
				sscanf(buffer,"%s %s %s %d\n", command, language_buffer, ip_buffer, &port_buffer); // get info from server

				for (unsigned i = 0; i < servers.size(); ++i)
				{
					if (!strcmp(servers[i].language,language_buffer) && servers[i].port == port_buffer && !strcmp(servers[i].ip_addr,ip_buffer)) // find server that matche sthe one to remove
					{
						cout << "Removed: " << servers[i].language << ' ' << servers[i].ip_addr << ' ' << servers[i].port << endl;
						servers.erase(servers.begin()+i);
					}
				}
				if(sendto(fd, CLOSE_SERVER_OK, sizeof(CLOSE_SERVER_OK), 0, (struct sockaddr*) &clientaddr, addrlen) == -1) //send confirmation message to removed server
					exit(1);
			}
	}
	return 0;
}	