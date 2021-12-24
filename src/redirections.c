#include "redirections.h"

// function where the piped system commands is executed
void pipedExec(char** parsed, char** parsedpipe)
{
    // 0 is read, 1 is write
    int pipefd[2]; 
    pid_t p1, p2;
  
    if (pipe(pipefd) < 0) {
        printf("\nPipe could not be initialized");
        return;
    }
    p1 = fork();
    if (p1 < 0) {
        printf("\nCould not fork");
        return;
    }
  
    if (p1 == 0) {
        // child 1
        // it only needs to write
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
  
        if (execvp(parsed[0], parsed) < 0) {
            printf("\nCould not execute command 1..");
            exit(0);
        }
    } else {
        // father
        p2 = fork();
  
        if (p2 < 0) {
            printf("\nCould not fork");
            return;
        }
  
        // child
        // it only needs to read
        if (p2 == 0) {
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
            if (execvp(parsedpipe[0], parsedpipe) < 0) {
                printf("\nCould not execute command 2..");
                exit(0);
            }
        } else {
            // parent executing, waiting for two children
            wait(NULL);
            wait(NULL);
        }
    }
}

// check if str contain a pipe
int isNotPiped(char ** tabcmd, char** str_piped, int cmd_size)
{
    int i;
    int c = 0;
    bool pipe = false;
    // we search for a pipe
    for (i = 0; i < cmd_size; i++) {
        printf("%s\n", tabcmd[i]);
        if (strstr(tabcmd[i], "|"))
            pipe = true;
        if (pipe) {
            printf("pipe");
            // we copy the content after the pipe
            str_piped[c++] = tabcmd[i];
        }
    }
    if (pipe)
        return c; // return the size if a pipe was found
    else
        return 0;
}