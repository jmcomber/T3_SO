struct file
{
  char name[11];
  int open;
  int write;
  int read;
  unsigned int puntero_indice;
  unsigned int last_byte;
  int nro_entrada;
  int bytes_escritos;
};
typedef struct file czFILE;


struct dir_entrada
{
	char valid;
	char name[11];
	int puntero;
};
typedef struct dir_entrada Dir_Entrada;


struct directorio 
{
	Dir_Entrada* entradas[64];
};
typedef struct directorio Directorio;

// struct bloque
// {
// 	// 
// };
// typedef struct bloque Bloque;


struct indirect
{
	unsigned int bloques[256];
};
typedef struct indirect Indireccion;

struct indice
{
    unsigned int file_size;
    unsigned int created_at;
    unsigned int modified_at;
    unsigned int bloques[252];
    struct indirect * indireccion ;
    unsigned int numero_bloque_indireccion;
};
typedef struct indice Indice;



void cz_mount(char* disk_filename);
czFILE* cz_open(char* filename, char mode);
int cz_exists(char* filename);
int cz_read(czFILE* file_desc, void* buffer, int nbytes);
int cz_write(czFILE* file_desc, void* buffer, int nbytes);
int cz_close(czFILE* file_desc);
int cz_mv(char* orig, char *dest);
int cz_cp(char* orig, char* dest);
int cz_rm(char* filename);
void cz_ls();
void escribir_fecha_creacion(int n_bloque);
void escribir_nueva_entrada(char * filename, int numero_entrada, int n_bloque);
void setear_bitmap_ocupado(int n_bloque);
void escribir_dato(int nuevo_bloque_datos, int posicion, void * dato, int posicion_en_buffer);
int encontrar_bloque_libre();
void setear_bloque_datos(Indice * bloque_indice, int numero_bloque_indice, 
  int bloques_completados, int nuevo_bloque_datos);
void setear_bloque_datos_indireccion(Indireccion * bloque_indireccion, int numero_bloque_indireccion,
 int bloques_completados, int nuevo_bloque_datos);
void marcar_indireccion(Indice * puntero_indice, int numero_bloque_indice, int numero_bloque_indireccion);
void actualizar_headers_indice(Indice * indice, int numero_bloque_indice);
Indice* create_Indice(unsigned char bloque[1024]);
unsigned char* get_block(int nro_bloque);
void setear_bitmap_libre(int n_bloque);