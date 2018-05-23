#include "cz_API.c"


int main(int argc, char *argv[])
{
	cz_mount(argv[1]);
	// for (int i = 0; i < 63; i++){
 //        if (direct -> entradas[i] -> valid){
 //            printf("Nombre archivo: %s\n", direct -> entradas[i] -> name);
 //            printf("Bloque indice: %i \n", direct -> entradas[i] -> puntero);
 //        }
 //    }

    cz_ls();
    czFILE* texto = cz_open("texto.txt", 'r');
    unsigned char* buff = malloc(200);
    cz_read(texto, buff, 200);
    printf("%s\n", buff);
	return 0;
}
