struct file
{
  char name[11];
  int open;
  int write;
  unsigned int puntero_indice;
  unsigned int last_byte;
  int nro_entrada;
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
	// 
};

struct indice
{
    unsigned int file_size;
    unsigned int created_at;
    unsigned int modified_at;
    unsigned char bloques[252][4];
    struct indirect * Indireccion ;
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
