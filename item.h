#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef ITEM_H
#define ITEM_H

static int id = 0;
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
int getId();
void mostraItem(item *i, int tam);
void licat(char *ctg, item *i, int item_len);
void lisel(char *nome,  item *i, int item_len);
void lival(int value,  item *i, int item_len);
void litime(int time,  item *i, int item_len);
item* adicionaItem(item *i,int *tam,  char *n, int id, char *ctg, int vb, int cj, int tmp, char *, char *);
item *leFicheiroItem(char *nomeFich, item *i);
int contaItems(char*);
item * eliminaItem(int id, item *i, int *item_len);
int compraItem(item *i, int id, int valor, char *nome, int saldo, int *item_len);

#endif
