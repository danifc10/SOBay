#include <stdio.h>

#include "item.h"

#ifndef USER_H
#define USER_H

typedef struct Utilizador user;

struct Utilizador{
	char *nome;
	char *password;
	item *i;
	int nItem;
	int saldo;
	int bi;
	int idade;
};

void listaUsers ()
#endif
