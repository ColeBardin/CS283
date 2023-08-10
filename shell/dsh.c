#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "dsh.h"

#define MAXTOKS 128
#define BUFSIZE 1024

int handleRedir(char *line);
int handleCmd(char *line);

/**
 Tokenizes a string by whitespace.
 @param s String to tokenize
 @param toks buffer where tokens will be stored
 @param maxtoks size of toks array
 @return number of tokens found
 */
int tokenize(char *s, char *toks[], int maxtoks);

/**
 Function body for exit builtin command.
 @param argc Number of arguments
 @param argv Argument vector
 @retun 0
 */
int exitCmd(int argc, char *argv[]);

/**
 Funciton body for cd builtin command.
 @param argc Number of arguments
 @param argv Argument vector
 @return 0
 */
int cdCmd(int argc, char *argv[]);

/**
 Runs arbitrary command that is not a built in
 @param argc Number of arguments
 @param argv Argument vector
 @return 0
 */
int runCmd(int argc, char *argv[]);

Command cmds[] = {
	{"exit", &exitCmd},
	{"cd", &cdCmd},
	{"", &runCmd}
};
const int Ncmds = sizeof(cmds) / sizeof(Command);
char line[BUFSIZE];

int main(void){
	char *curDir;

	while(1){
		// Build basic prompt
		getcwd(line, BUFSIZE);
		curDir = strrchr(line, '/');
		if(strlen(curDir) != 1){
			curDir++;
		}
		printf("%s D$ ", curDir);

		// Read user's input
		if(fgets(line, BUFSIZE, stdin) == NULL){
			exit(0);
		}
		// Skip empty lines
		if(line[0] == '\n') continue;
	
		if(handleRedir(line) == 0) break;
	}
	
	exit(0);	
}

int handleRedir(char *line){
		
	return handleCmd(line);	
}

int handleCmd(char *line){
	int cmd;
	int n;
	char *toks[MAXTOKS];
	
	// Tokenize command
	n = tokenize(line, toks, MAXTOKS);	

	// Search through builtins or execute arbitrary command
	for(cmd = 0; cmd < Ncmds - 1 && strncmp(toks[0], cmds[cmd].name, strlen(toks[0])); cmd++);
	cmds[cmd].f(n, toks);

	return cmd;
}

int tokenize(char *s, char *toks[], int maxtoks){
	int i;
	char *p;

	i = 0;
	// Tokenize by whitespace
	toks[i] = strtok(s, " \t");
	while(toks[i] != NULL){
		p = strchr(toks[i], '\n');
		if(p != NULL){
			*p = '\0';
		}

		i++;
		if(i >= maxtoks - 1){
			toks[i] = NULL;
		}else{
			toks[i] = strtok(NULL, " \t");
		}
	}
	return i;
}

int exitCmd(int argc, char *argv[]){
	printf("[Exiting session]\n");
	return 0;
}

int cdCmd(int argc, char *argv[]){
	char *p;
	if(argc == 1 || argv[1][0] == '\0'){
		// Empty argument: go to home directory
		p = getenv("HOME");
	}else{
		// Remove newline from arg
		p = strchr(argv[1], '\n');
		if(p != NULL){
			*p = '\0';
		}
		p = argv[1];
	}
	// Change dir
	if(chdir(p) < 0){
		perror(line);
	}
	return 0;
}

int runCmd(int argc, char *argv[]){
	pid_t CHPID;
	int state;
	int n;

	CHPID = fork();
	if(CHPID == 0){
		execvp(argv[0], argv);
		printf("Command not found: %s\n", argv[0]);
		exit(1);
	}else{
		wait(&state);
	}
	return 0;
}

