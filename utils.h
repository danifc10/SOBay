typedef struct
{
	pid_t pid;
	char nome[100];
	char pass[100];
	char com[100];
} dataMsg;

typedef struct
{
	pid_t pidB;
	int res;
	item *i;
} dataRPL;