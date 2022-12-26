#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include "user.h"
#include "item.h"
#include "users_lib.h"
#include "utils.h"
dataMsg mensagem;
dataRPL resposta;

int compraItem(item *i, int id, int valor, char *nome, int saldo, int item_len){
	for(int j = 0; j < item_len ;j++){
		if(i[j].id == id){
			if(valor >= i[j].valor_base && valor <= saldo){
				strcpy(i[j].dono, nome);
				updateUserBalance(nome, (saldo-i[j].valor_base));
				saveUsersFile(USER_FILENAME);
				eliminaItem(id, i, item_len);
				//ELIMINA ITEM E MANDA NOVA STRUCT PARA BACKEND
				return 1;
			}else{
				return 0;
			}
		}
	}
}
/*
int leFicheiroItem(char *nomeFich, item *i)
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
	int count = 0;
	while (!feof(f))
	{

		char nome[100], categoria[100], nomeU[100], licitador[100];
		int id, valor_base, compra_ja, tempo;

		fgets(Linha, 100, f);
		sscanf(Linha, "%d %s %s %d %d %d %s %s", &id, &nome, &categoria, &valor_base, &compra_ja, &tempo, &nomeU, &licitador);

		adicionaItem(i, item_len,nome, id, categoria, valor_base, compra_ja, tempo);
		count++;
	}

	fclose(f);
	return count;
}*/





/*
void atualizaFichItem(char *nomeFich,int id){
	printf("entei");
	FILE *f, *fnew;
	char linha[30], arg[100];
	char output[]={' '};

	f = fopen("items.txt", "r");
	fnew = fopen("item.txt", "w+");

	if(f == NULL){
		printf("erro");
	}

	while(!feof(f)){
		char nome[100], categoria[100], nomeU[100], licitador[100];
		int idF, valor_base, compra_ja, tempo;

		fgets(linha, 30, f);
		sscanf(linha, "%d %s %s %d %d %d %s %s", &idF, &nome, &categoria, &valor_base, &compra_ja, &tempo, &nomeU, &licitador);

		if(id != idF){
			fwrite(linha,1, sizeof(linha), fnew);
		}
	}
	
	fclose(fnew);
	return;
}
*/
int leComandosCliente(char *comando, item *i, dataMsg mensagem, user *a, int item_len)
{
	struct tm *tempo;
	time_t segundos;
	loadUsersFile(USER_FILENAME);

	char aux[100];
	int count = 0; // variavel para contar os espaços em branco

	for (int j = 0; j < strlen(comando) - 1; j++)
	{
		if (isspace(comando[j]))
		{
			count++;
		}
	}

	strcpy(aux, comando);

	char argumento[20];
	char categoria[20];
	char nome[20];
	int compreJa, duracao, id, valor;

	switch (count)
	{
	case 0:
		sscanf(aux, "%s", comando);

		if (strcmp(comando, "list") == 0)
		{
			printf("\nvalido\n\n");
			mostraItem(i, item_len);
			return 1;
		}
		else if (strcmp(comando, "cash") == 0)
		{
			printf("%s", mensagem.nome);
			char nome[20+1];
			strcpy(nome, mensagem.nome);
			printf("\n--- SALDO : %d", getUserBalance(nome));
			return 1;
		}
		else if (strcmp(comando, "time") == 0)
		{
			time(&segundos);			  // obtem o tempo em segundos
			tempo = localtime(&segundos); // converte o tempo para a hora atual
			printf("\nHora : %d:%d\n\n", tempo->tm_hour, tempo->tm_min);
			return 1;
		}
		else if (strcmp(comando, "exit") == 0)
		{
			printf("\n valido\n\n");
			return 0;
		}
		else
			printf("Comando invalido\n");
		return 1;
		break;

	case 1:

		sscanf(aux, "%s %s", comando, argumento);
		if (strcmp(comando, "licat") == 0)
		{
			// printf("\nvalido -  comando: %s --- categoria %s", comando, argumento);
			licat(argumento, i, item_len);
			return 1;
		}
		else if (strcmp(comando, "lisel") == 0)
		{
			// printf("\nvalido -  comando: %s --- user %s", comando, argumento);
			lisel(argumento, i, item_len);
			return 1;
		}
		else if (strcmp(comando, "litime") == 0)
		{
			valor = atoi(argumento);
			// printf("\nvalido - comando: %s --- tempo %d", comando, atoi(argumento));
			litime(valor, i, item_len);
			return 1;
		}
		else if (strcmp(comando, "lival") == 0)
		{
			valor = atoi(argumento);

			// printf("\nvalido -  comando: %s --- precoMAx %d", comando, atoi(argumento));
			lival(valor, i, item_len);
			return 1;
		}
		else if (strcmp(comando, "add") == 0)
		{
			valor = atoi(argumento);
			updateUserBalance(mensagem.nome, valor);
			printf("Saldo:%d", getUserBalance(mensagem.nome));
			saveUsersFile(USER_FILENAME);
			return 1;
		}
		else
			printf("Comando invalido\n");
		return 1;
		break;

	case 2:

		sscanf(aux, "%s %d %d", comando, &id, &valor);
		if (strcmp(comando, "buy") == 0)
		{
			int saldo = getUserBalance(mensagem.nome);
			int aux  = compraItem(i, id, valor, mensagem.nome, saldo, item_len);
			//atualizaFichItem(FITEM, id);
			if(aux == 1){
				printf("Item comprado com sucesso!\n");
			}else{
				printf("Erro ao comprar item!\n");
			}

			return 1;
		}
		else
			printf("Comando invalido\n");
		return 1;
		break;

	case 5:

		sscanf(aux, "%s %s %s %d %d %d", comando, nome, categoria, &valor, &compreJa, &duracao);

		if (strcmp(comando, "sell") == 0)
		{
			printf("\n valido \n comado: %s \n nome: %s \ncatg: %s \npreco: %d \n compreJ: %d\n tempo: %d\n", comando, nome, categoria, valor, compreJa, duracao);
			//i = adicionaItem(i, item_len+1,nome, id , categoria, valor, compreJa, duracao, mensagem.nome, "-");
			return 1;
		}
		else
		{
			printf("Comando invalido\n");
			return 1;
		}

	default:
		break;
	}
}
void handler_sinal(int signal, siginfo_t *info, void *extra)
{
	printf("\n\nDESCONECTADO");
	unlink(CLIENT_FIFO_FINAL);
	exit(1);
}
void sair_sem_aviso(int signal, siginfo_t *info, void *extra)
{
	mensagem.sair = 1;
	int fd_envio = open(BACKEND_FIFO, O_WRONLY);
	int size = write(fd_envio, &mensagem, sizeof(mensagem));
	close(fd_envio);
	unlink(CLIENT_FIFO_FINAL);
	exit(1);
}

/*
void handler_alarm(int num){
	int fd_envio = open(BACKEND_FIFO, O_WRONLY);
	int size = write(fd_envio, &mensagem, sizeof(mensagem));
	close(fd_envio);
	alarm(5);
}*/

int main(int argc, char *argv[])
{

	if (argc == 3)
	{
		mensagem.pid = getpid();
		int fd_envio, fd_resposta;

		// struct para quando o backend fecha o frontend
		struct sigaction sac;
		sac.sa_sigaction = handler_sinal;
		sigaction(SIGUSR1, &sac, NULL);

		// struct para tratar o sinal do ^C
		struct sigaction sa;
		sa.sa_sigaction = sair_sem_aviso;
		sigaction(SIGINT, &sa, NULL);

		sprintf(CLIENT_FIFO_FINAL, CLIENT_FIFO, getpid());

		if (mkfifo(CLIENT_FIFO_FINAL, 0666) == -1)
		{

			if (errno == EEXIST)
			{
				printf("\nfifo ja existe\n");
			}
			printf("erro ao abrir fifo\n");
			return 1;
		}

		strcpy(mensagem.nome, argv[1]);
		strcpy(mensagem.pass, argv[2]);
		

		fd_envio = open(BACKEND_FIFO, O_WRONLY);
		
		

		if (fd_envio == -1)
		{ // backend nao esta a correr logo frontend nao corre
			return 2;
		}
		int size = write(fd_envio, &mensagem, sizeof(mensagem));
		close(fd_envio);
		// RECEBER RESPOSTA
		fd_resposta = open(CLIENT_FIFO_FINAL, O_RDONLY);

		if (fd_resposta == -1)
		{
			printf("erro ao abrir fifo do clinte");
		}

		read(fd_resposta, &resposta, sizeof(resposta));

		int item_len = resposta.itam;
		user_len = resposta.uTam;
		item *i;
		i = (item *)malloc(item_len * sizeof(item));
		a = (user *)malloc(user_len * sizeof(user));

		for (int j = 0; j < item_len; j++)
		{
			read(fd_resposta, &i[j], sizeof(item));
		}
		for (int j = 0; j < user_len; j++)
		{
			read(fd_resposta, &a[j], sizeof(user));
		}
		close(fd_resposta);

		// BACKEND ACEITOU

		int aux;
		if (resposta.res == 1)
		{

			char comando[20];
			printf("Bem vindo %s!\n", argv[1]);

			do
			{
				printf("\n>> Deseja testar que comando?");
				fgets(comando, 200, stdin);
				aux = leComandosCliente(comando, i, mensagem, a, item_len);
				// handler_alarm(2);
				//  se sair manda info ao backend
				if (strcmp(comando, "exit") == 0)
				{
					strcpy(mensagem.com, comando);
					fd_envio = open(BACKEND_FIFO, O_WRONLY);
					write(fd_envio, &mensagem, sizeof(mensagem));
					close(fd_envio);
					printf("\n>> A SAIR..");
					unlink(CLIENT_FIFO_FINAL);
					exit(1);
				}

			} while (aux != 0);
		}
		else // BACKEND NAO ACEITOU
		{

			printf(">> Erro no login\n");
			unlink(CLIENT_FIFO_FINAL);
			return 3;
		}
		close(fd_envio);
	}
	else
	{
		printf(">> Erro em numero de argumentos\nSintaxe: ./frontend <nome> <pass>\n");
	}

	return 4;
}