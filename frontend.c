#include "util.h"
#include "item.h"

#define CMD_SIZE 100

// Global Var
static int signal_exit = 0;
static int signal_notif = 0;

void signal_handler(int sig)
{
	if (sig == SIGCHLD)
	{
		signal_notif = 1;
		return;
	}
	signal_exit = 1;
}

void main(int argc, char *argv[])
{

	char *cmd = NULL;
	size_t n_chars, cmd_size;
	int amount, destination;
	char cmd_request[CMD_SIZE];
	char pipe[PIPE_SIZE];

	struct sigaction sa;
	sa.sa_flags = SA_SIGINFO;
	sa.sa_handler = signal_handler;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGPIPE, &sa, NULL);
	sigaction(SIGCHLD, &sa, NULL);

	request r;
	r.pid = getpid();

	item *i = NULL;
	int item_len = 0;

	sprintf(pipe, PIPE_CLIENT, r.pid);
	int res = access(pipe, F_OK);
	if (res != 0)
	{
		res = mkfifo(pipe, 0666);
		if (res != 0)
		{
			printf("error ao criar o fifo %s", pipe);
			exit(1);
		}
	}

	int fd = open(PIPE_SERVER, O_WRONLY);
	if (fd == -1)
	{
		printf("error - servidor nao disponivel\n");
		exit(1);
	}

	printf("Connecting...\n");
	int fc = open(pipe, O_RDWR);
	if (fc == -1)
	{
		printf("erro ao abrir pipe cliente\n");
		exit(1);
	}

	int n, value, value2;
	char argumento[100];
	response resp;
	int aux = 0;

	do
	{
		if (aux == 0)
		{
			r.request_type = ENTRADA;
			strcpy(r.a.nome, argv[1]);
			strcpy(r.a.pass, argv[2]);
		}
		else
		{
			printf("\nOperation:\n->");
			n_chars = getline(&cmd, &cmd_size, stdin);
			cmd[n_chars - 1] = '\0';
			sscanf(cmd, "%s", cmd_request);

			if (!strcmp(cmd_request, "exit") || signal_exit == 1)
			{
				r.request_type = EXIT;
			}
			else if (!strcmp(cmd_request, "list"))
			{
				r.request_type = LIST;
			}
			else if (!strcmp(cmd_request, "cash"))
			{
				r.request_type = CASH;
			}
			else if (!strcmp(cmd_request, "time"))
			{
				r.request_type = TIME;
			}
			else if (!strcmp(cmd_request, "licat"))
			{
				r.request_type = LICAT;
				sscanf(cmd, "%s %s", cmd_request, &argumento);
				licat(argumento, i, item_len);
			}
			else if (!strcmp(cmd_request, "lisel"))
			{
				r.request_type = LISEL;
				sscanf(cmd, "%s %s", cmd_request, &argumento);
				lisel(argumento, i, item_len);
			}
			else if (!strcmp(cmd_request, "lival"))
			{
				r.request_type = LIVAL;
				sscanf(cmd, "%s %d", cmd_request, &value);
				lival(value, i, item_len);
			}
			else if (!strcmp(cmd_request, "litime"))
			{
				r.request_type = LITIME;
				sscanf(cmd, "%s %d", cmd_request, &value);
				litime(value, i, item_len);
			}
			else if (!strcmp(cmd_request, "add"))
			{
				r.request_type = ADD;
				sscanf(cmd, "%s %d", cmd_request, &value);
				r.add.value = value;
			}
			else if (!strcmp(cmd_request, "buy"))
			{
				r.request_type = BUY;
				sscanf(cmd, "%s %d %d", cmd_request, &value, &value2);
				r.buy.id = value;
				r.buy.value = value2;
			}
			else if (!strcmp(cmd_request, "sell"))
			{
				char nome[30] = "", ctg[30] = "";
				int preco = 0, pcompra = 0, duracao = 0;
				r.request_type = SELL;
				sscanf(cmd, "%s %s %s %d %d %d", cmd_request, &nome, &ctg, &preco, &pcompra, &duracao);
				r.sell.duracao = duracao;
				r.sell.compra = pcompra;
				r.sell.value = preco;
				strcpy(r.sell.nome, nome);
				strcpy(r.sell.categoria, ctg);
			}
			else
			{
				continue;
			}
		}

		n = write(fd, &r, sizeof(request));

		if (n == -1 || signal_exit)
		{
			if (errno == EPIPE)
				printf("serv is closed\n");
			else if (signal_exit == 1)
				printf("disconnected...\n");
			close(fd);
			close(fc);
			unlink(pipe);
			exit(1);
		}

		if (r.request_type != EXIT)
		{

			if (r.request_type == SELL || r.request_type == BUY || r.request_type == LIST)
			{

				n = read(fc, &resp, sizeof(response));

				if (item_len != resp.value)
				{
					item_len = resp.value;
					i = (item *)realloc(i, sizeof(item) * item_len);
				}

				for (int j = 0; j < item_len; j++)
				{
					read(fc, &i[j], sizeof(item));
				}
			}
			else
			{
				n = read(fc, &resp, sizeof(response));
			}

			if (n == -1 || signal_exit == 1)
			{
				printf("error reading response from server");
				close(fd);
				close(fc);
				exit(1);
			}

			printf("%s\n", resp.res == SUCCESS ? "SUCCESS" : "FAILURE");
			if (r.request_type == CASH)
			{
				printf("Saldo: %d\n", resp.value);
			}
			else if (r.request_type == LIST)
			{
				mostraItem(i, item_len);
			}
			else if (r.request_type == ENTRADA)
			{
				if (resp.valido == 1)
				{
					printf("Welcome!\nClient: %d\n", r.pid);
				}
				else if (resp.valido == 0)
				{
					printf("sorry\n");
					exit(1);
				}
			}else if(r.request_type == TIME){
				printf("Tempo: %d\n", resp.value);
			}
		}

		aux++;
	} while (r.request_type != EXIT);

	close(fd);
	close(fc);
	unlink(pipe);
	free(cmd);
}