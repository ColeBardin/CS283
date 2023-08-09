#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "dsh.h"

#define MAXTOKS 128
#define BUFSIZE 1024

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
 Function body for pwd builtin command.
 @param argc Number of arguments
 @param argv Argument vector
 @return 0
 */
int pwdCmd(int argc, char *argv[]);

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
	{"pwd", &pwdCmd},
	{"", &runCmd}
};
const int Ncmds = sizeof(cmds) / sizeof(Command);
char line[BUFSIZE];

int main(void){
	int cmd;
	int n;
	char *toks[MAXTOKS];
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
		n = tokenize(line, toks, MAXTOKS);	

		// Search through builtins or execute arbitrary command
		for(cmd = 0; cmd < Ncmds - 1 && strncmp(toks[0], cmds[cmd].name, strlen(toks[0])); cmd++);
		cmds[cmd].f(n, toks);
		if(cmd == 0){
			exit(0);
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

		// Parse token for redirect input command 
		p = strchr(toks[i], '<');
		if(p != NULL){
			*p = '\0';
			p++;
			// Keep chars before < as token
			if(strlen(toks[i]) != 0){
				if(++i >= maxtoks - 1){
					toks[i] = NULL;
					return i;
				}
			}
			toks[i] = "<";
			// Add chars after < as token
			if(strlen(p) != 0){
				if(++i >= maxtoks - 1){
					toks[i] = NULL;
					return i;
				}
				toks[i] = p;
			}
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

int pwdCmd(int argc, char *argv[]){
	getcwd(line, BUFSIZE);
	printf("%s\n", line);
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

