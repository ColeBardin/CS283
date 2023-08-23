#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

int main(int argc, char **argv){
	struct sockaddr_in saddr;
	char *screenname, *hostname;
	int port;

	if(argc < 2 || argc > 4){
		fprintf(stderr, "Error: no screen name provided\n");
		fprintf(stderr, "Usage: ./client screenname [hostname [port]]\n");
		exit(1);
	}

	screenname = argv[1];
	hostname = "tux3";
	port = 2023;
	if(argc > 3){
		port = atoi(argv[3]);
	}
	if(argc > 2){
		hostname = argv[2];
	}
	
	exit(0);
}

//connect
//gethostbyname
//h_addr
//pthreads for stdin to sock and stdout to sock
// chatsrv on tux3
// name is argv[1]
// specify machine and port
