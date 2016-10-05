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
#define MESSAGE "hello"

using namespace std;

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
	int fileToSend_size = 60938;

	FILE *readf;
	readf = fopen("client_will_send_this.jpg", "r");
	char buff_read[fileToSend_size];
	fread(buff_read, fileToSend_size, 1, readf);
	fclose(readf);
	int writtenBytes = 0, i = 0;

	cout << writtenBytes << "/" << fileToSend_size << endl;
	while(writtenBytes < fileToSend_size){
		if((i = write(fd, buff_read + writtenBytes, fileToSend_size - writtenBytes)) == -1) exit(1);
		writtenBytes += i;
		cout << writtenBytes << "/" << fileToSend_size << endl;
	}
	cout << writtenBytes << "/" << fileToSend_size << endl;
	printf("sent a file?\n");




	/* Will now try to read a file */
	int fileToRecv_size = 52932;

	FILE *writef;
	writef = fopen("client_got_this_back.jpg", "w+");
	char buff_write[fileToRecv_size];
	int readBytes = 0, j = 0;

	cout << readBytes << "/" << fileToRecv_size << endl;
	while(readBytes < fileToRecv_size){
		if((j = read(fd, buff_write + readBytes, fileToRecv_size - readBytes)) == -1) exit(1);
		readBytes += j;
		cout << readBytes << "/" << fileToRecv_size << endl;
	}
	cout << readBytes << "/" << fileToRecv_size << endl;
	printf("received a file?\n"); 

	fwrite(buff_write, fileToRecv_size, 1, writef);
	fclose(writef);
	
	close(fd);

	return 0;
}
