#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include "dsh.h"

Command cmds[] = {
	{"exit", &exitCmd},
	{"cd", &cdCmd},
};

int main(void){
	char line[BUFSIZE];
	char *curDir, *p;
	int i, len;

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
			putchar('\n');
			exitCmd(0, NULL);			
			exit(0);
		}
		// Remove newline from line
		p = strchr(line, '\n');
		if(p != NULL) *p = '\0';
	
		// Ingore leading whitespace from string
		len = strlen(line);
		for(i = 0; i < len; i++){
			if(line[i] != ' ' && line[i] != '\t'){
				p = &line[i];
				break;	
			}
		}	
		// Skip empty lines
		if(i == len) continue;

		if(handlePipes(p) == 0) break;
	}
	
	exit(0);	
}

int handlePipes(char *line){
	int ret, nToks, i, nPipes;
	char *toks[MAXTOKS];	
	char *p;
	int pipes[MAXTOKS][2];
	int fdi, fdo;

	fdi = -1;
	fdo = -1;

	nToks = tokenize(line, toks, MAXTOKS, "|");
	nPipes = nToks - 2;
	if(nPipes < 0) nPipes = 0;

	// Open pipes
	for(i = 0; i < nPipes; i++){
		pipe(pipes[i]);
	}
		
	for(i = 0; i < nToks - 1; i++){
		if(i == 0) fdi = -1;
		else fdi = pipes[i-1][0]; // pipes[i-1] out

		if(i == nToks - 2) fdo = -1;
		else fdo = pipes[i][1]; // pipes[i] in

		ret = handleRedir(toks[i], fdi, fdo);
		if(ret == 0) break;
	}

	// Close all pipes
	for(i = 0; i < nPipes; i++){
		close(pipes[i][0]);
		close(pipes[i][1]);
	}
	return ret;
}

int handleRedir(char *line, int fdi, int fdo){
	char *loc, *p;
	char buf[BUFSIZE];
	int i, j, len, truncate, flag;

	// Ingore leading whitespace from string
	len = strlen(line);
	for(i = 0; i < len; i++){
		if(line[i] != ' ' && line[i] != '\t'){
			p = &line[i];
			break;	
		}
	}	
	// Skip empty lines
	if(i == len) return -1;

	// Search for input redir
	loc = strchr(line, '<');
	if(loc != NULL){
		p = loc++;
		// Set loc ptr to first non whitespace character after <
		while(*loc == ' ' || *loc == '\t') loc++;
		len = strlen(loc);
		// Find end of argument for redir
		for(i = 0; i < len; i++){
			if(loc[i] == '\t' || loc[i] == '>' || loc[i] <= ' ' || loc[i] > '~') break;
		}
		// Copy filename into buffer
		memcpy(buf, loc, i);
		buf[i] = '\0';
		// Remove redir from line
		memset(p, ' ', loc - p + i);

		// Give priority to pipes
		if(fdi == -1){
			fdi = open(buf, O_RDONLY);
			if(fdi == -1){
				fprintf(stderr, "Error opening '%s': ", buf);
				perror("");
				return -1;
			}
		}
	}

	// Search for output redir
	loc = strchr(line, '>');
	if(loc != NULL){
		truncate = 1; 
		p = loc++;
		// Set loc ptr to first non whitespace character after >, account for appending case 
		len = strlen(loc);
		for(i = 0; i < len; i++){
			if(loc[i] == '>'){
				if(truncate == 1){
					truncate = 0;
					continue;
				}else{
					fprintf(stderr, "Error: More than 2 consecutive > characters\n");
					return -1;
				}
			}
			if(loc[i] != ' ' && loc[i] != '\t') break;
		}
		loc += i;
		len = strlen(loc);
		// Find end of argument for redir
		for(i = 0; i < len; i++){
			if(loc[i] == '\t' || loc[i] == '<' || loc[i] <= ' ' || loc[i] > '~') break;
		}
		// Copy filename into buffer
		memcpy(buf, loc, i);
		buf[i] = '\0';
		// Remove redir from line
		memset(p, ' ', loc - p + i);
	
		if(truncate){
			flag = O_WRONLY | O_CREAT | O_TRUNC;
		}else{
			flag = O_WRONLY | O_CREAT | O_APPEND;
		}
		// Give priority to pipes
		if(fdo == -1){
			fdo = open(buf, flag, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
			if(fdo == -1){
				fprintf(stderr, "Error opening '%s': ", buf);
				perror("");
				return -1;
			}
		}
	}
	
	p = strchr(line, '>');
	if(p != NULL){
		fprintf(stderr, "Error: Too many output redirections\n");
		return -1;
	}
	p = strchr(line, '<');
	if(p != NULL){
		fprintf(stderr, "Error: Too many input redirections\n");
		return -1;
	}

	i = handleCmd(line, fdi, fdo);	
	close(fdi);
	close(fdo);
	return i;
}

int handleCmd(char *line, int fdi, int fdo){
	static const int Ncmds = sizeof(cmds) / sizeof(Command);
	int cmd, n;
	char *toks[MAXTOKS];
	
	// Tokenize command
	n = tokenize(line, toks, MAXTOKS, " \t");	
	if(n == 0) return -1;

	// Search through builtins or execute arbitrary command
	for(cmd = 0; cmd < Ncmds && strcmp(toks[0], cmds[cmd].name); cmd++);
	if(cmd != Ncmds) cmds[cmd].f(n, toks);
	else runCmd(n, toks, fdi, fdo);

	return cmd;
}

int tokenize(char *s, char *toks[], int maxtoks, char *delim){
	int i;
	char *p;

	i = 0;
	toks[i] = strtok(s, delim); 
	while(toks[i++] != NULL){
		if(i >= maxtoks - 1){
			toks[i] = NULL;
		}else{
			toks[i] = strtok(NULL, delim);
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

	if(argc == 1 || argv[1] == NULL || argv[1][0] == '\0'){
		// Empty argument: go to home directory
		p = getenv("HOME");
	}else{
		p = argv[1];
	}
	// Change dir
	if(chdir(p) < 0){
		perror(argv[0]);
	}
	return 0;
}

int runCmd(int argc, char *argv[], int fdi, int fdo){
	pid_t CHPID;
	int state;

	CHPID = fork();
	if(CHPID == 0){
		if(fdi != -1){
			dup2(fdi, 0);
			close(fdi);
		}
		if(fdo != -1){
			dup2(fdo, 1);
			dup2(fdo, 2);
			close(fdo);
		}

		execvp(argv[0], argv);
		printf("Command not found: %s\n", argv[0]);
		exit(1);
	}else{
		wait(&state);
	}
	return 0;
}

