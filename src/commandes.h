#include "global.h"
#include <grp.h>
#include <pwd.h>
#include <stdint.h>
#include <math.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <assert.h>

void mycd(char * directory);

void myls(char * directory, char * parameters);

void myexit();
