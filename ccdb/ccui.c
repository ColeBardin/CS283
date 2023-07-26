#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
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
 Runs a given argument vector with execv
 @param argv argument vector to run on command line, appeneded with NULL to signal end of list
 */
void run(char **argv);

/**
 Prompts the user for parameters to add an entry to the database, runs the add program
 */
void add();

/**
 Prompts the user for ID number to delete, runs delete program
 */
void delete();

/**
 Prompts the user for ID number to edit, runs edit program
 */
void edit();

/**
 Prompts the user for ID numner to display, runs the item program
 */
void item();

/**
 Runs the list program
 */
void list();

/**
 Prompts the user for a keyword to match, runs the match program
 */
void match();

/**
 Prompts the user for start and stop years, runs the year program
 */
void year();

int main(int argc, char **argv){
	char responseBuf[32];
	int ret;	

	ret = 0;
	printf("Classic Computer Database\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	// Print menu and process response until user quits
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
	// Read user input and handle command
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
	char id[16];
	char name[Nname];
	char maker[Nmaker];
	char cpu[Ncpu];
	char year[16];
	char desc[Ndesc];
	int ret;
	char *argv[8];

	// Get input parameters from user
	printf("Enter ID number for new item or use -a flag to automatically assign ID:\n");
	ret = scanf("%s", id);
	if(ret != 1){
		printf("Failed to read input\n");
		return;
	}
	printf("Enter item Name:\n");
	ret = scanf("%s", name);
	if(ret != 1){
		printf("Failed to read input:\n");
		return;
	}
	printf("Enter item Maker:\n");
	ret = scanf("%s", maker);
	if(ret != 1){
		printf("Failed to read input\n");
		return;
	}
	printf("Enter item CPU:\n");
	ret = scanf("%s", cpu);
	if(ret != 1){
		printf("Failed to read input\n");
		return;
	}
	printf("Enter item Year:\n");
	ret = scanf("%s", year);
	if(ret != 1){
		printf("Failed to read input\n");
		return;
	}
	printf("Enter item Description:\n");
	ret = scanf("%s", desc);
	if(ret != 1){
		printf("Failed to read input\n");
		return;
	}

	// Build argv and run it
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

	// Get ID from user
	printf("Enter the ID number to delete:\n");
	ret = scanf("%s", id);
	if(ret != 1){
		printf("Invalid input\n");
		return;
	}
	// Build argv and run it
	argv[0] = "./ccdel";
	argv[1] = id;
	argv[2] = NULL;
	run(argv);
}

void edit(){
	int ret;
	char id[16];
	char *argv[3];

	// Get ID from user
	printf("Enter the ID number to edit:\n");
	ret = scanf("%s", id);
	if(ret != 1){
		printf("Invalid input\n");
		return;
	}
	// Build argv and run it
	argv[0] = "./ccedit";
	argv[1] = id;
	argv[2] = NULL;
	run(argv);
}

void item(){
	int ret;
	char id[16];
	char *argv[3];

	// Get ID from user
	printf("Enter the ID number to view:\n");
	ret = scanf("%s", id);
	if(ret != 1){
		printf("Invalid input\n");
		return;
	}
	// Build argv and run it
	argv[0] = "./ccitem";
	argv[1] = id;
	argv[2] = NULL;
	run(argv);
}

void list(){
	char *argv[2];

	// Build argv and run it
	argv[0] = "./cclist";
	argv[1] = NULL;
	run(argv);
}

void match(){	
	int ret;
	char word[32];
	char *argv[3];

	// Get keyword to search for from user
	printf("Enter keyword to search for in DB:\n");
	ret = scanf("%s", word);
	if(ret != 1){
		printf("Invalid input\n");
		return;
	}
	// Build argv and run it
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

	// Get start and stop year from user
	printf("Enter start and stop years separated by a space:\n");
	ret = scanf("%s %s", start, stop);
	if(ret != 2){
		printf("Invalid input\n");
		return;
	}
	// Build argv and run it
	argv[0] = "./ccyear";
	argv[1] = start;
	argv[2] = stop;
	argv[3] = NULL;
	run(argv);
}

void run(char **argv){
	pid_t CHPID;
	int state;

	// Create child process and run argument vector
	CHPID = fork();
	if(CHPID == 0){
		execv(argv[0], argv);
	}else{
		wait(&state);
	}
}

