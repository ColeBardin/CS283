#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/file.h>
#include <sys/stat.h>
#include "cc.h"

int main(int argc, char *argv[]){
	CComp comp;
	struct stat buffer;
	FILE *fp;
	int start, stop;
	int i;
	
	// Handle user arguments
	if(argc != 3){
		fprintf(stderr, "Usage: ccyear startyr stopyr\n");
		exit(1);
	}
	start = atoi(argv[1]);
	stop = atoi(argv[2]);
	
	// Open file
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
	// Use fstat to calculate the number of items
	fstat(fileno(fp), &buffer);
	if(buffer.st_size == 0){
		fprintf(stderr, "No items in database to search\n");
		exit(0);	
	}
	// Iterate through each item to see if its year is within the desired range
	for(i = 1; i < buffer.st_size / sizeof(CComp); i++){
		fseek(fp, i * sizeof(CComp), SEEK_SET);
		fread(&comp, sizeof(CComp), 1, fp);	
		if(i == comp.id){
			if(comp.year >= start && comp.year <= stop){
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
	}

	flock(fileno(fp), LOCK_UN);	
	fclose(fp);
	exit(0);
}

