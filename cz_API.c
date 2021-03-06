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

void cz_mount(char* disk_filename) {

	direct = malloc(sizeof(Directorio));
	init_entradas();
	FILE * fp;
	fp = fopen(disk_filename, "rb");
	unsigned char AUX [64*16];
	int leidos = fread(AUX, sizeof(char), 16*64, fp);
	// printf("Tamaño AUX: %lu \n", sizeof(AUX));
	// printf("Nº bytes leidos: %i \n", leidos);
	// printf("Imprimiendo mi bloque directorio\n");
	// printf("Aux en 0: %u \n", AUX[10]);
	nombre_disco = disk_filename;
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
	fclose(fp);
}

void cz_ls() {
	for (int i=0; i<64; i++) {
		if (direct -> entradas[i] -> valid) {
			printf("%s\n", direct -> entradas[i] -> name);
		}
	}
}

int cz_cp(char* orig, char* dest){
	if (strcmp(orig, dest) == 0){
		fprintf(stderr, "Nombres deben ser distintos\n");
		return -1;
	}
	for (int i=0; i<64; i++) {
		if (direct -> entradas[i] -> valid && strcmp(direct -> entradas[i] -> name, dest) == 0) {
			fprintf(stderr, "Nombre ya existente %s \n", dest);
			return -1;
		}
	}
	int orig_entr;
	Indice* orig_ind;
	Indice* dest_ind = malloc(sizeof(Indice));
	dest_ind -> indireccion = malloc(sizeof(Indireccion));
	for (int i=0; i<64; i++) {
		if (direct -> entradas[i] -> valid && strcmp(direct -> entradas[i] -> name, orig) == 0) {
			orig_ind = create_Indice(get_block(direct -> entradas[i] -> puntero));
			orig_entr = i;
			dest_ind -> file_size = orig_ind -> file_size;
			dest_ind -> created_at = orig_ind -> created_at;
			dest_ind -> modified_at = orig_ind -> modified_at;
			// dest_ind -> numero_bloque_indireccion = orig_ind -> numero_bloque_indireccion;
			// for (int i=0; i<252; i++) {
			// 	dest_ind -> bloques[i] = orig_ind -> bloques[i];
			// }
			// dest_ind -> indireccion = orig_ind -> indireccion;
			break;
		} 
	}
	int dest_entr;
	for (int i=0; i<64; i++) {
		if (! direct -> entradas[i] -> valid) {
			dest_entr = i;
			break;
		}
		if (i == 63) {
			fprintf(stderr, "Error: no hay espacio en bloque directorio para este nuevo archivo\n");
			return -1;
		}
	}
	direct -> entradas[dest_entr] -> valid = 1;
	strcpy(direct -> entradas[dest_entr] -> name, dest);
	direct -> entradas[dest_entr] -> puntero = encontrar_bloque_libre();
	//printf("Asignando índice %d\n", direct -> entradas[dest_entr] -> puntero);
	escribir_nueva_entrada(dest, dest_entr, direct -> entradas[dest_entr] -> puntero);
	setear_bitmap_ocupado(direct -> entradas[dest_entr] -> puntero);
	unsigned int copied_so_far = 0;
	while (copied_so_far < orig_ind -> file_size) {
		
		unsigned int n_bl = copied_so_far / 1024;
		if (n_bl < 252) {
			n_bl = orig_ind -> bloques[n_bl];
		} else {
			n_bl = orig_ind -> indireccion -> bloques[n_bl - 252];
		}
		int new_bl = encontrar_bloque_libre();
		if (copied_so_far / 1024 < 252) {
			dest_ind -> bloques[copied_so_far / 1024] = new_bl;
		} else {
			dest_ind -> indireccion -> bloques[(copied_so_far / 1024) - 252] = new_bl;
		}
	

		FILE* fp;
		fp = fopen(nombre_disco, "rb+");
		fseek(fp, 1024 * n_bl, SEEK_SET);
		unsigned char buffer[1024];
		fread(buffer, 1024, 1, fp);
		fclose(fp);
		fp = fopen(nombre_disco, "rb+");
		//printf("buffer es %s\n", buffer);
		fseek(fp, 1024 * new_bl, SEEK_SET);
		//printf("new_bl es %d\n\n", new_bl);
		//printf("n_bl es %d\n\n", n_bl);
		// for (int j=0; j<1024; j++) {
		// 	printf("%u\n", buffer[j]);
		// 	fwrite(&buffer[j], sizeof(unsigned char), 1, fp);
		// }
		fwrite(&buffer, sizeof(unsigned char), 1024, fp);
		// fseek(fp, 1024 * new_bl, SEEK_SET);
		// fread(buffer, 1024, 1, fp);
		// printf("(2) buffer es %s\n", buffer);
		copied_so_far += 1024;
		fclose(fp);
		setear_bitmap_ocupado(new_bl);
	}

	FILE *fptr;
	if ((fptr = fopen(nombre_disco,"rb+")) == NULL){
	   fprintf(stderr, "Error! opening file");
	   // Program exits if the file pointer returns NULL.
	   exit(1);
	}

	fseek(fptr, direct -> entradas[dest_entr] -> puntero * 1024, SEEK_SET);

	unsigned char primer_digito = dest_ind -> file_size / 16777216;
	unsigned char segundo_digito = dest_ind -> file_size / 65536;
	unsigned char tercer_digito = dest_ind -> file_size / 256;
	unsigned char cuarto_digito = dest_ind -> file_size % 256;

	// escribimos file_size
	fwrite(&primer_digito,sizeof(unsigned char),1,fptr);
	fwrite(&segundo_digito,sizeof(unsigned char),1,fptr);
	fwrite(&tercer_digito,sizeof(unsigned char),1,fptr);
	fwrite(&cuarto_digito,sizeof(unsigned char),1,fptr);


	time_t seconds;
	seconds = time(NULL);
	primer_digito = seconds / 16777216;
	segundo_digito = seconds / 65536;
	tercer_digito = seconds / 256;
	cuarto_digito = seconds % 256;

	// escribimos fecha de creacion
	fwrite(&primer_digito,sizeof(unsigned char),1,fptr);
	fwrite(&segundo_digito,sizeof(unsigned char),1,fptr);
	fwrite(&tercer_digito,sizeof(unsigned char),1,fptr);
	fwrite(&cuarto_digito,sizeof(unsigned char),1,fptr);

	// escribimos fecha de update
	fwrite(&primer_digito,sizeof(unsigned char),1,fptr);
	fwrite(&segundo_digito,sizeof(unsigned char),1,fptr);
	fwrite(&tercer_digito,sizeof(unsigned char),1,fptr);
	fwrite(&cuarto_digito,sizeof(unsigned char),1,fptr);

	// ESCRIBIR EN EL BLOQUE EL ARREGLO BLOQUES[252]

	for (int i=0; i<252; i++) {
		// fseek(fptr, direct -> entradas[dest_entr] -> puntero * 1024 + 12 + i * 1024, SEEK_SET);
		primer_digito = dest_ind -> bloques[i] / 16777216;
		segundo_digito = dest_ind -> bloques[i] / 65536;
		tercer_digito = dest_ind -> bloques[i] / 256;
		cuarto_digito = dest_ind -> bloques[i] % 256;
		fwrite(&primer_digito,sizeof(unsigned char),1,fptr);
		fwrite(&segundo_digito,sizeof(unsigned char),1,fptr);
		fwrite(&tercer_digito,sizeof(unsigned char),1,fptr);
		fwrite(&cuarto_digito,sizeof(unsigned char),1,fptr);
	}

	dest_ind -> numero_bloque_indireccion = encontrar_bloque_libre();
	primer_digito = dest_ind -> numero_bloque_indireccion / 16777216;
	segundo_digito = dest_ind -> numero_bloque_indireccion / 65536;
	tercer_digito = dest_ind -> numero_bloque_indireccion / 256;
	cuarto_digito = dest_ind -> numero_bloque_indireccion % 256;

	fwrite(&primer_digito,sizeof(unsigned char),1,fptr);
	fwrite(&segundo_digito,sizeof(unsigned char),1,fptr);
	fwrite(&tercer_digito,sizeof(unsigned char),1,fptr);
	fwrite(&cuarto_digito,sizeof(unsigned char),1,fptr);


	// CASO QUE OCUPE INDIRECCION

	if (dest_ind -> file_size > 1024 * 252) {
		setear_bitmap_ocupado(dest_ind -> numero_bloque_indireccion);
		int aux = 1024 * 252; //va avanzando de a 1024
		fseek(fptr, dest_ind -> numero_bloque_indireccion * 1024, SEEK_SET);
		while (aux < dest_ind -> file_size) {
			primer_digito = dest_ind -> indireccion -> bloques[aux / 1024 - 252] / 16777216;
			segundo_digito = dest_ind -> indireccion -> bloques[aux / 1024 - 252] / 65536;
			tercer_digito = dest_ind -> indireccion -> bloques[aux / 1024 - 252] / 256;
			cuarto_digito = dest_ind -> indireccion -> bloques[aux / 1024 - 252] % 256;

			fwrite(&primer_digito,sizeof(unsigned char),1,fptr);
			fwrite(&segundo_digito,sizeof(unsigned char),1,fptr);
			fwrite(&tercer_digito,sizeof(unsigned char),1,fptr);
			fwrite(&cuarto_digito,sizeof(unsigned char),1,fptr);
			aux += 1024;
		}
	}


	fclose(fptr);

	return 0;

}

int cz_rm(char* filename) {
	for (int i=0; i<64; i++) {
		if (direct -> entradas[i] -> valid && strcmp(direct -> entradas[i] -> name, filename) == 0) {
			direct -> entradas[i] -> valid = 0;
			// escribir 0 en bit valido
			FILE * fptr;
			if ((fptr = fopen(nombre_disco,"rb+")) == NULL){
			   fprintf(stderr, "Error! opening file");
			   // Program exits if the file pointer returns NULL.
			   exit(1);
			}
			unsigned char invalid = 0;
			fseek(fptr, 16 * i, SEEK_SET);
			fwrite(&invalid, sizeof(unsigned char), 1, fptr);
		   	fclose(fptr);


			fprintf(stderr, "Se eliminó %s\n", filename);
			Indice* ind = create_Indice(get_block(direct -> entradas[i] -> puntero));
			for (int j=0; j<252;j++) {
				if (ind -> file_size > 1024*j) {
					setear_bitmap_libre(ind -> bloques[j]);
				}
			}
			setear_bitmap_libre(direct -> entradas[i] -> puntero);
			int y = 252*1024;
			while (y < ind -> file_size) {
				setear_bitmap_libre(ind -> indireccion -> bloques[y/1024 - 252]);
				y += 1024;
			}
			setear_bitmap_libre(ind -> numero_bloque_indireccion);
			return 0;
		}
	}
	fprintf(stderr, "No se encontró a %s\n", filename);
	return -1;
}

int cz_mv(char* orig, char *dest) {
	char origen[11];
	Dir_Entrada* entrada_origen;
	char destino[11];
	for (int i=0; i<64; i++) {
		if (direct -> entradas[i] -> valid && strcmp(direct -> entradas[i] -> name, dest) == 0) {
			fprintf(stderr, "Nombre ya existente: %s ya es un archivo\n", dest);
			return -1;
		}
	}
	for (int i=0; i<64; i++) {
		if (direct -> entradas[i] -> valid && strcmp(direct -> entradas[i] -> name, orig) == 0) {
			strcpy(direct -> entradas[i] -> name, dest);
			cambiar_nombre_en_disco(i, dest);
			return 0;
		}
	}
	fprintf(stderr, "No se encontró el archivo con nombre %s \n",orig);
	return -1;
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
		fprintf(stderr, "Se intentó abrir archivo %s en modo lectura pero no existe\n", filename);
		return NULL;
	}
	if (mode == 'w' && cz_exists(filename)) {
		fprintf(stderr, "Se intentó abrir archivo %s en modo escritura pero ya existe\n", filename);
		return NULL;
	}
	czFILE* file = malloc(sizeof(czFILE));
	file -> open = 1;
	strcpy(file -> name, filename);
	if (mode == 'r') {
		file -> read = 1;
		for (int i=0; i<64; i++) {
			if (direct -> entradas[i] -> valid && strcmp(direct -> entradas[i] -> name, filename) == 0) {
				file -> nro_entrada = i;
				//printf("Encontre el archivo, estaba en la posicion %i \n",i);
				break;
			}
		}
	} else {
		
		file -> read = 0;
		file -> write = 1;
		for (int i=0; i<64; i++) {
			if (i == 63 && direct -> entradas[i] -> valid) {
				// no hay espacio disponible para nuevo archivo
				return NULL;
			}
			if (!direct -> entradas[i] -> valid) {
				file -> nro_entrada = i;
				direct -> entradas[i] -> valid = 1;
				strcpy(direct -> entradas[i] -> name, filename);
		
				// Buscamos el primer BITMAP con 0
				// Usamos este flag porque no se como hacer break 2 veces
				int n_bloque = encontrar_bloque_libre();
				setear_bitmap_ocupado(n_bloque);
				direct -> entradas[i] -> puntero = n_bloque;	
				escribir_nueva_entrada(filename, i, n_bloque);
				escribir_fecha_creacion(n_bloque);
				file -> puntero_indice = n_bloque;
				break;
			
			}
		}
	}
	return file;
}

// void setear_bitmap_ocupado(int n_bloque){
// 	FILE *fptr;
// 	if ((fptr = fopen(nombre_disco,"rb+")) == NULL){
// 	   fprintf(stderr, "Error! opening file");
// 	   // Program exits if the file pointer returns NULL.
// 	   exit(1);
// 	}
// 	// me paro al 
// 	fseek(fptr, 1024 + n_bloque, SEEK_SET);
// 	// inicializamos el tamaño en 0
// 	unsigned char usado = 1;
// 	fwrite(&usado,sizeof(unsigned char),1,fptr);
//    	fclose(fptr);
void setear_bitmap_ocupado(int n_bloque){
	FILE *fptr;
	if ((fptr = fopen(nombre_disco,"rb+")) == NULL){
	   fprintf(stderr, "Error! opening file");
	   // Program exits if the file pointer returns NULL.
	   exit(1);
	}
	int byte_a_leer = (n_bloque / 8);
	fseek(fptr, 1024 + byte_a_leer, SEEK_SET);
	unsigned char cur_val;
	fread(&cur_val, sizeof(unsigned char), 1, fptr);
	int suma = 1;
	for (int i=1; i< 8 - n_bloque % 8; i++) {
		suma *= 2;
	}
	// suma es 2^(8 - n_bloque % 8)
	fclose(fptr);
	FILE *fp;
	if ((fp = fopen(nombre_disco,"rb+")) == NULL){
	   fprintf(stderr, "Error! opening file");
	   // Program exits if the file pointer returns NULL.
	   exit(1);
	}
	cur_val += suma;
	fseek(fp, 1024 + byte_a_leer, SEEK_SET);
	fwrite(&cur_val, sizeof(unsigned char), 1, fp);
	fclose(fp);
}

void setear_bitmap_libre(int n_bloque){
	FILE *fptr;
	if ((fptr = fopen(nombre_disco,"rb+")) == NULL){
	   fprintf(stderr, "Error! opening file");
	   // Program exits if the file pointer returns NULL.
	   exit(1);
	}
	int byte_a_leer = (n_bloque / 8);
	fseek(fptr, 1024 + byte_a_leer, SEEK_SET);
	unsigned char cur_val;
	fread(&cur_val, sizeof(unsigned char), 1, fptr);
	int suma = 1;
	for (int i=1; i< 8 - n_bloque % 8; i++) {
		suma *= 2;
	}
	// suma es 2^(8 - n_bloque % 8)
	fclose(fptr);
	FILE *fp;
	if ((fp = fopen(nombre_disco,"rb+")) == NULL){
	   fprintf(stderr, "Error! opening file");
	   // Program exits if the file pointer returns NULL.
	   exit(1);
	}
	cur_val -= suma;
	fseek(fp, 1024 + byte_a_leer, SEEK_SET);
	fwrite(&cur_val, sizeof(unsigned char), 1, fp);
	fclose(fp);
}



void escribir_fecha_creacion(int n_bloque){
	FILE *fptr;
	if ((fptr = fopen(nombre_disco,"rb+")) == NULL){
	   fprintf(stderr, "Error! opening file");
	   // Program exits if the file pointer returns NULL.
	   exit(1);
	}
	fseek(fptr, n_bloque * 1024, SEEK_SET);
	// inicializamos el tamaño en 0
	unsigned char size = 0;
	fwrite(&size,sizeof(unsigned char),1,fptr);
	fwrite(&size,sizeof(unsigned char),1,fptr);
	fwrite(&size,sizeof(unsigned char),1,fptr);
	fwrite(&size,sizeof(unsigned char),1,fptr);
	// Aqui le puse 4 "0"s
	time_t seconds;
	seconds = time(NULL);
	unsigned char primer_digito = seconds / 16777216;
	unsigned char segundo_digito = seconds / 65536;
	unsigned char tercer_digito = seconds / 256;
	unsigned char cuarto_digito = seconds % 256;

	// escribimos fecha de creacion
	fwrite(&primer_digito,sizeof(unsigned char),1,fptr);
	fwrite(&segundo_digito,sizeof(unsigned char),1,fptr);
	fwrite(&tercer_digito,sizeof(unsigned char),1,fptr);
	fwrite(&cuarto_digito,sizeof(unsigned char),1,fptr);

	// escribimos fecha de update
	fwrite(&primer_digito,sizeof(unsigned char),1,fptr);
	fwrite(&segundo_digito,sizeof(unsigned char),1,fptr);
	fwrite(&tercer_digito,sizeof(unsigned char),1,fptr);
	fwrite(&cuarto_digito,sizeof(unsigned char),1,fptr);

   	fclose(fptr);
}

unsigned char* get_block(int nro_bloque) {
	FILE* fp;
	fp = fopen(nombre_disco, "rb");
	static unsigned char internal_aux [1024];
	fseek(fp, nro_bloque * 1024, SEEK_SET);
	fread(internal_aux, 16, 64, fp);
	fclose(fp);
	return internal_aux;
}

Indice* create_Indice(unsigned char bloque[1024]) {
	Indice* indice = malloc(sizeof(Indice));
	indice -> file_size = 16777216 * bloque[0] + 65536 * bloque[1] + 256 * bloque[2] + bloque[3];
	indice -> created_at = 16777216 * bloque[4] + 65536 * bloque[5] + 256 * bloque[6] + bloque[7];
	indice -> modified_at = 16777216 * bloque[8] + 65536 * bloque[9] + 256 * bloque[10] + bloque[11];
	for (int i=0; i<252; i++) {
		// memcpy(indice -> bloques[i], bloque[12 + 4*i], 4);
		indice -> bloques[i] = 256 * bloque[12 + 4*i + 2] + bloque[12 + 4*i + 3];
	}
	indice -> indireccion = malloc(sizeof(struct indirect));
	int numero_bloque_indireccion = 256 * bloque[1022] + bloque[1023];
	indice -> numero_bloque_indireccion = numero_bloque_indireccion;
	unsigned char * bloque_indice = get_block(numero_bloque_indireccion);
	for (int j=0; j < 255; j++){
		indice -> indireccion -> bloques[j] = 256 * bloque[4*j + 2] + bloque[4*j + 3];
	}
	return indice;
}
int cz_read(czFILE* file_desc, void* buffer, int nbytes) {
	if (file_desc -> write || !file_desc -> read) {
		fprintf(stderr, "Archivo no puede leerse porque se abrió en modo escritura\n");
		return -1;
	}
	Indice* indice = create_Indice(get_block(direct -> entradas[file_desc -> nro_entrada] -> puntero));
	//printf("Estoy leyendo %d\n", direct -> entradas[file_desc -> nro_entrada] -> puntero);
	if (nbytes > indice -> file_size) {
		nbytes = indice -> file_size;
	}

	int read_so_far = 0;
	int bl_idx = file_desc -> last_byte / 1024;
	int offset = file_desc -> last_byte - (1024 * bl_idx);
	unsigned char* bloque = malloc(1024);
	unsigned char* buff = (unsigned char*) buffer;
	if (bl_idx >= 252) {
		bloque = get_block(indice -> indireccion -> bloques[bl_idx - 252]);
	} else {
		bloque = get_block(indice -> bloques[bl_idx]);
	}
	if (nbytes <= indice -> file_size - file_desc -> last_byte) {
		while (read_so_far < nbytes) {
			if (nbytes - read_so_far < 1024 - offset) {
				for (int j=0; j<nbytes - read_so_far; j++) {
					buff[read_so_far + j] = bloque[offset + j];
				}
				read_so_far = nbytes;
				file_desc -> last_byte += nbytes;
			} else { 	// hay que cambiar de bloque
				for (int j=0; j<1024 - offset; j++) {
					buff[read_so_far + j] = bloque[offset + j];
				}
				read_so_far += 1024 - offset;
				bl_idx++;
				if (bl_idx >= 252) {
					bloque = get_block(indice -> indireccion -> bloques[bl_idx - 252]);
				} else {
					bloque = get_block(indice -> bloques[bl_idx]);
				}
				offset = 0;
			}
		}
		buffer = buff;
		return nbytes;
	} else { //no va a leerse tanto: se termina el archivo antes
		file_desc -> last_byte = indice -> file_size;
		int a_leer = indice -> file_size - file_desc -> last_byte;
		while (read_so_far < a_leer) {
			if (a_leer - read_so_far < 1024 - offset) {
				for (int j=0; j < a_leer - read_so_far; j++) {
					buff[read_so_far + j] = bloque[offset + j];
				}
				read_so_far = indice -> file_size - file_desc -> last_byte;
				file_desc -> last_byte += a_leer;
			} else { 	// hay que cambiar de bloque
				for (int j=0; j<1024 - offset; j++) {
					buff[read_so_far + j] = bloque[offset + j];
				}
				read_so_far += 1024 - offset;
				bl_idx++;
				if (bl_idx >= 252) {
					bloque = get_block(indice -> indireccion -> bloques[bl_idx - 252]);
				} else {
					bloque = get_block(indice -> bloques[bl_idx]);
				}
				offset = 0;
			}
		}
		buffer = buff;
		return read_so_far;
	}
	return -2; //no deberiamos llegar
}
int cz_write(czFILE* file_desc, void* buffer, int nbytes){
	int numero_bloque_indice = direct -> entradas[file_desc -> nro_entrada] -> puntero;
	Indice* indice = create_Indice(get_block(numero_bloque_indice));
	int contador = 0;
	for (int k = 0; k < nbytes; k++){
		// Por ejemplo si ya he escrito 1024 bytes, quiere decir que tengo uno completado
		int bloques_completados = indice -> file_size / 1024;

		// reviso si escribo en alguna direccion de indireccion
		if (bloques_completados < 252){
			if (indice -> file_size % 1024 == 0){
				// Encuentro un bloque libre para poner datos
				int nuevo_bloque_datos = encontrar_bloque_libre();
				// seteo el numero del bloque en el indice
				setear_bloque_datos(indice, numero_bloque_indice, bloques_completados, nuevo_bloque_datos);
				// Marco el el bitmap en el disco como utilizado
				setear_bitmap_ocupado(nuevo_bloque_datos);
				// escribo en la posicion 0 del disco
				escribir_dato(nuevo_bloque_datos, 0, buffer, k);
				indice -> file_size ++;
				contador ++;
			}
			else {
				int datos_escritos_en_bloque = indice -> file_size % 1024;
				int bloque_de_datos = indice -> bloques[bloques_completados];
				escribir_dato(bloque_de_datos, datos_escritos_en_bloque, buffer, k);
				indice -> file_size ++;
				contador ++;
			}
		}
		else{ //Aqui estamos en el caso de que sea necesario la indireccion
			if (indice -> file_size % 1024 == 0){
				if (bloques_completados == 252){
					int numero_bloque_indireccion = encontrar_bloque_libre();
					marcar_indireccion(indice, numero_bloque_indice, numero_bloque_indireccion);

				}
				// Encuentro un bloque libre para poner datos
				int nuevo_bloque_datos = encontrar_bloque_libre();
				// seteo el numero del bloque indireccion
				setear_bloque_datos_indireccion(indice -> indireccion, indice -> numero_bloque_indireccion, bloques_completados - 252, nuevo_bloque_datos);
				// Marco el el bitmap en el disco como utilizado
				setear_bitmap_ocupado(nuevo_bloque_datos);
				// escribo en la posicion 0 del disco
				escribir_dato(nuevo_bloque_datos, 0, buffer, k);
				indice -> file_size ++;
				contador ++;
			}
			else {
				int datos_escritos_en_bloque = indice -> file_size % 1024;
				int bloque_de_datos = indice -> indireccion -> bloques[bloques_completados - 252];
				escribir_dato(bloque_de_datos, datos_escritos_en_bloque, buffer, k);
				indice -> file_size ++;
				contador ++;
			}

		}
	}
	actualizar_headers_indice(indice, numero_bloque_indice);

}
void escribir_nueva_entrada(char * filename, int numero_entrada, int n_bloque){
	FILE * fp = fopen(nombre_disco, "rb+");
	fseek(fp, numero_entrada*16, SEEK_SET);
	
	unsigned char uno = 1;

	// marco como vailda
	int bytes_validez = fwrite(&uno,sizeof(unsigned char),1,fp);

	// escribo nombre
	int bytes_nombres = fwrite(filename,sizeof(char),11,fp);
	//printf("Numero de bloque: %i\n", n_bloque);
	unsigned char primer_digito;
	unsigned char segundo_digito;
	primer_digito =  n_bloque / 256;
	segundo_digito = n_bloque % 256;
	fseek(fp, numero_entrada* 16 + 14, SEEK_SET);
	// printf("primer digito : %u \n", primer_digito);
	// printf("segundo digito : %u \n", segundo_digito);
	fwrite(&primer_digito, sizeof(unsigned char), 1,fp);
	fwrite(&segundo_digito, sizeof(unsigned char), 1,fp);
	fclose(fp);



}

// int encontrar_bloque_libre(){
// 	for (int z=0; z < 8; z ++){
// 		//Recorremos cada bloque del bitmpap
// 		for (int j= 0; j < 1023; j++){
// 			// Si: todavia no encuentro, el bit map esta en 0 y no estoy usando uno de los de al ppo
// 			if (bitmaps[z][j] == 0 && (z > 0 || j > 8)){
// 				int n_bloque = z * 1024 + j;
// 				bitmaps[z][j] = 1;	
// 				return n_bloque;
// 			}
// 		}
// 	}
// }
int encontrar_bloque_libre(){
	for (int z=0; z < 8; z ++){
		//Recorremos cada bloque del bitmpap
		for (int j= 0; j < 1023; j++){
			// Si: todavia no encuentro, el bit map esta en 0 y no estoy usando uno de los de al ppo
			
			if (bitmaps[z][j] < 255) { //hay alguno libre aca
				int off = 0;
				if (bitmaps[z][j] < 128) { //el primer bit esta en 0
					bitmaps[z][j] += 128;
					// off = 0;
				} else if (bitmaps[z][j] < 128 + 64) { // el seugndo bit esta en 0
					bitmaps[z][j] += 64;
					off = 1;
				} else if (bitmaps[z][j] < 128 + 64 + 32) { // el tercer bit esta en 0
					bitmaps[z][j] += 32;
					off = 2;
				} else if (bitmaps[z][j] < 128 + 64 + 32 + 16) { // el tercer bit esta en 0
					bitmaps[z][j] += 16;
					off = 3;
				} else if (bitmaps[z][j] < 128 + 64 + 32 + 16 + 8) { // el tercer bit esta en 0
					bitmaps[z][j] += 8;
					off = 4;
				} else if (bitmaps[z][j] < 128 + 64 + 32 + 16 + 8 + 4) { // el tercer bit esta en 0
					bitmaps[z][j] += 4;
					off = 5;
				} else if (bitmaps[z][j] < 128 + 64 + 32 + 16 + 8 + 4 + 2) { // el tercer bit esta en 0
					bitmaps[z][j] += 2;
					off = 6;
				} else {
					bitmaps[z][j] += 1;
					off = 7;
				}
				//printf("Encontre bloque libre: %i\n", (1024 * 8 * z) + (8 * j) + off);
				return (1024 * 8 * z) + (8 * j) + off;
			}
		}
	}
}

void escribir_dato(int nuevo_bloque_datos, int posicion, void * dato, int posicion_en_buffer){
	FILE * fp = fopen(nombre_disco, "rb+");
	fseek(fp, nuevo_bloque_datos * 1024 + posicion, SEEK_SET);
	fwrite(&dato[posicion_en_buffer], sizeof(unsigned char), 1,fp);
	fclose(fp);
}


void setear_bloque_datos(Indice * bloque_indice, int numero_bloque_indice, int bloques_completados, int nuevo_bloque_datos){
	// Aqui los guardo como unsigned int
	//printf("Escribiendo en el bloque indice, nº %i, en la posicion %i, al bloque de datos %i\n", numero_bloque_indice, bloques_completados, nuevo_bloque_datos);
	bloque_indice -> bloques[bloques_completados] = nuevo_bloque_datos;
	// Y ahora escribo en el disco
	FILE * fp = fopen(nombre_disco, "rb+");
	// el numero de bloque esta en file
	fseek(fp, numero_bloque_indice * 1024 + 12 + (4 *bloques_completados) + 2, SEEK_SET);
	// 
	unsigned char primer_digito = nuevo_bloque_datos / 256;
	unsigned char segundo_digito = nuevo_bloque_datos % 256;
	fwrite(&primer_digito, sizeof(unsigned char), 1,fp);
	fwrite(&segundo_digito, sizeof(unsigned char), 1,fp);
	fclose(fp);
}
void setear_bloque_datos_indireccion(Indireccion * bloque_indireccion, int numero_bloque_indireccion, int bloques_completados, int nuevo_bloque_datos){
	// Aqui los guardo como unsigned int
	bloque_indireccion -> bloques[bloques_completados] = nuevo_bloque_datos;
	// Y ahora escribo en el disco
	FILE * fp = fopen(nombre_disco, "rb+");
	// el numero de bloque esta en file
	fseek(fp, numero_bloque_indireccion * 1024 + (4 *bloques_completados) + 2, SEEK_SET);
	unsigned char primer_digito = nuevo_bloque_datos / 256;
	unsigned char segundo_digito = nuevo_bloque_datos % 256;
	fwrite(&primer_digito, sizeof(unsigned char), 1,fp);
	fwrite(&segundo_digito, sizeof(unsigned char), 1,fp);
	fclose(fp);
}

void marcar_indireccion(Indice * puntero_indice, int numero_bloque_indice, int numero_bloque_indireccion){
	FILE * fp = fopen(nombre_disco, "rb+");	
	fseek(fp, numero_bloque_indireccion * 1024 + 1022, SEEK_SET);
	unsigned char primer_digito = numero_bloque_indireccion / 256;
	unsigned char segundo_digito = numero_bloque_indireccion % 256;
	fwrite(&primer_digito, sizeof(unsigned char), 1,fp);
	fwrite(&segundo_digito, sizeof(unsigned char), 1,fp);
	fclose(fp);

}


void actualizar_headers_indice(Indice * indice, int n_bloque){
	FILE * fp = fopen(nombre_disco, "rb+");	
	fseek(fp, n_bloque * 1024, SEEK_SET);
	// Aqui escribimos el tamaño del archivo
	unsigned char primer_digito = indice -> file_size / 16777216;
	unsigned char segundo_digito = indice -> file_size / 65536;
	unsigned char tercer_digito = indice -> file_size / 256;
	unsigned char cuarto_digito = indice -> file_size % 256;
	fwrite(&primer_digito, sizeof(unsigned char), 1,fp);
	fwrite(&segundo_digito, sizeof(unsigned char), 1,fp);
	fwrite(&tercer_digito, sizeof(unsigned char), 1,fp);
	fwrite(&cuarto_digito, sizeof(unsigned char), 1,fp);
	fseek(fp, n_bloque * 1024 + 8, SEEK_SET);
	// Aqui escribimos la nueva fecha de actualización
	time_t seconds;
	seconds = time(NULL);
	unsigned char primer_digito_f = seconds / 16777216;
	unsigned char segundo_digito_f = seconds / 65536;
	unsigned char tercer_digito_f = seconds / 256;
	unsigned char cuarto_digito_f = seconds % 256;
	fwrite(&primer_digito_f,sizeof(unsigned char),1,fp);
	fwrite(&segundo_digito_f,sizeof(unsigned char),1,fp);
	fwrite(&tercer_digito_f,sizeof(unsigned char),1,fp);
	fwrite(&cuarto_digito_f,sizeof(unsigned char),1,fp);
	//printf("Actualizando indices: Nuevo Tamaño: %i, Nueva fecha: %i \n", indice -> file_size, seconds);

	fclose(fp);
}

void cambiar_nombre_en_disco(int i, char * dest){
	FILE * fp = fopen(nombre_disco, "rb+");	
	fseek(fp, 16 * i +  1 , SEEK_SET);
	fwrite(dest,sizeof(char),11,fp);
}

int cz_close(czFILE* file_desc) {
	if (!file_desc || !file_desc -> open) {
		return 1;
	}
	file_desc -> open = 0;
	return 0;
}

