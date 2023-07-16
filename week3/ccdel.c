#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/file.h>
#include "cc.h"

int main(int argc, char *argv[]){
	CComp oldcomp;
	FILE *fp;
	int id;
	
	// Handle command line arguments from user
	if(argc != 2){
		fprintf(stderr, "Usage: ccdel id\n");
		exit(1);
	}

	id = atoi(argv[1]);
	if(id == 0){
		fprintf(stderr, "ID number must be greater than 0\n");
		exit(1);
	} 
	
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
	// Attempt to read item with given ID
	fseek(fp, id * sizeof(CComp), SEEK_SET);
	if(fread(&oldcomp, sizeof(CComp), 1, fp) == 0){
		fprintf(stderr, "No such item %d in database\n", id);
		exit(3);
	}

	// Set ID to 0 to mark as deleted
	oldcomp.id = 0;

	// Write zeroed data to file
	fseek(fp, id * sizeof(CComp), SEEK_SET);
	fwrite(&oldcomp, sizeof(CComp), 1, fp);

	flock(fileno(fp), LOCK_UN);	
	fclose(fp);
	exit(0);
}

