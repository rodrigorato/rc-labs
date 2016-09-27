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
#include <netdb.h>
#include <string.h>

// C++ Headers/libs
#include <iostream>
#include <string>
 

// Default values
#define TRSPORT_CONST 59000
#define TCSNAME_CONST "localhost"
#define TCSPORT_CONST (58000 + GN)

#define TERM_CHAR '\n'

#define MAX_CHARS_PER_WORD 30
#define MAX_NUM_WORDS_PER_COMMAND 10
#define MAX_LANGS_PER_INSTANCE 99
#define MAX_CHARS_PER_LANGNAME 20
#define MAX_CHARS_UDP_PROTO_MESSAGE 256

#define MAX_COMPUTER_NAME 25 // Ask. Idunno.

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
	printf("A vital syscall for this program failed, we will not be able to continue.\n");
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

int main(int argc, char* argv[]){
	// TRS config variables.
	int TRSport = TRSPORT_CONST, TCSport = TCSPORT_CONST;
	string TCSname = TCSNAME_CONST;
	char* lang_name = argv[1]; // No harm doing this even without checking argc.
	char local_name[MAX_COMPUTER_NAME]; local_name[MAX_COMPUTER_NAME - 1] = '\0';

	// Socket variables.
	int fd;
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
	printf("[%s:%d] - Now serving \'%s\' for ", local_name, TRSport, lang_name);
	cout << TCSname;
	printf(":%d.\n", TCSport);

	// Attempts to get a socket for an UDP connection with the TCS.
	printf("[%s:%d] - Will now try to create a socket.\n", local_name, TRSport);
	if((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) printSysCallFailed();
	printf("[%s:%d] - Successfully created the socket.\n", local_name, TRSport);

	tcs_ptr = gethostbyname(TCSname.c_str());

	memset((void*)&tcs_address, (int)'\0', sizeof(tcs_address)); // Clears tcs_address's struct
	tcs_address.sin_family = AF_INET; // Setting up the socket's struct
	tcs_address.sin_addr.s_addr = ((struct in_addr*) (tcs_ptr->h_addr_list[0]))->s_addr;
	tcs_address.sin_port = htons((u_short)TCSport);

	
	
	sendUdpMessage("Some test message haha!\n", fd, 0, tcs_address);
	string recvd = receiveUdpMessage(fd, MAX_CHARS_UDP_PROTO_MESSAGE, 0, tcs_address).c_str(); 

	close(fd);
	return 0;
}
