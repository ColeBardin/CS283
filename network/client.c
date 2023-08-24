#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netdb.h>
#include <string.h>

typedef struct Thread Thread;
struct Thread{
	int sock;
	pthread_t tid;
};

void *doSend(void *x);
void *doRecv(void *x);

int pthread_fault;

int main(int argc, char **argv){
	struct sockaddr_in saddr;
	char *screenname, *hostname;
	int sock, port, addrlen, n;
	int sendTID, recvTID;
	struct hostent *host;
	Thread sendT;
	Thread recvT;
	
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
	
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0){
		perror("Socket");
		exit(1);
	}

	host = gethostbyname(hostname);
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	memcpy((char *)&saddr.sin_addr.s_addr, host->h_addr, sizeof(host->h_length));

	addrlen = sizeof(saddr);	

	printf("Hostname: %s\n", hostname);
	printf("Connecting to port: %d\n", port);

	if(connect(sock, (struct sockaddr *)&saddr, addrlen) < 0){
		perror("Connect");
		exit(1);
	}	
	
	printf("Connection successful\n");
	if(send(sock, screenname, strlen(screenname) + 1, 0) < 0){
		perror("Send");
		exit(1);
	}

	pthread_fault = 0;
	sendT.sock = sock;
	recvT.sock = sock;
	pthread_create(&sendT.tid, NULL, doSend, (void *)&sendT);
	pthread_create(&recvT.tid, NULL, doRecv, (void *)&recvT);
		
	pthread_join(sendT.tid, NULL);
	pthread_join(recvT.tid, NULL);

	shutdown(sock, SHUT_RDWR);
	exit(0);
}

void *doSend(void *x){
	Thread *t = x;
	char buf[256];

	while(pthread_fault == 0){
		if(fgets(buf, 256, stdin) == NULL) break;
		//if(buf[strlen(buf) - 1] == '\n') buf[strlen(buf) - 1] = '\0';
		if(send(t->sock, buf, strlen(buf) + 1, 0) < 0){
			perror("Pthread Send");
			break;
		}
	}

	pthread_fault = 1;
	pthread_exit(NULL);
	
	return NULL;
}

void *doRecv(void *x){
	Thread *t = x;
	char buf[256];

	while(pthread_fault == 0){
		if(recv(t->sock, buf, 256, 0) < 1){
			perror("Pthread Recv");
			break;
		}
		if(fputs(buf, stdout) == EOF){
			perror("fputs");
			break;
		}
	}
	
	pthread_fault = 1;
	pthread_exit(NULL);
	
	return NULL;
}

