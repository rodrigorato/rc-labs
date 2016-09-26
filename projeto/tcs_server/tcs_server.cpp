#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <string.h>

#ifndef PORT	
#define PORT 58014 //default connection port
#endif

#ifndef MAX_SIZE
#define MAX_SIZE 1024 //max message size
#endif

using namespace std;

int main(int argc, char const *argv[])
{	
	int fd; // file discriptor for socket
	socklen_t addrlen;
	struct sockaddr_in serveraddr, clientaddr;
	char buffer[MAX_SIZE];		// buffer for command input
	char command[MAX_SIZE];

	if ((fd = socket(AF_INET, SOCK_DGRAM,0)) ==-1)
		exit(-1);

	memset((void*)&serveraddr, (int)'\0', sizeof(serveraddr));
	serveraddr.sin_family= AF_INET;
	serveraddr.sin_addr.s_addr= htonl(INADDR_ANY);
	if(argc > 1)	//check if a custom port has been given. if not set default port
	{
		serveraddr.sin_port = htons((u_short) atoi(argv[2]) );
		cout << argv[2] << endl;
	}
	else
	{
		serveraddr.sin_port = htons((u_short)PORT);
		cout << PORT << endl;
	}


	if (bind(fd,(struct sockaddr*)&serveraddr,sizeof(serveraddr)) == -1) // bind socket for connection
		exit(-1);

	addrlen = sizeof(clientaddr);

	while(1)
	{
		if (recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr*) &clientaddr, &addrlen) == -1) // get message from clients
		exit(-1);

		sscanf(buffer, "%s ",command);
		cout << command << endl;
	}


	return 0;
}	