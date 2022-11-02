#include <stdio.h>

#include "user.h"

void preencheUtilizador(user *a){
	a->nItem=0;
	a->bi=30766643;
}
void mostraInfo(user a){
	printf("nItem = %d, bi = %d",a.nItem,a.bi);

}
int main(int *argc,int *argv[]){
	user a;
	preencheUtilizador(&a);
	mostraInfo(a);
	return 0;
}
