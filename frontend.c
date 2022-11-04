#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "user.h"
#include "item.h"

user *recuperaUtilizadores(char *filename, int *tamanho)
{
	FILE *f;
	f = fopen(filename, "rt");
	if (f == NULL)
	{
		printf("Erro a abrir %s", filename);
		return NULL;
	}
	else
	{
		user *a = NULL;
		int numUsers;
		if(fscanf(f, "%d\n", &numUsers)==EOF){
			numUsers=0;
			a=malloc(sizeof(user));
		}else{
			a=malloc(sizeof(user)*numUsers);
		}
		
		
		if (a == NULL)
		{
			return NULL;
		}
		char buffer[500];
		int i = 0;
		while (feof(f) == 0)
		{
			fgets(buffer, 500, f);
			sscanf(buffer, "%s %s %d", a[i].nome, a[i].password, &a[i].saldo);
			i++;
		}
		*tamanho = numUsers;
		fclose(f);

		return a;
	}
}

void escreveFicheiro(char *filename, user *a, int tamanho)
{
	FILE *f;
	f = fopen(filename, "wt");
	if (f == NULL)
	{
		return;
	}
	int i = 0;
	fprintf(f, "%d\n", tamanho);
	while (i < tamanho)
	{
		fprintf(f, "%s %s %d\n", a[i].nome, a[i].password, a[i].saldo);

		i++;
	}
	fclose(f);
}

user *adicionaUser(user *a, int *tamanho, char n[], char pass[])
{

	user *aux = NULL;
	aux = malloc(sizeof(user));
	if (aux == NULL)
	{
		return NULL;
	}
	else
	{
		strcpy(aux->nome, n);
		strcpy(aux->password, pass);
		aux->saldo = 0;
		a[*tamanho] = *aux;
		*tamanho = *tamanho + 1;
	}
	return a;
}

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
		int tamanho = 0;
		user *a = NULL;
		a = recuperaUtilizadores("ficheiro_utilizadores.txt", &tamanho);
		a = adicionaUser(a, &tamanho, argv[1], argv[2]);
		escreveFicheiro("ficheiro_utilizadores.txt", a, tamanho);
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