#include "redirections.h"

// function where the piped system commands is executed
void pipedExec(char** parsed, char** parsedpipe, int pipe_size)
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

        bool pipe = true; // start as true to execute the while once
        char * str_piped[100];
        char * str[100];
        int size;
        int c = 1;
        while (pipe) {
            c++;
            if ((size = isPiped(parsedpipe, str_piped, pipe_size, str)) > 0) {
                // printf("more pipe\n");

                // child
                // it needs to read and write
                if (fork() == 0) {
                    dup2(pipefd[0], STDIN_FILENO);
                    close(pipefd[0]);
                    dup2(pipefd[1], STDOUT_FILENO);
                    close(pipefd[1]);

                    execvp(str[0], str);
                    exit(FAILED_EXEC);
                }

                for (int v = 0; v < pipe_size; v++) {
                    if (v < size) parsedpipe[v] = str_piped[v];
                    else parsedpipe[v] = NULL;
                }
                pipe_size = size;
            } else {
                // printf("no more pipe\n");
                pipe = false;

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

                    execvp(str[0], str);
                    exit(FAILED_EXEC);
                }
                else {
                    // close the pipe for the father or the childs will wait indefinitely for EOF in the pipe
                    close(pipefd[1]);
                    close(pipefd[0]);
                    // printf("father's waiting...\n");
                    // parent executing, waiting for two children
                    for (int v = 0; v < c; v++) {
                        // printf("wait...\n");
                        wait(NULL);
                    }
                }
            }
        }
    }
}

// check if str contain a pipe and store it in str_piped and str
int isPiped(char ** tabcmd, char** str_piped, int cmd_size, char ** str)
{
    int i;
    int c = 0;
    int n = 0;
    bool pipe = false;
    // we search for a pipe
    for (i = 0; i < cmd_size; i++) {
        if (strstr(tabcmd[i], "|") && !pipe)
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