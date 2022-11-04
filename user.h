#include <stdio.h>

#include "item.h"

#ifndef USER_H
#define USER_H
#define USER_FILENAME "ficheiro_utilizadores.txt"
typedef struct Utilizador user;

struct Utilizador{
	char nome[100];
	char password[100];
	item *i;
	int nItem;
	int saldo;
};

void listaUsers ();
#endif
