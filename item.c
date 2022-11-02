#include "item.h"

void mostraItems(){
    item *p;
    if(p == NULL)
    {
        return;
    }else{
        while (p->prox != NULL)
        {
            printf("Nome: %s, ", p->nome );
            printf("ID: %d, ", p->id);
            printf("Categoria: %s, ", p->categoria);
            printf("Valor Base: %d, ", p->valor_base);
            printf("Valor Compra já: %d, ", p->compra_ja);
            printf("Tempo: %d", p->tempo);
            p = p->prox;
            printf("\n\n");
        }
    }
}


void addItem(item **p, int id, char *nome, char *catg, int vb, int cj, int tempo){
    if(*p == NULL)
    {
        *p = malloc(sizeof(item));
        if(*p == NULL)
        {
            return;
        }else{
            (*p)->id = id;
            strcpy((*p)->categoria, catg);
            strcpy((*p)->nome, nome);
            (*p)->tempo = tempo;
            (*p)->valor_base = vb;
            (*p)->compra_ja = cj;
            (**p).prox = NULL;
            printf("adicionei");
        }
    }else{
        addItem(&(**p).prox, id, nome, catg, vb, cj, tempo);
    }
}