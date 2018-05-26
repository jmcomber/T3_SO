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
    //cz_ls();

    // FILE * fp = fopen("prueba_grande.txt", "r");
    // char buffer[445593];
    // fread(buffer, 445593, 1, fp);
    // fclose(fp);

    // czFILE* nuevo = cz_open("test.txt", 'w');
    // cz_write(nuevo, buffer, 445593);


    //int a = cz_cp("tres.txt","t4.txt");


    // char buffer2[445593];
    // czFILE * file2 = cz_open("test.txt", 'r');
    // cz_read(file2, buffer2, 445593);
    // printf("%s\n", buffer2);

    //printf("Despues de Move: (imprimio %i) \n\n\n\n", a);

    // printf("%d\n", cz_close(nuevo));

    // ESCRITURA //
    //czFILE* nuevo = cz_open("write.txt", 'w');
    //cz_write(nuevo, buffer, 258094);



    // char buffer2[258094];
    // czFILE * file = cz_open("write.txt", 'r');
    // cz_read(file, buffer2, 258094);
    // printf("%s\n", buffer2);

    // COPY 
    //cz_cp("texto.txt", "chico.txt");
    // cz_rm("new3.txt");
    // cz_rm("new4.txt");
    // cz_rm("new5.txt");

    char buffer3[445593];
    czFILE * file = cz_open("test.txt", 'r');
    cz_read(file, buffer3, 445593);
    printf("%s\n", buffer3);







    cz_ls();



	return 0;
}
