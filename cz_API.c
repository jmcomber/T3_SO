#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cz_API.h"
#include <time.h>


char* nombre_disco;
Directorio* direct;
unsigned char bitmaps[8][1024];


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

	nombre_disco = disk_filename;
	printf("AUX: %u \n", AUX[3]);
	printf("AUX: %c \n", AUX[3]);

	for (int i=0; i<64; i++) {
		direct -> entradas[i] -> valid = AUX[16*i]; //aqui copio el bit de validez
		for (int j= 0; j < 11; j++){
			direct -> entradas[i] -> name[j] = AUX[16*i + 1 + j];
		}

		int otro = 256 * AUX[16*i + 14] + AUX[16*i + 15];
		direct -> entradas[i] -> puntero = otro;
	}
	for (int i=0; i<8; i++) {
		fread(bitmaps[i], 1024, 1, fp);
	}
	printf("%i\n", bitmaps[1][0]);
	//printf("%u\n", direct -> entradas[3] -> name);
	printf("%i \n", direct -> entradas[3] -> puntero);
	fclose(fp);
}

void cz_ls() {
	for (int i=0; i<64; i++) {
		if (direct -> entradas[i] -> valid) {
			printf("%s\n", direct -> entradas[i] -> name);
		}
	}
}

int cz_rm(char* filename) {
	for (int i=0; i<64; i++) {
		if (direct -> entradas[i] -> valid && strcmp(direct -> entradas[i] -> name, filename) == 0) {
			direct -> entradas[i] -> valid = 0;
			printf("Eliminado \n");
			return 0;
		}
	}
	return -1;
}

int cz_mv(char* orig, char *dest) {
	char origen[11];
	Dir_Entrada* entrada_origen;
	char destino[11];
	for (int i=0; i<64; i++) {
		if (direct -> entradas[i] -> valid && strcmp(direct -> entradas[i] -> name, dest) == 0) {
			printf("Nombre ya existente %s \n",orig);
			return -1;
		}
	}
	for (int i=0; i<64; i++) {
		if (direct -> entradas[i] -> valid && strcmp(direct -> entradas[i] -> name, orig) == 0) {
			strcpy(direct -> entradas[i] -> name, dest);
			return 0;
		}
	}
	printf("No se encontro el archivo con nombre %s \n",orig);
	return -1;
}

int cz_cp(char* orig, char* dest){
	if (strcmp(orig, dest) == 0){
		printf("Nombres deben ser distintos\n");
		return -1;
	}
	for (int i=0; i<64; i++) {
		if (direct -> entradas[i] -> valid && strcmp(direct -> entradas[i] -> name, dest) == 0) {
			printf("Nombre ya existente %s \n",orig);
			return -1;
		}
	}
	// Aqui viene la parte complicada: escribir el nuevo archivo

}

int cz_exists(char* filename) {
	for (int i=0; i<64; i++) {
		if (direct -> entradas[i] -> valid && strcmp(direct -> entradas[i] -> name, filename) == 0) {
			return 1;
		}
	}
	return 0;
}

czFILE* cz_open(char* filename, char mode) {
	if (mode == 'r' && !cz_exists(filename)) {
		return NULL;
	}
	if (mode == 'w' && cz_exists(filename)) {
		return NULL;
	}
	czFILE* file = malloc(sizeof(czFILE));
	file -> open = 1;
	strcpy(file -> name, filename);
	if (mode == 'r') {
		file -> read = 1;
		for (int i=0; i<64; i++) {
			if (direct -> entradas[i] -> valid && strcmp(direct -> entradas[i] -> name, orig) == 0) {
				file -> nro_entrada = i;
				break;
			}
		}
	} else {
		file -> read = 0;
		for (int i=0; i<64; i++) {
			if (i = 63 && direct -> entradas[i] -> valid) {
				// no hay espacio disponible para nuevo archivo
				return NULL;
			}
			if (!direct -> entradas[i] -> valid) {
				file -> nro_entrada = i;
				direct -> entradas[i] -> valid = 1;
				strcpy(direct -> entradas[i] -> name, filename);
				// Buscamos el primer BITMAP con 0
				// Usamos este flag porque no se como hacer break 2 veces
				int encontrado = 0;
				for (int z=0; bloque < 8; j ++){
					//Recorremos cada bloque del bitmpap
					for (int j= 0; j < 1023){
						// Si: todavia no encuentro, el bit map esta en 0 y no estoy usando uno de los de al ppo
						if (encontrado == 0 && bitmaps[z][j] == 0 && (z > 0 || j > 8)){
							// Este es el numero del bloque indice 
							int n_bloque = z * 1024 + j;
							// marcamos como ocupado el bloque
							bitmaps[z][j] == 1;
							// guardamos en bloque directorio este numero, indice es unsigned int
							direct -> entradas[i] -> puntero_indice = n_bloque;
							escribir_fecha_creacion(n_bloque);
							encontrado = 1;
						}
					}
				}
				break;
			}
		}
	}
	return file;
}

void escribir_fecha_creacion(int n_bloque){
	FILE *fptr;
	if ((fptr = fopen(nombre_disco,"wb")) == NULL){
	   printf("Error! opening file");
	   // Program exits if the file pointer returns NULL.
	   exit(1);
	}
	fseek(fptr, n_bloque * 1024, SEKK_SET);
	int size = 0;
	fwrite(&size,sizeof(int),1,fptr);
	time_t seconds;
	seconds = time(NULL);
	fwrite(&seconds,sizeof(time_t),1,fptr);
   	fclose(fptr);
}

unsigned char* get_block(int nro_bloque) {
	fp = fopen(disk_filename, "rb");
	static unsigned char internal_aux [1024];
	fread(internal_aux, 16, 64, fp);
	close(fp);
	return internal_aux;
}

Indice* create_Indice(unsigned char bloque[1024]) {
	Indice* indice = malloc(sizeof(Indice));
	indice -> file_size = 16777216 * bloque[0] + 65536 * bloque[1] + 256 * bloque[2] + bloque[3];
	indice -> created_at = 16777216 * bloque[4] + 65536 * bloque[5] + 256 * bloque[6] + bloque[7];
	indice -> modified_at = 16777216 * bloque[8] + 65536 * bloque[9] + 256 * bloque[10] + bloque[11];
	for (int i=0; i<252; i++) {
		memcpy(indice -> bloques[i], bloque[12 + 4*i], 4);
	}
	indice -> indirect = malloc(sizeof(Indireccion));
}

int cz_read(czFILE* file_desc, void* buffer, int nbytes) {
	if (!file_desc -> read) {
		return -1;
	}
	Indice* indice = create_Indice(get_block(entradas[file_desc -> nro_entrada] -> puntero));
	if (nbytes <= indice -> file_size - file_desc -> last_byte) {
		int bl_idx = file -> last_byte / 1024;
		int offset = file -> last_byte - (1024 * bl_idx);
		int read_so_far = 0;
		unsigned char bloque = get_block(bl_idx);
		while (read_so_far < nbytes) {
			if (offset < 1024) {
				// ver si nbytes - read_so_far < 1024 - offset: si ese es el caso, leer y terminarlo. Si no,
				// escribir en buffer el pedazo bloque[offset:1024], y aumentar en 1024 - offset el read_so_far
			}
		}
		// ...

		return nbytes;
	} else { //no va a leerse tanto: se termina el archivo antes
		file -> last_byte = indice -> file_size;
	}

}


