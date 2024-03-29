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

typedef struct prom
{
	char nome[100];
	pid_t pid;
} prom;

typedef struct structs
{
	item *i;
	int itam;
	int utam;
	int ntam;
	user *u;
	prom *p;
	int ptam;
	pthread_mutex_t *mutex;
	notificacao * not ;
} structs;

// Global Var
char *FUSERS;
char *FPROMOTERS;
char *FITEMS;
static int signal_exit = 0;
static int signal_notif = 0;
static int signal_prom = 0;
static int count = 0;
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
	u[*tam].saldo = getUserBalance(nome);
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

// retorna o item atual
item *getItem(item *i, int tam, int id)
{
	for (int j = 0; j < tam; j++)
		if (i[j].id == id)
		{
			return i + j;
		}
}
notificacao *addNot(notificacao * not, int *tam, enum NotificacaoType n, int id_duracao, item *a, int itam, char *ctg, int preco)
{
	notificacao *u = (notificacao *)realloc(not, sizeof(notificacao) * ((*tam) + 1));
	item *i = NULL;
	i = getItem(a, itam, id_duracao);
	if (u == NULL)
	{
		printf("erro ao alocar memoria\n");
		return NULL;
	}
	if (n == COMPRA)
	{
		u[*tam].notType = COMPRA;
		strcpy(u[*tam].nomeI, i->nome);
		u[*tam].preco = i->valor_base;
		u[*tam].compraJa = i->compra_ja;
		strcpy(u[*tam].ctg, i->categoria);
		strcpy(u[*tam].nomeU, i->licitador);
		u[*tam].id = i->id;
	}
	else if (n == VENDA)
	{
		u[*tam].notType = VENDA;
		strcpy(u[*tam].nomeI, i->nome);
		u[*tam].preco = i->valor_base;
		u[*tam].compraJa = i->compra_ja;
		strcpy(u[*tam].ctg, i->categoria);
		strcpy(u[*tam].nomeU, i->licitador);
		u[*tam].id = i->id;
	}
	else if (n == PROM || n == PROM_END)
	{
		u[*tam].notType = n;
		strcpy(u[*tam].ctg, ctg);
		u[*tam].duracao = id_duracao;
		u[*tam].preco = preco;
	}

	++(*tam);
	return u;
}
// verifica se ja terminou o tempo se sim faz o q tem a fazer
int verificaLeilao(item *i, int *tam, user *u, int utam)
{
	int time = tempo;
	for (int j = 0; j < *tam; j++)
	{
		if ((i[j].tempoInicio + i[j].tempo) <= tempo)
		{
			for (int a = 0; a < utam; a++)
			{
				if (!strcmp(i[j].licitador, u[a].nome))
				{
					++(u[a].nItem);
					if (i[j].tempoProm >= i[j].buyTempo)
					{
						float desconto = (i[j].valor_base * (i[j].valorProm * 0.01));
						u[a].saldo -= (i[j].valor_base - desconto);
						updateUserBalance(u[a].nome, u[a].saldo);
					}
					else
					{
						u[a].saldo -= i[j].valor_base;
						updateUserBalance(u[a].nome, u[a].saldo);
					}
				}

				if (!strcmp(i[j].dono, u[a].nome) && strcmp(i[j].licitador, "-") != 0)
				{
					u[a].saldo += i[j].valor_base;
					updateUserBalance(u[a].nome, u[a].saldo);
				}
			}
			return i[j].id;
		}
	}
	return 0;
}

// configura items para promocao
item *apanhaProm(item *i, int tam, char *ctg, int valor, int duracao)
{
	for (int j = 0; j < tam; j++)
	{
		if (!strcmp(i[j].categoria, ctg))
		{
			i[j].tempoProm = duracao + tempo;
			i[j].valorProm = valor;
		}
	}
	return i;
}

// le a msg e retorna
char *recebePromotor(int fd_p2b[2])
{
	char msg[100];
	read(fd_p2b[0], msg, 100);
	return strtok(msg, "\n");
}

// executa o promoto
int executaPromotor(int fd_p2b[2], char *nome)
{
	int f = fork();
	char output[100];

	if (f == -1)
	{
		printf("ERRO: %s\n", getLastErrorText());
	}
	else if (f == 0)
	{
		close(1);
		dup(fd_p2b[1]);
		close(fd_p2b[0]);
		close(fd_p2b[1]);
		int aux = execl(nome, nome);
		printf("%d", aux);
		if (aux == -1)
		{
			return aux;
		}
	}
	return f;
}

// le o ficheiro dos promotores e guarda
prom *leProms(char *nomeFich, prom *p, int *tam)
{
	FILE *f;
	char Linha[100];
	char nome[100];

	f = fopen(nomeFich, "rt");

	if (f == NULL)
	{
		printf("ERRO");
		fclose(f);
		return 0;
	}
	int i = 0;
	while (!feof(f))
	{
		fgets(Linha, 100, f);
		sscanf(Linha, "%s", nome);
		if (access(nome, F_OK) == 0)
		{
			p = (prom *)realloc(p, sizeof(prom) * (i + 1));
			strcpy(p[i].nome, nome);
			i++;
		}
	}
	(*tam) = (i);
	fclose(f);
	return p;
}

// elimina promotor
prom *eliminaProm(prom *p, int *tam, char *nome)
{
	for (int j = 0; j < *tam; j++)
	{
		if (strcmp(p[j].nome, nome) == 0)
		{
			if (p[j].pid != -1)
			{
				kill(p[j].pid, SIGUSR1);
			}
			for (int i = j; i < ((*tam) - 1); i++)
				p[i] = p[i + 1];
			break;
		}
	}
	--(*tam);
	if (*tam == 0)
	{
		free(p);
		return NULL;
	}

	prom *a = (prom *)realloc(p, sizeof(prom) * (*tam));

	if (a == NULL)
	{
		printf("error allocating memory\n");
		exit(1);
	}
	return a;
}

// atualiza ficheiro Promotores
void atualizaFproms(prom *p, int tam, char *nome)
{
	FILE *f;

	f = fopen(nome, "w");

	if (f == NULL)
		return;

	for (int j = 0; j < tam; j++)
	{
		if (j == tam - 1)
		{
			fprintf(f, "%s", p[j].nome);
		}
		else
		{
			fprintf(f, "%s\n", p[j].nome);
		}
	}
	fclose(f);
}

notificacao *eliminaNot(notificacao *a, int *tam, int id)
{
	for (int i = 0; i < *tam; i++)
	{
		for (int j = i; j < ((*tam)); j++)
		{
			a[j] = a[j + 1];
		}
		--(*tam);
	}

	notificacao *c = (notificacao *)realloc(a, sizeof(notificacao) * ((*tam)));
	return c;
}

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
	else
	{
		printf("error Serv ja existe!\n");
		exit(1);
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
	notificacao *nt = NULL;
	item *it = NULL;

	char pipe[PIPE_SIZE];
	pthread_mutex_lock(st->mutex);
	while ((n = read(fd, &r, sizeof(request))))
	{
		if (!n || signal_exit)
		{
			free(st->not );
			saveUsersFile(FUSERS);
			if (st->itam == 0)
			{
				remove(FITEMS);
			}
			else
			{
				atualizaFitems(st->i, st->itam, FITEMS, tempo);
			}

			close(fd);
			closeFrontends(st->u, st->utam);
			free(st->i);
			free(st->u);
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
			u = getClient(st->u, st->utam, r.pid);
			resp.value = getUserBalance(u->nome);
			if (resp.value != -1)
				resp.res = SUCCESS;
			else
				resp.res = FAILURE;
			break;
		case ADD:
			u = getClient(st->u, st->utam, r.pid);
			if (r.add.value <= 0)
			{
				resp.res = FAILURE;
				break;
			}
			valido = updateUserBalance(u->nome, r.add.value);
			if (valido == -1)
				resp.res = SUCCESS;
			else
				resp.res = FAILURE;
			break;
		case BUY:
			u = getClient(st->u, st->utam, r.pid);
			it = getItem(st->i, st->itam, r.buy.id);
			printf("saldo:%d", getUserBalance(u->nome));
			valido = compraItem(st->i, r.buy.id, r.buy.value, u->nome, getUserBalance(u->nome), &(st->itam));
			if (valido == 1) // licita
			{
				resp.res = SUCCESS;
				strcpy(it->licitador, u->nome);
				it->valor_base = r.buy.value;
				it->buyTempo = tempo;
				resp.value = st->itam;
				break;
			}
			else if (valido == 2) // compra ja
			{
				resp.res = SUCCESS;
				u->nItem++;
				u->saldo = getUserBalance(u->nome);
				u->saldo -= it->compra_ja;
				for (int j = 0; j < st->utam; j++)
				{
					if (!strcmp(it->dono, u[j].nome))
					{
						u[j].saldo += it->compra_ja;
						updateUserBalance(u[j].nome, u[j].saldo);
						
					}
				}
				updateUserBalance(u->nome, u->saldo);		
				strcpy(it->licitador, u->nome);
				// cria notificacao
				st->not = addNot(st->not, &(st->ntam), COMPRA, it->id, st->i, st->itam, "", 0);
				st->i = eliminaItem(it->id, st->i, &(st->itam));
				resp.value = st->itam;
				resp.valido = st->ntam;
				signal_notif = 1;
				count = 0;
				break;
			}
			resp.value = st->itam;
			resp.res = FAILURE;

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
			it = getItem(st->i, st->itam, id);
			//  cria notificacao
			st->not = addNot(st->not, &(st->ntam), VENDA, it->id, st->i, st->itam, "", 0);
			resp.valido = st->ntam;
			signal_notif = 1;
			count = 0;
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
			u = getClient(st->u, st->utam, r.pid);
			printf("\nCliente %s saiu!\n", u->nome);
			st->u = eliminaUser(st->u, &(st->utam), r.pid);
			break;
		case NOTIF:
			resp.res = SUCCESS;
			resp.value = st->itam;
			resp.valido = st->ntam;
			break;
		default:
			printf("tipo de pedido invalido\n");
			resp.res = FAILURE;
			break;
		}

		pthread_mutex_unlock(st->mutex);

		if (r.request_type == SELL || r.request_type == BUY || r.request_type == LIST || r.request_type == NOTIF)
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

			if (signal_notif && signal_prom == 0)
			{
				union sigval val;
				for (int j = 0; j < st->utam; j++)
				{
					if (st->u[j].pid != r.pid)
						sigqueue(st->u[j].pid, SIGUSR1, val);
				}
				signal_notif = 0;
			}

			if (r.request_type == NOTIF)
			{
				for (int i = 0; i < st->ntam; i++)
				{
					write(fc, &st->not [i], sizeof(notificacao));
				}
				count++;
			}

			resp.valido = st->ntam;

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
void enviaSinal(user *u, int tam)
{
	signal_prom = 1;
	union sigval val;
	for (int i = 0; i < tam; i++)
	{
		sigqueue(u[i].pid, SIGUSR1, val);
	}
	count = 0;
}
void *handler_time(void *data)
{
	structs *st = (structs *)data;
	while (signal_exit != 1)
	{
		++tempo;
		sleep(1);
		int aux = verificaLeilao(st->i, &(st->itam), st->u, st->utam);

		for (int j = 0; j < st->itam; j++)
		{
			if (tempo == st->i[j].tempoProm && st->i[j].tempoProm != 0)
			{
				st->not = addNot(st->not, &(st->ntam), PROM_END, (tempo - st->i[j].tempoProm), st->i, st->itam, st->i[j].categoria, st->i[j].valorProm);
				enviaSinal(st->u, st->utam);
			}
		}

		if (aux != 0)
		{
			item *i = getItem(st->i, st->itam, aux);
			st->not = addNot(st->not, &(st->ntam), COMPRA, i->id, st->i, st->itam, "", 0);
			st->i = eliminaItem(aux, st->i, &(st->itam));
			enviaSinal(st->u, st->utam);
		}

		if (count >= st->utam && signal_prom == 1)
		{
			free(st->not );
			st->not = NULL;
			st->ntam = 0;
			count = 0;
			signal_prom = 0;
		}
		else if (count == (st->utam - 1) && signal_prom != 1)
		{
			free(st->not );
			st->not = NULL;
			st->ntam = 0;
			count = 0;
			signal_prom = 0;
		}
	}
}

void *handler_proms(void *data)
{
	structs *st = (structs *)data;

	int fd[2], valor = 0, duracao = 0, x = -1;
	int Ppipe = pipe(fd);
	char output[100], ctg[100];
	while (signal_exit != 1)
	{
		for (int j = 0; j < st->ptam; j++)
		{
			st->p[j].pid = executaPromotor(fd, st->p[j].nome);
			strcpy(output, recebePromotor(fd));
			printf("\n%s\n", output);
			sscanf(output, "%s %d %d", &ctg, &valor, &duracao);
			st->not = addNot(st->not, &(st->ntam), PROM, duracao, st->i, st->itam, ctg, valor);
			st->i = apanhaProm(st->i, st->itam, ctg, valor, duracao);
			enviaSinal(st->u, st->utam);
			sleep(50);
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
	
	FUSERS = getenv("FUSERS");
	FPROMOTERS = getenv("FPROMOTERS");
	FITEMS = getenv("FITEMS");

	pthread_mutex_t mutex;
	int res = pthread_mutex_init(&mutex, NULL);
	if (res != 0)
	{
		printf("error ao inicializar mutex\n");
		exit(1);
	}

	structs st = {NULL, 0, 0, 0, NULL, NULL, 0, &mutex, NULL};

	if (access(FITEMS, F_OK) == 0)
	{
		st.i = leFicheiroItem(FITEMS, st.i, &(st.itam));
	}
	st.p = leProms(FPROMOTERS, st.p, &(st.ptam));
	loadUsersFile(FUSERS);

	pthread_t pipe_thread;
	pthread_t tempo_thread;
	pthread_t proms_thread;

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
	res = pthread_create(&proms_thread, NULL, handler_proms, (void *)&st);
	if (res != 0)
	{
		printf("error a criar thread do tempo");
		exit(1);
	}

	printf("Serv is running...\n");
	int aux = 0;
	char teste[10] = "";
	while (1)
	{
		printf("\n>>");
		n_chars = getline(&cmd, &cmd_size, stdin);
		cmd[n_chars - 1] = '\0';
		sscanf(cmd, "%s", cmd_request);

		if (!strcmp(cmd_request, "close") || signal_exit) // close
		{
			printf("Closing..\n");
			break;
		}
		else if (!strcmp(cmd_request, "users")) // users
		{
			sscanf(cmd, "%s %s", cmd_request, &teste);
			if (strcmp(teste, "") != 0)
			{
				strcpy(teste, "");
				printf("FAILURE\n");
				continue;
			}
			if (st.utam == 0)
				printf("Nao existem users para mostrar\n");
			for (int i = 0; i < st.utam; ++i)
			{
				printf("PID: %d\tSaldo: %d\tNome: %s\tN_items: %d\n", st.u[i].pid, getUserBalance(st.u[i].nome), st.u[i].nome, st.u[i].nItem);
			}
		}
		else if (!strcmp(cmd_request, "list")) // list
		{
			sscanf(cmd, "%s %s", cmd_request, &teste);
			if (strcmp(teste, "") != 0)
			{
				strcpy(teste, "");
				printf("FAILURE\n");
				continue;
			}
			if (st.itam == 0)
			{
				printf("Nao existem items a leilao\n");
			}
			else
			{
				mostraItem(st.i, st.itam);
			}
		}
		else if (!strcmp(cmd_request, "kick")) // kick
		{
			sscanf(cmd, "%s %s %s", cmd_request, &arg, &teste);

			if (strcmp(teste, "") != 0)
			{
				strcpy(teste, "");
				printf("FAILURE\n");
				continue;
			}

			int n = kick_cmd(st.u, st.utam, arg);
			if (n)
				printf("SUCCESS\n");
			else
				printf("FAILURE\n");
		}
		else if (!strcmp(cmd_request, "prom")) // prom
		{
			sscanf(cmd, "%s %s", cmd_request, &teste);
			if (strcmp(teste, "") != 0)
			{
				strcpy(teste, "");
				printf("FAILURE\n");
				continue;
			}
			if (st.ptam == 0)
				printf("Nao existem promotores disponiveis\n");
			else if (aux == 0)
			{
				for (int i = 0; i < st.ptam; i++)
				{
					if (st.p[i].pid != 0)
						printf("Nome: %s\n", st.p[i].nome);
				}
			}
		}
		else if (!strcmp(cmd_request, "cancel")) // cancel
		{
			sscanf(cmd, "%s %s %s", cmd_request, &arg, &teste);
			if (strcmp(teste, "") != 0 || strcmp(arg, "") == 0)
			{
				strcpy(teste, "");
				printf("FAILURE\n");
				continue;
			}
			st.p = eliminaProm(st.p, &(st.ptam), arg);
			printf("SUCCESS\n");
		}
		else if (!strcmp(cmd_request, "reprom")) // reprom
		{
			sscanf(cmd, "%s %s", cmd_request, &teste);
			if (strcmp(teste, "") != 0)
			{
				strcpy(teste, "");
				printf("FAILURE\n");
				continue;
			}
			atualizaFproms(st.p, st.ptam, FPROMOTERS);
			st.p = leProms(FPROMOTERS, st.p, &(st.ptam));
			printf("SUCCESS\n");
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

	res = pthread_kill(proms_thread, SIGUSR1);
	if (res != 0)
	{
		printf("error ao enviar sinal para a thread");
		exit(1);
	}

	res = pthread_join(proms_thread, NULL);
	if (res != 0)
	{
		printf("error ao esperar pela thread\n");
		exit(1);
	}

	printf("Bye\n");
	exit(0);
}