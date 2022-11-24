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
#define SERVER_FIFO "SERVIDOR"
#define CLIENT_FIFO "CLIENTE%d"
char CLIENT_FIFO_FINAL[100];

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
			return 1;
		}
		else if (strcmp(comando, "add") == 0)
		{
			printf("\n valido");
			return 1;
		}
		else if (strcmp(comando, "cash") == 0)
		{
			printf("\n valido");
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
			exit(1);
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
			return 1;
		}
		else if (strcmp(comando, "lisel") == 0)
		{
			printf("\nvalido -  comando: %s --- user %s", comando, argumento);
			return 1;
		}
		else if (strcmp(comando, "litime") == 0)
		{
			printf("\nvalido - comando: %s --- tempo %d", comando, atoi(argumento));
			return 1;
		}
		else if (strcmp(comando, "lival") == 0)
		{
			printf("\nvalido -  comando: %s --- precoMAx %d", comando, atoi(argumento));
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
		sscanf(aux, "%s %s %s %s %s %s", comando, nome, categoria, preco, compreJa, duracao);

		if (strcmp(comando, "sell") == 0)
		{
			printf("\n valido \n comado: %s \n nome: %s \ncatg: %s \npreco: %d \n compreJ: %d\n tempo: %d\n", comando, nome, categoria, atoi(preco), atoi(compreJa), atoi(duracao));
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

typedef struct
{
	pid_t pid;
	char nome[100];
	char pass[100];
} dataMsg;

typedef struct
{
	int res;
} dataRPL;

int main(int argc, char *argv[])
{
	int aux;

	if (argc == 3)
	{

		dataMsg mensagem;
		dataRPL resposta;
		mensagem.pid = getpid();
		int fd_envio, fd_resposta;

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
		fd_envio = open(SERVER_FIFO, O_WRONLY);
		int size = write(fd_envio, &mensagem, sizeof(mensagem));
		close(fd_envio);
		fd_resposta = open(CLIENT_FIFO_FINAL, O_RDONLY);
		if (fd_resposta == -1)
		{
			printf("erro ao abrir fifo do clinte");
		}
		int siz2 = read(fd_resposta, &resposta, sizeof(resposta));
		close(fd_resposta);
		
		if (resposta.res == 1)
		{
			char comando[20];
			printf("Bem vindo %s!\n", argv[1]);
			do
			{
				printf("\n>>Deseja testar que comando?");
				fgets(comando, 200, stdin);
				aux = leComandosCliente(comando);
			} while (aux != 0);
		}else{
			printf("permissao recusada");
		}
	}
	else
	{
		printf("Erro em numero de argumentos\n");
	}

	return 0;
}