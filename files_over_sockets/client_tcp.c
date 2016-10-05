#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#define PORT 59000
#define MESSAGE "hello"

int main(){	
	int fd, clientlen, newfd;
	struct hostent* hostptr;
	struct sockaddr_in serveraddr, clientaddr;
	
	if((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) exit(1);
	
	hostptr = gethostbyname("localhost");
		
	memset((void*)&serveraddr, (int)'\0', sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = ((struct in_addr*) (hostptr->h_addr_list[0]))->s_addr;
	serveraddr.sin_port = htons((u_short)PORT);
	
	
	if(connect(fd, (struct sockaddr*) &serveraddr, sizeof(serveraddr)) == -1) exit(1);
	printf("Connected successfully\n");

	/* Will now try to write a file */
	FILE *readf;
	readf = fopen("read.png", "r");
	char buff_read[1510];
	fread(buff_read, 1510, 1, readf);
	fclose(readf);

	if(write(fd, buff_read, 1510) == -1) exit(1);
	printf("sent a file?\n");




	/* Will now try to read a file */
	FILE *writef;
	writef = fopen("client_got_this_back.jpg", "w+");
	char buff_write[3218];

	if(read(fd, buff_write, 3218) == -1) exit(1);
	printf("received a file?\n"); 

	fwrite(buff_write, 3218, 1, writef);
	fclose(writef);
	
	close(fd);

	return 0;
}
