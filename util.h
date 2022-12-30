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
#define PIPE_SIZE 20
#define FUSERS "ficheiro_utilizadores.txt"

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
	ENTRADA
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
	union
	{
		sell_cmd sell;
		lists_cmd lists;
		buy_cmd buy;
		add_cmd add;
		entrada a;
	};
} request;

typedef struct response
{
	enum RequestResult res;
	int value;
	int valido;
} response;

#endif