#include "commandes.h"

void mycd(char * directory) {
    printf("%s", directory);
    if (chdir(directory) != 0)
        printf("No such file or directory");
}
