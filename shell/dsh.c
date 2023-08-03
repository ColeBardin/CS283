#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "dsh.h"

#define MAXTOKS 128
#define BUFSIZE 256

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
 @param argv Argument vector
 @retun 0
 */
int exitCmd(char *argv[]);

/**
 Funciton body for cd builtin command.
 @param argv Argument vector
 @return 0
 */
int cdCmd(char *argv[]);

/**
 Function body for pwd builtin command.
 @param argv Argument vector
 @return 0
 */
int pwdCmd(char *argv[]);

Command cmds[] = {
	{"exit", &exitCmd},
	{"cd", &cdCmd},
	{"pwd", &pwdCmd}
};
const int Ncmds = sizeof(cmds) / sizeof(Command);
char line[BUFSIZE];

int main(void){
	int cmd;
	int n;
	int state;
	char *toks[MAXTOKS];
	char *curDir;
	pid_t CHPID;

	while(1){
		// Build basic prompt
		getcwd(line, BUFSIZE);
		curDir = strrchr(line, '/');
		if(strlen(curDir) != 1){
			curDir++;
		}
		printf("%s D$ ", curDir);

		// Read and tokenie user input
		if(fgets(line, BUFSIZE, stdin) == NULL){
			exit(0);
		}
		// Skip empty lines
		if(line[0] == '\n') continue;
		n = tokenize(line, toks, MAXTOKS);	

		// Search through builtins and execute if found
		for(cmd = 0; cmd < Ncmds && strncmp(toks[0], cmds[cmd].name, strlen(toks[0])); cmd++);
		if(cmd < Ncmds){
			cmds[cmd].f(toks);
			if(cmd == 0){
				exit(0);
			}
			continue;
		}

		// If not a builtin, fork and exec arbitrary command
		CHPID = fork();
		if(CHPID == 0){
			if(execvp(toks[0], toks) == -1){
				printf("Command not found: %s\n", toks[0]);
				exit(1);
			}
		}else{
			wait(&state);
		}
	}
	
	exit(0);	
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

int exitCmd(char *argv[]){
	printf("[Exiting session]\n");
	return 0;
}

int cdCmd(char *argv[]){
	char *p;
	if(argv[1] == NULL || argv[1][0] == '\0'){
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

int pwdCmd(char *argv[]){
	getcwd(line, BUFSIZE);
	printf("%s\n", line);
	return 0;
}

