#include <stdio.h>
#include <ctype.h>
#include "item.h"
#include "user.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include "users_lib.h"

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

int executaPromotor(int fd_p2b[2])
{
	int f = fork();
	if (f == -1)
	{
		printf("erro ao criar filho\n");
	}
	else if (f == 0)
	{
		close(1);
		dup(fd_p2b[1]);
		close(fd_p2b[0]);
		close(fd_p2b[1]);
		execl("./promotor_oficial", "promotor_oficial", NULL);
	}
	return f;
}
char *recebePromotor(int fd_p2b[2])
{
	char msg[100];
	read(fd_p2b[0], msg, 100);
	return strtok(msg, "\n");
}

int loadUsersFile(char *pathname)
{

	FILE *f;
	char Linha[100];

	f = fopen(pathname, "rb");

	if (f == NULL)
	{
		printf("\nErro ao abrir ficheiro");
		return -1;
	}
	int i = 0;
	while (fgets(Linha, 100, f))
	{
		char username[100], password[100];
		int saldo;
		sscanf(Linha, "%s %s %d", &username, &password, &saldo);
		i++;
	}
	fclose(f);
	return i;
}

int isUserValid(char *username, char *password)
{

	FILE *f;
	char Linha[100];

	f = fopen("ficheiro_utilizadores.txt", "rb");

	if (f == NULL)
	{
		printf("\nErro ao abrir ficheiro");
		return -1;
	}
	int i = 0;
	int aux = 0;
	while (fgets(Linha, 100, f))
	{
		char user[100], pass[100];
		int saldo;

		sscanf(Linha, "%s %s %d", &user, &pass, &saldo);

		if (strcmp(username, user) == 0)
		{
			aux++;
			if (strcmp(pass, password) == 0)
			{
				aux++;
			}
		}
	}

	fclose(f);

	if (aux == 2)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
int main()
{
	char outputPromotores[100];
	// maximo de promotores
	int pid_promotor[10];

	for (int i = 0; i < 10; i++)
	{
		pid_promotor[i] = 0;
	}

	char comando[20];
	int aux = 0;
	// criar 2 unnamed pipes
	int fd_p2b[2];
	// pipes
	int Rpipe = pipe(fd_p2b);

	if (Rpipe == -1)
	{
		printf("erro ao criar pipe\n");
		exit(1);
	}

	int pid = executaPromotor(fd_p2b);

	for (int i = 0; i < 10; i++)
	{
		if (pid_promotor[i] == 0)
		{
			pid_promotor[i] = pid;
			break;
		}
	}

	// --------------variaveis para teste------------
	//char comando[100];
	char *nome = "Daniela";
	char *pass = "ola";
	int aux1;
	char *nomeF = "ficheiro_utilizadores.txt";
	leFicheiroItem("items.txt");
	mostraItem();
	printf("\npid backend: %d pid promotor: %d\n", getpid(), pid);
	do
	{
		loadUsersFile(nomeF);
		int b = isUserValid(nome, pass); // 1 se existe 0 se nao existe ou pass errada
		printf("%d\n", b);
		printf("\n\n Deseja testar que funcionalidade?\n");
		fgets(comando, 200, stdin);
		aux1 = leComandosAdmin(comando);

	} while (aux1 != 0);

	char resposta;
	printf("\nDeseja lançar um promotor ?(y/n)\n");
	scanf("%c", &resposta);
	union sigval valores;
	valores.sival_int = -1;
	if (resposta == 'y')
	{
		int i = 0; // tem que aparecer 3 promo antes de  terminar o processo;
		while (1)
		{
			strcpy(outputPromotores, recebePromotor(fd_p2b));
			printf("\nmsg:%s\n", outputPromotores);
			if (i == 2)
			{
				sigqueue(pid, SIGUSR1, valores); // fechar promotor
			}
			i++;
		}
	}

	return 0;
}
