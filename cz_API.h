

struct file
{
  char name[11];
  int open;
  int write;

};
typedef struct file czFILE;


struct dir_entrada
{
	int valid;
	char name[11];
	czFILE* puntero;
};
typedef struct dir_entrada Dir_Entrada;


struct directorio 
{
	Dir_Entrada* entradas[64];
};
typedef struct directorio Directorio;

struct bloque
{
	// 
};
typedef struct bloque Bloque;


struct indirect
{
	// 
};

struct indice
{
	int file_size;   //es un int?
	int created_at;  //es un int?
	int modified_at; //es un int?

	Bloque* bloques[252]; //o hacerlos int no mas, pa que struct

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
