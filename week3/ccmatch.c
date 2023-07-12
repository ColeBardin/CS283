#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/file.h>
#include <sys/stat.h>
#include "cc.h"

int isSubstr(char *str1, char *str2);
int isMatch(struct CComp *comp, char *match);

int main(int argc, char *argv[]){
	CComp comp;
	struct stat buffer;
	FILE *fp;
	char *match;
	int i;
	
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
		exit(0);	
	}
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
}

int isSubstr(char *str1, char *str2){
	int i, n;
	
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




