/**
 *	TRS Server for RC-Translate
 *	Written by group number (GN) 14.
 *
 **/

// PRINT USER REQUESTS!!!!!!!!!!!!!!!1
//!!!!!!!!!!!!!!!!!!!!!!


#define GN 14 

// C Headers/libs
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <net/if.h>
#include <sys/ioctl.h>

// C++ Headers/libs
#include <iostream>
#include <string>
#include <sstream>
#include <list>
#include <fstream>
 

// Default values
#define TRSPORT_CONST 59000
#define TCSNAME_CONST "localhost"
#define TCSPORT_CONST (58000 + GN)

#define MAX_CHARS_PER_WORD 30
#define MAX_NUM_WORDS_PER_REQUEST 10
#define MAX_LANGS_PER_INSTANCE 99
#define MAX_CHARS_PER_LANGNAME 20
#define MAX_CHARS_UDP_PROTO_MESSAGE 256
#define MAX_CHARS_TCP_PROTO_MESSAGE 256
#define TRANSLATION_NOT_AVAILABLE_WORD ""

#define MAX_COMPUTER_NAME 25 // Ask. Idunno.
#define MAX_USER_BACKLOG 5

// Protocol Messages
#define TERM_CHAR '\n'
#define TCS_SRR_OK "SRR OK\n"
#define TCS_SRR_NOK "SRR NOK\n"
#define TCS_SRR_ERR "SRR ERR\n"
#define TCS_SUN_OK "SUR OK\n"
#define TCS_SUN_NOK "SUR NOK\n"
#define TCS_SUN_ERR "SUR ERR\n"


using namespace std;

void printWrongUsageExit(){
	printf("Usage: ./TRS language [-p TRSport] [-n TCSname] [-e TCSport]\n");
	printf("In which language is this server's known language.\n");
	printf("\tOptional parameters:\n"); 
	printf("\tTRSport: will set the port on which this server will use TCP on to talk to the users\n"); 
	printf("\tTCSname: will set the name of the machine that registers all the TRSs (the TCS)\n"); 
	printf("\tTCSport: will set the port on which to talk to the TCS machine using UDP.\n");
	exit(1);
}

void printSysCallFailed(){
	printf("Syscall failed!\n");
	printf("For further detail check the log that was being written on this terminal.\n");
	exit(1);
}

void printBufferReadFailed(){
	printf("Failed to read data into a buffer, size was not big enough.\n");
	printf("For further detail check the log that was being written on this terminal.\n");
	exit(1);
}

void sendUdpMessage(string message, int sock_fd, int flags, sockaddr_in tcs_address){
	socklen_t addrlen = sizeof(tcs_address);
	// sendto(int sockfd, char* message, size_t len(message), int flags = 0, sockaddr* dest_addr, socklen_t addrlen);
	if(sendto(sock_fd, message.c_str(), strlen(message.c_str()), 0, (struct sockaddr*) &tcs_address, addrlen) == -1) 
		printSysCallFailed();
	
}	

string receiveUdpMessage(int socket_fd, int buffersize, int flags, sockaddr_in src_addr){
		socklen_t addrlen = sizeof(src_addr);
		char buffer[buffersize];
		string final_message;
		if(recvfrom(socket_fd, buffer, buffersize, 0, (struct sockaddr*) &src_addr, &addrlen) == -1) 
			printSysCallFailed();

		for(int i = buffersize-1; i >= 0; i--)
			if(buffer[i] == TERM_CHAR){
				if((i+1) < buffersize)
					buffer[i+1] = '\0';
				else
					printBufferReadFailed();
			}

		final_message = buffer;
		return final_message;
}

string receiveTcpMessage(int socketFd, int bufferSize){
	int read_bytes = 0;
	char buffer[bufferSize];
	string message;
	if((read_bytes = read(socketFd, buffer, bufferSize)) == -1) 
		printSysCallFailed();
	buffer[read_bytes] = '\0';
	message = buffer;
	return message;
}

void sendTcpMessage(int socketFd, string message){
	if(write(socketFd, message.c_str(), strlen(message.c_str())) == -1) 
		printSysCallFailed();
}

string getMyIp(int buffersize){
    struct hostent *h;
	struct in_addr *a;
	char buffer[buffersize];
	string ip;

	gethostname(buffer, buffersize);
	if((h=gethostbyname(buffer))==NULL)exit(1);//error

	a=(struct in_addr*)h->h_addr_list[0];
	ip = inet_ntoa(*a);
    return ip;
}

string registerWithTCS(int socket_fd, sockaddr_in src_addr, int flags, string language, string ourIP, int ourPort){
	// Registers this TRS with the main TCS server and returns the answer.
	stringstream temp; temp << ourPort;
	string port_str = temp.str();
	string srg_message = "SRG " + language + " " + ourIP + " " + port_str + TERM_CHAR;
	sendUdpMessage(srg_message.c_str(), socket_fd, 0, src_addr); // Sends the SRG request
	return receiveUdpMessage(socket_fd, MAX_CHARS_UDP_PROTO_MESSAGE, flags, src_addr);
}

string unregisterWithTCS(int socket_fd, sockaddr_in src_addr, int flags, string language, string ourIP, int ourPort){
	// Registers this TRS with the main TCS server and returns the answer.
	stringstream temp; temp << ourPort;
	string port_str = temp.str();
	string srg_message = "SUN " + language + " " + ourIP + " " + port_str + TERM_CHAR;
	sendUdpMessage(srg_message.c_str(), socket_fd, 0, src_addr); // Sends the SRG request
	return receiveUdpMessage(socket_fd, MAX_CHARS_UDP_PROTO_MESSAGE, flags, src_addr);
}

string getTranslation(string word){
	return "le_" + word;
}

string intToString(int num){
	ostringstream s;
	s << num;
	return s.str();
}

int main(int argc, char* argv[]){
	// TRS config variables.
	int TRSport = TRSPORT_CONST, TCSport = TCSPORT_CONST;
	string TCSname = TCSNAME_CONST;
	char* lang_name = argv[1]; // No harm doing this even without checking argc.
	char local_name[MAX_COMPUTER_NAME]; local_name[MAX_COMPUTER_NAME - 1] = '\0';

	// Socket variables.
	int TCS_socket_fd;
	struct hostent* tcs_ptr;
	struct sockaddr_in tcs_address;

	// Parse the arguments and the the variables accordingly.
	if(argc < 2 || argc > 8)
		printWrongUsageExit();
	for(int i = 2; i < argc; i+=2){
		if(argv[i][0] != '-')
			printWrongUsageExit();

		switch(argv[i][1]){
			case 'p': // NEW TRSport
				sscanf(argv[i+1], "%d", &TRSport);
				printf("[!] - Custom TRS port set to %d.\n", TRSport);
				break;

			case 'n': // NEW TCSname
				char temp[MAX_COMPUTER_NAME];
				sscanf(argv[i+1], "%s", temp);
				printf("[!] - Custom TCS name set to ");
				cout << temp << endl;
				TCSname = temp;
				break;

			case 'e': // NEW TCSport
				sscanf(argv[i+1], "%d", &TCSport);
				printf("[!] - Custom TCS port set to %d.\n", TCSport);
				break;

			default:
				printWrongUsageExit();
		}
	}
	
	// Writes out the current session data.
	if(gethostname(local_name, MAX_COMPUTER_NAME - 1) == -1) printSysCallFailed();
	printf("[%s:%d] - Now serving the \'%s\' language for the TCS server at ", local_name, TRSport, lang_name);
	cout << TCSname;
	printf(":%d.\n", TCSport);

	// Attempts to get a socket for an UDP connection with the TCS.
	printf("[%s:%d] - Will now try to create a UDP socket... ", local_name, TRSport); fflush(stdout);
	if((TCS_socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) printSysCallFailed();
	printf("Success!\n");

	tcs_ptr = gethostbyname(TCSname.c_str());
	if(tcs_ptr == NULL)
		printSysCallFailed();


	printf("[%s:%d] - Setting up socket settings... ", local_name, TRSport); fflush(stdout);
	memset((void*)&tcs_address, (int)'\0', sizeof(tcs_address)); // Clears tcs_address's struct
	tcs_address.sin_family = AF_INET; // Setting up the socket's struct
	tcs_address.sin_addr.s_addr = ((struct in_addr*) (tcs_ptr->h_addr_list[0]))->s_addr; // THIS IS FAILING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!	
	tcs_address.sin_port = htons((u_short)TCSport); 
	printf("Success!\n");

	// Done setting up, will now warn TCS that we're live.
	printf("[%s:%d] - Telling the TCS that we are live... ", local_name, TRSport); fflush(stdout);
	string TCSresp;
	while((TCSresp = registerWithTCS(TCS_socket_fd, tcs_address, 0, lang_name, getMyIp(MAX_COMPUTER_NAME), TRSport)) != TCS_SRR_OK){
		// Failed to register with the TCS
		if (TCSresp == TCS_SRR_NOK){
			// Due to NOK message
			printf("NOK\n");
		} else if (TCSresp == TCS_SRR_ERR){
			// Due to ERR message
			printf("ERR\n");
		} else{
			// Due to protocol error	
			printf("PROTO ERR\n");
		}
	}

	//Registered with the TCS successfully
	printf("Success!\n");
	
	// Now accepting connections from the users, starting the TCP server
	int user_serversocket_fd, user_connsocket_fd;
	struct sockaddr_in my_addr, user_addr;
	socklen_t useraddr_len;

	
	printf("[%s:%d] - Will now try to create a TCP socket... ", local_name, TRSport); fflush(stdout);
	if((user_serversocket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) printSysCallFailed();
	printf("Success!\n");
	
	printf("[%s:%d] - Setting up socket settings... ", local_name, TRSport); fflush(stdout);
	memset((void*)&my_addr, (int)'\0', sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	my_addr.sin_port = htons((u_short)TRSport);
	printf("Success!\n");
	
	printf("[%s:%d] - Binding the TCP socket to our network name... ", local_name, TRSport); fflush(stdout);
	if(bind(user_serversocket_fd, (struct sockaddr*) &my_addr, sizeof(my_addr)) == -1) printSysCallFailed();
	printf("Success!\n");

	printf("[%s:%d] - Starting to listen to connections on the TCP socket... ", local_name, TRSport); fflush(stdout);
	if(listen(user_serversocket_fd, MAX_USER_BACKLOG) == -1) printSysCallFailed();
	printf("Success!\n");
	
	useraddr_len = sizeof(user_addr);

	printf("[%s:%d] - Ready to accept connections from users.\n", local_name, TRSport);

	bool continueListening = true;
	while(continueListening){ // TO-DO decent loop to end this
		int forkId = -1;

		if((user_connsocket_fd = accept(user_serversocket_fd, (struct sockaddr*) &user_addr, &useraddr_len)) == -1) 
			printSysCallFailed();
		
		struct hostent* he = gethostbyaddr(&user_addr.sin_addr, sizeof user_addr.sin_addr, AF_INET);
		
		string userNameAndPort = (he->h_name); 
		userNameAndPort += ":"; 
		userNameAndPort += intToString(ntohs(user_addr.sin_port));
		
		string userAddrAndPort = (inet_ntoa(user_addr.sin_addr)); 
		userAddrAndPort += ":"; 
		userAddrAndPort += intToString(ntohs(user_addr.sin_port));
		
		printf("[%s:%d] - Accepted connection from user at %s (%s).\n", local_name, TRSport, 
									userNameAndPort.c_str(), 
									userAddrAndPort.c_str());
		forkId = fork();

		if(forkId == -1)
			printSysCallFailed();
		else if(forkId == 0){
			// Child process code, tries to read the user's request
			

			//string receiveTcpMessage(int socketFd, int bufferSize)
			//void sendTcpMessage(int socketFd, string message)

			stringstream cmd; cmd << receiveTcpMessage(user_connsocket_fd, MAX_CHARS_TCP_PROTO_MESSAGE);
			string temp; cmd >> temp;

			if(temp == "TRQ"){
				cmd >> temp;
				if(temp == "t"){
					// Text translation
					cmd >> temp; // temp should contain a number >0 and <=30
					int numWords = atoi(temp.c_str());
					string userRequest = "[REQUEST] User at " + userNameAndPort + " requested us to translate a sentence with " + intToString(numWords) + " words:\n[REQUEST] ";
					bool transAvailable = true;
					if(numWords >= 0 && numWords <= MAX_NUM_WORDS_PER_REQUEST){
						// Read the words
						list<string> wordsToTranslate, translatedWords;
						for(int i = 0; i < numWords; i++){
							cmd >> temp;
							wordsToTranslate.push_back(temp);
							userRequest += temp + " ";
						}

						userRequest.pop_back(); // Remove the string's last char in O(1)
						printf("%s.\n", userRequest.c_str());

						while((!wordsToTranslate.empty()) && transAvailable){
							// Do the actual translation and build the translated words list
							string translatedWord = getTranslation(wordsToTranslate.front());
							if(translatedWord == TRANSLATION_NOT_AVAILABLE_WORD)
								transAvailable = false;
							else{
								translatedWords.push_back(translatedWord);
								wordsToTranslate.pop_front();
							}
						}

						if(transAvailable){
							// There was a translation and its in the translatedWords list
							string response = "TRR t " + intToString(numWords) + " ";
							string answer;

							while(!translatedWords.empty()){
								answer += translatedWords.front() + " ";
								translatedWords.pop_front();
							}

							answer.pop_back();
							printf("[ANSWER] Answering user at %s with the translation:\n[ANSWER] %s.\n", userNameAndPort.c_str(), answer.c_str());
							answer += TERM_CHAR;
							// 0 word answer
							sendTcpMessage(user_connsocket_fd, response + answer);
						}

						else{
							// There wasn't a translation available
							// TO-DO: handle the proto message here
							string response = "TRR NTA";
							response += TERM_CHAR;
							sendTcpMessage(user_connsocket_fd, response);
						}

					} else{ 
						// TO-DO: handle protocol error
						printf("PROTO ERR\n");
					}

				} else if(temp == "f"){
					// File translation
					// TO-DO

				} else printf("PROTO ERR\n");
			}


			
			close(user_connsocket_fd);
			exit(0);
		}
		else{
			// parent code
			char temp_command[MAX_CHARS_PER_WORD];
			scanf("%s", temp_command);
			if(!strcmp(temp_command, "exit")) continueListening = false;

		}

	}
	close(user_serversocket_fd);
	
	
	
	// Unregistering with the TCS so we can exit
	printf("[%s:%d] - Telling the TCS that we are going offline.\n", local_name, TRSport);
	while((TCSresp = unregisterWithTCS(TCS_socket_fd, tcs_address, 0, lang_name, getMyIp(MAX_COMPUTER_NAME), TRSport)) != TCS_SUN_OK){
		// Failed to register with the TCS
		if (TCSresp == TCS_SUN_NOK){
			// Due to NOK message
			printf("NOK\n");

		} else if (TCSresp == TCS_SUN_ERR){
			// Due to ERR message
			printf("ERR\n");
		} else{
			// Due to protocol error	
			printf("PROTO ERR\n");
		}
	}
	// Successfully unregistered with the TCS
	printf("[%s:%d] - Successfully unregistered with the TRS.\n", local_name, TRSport);

	printf("[%s:%d] - All done, now exiting.\n", local_name, TRSport);
	close(TCS_socket_fd);
	return 0;
}
