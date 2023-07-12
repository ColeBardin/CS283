#include <stdio.h>
#include <stdlib.h>

#define EXPECTED_HEADER_TERMS 3
#define DESIRED_WIDTH 300

int main(void){
	int width, height, max, newWidth, newHeight, scaleFactor;
	int i, row, col;
	int bytesToRead, bytesToWrite;
	int offset, scaledOffset;
	char *old, *new;
	char current;
	
	/* Read PGM file header */
	if(scanf("P5 %d %d %d", &width, &height, &max) != EXPECTED_HEADER_TERMS){
		fprintf(stderr, "ERROR: Failed to parse file header from PGM file on STDIN\n");
		return 1;
	}

	/* Calculate scaling factor */
	scaleFactor = width / DESIRED_WIDTH;
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
	bytesToWrite = newWidth * newHeight;
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
	
	/* Compress file with Top-Left pixel favoring */
	for(row = 0; row < newHeight; row++){
		for(col = 0; col < newWidth; col++){
			offset = col + row * newWidth;
			scaledOffset = (col * scaleFactor) + (row * scaleFactor * width);
			*(new + offset) = *(old + scaledOffset);
		}
		/* Add newline after each row */
		*(new + offset + 1) = '\n';
	}

	/* Output compressed file to STDOUT */
	printf("P5\n%d %d\n%d", newWidth, newHeight, max);
	for(i = 0; i < bytesToWrite; i++){
		putchar(*(new + i));
	}
	putchar(EOF);

	free(old);
	free(new);
	return 0;
}

