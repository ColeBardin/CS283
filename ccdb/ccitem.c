#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include "cc.h"

int
main(int argc, char *argv[])
{
	CComp item;
	FILE *fp;
	int n, inum;

	if(argc != 2) {
		fprintf(stderr, "Usage: ccitem num\n");
		exit(1);
	}

	fp = fopen("ccdb", "r");
	if(fp == NULL) {
		perror("fopen");
		exit(2);
	}
	flock(fileno(fp), LOCK_EX);
	inum = atoi(argv[1]);
	fseek(fp, inum * sizeof(CComp), SEEK_SET);
	n = fread(&item, sizeof(CComp), 1, fp);
	flock(fileno(fp), LOCK_UN);
	fclose(fp);
	if(n == 0 || item.id != inum) {
		fprintf(stderr, "No such item %d in database\n", inum);
		exit(3);
	}
	if(n < 0) {
		perror("fread");
		exit(3);
	}
	printf("Name: %s\n", item.name);
	printf("Maker: %s\n", item.maker);
	printf("CPU: %s\n", item.cpu);
	printf("Year: %d\n", item.year);
	printf("ID: %d\n", item.id);
	printf("Desc: %s\n", item.desc);
}
