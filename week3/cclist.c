#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include "cc.h"

int
main(int argc, char *argv[])
{
	CComp item;
	FILE *fp;
	int id;

	if(argc != 1) {
		fprintf(stderr, "Usage: cclist\n");
		exit(1);
	}
	fp = fopen("ccdb", "r");
	if(fp == NULL) {
		perror("fopen");
		exit(2);
	}
	flock(fileno(fp), LOCK_EX);
	id = 1;
	fseek(fp, sizeof(CComp), SEEK_SET);
	while(fread(&item, sizeof(CComp), 1, fp) > 0) {
		if(id == item.id) {
			printf("\n");
			printf("Name: %s\n", item.name);
			printf("Maker: %s\n", item.maker);
			printf("CPU: %s\n", item.cpu);
			printf("Year: %d\n", item.year);
			printf("ID: %d\n", item.id);
			printf("Desc: %s\n", item.desc);
			printf("----------------\n");
		}
		id++;
	}
	flock(fileno(fp), LOCK_UN);
	fclose(fp);
	exit(0);
}
