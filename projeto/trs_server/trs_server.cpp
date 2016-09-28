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
 

// Default values
#define TRSPORT_CONST 59000
#define TCSNAME_CONST "localhost"
#define TCSPORT_CONST (58000 + GN)

#define MAX_CHARS_PER_WORD 30
#define MAX_NUM_WORDS_PER_COMMAND 10
#define MAX_LANGS_PER_INSTANCE 99
#define MAX_CHARS_PER_LANGNAME 20
#define MAX_CHARS_UDP_PROTO_MESSAGE 256

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
	printf("[%s:%d] - Now serving \'%s\' for ", local_name, TRSport, lang_name);
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
	char buffer[128];

	
	printf("[%s:%d] - Will now try to create a UDP socket... ", local_name, TRSport); fflush(stdout);
	if((user_serversocket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) exit(1);
	printf("Success!\n");
	
	printf("[%s:%d] - Setting up socket settings... ", local_name, TRSport); fflush(stdout);
	memset((void*)&my_addr, (int)'\0', sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	my_addr.sin_port = htons((u_short)TRSport);
	printf("Success!\n");
	
	printf("[%s:%d] - Binding the TCP socket to our network name... ", local_name, TRSport); fflush(stdout);
	if(bind(user_serversocket_fd, (struct sockaddr*) &my_addr, sizeof(my_addr)) == -1) exit(1);
	printf("Success!\n");

	printf("[%s:%d] - Starting to listen to connections on the TCP socket... ", local_name, TRSport); fflush(stdout);
	if(listen(user_serversocket_fd, MAX_USER_BACKLOG) == -1) exit(1);
	printf("Success!\n");
	
	useraddr_len = sizeof(user_addr);

	printf("[%s:%d] - Ready to accept connections from users.\n", local_name, TRSport);
	
	if((user_connsocket_fd = accept(user_serversocket_fd, (struct sockaddr*) &user_addr, &useraddr_len)) == -1) exit(1);
	

	
	int read_bytes = 0;
	if((read_bytes = read(user_connsocket_fd, buffer, 128))== -1) exit(1);
	buffer[read_bytes] = '\0';
	printf("Received message:\n%s\n", buffer); 

	if(write(user_connsocket_fd, buffer, strlen(buffer)) == -1) exit(1);
	printf("Sent message:\n%s\n", buffer);	

	close(user_serversocket_fd);
	close(user_connsocket_fd);
	
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
