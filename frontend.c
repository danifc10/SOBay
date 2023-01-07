#include "util.h"
#include "item.h"

#define CMD_SIZE 100

// Global Var
static int signal_exit = 0;
static int signal_notif = 0;

void signal_handler(int sig)
{

	signal_exit = 1;
}

void signal_notific(int sig)
{
	signal_notif = 1;
	printf("\nTem uma notificacao\nCarregue ENTER para visualizar\n");
}

notificacao *eliminaNot(notificacao *a, int *tam, int id)
{
	for (int i = 0; i < *tam; i++)
	{
		if (i == id)
		{
			for (int j = i; j < ((*tam) - 1); j++)
			{
				a[j] = a[j + 1];
			}
			break;
		}
	}
	--(*tam);
	/*if (*tam == 0)
	{
		free(a);
		return NULL;
	}
	notificacao *c = (notificacao *)realloc(a, sizeof(notificacao) * ((*tam)));
	if (c == NULL)
	{
		printf("error allocating memory\n");
		exit(1);
	}*/
	return a;
}

void main(int argc, char *argv[])
{

	char *cmd = NULL;
	size_t cmd_size;
	int amount, destination, n_chars = 0;
	char cmd_request[CMD_SIZE];
	char pipe[PIPE_SIZE];

	struct sigaction sa;
	sa.sa_flags = SA_SIGINFO;
	sa.sa_handler = signal_handler;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGPIPE, &sa, NULL);
	// sigaction(SIGUSR1, &sac, NULL);

	signal(SIGUSR1, signal_notific);

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
	char argumento[100] = "", teste[100] = "";
	response resp;
	notificacao *nt = NULL;
	int ntam = 0;
	int aux = 0;
	int atualiza = 0;
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
			else if (signal_notif == 1)
			{
				r.request_type = NOTIF;
			}
			else if (!strcmp(cmd_request, "list"))
			{
				sscanf(cmd, "%s %s", cmd_request, &teste);
				if (strcmp(teste, "") != 0)
					r.request_type = FAIL;
				else
					r.request_type = LIST;
			}
			else if (!strcmp(cmd_request, "cash"))
			{
				sscanf(cmd, "%s %s", cmd_request, &teste);
				if (strcmp(teste, "") != 0)
					r.request_type = FAIL;
				else
					r.request_type = CASH;
			}
			else if (!strcmp(cmd_request, "time"))
			{
				r.request_type = TIME;
				sscanf(cmd, "%s %s", cmd_request, &teste);
				if (strcmp(teste, "") != 0)
					r.request_type = FAIL;
				else
					r.request_type = TIME;
			}
			else if (!strcmp(cmd_request, "licat"))
			{
				r.request_type = LICAT;
				sscanf(cmd, "%s %s", cmd_request, &argumento, &teste);
				if (strcmp(teste, "") != 0 || strcmp(argumento, "") == 0)
					r.request_type = FAIL;
				else
					licat(argumento, i, item_len);
			}
			else if (!strcmp(cmd_request, "lisel"))
			{
				r.request_type = LISEL;
				sscanf(cmd, "%s %s", cmd_request, &argumento, &teste);
				if (strcmp(teste, "") != 0 || strcmp(argumento, "") == 0)
					r.request_type = FAIL;
				else
					lisel(argumento, i, item_len);
			}
			else if (!strcmp(cmd_request, "lival"))
			{
				r.request_type = LIVAL;
				sscanf(cmd, "%s %d", cmd_request, &value, &teste);
				if (strcmp(teste, "") != 0 || value == -1)
					r.request_type = FAIL;
				else
					lival(value, i, item_len);
			}
			else if (!strcmp(cmd_request, "litime"))
			{
				r.request_type = LITIME;
				value = -1;
				sscanf(cmd, "%s %d %s", cmd_request, &value, &teste);
				if (strcmp(teste, "") != 0 || value == -1)
					r.request_type = FAIL;
				else
					litime(value, i, item_len);
			}
			else if (!strcmp(cmd_request, "add"))
			{
				r.request_type = ADD;
				value = -1;
				sscanf(cmd, "%s %d %s", cmd_request, &value, &teste);
				if (strcmp(teste, "") != 0)
					r.request_type = FAIL;
				else
				{
					r.add.value = value;
				}
			}
			else if (!strcmp(cmd_request, "buy"))
			{
				value = -1;
				value2 = -1;
				sscanf(cmd, "%s %d %d %s", cmd_request, &value, &value2, &teste);
				if (strcmp(teste, "") != 0 || value == -1 || value2 == -1)
					r.request_type = FAIL;
				else
				{
					r.buy.id = value;
					r.buy.value = value2;
					r.request_type = BUY;
				}
			}
			else if (!strcmp(cmd_request, "sell"))
			{
				char nome[30] = "", ctg[30] = "";
				int preco = -1, pcompra = -1, duracao = -1;
				sscanf(cmd, "%s %s %s %d %d %d %s", cmd_request, &nome, &ctg, &preco, &pcompra, &duracao, &teste);
				if (strcmp(teste, "") != 0 || strcmp(nome, "") == 0 || strcmp(ctg, "") == 0 || preco == -1 || pcompra == -1 || duracao == -1)
					r.request_type = FAIL;
				else
				{
					r.sell.duracao = duracao;
					r.sell.compra = pcompra;
					r.sell.value = preco;
					strcpy(r.sell.nome, nome);
					strcpy(r.sell.categoria, ctg);
					r.request_type = SELL;
				}
			}
			else
			{
				printf("FAILURE\n");
				continue;
			}
		}

		if (r.request_type != FAIL)
		{
			n = write(fd, &r, sizeof(request));

		}

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

		if (r.request_type != EXIT && r.request_type != FAIL)
		{

			if (r.request_type == SELL || r.request_type == BUY || r.request_type == LIST || r.request_type == NOTIF)
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

				if (r.request_type == NOTIF)
				{
					if (ntam != resp.valido)
					{
						ntam = resp.valido;
						nt = (notificacao *)realloc(nt, sizeof(notificacao) * ntam);
					}
					for (int i = 0; i < resp.valido; i++)
					{
						n = read(fc, &nt[i], sizeof(notificacao));
					}

					signal_notif = 0;
				}
				ntam = resp.valido;
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
			}
			else if (r.request_type == TIME)
			{
				printf("Tempo: %d\n", resp.value);
			}
			else if (r.request_type == NOTIF)
			{
				for (int i = 0; i < resp.valido; i++)
				{
					if (nt[i].notType == COMPRA)
					{
						if ( strcmp(nt[i].nomeU, "-")== 0)
						{
							strcpy(nt[i].nomeU, "Por Vender");
							printf("\nDuracao do item terminou!\n");
						}
						else
						{
							printf("\nUm item foi comprado!\n");
						}
						printf("Id: %d Nome: %s Categoria: %s Preco: %d User: %s\n", nt[i].id, nt[i].nomeI, nt[i].ctg, nt[i].preco, nt[i].nomeU);
					}
					else if (nt[i].notType == VENDA)
					{
						printf("\nNovo item a venda!\n");
						printf("Id: %d Nome: %s Categoria: %s Preco: %d Compra Ja: %d\n", nt[i].id, nt[i].nomeI, nt[i].ctg, nt[i].preco, nt[i].compraJa);
					}
					else if (nt[i].notType == PROM)
					{
						printf("\nNova Promocao ativa!\n");
						printf("Categoria: %s Preco: %d Tempo: %d\n", nt[i].ctg, nt[i].preco, nt[i].duracao);
					}
				}
			}
		}
		else if (r.request_type == FAIL)
		{
			strcpy(teste, "");
			printf("FAILURE\n");
		}

		aux++;
		signal_notif = 0;
	} while (r.request_type != EXIT);

	close(fd);
	close(fc);
	unlink(pipe);
	free(cmd);
}