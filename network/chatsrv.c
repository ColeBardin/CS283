#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>

typedef struct Client Client;

struct Client {
	int sock;
	pthread_t tid;
	char name[16];
	Client *next, *prev;
};

void *client(void *);

Client *chead;
pthread_mutex_t client_lock;

int
main()
{
	int sock;
	pthread_t tid;
	struct sockaddr_in listener;
	Client *c;

	pthread_mutex_init(&client_lock, NULL);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	listener.sin_family = AF_INET;
	listener.sin_port = htons(2002);
	listener.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(sock, (struct sockaddr *)&listener, sizeof(struct sockaddr_in)) < 0) {
		perror("bind");
		exit(1);
	}
	while(1) {
		if(listen(sock, 5) < 0) {
			perror("listen");
			exit(2);
		}
		c = malloc(sizeof(Client));
		c->sock = accept(sock, NULL, NULL);
		if(c->sock < 0) {
			perror("accept");
			free(c);
		}
		else {
			pthread_create(&tid, NULL, client, c);
		}
	}
}

void
cadd(Client *c)
{
	pthread_mutex_lock(&client_lock);
	c->prev = NULL;
	if(chead != NULL) {
		chead->prev = c;
	}
	c->next = chead;
	chead = c;
	pthread_mutex_unlock(&client_lock);
}

void
cdel(Client *c)
{
	if(chead == c) {
		chead = c->next;
	}
	if(c->next != NULL) {
		c->next->prev = c->prev;
	}
	if(c->prev != NULL) {
		c->prev->next = c->next;
	}
}

void *
client(void *p)
{
	Client *c = p;
	Client *q, *r;
	char buf[1024];
	int n, prefix;

	c->tid = pthread_self();
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &n);
	n = recv(c->sock, buf, 1024, 0);
	if(n <= 0) {
		free(c);
		pthread_exit(NULL);
	}
	if(n > 15)
		n = 15;
	if(buf[n-1] == '\n')
		n--;
	strncpy(c->name, buf, n);
	c->name[n] = '\0';
	fprintf(stderr, "%s connected\n", c->name);
	prefix = sprintf(buf, "%s> ", c->name);
	cadd(c);
	while(1) {
		n = recv(c->sock, buf + prefix, 1024 - prefix, 0);
		if(n <= 0) {
			pthread_mutex_lock(&client_lock);
			cdel(c);
			pthread_mutex_unlock(&client_lock);
			free(c);
			pthread_exit(NULL);
		}
		pthread_mutex_lock(&client_lock);
		for(q = chead; q != NULL; q = q->next) {
			if(q != c) {
				if(send(q->sock, buf, n + prefix, 0) <= 0) {
					fprintf(stderr, "%s left\n", q->name);
					pthread_cancel(q->tid);
					cdel(q);
					r = q;
					q = q->next;
					free(r);
				}
			}
		}
		pthread_mutex_unlock(&client_lock);
	}
}
