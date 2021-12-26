#include "global.h"
#include <glob.h>
#include <grp.h>
#include <pwd.h>
#include <stdint.h>
#include <math.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <assert.h>

typedef struct Variable Variable;

struct Variable {
  char * name;
  char * val;
  Variable *precedent;
  Variable *suivant;
};

typedef struct Liste Liste;

struct Liste {
    Variable *variable;
};

void mycd(char * directory);

void myls(char * directory, char * parameters);

int myglob(glob_t globbuf, char * tab[], int limit);

void myexit();

int setLocalVariable(char * infos, Liste *liste);

int manageVariables(int p[2], char * tab[], int size, Liste *liste);

void allVariables(Liste *liste);

int isVariable(char * val);

char *valVariable(char * var, Liste *liste);

void freeVariables(Liste *liste);
