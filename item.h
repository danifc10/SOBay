#include <stdio.h>
#ifndef ITEM_H
#define ITEM_H

typedef struct Item item;
struct Item{
	int id;
	char *nome;
	char *categoria;
	int valor_base;
	int compra_ja;
	int tempo;
};

#endif
