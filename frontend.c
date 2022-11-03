#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "user.h"


user *recuperaUtilizadores(char *filename,int *tamanho){
	FILE *f;
	f=fopen(filename,"rt");
	if(f==NULL){
		printf("Erro a abrir %s",filename);
		return NULL;

	}else{
		user *a=NULL;
		int numUsers;
		fscanf(f,"%d\n",&numUsers);
		a=malloc(sizeof(user)*numUsers);
		
		if(a==NULL){
			return NULL;
		}
		char buffer[500];
	
		int i=0;
		while(feof(f)==0){
			fgets(buffer,500,f);
			sscanf(buffer,"%s %s %d",a[i].nome,a[i].password,&a[i].saldo);
			i++;
		}
		*tamanho=numUsers;
		fclose(f);
		
		return a;
	}

}

void escreveFicheiro(char *filename,user *a,int tamanho){
	FILE *f;
	f=fopen(filename,"wt");
	if(f==NULL){
		return;
	}
	int i=0;
	fprintf(f,"%d\n",tamanho);
	while(i<tamanho){
		fprintf(f,"%s %s %d\n",a[i].nome,a[i].password,a[i].saldo);

		i++;
	}
	fclose(f);
}

user *adicionaUser(user *a,int *tamanho,char n[],char pass[]){

	user *aux = NULL;
	aux=malloc(sizeof(user));
	if(aux==NULL){
		return NULL;
	}else{
		strcpy(aux->nome,n);
		strcpy(aux->password,pass);
		aux->saldo=0;
		a[*tamanho]=*aux;
		*tamanho=*tamanho +1;
	}
	free(aux);
	return a;
}

int main(int argc,char *argv[]){
	if(argc==3){
		int tamanho=0;
		user *a=NULL;
		a=recuperaUtilizadores("ficheiro_utilizadores.txt",&tamanho);
		a=adicionaUser(a,&tamanho,argv[1],argv[2]);
		escreveFicheiro("ficheiro_utilizadores.txt",a,tamanho);
	}else{
		printf("Falta de argumentos\n");
	}

	
	return 0;
}