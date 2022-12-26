#include "item.h"


void mostraItem(item *i, int tam)
{
	for (int j = 0; j < tam; j++)
	{
		printf("ITEM %d :\n", j);
		printf("Nome:%s Id:%d Catg.:%s Valor:%d CompraJa:%d Dono:%s Tempo:%d\n\n ", i[j].nome, i[j].id, i[j].categoria, i[j].valor_base, i[j].compra_ja, i[j].dono, i[j].tempo);
	}
}

item *adicionaItem(item *i, int tam, char *n, int id, char *ctg, int vb, int cj, int tmp, char *dono, char *licitador)
{
	strcpy(i[tam - 1].nome, n);
	strcpy(i[tam - 1].categoria, ctg);
	strcpy(i[tam - 1].dono, dono);
	strcpy(i[tam - 1].licitador, licitador);
	i[tam - 1].id = id;
	i[tam - 1].valor_base = vb;
	i[tam - 1].compra_ja = cj;
	i[tam - 1].tempo = tmp;

	return i;
}

item *leFicheiroItem(char *nomeFich, item *i)
{
	FILE *f;
	char Linha[100];

	f = fopen(nomeFich, "rt");

	if (f == NULL)
	{
		printf("ERRO");
		fclose(f);
		return 0;
	}
	int count = 1;
	while (!feof(f))
	{

		char nome[100], categoria[100], nomeU[100], licitador[100];
		int id, valor_base, compra_ja, tempo;

		fgets(Linha, 100, f);
		sscanf(Linha, "%d %s %s %d %d %d %s %s", &id, &nome, &categoria, &valor_base, &compra_ja, &tempo, &nomeU, &licitador);

		i = adicionaItem(i, count, nome, id, categoria, valor_base, compra_ja, tempo, nomeU, licitador);
		count++;
	}

	fclose(f);
	return i;
}
int contaItems(char *filename)
{
	FILE *f;
	int count = 0;
	char buff[100];

	f = fopen(filename, "rt");

	while (!feof(f))
	{
		fgets(buff, 100, f);
		count++;
	}
	return count;
}

void licat(char *ctg, item *i, int item_len)
{

	for (int j = 0; j < item_len; j++)
	{
		if (strcmp(i[j].categoria, ctg) == 0)
		{
			printf("Nome:%s Id:%d Catg.:%s Valor:%d CompraJa:%d Dono:%s Tempo:%d\n\n", i[j].nome, i[j].id, i[j].categoria, i[j].valor_base, i[j].compra_ja, i[j].dono, i[j].tempo);
		}
	}
}

void lisel(char *nome, item *i, int item_len)
{
	for (int j = 0; j < item_len; j++)
	{
		if (strcmp(i[j].dono, nome) == 0)
		{
			printf("Nome:%s Id:%d Catg.:%s Valor:%d CompraJa:%d Dono:%s Tempo:%d \n\n", i[j].nome, i[j].id, i[j].categoria, i[j].valor_base, i[j].compra_ja, i[j].dono, i[j].tempo);
		}
	}
}

void lival(int value, item *i,int item_len)
{

	for (int j = 0; j < item_len; j++)
	{
		if (value >= i[j].valor_base)
		{
			printf("Nome:%s Id:%d Catg.:%s Valor:%d CompraJa:%d Dono:%s Tempo:%d\n\n ", i[j].nome, i[j].id, i[j].categoria, i[j].valor_base, i[j].compra_ja, i[j].dono, i[j].tempo);
		}
	}
}

void litime(int time, item *i, int item_len)
{
	for (int j = 0; j < item_len; j++)
	{
		if (time >= i[j].tempo)
		{
			printf("Nome:%s Id:%d Catg.:%s Valor:%d CompraJa:%d Dono:%s Tempo:%d\n\n ", i[j].nome, i[j].id, i[j].categoria, i[j].valor_base, i[j].compra_ja, i[j].dono, i[j].tempo);
		}
	}
}
// 1 s eelimindao 0 se erro
int eliminaItem(int id, item *i, int item_len){
	for(int j =0; j < item_len; j++){		

		if(i[j].id == id){
			for(int a  = j; a < item_len-1;  a++){
				i[j]=i[j+1];
			}
			--item_len;
			return 1;
		}
	}
	return 0;
}

