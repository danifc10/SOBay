#include <stdio.h>
#include "item.h"
item *p;

addItem(&p, 10, "ola", "categoria", 100, 200, 5);
void comandosCliente(int agrc, char argv[]){
    char comando[20+1];

    switch (argv[0])
    {
    case "list":
        mostraItems();
        break;
    
    default:
        printf("argumento invalido");
        break;
    }

}