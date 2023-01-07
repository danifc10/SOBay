#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include "item.h"

#define PIPE_SERVER "servidor"
#define PIPE_CLIENT "p_cli_%d"
#define MAX_CLIENTS 20
#define MAX_ITEMS 30
#define MAX_PROMS 10
#define PIPE_SIZE 20
#define FUSERS "ficheiro_utilizadores.txt"
#define FITEM "items.txt"
#define FPROMS "ficheiro_promotores.txt"

enum RequestType
{
	LIST,
	EXIT,
	CASH,
	TIME,
	LICAT,
	LISEL,
	LITIME,
	LIVAL,
	ADD,
	BUY,
	SELL,
	ENTRADA,
	FAIL, 
	NOTIF
};

enum RequestResult
{
	FAILURE,
	SUCCESS
};

typedef struct sell_cmd
{
	int value;
	int compra;
	int duracao;
	char categoria[30];
	char nome[30];
	char user[30];
} sell_cmd;

typedef struct lists_cmd
{
	char arg[100];
} lists_cmd;

typedef struct buy_cmd
{
	int id;
	int value;
} buy_cmd;

typedef struct add_cmd
{

	int value;
} add_cmd;

typedef struct entrada
{
	char nome[20];
	char pass[20];
	int pid;
	int valido;
} entrada;

typedef struct request
{
	pid_t pid;
	enum RequestType request_type;
	int aux;
	union
	{
		sell_cmd sell;
		lists_cmd lists;
		buy_cmd buy;
		add_cmd add;
		entrada a;
	};
} request;

enum NotificacaoType
{
	VENDA,
	COMPRA,
	PROM
};

typedef struct response
{
	enum RequestResult res;
	int value;
	int valido;
} response;

typedef struct notificacao{
	enum NotificacaoType notType;
	int id;
	char nomeI[20];
	char nomeU[20];
	char ctg[20];
	int preco;
	int compraJa;
	int duracao;
}notificacao;
#endif