#include "redirections.h"

// function where the piped system commands is executed
void pipedExec(char** parsed, char** parsedpipe)
{
    // printf("piped Exec\n");

    // 0 is read, 1 is write
    int pipefd[2]; 
    pid_t p1, p2;
  
    if (pipe(pipefd) < 0) {
        printf("Pipe could not be initialized\n");
        return;
    }
    p1 = fork();
    if (p1 < 0) {
        printf("Could not fork\n");
        return;
    }
  
    if (p1 == 0) {
        // child 1
        // it only needs to write
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
  
        execvp(parsed[0], parsed);
        exit(FAILED_EXEC);
    } 
    else {
        // father
        p2 = fork();
  
        if (p2 < 0) {
            printf("Could not fork\n");
            return;
        }
  
        // child
        // it only needs to read
        if (p2 == 0) {
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);

            execvp(parsedpipe[0], parsedpipe);
            exit(FAILED_EXEC);
        } else {
            close(pipefd[1]);
            close(pipefd[0]);
            printf("father's waiting...\n");
            // parent executing, waiting for two children
            wait(NULL);
            wait(NULL);
        }
    }
}

// check if str contain a pipe
int isNotPiped(char ** tabcmd, char** str_piped, int cmd_size, char ** str)
{
    int i;
    int c = 0;
    int n = 0;
    bool pipe = false;
    // we search for a pipe
    for (i = 0; i < cmd_size; i++) {
        if (strstr(tabcmd[i], "|"))
            pipe = true;
        else if (pipe) {
            // we copy the content after the pipe
            str_piped[c++] = tabcmd[i];
        }
        else {
            str[n++] = tabcmd[i];
        }
    }
    if (pipe)
        return c; // return the size if a pipe was found
    else
        return 0;
}