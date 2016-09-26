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
//#define MESSAGE2 "UNQ "

#define MAX_SIZE_WORD 30
#define MAX_WORDS 10
#define MAX_SIZE_LANGUAGE 20
#define MAX_LANGS 99

int main(){	
	int fd;
	socklen_t addrlen;
	struct hostent* hostptr;
	struct sockaddr_in serveraddr, clientaddr;
	char buffer[128], instruction[8];
	char *message;
	char languages[MAX_LANGS][MAX_SIZE_LANGUAGE], words[MAX_WORDS][MAX_SIZE_WORD];


	
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
			
			printf("Sent message:%s", MESSAGE1);//ULQ
		}
		else if (!strcmp(instruction,"request")){
			int langNum;
			char tf;
			scanf("%d %c", &langNum, &tf );

			//UNQ + language   languages[langNum-1]
			char message2[4+MAX_SIZE_LANGUAGE+1] = "UNQ ";// necessario espaco pa \n?
			strcat(message2, languages[langNum-1]);
			strcat(message2, '\n');
			if(sendto(fd, message2, strlen(message2) + 1, 0, (struct sockaddr*) &serveraddr, addrlen) == -1) exit(1);
			printf("Sent message: %s", message2);//UNQ language
			if(tf=='t'){
				int numWords;
				scanf("%d", &numWords);
				printf("Palavras a imprimir para %s:\n",languages[langNum-1]);
				for(int i=0; i<numWords;i++){
					scanf("%s",words[i]);
					printf("%s\n",words[i]);
				}
			}else if(tf=='f'){
				scanf("%s",filename);
			}

		}
			
		if(recvfrom(fd, buffer, strlen(buffer), 0, (struct sockaddr*) &serveraddr, &addrlen) == -1) exit(1);
		printf("Received message:\n%s", buffer); 

		sscanf(buffer,"%s", message);
		if(buffer[4]=='E'){ // TCS devolve mensagem de erro
			char *m;
			sscanf(buffer,"%s",m);
			if(!strcmp(m,"EOF")) printf("pedido nao pode ser cumprido\n");
			else if (!strcmp(m,"ERR")) printf("Wrong instruction\n");
		}else if(!strcmp(message,"ULR")){    //ULR , resposta ao pedido de linguas
			int numLangs;
			sscanf(buffer,"%d",&numLangs);
			printf("Languages available:\n");
			for(int i=0; i<numLangs;i++){

				sscanf(buffer,"%s",languages[i]);
				printf("%d - %s\n", i+1, languages[i]);
			}
			
		}else if (!strcmp(message,"UNR")){
			sscanf(buffer, "%s %d",ipTRS,&portTRS); //id é string num ou struct hostent*?
			//inicio comunicacao com TRS usando TCP
			/*int fd;
			struct hostent* hostptr;
			struct sockaddr_in serveraddr;
			char buffer[128];	
			
			if((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) exit(1);
			
			hostptr = gethostbyname("lima.tecnico.ulisboa.pt");
				
			memset((void*)&serveraddr, (int)'\0', sizeof(serveraddr));
			serveraddr.sin_family = AF_INET;
			serveraddr.sin_addr.s_addr = ((struct in_addr*) (hostptr->h_addr_list[0]))->s_addr;
			serveraddr.sin_port = htons((u_short)PORT);
			
			
			if(connect(fd, (struct sockaddr*) &serveraddr, sizeof(serveraddr)) == -1) exit(1);
			printf("Connected successfully\n");


			if(write(fd, MESSAGE, strlen(MESSAGE)) == -1) exit(1);
			printf("Sent message:\n%s\n", MESSAGE);	


			if(read(fd, buffer, 128) == -1) exit(1);
			printf("Received message:\n%s\n", buffer); 

			
			close(fd);*/
		}

		scanf("%s", instruction);
	}		
	close(fd);

	return 0;
}
