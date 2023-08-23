#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

typedef struct Client Client;
struct Client {
	pthread_t tid;
	int sock;
	Client *next;
};


int main(int argc, char **argv){
	if(argc < 2){
		fprintf(stderr, "Error: no screen name provided\n");
		exit(1);
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
