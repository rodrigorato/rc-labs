#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <sstream>
#include <string.h>
#include <vector>

#ifndef PORT	
#define PORT 58014 //default connection port
#endif

#ifndef MAX_SIZE
#define MAX_SIZE 1024 //max message size
#endif

//macros for commands and errors to facilitate code reading
#ifndef COMMANDS
#define LIST_REQUEST "ULQ"	

#define LIST_RESPONSE "ULR "

#define NO_SERVERS_ERROR "ULR EOF\n"

#define CONNECT_REQUEST "UNQ"

#define CONNECT_RETURN "UNR "

#define CONNECT_RETURN_UNKNOWN "UNR EOF\n"

#define CONNECT_RETURN_ERROR "UNR ERR\n"

#define NEW_SERVER "SRG"

#define NEW_SERVER_OK "SRR OK\n"

#define NEW_SERVER_NOK "SRR NOK\n"

#define NEW_SERVER_ERR "SRR ERR\n"

#define CLOSE_SERVER "SUN"

#define CLOSE_SERVER_OK "SUN OK\n"

#define CLOSE_SERVER_NOK "SUN NOK\n"

#define CLOSE_SERVER_ERR "SUN ERR\n"

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

int duplicateLanguage(vector<Server> servers, char* language);

int main(int argc, char const *argv[])
{	
	int fd; // file discriptor for socket
	socklen_t addrlen;
	struct sockaddr_in serveraddr, clientaddr;
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

	while(1) // main connection loop
	{
		char buffer[MAX_SIZE] = "";		// buffer for command input

		if (recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr*) &clientaddr, &addrlen) == -1) // get message from clients
			exit(-1);


		sscanf(buffer, "%s",command); // determine the received command


		if (!strcmp(command,LIST_REQUEST))		// list command processing
			{
				cout << "List request received." << endl;
				if (servers.size() == 0) // There are no servers available
				{
					if(sendto(fd, NO_SERVERS_ERROR, strlen(NO_SERVERS_ERROR), 0, (struct sockaddr*) &clientaddr, addrlen) == -1) // send no available servers error
					exit(-1);
				}
				else
				{
					string data = LIST_RESPONSE;
					stringstream stream;

					// add current number of servers to message
					stream << servers.size();
					data += stream.str() + ' '; 

					// add all languages names in server vector to message
					for (unsigned i = 0; i < servers.size(); ++i)
					{
						data.append(servers[i].language);
						data += ' ';
					}

					data += '\n'; // add new line character to end of message

					if(sendto(fd, data.c_str(), strlen(data.c_str()), 0, (struct sockaddr*) &clientaddr, addrlen) == -1) // send no available servers error
						exit(-1);

					cout << "List request sent." << endl;

				}
				
			}
		else if (!strcmp(command,CONNECT_REQUEST)) // translator connection request
			{
				char language_buffer[20] ;
				string data = CONNECT_RETURN;
				stringstream stream;

				cout << "Connect request received." << endl;
				
				sscanf(buffer,"%s %s\n", command, language_buffer);

				if (language_buffer[0] == '\0')
				{
					if(sendto(fd, CONNECT_RETURN_ERROR, strlen(CONNECT_RETURN_ERROR), 0, (struct sockaddr*) &clientaddr, addrlen) == -1) 
						exit(-1);
					cout << "Connect request failed. No language specified." << endl;
				}

				else if (duplicateLanguage(servers,language_buffer))
				{
					for (unsigned i = 0; i < servers.size(); ++i)
					{
						if (!strcmp(servers[i].language,language_buffer))
						{
							data += servers[i].language;
							data += ' ';
							data += servers[i].ip_addr;
							data += ' ';
							stream << servers[i].port;
							data += stream.str();
							data += ' ';
							data += '\n';

							if(sendto(fd, data.c_str(), strlen(data.c_str()), 0, (struct sockaddr*) &clientaddr, addrlen) == -1) // send no available servers error
								exit(-1);

							cout << "Connection data sent to client." << endl;

						}
					}
				}
				else
				{
						if(sendto(fd, CONNECT_RETURN_UNKNOWN, strlen(CONNECT_RETURN_UNKNOWN), 0, (struct sockaddr*) &clientaddr, addrlen) == -1) 
								exit(-1);
				}
			}
		
		else if (!strcmp(command,NEW_SERVER)) // add new avalable translation server
			{
				char language_buffer[20];
				char ip_buffer[30];
				int port_buffer = -1;
				sscanf(buffer,"%s %s %s %d\n", command, language_buffer, ip_buffer, &port_buffer); //get info from new server

				if (language_buffer[0] == '\0' || ip_buffer[0] == '\0' || port_buffer == -1)
				{
					if(sendto(fd, NEW_SERVER_ERR, strlen(NEW_SERVER_ERR), 0, (struct sockaddr*) &clientaddr, addrlen) == -1) //send confirmation message to translation server
						exit(-1);
					cout << "Failed to add TRS server. Protocol error." << endl;

				}

				else if (!duplicateLanguage(servers,language_buffer))
				{
					Server s(language_buffer,ip_buffer, port_buffer); // create new server and add to server vector
					servers.push_back(s);
					cout << "Added: " <<  servers[servers.size()-1].language << ' ' << servers[servers.size()-1].ip_addr << ' ' << servers[servers.size()-1].port << endl;
					if(sendto(fd, NEW_SERVER_OK, strlen(NEW_SERVER_OK), 0, (struct sockaddr*) &clientaddr, addrlen) == -1) //send confirmation message to translation server
						exit(-1); 
				}
				else
				{
					if(sendto(fd, NEW_SERVER_NOK, strlen(NEW_SERVER_NOK), 0, (struct sockaddr*) &clientaddr, addrlen) == -1) //send confirmation message to translation server
						exit(-1);
					cout << "Failed to add TRS server. Duplicate language." << endl;

				}
			}
		
		else if (!strcmp(command,CLOSE_SERVER)) // remove server from available list
			{
				char language_buffer[20];
				char ip_buffer[30];
				int port_buffer = -1;
				sscanf(buffer,"%s %s %s %d\n", command, language_buffer, ip_buffer, &port_buffer); // get info from server

				if (language_buffer[0] == '\0' || ip_buffer[0] == '\0' || port_buffer == -1)
				{
					if(sendto(fd, CLOSE_SERVER_ERR, strlen(CLOSE_SERVER_ERR), 0, (struct sockaddr*) &clientaddr, addrlen) == -1) //send confirmation message to translation server
						exit(-1);
					cout << "Failed to remove TRS server. Protocol error." << endl;
				}

				else if (duplicateLanguage(servers,language_buffer))
				{
					for (unsigned i = 0; i < servers.size(); ++i)
						{
							if (!strcmp(servers[i].language,language_buffer) && servers[i].port == port_buffer && !strcmp(servers[i].ip_addr,ip_buffer)) // find server that matche sthe one to remove
								{
									cout << "Removed: " << servers[i].language << ' ' << servers[i].ip_addr << ' ' << servers[i].port << endl;
									servers.erase(servers.begin()+i);
									if(sendto(fd, CLOSE_SERVER_OK, strlen(CLOSE_SERVER_OK), 0, (struct sockaddr*) &clientaddr, addrlen) == -1) //send confirmation message to removed server
										exit(-1);
								}
							else if (!strcmp(servers[i].language,language_buffer))
							{
								if(sendto(fd, CLOSE_SERVER_NOK, strlen(CLOSE_SERVER_NOK), 0, (struct sockaddr*) &clientaddr, addrlen) == -1) //send confirmation message to removed server
									exit(-1);
								cout << "Failed to remove TRS server. IP and/or port number do not match." << endl;
							}
						}
				}

				else
				{
					if(sendto(fd, CLOSE_SERVER_NOK, strlen(CLOSE_SERVER_NOK), 0, (struct sockaddr*) &clientaddr, addrlen) == -1) //send confirmation message to removed server
						exit(-1);
					cout << "Failed to remove TRS server. Language does not exist." << endl;

				}
			}
	}
	return 0;
}


int duplicateLanguage(vector<Server> servers, char* language)
{
	for (unsigned i = 0; i < servers.size() ; ++i)
	{
		if (!strcmp(servers[i].language,language))
		return 1;
	}
	return 0;
}