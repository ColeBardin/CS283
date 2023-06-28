#include <errno.h>
#include <stdio.h>
#include <string.h>

#define FILE_ACCESS_ERR 2
#define FILE_DIFFERENT 1
#define FILE_IDENTICAL 0

#define EXPECTED_ARGC 3

#define FILE0_INDEX 1
#define FILE1_INDEX 2

void usage(void);
void fopenERR(char *fn);
int compareFiles(FILE *f0, char *fn0, FILE *f1, char *fn1);

int main(int argc, char **argv){
	FILE *file0;
	FILE *file1;
	int ret;

	/* Check number of arguments passed */
	if(argc != EXPECTED_ARGC){
		fprintf(stderr, "ERROR: Incorrect argument count. Received %d, expected %d argument(s)\n", argc-1, EXPECTED_ARGC-1);
		usage();
		return FILE_ACCESS_ERR;
	}

	/* Don't compare a file with itself */
	if(!strcmp(argv[FILE0_INDEX], argv[FILE1_INDEX])){
		fprintf(stderr, "ERROR: Filenames are not unique\n");
		usage();
		return FILE_ACCESS_ERR;
	}
	
	/* Open file0 */
	file0 = fopen(argv[FILE0_INDEX], "r");
	if(file0 == NULL){
		/* If file open fails, handle error */
		fopenERR(argv[FILE0_INDEX]);
		return FILE_ACCESS_ERR;
	}

	/* Open file1 */
	file1 = fopen(argv[FILE1_INDEX], "r");
	if(file1 == NULL){
		/* If file open fails, handle error */
		fopenERR(argv[FILE1_INDEX]);
		fclose(file0);
		return FILE_ACCESS_ERR;
	}

	compareFiles(file0, argv[FILE0_INDEX], file1, argv[FILE1_INDEX]);

	/* Close files upon exit */
	fclose(file0);
	fclose(file1);
}

void usage(void){
	printf("Usage: cmp file1 file2\n");
}

void fopenERR(char *fn){
	if(errno == ENOENT){
		/* File does not exist error */
		fprintf(stderr, "ERROR: Cannot find file %s\n", fn);
	}else if(errno == EACCES){
		/* File permissions error */
		fprintf(stderr, "ERROR: Cannot access file %s. Check permissions\n", fn);
	}else{
		/* All other errors */
		fprintf(stderr, "ERROR: Unexpected error (0x%x) while opening file %s\n", errno, fn);
	}
}

int compareFiles(FILE *f0, char *fn0, FILE *f1, char *fn1){
	int byteCount;
	int lineNumber;
	char f0Char;
	char f1Char;

	/* Set counters */
	lineNumber = 1;
	byteCount = 1;
	/* Set char values so they don't init to EOF */
	f0Char = 0;
	f1Char = 0;

	/* Compare each byte */
	while(f0Char != EOF && f1Char != EOF){
		f0Char = fgetc(f0);
		f1Char = fgetc(f1);

		/* Check if characters are different */
		if(f0Char != f1Char){
			if(f0Char == EOF && f1Char != EOF){
				/* If end of file 0 has been reached */
				printf("file1 (%s) is initial subsequence of file2 (%s)\n", fn0, fn1);
			}else if(f0Char != EOF && f1Char == EOF){
				/* If end of file 1 has been reached */
				printf("file2 (%s) is initial subsequence of file1 (%s)\n", fn1, fn0);
			}else{
				/* No EOFs, just two different characters */
				printf("Files are different at byte %d on line %d\n", byteCount, lineNumber);
			}
			return FILE_DIFFERENT;
		}

		/* Track newlines and bytes */
		if(f0Char == '\n'){
			lineNumber++;
		}
		byteCount++;
	}


	/* If control reaches here, files are identical */
	return FILE_IDENTICAL;
}
