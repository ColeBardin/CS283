#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/file.h>
#include "cc.h"

/**
 Prompts user to change a field and saves response in destination string
 Will run recursively if y/n is not provided
 @param dest string used to store user's response
 @return 1 if the user decided to change the field, 0 otherwise
 */
int promptChange(char *dest);

int main(int argc, char *argv[]){
	CComp oldcomp;
	FILE *fp;
	int id, ret;
	char response[Ndesc];
	
	// Ensure there is an additional command line argument
	if(argc != 2){
		fprintf(stderr, "Usage: cceit id\n");
		exit(1);
	}

	if((id = atoi(argv[1])) <= 0){
		fprintf(stderr, "ID number must be greater than 0\n");
		exit(1);
	}
	
	// Open file
	fp = fopen("ccdb", "r+");
	if(fp == NULL){
		perror("fopen");
		exit(2);
	}
	flock(fileno(fp), LOCK_EX);
	// Read specified ID if valid
	fseek(fp, id * sizeof(CComp), SEEK_SET);
	ret = fread(&oldcomp, sizeof(CComp), 1, fp);
	if(ret == 0 || oldcomp.id != id){
		fprintf(stderr, "No such item %d in database\n", id);
		exit(3);
	}
	// Print out each attribute and prompt user for change
	printf("Name: %s\n", oldcomp.name);
	if(promptChange(response)){
		strncpy(oldcomp.name, response, Nname);
	}
	printf("Maker: %s\n", oldcomp.maker);
	if(promptChange(response)){
		strncpy(oldcomp.maker, response, Nname);
	}
	printf("CPU: %s\n", oldcomp.cpu);
	if(promptChange(response)){
		strncpy(oldcomp.cpu, response, Nname);
	}
	printf("Year: %d\n", oldcomp.year);
	if(promptChange(response)){
		oldcomp.year = atoi(response);
	}
	printf("Desc: %s\n", oldcomp.desc);
	if(promptChange(response)){
		strncpy(oldcomp.desc, response, Nname);
	}
	// Write new data to file
	fseek(fp, id * sizeof(CComp), SEEK_SET);
	fwrite(&oldcomp, sizeof(CComp), 1, fp);

	flock(fileno(fp), LOCK_UN);	
	fclose(fp);
	exit(0);
}

int promptChange(char *dest){
	char response[10];;
	char c;

	printf("Edit field [Y/N]?\n");
	scanf("%s", response);
	c = response[0];
	if(c == 'y' || c == 'Y'){
		printf("Enter new value for field:\n");
		scanf("%s", dest);
		return 1;
	}else if(c == 'n' || c == 'N'){
		return 0;
	}else{
		return promptChange(dest);
	}
}

