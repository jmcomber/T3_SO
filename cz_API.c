#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cz_API.h"


char* nombre_disco;
Directorio* direct;

void init_entradas() {
	for (int i=0; i<64; i++) {
		printf("%d\n", i);
		direct -> entradas[i] = malloc(sizeof(Dir_Entrada));
		direct -> entradas[i] -> valid = 3; //algun numero raro
		strcpy(direct -> entradas[i] -> name, "null.null");
	}
} 


unsigned concatenate(unsigned x, unsigned y) {
	unsigned pow = 10;
	while(y >= pow)
		pow *= 10;
	return x * pow + y;
}

void cz_mount(char* disk_filename) {
	// init_entradas();
	FILE * fp;
	fp = fopen(disk_filename, "r");
	unsigned char AUX[64][16];
	fread(AUX, 16, 64, fp);
	fclose(fp);
	nombre_disco = disk_filename;
	for (int i=0; i<64; i++) {
		for (int j=0; j<16; j++) {
			printf("%c ", AUX[i][j]);
		}
		printf("\n");
		direct -> entradas[i] -> valid = AUX[i][1]; //2do byte dice que es valido o no?
		printf("1\n");
		char otro[11];
		for (int j=0; j<11; j++) {
			otro[j] = AUX[i][2 + j];
		}
		strcpy(direct -> entradas[i] -> name, otro);
		unsigned char aux2[4];
		for (int j=0; j<4; j++) {
			aux2[j] = AUX[i][12 + j];
		}
		printf("aux2[0] y aux2[1] son %u y %u\n", aux2[0], aux2[1]);
		printf("y el concat es %u\n", concatenate(aux2[0], aux2[1]));
		exit(0);

		// direct -> entradas[i] -> puntero = 
	}


}