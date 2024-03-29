#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <math.h>
#include <signal.h>


#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#define TCSNAME "localhost"
#define TCSPORT 58014

#define ULQ "ULQ\n"
#define UNQ "UNQ "
#define ULR "ULR"
#define UNR "UNR"
#define TRQ "TRQ "
#define TRR "TRR"

#define MAX_SIZE_WORD 30
#define MAX_WORDS 10
#define MAX_SIZE_LANGUAGE 20
#define MAX_LANGS 99

#define BUFFER_SIZE 256		
#define MAX_COMPUTER_NAME 50  //valor nao especificado no enunciado
#define MAX_FILE_NAME  99 	  //valor nao especificado no enunciado

using namespace std;
//por if em gethosts, perguntar ao user what do do se o tejo nao responde , verificar se os argumentos tao certos,ostringstream
//wantsAnswer é solucao ate se separar melhor as coisas, nao deixar fazer requests primeiro, nao ter numeros randoms no programa
//assegurar k tamanho max sao cumpridos(filename), erros no fopen fseek e cenas, ,verificar mensagens de user e server

bool isError(char buff[]){
	char mRead[3];
	char m[3];
	sscanf(buff,"%s %s",mRead, m);
	if(buff[4]=='E'){ // TCS devolve mensagem de erro
		if(!strcmp(m,"EOF")) printf("Error: The server was not able to do the request\n");
		else if (!strcmp(m,"ERR")) printf("Error: Wrong instruction sent to server\n");
	}
	else if (!strcmp(m,"NTA")) printf("Error: Translation is not available\n");
	else return false;
	return true;
}

void oopsError(){
	printf("There was an error executing a function\n");
	exit(1);
}

void alarmCatcher(int error){}

struct sockaddr_in startUDP(struct hostent* hostptr,string TCSname,struct sockaddr_in serveraddr,int TCSport){

		if((hostptr = gethostbyname(TCSname.c_str()))==NULL) oopsError();
			
		memset((void*)&serveraddr, (int)'\0', sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_addr.s_addr = ((struct in_addr*) (hostptr->h_addr_list[0]))->s_addr;
		serveraddr.sin_port = htons((u_short)TCSport);
		
		return serveraddr;
}

int intLength(int n){
	stringstream ss;
	ss << n;
	string s =ss.str();
	return s.length();
}

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
	struct sockaddr_in serveraddr;
	int TCSport=TCSPORT;
	string TCSname=TCSNAME;
	char buffer[BUFFER_SIZE];
	
	
	char languages[MAX_LANGS][MAX_SIZE_LANGUAGE], words[MAX_WORDS][MAX_SIZE_WORD];
	string filename;
	char tf;//indicador se vai-se traduzir texto ou ficheiro
	int numWords, numLangs;
	bool wantsAnswer=false;
	bool first=true;
	bool fdOpen=false;

	
	if(signal(SIGALRM,alarmCatcher) == SIG_ERR) oopsError();
	if(siginterrupt(SIGALRM,1) == -1) oopsError();


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
				printf("Wrong input\n");
		}
	}

	string user_input,instruction;
	stringstream input_stream;
	getline(cin, user_input);
	input_stream << user_input;
	input_stream >> instruction; 


	while(strcmp(instruction.c_str(),"exit")){          // ciclo de espera por input do utilizador
		if (!strcmp(instruction.c_str(),"list")){       

			if((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) oopsError();//udp

			serveraddr=startUDP(hostptr,TCSname,serveraddr,TCSport);
			addrlen = sizeof(serveraddr);
			fdOpen = true;

			cout <<"Connected to TCS successfully"<<endl;
			if(sendto(fd, ULQ, strlen(ULQ), 0, (struct sockaddr*) &serveraddr, addrlen) == -1) oopsError();
			
			first=false;
			wantsAnswer=true;
		}
		else if (!strcmp(instruction.c_str(),"request")){ //request n t palvra
			if (first) printf("You must must first use 'list' before you can request\n");
			else{
				if(!fdOpen){
					if((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) oopsError();
					serveraddr=startUDP(hostptr,TCSname,serveraddr,TCSport);
					addrlen = sizeof(serveraddr);
					fdOpen = true;
				}
				int langNum;
				if (input_stream >> langNum && input_stream >> tf){

					//UNQ + language   l
					string message=UNQ;
					string temp= languages[langNum-1];
					message+=temp;
					message+='\n';
					
					if(sendto(fd, message.c_str(), message.length() , 0, (struct sockaddr*) &serveraddr, addrlen) == -1) oopsError();
					if(langNum > numLangs) printf("That language is not available!\n");
					else{
						if(tf=='t'){
							printf("Palavras a traduzir de %s para portugues:\n",languages[langNum-1]);
							int i;
							printf("->");
							for(i=0;input_stream >> words[i];i++){
								printf("%s ",words[i]);
							}
							printf("\n");
							numWords=i;
							wantsAnswer=true;

						}else if(tf=='f'){
							wantsAnswer=true;
							input_stream >> filename;
							printf("Imagem a traduzir de %s: %s\n",languages[langNum-1],filename.c_str() );
						}else{printf("Wrong input\n");}
					}
				}else{printf("Wrong input\n");}
			}
		}
		if (wantsAnswer){     // recieving message from central server
			char message[3];// array para onde se vai copiar as instrucoes de 3 chars (ULR,UNR)
			alarm(15);                                 
			if(recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr*) &serveraddr, &addrlen) == -1) oopsError();
			alarm(0);

			sscanf(buffer,"%s", message);
			if (!isError(buffer)){
				if(!strcmp(message,ULR)){    //ULR , resposta ao pedido de linguas
					sscanf(buffer,"%s %d",message, &numLangs);//ULR n linguas
					printf("Languages available:\n");

					int i = strlen(ULR)+intLength(numLangs)+strlen(" ")*2;//indice para inicio da 1a lingua
					int j;
					for(int l=0; l<numLangs;l++){
						for (j=0; buffer[i]!=' ' && buffer[i]!='\n';j++,i++)
							languages[l][j]=buffer[i];
						languages[l][j]='\0'; 
						i++;
						printf("%d - %s\n", l+1, languages[l]);
					}
				
				}else if (!strcmp(message,UNR)){//UNR lingua ip port, e depois comunicacao com trs
					char ipTRS[16];//16-tamanho maximo de ip
					int portTRS;
					sscanf(buffer, "%s %s %d",message, ipTRS,&portTRS); 

					//inicio comunicacao com TRS usando TCP
					int fd2;
					struct hostent* hostptr2;
					struct sockaddr_in serveraddr2;
					char buffer2[BUFFER_SIZE];
					
					if((fd2 = socket(AF_INET, SOCK_STREAM, 0)) == -1) oopsError();
					if((hostptr2=gethostbyname(ipTRS))==NULL) oopsError(); 
					memset((void*)&serveraddr2, (int)'\0', sizeof(serveraddr2));
					serveraddr2.sin_family = AF_INET;
					serveraddr2.sin_addr.s_addr = ((struct in_addr*) (hostptr2->h_addr_list[0]))->s_addr;
					serveraddr2.sin_port = htons((u_short)portTRS);
					
					
					if(connect(fd2, (struct sockaddr*) &serveraddr2, sizeof(serveraddr2)) == -1) oopsError();
					printf("Connected to TRS successfully\n");

					int size=0;
					string message2 = TRQ;
					message2 += tf;
					message2 +=' ';
					if(tf=='t'){
						stringstream stream;
						stream << numWords;
						message2 += stream.str();
						
						for(int i=0; i<numWords;i++){	
							stringstream s ;
							s<< words[i];
							message2+= ' ' + s.str();
						}
						message2 += '\n';
						if(write(fd2, message2.c_str(), message2.length()) == -1) oopsError();

						//recieving answer
						int num_bytes=0;
						if((num_bytes=read(fd2, buffer2, BUFFER_SIZE)) == -1) oopsError();
						buffer2[num_bytes]='\0';

						if(!isError(buffer2)){
							stringstream s;
							string trr;
							s.str(buffer2);
							s >> trr;
							if(trr==TRR){
								s >> tf;
								s >>numWords;
								if (tf=='t'){ 
									cout << "traducao:\n->";
									for(int i=0;s >> words[i];i++)
										cout<<words[i]<<' ';
									cout<<endl;
								}else {printf("Unexpected format in server message\n");}
							}else {printf("Unexpected format in server message\n");}
						}

					}else if (tf=='f'){
						stringstream temp;
						message2 += filename; 
						message2 += ' ';
						
						FILE * file = fopen(filename.c_str(), "rb");
						if (file) {
							if(fseek(file, 0L, SEEK_END) == -1) oopsError() ;//calculate the size of the file
							if((size = ftell(file))==-1) oopsError();
							rewind(file);


							temp <<size;
							message2 += temp.str();
							message2 += ' ';
							if(write(fd2, message2.c_str(), message2.length()) == -1) oopsError();

							char content[size];
	       					while(fread(content, size,1 , file)==1);
	       					if(fclose(file)==EOF) oopsError();
							
							int n;
							int total=0;
	   						while(total<size){
								if((n=write(fd2, content+total, size-total)) == -1) oopsError();
								total +=n;
							}
							if((write(fd2, "\n", 1)) == -1) oopsError();
				
	       					cout<< "File sent "<<endl;

			
							// recieving responce and creating file

							int num_bytes=0;
							if((num_bytes=read(fd2, buffer2, BUFFER_SIZE)) == -1) oopsError();
							
							//trr f filename size 
							if(!isError(buffer2)){
								string temp2;
								int filesize;
								stringstream st;
									
								st.str(buffer2);
								st >> temp2; //TRR
								if(temp2==TRR){
									st >> tf;  //f
									if(tf=='f'){
										st >> filename;
										st >> filesize ;

										//headersize=length(TRR f filename size )
										int headersize = strlen(TRR)+strlen("f")+filename.length()+intLength(filesize)+strlen(" ")*4;
										filename="translated_"+filename;
										char data[filesize];
										
										for(int j = headersize , k = 0; j != num_bytes; j++, k++)
											data[k] = buffer2[j];

										total=num_bytes - headersize;
										while(total < filesize){
											if((n = read(fd2, data + total, filesize - total)) == -1) oopsError();
											total += n;
										}
										cout<<"translated file recieved: "<<filename<<endl;
										FILE* file = fopen(filename.c_str(), "w+b");
										if (!file) oopsError();
										total=0;
										while(total < filesize){
											if((n=fwrite(data+total, 1,filesize , file))==0) oopsError();
											total+=n;
										}
										cout << "translated file created"<<endl;
										if(fclose(file) == EOF) oopsError();	
									}else {printf("Unexpected format in server message\n");}
								}else {printf("Unexpected format in server message\n");}
							}
						}else printf("That file does not exist\n");
					}
					if(close(fd2)==-1) oopsError();
				}else {printf("Unexpected format in server message\n");}
			}
			if(close(fd)==-1) oopsError();
			fdOpen=false;
		}else{printf("Please correct your input\n");}
		wantsAnswer=false;
		getline(cin, user_input);
		stringstream s;
		s<<user_input;
		swapStreams(&s, &input_stream);
		input_stream >> instruction;
	}		
	return 0;
}
