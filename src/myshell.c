#include "myshell.h"

void init() { // Clearing and initializing the shell
    clear();
    printf("---------- WELCOME TO OUR SHELL PROJECT ----------\n\n\n\n");
    printf("Made by Avot Kévin and Delacroix Grégoire\n\n\n\n");
    sleep(1);
}

void printDirectory() {
    printf(">");
}

void loop() {
    int status=1;
    do {
        printDirectory();
        requiredLine();
    } while(status);
}

void requiredLine() {
    int bufferSize = BUFFER_SIZE;
    int position = 0;
    char *buf = malloc(sizeof(char) * bufferSize);
    int c;

    if (!buf) err("Allocation error",2);
    
    while (1) {
        c = getchar();
        if (c == EOF || c == '\n') {
            buf[position++] = '\0';
            break;
        } else buf[position++] = c;
        if (position >= bufferSize) {
            bufferSize += BUFFER_SIZE;
            buf = realloc(buf, bufferSize);
            if (!buf) err("Allocation error :", 2)
        }
    }
    if(position) readLine(buf);
}

int readLine(const char *commande) {
    pid_t pid;
    int status;
    if((pid=fork()) == ERR) return ERR;
    if(!pid) {
        char *tab[]={"/bin/sh","-c",commande,NULL};
        execv(*tab,tab);
        return FAILED_EXEC;
    }
    wait(&status);
    if(WIFEXITED(status)) return WEXITSTATUS(status);
    else return ERR;
}

void printStatus(int status) {
    if(status == ERR) printf("Exec failed\n");
    // else printf("exit status of [%s]=%d",,);
}