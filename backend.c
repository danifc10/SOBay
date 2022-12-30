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


user* eliminaUser(user *a, int *tam, pid_t pid)
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
	if(*tam == 0){
		free(a);
		return NULL;
	}
	user* c = (user*)realloc(a, sizeof(user)* ((*tam)));
	if(c == NULL){
		printf("error allocating memory\n");
		exit(1);
	}
	return c;
}



void showClients(user *u, int tam)
{
	for (int i = 0; i < tam; i++)
	{
		printf("Pid:%d\tNome:%s\tSaldo:%d\n", u[i].pid, u[i].nome, u[i].saldo);
	}
}

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

user *getClient(user *u, int tam, pid_t pid)
{
	for (int i = 0; i < tam; ++i)
		if (u[i].pid == pid)
		{
			return u + i;
		}
}

int atualizaSaldo(user *u, int tam, pid_t pid, int value)
{
	for (int i = 0; i < tam; i++)
	{
		if (u[i].pid == pid)
		{
			u[i].saldo = value;
			return 1;
		}
	}
	return 0;
}

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

void *answer_clients(void *data)
{
	structs *st = (structs *)data;
	loadUsersFile(FUSERS);
	st->i = leFicheiroItem(FITEM, st->i);
	st->itam = contaItems(FITEM);
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

	char pipe[PIPE_SIZE];
	pthread_mutex_lock(st->mutex);

	while ((n = read(fd, &r, sizeof(request))))
	{
		if (!n || signal_exit)
		{
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
			valido = isUserValid(r.a.nome, r.a.pass);
			if (valido == 1)
			{
				int val = exist(st->u, st->utam, r.a.nome);
				if (val)
				{
					resp.valido = 0;
					resp.res = FAILURE;
				}
				else
				{
					st->u = addUser(st->u, &(st->utam), r.a.nome, r.pid);
					resp.valido = 1;
					resp.res = SUCCESS;
				}
			}
			else
			{
				resp.valido = 0;
				resp.res = FAILURE;
			}
			break;
		case CASH:
			resp.res = SUCCESS;
			resp.value = getSaldo(st->u, st->utam, r.pid);
			break;
		case ADD:
			resp.res = SUCCESS;
			valido = atualizaSaldo(st->u, st->utam, r.pid, r.add.value);
			if (valido)
			{
				resp.res = SUCCESS;
			}
			else
			{
				resp.res = FAILURE;
			}
			break;
		case BUY:
			resp.res = SUCCESS;
			u = getClient(st->u, st->utam, r.pid);
			valido = compraItem(st->i, r.buy.id, r.buy.value, u->nome, u->saldo, &(st->itam));
			if (valido)
			{
				resp.res = SUCCESS;
				st->i = eliminaItem(r.buy.id, st->i, &(st->itam));
				resp.value = st->itam;
				break;
			}
			resp.res = FAILURE;
			break;
		case SELL:
			resp.res = SUCCESS;
			user *u = getClient(st->u, st->utam, r.pid);
			int id = getId(st->itam);
			st->i = adicionaItem(st->i, &(st->itam), r.sell.nome, id, r.sell.categoria, r.sell.value, r.sell.compra, r.sell.duracao, u->nome, "-");
			resp.res = SUCCESS;
			resp.value = st->itam;
			break;
		case TIME:
			resp.res = SUCCESS;
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

	pthread_t pipe_thread;
	res = pthread_create(&pipe_thread, NULL, answer_clients, (void *)&st);
	if (res != 0)
	{
		printf("error a criar thread");
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
				printf("PID: %d\tSaldo: %d\tNome: %s\n", st.u[i].pid, st.u[i].saldo, st.u[i].nome);
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
		}
		else
		{
			printf("invalid command\n");
			continue;
		}
		strcpy(cmd, "");
	}

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

	printf("Bye\n");
	exit(0);
}