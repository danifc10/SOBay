
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
	struct Item *prox;
};

void mostraItem(item *i);
void licat(char *ctg, item *i);
void lisel(char *nome,  item *i);
void lival(int value,  item *i);
void litime(int time,  item *i);
void adicionaItem(item *i, char *n, int id, char *ctg, int vb, int cj, int tmp);
void leFicheiroItem(char *nomeFich, item *i);

#endif
