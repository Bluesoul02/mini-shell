#include "commandes.h"

void mycd(char * directory) {
    printf("%s", directory);
    if (chdir(directory) != 0)
        perror("No such file or directory");
}
