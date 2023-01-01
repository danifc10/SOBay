#include "util.h"
#include "item.h"
#include "users_lib.h"

typedef struct user
{
	char nome[100];
	char password[100];
	int nItem;
	int saldo;
	pid_t pid;
} user;

typedef struct structs
{
	item *i;
	int itam;
	int utam;
	user *u;
	pthread_mutex_t *mutex;
} structs;

// Global Var
static int signal_exit = 0;
static int signal_notif = 1;
static int tempo = 0;

void signal_handler(int sig)
{
	signal_exit = 1;
}

user *addUser(user *a, int *tam, char *nome, pid_t user_pid)
{
	user *u = (user *)realloc(a, sizeof(user) * ((*tam) + 1));
	if (u == NULL)
	{
		printf("erro ao alocar memoria\n");
		return NULL;
	}
	strcpy(u[*tam].nome, nome);
	u[*tam].saldo = 0;
	u[*tam].pid = user_pid;
	u[*tam].nItem = 0;
	++(*tam);
	return u;
}

user *eliminaUser(user *a, int *tam, pid_t pid)
{
	for (int i = 0; i < *tam; i++)
	{
		if (a[i].pid == pid)
		{
			for (int j = i; j < ((*tam) - 1); j++)
			{
				a[j] = a[j + 1];
			}
			break;
		}
	}
	--(*tam);
	if (*tam == 0)
	{
		free(a);
		return NULL;
	}
	user *c = (user *)realloc(a, sizeof(user) * ((*tam)));
	if (c == NULL)
	{
		printf("error allocating memory\n");
		exit(1);
	}
	return c;
}

// 1 elimina / 0 nao elimina
int kick_cmd(user *u, int tam, char *nome)
{
	for (int i = 0; i < tam; i++)
	{
		if (!strcmp(u[i].nome, nome))
		{
			kill(u[i].pid, SIGINT);
			return 1;
		}
	}
	return 0;
}

// 1 se existir, 0 se nao
int exist(user *u, int tam, char *nome)
{
	for (int j = 0; j < tam; j++)
	{
		if (strcmp(u[j].nome, nome) == 0)
		{
			return 1;
		}
	}
	return 0;
}

void closeFrontends(user *u, int tam)
{
	union sigval val;
	for (int j = 0; j < tam; j++)
	{
		sigqueue(u[j].pid, SIGINT, val);
	}
}

// retorna o user atual
user *getClient(user *u, int tam, pid_t pid)
{
	for (int i = 0; i < tam; i++)
		if (u[i].pid == pid)
		{
			return u + i;
		}
}
item *getItem(item *i, int tam, int id)
{
	for (int j = 0; j < tam; j++)
		if (i[j].id == id)
		{
			return i + j;
		}
}

// retorna 1 se atualizou e 0 se nao
int atualizaSaldo(user *u, int tam, pid_t pid, int value)
{
	for (int i = 0; i < tam; i++)
	{
		if (u[i].pid == pid)
		{
			u[i].saldo += value;
			return 1;
		}
	}
	return 0;
}

// retorna o saldo
int getSaldo(user *u, int tam, pid_t pid)
{
	for (int i = 0; i < tam; i++)
	{
		if (u[i].pid == pid)
		{
			return u[i].saldo;
		}
	}
	return 0;
}

item *verificaLeilao(item *i, int *tam, user *u, int utam)
{
	int time = tempo;
	for (int j = 0; j < *tam; j++)
	{
		if (tempo == (i[j].tempo + i[j].tempoInicio))
		{
			for (int a = 0; a < utam; a++)
			{
				if (!strcmp(i[j].licitador, u[a].nome))
				{
					++(u[a].nItem);
					u[a].saldo -= i[j].valor_base;
				}
			}
			for (int a = 0; a < utam; a++)
			{
				if (!strcmp(i[j].dono, u[a].nome))
					u[a].saldo += i[j].valor_base;
			}
			i = eliminaItem(i[j].id, i, tam);
			// enviar notificações
			signal_notif = 1;
		}
	}
	return i;
}
/*
void enviaSinal(user *u, int tam, pid_t pid)
{
	union sigval sig;
	for (int i = 0; i < tam; i++)
	{
		if(u[i].pid != pid){
			sigqueue(u[i].pid, SIGCHLD, sig);
		}
	}
	return;
}*/

void *answer_clients(void *data)
{
	structs *st = (structs *)data;
	int res = access(PIPE_SERVER, F_OK);
	if (res != 0)
	{
		res = mkfifo(PIPE_SERVER, 0666);
		if (res != 0)
		{
			printf("error creating the fifo %s", PIPE_SERVER);
			exit(1);
		}
	}

	int fd = open(PIPE_SERVER, O_RDWR);
	if (fd == -1)
	{
		printf("error opening the pipe %s", PIPE_SERVER);
		exit(1);
	}

	int n, fc, buy = 0;
	request r;
	response resp;
	user *u = NULL;
	item *it = NULL;

	char pipe[PIPE_SIZE];
	pthread_mutex_lock(st->mutex);

	while ((n = read(fd, &r, sizeof(request))))
	{
		if (!n || signal_exit)
		{
			saveUsersFile(FUSERS);
			atualizaFitems(st->i, (st->itam), FITEM, tempo);
			close(fd);
			closeFrontends(st->u, st->utam);
			unlink(PIPE_SERVER);
			pthread_exit(NULL);
		}
		else if (n != sizeof(request))
		{
			printf("errro a ler msg\n");
			exit(1);
		}

		int valido = 0;
		switch (r.request_type)
		{
		case ENTRADA:
			if ((st->utam) == MAX_CLIENTS)
			{
				resp.valido = 0;
				resp.res = FAILURE;
				break;
			}
			valido = isUserValid(r.a.nome, r.a.pass);
			if (valido)
			{
				valido = exist(st->u, st->utam, r.a.nome);
				if (valido)
				{
					resp.valido = 0;
					resp.res = FAILURE;
					break;
				}

				st->u = addUser(st->u, &(st->utam), r.a.nome, r.pid);
				resp.valido = 1;
				resp.res = SUCCESS;
				break;
			}

			resp.valido = 0;
			resp.res = FAILURE;
			break;
		case CASH:
			resp.res = SUCCESS;
			resp.value = getSaldo(st->u, st->utam, r.pid);
			break;
		case ADD:
			if (r.buy.value < 0)
			{
				resp.res = FAILURE;
				break;
			}
			valido = atualizaSaldo(st->u, st->utam, r.pid, r.add.value);
			if (valido)
				resp.res = SUCCESS;
			else
				resp.res = FAILURE;
			break;
		case BUY:
			u = getClient(st->u, st->utam, r.pid);
			it = getItem(st->i, st->itam, r.buy.id);
			valido = compraItem(st->i, r.buy.id, r.buy.value, u->nome, u->saldo, &(st->itam));
			if (valido == 1) // licita
			{
				resp.res = SUCCESS;
				strcpy(it->licitador, u->nome);
				it->valor_base = r.buy.value;
				resp.value = st->itam;
				break;
			}
			else if (valido == 2) // compra ja
			{
				resp.res = SUCCESS;
				u->nItem++;
				u->saldo -= it->compra_ja;
				for (int j = 0; j < st->utam; j++)
				{
					if (!strcmp(it->dono, u[j].nome))
					{
						u[j].saldo += it->compra_ja;
					}
				}
				it = eliminaItem(it->id, st->i, &(st->itam));
				resp.value = st->itam;
				break;
			}
			resp.res = FAILURE; // erros
			// envia notificaces
			break;
		case SELL:
			if ((st->itam) == MAX_ITEMS)
			{
				resp.value = st->itam;
				resp.res = FAILURE;
				break;
			}
			u = getClient(st->u, st->utam, r.pid);
			int id = getId(st->itam);
			int tam = st->itam;
			st->i = adicionaItem(st->i, &(st->itam), r.sell.nome, id, r.sell.categoria, r.sell.value, r.sell.compra, r.sell.duracao, u->nome, "-", tempo);
			resp.res = SUCCESS;
			resp.value = st->itam;
			r.request_type = SELL;
			// resp.notif = 1;
			//  cria notificacaos
			break;
		case TIME:
			resp.res = SUCCESS;
			resp.value = tempo;
			break;
		case LISEL:
			resp.res = SUCCESS;
			break;
		case LIVAL:
			resp.res = SUCCESS;
			break;
		case LITIME:
			resp.res = SUCCESS;
			break;
		case LIST:
			resp.res = SUCCESS;
			resp.value = st->itam;
			break;
		case LICAT:
			resp.res = SUCCESS;
			break;
		case EXIT:
			printf("\nCliente %d saiu!\n", r.pid);
			st->u = eliminaUser(st->u, &(st->utam), r.pid);
			break;
		default:
			printf("tipo de pedido invalido\n");
			resp.res = FAILURE;
			break;
		}

		pthread_mutex_unlock(st->mutex);

		if (r.request_type == SELL || r.request_type == BUY || r.request_type == LIST)
		{
			sprintf(pipe, PIPE_CLIENT, r.pid);
			fc = open(pipe, O_WRONLY);
			if (fc == -1)
			{
				printf("error ao abrir pipe cliente\n");
				exit(1);
			}

			write(fc, &resp, sizeof(response));

			for (int j = 0; j < st->itam; j++)
			{
				write(fc, &st->i[j], sizeof(item));
			}

			close(fc);
		}
		else if (r.request_type != EXIT)
		{
			sprintf(pipe, PIPE_CLIENT, r.pid);
			fc = open(pipe, O_WRONLY);
			if (fc == -1)
			{
				printf("error ao abrir pipe cliente\n");
				exit(1);
			}
			write(fc, &resp, sizeof(response));
			close(fc);
		}
	}
}

void *handler_time(void *data)
{
	structs *st = (structs *)data;
	while (1)
	{
		++tempo;
		sleep(1);
		st->i = verificaLeilao(st->i, &(st->itam), st->u, st->utam);
		// mostraItem(st->i, st->itam);
	}
}

int main()
{
	setbuf(stdout, NULL);
	char *cmd = NULL;
	size_t n_chars, cmd_size;
	char cmd_request[100];
	char arg[100];

	struct sigaction sa;

	sa.sa_handler = signal_handler;
	sa.sa_flags = SA_SIGINFO;
	sigaction(SIGUSR1, &sa, NULL);
	sigaction(SIGINT, &sa, NULL);

	pthread_mutex_t mutex;
	int res = pthread_mutex_init(&mutex, NULL);
	if (res != 0)
	{
		printf("error ao inicializar mutex\n");
		exit(1);
	}

	structs st = {NULL, 0, 0, NULL, &mutex};
	st.i = leFicheiroItem(FITEM, st.i, &(st.itam));
	loadUsersFile(FUSERS);

	pthread_t pipe_thread;
	pthread_t tempo_thread;
	res = pthread_create(&pipe_thread, NULL, answer_clients, (void *)&st);
	if (res != 0)
	{
		printf("error a criar thread");
		exit(1);
	}
	res = pthread_create(&tempo_thread, NULL, handler_time, (void *)&st);
	if (res != 0)
	{
		printf("error a criar thread do tempo");
		exit(1);
	}

	printf("Serv is running...\n");

	while (1)
	{
		printf("\n>>");
		n_chars = getline(&cmd, &cmd_size, stdin);
		cmd[n_chars - 1] = '\0';
		sscanf(cmd, "%s", cmd_request);

		if (!strcmp(cmd_request, "close") || signal_exit)
		{
			printf("Closing..\n");
			break;
		}
		else if (!strcmp(cmd_request, "users"))
		{
			pthread_mutex_lock(&mutex);
			if (st.utam == 0)
				printf("No clients to display\n");
			for (int i = 0; i < st.utam; ++i)
			{
				printf("PID: %d\tSaldo: %d\tNome: %s\tN_items: %d\n", st.u[i].pid, st.u[i].saldo, st.u[i].nome, st.u[i].nItem);
			}

			pthread_mutex_unlock(&mutex);
		}
		else if (!strcmp(cmd_request, "list"))
		{
			if (st.itam == 0)
			{
				printf("Nao existem items a leilao\n");
			}
			else
			{
				mostraItem(st.i, st.itam);
			}
		}
		else if (!strcmp(cmd_request, "kick"))
		{
			sscanf(cmd, "%s %s", cmd_request, &arg);
			int n = kick_cmd(st.u, st.utam, arg);
			if(n)
				printf("SUCCESS\n");
			else
				printf("FAILURE\n");
		}
		else
		{
			printf("FAILURE\n");
			continue;
		}
		strcpy(cmd, "");
	}

	saveUsersFile(FUSERS);

	res = pthread_kill(pipe_thread, SIGUSR1);
	if (res != 0)
	{
		printf("error ao enviar sinal para a thread");
		exit(1);
	}

	res = pthread_join(pipe_thread, NULL);
	if (res != 0)
	{
		printf("error ao esperar pela thread\n");
		exit(1);
	}

	res = pthread_mutex_destroy(&mutex);
	if (res != 0)
	{
		exit(1);
	}

	res = pthread_kill(tempo_thread, SIGUSR1);
	if (res != 0)
	{
		printf("error ao enviar sinal para a thread");
		exit(1);
	}

	res = pthread_join(tempo_thread, NULL);
	if (res != 0)
	{
		printf("error ao esperar pela thread\n");
		exit(1);
	}

	printf("Bye\n");
	exit(0);
}