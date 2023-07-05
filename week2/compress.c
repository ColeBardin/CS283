#include <stdio.h>
#include <stdlib.h>

#define EXPECTED_HEADER_TERMS 3
#define OUTPUT_WIDTH 300

int main(void){
	int ret, width, height, max;
	int newWidth, newHeight, scaleFactor;
	char *old, *new;
	char current;
	int row, col;
	int i;
	int bytesToRead, bytesToWrite;
	
	ret = scanf("P5 %d %d %d", &width, &height, &max);
	if(ret != EXPECTED_HEADER_TERMS){
		printf("ERROR: Failed to parse file header from PGM file on STDIN\n");
		return 1;
	}

	scaleFactor = width / 300;
	if(scaleFactor == 0){
		scaleFactor = 1;
	}

	newWidth = width / scaleFactor;
	newHeight = height / scaleFactor;

	bytesToRead = width * height;

	old = malloc(bytesToRead);
	if(old == NULL){
		printf("ERROR: Failed to allocate memory for original image\n");
		return 1;
	}

	bytesToWrite = (newWidth + 1) * newHeight;
	new = malloc(bytesToWrite);
	if(new == NULL){
		printf("ERROR: Failed to allocate memory for new image\n");
		free(old);
		return 1;
	}
	
	current = 0;
	row = 0;
	col = 0;
	
	/* Read input file to memory */
	for(i = 0; i < bytesToRead; i++){
		current = getchar();
		*(old + i) = current;
	}
	
	int offset;
	/* Compress file */
	for(row = 0; row < newHeight; row++){
		for(col = 0; col < newWidth; col++){
			offset = (col * scaleFactor) + (row * scaleFactor * width);
			*(new + col + row * newWidth) = *(old + offset);
		}
		*(new + col + row * newWidth + 1) = '\n';
	}

	/* Output compressed file */
	printf("P5\n%d %d\n%d", newWidth, newHeight, max);
	for(i = 0; i < bytesToWrite; i++){
		putchar(*(new + i));
	}
	putchar(EOF);

	free(old);
	free(new);
	return 0;
}

