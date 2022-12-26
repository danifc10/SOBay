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
#include <pthread.h>
#include <sys/select.h>
#include "utils.h"

#define MAXPROM 10


char *recebePromotor(int fd_p2b[2])
{
	printf("entrei recebe\n");
	char msg[100];
	read(fd_p2b[0], msg, 100);
	return strtok(msg, "\n");
}

int executaPromotor(int fd_p2b[2], char *PromsName)
{
	int f = fork();
	char output[100];
	printf("entrei executa");
	if (f == -1)
	{
		printf("ERRO: %s\n", getLastErrorText());
	}
	else if (f == 0)
	{
		// close(1);
		dup(fd_p2b[1]);
		close(fd_p2b[0]);
		close(fd_p2b[1]);

		// printf("nome :: %s", PromsName);
		execlp(PromsName, PromsName, NULL);
		strcpy(output, recebePromotor(fd_p2b));
		printf("msg: %s", output);
	}
	return f;
}

int leProms(char *nomeFich, char *PromsName[TAM], int fd[2])
{
	FILE *f;
	char Linha[100];

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
		PromsName[i] = malloc(30);
		fgets(Linha, 100, f);
		sscanf(Linha, "%s", PromsName[i]);
		executaPromotor(fd, PromsName[i]);
		sleep(5);
		i++;
	}

	fclose(f);
	return i;
}

void mostraProms(char *PromsName[TAM], int count)
{
	for (int i = 0; i < count; i++)
	{
		printf("\nnome: %s \n", PromsName[i]);
	}
}

void userscmd(user *a, int tam)
{
	for (int i = 0; i < tam; i++)
	{
		printf("User %d: %s\n", i + 1, a[i].nome);
	}
}
int eliminaUser(user *a, char *nome, int tam)
{
	for (int i = 0; i < tam; i++)
	{
		if (strcmp(a[i].nome, nome) == 0)
		{
			kill(a[i].pid, SIGUSR1);
			for (int j = i; j < tam - 1; j++)
			{
				a[j] = a[j + 1];
			}
			return tam - 1;
		}
	}
	return tam;
}

void fechaFrontends(user *a, int tam)
{
	for (int i = 0; i < tam; i++)
	{
		if (a[i].pid != 0 && strcmp(a[i].nome, "") != 0)
		{
			kill(a[i].pid, SIGUSR1);
		}
	}
}

// 0 se nao existe 1 se existe
int existe(user *a, int tamanho, char *nome)
{
	for (int i = 0; i < tamanho; i++)
	{
		if (strcmp(a[i].nome, nome) == 0)
		{
			return 1;
		}
	}
	return 0;
}

user *addUser(user *a, int tam, char *nome, int pid)
{
	strcpy(a[tam - 1].nome, nome);
	a[tam - 1].saldo = 0;
	a[tam - 1].pid = pid;
	a[tam - 1].TempoVida = atoi(getenv("HEARTBEAT"));
	return a;
}

int leComandosAdmin(char *comando, user *a, int contaUsers, item *i, int item_len)
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
			printf("--------------------ITEMS---------------------\n");
			mostraItem(i, item_len);
			printf("Comando Valido!\n");
		}
		else if (strcmp(comando, "users") == 0)
		{
			printf("--------------------USERS---------------------\n");
			userscmd(a, contaUsers);

			printf("Comando Valido!\n");
		}
		else if (strcmp(comando, "prom") == 0)
		{
			printf("Comando Valido!\n");
		}
		else if (strcmp(comando, "reprom") == 0)
		{
			printf("Comando Valido!\n");
		}
		else if (strcmp(comando, "close") == 0)
		{
			fechaFrontends(a, contaUsers);
			saveUsersFile(USER_FILENAME);
			union sigval xpto;
			sigqueue(getpid(), SIGINT, xpto);
			printf("Comando Valido!\n");
		}
		return contaUsers;

	case 1:

		sscanf(aux, "%s %s", comando, argumento);
		if (strcmp(comando, "kick") == 0)
		{
			contaUsers = eliminaUser(a, argumento, contaUsers);
			printf(">> User %s expulso!\n", argumento);
		}
		else if (strcmp(comando, "cancel") == 0)
		{
			printf("Comando Valido!\n");
		}
		return contaUsers;

	default:
		printf("Comando Invalido!\n");
		return contaUsers;
	}
}
/*
void AwayFromKeyboard(user *a,char nome[],int tam){
	for(int i = 0;i<tam;i++){
		a[i].TempoVida=a[i].TempoVida-1;
	}
	for(int i = 0;i<tam;i++){

			if(a[i].TempoVida<=0){
				eliminaUser(a,nome,tam);
			}
			break;

	}
}*/

void sair(int s, siginfo_t *i, void *v)
{
	fechaFrontends(a, user_len);
	unlink(BACKEND_FIFO);
	exit(1);
}

int main()
{
	setenv("HEARTBEAT", "20", 0);
	dataMsg mensagemRecebida;
	dataRPL resposta;
	int fdRecebe, fdEnvio;
	char *Proms[MAXPROM];
	char output[100];

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

	resposta.pidB = getpid();
	int item_len = contaItems(FITEM);
	item *i;
	i = (item *)malloc(item_len * sizeof(item));
	i = leFicheiroItem(FITEM, i);

	/*----------------------------------- LEITURA DE FICHEIROS DE TEXTO ------------------------------------*/

	user_len = loadUsersFile(USER_FILENAME);
	a = (user *)malloc(user_len * sizeof(user));
	resposta.itam = item_len;
	resposta.uTam =  user_len;

	// strcpy(output, recebePromotor(fd_p2b));
	// printf("\nmsg:%s\n", output);
	/*------------------------------------------------------------------------------------------------------*/
	/*
		// criar 2 unnamed pipes
		int fd_p2b[2];
		// pipes
		int Rpipe = pipe(fd_p2b);

		if (Rpipe == -1)
		{
			printf("erro ao criar pipe\n");
			exit(1);
		}

		int countProms = leProms(FPROMOTORES, Proms, fd_p2b);*/

	printf("\n...\n");
	fdRecebe = open(BACKEND_FIFO, O_RDONLY);

	if (fdRecebe == -1)
	{
		printf(">> Erro ao abrir o backend");
		return 1;
	}

	int nfd;
	fd_set read_fds;
	struct timeval tv;
	int maior = fdRecebe + 1; //  maior FD acrescido de 1
	char buffer[100];

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
			contaUsers = leComandosAdmin(buffer, a, contaUsers, i, item_len);
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
					contaUsers = eliminaUser(a, mensagemRecebida.nome, contaUsers);
				}
				else if (mensagemRecebida.sair == 1)
				{
					printf("\n>> User %s saiu sem avisar!\n", mensagemRecebida.nome);
					contaUsers = eliminaUser(a, mensagemRecebida.nome, contaUsers);
				}
				else
				{

					int valido = isUserValid(mensagemRecebida.nome, mensagemRecebida.pass);
					int aux = existe(a, contaUsers, mensagemRecebida.nome); // se ja estiver logado nao pode logar otv
					if (aux == 0 && valido == 1)
					{
						printf("\n>> Utilizador: %s logado com pid %d\n", mensagemRecebida.nome, mensagemRecebida.pid);
						resposta.res = 1;
						++contaUsers;
						a = addUser(a, contaUsers, mensagemRecebida.nome, mensagemRecebida.pid);
					}
					else
					{
						// AwayFromKeyboard(a,mensagemRecebida.nome,user_len);
						resposta.res = 0;
					}
					sprintf(CLIENT_FIFO_FINAL, CLIENT_FIFO, mensagemRecebida.pid);
					fdEnvio = open(CLIENT_FIFO_FINAL, O_WRONLY);
					write(fdEnvio, &resposta, sizeof(resposta));
					for (int j = 0; j < item_len; j++)
					{
						write(fdEnvio, &i[j], sizeof(item));
					}
					for (int j = 0; j < item_len; j++)
					{
						write(fdEnvio, &a[j], sizeof(user));
					}
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
