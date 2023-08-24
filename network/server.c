#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

void *doClient(void *arg);
typedef struct Client Client;
struct Client {
	pthread_t tid;
	int sock;
	Client *next;
};

Client *head = NULL;
pthread_mutex_t clientLock = PTHREAD_MUTEX_INITIALIZER;

int main(){
	int listensock, connsock, addrsize;
	struct sockaddr_in laddr;
	struct sockaddr_in caddr;
	Client *newClient;
	
	listensock = socket(PF_INET, SOCK_STREAM, 0);
	if(listensock < 0){
		perror("socket");
		exit(1);
	}
	
	laddr.sin_family = AF_INET;
	laddr.sin_port = htons(2002);
	laddr.sin_addr.s_addr = INADDR_ANY;
	
	if(bind(listensock, (struct sockaddr *)&laddr, sizeof(struct sockaddr_in)) < 0){
		perror("bind");
		exit(1);
	}
	
	if(listen(listensock, 5) < 0){
		perror("listen");
		exit(1);
	}
	
	addrsize = sizeof(struct sockaddr_in);
	while(1){
		connsock = accept(listensock, (struct sockaddr *)&caddr, (socklen_t *)&addrsize);	
		printf("port: %d\n", ntohs(caddr.sin_port));
		printf("addr: %s\n", inet_ntoa(caddr.sin_addr));
		newClient = malloc(sizeof(Client));
		pthread_mutex_lock(&clientLock);
		newClient->next = head;
		head = newClient;
		newClient->sock = connsock;
		pthread_create(&newClient->tid, NULL, doClient, newClient);
		pthread_mutex_unlock(&clientLock);
	}
	
	exit(0);	
}

void *doClient(void *arg){
	Client *c = arg;
	Client *p;
	char buf[256];
	int n, namelen;
	
	n = recv(c->sock, buf, 256, 0);
	if(buf[n-1] == '\n') n--;
	buf[n] = '>';
	buf[n+1] = ' ';
	namelen = n + 2;
	
	while(1){
		n = recv(c->sock, buf + namelen, 256 - namelen, 0);
		pthread_mutex_lock(&clientLock);
		for(p = head; p != NULL; p = p->next){
			if(c != p) send(p->sock, buf, n + namelen, 0);
		}	
		pthread_mutex_unlock(&clientLock);
	}

	return NULL;
}

