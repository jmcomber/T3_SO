#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cz_API.h"


char* nombre_disco;
Directorio* direct;


void init_entradas() {
	for (int i=0; i<64; i++) {
		//printf("%d\n", i);
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

	direct = malloc(sizeof(Directorio));
	init_entradas();
	FILE * fp;

	fp = fopen(disk_filename, "rb");

	unsigned char AUX [64*16];
	fread(AUX, 16, 64, fp);

	fclose(fp);

	nombre_disco = disk_filename;
	printf("AUX: %u \n", AUX[0]);

	for (int i=0; i<64; i++) {
		//printf("Bit de validez: %c \n",AUX[16*i]);
		
			//printf("%c", AUX[16*i+j]);
			//printf("Bit de validez: %c \n",AUX[16*i]);
		direct -> entradas[i] -> valid = AUX[16*i]; //aqui copio el bit de validez
		strncpy(direct -> entradas[i] -> name, (char *) AUX[16*i], 11);
		// Aqui falta lo del puntero // Sera inicialmente un que, un
			 




	
		//printf("\n");
		//direct -> entradas[i] -> valid = AUX[i][1]; //2do byte dice que es valido o no?
		// char otro[11];
		// for (int j=0; j<11; j++) {
		// 	otro[j] = AUX[i][2 + j];
		// }
		//strcpy(direct -> entradas[i] -> name, otro);
		// unsigned char aux2[4];
		// // for (int j=0; j<4; j++) {
		// // 	aux2[j] = AUX[i][12 + j];
		// // }
		// printf("aux2[0] y aux2[1] son %u y %u\n", aux2[0], aux2[1]);
		// printf("y el concat es %u\n", concatenate(aux2[0], aux2[1]));
		//exit(0);

		// direct -> entradas[i] -> puntero = 
	}
	printf("%s\n", direct -> entradas[0] -> name);
	


}