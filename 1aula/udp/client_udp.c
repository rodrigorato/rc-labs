#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#define PORT 59000
#define MESSAGE "Segmentation fault"

int main(){	
	int fd, addrlen;
	struct hostent* hostptr;
	struct sockaddr_in serveraddr, clientaddr;
	char buffer[128];	
	
	if((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) exit(1);

	hostptr = gethostbyname("minho.tecnico.ulisboa.pt");
	
	memset((void*)&serveraddr, (int)'\0', sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = ((struct in_addr*) (hostptr->h_addr_list[0]))->s_addr;
	serveraddr.sin_port = htons((u_short)PORT);
	
	addrlen = sizeof(serveraddr);

	if(sendto(fd, MESSAGE, strlen(MESSAGE) + 1, 0, (struct sockaddr*) &serveraddr, addrlen) == -1) exit(1);
	
	printf("Sent message:\n%s\n", MESSAGE);
	
	if(recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr*) &serveraddr, &addrlen) == -1) exit(1);
	
	printf("Received message:\n%s\n", buffer); 

	
		

	close(fd);

	return 0;
}
