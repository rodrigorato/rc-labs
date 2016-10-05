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
	struct sockaddr_in serveraddr, clientaddr;
	
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
	

	
	/* Trying to receive a file */
	FILE *writef;
	writef = fopen("server_got_this.png", "w+");
	char buffer_write[1510];

	if(read(newfd, buffer_write, 1510) == -1) exit(1);
	printf("Received a file?\n"); 

	fwrite(buffer_write, 1510, 1, writef);
	fclose(writef);


	/* Will now try to send a file */
	FILE *readf;
	readf = fopen("server_will_send_this.jpg", "r");
	char buffer_read[3218];

	fread(buffer_read, 3218, 1, readf);
	printf("%s\n", buffer_read);
	fclose(readf);

	if(write(newfd, buffer_read, 3218) == -1) exit(1);
	printf("Sent a file?\n");	

	close(fd);
	close(newfd);

	return 0;
}
