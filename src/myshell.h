#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#define clear() printf("\e[1;1H\e[2J");
#define syserr(m,n) perror(#m), exit(n);
#define err(m,n) printf("Error %c",m), exit(n);
#define ERR -1
#define ERR_EXEC 127
#define BUFFER_SIZE 1024

void init();
void printDirectory();
void loop();
int system(const char *commande);
void requiredLine();
int readLine(const char *commande);