#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "dsh.h"

#define MAXTOKS 128
#define BUFSIZE 256

int tokenize(char *s, char *toks[], int maxtoks);
int exitCmd(char *argv[]);
int cdCmd(char *argv[]);
int pwdCmd(char *argv[]);

Command cmds[] = {
	{"exit", &exitCmd},
	{"cd", &cdCmd},
	{"pwd", &pwdCmd}
};
const int Ncmds = sizeof(cmds) / sizeof(Command);
char line[BUFSIZE];

int main(void){
	char *curDir;
	int cmd;
	int n;
	char *toks[MAXTOKS];
	pid_t CHPID;
	int state;

	while(1){
		getcwd(line, BUFSIZE);
		curDir = strrchr(line, '/');
		if(strlen(curDir) != 1){
			curDir++;
		}
		
		printf("%s D$ ", curDir);
		if(fgets(line, BUFSIZE, stdin) == NULL){
			exit(0);
		}
		
		n = tokenize(line, toks, MAXTOKS);	
		if(n == 0) return -1;

		for(cmd = 0; cmd < Ncmds && strncmp(toks[0], cmds[cmd].name, strlen(toks[0])); cmd++);
		if(cmd < Ncmds){
			cmds[cmd].f(toks);
			if(cmd == 0){
				exit(0);
			}
			continue;
		}

		// Create child process and run argument vector
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
	toks[i] = strtok(s, " ");
	while(toks[i] != NULL){
		p = strchr(toks[i], '\n');
		if(p != NULL){
			*p = '\0';
		}
		i++;
		if(i >= maxtoks - 1){
			toks[i] = NULL;
		}else{
			toks[i] = strtok(NULL, " ");
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
	if(argv[1] == NULL || strlen(argv[1]) == 0){
		p = getenv("HOME");
	}else{
		p = strchr(argv[1], '\n');
		if(p != NULL){
			*p = '\0';
		}
		p = argv[1];
	}
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
