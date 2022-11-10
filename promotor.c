#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
int main(){
	
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
	}
	fclose(f);
	

	return 0;
}
