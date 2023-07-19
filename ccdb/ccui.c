#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "cc.h"

/**
 Prints out the menu and the options
 */
void printMenu();
/**
 Reads the user reponse handles it
 @return -1 when user enters quit option, 0 otherwise
 */
int getResponse(char *buf);

void add();
void delete();
void edit();
void item();
void list();
void match();
void year();

int main(int argc, char **argv){
	char responseBuf[16];
	int ret;	

	ret = 0;
	printf("Classic Computer Database\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	while(ret != -1){
		printMenu();
		ret = getResponse(responseBuf);
		printf("\n");
	}
	exit(0);
}

void printMenu(){
	printf("Select an option:\n");
	printf("(A) Add entry\n");
	printf("(D) Delete entry\n");
	printf("(E) Edit entry\n");
	printf("(I) Print item\n");
	printf("(L) List all entries\n");
	printf("(M) Find items with matching words\n");
	printf("(Q) Quit UI\n");
	printf("(Y) Find items in given range of years\n");
	return;
}

int getResponse(char *buf){
	scanf("%s", buf);
	switch(buf[0]){
		case 'a':
		case 'A':
			add();
			break;
		case 'd':
		case 'D':
			delete();
			break;
		case 'e':
		case 'E':
			edit();
			break;
		case 'i':
		case 'I':
			item();
			break;
		case 'l':
		case 'L':
			list();
			break;
		case 'm':
		case 'M':
			match();
			break;
		case 'q':
		case 'Q':
			return -1;
		case 'y':
		case 'Y':
			year();
			break;
		default:
			printf("Unrecognized input. Try again\n");
			return getResponse(buf);
			break;
	}
	return 0;
}

void add(){
	pid_t CHPID;
	int state;
	char id[16];
	char name[Nname];
	char maker[Nmaker];
	char cpu[Ncpu];
	char year[16];
	char desc[Ndesc];
	int ret;

	printf("Enter new item details in following format:\n");
	printf("[id | -a] name maker cpu year desc\n");
	ret = scanf("%s %s %s %s %s %s", id, name, maker, cpu, year, desc);
	if(ret != 6){
		printf("Invalid input\n");
		return;
	}
	CHPID = fork();
	if(CHPID == 0){
		execl("./ccadd", "./ccadd", id, name, maker, cpu, year, desc, NULL);
	}else{
		wait(&state);
	}
}

void delete(){
	int ret;
	int state;
	pid_t CHPID;
	char id[16];

	printf("Enter the ID number to delete:\n");
	ret = scanf("%s", id);
	if(ret != 1){
		printf("Invalid input\n");
		return;
	}
	CHPID = fork();
	if(CHPID == 0){
		execl("./ccdel", "./ccdel", id, NULL);
	}else{
		wait(&state);
	}
}

void edit(){
	int ret;
	int state;
	pid_t CHPID;
	char id[16];

	printf("Enter the ID number to edit:\n");
	ret = scanf("%s", id);
	if(ret != 1){
		printf("Invalid input\n");
		return;
	}
	CHPID = fork();
	if(CHPID == 0){
		execl("./ccedit", "./ccedit", id, NULL);
	}else{
		wait(&state);
	}
}

void item(){

}

void list(){
	int state;
	pid_t CHPID;

	CHPID = fork();
	if(CHPID == 0){
		execl("./cclist", "./cclist", NULL); 
	}else{
		wait(&state);
	}
}

void match(){

}

void year(){

}

