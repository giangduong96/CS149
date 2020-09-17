#include "CommandNode.h"
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <fcntl.h>
#include <zconf.h>
int main(int argc, const char * argv[]) {
    char *line = NULL; // used to return value in size bytes.
    size_t leng = 0;   // used ssize_t because the return value in size byte or negative error value
    ssize_t read;       // read file
    char newString[20][20]; // used to store command line
    CommandNode *nodePtr; // to move through the list
    char *buf2[10]; // unused but need to exec
    char *buf1[10]; // unused but need to exec
    char fileNameOut[20]; //store file name ouput
    char fileNameErr[20];   // store file name for error
    int i, j, ctr;  // i for index, j for line , ctr next word
    i = 1; // first index
    CommandNode *headNode; // the headPTR
    CommandNode *tmpNode; // the tempPTR//
    tmpNode = (CommandNode *) malloc(sizeof(CommandNode));  // allocate memory
    headNode = (CommandNode *) malloc(sizeof(CommandNode)); // allocated memory
    headNode =NULL;
    FILE *filePtr; // file directory used to open text file
    filePtr = fopen(argv[1], "r"); //if the file pointer points to wrong or nothing exit with error code 1
    if (filePtr == NULL) {
        printf("Error: File pointer is Null from file %s\n", argv[1]);
        return 1;
    }// otherwise say
    else
        printf("Open is success files %s! \n",argv[1]); // scan each line in the file into the function and compute the average
    while ((read = getline(&line, &leng, filePtr)) != -1) {
        for (int i = 0; i < 20; i++) {
            for (int j = 0; j < 20; j++) {
                newString[i][j] = '\0';
            }
        }
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        j = 0;
        ctr = 0;
        for (int i = 0; i < strlen(line); i++) {
            // if space or NULL found, assign NULL into newString[ctr]
            if (line[i] == ' ' || line[i] == '\0') {
                newString[ctr][j] = '\0';
                ctr++;  // for next word
                j = 0;   // for next word, init index to 0
            } else {
                newString[ctr][j] = line[i];
                j++;
            }
        }
        printf("--%s,\n", newString[1]); // test the command is read correctly
        printf("--%s,\n",newString[0]);  // test the command is read correctly
        i++;
        buf2[i] = newString[1]; // need to exec
        buf1[i] = newString[0]; // need to exec
        printf("%s, %s\n", buf1[i], buf2[i]);
        CommandNode *nextCommand; // create next Node for next Command
        nextCommand = (CommandNode*) malloc(sizeof(CommandNode));   // allocated memonry for next Node
        CreateCommandNode(nextCommand,newString, i, 0, 0, 0, 0, NULL); //create the nextnode
        //if there is no nodes in the list then make newNode the first node

        if (!headNode)
            headNode = nextCommand;
        else //otherwise, insert newNode at the end
            nodePtr = headNode; //initialize nodePtr to head of list
        while (nodePtr->nextNodePtr) { // find the last node in the list
            nodePtr = nodePtr->nextNodePtr;
        }
        nodePtr->nextNodePtr = nextCommand; // insert newCommand node as the last cmd
    }
    printf("Run! Pass firsr loop\n");
    //  if the file cannot be close, exit with the code 1
    if (fclose(filePtr) != 0) {
        printf("Closing file error!\n");
        exit(1);
    }

    tmpNode = headNode; // re-assign headptr to temp node
    pid_t p; // process ID
    int status; // status for the node
    struct timespec start, finish;
    double elapsed;
    while (tmpNode != NULL) {
        if(tmpNode->index==1){ //skip headNode
            tmpNode=tmpNode->nextNodePtr;}
        tmpNode->startTime = clock_gettime(CLOCK_MONOTONIC, &start);
        printf("start %ld\n", start.tv_sec); // print the start-time
        if ((p = fork()) < 0) {
            fprintf(stderr, "fork failed\n");
            exit(1);
        } else if (p == 0) { // child process
            char *bufIn[4];
            bufIn[0]= tmpNode->cmd[0]; // path
            bufIn[1]= tmpNode->cmd[1];  // [] for exec, specific for each node
            //printf("%s, %s, index :%d\n", tmpNode->cmd[0],tmpNode->cmd[1], tmpNode->index);
            if((execvp(bufIn[0],bufIn)) <0) {
                printf("*** ERROR: exec failed with command :%d\n", tmpNode->index -1);}
            exit(0);
        }
        else if(p >0){
            tmpNode->PID = p; // get the pid
            //use dup2 to redirect the file, also use to write the file
            sprintf(fileNameOut, "%d.out", tmpNode->index);
            if ((fileNameOut[tmpNode->index]= open(fileNameOut, O_CREAT | O_RDWR | O_APPEND,0664)) < 0) {
                printf("%s fail", fileNameOut);exit(1); // check error
            }
            dup2(fileNameOut[tmpNode->index], 1); // redirect to the output file start from here
            printf("Starting command %d: child %d pid of parent %d.\n ", tmpNode->index, getpid(), getppid());
            close(fileNameOut[tmpNode->index]);// close file
            tmpNode = tmpNode->nextNodePtr; //move to the next command
        }
    }
    // Final loop waits unitl anything has completed this will exit with wait return -1 when there is no more child
    //process. Then parent exit
        while ((p = wait(&status)) >= 0) {
            if (p > 0) {
                tmpNode = FindCommand(headNode, p); // find the command by its PID
                sprintf(fileNameOut, "%d.out", tmpNode->index); // write to file out put
                if ((fileNameOut[tmpNode->index] = open(fileNameOut, O_CREAT | O_RDWR | O_APPEND, 0664)) < 0) {
                    printf("%s fail", fileNameOut);
                    exit(1); // check if fail
                }
                dup2(fileNameOut[tmpNode->index], 1); // redirect to the output file start from here
                clock_gettime(CLOCK_MONOTONIC, &finish);
                printf("Finish at %ld, ", finish.tv_sec);
                elapsed = (finish.tv_sec - tmpNode->startTime);
                printf("Command took %f seconds to execute \n", elapsed);
                close(fileNameOut[tmpNode->index]); // close file

                sprintf(fileNameErr, "%d-err.out", tmpNode->index); // write to error file
                if ((fileNameErr[tmpNode->index] = open(fileNameErr, O_CREAT | O_RDWR | O_APPEND, 0664)) < 0) {
                    printf("%s fail", fileNameErr);
                    exit(1);
                }
                dup2(fileNameErr[tmpNode->index], 1); // redirect to the output file start from here
                printf("Process with PID %d terminated.\n", p);
                if (WIFEXITED(status)) {
                    printf("Child %d terminated normally with exit code: %d\n",
                           p, WEXITSTATUS(status));
                } else if (WIFSIGNALED(status)) {
                    printf("Child %d terminated abnormally with signal number: %d\n",
                           p, WTERMSIG(status));
                }
                close(fileNameErr[tmpNode->index]); // close write to file

                // if elapsed time is greater than 2 then start a new process
                if (elapsed > 2) {
                    //record a new start time
                    tmpNode->startTime = clock_gettime(CLOCK_MONOTONIC, &start);
                    if ((p = fork()) < 0) {
                        fprintf(stderr, "fork failed..\n");
                        exit(1);
                    } else if (p == 0) {
                        if (tmpNode->index == 1) { //skip head
                            tmpNode = tmpNode->nextNodePtr;
                        }
                        sprintf(fileNameOut, "%d.out", tmpNode->index); // write to file output
                        if ((fileNameOut[tmpNode->index] = open(fileNameOut, O_CREAT | O_RDWR | O_APPEND, 0664)) < 0) {
                            printf("%s fail", fileNameOut);
                            exit(1);
                        }
                        dup2(fileNameOut[tmpNode->index], 1); // redirect to the output file start from here
                        printf("Starting command %d: child %d pid of parent *time2 %d.\n ", tmpNode->index, getpid(),
                               getppid());
                        char *bufIn[4];
                        bufIn[0] = tmpNode->cmd[0];
                        bufIn[1] = tmpNode->cmd[1];
                        //printf("%s, %s, index :%d\n", tmpNode->cmd[0],tmpNode->cmd[1], tmpNode->index);
                        if ((execvp(bufIn[0], bufIn)) < 0) {
                            printf("*** ERROR: exec failed with command %d\n", tmpNode->index - 1);
                        }
                        close(fileNameOut[tmpNode->index]); // close write file
                        tmpNode = tmpNode->nextNodePtr;
                        exit(0);
                    } else if (p > 0) {
                        tmpNode->PID = p;
                        sprintf(fileNameErr, "%d-err.out", tmpNode->index); // write file error
                        if ((fileNameErr[tmpNode->index] = open(fileNameErr, O_CREAT | O_RDWR | O_APPEND, 0664)) < 0) {
                            printf("%s fail", fileNameErr);
                            exit(1); // check if there is error
                        }
                        dup2(fileNameErr[tmpNode->index], 1); // redirect to the output file start from here
                        printf("spawning too fast. \n");
                        close(fileNameErr[tmpNode->index]);
                        tmpNode->PID = p;
                        wait(NULL);//wait for the file exit1
                    }
                }
            }
        }
    if (line) free(line);
    exit(EXIT_SUCCESS);
}
