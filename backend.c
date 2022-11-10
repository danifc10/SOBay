#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
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
		else if(strcmp(comando, "prom")== 0 )
		{
			printf("\n valido");
			return 1;
		}
		else if(strcmp(comando, "reprom") == 0)
		{
			printf("\n valido");
			return 1;
		}
		else if(strcmp(comando, "close")==0)
		{
			printf("\n valido");
			return 0; //comando valido mas retorna 0 porque vai fechar
		}else
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
		else if(strcmp(comando, "cancel") == 0)
		{
			printf("\n valido comando: %s nomePromotor: %s", comando, argumento);
			return 1;
		}else
			printf("\n\nComando Invalido");
			return 0;
		break;

	default:
		printf("\n\nComando Invalido");
		return 0;
		break;	
	}
}
void mostra(int signal,siginfo_t *info,void *v){
	printf("%s\n",info->si_value.sival_char);
	srand(time(NULL));
}
int main(){
	char comando[20];
	int aux = 0;
	int pid;
	struct sigaction sa ;
	setbuf(stdout,NULL);
	sa.sa_sigaction=mostra;
	sa.sa_flags=SA_RESTART | SA_SIGINFO;
	sigaction(SIGUSR1,&sa,NULL);
	srand(time(NULL));
	do 
	{	//fiz o ciclo porque
		//o programa so é executado uma vez e dependendo do valor 
		//que a funcao retorna ele deixa ou nao o admin continuar a enserir comandos
		printf("\n\n Deseja testar que funcionalidade?\n");
		fgets(comando, 200, stdin);
		aux = leComandosAdmin(comando);

	}while (aux != 0);
	
	return 0;
}
