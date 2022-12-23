#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include "user.h"
#include "item.h"
#include "users_lib.h"
#include "utils.h"
#define BACKEND_FIFO "BACKEND"
#define CLIENT_FIFO "CLIENTE%d"
#define HEARTBEAT 4
char CLIENT_FIFO_FINAL[100];


void adicionaItem(item *i, char *n, int id, char *ctg, int vb, int cj, int tmp)
{
	item *new = malloc(sizeof(item));
	new->id = id;
	strcpy(new->categoria, ctg);
	strcpy(new->nome, n);
	new->tempo = tmp;
	new->compra_ja = cj;
	new->valor_base = vb;
	new->prox = NULL;
	if (i->prox == NULL)
	{
		i->prox = new;
	}
	else
	{
		item *aux = i->prox;
		while (aux->prox != NULL)
		{
			aux = aux->prox;
			aux->prox = new;
		}
	}
}

void leFicheiroItem(char *nomeFich, item *i)
{
	FILE *f;
	char Linha[100];

	f = fopen(nomeFich, "rt");

	if (f == NULL)
	{
		printf("ERRO");
		fclose(f);
		return;
	}

	while (!feof(f))
	{

		char nome[100], categoria[100], nomeU[100], licitador[100];
		int id, valor_base, compra_ja, tempo;

		fgets(Linha, 100, f);
		sscanf(Linha, "%d %s %s %d %d %d %s %s", &id, &nome, &categoria, &valor_base, &compra_ja, &tempo, &nomeU, &licitador);

		adicionaItem(i, nome, id, categoria, valor_base, compra_ja, tempo);
		i = i->prox;
	}

	fclose(f);
}

void mostraItem(item *i)
{
	item *aux;
	aux = i->prox;
	while (aux != NULL)
	{
		printf("\n\nID: %d\n", aux->id);
		printf("Nome: %s\n", aux->nome);
		printf("Catg: %s\n", aux->categoria);
		printf("ValorBase: %d\n", aux->valor_base);
		printf("CompraJa: %d\n", aux->compra_ja);
		printf("Tempo: %d\n", aux->tempo);

		aux = aux->prox;
	}
}

void licat(char *ctg, item *i)
{

	while (i)
	{
		if (strcmp(i->categoria, ctg) == 0)
		{
			printf("\n\nID: %d\n", i->id);
			printf("Nome: %s\n", i->nome);
			printf("Catg: %s\n", i->categoria);
			printf("ValorBase: %d\n", i->valor_base);
			printf("CompraJa: %d\n", i->compra_ja);
			printf("Tempo: %d\n", i->tempo);
		}

		i = i->prox;
	}
}
/*
void lisel(char *nome, item *i)
{
	while (a)
	{
		if (strcmp(a->nome, nome) == 0)
		{
			printf("\n\nID: %d\n", a->i->id);
			printf("Nome: %s\n", a->i->nome);
			printf("Catg: %s\n", a->i->categoria);
			printf("ValorBase: %d\n", a->i->valor_base);
			printf("CompraJa: %d\n", a->i->compra_ja);
			printf("Tempo: %d\n", a->i->tempo);
		}
	}
}*/

void lival(int value, item *i)
{

	while (i->prox != NULL)
	{
		if (value >= (i->valor_base))
		{
			printf("\n\nID: %d\n", i->id);
			printf("Nome: %s\n", i->nome);
			printf("Catg: %s\n", i->categoria);
			printf("ValorBase: %d\n", i->valor_base);
			printf("CompraJa: %d\n", i->compra_ja);
			printf("Tempo: %d\n", i->tempo);
		}

		i = i->prox;
	}
}
void litime(int time, item *i)
{

	while (i)
	{
		if (time >= (i->tempo))
		{
			printf("\n\nID: %d\n", i->id);
			printf("Nome: %s\n", i->nome);
			printf("Catg: %s\n", i->categoria);
			printf("ValorBase: %d\n", i->valor_base);
			printf("CompraJa: %d\n", i->compra_ja);
			printf("Tempo: %d\n", i->tempo);
		}

		i = i->prox;
	}
}



int leComandosCliente(char *comando, item *i, dataMsg mensagem)
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
	char nome[20];
	int preco, compreJa, duracao;

	switch (count)
	{
	case 0:
		sscanf(aux, "%s", comando);

		if (strcmp(comando, "list") == 0)
		{
			printf("\nvalido");
			mostraItem(i);
			return 1;
		}
		else if (strcmp(comando, "cash") == 0)
		{
			printf("%s", mensagem.nome);
			printf("\n--- SALDO : %d", getUserBalance(mensagem.nome));
			return 1;
		}
		else if (strcmp(comando, "time") == 0)
		{
			printf("\n valido");
			return 1;
		}
		else if (strcmp(comando, "exit") == 0)
		{
			printf("\n valido\n");
			return 0;
		}
		else
			printf("Comando invalido\n");
		return 1;
		break;

	case 1:

		sscanf(aux, "%s %s", comando, argumento);
		if (strcmp(comando, "licat") == 0)
		{
			printf("\nvalido -  comando: %s --- categoria %s", comando, argumento);
			licat(argumento, i);
			return 1;
		}
		else if (strcmp(comando, "lisel") == 0)
		{
			printf("\nvalido -  comando: %s --- user %s", comando, argumento);
			// lisel(argumento, i);
			return 1;
		}
		else if (strcmp(comando, "litime") == 0)
		{
			int tempo = atoi(argumento);
			printf("\nvalido - comando: %s --- tempo %d", comando, atoi(argumento));
			litime(tempo, i);
			return 1;
		}
		else if (strcmp(comando, "lival") == 0)
		{
			int valor = atoi(argumento);

			printf("\nvalido -  comando: %s --- precoMAx %d", comando, atoi(argumento));
			lival(valor, i);
			return 1;
		}
		else if (strcmp(comando, "add") == 0)
		{
			int value = atoi(argumento);
			updateUserBalance(mensagem.nome, value);
			printf("SAldo: %d", getUserBalance(mensagem.nome));

			return 1;
		}
		else
			printf("Comando invalido\n");
		return 1;
		break;

	case 2:

		sscanf(aux, "%s %s %s", comando, id, valor);
		if (strcmp(comando, "buy") == 0)
		{
			printf("\nvalido -  comando: %s -- id %d -- valor %d", comando, atoi(id), atoi(valor));
			return 1;
		}
		else
			printf("Comando invalido\n");
		return 1;
		break;

	case 5:

		sscanf(aux, "%s %s %s %d %d %d", comando, nome, categoria, &preco, &compreJa, &duracao);

		if (strcmp(comando, "sell") == 0)
		{
			printf("\n valido \n comado: %s \n nome: %s \ncatg: %s \npreco: %d \n compreJ: %d\n tempo: %d\n", comando, nome, categoria, preco, compreJa, duracao);
			adicionaItem(i, nome, 2, categoria, preco, compreJa, duracao);
			return 1;
		}
		else
		{
			printf("Comando invalido\n");
			return 1;
		}

	default:
		break;
	}
}

void handler_sinal(int signal, siginfo_t *info, void *extra)
{

	printf("\n\nDESCONECTADO");
	unlink(CLIENT_FIFO_FINAL);
	exit(1);
}

int main(int argc, char *argv[])
{

	if (argc == 3)
	{	
		dataMsg mensagem;
		dataRPL resposta;
		mensagem.pid = getpid();
		int fd_envio, fd_resposta;

		struct sigaction sac;
		sac.sa_sigaction = handler_sinal;
		sigaction(SIGUSR1, &sac, NULL);

		sprintf(CLIENT_FIFO_FINAL, CLIENT_FIFO, getpid());

		if (mkfifo(CLIENT_FIFO_FINAL, 0666) == -1)
		{

			if (errno == EEXIST)
			{
				printf("\nfifo ja existe\n");
			}
			printf("erro ao abrir fifo\n");
			return 1;
		}

		strcpy(mensagem.nome, argv[1]);
		strcpy(mensagem.pass, argv[2]);

		fd_envio = open(BACKEND_FIFO, O_WRONLY);
		if (fd_envio == -1)
		{ // backend nao esta a correr logo frontend nao corre
			return 2;
		}
		int size = write(fd_envio, &mensagem, sizeof(mensagem));
		close(fd_envio);
		// RECEBER RESPOSTA
		fd_resposta = open(CLIENT_FIFO_FINAL, O_RDONLY);
		if (fd_resposta == -1)
		{
			printf("erro ao abrir fifo do clinte");
		}
		int siz2 = read(fd_resposta, &resposta, sizeof(resposta));
		close(fd_resposta);

		// BACKEND ACEITOU
		int aux;
		if (resposta.res == 1)
		{

			resposta.i = malloc(sizeof(item));
			//leFicheiroItem(FITEM, i);

			char comando[20];
			printf("Bem vindo %s!\n", argv[1]);
			do
			{

				printf("\n>>Deseja testar que comando?");
				fgets(comando, 200, stdin);
				aux = leComandosCliente(comando, resposta.i, mensagem);

				// se sair manda info ao backend
				if (strcmp(comando, "exit") == 0)
				{
					strcpy(mensagem.com, comando);
					fd_envio = open(BACKEND_FIFO, O_WRONLY);
					write(fd_envio, &mensagem, sizeof(mensagem));
					close(fd_envio);
					printf("\nA SAIR..");
					unlink(CLIENT_FIFO_FINAL);
					exit(1);
				}

			} while (aux != 0);
		}
		else // BACKEND NAO ACEITOU
		{
			printf("Erro no login\n");
			unlink(CLIENT_FIFO_FINAL);
			return 3;
		}
		close(fd_envio);
	}
	else
	{
		printf("Erro em numero de argumentos\nSintaxe: ./frontend nome pass\n");
	}

	return 4;
}