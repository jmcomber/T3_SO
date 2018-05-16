#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char* nombre_disco;

void cz_mount(char* disk_filename) {
	FILE * fp;

	fp = fopen(disk_filename, "r");
	int* AUX = calloc(64, 16);
	size_t fread(AUX, 16, 64, fp);


	fclose(fp);
   	strcpy(nombre_disco, disk_filename);
   	return(0);
}