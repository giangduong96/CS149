//
// Created by Giang Duong on 4/09/20.
//
//  Giang Duong
//  Assignment 4
//
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
#include <stdbool.h>
typedef struct linkedList_Node_STRUCT{
    int index; // holds the index of the file
    size_t size; // used to store the size of the line
    char *commandLine;  // stores the command from file
    struct linkedList_Node_STRUCT *nextNodePtr; // point to next node
}linkedList_Node_STRUCT; /* end of struct*/

typedef struct linkedList_Node_STRUCT linkedList_Node; // make the default
static linkedList_Node* head_OF_STACK = NULL; // ptr to the top of stack
/*
 * This function will push a line node (cmd) to Stack LinkedList
 */
void pushOnStack(size_t length,char** cmd){
    linkedList_Node *tmpNode; // temp Node
    static  char global[]="global"; // head Node in global area
    // first head node
    if (head_OF_STACK==NULL){
        head_OF_STACK = (linkedList_Node*) malloc(sizeof(linkedList_Node));
        //no recovery needed if allocation failed, this is only used in debugging, not in prod
        if (head_OF_STACK==NULL){
            printf("HeadNode memory allocation error\n"); exit(1);
        }
        head_OF_STACK->commandLine= global; // set global
        head_OF_STACK->nextNodePtr= NULL; // set the nextPtr of head == NULL
        head_OF_STACK->size= 7;
        head_OF_STACK->index =-1;       // set the index of global
        head_OF_STACK->index++;         // index +1
        tmpNode =(linkedList_Node*)malloc(sizeof(linkedList_Node));
        if (tmpNode == NULL) {
            printf("Push_On_Stack allocation error.\n");
            exit(1);
        }
        char *p;
        p= malloc((sizeof(char) * length) + 1); // +1 for \0
        if (p == NULL) {
            printf("Fail to allocated string.\n");
            exit(1);
        }
        strcpy(p,*cmd);
        tmpNode->commandLine= p; // copy the address of the cmd
        tmpNode->nextNodePtr= head_OF_STACK; // insert tmp as the first node in the list
        tmpNode->index=head_OF_STACK->index;
        tmpNode->size= length;
        head_OF_STACK = tmpNode; // set tmpNode as the head_of_stack
        head_OF_STACK->index++;

    } else { // the rest of list
        tmpNode =(linkedList_Node*)malloc(sizeof(linkedList_Node));
        if (tmpNode == NULL) {
            printf("Push_On_Stack allocation error.\n");
            exit(1);
        }
        char *p;
        p= malloc(length); // +1 for \0
        if (p == NULL) {
            printf("Fail to allocated string.\n");
            exit(1);
        }
        strcpy(p,*cmd);
        tmpNode->commandLine= p; // copy the address of the cmd
        tmpNode->nextNodePtr= head_OF_STACK; // insert tmp as the first node in the list
        tmpNode->index=head_OF_STACK->index;
        tmpNode->size = length;
        head_OF_STACK = tmpNode; // set tmpNode as the head_of_stack
        head_OF_STACK->index++;
    }
}// end pushOnStack

/*
 * This function is used to remove the top of Stack
 */
void pop_On_Stack(){
        linkedList_Node *tmpNode;
        tmpNode = head_OF_STACK;
        head_OF_STACK = head_OF_STACK->nextNodePtr;
        free(tmpNode);
} // end pop_On_Stack

/*
 * print_List prints the sequences of cmd that are on the stack at this instance
 * it returns a string that looks like: funcA:funcB:funcC.
 */
char* print_List(){
    int depth = 200; // a max of 50 levels in the stack will be combined in a string
    int i, length, j;
    linkedList_Node* tmpNode;
    static char buf[200];

    if (head_OF_STACK==NULL){ //stack not initialized yet so we are
        strcpy(buf,"global");  // still in the "global" area
        return buf;
    }
    //peek at the depth top entries on the stack, but do nt go over 100 chars and do not go
    //over the bottom of the stack
    sprintf(buf, "%s", head_OF_STACK->commandLine);
    length =strlen(buf); // length of the string so far
    for(i = 0, tmpNode= head_OF_STACK->nextNodePtr; tmpNode!=NULL && i< depth;i++,tmpNode=tmpNode->nextNodePtr){
        j = strlen(tmpNode->commandLine);   // length of string so far
        if(length+j+1 < 200){       // check for total length
            sprintf(buf+length, ":%s",tmpNode->commandLine);
            length+=j+1;
        }else break;    // break if its too long
    }
    return buf;
}

/*
 * this function will calls realloc
 */
void *REALLOC(void* p, size_t t, char* file, int line){
    if(t ==0){
        printf("Error malloc size = 0\n"); exit(1);
    } // check t < 0
    p= realloc(p,t); // reallocate the memory
    printf("File %s,line %d,function=%s reallocated the memory segment at address %p to a new size %zu.\n",
           file,line,print_List(), (void*)p,t);
    return p;
}
/*
 * function MALLOC calls malloc
 */
void* MALLOC(size_t t,char* file,int line)
{
    void* p;
    if(t ==0){
        printf("Error malloc size = 0\n"); exit(1);
    } // check the size > 0
    p = malloc(t); // allocated memory
    printf("File:%s,line %d,function:%s allocated the memory segment at address %p to a size %zu.\n",
           file,line,print_List(), (void*)p,t);

    return p;
}
/*
 * FREE uses to print infor aboyt memory used.
 */
void FREE(void* p,char* file,int line)
{
    printf("File: %s,line %d,function:%s deallocated the memory segment at address %p.\n",
           file,line,print_List(), (void*)p);
    free(p); // free func
}
#define realloc(a,b) REALLOC(a,b,__FILE__,__LINE__)
#define malloc(a) MALLOC(a,__FILE__,__LINE__)
#define free(a) FREE(a,__FILE__,__LINE__)
/*
 * this function add column will add an extra column to a 2d array of char
 */
void add_colum(int **array, int rows, int columns){
    char *a = "add_column";
    pushOnStack(0,&a);
    int i;
    for (i = 0; i < rows; i++){
        array[i]=(int*) REALLOC(array[i],sizeof(int)*(columns+1),__FILE__,__LINE__);
        array[i][columns]= 10*i+columns;
    }
    pop_On_Stack();
    return;
} // end add_column


/*
 *
 */
void make_extend_array(){
    char* a1="make_extend_array"; // make extend array function
    pushOnStack(0,&a1); // push it on stack
    int i, j;
    int **array; // array file
    int sizeInt=4;
    //make arrray
    array= (int**) MALLOC(sizeof(char*)*sizeInt,__FILE__,__LINE__); // 4 bytes
    for(i = 0;i<4;i++){
        array[i]= (int*)MALLOC(sizeof(int)*3,__FILE__,__LINE__); //   // 3 columns
        for(j =0; j<3; j++){
            array[i][j]=10*i+j;
        }
    }
    //display array
    for(i =0; i <4; i++){
        for(j = 0; j<3; j++){
            printf("array[%d][%d]=%d\n",i,j,array[i][j]);
        }
    }
    //add new column
    add_colum(array,4,3);
    // now display the array again
    for(i=0; i<4; i++){
        for(j=0; j<4; j++){
            printf("array[%d][%d]=%d\n",i,j,array[i][j]);}}

    //now deallocate it
    for(i=0; i<4; i++)
        free((void*)array[i]);
    free((void*)array);
    pop_On_Stack();
    return;
}//end make_extend_array

/*
 * this function will allocate memory for a string of length which return 0 if fail, 1 if succeed
*/
int allocateString(int length, char **retPtr){
    char* p = MALLOC(length+1,__FILE__,__LINE__); // +1 for \0
    if (p == NULL) {
        printf("Fail to allocated string.\n");
        return 0;
    }
    *retPtr = p;
    //pushOnStack(*retPtr);
    return 1;
}

int main(int argc, const char * argv[]) {
    // save process to file output by using dup2()
    char fileOut[20]; // name of the file output
    sprintf(fileOut, "%s.out", "memtrace");
    if((fileOut[0] = open(fileOut, O_CREAT | O_RDWR | O_APPEND, 0664)) < 0) {
        printf("%s fail", fileOut);
        exit(1); // check if there is error
    }
    dup2(fileOut[0], 1);
    char *line = NULL; // used to return value in size bytes.
    size_t leng = 0;   // used ssize_t because the return value in size byte or negative error value
    ssize_t read;       // read file
    linkedList_Node* tmpNode = NULL;
    int maxSize = 5;  // initial size
    char** command; // array char pointer to each line is read in file
    command= (char**)malloc(sizeof(char*)*maxSize);
    int i, j, k;
    i = 0; // use to hold the number of the line
    FILE *filePtr; // file directory used to open text file
    filePtr = fopen("cmdfile.txt", "r"); //if the file pointer points to wrong or nothing exit with error code 1
    if (filePtr == NULL) {
        printf("Error: File pointer is Null from file %s\n", argv[1]);
        return 1;
    }// otherwise say
    else
        printf("Open is success files %s! \n",argv[1]); // scan each line in the file into the function and compute the average
    while ((read = getline(&line, &leng, filePtr)) != -1) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        pushOnStack(read,&line); // push the line to the stack node
        //command array
        // if the intial size  <  number of real line in the file, then reallocates it
        if(i > maxSize) {
            printf("Initial size is 5 rows now add more rows by reallocated to the maxSize =%d!\n",i);
            // reallocate the number
            command=realloc(command, sizeof(char*)*(i + 1));
        } //printf("%zd\n",read);
        command[i] = malloc(sizeof(char)*(read + 1)); // save each line
        strcpy(command[i],line); // copy to the command
        i++; // line ++
    }
    printf("%s*\n", print_List()); // try to print what have got
    //printf("Run! Pass!%i\n",i);
    printf("\nList of command from LINKED LIST: \n");
    tmpNode=head_OF_STACK;
    while(tmpNode!=NULL){
        printf("Command %d:-%s.\n",tmpNode->index,tmpNode->commandLine);
        tmpNode=tmpNode->nextNodePtr ;
    }
    //function in main
    char *main="main";
    pushOnStack(0,&main); //push on stack
    char* a1="make_extend_array"; // function make extend array;
    pushOnStack(0,&a1); // push on stack
    make_extend_array();
/*
    char *a = "add_column +'A' ";
    pushOnStack(0,&a);
    tmpNode = head_OF_STACK;
    while (tmpNode!=NULL){
        command[tmpNode->index]= realloc(command[tmpNode->index],tmpNode->size+i*i);
        command[tmpNode->index][tmpNode->size]= 'A';
        tmpNode=tmpNode->nextNodePtr;
    }

    char *b = "add_column +'B' ";
    pushOnStack(0,&b);
    tmpNode = head_OF_STACK;
    for (j = 0; j < i; j++){
        command[j]=realloc(command[j],sizeof(char)*tmpNode->size+1);
        command[j][tmpNode->size+2]= 'B';
    }
    */

    printf("\nList of command from **command array: \n");
    tmpNode=head_OF_STACK;
    while(tmpNode!=NULL){
        printf("Command %d:-%s.\n",tmpNode->index,tmpNode->commandLine);
        tmpNode=tmpNode->nextNodePtr ;
    }
    printf("Now print the array command[]\n");
    int maxLength=100; // max length of the line
    for(int j = 0; j< i; j++){
        printf("command[%d]:",j);
        for(int k = 0; k < maxLength; k++){
            if(command[j][k]!='\0'){
            printf("%c",command[j][k]);}
            else break;
        }
        printf("\n");
    }

    //now deallocate the rows
    for(int k=0; k<i; k++) {
        free(command[k]);
    }
    free(command);

//  if the file cannot be close, exit with the code 1
    if (fclose(filePtr) != 0) {
        printf("Closing file error!\n");
        exit(1);
    }
    tmpNode=head_OF_STACK->nextNodePtr;
    while(tmpNode->nextNodePtr!=NULL){
        {
            linkedList_Node *tmp = tmpNode;
            free(tmp->commandLine);
            free(tmp);
        }
        tmpNode=tmpNode->nextNodePtr;
    }
    printf("DONE with LinkedList MEMORY deallocation1.\n");
    pop_On_Stack(); // deallocate all the memory.
    free(line);
    free(tmpNode);
    printf("DONE with LinkedList MEMORY deallocation.\n");
    close(fileOut[0]);
    exit(0);
}
