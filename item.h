#include <stdio.h>
#ifndef ITEM_H
#define ITEM_H

typedef struct Item item, *ptr;
struct Item{
	int id;
	char *nome;
	char *categoria;
	int valor_base;
	int compra_ja;
	int tempo;
	ptr prox;
};
void addItem(item *p, int id, char *nome, char *catg, int vb, int cj, int tempo);
void mostraItems();
#endif
