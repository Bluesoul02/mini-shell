#include "commandes.h"

void mycd(char * directory) {
    printf("%s", directory);
    if (chdir(directory) != 0)
        printf("No such file or directory");
}

void myls(char * directory, char * parameters) {
    if (strcmp(directory, "") == 0) directory = ".";
    char * args[] = {"ls", "--color", "-l", parameters, directory, NULL};
    execvp("ls", args);
}
