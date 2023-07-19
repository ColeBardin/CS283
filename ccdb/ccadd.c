#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/file.h>
#include <sys/stat.h>
#include "cc.h"

/**
 Gets the lowest unused ID number. Overwrites deleted items with id=0 or appends to end of DB.
 @param fp FILE pointer to opened ccdb file
 @return ID number to use
 */
int getNewId(FILE* fp);

int main(int argc, char *argv[]){
	CComp newcomp;
	FILE *fp;

	if(argc != 7) {
		fprintf(stderr, "Usage: ccadd [id | -a] name maker cpu year desc\n");
		exit(1);
	}
	// Copy command line arguments into the new struct, except ID
	strncpy(newcomp.name, argv[2], Nname-1);
	newcomp.name[Nname-1] = '\0';
	strncpy(newcomp.maker, argv[3], Nmaker-1);
	newcomp.maker[Nmaker-1] = '\0';
	strncpy(newcomp.cpu, argv[4], Ncpu-1);
	newcomp.cpu[Ncpu-1] = '\0';
	newcomp.year = atoi(argv[5]);
	strncpy(newcomp.desc, argv[6], Ndesc-1);
	newcomp.desc[Ndesc-1] = '\0';

	// Open ccdb file
	fp = fopen("ccdb", "r+");
	if(fp == NULL) {
		if(errno == ENOENT) {
			fp = fopen("ccdb", "w+");
			if(fp == NULL ) {
				perror("fopen");
				exit(2);
			}
		}
	}
	flock(fileno(fp), LOCK_EX);
	// If -a flag is given, find largest ID being used
	if(!strncmp(argv[1], "-a", Nname-1)){
		newcomp.id = getNewId(fp);
	}else{
		newcomp.id = atoi(argv[1]);
	}

	// Write new data to file
	fseek(fp, newcomp.id * sizeof(CComp), SEEK_SET);
	fwrite(&newcomp, sizeof(CComp), 1, fp);

	flock(fileno(fp), LOCK_UN);
	fclose(fp);
	exit(0);
}

int getNewId(FILE* fp){
	CComp item;
	struct stat buffer;
	int id, fstatid;

	// Use fstat to calculate next highest ID number
	fstat(fileno(fp), &buffer);
	fstatid = buffer.st_size / sizeof(CComp);
	if(fstatid == 0){
		fstatid = 1;
	}

	id = 1;
	// Search through file for deleted item (marked w/ id=0)
	fseek(fp, sizeof(CComp), SEEK_SET);
	while(fread(&item, sizeof(CComp), 1, fp) > 0){
		if(item.id == 0){
			break;
		}
		id++;
	}

	// Use the id of delted item if found, else append item at end of db
	if(id < fstatid){
		return id;
	}else{
		return fstatid;
	}
}

