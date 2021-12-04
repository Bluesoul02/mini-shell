#include "myshell.h"
#include "commandes.h"

#define CUSTOMCMD_SIZE 1

char *errormsg[]={"No error",ROUGE("Impossible to fork process"),ROUGE("Exec failed")};
char *customcmd[CUSTOMCMD_SIZE]= {"myls"};
void (*customfct[CUSTOMCMD_SIZE]) (char * directory, char * parameters) = {&myls};

void init() { // Clearing and initializing the shell
    clear();
    printf("---------- WELCOME TO OUR SHELL PROJECT ----------\n\n\n\n");
    printf("Made by Kévin Avot and Grégoire Delacroix\n\n\n\n");
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
    char lgcmd[LGCMD_SIZE],*tabcmd2[100][BUFFER_SIZE],*s,**ps;
    pid_t pid;
    int i,j,status,in,out;
    char * parameters = "";
    char * directory = "";

    for(;;){

        in=0;
        out=0;
        if(!fgets(lgcmd,LGCMD_SIZE-1,stdin)) break;
        for(s=lgcmd;isspace(*s) && *s != ';';s++); // we skip the spaces and semicolons at the beginning of the sentence
        for(i=0;*s;i++) { // for all the sentence
            tabcmd2[out][in++]=s; // we affect the actual s that represents our position in the sentence
            while(!isspace(*s) && *s != ';') s++; // we go to the end of the actual word
            if(*s == ';') { // multiple commands
                tabcmd2[out++][in]=NULL; // one command per array
                in=0;
            }
            *s++ = '\0'; // we put the end of word
            while(isspace(*s) || *s == ';') s++; // we skip all the spaces and semicolons for the next word
            if(*s == '\0' && in == 0) out--; // if we went to the next array but no need
        }
        if(i){
            if(in) tabcmd2[out][in]=NULL;
            for(j=0;j<=out;j++) { // one processus per task/command
                if((pid=fork()) == ERR) fatalsyserror(1);
                if(!pid) { // execute the next command
                    for (int k = 0; k < CUSTOMCMD_SIZE; k++) {
                        if (strcmp(*tabcmd2[j], customcmd[k]) == 0) {
                            for (int m = 1; m < in; m++) {
                                printf("param : %s\n", tabcmd2[j][m]);
                                if (!strncmp(tabcmd2[j][m], "-", 1)) {
                                    // parameter
                                    strcat(parameters, tabcmd2[j][m]);
                                    printf("parameters : %s\n", parameters);
                                } else {
                                    // not a parameter
                                    strcat(directory, tabcmd2[j][m]);
                                }
                            }
                            (*customfct[k])(directory, parameters);
                            exit(0);
                        }
                    }
                    execvp(*tabcmd2[j],tabcmd2[j]);
                    syserror(2);
                    exit(FAILED_EXEC);
                } else { // wait for his sons to finish their tasks
                    wait(&status);
                    if(WIFEXITED(status)){ // print the commmand status
                        if((status=WEXITSTATUS(status)) != FAILED_EXEC){
                            printf(VERT("exit status of ["));
                            for(ps=tabcmd2[j];*ps;ps++) printf("%s",*ps);
                            printf(VERT("]=%d\n"),status);
                        }
                    } else puts(ROUGE("Anormal exit"));
                }
            }
        }
    }
    exit(0);
}