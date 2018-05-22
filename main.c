#include "cz_API.c"


int main(int argc, char *argv[])
{
	cz_mount(argv[1]);  
    //cz_ls();

    // //cz_write(file, "Hola", 4);

    czFILE * file = cz_open("test_1.txt",'r');
    //cz_write(file, "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Etiam non felis in arcu viverra condimentum. Maecenas vestibulum, orci ac finibus ullamcorper, ante tellus mollis erat, sed dapibus massa augue quis est. Quisque id odio et lectus aliquam consequat et sed urna. In volutpat hendrerit lobortis. Vestibulum vel odio at ex tincidunt dignissim. Fusce vitae dui erat. In hac habitasse platea dictumst. Proin a varius arcu, sed ornare sapien. Duis vitae felis non erat mollis fermentum. Aenean et gravida nisi. Maecenas pulvinar eros suscipit rhoncus porta. Vestibulum rutrum dignissim justo, a ornare ipsum malesuada vitae. Morbi congue, sapien luctus ultricies mollis, velit mi dictum est, ut porta elit felis sit amet lacus. In elit orci, lobortis ut imperdiet ut, elementum id eros. Maecenas eu imperdiet ligula, a ultricies nisi. Praesent vel ante odio. Praesent sit amet mi aliquam tellus fermentum tincidunt ut quis justo. In sodales, risus sit amet rutrum pretium, felis odio suscipit sem, ac venenatis sem erat quis metus.", 1030);
    //cz_write(file, " como va? ", 10);
    //cz_write(file, "Bien y tu", 8);

    //unsigned char buffer[15];
    unsigned char buffer[1030];
    cz_read(file, buffer, 1030);
    //cz_read(file, buffer2, 8);
    printf("Buffer 1: %s\n", buffer);
    //printf("Buffer 1: %s\n", buffer);
    //printf("Buffer 2: %s\n", buffer2);
    // for (int i = 0; i < 63; i++){
    //     if (direct -> entradas[i] -> valid){
    //         printf("Nombre archivo: %s\n", direct -> entradas[i] -> name);
    //         printf("Bloque indice: %i \n", direct -> entradas[i] -> puntero);
    //     }
    // }
	return 0;
}
