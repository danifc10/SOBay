#include <stdio.h>
#include <ctype.h>
#include "item.h"
#include "user.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include "users_lib.h"
#include <errno.h>

item *i;
user *utilizadores;
int utilizadores_len;
void mostraItem()
{
	while (i)
	{
		printf("\n\nID: %d\n", i->id);
		printf("Nome: %s\n", i->nome);
		printf("Catg: %s\n", i->categoria);
		printf("ValorBase: %d\n", i->valor_base);
		printf("CompraJa: %d\n", i->compra_ja);
		printf("Tempo: %d\n", i->tempo);

		i = i->prox;
	}
}

void adicionaItem(char *n, int id, char *ctg, int vb, int cj, int tmp)
{
	item *aux, *new = malloc(sizeof(item));

	if (new)
	{
		strcpy((new->nome), n);
		strcpy((new->categoria), ctg);
		new->id = id;
		new->tempo = tmp;
		new->valor_base = vb;
		new->compra_ja = cj;
		new->prox = NULL;

		if (i == NULL)
		{
			i = new;
		}
		else
		{
			aux = i;
			while (aux->prox)
			{
				aux = aux->prox;
			}

			aux->prox = new;
		}
	}
	else
	{
		printf("erro ao alocar memoria\n");
	}
}

void leFicheiroItem(char *nomeFich)
{
	FILE *f;
	char Linha[100];

	f = fopen(nomeFich, "rb");

	if (f == NULL)
	{
		printf("ERRO ao abrir ficheiro\n");
		return;
	}

	while (!feof(f))
	{

		char nome[100], categoria[100], nomeU[100], licitador[100];
		int id, valor_base, compra_ja, tempo;

		fgets(Linha, 100, f);
		sscanf(Linha, "%d %s %s %d %d %d %s %s", &id, &nome, &categoria, &valor_base, &compra_ja, &tempo, &nomeU, &licitador);

		adicionaItem(nome, id, categoria, valor_base, compra_ja, tempo);
	}

	fclose(f);
}

int leComandosAdmin(char *comando)
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
			printf("\n valido");
			return 1;
		}
		else if (strcmp(comando, "users") == 0)
		{
			printf("\n valido");
			return 1;
		}
		else if (strcmp(comando, "prom") == 0)
		{
			printf("\n valido");
			return 1;
		}
		else if (strcmp(comando, "reprom") == 0)
		{
			printf("\n valido");
			return 1;
		}
		else if (strcmp(comando, "close") == 0)
		{
			printf("\n valido");
			return 0; // comando valido mas retorna 0 porque vai fechar
		}
		else
			printf("\n\nComando Invalido");
		return 0;
		break;

	case 1:

		sscanf(aux, "%s %s", comando, argumento);
		if (strcmp(comando, "kick") == 0)
		{
			printf("\nvalido comando: %s user %s", comando, argumento);
			return 1;
		}
		else if (strcmp(comando, "cancel") == 0)
		{
			printf("\n valido comando: %s nomePromotor: %s", comando, argumento);
			return 1;
		}
		else
			printf("\n\nComando Invalido");
		return 0;
		break;

	default:
		printf("\n\nComando Invalido");
		return 0;
		break;
	}
}

int executaPromotor(int fd_p2b[2])
{
	int f = fork();
	if (f == -1)
	{
		printf("erro ao criar filho\n");
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

int loadUsersFile(char *pathname)
{
	char buffer[100];
	FILE *f;
	f = fopen(pathname, "rt");
	if (f == NULL)
	{
		printf("ERRO: %s\n", getLastErrorText());
		fclose(f);
		return -1;
	}
	int j = 0;
	while(fgets(buffer,100,f)){j++;}

	fclose(f);
	FILE *f1;
	f1=fopen(pathname,"rt");
	utilizadores=(user *) malloc(j*sizeof(user));
	int k=0;
	while(fgets(buffer,100,f1)){
		sscanf(buffer,"%s %s %d",utilizadores[k].nome,utilizadores[k].password,&utilizadores[k].saldo);
		k++;
	}
	fclose(f1);
	return j;
}

int saveUsersFile(char *filename)
{
	FILE *f;
	f = fopen(filename, "wt");
	if (f == NULL)
	{
		printf("ERRO: %s\n", getLastErrorText());
		fclose(f);
		return -1;
	}
	for (int j = 0; j < utilizadores_len; j++)
	{
		fprintf(f, "%s %s %d\n", utilizadores[j].nome, utilizadores[j].password, utilizadores[j].saldo);
	}
	fclose(f);
}
int isUserValid(char *username, char *password)
{
	FILE *f;
	char Linha[100];

	f = fopen(USER_FILENAME, "rb");

	if (f == NULL)
	{
		printf("\nERRO:	 %s", getLastErrorText());
		return -1;
	}
	int i = 0;
	int aux = 0;
	while (fgets(Linha, 100, f))
	{
		char user[100], pass[100];
		int saldo;

		sscanf(Linha, "%s %s %d", user, pass, &saldo);

		if (strcmp(username, user) == 0)
		{
			aux++;
			if (strcmp(pass, password) == 0)
			{
				aux++;
			}
		}
	}

	fclose(f);

	if (aux == 2)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
int getUserBalance(char *username)
{
	int j;
	for (j = 0; strcmp(utilizadores[j].nome, username) != 0; j++)
	{
		if (j == utilizadores_len)
		{
			break;
		}
	}
	if (strcmp(utilizadores[j].nome, username) == 0)
	{
		if (utilizadores[j].saldo > 0)
		{
			utilizadores[j].saldo = utilizadores[j].saldo - 1;
		}
		return utilizadores[j].saldo;
	}
	return -1;
}
int updateUserBalance(char *username, int value)
{
	int j;
	for (j = 0; strcmp(utilizadores[j].nome, username) != 0; j++)
	{
		if (j == utilizadores_len)
		{
			break;
		}
	}
	if (strcmp(utilizadores[j].nome, username) == 0)
	{
		if (utilizadores[j].saldo > 0)
		{
			utilizadores[j].saldo = utilizadores[j].saldo - 1;
		}
		utilizadores[j].saldo = value;
		return 0;
	}
	return -1;
}
void mostrausers()
{

	for (int j = 0; j < utilizadores_len; j++)
	{
		printf("nome: %s pass: %s saldo: %d \n", utilizadores[j].nome, utilizadores[j].password, utilizadores[j].saldo);
	}
}
const char *getLastErrorText()
{
	return strerror(errno);
	;
}
void freeUsers(){
	for(int j=0;j<utilizadores_len;j++){
		free(utilizadores[j]);
	}
}
int main()
{
	char outputPromotores[100];
	// maximo de promotores
	int pid_promotor[10];

	for (int i = 0; i < 10; i++)
	{
		pid_promotor[i] = 0;
	}

	char comando[20];
	int aux = 0;
	// criar 2 unnamed pipes
	int fd_p2b[2];
	// pipes
	int Rpipe = pipe(fd_p2b);

	if (Rpipe == -1)
	{
		printf("erro ao criar pipe\n");
		exit(1);
	}

	int pid = executaPromotor(fd_p2b);

	for (int i = 0; i < 10; i++)
	{
		if (pid_promotor[i] == 0)
		{
			pid_promotor[i] = pid;
			break;
		}
	}

	// --------------variaveis para teste------------

	char *nome = "daniela";
	char *pass = "ola";
	int aux1;

	printf("\n-----------Informacao do pid do backend e promotores-----------\n");
	printf("\n>>Pid backend: %d Pid promotor: %d\n", getpid(), pid);

	printf("\n-----------Leitura do ficheiro dos items--------------------\n");
	leFicheiroItem(FITEM);
	mostraItem();

	printf("\n-----------Leitura do ficheiro dos utilizadores----------------\n");
	utilizadores_len = loadUsersFile(USER_FILENAME);
	printf("\n>>Numero de utilizadores: %d\n", utilizadores_len);

	printf("\n-----------Verificacao de credenciais do user----------------\n");
	int b = isUserValid(nome, pass); // 1 se existe 0 se nao existe ou pass errada
	printf("\n>>(1-existe ; 0-nao existe ou pass errada ):: %d\n", b);

	printf("\n-----------Lista dos users----------------\n");
	printf("\n-----------Teste funcao getUserBalance----------------\n");
	printf("\n>>Saldo do utilizador/a %s : %d\n", nome, getUserBalance(nome));
	updateUserBalance(nome, 10);
	printf("\n-----------Lista dos users depois de atualizada----------------\n");
	mostrausers();
	saveUsersFile(USER_FILENAME);
	
	do
	{
		printf("\n\n>>Deseja testar que comando?\n");
		fgets(comando, 200, stdin);
		aux1 = leComandosAdmin(comando);

	} while (aux1 != 0);

	char resposta;
	printf("\n>>Deseja lançar um promotor ?(y/n)\n");
	scanf("%c", &resposta);
	union sigval valores;
	valores.sival_int = -1;
	if (resposta == 'y')
	{
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
	}

	return 0;
}
