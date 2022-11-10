#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <time.h>
union sigval{
	int sival_int;
	char sival_char[4098];
	void *sival_ptr;
}
int main(int argc,int *argv[]){
	int pid;
	if(argc!=2){
		printf("Falta de pid\n");
		return 1;
	}
	pid=atoi(argv[1]);
	union sigval valor;
	char buffer[100];
	char categoria[30];
	int promocao,tempo;
	FILE *f;
	f= fopen("ficheiro_promotores.txt","rt");
	if(f==NULL){
		printf("erro a ler o ficheiro");
		return 0;
	}
	while(feof(f)==0){
		fgets(buffer,100,f);
		strcpy(valor.sival_char,buffer);
		sigqueue(pid,SIGUSR1,valor);
		//sscanf(buffer,"%s %d %d",categoria,&promocao,&tempo);
	}
	fclose(f);
	

	return 0;
}
