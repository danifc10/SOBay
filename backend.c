#include <stdio.h>
#include <ctype.h>
#include "item.h"
#include "user.h"
#include <stdlib.h>
#include <stdio.h>

item *i;
void mostraItem()
{
	while (i)
	{
		printf("\n\nID: %d\n", i->id);
		printf("Nome: %s\n", i->nome);
		printf("Catg: %s\n", i->categoria);
		printf("ValorBase: %d\n", i->valor_base);
		printf("CompraJa: %d\n", i->compra_ja);
		printf("Tempo: %d\n", i->tempo);

		i = i->prox;
	}
}

void adicionaItem(item **i, char *n, int id, char *ctg, int vb, int cj, int tmp)
{
	item *aux, *new = malloc(sizeof(item));

	if (new)
	{
		strcpy((new->nome), n);
		strcpy((new->categoria), ctg);
		new->id = id;
		new->tempo = tmp;
		new->valor_base = vb;
		new->compra_ja = cj;
		new->prox = NULL;

		if (*i == NULL)
		{
			*i = new;
		}
		else
		{
			aux = *i;
			while (aux->prox)
			{
				aux = aux->prox;
			}

			aux->prox = new;
		}
	}
	else
	{
		printf("erro ao alocar memoria\n");
	}
}

void leFicheiroItem(char *nomeFich)
{
	FILE *f;
	user *u;
	char Linha[100];

	f = fopen("items.txt", "rb");

	if (f == NULL)
	{
		printf("Erro ao abrir ficheiro");
		return;
	}

	while (!feof(f))
	{

		char nome[100], categoria[100], nomeU[100], licitador[100];
		int id, valor_base, compra_ja, tempo;

		fgets(Linha, 100, f);
		sscanf(Linha, "%d %s %s %d %d %d %s %s", &id, &nome, &categoria, &valor_base, &compra_ja, &tempo, &nomeU, &licitador);

		adicionaItem(&i, nome, id, categoria, valor_base, compra_ja, tempo);
	}

	fclose(f);
}

int leComandosAdmin(char *comando)
{
	char aux[100];
	char argumento[20];
	int count = 0;

	for (int i = 0; i < strlen(comando) - 1; i++)
	{
		if (isspace(comando[i]))
		{
			count++;
		}
	}

	strcpy(aux, comando);

	switch (count)
	{
	case 0:
		sscanf(aux, "%s", comando);

		if (strcmp(comando, "list") == 0)
		{
			printf("\n valido");
			return 1;
		}
		else if (strcmp(comando, "users") == 0)
		{
			printf("\n valido");
			return 1;
		}
		else if (strcmp(comando, "prom") == 0)
		{
			printf("\n valido");
			return 1;
		}
		else if (strcmp(comando, "reprom") == 0)
		{
			printf("\n valido");
			return 1;
		}
		else if (strcmp(comando, "close") == 0)
		{
			printf("\n valido");
			return 0; // comando valido mas retorna 0 porque vai fechar
		}
		else
			printf("\n\nComando Invalido");
		return 0;
		break;

	case 1:

		sscanf(aux, "%s %s", comando, argumento);
		if (strcmp(comando, "kick") == 0)
		{
			printf("\nvalido comando: %s user %s", comando, argumento);
			return 1;
		}
		else if (strcmp(comando, "cancel") == 0)
		{
			printf("\n valido comando: %s nomePromotor: %s", comando, argumento);
			return 1;
		}
		else
			printf("\n\nComando Invalido");
		return 0;
		break;

	default:
		printf("\n\nComando Invalido");
		return 0;
		break;
	}
}

int main()
{
	char comando[20];
	int aux = 0;

	/*
		 //para testar a leitura e o save dos items ::
		// leFicheiroItem("items.txt");
		// mostraItem();
	*/
	do
	{
		leFicheiroItem("items.txt");
		printf("\n\n Deseja testar que funcionalidade?\n");
		fgets(comando, 200, stdin);
		aux = leComandosAdmin(comando);

	} while (aux != 0);

	return 0;
}
