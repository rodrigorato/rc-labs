#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#include <iostream>
#include <string>

#define TRSPORT 59000
#define TCSNAME "localhost"
#define TCSPORT 58014

#define MESSAGE1 "ULQ\n"

#define MAX_SIZE_WORD 30
#define MAX_WORDS 10
#define MAX_SIZE_LANGUAGE 20
#define MAX_LANGS 99

int main(){	
	int fd;
	socklen_t addrlen;
	struct hostent* hostptr;
	struct sockaddr_in serveraddr, clientaddr;
	char buffer[128];
	char *instruction, *message;
	
	scanf("%s", instruction);
	while(strcmp(instruction,"exit")){ 
		if (!strcmp(instruction,"list")){  

			if((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) exit(1);//udp

			hostptr = gethostbyname(TCSNAME);
			
			memset((void*)&serveraddr, (int)'\0', sizeof(serveraddr));
			serveraddr.sin_family = AF_INET;
			serveraddr.sin_addr.s_addr = ((struct in_addr*) (hostptr->h_addr_list[0]))->s_addr;
			serveraddr.sin_port = htons((u_short)TCSPORT);
			
			addrlen = sizeof(serveraddr);

			if(sendto(fd, MESSAGE1, strlen(MESSAGE1) + 1, 0, (struct sockaddr*) &serveraddr, addrlen) == -1) exit(1);
			
			printf("Sent message:\n%s", MESSAGE1);//ULQ
		}
		else if (!strcmp(instruction,"request")){

		}
			
		if(recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr*) &serveraddr, &addrlen) == -1) exit(1);
			
		printf("Received message:\n%s", buffer); 
		sscanf(buffer,"%s", message);
		if(!strcmp(message,"ULR")){    //ULR , resposta ao pedido de linguas
			if(buffer[4]=='E'){ // TCS devolve mensagem de erro
				sscanf(buffer,"%s",message);
				if(!strcmp(message,"EOF")) printf("TCS is busy\n");
				else if (!strcmp(message,"ERR")) printf("Wrong instruction\n");
			}
			else{
				int nL;
				sscanf(buffer,"%d",&nL);
				printf("Languages available:\n");
				for(int i=0; i<nL;i++){
					char language[MAX_SIZE_LANGUAGE];
					sscanf(buffer,"%s",language);
					printf("%d - %s\n", i+1, language);
				}
			}
		}
		scanf("%s", instruction);
	}		
	close(fd);

	return 0;
}
