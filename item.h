#include <stdio.h>
#include <string.h>

#ifndef ITEM_H
#define ITEM_H
#define FITEM "items.txt"

typedef struct Item item, *ptr;
struct Item{
	int id;
	char nome[20+1];
	char categoria[20+1];
	int valor_base;
	int compra_ja;
	int tempo;
	char dono[20+1];
	char licitador[20+1];
};

void mostraItem(item *i, int tam);
void licat(char *ctg, item *i, int item_len);
void lisel(char *nome,  item *i, int item_len);
void lival(int value,  item *i, int item_len);
void litime(int time,  item *i, int item_len);
item* adicionaItem(item *i,int tam,  char *n, int id, char *ctg, int vb, int cj, int tmp, char *, char *);
item *leFicheiroItem(char *nomeFich, item *i);
int contaItems(char*);
int eliminaItem(int id, item *i, int item_len);
#endif
