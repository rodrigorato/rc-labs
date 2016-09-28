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
#include <sstream>

//#define TRSPORT 59000
#define TCSNAME "localhost"
#define TCSPORT 58014

#define MESSAGE1 "ULQ\n"
//#define MESSAGE2 "UNQ "

#define MAX_SIZE_WORD 30
#define MAX_WORDS 10
#define MAX_SIZE_LANGUAGE 20
#define MAX_LANGS 99
#define MAX_COMPUTER_NAME 256 // Ask. Idunno.
#define MAX_FILE_NAME  99 //idk

using namespace std;
//por if em gethosts, perguntar ao user what do do se o tejo nao responde 
int main(int argc, char** argv){	
	int fd;
	socklen_t addrlen;
	struct hostent* hostptr;
	struct sockaddr_in serveraddr, clientaddr;
	char buffer[128], instruction[8];
	char message[128];   //  buffer and message have a random bullshit size
	int TCSport=TCSPORT;
	string TCSname=TCSNAME;
	
	char languages[MAX_LANGS][MAX_SIZE_LANGUAGE], words[MAX_WORDS][MAX_SIZE_WORD];
	char tf;//indicador se vai-se traduzir texto ou ficheiro
	int numWords;
	//./user -n TCSname -p TCSport

	if(argc < 1 || argc > 5)
		printf("wrong input\n");
	for(int i = 1; i < argc; i+=2){
		if(argv[i][0] != '-')
			printf("wrong input\n");

		switch(argv[i][1]){
			case 'p': // NEW TRSport
				sscanf(argv[i+1], "%d", &TCSport);
				printf("[!] - Custom TCS port set @ %d.\n", TCSport);
				break;

			case 'n': // NEW TCSname
				char temp[MAX_COMPUTER_NAME];
				sscanf(argv[i+1], "%s", temp);
				printf("[!] - Custom TCS name set @ ");
				cout << temp << endl;
				TCSname = temp;
				break;

			default:
				printf("wrong input\n");
		}
	}
	
	scanf("%s", instruction);
	while(strcmp(instruction,"exit")){ 
		if (!strcmp(instruction,"list")){       

			if((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) exit(1);//udp

			hostptr = gethostbyname(TCSname.c_str());
			
			memset((void*)&serveraddr, (int)'\0', sizeof(serveraddr));
			serveraddr.sin_family = AF_INET;
			serveraddr.sin_addr.s_addr = ((struct in_addr*) (hostptr->h_addr_list[0]))->s_addr;
			serveraddr.sin_port = htons((u_short)TCSport);
			
			addrlen = sizeof(serveraddr);

			if(sendto(fd, MESSAGE1, strlen(MESSAGE1), 0, (struct sockaddr*) &serveraddr, addrlen) == -1) exit(1);
			
			printf("Sent message:%s", MESSAGE1);//ULQ
		}
		else if (!strcmp(instruction,"request")){
			int langNum;
			scanf("%d %c", &langNum, &tf );

			//UNQ + language   languages[langNum-1]
			string message2="UNQ ";
			string temp= languages[langNum-1];
			message2+=temp;
			message2+='\n';

			if(sendto(fd, message2.c_str(), message2.length() + 1, 0, (struct sockaddr*) &serveraddr, addrlen) == -1) exit(1);
			printf("Sent message: %s", message2.c_str());//UNQ language
			if(tf=='t'){
				scanf("%d", &numWords);
				printf("Palavras a traduzir para %s:\n",languages[langNum-1]);
				for(int i=0; i<numWords;i++){
					scanf("%s",words[i]);
					printf("%s\n",words[i]);
				}
			}else if(tf=='f'){
				char filename[MAX_FILE_NAME];  //provavelmente sitiu mau pa declarar isto????????
				scanf("%s",filename);
			}

		}
		int num_bytes=0;
		if((num_bytes=recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr*) &serveraddr, &addrlen)) == -1) exit(1);
		buffer[num_bytes]='\0';
		printf("Received message:\n%s\n",buffer); 

		sscanf(buffer,"%s", message);
		if(buffer[4]=='E'){ // TCS devolve mensagem de erro
			char m[3];
			sscanf(buffer,"%s %s",message, m);
			if(!strcmp(m,"EOF")) printf("pedido nao pode ser cumprido\n");
			else if (!strcmp(m,"ERR")) printf("Wrong instruction\n");
		}else if(!strcmp(message,"ULR")){    //ULR , resposta ao pedido de linguas
			int numLangs;
			sscanf(buffer,"%s %d",message, &numLangs);
			printf("Languages available:\n");/*
			for(int i=0; i<numLangs;i++){//                  ULR 3 frances ingles whatever

				sscanf(buffer,"%s %d %s",message,&numLangs,languages[i]);
				printf("%d - %s\n", i+1, languages[i]);
			}*/
			int i, j;
			if(numLangs>9) i=7;
			else i=6;
			
			for(int l=0; l<numLangs;l++){
				for (j=0; buffer[i]!=' ' && buffer[i]!='\n' /*&& buffer[i]!='\0'*/;j++,i++)
					languages[l][j]=buffer[i];
				
				languages[l][j]='\0'; 
				i++;
				printf("%d - %s\n", l+1, languages[l]);
			}
			
		}else if (!strcmp(message,"UNR")){
			char ipTRS[16];    //provavelmente sitiu mau pa declarar isto??????
			int portTRS;
			sscanf(buffer, "%s %s %d",message, ipTRS,&portTRS); //id é string num ou struct hostent*?

			//inicio comunicacao com TRS usando TCP
			int fd2;
			struct hostent* hostptr2;
			struct sockaddr_in serveraddr2;
			char buffer2[128];

			if((fd2 = socket(AF_INET, SOCK_STREAM, 0)) == -1) exit(1);
			if((hostptr2=gethostbyname(ipTRS))==NULL) printf("nope\n");

			memset((void*)&serveraddr2, (int)'\0', sizeof(serveraddr2));
			serveraddr2.sin_family = AF_INET;
			serveraddr2.sin_addr.s_addr = ((struct in_addr*) (hostptr2->h_addr_list[0]))->s_addr;//seg fault
			serveraddr2.sin_port = htons((u_short)portTRS);

			if(connect(fd2, (struct sockaddr*) &serveraddr2, sizeof(serveraddr2)) == -1) exit(1);//ta testado ate aqui pk nao trs
			printf("Connected successfully\n");

			if(tf=='t'){
				ostringstream stream;
				string message3 = "TRQ t ";
				stream << numWords;
				message3 += stream.str();
				for(int i=0; i<numWords;i++)	
					message3 += ' ' + words[i];
				message3 += '\n';
				if(write(fd2, message3.c_str(), message3.length()) == -1) exit(1);
				cout << "sent message: \n" << message3 << endl;	
				
			}


			if(read(fd2, buffer2, 128) == -1) exit(1);
			printf("Received message:\n%s\n", buffer2); 
			
			close(fd2);

		}

		scanf("%s", instruction);
	}		
	close(fd);

	return 0;
}
