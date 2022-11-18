#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "user.h"
#include "item.h"

int leComandosCliente(char *comando)
{
	char aux[100];
	int count = 0; // variavel para contar os espaços em branco

	for (int i = 0; i < strlen(comando) - 1; i++)
	{
		if (isspace(comando[i]))
		{
			count++;
		}
	}

	strcpy(aux, comando);

	char argumento[20];
	char valor[5], id[5];
	char categoria[20];
	char nome[20], preco[6], compreJa[6], duracao[6];

	switch (count)
	{
	case 0:
		sscanf(aux, "%s", comando);

		if (strcmp(comando, "list") == 0)
		{
			printf("\nvalido");
		}
		else if (strcmp(comando, "add") == 0)
		{
			printf("\n valido");
		}
		else if (strcmp(comando, "cash") == 0)
		{
			printf("\n valido");
		}
		else if (strcmp(comando, "time") == 0)
		{
			printf("\n valido");
		}
		else if (strcmp(comando, "exit") == 0)
		{
			printf("\n valido");
			exit(1);
		}
		else
			printf("Comando invalido");
		break;

	case 1:

		sscanf(aux, "%s %s", comando, argumento);
		if (strcmp(comando, "licat") == 0)
		{
			printf("\nvalido -  comando: %s --- categoria %s", comando, argumento);
		}
		else if (strcmp(comando, "lisel") == 0)
		{
			printf("\nvalido -  comando: %s --- user %s", comando, argumento);
		}
		else if (strcmp(comando, "litime") == 0)
		{
			printf("\nvalido - comando: %s --- tempo %d", comando, atoi(argumento));
		}
		else if (strcmp(comando, "lival") == 0)
		{
			printf("\nvalido -  comando: %s --- precoMAx %d", comando, atoi(argumento));
		}
		else
			printf("Comando invalido");
		break;

	case 2:

		sscanf(aux, "%s %s %s", comando, id, valor);
		if (strcmp(comando, "buy") == 0)
		{

			printf("\nvalido -  comando: %s -- id %d -- valor %d", comando, atoi(id), atoi(valor));
		}
		else
			printf("Comando invalido");
		break;

	case 5:
		sscanf(aux, "%s %s %s %s %s %s", comando, nome, categoria, preco, compreJa, duracao);

		if (strcmp(comando, "sell") == 0)
		{
			printf("\n valido \n comado: %s \n nome: %s \ncatg: %s \npreco: %d \n compreJ: %d\n tempo: %d\n", comando, nome, categoria, atoi(preco), atoi(compreJa), atoi(duracao));
		}
		else
		{
			printf("Comando invalido");
		}

	default:
		break;
	}
}

int main(int argc , char *argv[])
{
	if (argc == 3)
	{
		char comando[20];
		fgets(comando, 200, stdin);
		leComandosCliente(comando);
	}
	else
	{
		printf("Falta de argumentos\n");
	}
	

	return 0;
}