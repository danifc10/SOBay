#include <stdio.h>
#include "item.h"
#define LIST "list"
//teste de main para receber comandos (neste caso o list)
// para compilar: gcc cliente.c item.c item.h -o cliente
// exemplo > $./cliente list
//      output -> comando valido
int main(int agrc, char *argv[]){
    item *p;

    if(strcmp(argv[1], LIST) == 0)
    {
        mostraItems(p);
    }else{
        printf("comando invalido\n");
        return 1;
    }

}

