#include "myshell.h"
#include "commandes.c"

#define CUSTOMCMD_SIZE 1

char *errormsg[]={"No error",ROUGE("Impossible to fork process"),ROUGE("Exec failed")};
char *customcmd[CUSTOMCMD_SIZE]= {"cd"};
// void (*customfct[CUSTOMCMD_SIZE]) (char * string) =  {&mycd};

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

int requiredLine() {
    char lgcmd[LGCMD_SIZE],*tabcmd[BUFFER_SIZE],*s,**ps;
    pid_t pid;
    int i,status;

    for(;;){
        if(!fgets(lgcmd,LGCMD_SIZE-1,stdin)) break;
        for(s=lgcmd;isspace(*s);s++);
        for(i=0;*s;i++) {
            tabcmd[i]=s;
            while(!isspace(*s)) s++;
            *s++ = '\0';
            while(isspace(*s)) s++;
        }
        if(i){
            tabcmd[i]=NULL;
            if((pid=fork()) == ERR) fatalsyserror(1);
            if(pid){
                wait(&status);
                if(WIFEXITED(status)){ // print the commmand status
                    if((status=WEXITSTATUS(status)) != FAILED_EXEC){
                        printf(VERT("exit status of ["));
                        for(ps=tabcmd;*ps;ps++) printf("%s",*ps);
                        printf(VERT("]=%d\n"),status);
                    }
                } else puts(ROUGE("Anormal exit"));
            } else {
                // filtrer avec strstr pour vérfier si dans la liste de cmd?
                for (int j = 0; j < CUSTOMCMD_SIZE; j++) 
                    if (strstr(*tabcmd, customcmd[i])) {
                        // customfct[i] ("build");
                    }
                execvp(*tabcmd,tabcmd);
                syserror(2);
                exit(FAILED_EXEC);
            }
        }
    }
    exit(0);
}