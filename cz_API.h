

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


struct directorio {
	struct dir_entrada* entradas[64];
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

	Indireccion* struct indirect;
};
typedef struct indice Indice;
