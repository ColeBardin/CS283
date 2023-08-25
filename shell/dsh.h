#pragma once

#define MAXTOKS 128
#define BUFSIZE 1024

typedef struct {
	char *name;
	int (*f)(int argc, char *argv[]);
} Command;

/**
 Handles opening and closing pipes.
 Calls handle redir on substrings between pipes.
 @param line current line to be processed
 @return Index of command run, or 0 if exit is called
 */
int handlePipes(char *line);

/**
 Handles truncating and appending output and input file redireciton.
 Parses nonwhitespace after input or output redirection characters.
 Opens the desired files and sets the global file descriptor values for runCmd.
 Then calls handleCmd.
 @param line current line to be processed
 @return Index of command run
 */
int handleRedir(char *line, int fdi, int fdo);

/**
 Tokenizes line by whitespace and tries to call builtin command. 
 If command is not recognized, the argument vector will be run.
 @param line current line to be processed
 @return Index of command run
 */
int handleCmd(char *line, int fdi, int fdo);

/**
 Tokenizes a string by whitespace.
 @param s String to tokenize
 @param toks buffer where tokens will be stored
 @param maxtoks size of toks array
 @return number of tokens found
 */
int tokenize(char *s, char *toks[], int maxtoks, char *delim);

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
int runCmd(int argc, char *argv[], int fdi, int fdo);

