#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <time.h>
int main(int argc,char *argv[]){
	setbuf(stdout,NULL);
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
		sscanf(buffer,"%s %d %d",categoria,&promocao,&tempo);
		printf("\n");
		fprintf(stdout,buffer);
		printf("\n");
	}
	fclose(f);
	

	return 0;
}
