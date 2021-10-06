#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <errno.h>

#define ERR -1
#define FAILED_EXEC 127
#define LGCMD_SIZE 4096 
#define BUFFER_SIZE 1024

#define clear() printf("\e[1;1H\e[2J");
#define syserror(x) perror(perror[errormsg[x]]);
#define fatalsyserror(x) syserror(x), exit (x);
#define err(m,n) printf("Error %c",m), exit(n);
#define ROUGE(m) "\033[01;31m"m"\033[0m"
#define VERT(m) "\033[01;32m"m"\033[0m"

char *errormsg[]={"No error",ROUGE("Impossible to fork process"), ROUGE("Exec failed")};