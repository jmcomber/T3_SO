CC=gcc -std=gnu11

czfs: main.c cz_API.c cz_API.h
	$(CC) -o czfs main.c