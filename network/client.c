#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netdb.h>

void *doSend(void *x);
void *doRecv(void *x);

int main(int argc, char **argv){
	struct sockaddr_in caddr;
	char *screenname, *hostname;
	int sock, port, addrlen;
	int sendTID, recvTID;
	struct hostent *host;
	

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
	
	host = gethostbyname(hostname);
	printf("%s -> %d\n", hostname, *host->h_addr);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0){
		perror("Socket");
		exit(1);
	}
	caddr.sin_family = AF_INET;
	caddr.sin_port = htons(port);
	caddr.sin_addr.s_addr = *host->h_addr;

	addrlen = sizeof(struct sockaddr_in);	
	if(bind(sock, (struct sockaddr *)&caddr, addrlen) < 0){
		perror("Bind");
		exit(1);
	}
	
	if(connect(sock, (struct sockaddr *)&caddr, addrlen)){
		perror("Connect");
		exit(1);
	}	
	pthread_create((pthread_t *)&sendTID, NULL, doSend, &sock);
	//pthread_create((pthread_t *)&recvTID, NULL, doRecv, &sock);
		
	exit(0);
}

void *doSend(void *x){
	int sock = *(int *)x;
	
	return NULL;
}

void *doRecv(void *x){
	int sock = *(int *)x;
	
	return NULL;
}

//socket
//bind
//connect
//gethostbyname
//h_addr
//pthreads for stdin to sock and stdout to sock
// chatsrv on tux3
// name is argv[1]
// specify machine and port
// shutdown
