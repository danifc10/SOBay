#define BACKEND_FIFO "BACKEND"
#define FPROMOTORES "ficheiro_promotores.txt"
#define CLIENT_FIFO "CLIENTE%d"
#define TAM 20
char CLIENT_FIFO_FINAL[100];
typedef struct
{
	pid_t pid;
	char nome[100];
	char pass[100];
	char com[100];
	int sair;
} dataMsg;

typedef struct
{
	pid_t pidB;
	int res;
	item *i;
    int itam;
    int uTam;
} dataRPL;


