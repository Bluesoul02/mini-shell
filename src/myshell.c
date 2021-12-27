#include "myshell.h"

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
    char * buff = getcwd(NULL, 0);
    printf("%s>", buff);
    free(buff);
}

void loop() {
    printDirectory();
    requiredLine(); // waiting for input
}

/**
 * @brief Get the Mem Segment object
 *
 * @param key
 * @return int*
 */
int * getMemSegment(key_t key, int *shmid) {
    int *shm;

    // Create the segment.
    if ((*shmid = shmget(key, sizeof(int), IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    // printf("shmid = %d\n", *shmid);

    // Now we attach the segment to our data space.
    if ((shm = shmat(*shmid, NULL, 0)) == (int *)-1) {
        perror("shmat");
        exit(1);
    }

    return shm;
} // getMemSegment

int requiredLine() {
    char lgcmd[LGCMD_SIZE],*tabcmd2[100][BUFFER_SIZE],*s,**ps;
    pid_t pid;
    int i,j,status,in,out;
    // help distinguishing command executed from command executed by its childrens
    bool fathercmd = false;
    bool using_parameters = false;
    // contain command that will be executed after the command contained in str
    char * str_piped[100];
    // contain the first command in a pipeline
    char * str[100];
    // size of str_piped
    int pipe_size = 0;
    
    int shmid;
    int * shm;

    shm = getMemSegment(2905, &shmid);

    Liste* localVars = malloc(sizeof(*localVars)); // local variables
    localVars->variable = NULL;

    char * parameters = malloc(100 * sizeof(char)); // parameters of the command
    char * directory  = malloc(100 * sizeof(char)); // directory or file the command is aimed at

    for(;;){
        in=0;
        out=0;
        if(!fgets(lgcmd,LGCMD_SIZE-1,stdin)) break;
        for(s=lgcmd;isspace(*s);s++); // we skip the spaces at the beginning of the sentence
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
                int to=0,and=0,or=0,index;
                while(1) {
                    parameters = "";
                    index=0;
                    glob_t globbuf;
                    int p[2];
                    if (pipe(p)==ERR) fatalsyserror(1);;
                    char *tabcmd[BUFFER_SIZE] = { NULL };
                    while(tabcmd2[j][to] != NULL && strcmp("&&",tabcmd2[j][to]) && strcmp("||",tabcmd2[j][to])) { // separate each instruction
                        tabcmd[index++] = tabcmd2[j][to++];
                        if(index>1 && strcmp(tabcmd[index-2],"unset") && isVariable(tabcmd[index-1])) { // check to replace the value with the value of an existing variable
                            char *tempVar = valVariable(tabcmd[index-1], localVars);
                            if(tempVar) strcpy(tabcmd[index-1],tempVar);
                        }
                    }
                    if(tabcmd2[j][to] != NULL && strcmp("&&",tabcmd2[j][to]) == 0) { // in the case of multiple commands linked conditionally, execute each command separately one after the other
                        tabcmd[index] = NULL;
                        and = 1;
                    } else if(tabcmd2[j][to] != NULL && strcmp("||",tabcmd2[j][to]) == 0) {
                        tabcmd[index] = NULL;
                        or = 1;
                    }

                    if (strcmp(*tabcmd2[j], "cd") == 0) {
                        fathercmd = true; // the father executed the cmd
                        if (tabcmd2[j][1] == NULL) mycd("~"); // if no directory is set we cd to home directory
                        else mycd(tabcmd2[j][1]);
                    }
                    if (strcmp(*tabcmd2[j], "exit") == 0) {
                        fathercmd = true; // the father executed the cmd
                        myexit(tabcmd2[j][1]);
                    }
                    
                    if ((pipe_size = isPiped(tabcmd2[j], str_piped, index, str)) > 0) {

                        pipedExec(str, str_piped, pipe_size);

                        // end of pipelines
                        pipe_size = 0;
                        fathercmd = true;
                    }

                    if((pid=fork()) == ERR) fatalsyserror(1);
                    if(!pid && !fathercmd) { // execute the next command except if father already executed it
                        for (int k = 0; k < CUSTOMCMD_SIZE; k++) {
                            if (strcmp(*tabcmd, customcmd[k]) == 0) {
                                for (int m = 1; m < index; m++) {
                                    if (!strncmp(tabcmd[m], "-", 1)) {
                                        // parameter
                                        strcat(parameters, tabcmd[m]);
                                    } else {
                                        // not a parameter
                                        strcat(directory, tabcmd[m]);
                                    }
                                }
                                // launch the function associated to the cmd
                                (*customfct[k])(directory, parameters);
                                freeVariables(localVars);
                                exit(0);
                            }
                        }
                        if(strcmp("set",*tabcmd) == 0 || strcmp("setenv",*tabcmd) == 0 || strcmp("unset",*tabcmd) == 0 || strcmp("unsetenv",*tabcmd) == 0) { // set local or environment variable
                            free(directory);
                            int retour = manageVariables(p,tabcmd,index,localVars); // send value to the father
                            freeVariables(localVars);
                            exit(retour);
                        }
                        freeVariables(localVars);
                        if(!myglob(globbuf, tabcmd,index)) exit(0); // check for wildcards
                        execvp(*tabcmd,tabcmd);
                        syserror(2);
                        exit(FAILED_EXEC);
                    } else { // wait for his sons to finish their tasks
                        close(p[1]);
                        wait(&status);
                        if(WIFEXITED(status)){ // print the commmand status
                            if((status=WEXITSTATUS(status)) != FAILED_EXEC || fathercmd){
                                if(status == 47) { // the son wants to create local variable
                                    char infos[BUFFER_SIZE];
                                    read(p[0],infos,sizeof(char) * BUFFER_SIZE); // get informations from the pipe
                                    status = setLocalVariable(infos, localVars);
                                } else if(status == 57) { // the son wants to delete local variable
                                    char infos[BUFFER_SIZE];
                                    read(p[0],infos,sizeof(char) * BUFFER_SIZE); // get informations from the pipe
                                    status = unsetVariable(infos, localVars);
                                }
                                close(p[0]);
                                printf(VERT("exit status of ["));
                                for(ps=tabcmd;*ps;ps++) printf("%s",*ps);
                                printf(VERT("]=%d\033[0m\n"),status);
                                if(or) break;
                            } else if(and) break;
                        } else puts(ROUGE("Anormal exit"));
                        fathercmd = false;
                        using_parameters = false;
                    }
                    if(tabcmd2[out][to++] == NULL) break;
                }
            }
        }
        printDirectory();
    }
    /* detach shared memory segment */  
    shmdt(shm);
    /* remove shared memory segment */  
    shmctl(shmid, IPC_RMID, NULL);
    free(parameters);
    free(directory);
    freeVariables(localVars);
    exit(0);
}