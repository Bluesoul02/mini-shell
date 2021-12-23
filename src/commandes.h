#include "global.h"
#include <glob.h>

void mycd(char * directory);

void myls(char * directory, char * parameters);

int myglob(glob_t globbuf, char *tab[], int limit);