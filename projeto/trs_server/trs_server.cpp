/**
 *	TRS Server for RC-Translate
 *	Written by group number (GN) 14.
 * 
 * 	Usage: ./TRS language [-p TRSport] [-n TCSname] [-e TCSport]
 *
 **/

// PRINT USER REQUESTS!!!!!!!!!!!!!!!1
//!!!!!!!!!!!!!!!!!!!!!!


#define GN 14 

// Headers/libs
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
 

// Default values
#define TRSPORT_CONST 59000
#define TCSNAME_CONST "localhost"
#define TCSPORT_CONST (58000 + GN)

#define MAX_CHARS_PER_WORD 30
#define MAX_NUM_WORDS_PER_COMMAND 10
#define MAX_LANGS_PER_INSTANCE 99
#define MAX_CHARS_PER_LANGNAME 20

#define MAX_COMPUTER_NAME 256 // Ask. Idunno.

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

int main(int argc, char* argv[]){
	int TRSport = TRSPORT_CONST, TCSport = TCSPORT_CONST;
	string TCSname = TCSNAME_CONST;

	char* lang_name = argv[1]; // No harm doing this even without checking argc.

	if(argc < 2 || argc > 8)
		printWrongUsageExit();
		
	for(int i = 2; i < argc; i+=2){
		if(argv[i][0] != '-'){
			printWrongUsageExit();
		}

		switch(argv[i][1]){
			case 'p': // NEW TRSport
				sscanf(argv[i+1], "%d", &TRSport);
				printf("[!] - Custom TRS port set @ %d.\n", TRSport);
				break;

			case 'n': // NEW TCSname
				char temp[MAX_COMPUTER_NAME];
				sscanf(argv[i+1], "%s", temp);
				printf("[!] - Custom TCS name set @ ");
				cout << TCSname << endl;
				TCSname = temp;
				break;

			case 'e': // NEW TCSport
				sscanf(argv[i+1], "%d", &TCSport);
				printf("[!] - Custom TCS port set @ %d.\n", TCSport);
				break;

			default:
				printWrongUsageExit();
		}
	}

	
	printf("[TRS-Server @ %d] - Now serving \'%s\' for ", TRSport, lang_name);
	cout << TCSname;
	printf(":%d.\n", TCSport);

	return 0;
}
