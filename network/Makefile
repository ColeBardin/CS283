all: client server

client: client.c
	cc -o client client.c

server: chatsrv.c
	cc -o server chatsrv.c

clean:
	rm *.o .*.swp client server
