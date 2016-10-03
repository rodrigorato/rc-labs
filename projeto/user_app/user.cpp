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
//por if em gethosts, perguntar ao user what do do se o tejo nao responde , verificar se os argumentos tao certos,ostringstream
//wantsAnswer é solucao ate se separar melhor as coisas

bool isError(char buff[]){
	char mRead[3];
	char m[3];
	sscanf(buff,"%s %s",mRead, m);
	if(buff[4]=='E'){ // TCS devolve mensagem de erro
		if(!strcmp(m,"EOF")) printf("pedido nao pode ser cumprido\n");
		else if (!strcmp(m,"ERR")) printf("Wrong instruction\n");
	}
	else if (!strcmp(m,"NTA")) printf("translation is not available\n");
	else return false;
	return true;
}
/*
int startUDP(int fd,struct hostent* hostptr,string TCSname,struct sockaddr_in serveraddr,int TCSport){
	if((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) exit(1);//udp

		hostptr = gethostbyname(TCSname.c_str());
			
		memset((void*)&serveraddr, (int)'\0', sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_addr.s_addr = ((struct in_addr*) (hostptr->h_addr_list[0]))->s_addr;
		serveraddr.sin_port = htons((u_short)TCSport);
			
		addrlen = sizeof(serveraddr);
}*/


void swapStreams(stringstream* a, stringstream *b){
	string a_content = a->str();
	string b_content = b->str();
	a->clear(); a->str(b_content);
	b->clear(); b->str(a_content);
}

int main(int argc, char** argv){	
	int fd;
	socklen_t addrlen;
	struct hostent* hostptr;
	struct sockaddr_in serveraddr, clientaddr;
	char buffer[128];//,instruction[8];
	char message[128];   //  buffer and message have a random bullshit size
	int TCSport=TCSPORT;
	string TCSname=TCSNAME;
	
	char languages[MAX_LANGS][MAX_SIZE_LANGUAGE], words[MAX_WORDS][MAX_SIZE_WORD];
	char tf;//indicador se vai-se traduzir texto ou ficheiro
	int numWords;
	bool wantsAnswer=false;
	//./user -n TCSname -p TCSport

	if(argc < 1 || argc > 5)  // reading input
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
	string user_input,instruction;

	stringstream input_stream;
	getline(cin, user_input);
	input_stream << user_input;
	input_stream >> instruction; 

	/*for( i=0;user_input[i]!=' ';i++){
		instruction[i]=user_input[i];
	}*/
	//scanf("%s", instruction);


	while(strcmp(instruction.c_str(),"exit")){                      // ciclo de espera por input do utilizador
		if (!strcmp(instruction.c_str(),"list")){       

			if((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) exit(1);//udp

			hostptr = gethostbyname(TCSname.c_str());
			
			memset((void*)&serveraddr, (int)'\0', sizeof(serveraddr));
			serveraddr.sin_family = AF_INET;
			serveraddr.sin_addr.s_addr = ((struct in_addr*) (hostptr->h_addr_list[0]))->s_addr;
			serveraddr.sin_port = htons((u_short)TCSport);
			
			addrlen = sizeof(serveraddr);

			if(sendto(fd, MESSAGE1, strlen(MESSAGE1), 0, (struct sockaddr*) &serveraddr, addrlen) == -1) exit(1);
			
			printf("Sent message:%s", MESSAGE1);//ULQ
			wantsAnswer=true;
		}
		else if (!strcmp(instruction.c_str(),"request")){ //request n t palvra
			int langNum;

			input_stream >> langNum;
			input_stream >> tf;


			//UNQ + language   languages[langNum-1]
			string message2="UNQ ";
			string temp2= languages[langNum-1];
			message2+=temp2;
			message2+='\n';
			
			if(sendto(fd, message2.c_str(), message2.length() + 1, 0, (struct sockaddr*) &serveraddr, addrlen) == -1) exit(1);
			printf("Sent message: %s", message2.c_str());//UNQ language

			if(tf=='t'){
				printf("Palavras a traduzir para %s:\n",languages[langNum-1]);
				char temp3[MAX_SIZE_WORD];
				int i;
				for(i=0;input_stream >> words[i];i++){
					printf("%s\n",words[i]);
				}
				numWords=i;

				
			}else if(tf=='f'){
				char filename[MAX_FILE_NAME];  //provavelmente sitiu mau pa declarar isto????????
				scanf("%s",filename);
			}
			wantsAnswer=true;

		}
		if (wantsAnswer){
			int num_bytes=0;                                   // recieving message from central server
			if((num_bytes=recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr*) &serveraddr, &addrlen)) == -1) exit(1);
			buffer[num_bytes]='\0';
			printf("Received message:\n%s",buffer); 
	//to do: por issto numa funçao á parte
			sscanf(buffer,"%s", message);
			if (!isError(buffer)){
				if(!strcmp(message,"ULR")){    //ULR , resposta ao pedido de linguas
					int numLangs;
					sscanf(buffer,"%s %d",message, &numLangs);//ULR 3 frances ingles whatever
					printf("Languages available:\n");

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
				
				}else if (!strcmp(message,"UNR")){//UNR lingua ip port, e depois comunicacao com trs
					char ipTRS[16];    //provavelmente sitiu mau pa declarar isto??????
					int portTRS;
					sscanf(buffer, "%s %s %d",message, ipTRS,&portTRS); //id é string num ou struct hostent*?

					//inicio comunicacao com TRS usando TCP
					int fd2;
					struct hostent* hostptr2;
					struct sockaddr_in serveraddr2;
					char buffer2[128];

					if((fd2 = socket(AF_INET, SOCK_STREAM, 0)) == -1) exit(1);
					if((hostptr2=gethostbyname(ipTRS))==NULL) printf("erro gethostbyname\n"); // to do: exit(1)

					memset((void*)&serveraddr2, (int)'\0', sizeof(serveraddr2));
					serveraddr2.sin_family = AF_INET;
					serveraddr2.sin_addr.s_addr = ((struct in_addr*) (hostptr2->h_addr_list[0]))->s_addr;//seg fault
					serveraddr2.sin_port = htons((u_short)portTRS);

					if(connect(fd2, (struct sockaddr*) &serveraddr2, sizeof(serveraddr2)) == -1) exit(1);//ta testado ate aqui pk nao trs
					printf("Connected successfully\n");

					if(tf=='t'){
						stringstream stream;
						string message3 = "TRQ t ";
						stream << numWords;
						message3 += stream.str();
						
						for(int i=0; i<numWords;i++){	
							stringstream s ;
							s<< words[i];
							message3+= ' ' + s.str();
							//cout<< message3<<endl;
						}
						message3 += '\n';
						if(write(fd2, message3.c_str(), message3.length()) == -1) exit(1);
						cout << "sent message: \n" << message3 << endl;			
					}
					int num_bytes=0;
					if((num_bytes=read(fd2, buffer2, 128)) == -1) exit(1);
					buffer2[num_bytes]='\0';
					printf("Received message:\n%s\n", buffer2); 

					if(!isError(buffer2)){
						
						stringstream st;
						st.str(buffer2);
						st >> buffer2;//lixo, TRR é preciso verificar?
						st >> tf;
						st >>numWords;
						if (tf=='t'){
							cout << "traducao:"<<endl;
							for(int i=0;st >> words[i];i++)
								cout<<words[i]<<endl;
						}
					}
					


					close(fd2);
				}
			}
		}
		wantsAnswer=false;
		getline(cin, user_input);
		//input_stream.str(input_stream.str().replace(0,input_stream.str().length(),user_input));
		//input_stream << user_input;
		stringstream s;
		s<<user_input;
		swapStreams(&s, &input_stream);
		//s.swap(input_stream);
		input_stream >> instruction;
		cout<<instruction<<endl;
		
		/*
		for( i=0;user_input[i]!=' ';i++){
			instruction[i]=user_input[i];
		}*/
		//scanf("%s", instruction);
	}		
	close(fd);

	return 0;
}
