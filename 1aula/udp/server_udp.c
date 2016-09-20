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
	int fd, addrlen;
	struct hostent* hostptr;
	struct sockaddr_in serveraddr, clientaddr;
	char buffer[128];	
	
	if((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) exit(1);
	
	memset((void*)&serveraddr, (int)'\0', sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((u_short)PORT);
	
	
	if(bind(fd, (struct sockaddr*) &serveraddr, sizeof(serveraddr)) == -1) exit(1);
	
	addrlen = sizeof(clientaddr);
	
	printf("Server started, waiting on message...\n");

	if(recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr*) &clientaddr, &addrlen) == -1) exit(1);
	
	printf("Received message:\n%s\n", buffer); 

	if(sendto(fd, buffer, strlen(buffer) + 1, 0, (struct sockaddr*) &clientaddr, addrlen) == -1) exit(1);
	
	printf("Sent message:\n%s\n", buffer);	

	close(fd);

	return 0;
}
