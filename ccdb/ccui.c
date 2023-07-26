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

/**
 Runs the argument vector in a child process and waits for it to complete
 @param argv argument vector to run, appended with NULL to mark end
 */
void run(char **argv);

/**
 Gets users input to add a new item, runs ccadd program
 */
void add();

/**
 Gets ID from user and runs ccdel program to delete the item
 */
void delete();

/**
 Gets ID from user and runs ccedit program to edit the item
 */
void edit();

/** 
 gets ID from user and runs ccitem program to display the item
 */
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
	}
	return 0;
}

void add(){
	char id[16];
	char name[Nname];
	char maker[Nmaker];
	char cpu[Ncpu];
	char year[16];
	char desc[Ndesc];
	int ret;
	char *argv[8];

	printf("Enter new item details in following format:\n");
	printf("[id | -a] name maker cpu year desc\n");
	ret = scanf("%s %s %s %s %s %s", id, name, maker, cpu, year, desc);
	if(ret != 6){
		printf("Invalid input\n");
		return;
	}
	argv[0] = "./ccadd";
	argv[1] = id;
	argv[2] = name;
	argv[3] = maker;
	argv[4] = cpu;
	argv[5] = year;
	argv[6] = desc;
	argv[7] = NULL;
	run(argv);
}

void delete(){
	int ret;
	char id[16];
	char *argv[3];

	printf("Enter the ID number to delete:\n");
	ret = scanf("%s", id);
	if(ret != 1){
		printf("Invalid input\n");
		return;
	}
	argv[0] = "./ccdel";
	argv[1] = id;
	argv[2] = NULL;
	run(argv);
}

void edit(){
	int ret;
	char id[16];
	char *argv[3];

	printf("Enter the ID number to edit:\n");
	ret = scanf("%s", id);
	if(ret != 1){
		printf("Invalid input\n");
		return;
	}
	argv[0] = "./ccedit";
	argv[1] = id;
	argv[2] = NULL;
	run(argv);
}

void item(){
	int ret;
	char id[16];
	char *argv[3];

	printf("Enter the ID number to view:\n");
	ret = scanf("%s", id);
	if(ret != 1){
		printf("Invalid input\n");
		return;
	}
	argv[0] = "./ccitem";
	argv[1] = id;
	argv[2] = NULL;
	run(argv);
}

void list(){
	char *argv[2];

	argv[0] = "./cclist";
	argv[1] = NULL;
	run(argv);
}

void match(){	
	int ret;
	char word[32];
	char *argv[3];

	printf("Enter keyword to search for in DB:\n");
	ret = scanf("%s", word);
	if(ret != 1){
		printf("Invalid input\n");
		return;
	}
	argv[0] = "./ccmatch";
	argv[1] = word;
	argv[2] = NULL;
	run(argv);
}

void year(){
	int ret;
	char start[16];
	char stop[16];
	char *argv[4];

	printf("Enter start and stop years separated by a space:\n");
	ret = scanf("%s %s", start, stop);
	if(ret != 2){
		printf("Invalid input\n");
		return;
	}
	argv[0] = "./ccyear";
	argv[1] = start;
	argv[2] = stop;
	argv[3] = NULL;
	run(argv);
}

void run(char **argv){
	pid_t CHPID;
	int state;

	CHPID = fork();
	if(CHPID == 0){
		execv(argv[0], argv);
	}else{
		wait(&state);
	}
}

