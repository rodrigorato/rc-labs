#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#define PORT 59000

int main(){	
	int fd, clientlen, newfd;
	struct hostent* hostptr;
	struct sockaddr_in serveraddr, clientaddr;
	char buffer[128];	
	
	if((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) exit(1);
	
	memset((void*)&serveraddr, (int)'\0', sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((u_short)PORT);
	
	
	if(bind(fd, (struct sockaddr*) &serveraddr, sizeof(serveraddr)) == -1) exit(1);
	printf("Bound successfully\n");

	if(listen(fd, 5) == -1) exit(1);
	printf("Listened successfully\n");
	
	clientlen = sizeof(clientaddr);

	printf("Server started, waiting on handshake...\n");	
	
	if((newfd = accept(fd, (struct sockaddr*) &clientaddr, &clientlen)) == -1) exit(1);
	

	

	if(read(newfd, buffer, 128) == -1) exit(1);
	printf("Received message:\n%s\n", buffer); 

	if(write(newfd, buffer, strlen(buffer)) == -1) exit(1);
	printf("Sent message:\n%s\n", buffer);	

	close(fd);
	close(newfd);

	return 0;
}
