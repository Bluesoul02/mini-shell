#include "commandes.h"

void changeDir(char * directory) {
    if (chdir(directory) != 0)
        perror("No such file or directory");
}
