#include "item.h"

// retorna o prox id
int getId(int tam){
	return (++tam);
}

void mostraItem(item *i, int tam)
{
	for (int j = 0; j < tam; j++)
	{
		printf("Id: %d\tNome: %s\tCatg: %s\tValor: %d\tCompraJa: %d\tVendedor: %s\tLicitador: %s\n ", i[j].id, i[j].nome, i[j].categoria, i[j].valor_base, i[j].compra_ja, i[j].dono, i[j].licitador);
	}
}

item *adicionaItem(item *i, int *tam, char *n, int id, char *ctg, int vb, int cj, int tmp, char *dono, char *licitador, int tempoIni)
{

	i = (item *)realloc(i, sizeof(item)*((*tam)+1));
	if(i ==NULL){
		printf("erro ao alocar memoria\n");
		return NULL;
	}

	strcpy(i[*tam].nome, n);
	strcpy(i[*tam].categoria, ctg);
	strcpy(i[*tam].dono, dono);
	strcpy(i[*tam].licitador, licitador);
	i[*tam].id = id;
	i[*tam].valor_base = vb;
	i[*tam].compra_ja = cj;
	i[*tam].tempo = tmp;
	i[*tam].tempoInicio = tempoIni;
	i[*tam].buyTempo = 0;
	i[*tam].valorProm = 0;
	i[*tam].tempoProm = 0;
	++(*tam);

	return i;
}

item *leFicheiroItem(char *nomeFich, item *i, int *tam)
{
	FILE *f;
	char Linha[100];

	f = fopen(nomeFich, "rt");

	if (f == NULL)
	{
		printf("ERRO ao abrir ficheiro");
		fclose(f);
		return 0;
	}

	while (!feof(f))
	{

		char nome[100], categoria[100], nomeU[100], licitador[100];
		int id, valor_base, compra_ja, tempo;

		fgets(Linha, 100, f);
		sscanf(Linha, "%d %s %s %d %d %d %s %s", &id, &nome, &categoria, &valor_base, &compra_ja, &tempo, &nomeU, &licitador);
		i = adicionaItem(i, tam, nome, id, categoria, valor_base, compra_ja, tempo, nomeU, licitador, 0);

	}

	fclose(f);
	return i;
}

void licat(char *ctg, item *i, int item_len)
{

	for (int j = 0; j < item_len; j++)
	{
		if (strcmp(i[j].categoria, ctg) == 0)
		{
			printf("Id: %d\tNome: %s\tCatg: %s\tValor: %d\tCompraJa: %d\tVendedor: %s\tLicitador: %s\n\n ", i[j].id, i[j].nome, i[j].categoria, i[j].valor_base, i[j].compra_ja, i[j].dono, i[j].licitador);
		}
	}
}

void lisel(char *nome, item *i, int item_len)
{
	for (int j = 0; j < item_len; j++)
	{
		if (strcmp(i[j].dono, nome) == 0)
		{
			printf("Id: %d\tNome: %s\tCatg: %s\tValor: %d\tCompraJa: %d\tVendedor: %s\tLicitador: %s\n\n ", i[j].id, i[j].nome, i[j].categoria, i[j].valor_base, i[j].compra_ja, i[j].dono, i[j].licitador);
		}
	}
}

void lival(int value, item *i,int item_len)
{

	for (int j = 0; j < item_len; j++)
	{
		if (value >= i[j].valor_base)
		{
			printf("Id: %d\tNome: %s\tCatg: %s\tValor: %d\tCompraJa: %d\tVendedor: %s\tLicitador: %s\n\n ", i[j].id, i[j].nome, i[j].categoria, i[j].valor_base, i[j].compra_ja, i[j].dono, i[j].licitador);
		}
	}
}

void litime(int time, item *i, int item_len)
{
	for (int j = 0; j < item_len; j++)
	{
		if (time >= i[j].tempo)
		{
			printf("Id: %d\tNome: %s\tCatg: %s\tValor: %d\tCompraJa: %d\tVendedor: %s\tLicitador: %s\n\n ", i[j].id, i[j].nome, i[j].categoria, i[j].valor_base, i[j].compra_ja, i[j].dono, i[j].licitador);
		}
	}
}

item * eliminaItem(int id, item *i, int *item_len){
	for (int j = 0; j < *item_len; j++)
	{
		if (i[j].id == id)
		{
			for (int p = j; p < ((*item_len) - 1); p++)
				i[p] = i[p + 1];
			break;
		}
	}
	--(*item_len);
	if (*item_len == 0)
	{
		free(i);
		return NULL;
	}
	item *a = (item *)realloc(i, sizeof(item) * (*item_len));
	if (a == NULL)
	{
		printf("error allocating memory\n");
		exit(1);
	}
	return a;
}

// retorna 1 se pode comprar e 0 se nao 2 se compraJA
int compraItem(item *i, int id, int valor, char *nome, int saldo, int *item_len){
	for(int j = 0; j < *item_len ;j++){
		if(i[j].id == id){
			if(i[j].compra_ja <= valor && i[j].compra_ja != 0 &&( valor <= saldo)&&(strcmp(i[j].dono, nome)!=0) ){
				return 2;
			}else if((valor >= i[j].valor_base )&&( valor <= saldo)&&(strcmp(i[j].dono, nome)!=0)){
				return 1;
			}
		}
	}
	return 0;
}

void atualizaFitems(item *i, int tam, char *nome, int tempoAtual){
	FILE *p;

	p = fopen(nome, "w");

	if(p == NULL)
		return;

	for(int j = 0; j < tam ; j++){
		i[j].tempo = i[j].tempo - tempoAtual;
		if(j == tam -1){
			fprintf(p, "%d %s %s %d %d %d %s %s", i[j].id, i[j].nome, i[j].categoria, i[j].valor_base, i[j].compra_ja, i[j].tempo, i[j].dono, i[j].licitador);
		}else{
			fprintf(p, "%d %s %s %d %d %d %s %s\n", i[j].id, i[j].nome, i[j].categoria, i[j].valor_base, i[j].compra_ja, i[j].tempo, i[j].dono, i[j].licitador);
		}
		
	}
	fclose(p);
}

