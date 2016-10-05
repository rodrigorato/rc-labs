#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#include <iostream>

#define PORT 59002

using namespace std;

int main(){	
	int fd, newfd;
	socklen_t clientlen;
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
	int fileToRecv_size = 60938;

	FILE *writef;
	writef = fopen("server_got_this.jpg", "w+");
	char buffer_write[fileToRecv_size];
	int readBytes = 0, j = 0;

	cout << "Will now try to receive a file!" << endl;
	cout << readBytes << "/" << fileToRecv_size << endl;
	while(readBytes < fileToRecv_size){
		if((j = read(newfd, buffer_write + readBytes, fileToRecv_size - readBytes)) == -1) exit(1);
		readBytes += j;
		cout << readBytes << "/" << fileToRecv_size << endl;
	}
	cout << readBytes << "/" << fileToRecv_size << endl;
	printf("Received a file?\n");


	fwrite(buffer_write, fileToRecv_size, 1, writef);
	fclose(writef);


	/* Will now try to send a file */
	int fileToSend_size = 52932;

	FILE *readf;
	readf = fopen("server_will_send_this.jpg", "r");
	char buffer_read[fileToSend_size];

	fread(buffer_read, fileToSend_size, 1, readf);
	printf("%s\n", buffer_read);
	fclose(readf);
	int writtenBytes = 0, i = 0;

	cout << writtenBytes << "/" << fileToSend_size << endl;
	while(writtenBytes < fileToSend_size){
		if((i = write(newfd, buffer_read + writtenBytes, fileToSend_size - writtenBytes)) == -1) exit(1);
		writtenBytes += i;
		cout << writtenBytes << "/" << fileToSend_size << endl;
	}
	cout << writtenBytes << "/" << fileToSend_size << endl;
	printf("Sent a file?\n");	

	close(fd);
	close(newfd);

	return 0;
}
