#include <stdio.h>
#include <stdlib.h>

#define EXPECTED_HEADER_TERMS 3
#define OUTPUT_WIDTH 300

int main(void){
	int ret, width, height, max, newWidth, newHeight, scaleFactor;
	int i, row, col;
	int bytesToRead, bytesToWrite;
	int offset, scaledOffset;
	char *old, *new;
	char current;
	
	/* Read PGM file header */
	ret = scanf("P5 %d %d %d", &width, &height, &max);
	if(ret != EXPECTED_HEADER_TERMS){
		fprintf(stderr, "ERROR: Failed to parse file header from PGM file on STDIN\n");
		return 1;
	}

	/* Calculate scaling factor */
	scaleFactor = width / 300;
	if(scaleFactor == 0){
		scaleFactor = 1;
	}
	
	/* Calculate new dimensions */
	newWidth = width / scaleFactor;
	newHeight = height / scaleFactor;

	/* Allocate memory for input file data */
	bytesToRead = width * height;
	old = malloc(bytesToRead);
	if(old == NULL){
		fprintf(stderr, "ERROR: Failed to allocate memory for original image\n");
		return 1;
	}

	/* Allocate memoryt for output file data */
	bytesToWrite = (newWidth + 1) * newHeight;
	new = malloc(bytesToWrite);
	if(new == NULL){
		fprintf(stderr, "ERROR: Failed to allocate memory for new image\n");
		free(old);
		return 1;
	}
	
	/* Read input file to memory */
	for(i = 0; i < bytesToRead; i++){
		current = getchar();
		*(old + i) = current;
	}
	
	/* Compress file */
	for(row = 0; row < newHeight; row++){
		for(col = 0; col < newWidth; col++){
			scaledOffset = (col * scaleFactor) + (row * scaleFactor * width);
			offset = col + row * newWidth;
			*(new + offset) = *(old + scaledOffset);
		}
		*(new + offset + 1) = '\n';
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

