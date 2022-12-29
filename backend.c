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

typedef struct clients
{
	user *users;
	int tam;
	int file;
	pthread_mutex_t *mutex;
} clients;

// Global Var
static int signal_exit = 0;
item *i;
int item_len;
user *u = NULL;
int user_len;

void signal_handler(int sig)
{
	signal_exit = 1;
}

user *addUser(user *a, int tam, char *nome, pid_t user_pid)
{
	strcpy(u[tam].nome, nome);
	u[tam].saldo = 0;
	u[tam].pid = user_pid;
	u[tam].nItem = 0;
	++(tam);
	return u;
}

int eliminaUser(user *a, int tam, pid_t pid)
{
	for (int i = 0; i < tam; i++)
	{
		if (a[i].pid == pid)
		{
			for (int j = i; j < tam - 1; j++)
			{
				a[j] = a[j + 1];
			}
			return tam - 1;
		}
	}
	return tam;
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

user* getClient(user* u, int tam, pid_t pid){
	for(int i = 0; i < tam; ++i)
		if(u[i].pid == pid){
			return u+i;
		}
}

int atualizaSaldo(user *u, int tam, pid_t pid, int value){
	for(int i = 0; i < tam; i++){
		if(u[i].pid == pid){
			u[i].saldo = value;
			return 1;
		}
	}
	return 0;
}

int getSaldo(user *u, int tam, pid_t pid){
	for(int i = 0; i < tam; i++){
		if(u[i].pid == pid){
			return u[i].saldo;
		}
	}
	return 0;
}

void *answer_clients(void *data)
{
	clients *cli = (clients *)data;

	cli->file = loadUsersFile(FUSERS);
	u = (user *)malloc(sizeof(user) * (cli->file));

	item_len = contaItems(FITEM);
	i = (item *)malloc(item_len * sizeof(item));
	i = leFicheiroItem(FITEM, i);

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
	
	char pipe[PIPE_SIZE];
	pthread_mutex_lock(cli->mutex);

	while ((n = read(fd, &r, sizeof(request))))
	{
		if (!n || signal_exit)
		{
			close(fd);
			closeFrontends(u, cli->tam);
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
				int val = exist(u, cli->tam, r.a.nome);
				if (val){
					resp.valido = 0;
					resp.res = FAILURE;
				}
				else{
					u = addUser(u, cli->tam, r.a.nome, r.pid);
					cli->tam += 1;
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
			resp.value = getSaldo(u, cli->tam, r.pid);
			break;
		case ADD:
			n = atualizaSaldo(u, cli->tam, r.pid, r.add.value);
			if(n){
				resp.res = SUCCESS;
			}else{
				resp.res = FAILURE;
			}
			break;
		case BUY:
			resp.res = SUCCESS;
			int s = compraItem(i, r.buy.id, r.buy.value, u->nome, u->saldo, item_len);
			if (s == 1)
			{
				resp.res = SUCCESS;
				i = eliminaItem(r.buy.id, i, item_len);
				--item_len;
				r.request_type = LIST;
				break;
			}
			else
			{
				resp.res = FAILURE;
				r.request_type = LIST;
			}
			break;
		case SELL:
			resp.res = SUCCESS;
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
			break;
		case LICAT:
			resp.res = SUCCESS;
			break;
		case EXIT:
			u = getClient(u, cli->tam, r.pid);
			printf("\nCliente %d saiu!\n", r.pid);
			cli->tam = eliminaUser(u, cli->tam, r.pid);
			break;
		default:
			printf("tipo de pedido invalido\n");
			resp.res = FAILURE;
			break;
		}

		pthread_mutex_unlock(cli->mutex);

		if (r.request_type == LIST)
		{
			sprintf(pipe, PIPE_CLIENT, r.pid);
			fc = open(pipe, O_WRONLY);
			if (fc == -1)
			{
				printf("error ao abrir pipe cliente\n");
				exit(1);
			}
			for (int j = 0; j < item_len; j++)
			{
				write(fc, &i[j], sizeof(item));
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

	clients cli = {NULL, 0, 0, &mutex};

	pthread_t pipe_thread;
	res = pthread_create(&pipe_thread, NULL, answer_clients, (void *)&cli);
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
			if (cli.tam == 0)
			{
				printf("Nao existem users logados\n");
			}
			else
			{
				showClients(u, cli.tam);
			}
		}
		else if (!strcmp(cmd_request, "list"))
		{
			if (item_len == 0)
			{
				printf("Nao existem items a leilao\n");
			}
			else
			{
				mostraItem(i, item_len);
			}
		}
		else if (!strcmp(cmd_request, "kick"))
		{
			sscanf(cmd, "%s %s", cmd_request, &arg);
			int n = kick_cmd(u, cli.tam, arg);
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