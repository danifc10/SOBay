#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc , char *argv[], char *envp[]){
 
 int n, s, i;

 if(argc!=4){
    printf("[ERRO]Nr. argumentos\n");
    return 1;
 }
   
 n = atoi(argv[1]);
 s = atoi(argv[3]);
 
 printf("INICIO...\n");
 for( i = 0; i < n ; i++)
 {
   printf("'%s' ", argv[2]);
   sleep(s);
   fflush(stdout);
 }
 
 printf("FIM\n");
 
} 
 

 
