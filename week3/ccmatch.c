#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/file.h>
#include <sys/stat.h>
#include "cc.h"

/**
 Determines if str1 is a substring of str1
 @param str1 target string
 @param str2 string to search in 
 @return 1 if str1 is a substring of str2, 0 otherwise
 */
int isSubstr(char *str1, char *str2);

/**
 Checks if any attributes of CComp contain a substring
 @param comp CComp struct to search through
 @param match string to search for in comp attributes
 @return 1 if any attributes of comp contain match, 0 otherwise
 */
int isMatch(CComp *comp, char *match);

int main(int argc, char *argv[]){
	CComp comp;
	struct stat buffer;
	FILE *fp;
	char *match;
	int i;
	
	// Handle command line arguments from user
	if(argc != 2){
		fprintf(stderr, "Usage: ccmatch str\n");
		exit(1);
	}
	match = argv[1];
	
	fp = fopen("ccdb", "r+");
	if(fp == NULL){
		if(errno == ENOENT){
			fp = fopen("ccdb", "w+");
			if(fp == NULL){
				perror("fopen");
				exit(2);
			}
		}
	}
	flock(fileno(fp), LOCK_EX);
	fstat(fileno(fp), &buffer);
	if(buffer.st_size == 0){
		fprintf(stderr, "No items in database to search\n");
		exit(3);	
	}
	// Try each entry in ccdb for any matches. Print out details if match is found
	for(i = 1; i < buffer.st_size / sizeof(CComp); i++){
		fseek(fp, i * sizeof(CComp), SEEK_SET);
		fread(&comp, sizeof(CComp), 1, fp);	
		if(isMatch(&comp, match)){
			printf("\n");
			printf("Name: %s\n", comp.name);
			printf("Maker: %s\n", comp.maker);
			printf("CPU: %s\n", comp.cpu);
			printf("Year: %d\n", comp.year);
			printf("ID: %d\n", comp.id);
			printf("Desc: %s\n", comp.desc);
			printf("----------------\n");
		}
	}

	flock(fileno(fp), LOCK_UN);	
	fclose(fp);
	exit(0);
}

int isSubstr(char *str1, char *str2){
	int i, n;
	
	// Check for substrings of str1 in str2
	n = strlen(str1);
	for(i = 0; i < strlen(str2); i++){
		if(str2[i] == str1[0]){
			if(!strncmp(str1, &str2[i], n)){
				return 1;
			}
		}
	}
	return 0;
}

int isMatch(struct CComp *comp, char *match){
	// Test all attriutes of comp for matches
	if(isSubstr(match, comp->maker)){
		return 1;
	}
	if(isSubstr(match, comp->cpu)){
		return 1;
	}
	if(isSubstr(match, comp->desc)){
		return 1;
	}
	return 0;
}

