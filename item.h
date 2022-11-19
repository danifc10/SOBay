#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef ITEM_H
#define ITEM_H
<<<<<<< HEAD
#define ITEM_FILENAME
=======
#define FITEM "items.txt"
>>>>>>> d16db34305b1dd06e24b53540ec5a599e5e50910

typedef struct Item item, *ptr;
struct Item{
	int id;
	char nome[20+1];
	char categoria[20+1];
	int valor_base;
	int compra_ja;
	int tempo;
	ptr prox;
};

void mostraItem();
void adicionaItem(char *n, int id, char *ctg, int vb, int cj, int tmp);
void leFicheiroItem(char *nomeFich);

#endif
