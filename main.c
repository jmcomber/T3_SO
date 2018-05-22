#include "cz_API.c"


int main(int argc, char *argv[])
{
	cz_mount(argv[1]);
	for (int i = 0; i < 63; i++){
        if (direct -> entradas[i] -> valid){
            printf("Nombre archivo: %s\n", direct -> entradas[i] -> name);
            printf("Bloque indice: %i \n", direct -> entradas[i] -> puntero);
        }
    }
	return 0;
}
