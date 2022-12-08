#include <stdio.h>
#include "item.h"
#include <string.h>
#include <stdlib.h>

#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include "user.h"
#include "users_lib.h"
#include <errno.h>
#define FPROMOTORES "ficheiro_promotores.txt"
#define BACKEND_FIFO "BACKEND"
#define CLIENT_FIFO "CLIENTE%d"
#define TAM 1000
char CLIENT_FIFO_FINAL[100];

user *utilizadores;
int utilizadores_len;

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

void mostraUsers(char *vUser[TAM], int pids[TAM], int count)
{
	for (int i = 0; i < count; i++)
	{
		if (pids[i] != 0 && (strcmp(vUser[i], "") != 0))
		{
			printf("\nnome: %s pid: %d\n",vUser[i], pids[i]);
		}
	}
}

int eliminaUser(char *vUsers[TAM], int pids[TAM], char *nome, int contaUsers)
{

	for (int i = 0; i < contaUsers; i++)
	{
		if (strcmp(vUsers[i], nome) == 0)
		{
			free(vUsers[i]);
			contaUsers--;
			kill(pids[i], SIGUSR1);
			pids[i] = 0;
		}
	}
	return contaUsers;
}
void fechaFrontends(char *vUsers[TAM], int pids[TAM], int contaUsers)
{
	for (int i = 0; i < contaUsers; i++)
	{
		if (strcmp(vUsers[i], "") != 0 && pids[i] != 0)
		{
			kill(pids[i], SIGUSR1);
		}
	}
}


int executaPromotor(int fd_p2b[2])
{
	int f = fork();
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

		execl("./promotor_oficial", "promotor_oficial", NULL);
	}
	return f;
}

char *recebePromotor(int fd_p2b[2])
{
	char msg[100];
	read(fd_p2b[0], msg, 100);
	return strtok(msg, "\n");
}

int leComandosAdmin(char *comando, char *vUser[TAM], int pids[TAM], int contaUsers, item *i)
{
	char aux[100];
	char argumento[20];
	int count = 0;

	for (int i = 0; i < strlen(comando) - 1; i++)
	{
		if (isspace(comando[i]))
		{
			count++;
		}
	}

	strcpy(aux, comando);

	switch (count)
	{
	case 0:
		sscanf(aux, "%s", comando);

		if (strcmp(comando, "list") == 0)
		{
			mostraItem(i);
			return 1;
		}
		else if (strcmp(comando, "users") == 0)
		{
			mostraUsers(vUser, pids, contaUsers);
			return 1;
		}
		else if (strcmp(comando, "prom") == 0)
		{
			return 1;
		}
		else if (strcmp(comando, "reprom") == 0)
		{
			return 1;
		}
		else if (strcmp(comando, "close") == 0)
		{
			fechaFrontends(vUser, pids, contaUsers);
			union sigval xpto;
			sigqueue(getpid(), SIGINT, xpto);
			return 1;
		}
		else
			return 0;
		break;

	case 1:

		sscanf(aux, "%s %s", comando, argumento);
		if (strcmp(comando, "kick") == 0)
		{
			contaUsers = eliminaUser(vUser, pids, argumento, contaUsers);
			return 1;
		}
		else if (strcmp(comando, "cancel") == 0)
		{

			return 1;
		}
		else
			return 0;
		break;

	default:

		return 0;
		break;
	}
}

typedef struct
{
	pid_t pid;
	char nome[100];
	char pass[100];
	char com[100];
} dataMsg;

typedef struct
{
	pid_t pidB;
	int res;
	item *i;
} dataRPL;

void sair(int signal, siginfo_t *info, void *extra)
{
	unlink(BACKEND_FIFO);
	exit(1);
}

void addUser(char *vUsers[TAM], int pids[TAM], int contaUsers, char *nome, int pid)
{

	vUsers[contaUsers - 1] = malloc(30);
	strcpy(vUsers[contaUsers - 1], nome);
	pids[contaUsers - 1] = pid;
}

int main()
{

	dataMsg mensagemRecebida;
	dataRPL resposta;
	int fdRecebe, fdEnvio;

	struct sigaction sac;
	sac.sa_sigaction = sair;
	sigaction(SIGINT, &sac, NULL);

	if (mkfifo(BACKEND_FIFO, 0666) == -1)
	{

		if (errno == EEXIST)
		{
			printf("\nfifo ja existe\n");
		}
		printf("erro ao abrir fifo\n");
		return 1;
	}

	printf("\n...\n");
	fdRecebe = open(BACKEND_FIFO, O_RDONLY);
	if (fdRecebe == -1)
	{
		printf(">> Erro ao abrir o backend");
		return 1;
	}

	resposta.pidB = getpid();
	resposta.i = malloc(sizeof(item));
	leFicheiroItem(FITEM, resposta.i);

	int nfd;
	fd_set read_fds;
	struct timeval tv;
	int maior = fdRecebe + 1; //  maior FD acrescido de 1
	char buffer[100];

	char **vUSers = malloc(TAM);
	int *pids = malloc(TAM * sizeof(int));
	int contaUsers = 0;

	do
	{
		FD_ZERO(&read_fds);
		FD_SET(0, &read_fds);		 // incluir o stdin
		FD_SET(fdRecebe, &read_fds); // incluir o fdRecebe (server_fifo)
		tv.tv_sec = 5;
		tv.tv_usec = 0;
		// escutar stdin e fdRecebe
		nfd = select(maior, &read_fds, NULL, NULL, &tv);
		// escutar teclado
		if (FD_ISSET(0, &read_fds))
		{
			fgets(buffer, sizeof(buffer), stdin);
			int aux = leComandosAdmin(buffer, vUSers, pids, contaUsers, resposta.i);

			if (aux == 0)
			{
				printf("Comando Invalido!\n");
			}
			else if (aux == 1)
			{
				printf("Comando Valido!\n");
			}
		}
		if (FD_ISSET(fdRecebe, &read_fds))
		{
			int size = read(fdRecebe, &mensagemRecebida, sizeof(mensagemRecebida));
			if (size > 0)
			{
				if (strcmp(mensagemRecebida.com, "exit") == 0)
				{
					// so entra aqui quando user saiu com "exit"
					printf("\n>> User %s saiu!\n", mensagemRecebida.nome);
					contaUsers = eliminaUser(vUSers, pids, mensagemRecebida.nome, contaUsers);
				}
				else
				{
					utilizadores_len = loadUsersFile(USER_FILENAME);
					int aux = isUserValid(mensagemRecebida.nome, mensagemRecebida.pass);
					if (aux == 1)
					{
						printf("\n>> Utilizador: %s logado com pid %d\n", mensagemRecebida.nome, mensagemRecebida.pid);
						resposta.res = 1;
						++contaUsers;
						addUser(vUSers, pids, contaUsers, mensagemRecebida.nome, mensagemRecebida.pid);
					}
					else
					{
						printf("\n>> Utilizador nao conhecido!\n");
						resposta.res = 0;
					}
					sprintf(CLIENT_FIFO_FINAL, CLIENT_FIFO, mensagemRecebida.pid);
					fdEnvio = open(CLIENT_FIFO_FINAL, O_WRONLY);
					int size2 = write(fdEnvio, &resposta, sizeof(resposta));
					close(fdEnvio);
				}
			}
		}

	} while (1);

	//--------------------------------------------
	//--------------------------------------------
	/*
		char outputPromotores[100];
		// maximo de promotores
		int pid_promotor[10];

		for (int i = 0; i < 10; i++)
		{
			pid_promotor[i] = 0;
		}

		// criar 2 unnamed pipes
		int fd_p2b[2];
		// pipes
		int Rpipe = pipe(fd_p2b);

		if (Rpipe == -1)
		{
			printf("erro ao criar pipe\n");
			exit(1);
		}
		int pid;
		int opcao;
		union sigval valores;
		char comando[20];
		int aux = 0;
		int estado;

		do
		{

			printf("\n-------------Deseja testar que funcionalidade?-----------\n");
			printf("\n1. Inserir Comando\n2. Executar promotor\n3. Utilizadores\n4. Ver items\n5. Sair\n>>");
			scanf("%d", &opcao);
			getchar();

			switch (opcao)
			{
			case 1:
				do
				{
					printf("\n\n>>Deseja testar que comando?\n");
					fgets(comando, 200, stdin);
					aux = leComandosAdmin(comando);

				} while (aux != 0);
				break;
			case 2:
				pid = executaPromotor(fd_p2b);
				for (int i = 0; i < 10; i++)
				{
					if (pid_promotor[i] == 0)
					{
						pid_promotor[i] = pid;
						break;
					}
				}

				valores.sival_int = 1;

				// tem que aparecer 3 promo antes de  terminar o processo;
				for (int i = 0; i <= 2; i++)
				{
					strcpy(outputPromotores, recebePromotor(fd_p2b));
					printf("\nmsg:%s\n", outputPromotores);
					if (i == 2)
					{
						sigqueue(pid, SIGUSR1, valores); // fechar promotor
					}
				}
				//wait(&estado);
				//printf("%d\n",estado);
				break;

			case 3:
				utilizadores_len = loadUsersFile(USER_FILENAME);
				int opcaoUser = 0;
				do
				{

					char nome[100];
					char password[100];
					int saldo;
					printf("\n-----Utilizadores-----\n");
					printf("--1.Atualizar saldo\n--2.Verificar user\n--3.Obter saldo\n--4. Voltar\n");
					printf("\n\n>>");
					scanf("%d", &opcaoUser);
					getchar();

					if (opcaoUser == 1)
					{

						printf("\nInsira um username:");
						scanf("%s", &nome);
						printf("\nNovo saldo:");
						scanf("%d", &saldo);

						if (updateUserBalance(nome, saldo) == -1)
						{
							getLastErrorText();
						}
						else
						{
							printf("\nSaldo atualizado com sucesso!\n");
						}
					}
					else if (opcaoUser == 2)
					{
						printf("\nUsername:");
						scanf("%s", &nome);
						printf("\nPassword:");
						scanf("%s", &password);

						if (isUserValid(nome, password) == -1)
						{
							getLastErrorText();
						}
						else if (isUserValid(nome, password) == 1)
						{
							printf("\nUser valido!\n");
						}
						else if (isUserValid(nome, password) == 0)
						{
							printf("\nPassword errada ou user nao existe!\n");
						}
					}
					else if (opcaoUser == 3)
					{
						printf("\nUsername:");
						scanf("%s", &nome);
						saldo = getUserBalance(nome);
						if (saldo == -1)
						{
							getLastErrorText();
						}
						else
						{
							printf("\nSaldo: %d\n", saldo);
						}
					}
					saveUsersFile(USER_FILENAME);
				} while (opcaoUser != 4);
				free(utilizadores);
				break;
			case 4:
				mostraItem(resposta.i);
				break;
			case 5:
				exit(1);
				break;

			default:
				break;
			}

		} while (opcao != 5);
	*/
	return 0;
}
