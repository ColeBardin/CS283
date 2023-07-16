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
	// Find
	fseek(fp, id * sizeof(CComp), SEEK_SET);
	fread(&oldcomp, sizeof(CComp), 1, fp);

	memset(oldcomp.name, 0, Nname);
	memset(oldcomp.maker, 0, Nmaker);
	memset(oldcomp.cpu, 0, Ncpu);
	oldcomp.year = 0;
	oldcomp.id = 0;
	memset(oldcomp.desc, 0, Ndesc);

	fseek(fp, id * sizeof(CComp), SEEK_SET);
	fwrite(&oldcomp, sizeof(CComp), 1, fp);

	flock(fileno(fp), LOCK_UN);	
	fclose(fp);
}

