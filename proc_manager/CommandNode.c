//
// Created by Giang Duong on 3/26/20.
//
#include "CommandNode.h"
#include<stdlib.h>

void CreateCommandNode(struct CommandNode* thisNode, char cmd[20][20],
        int ind,int pid, int start, int end, bool act,
        struct CommandNode* nextCmd){
    for(int i = 0 ; i < 20 ; i++) {
        for (int j = 0; j < 20; j++) {
            thisNode->cmd[i][j] = cmd[i][j];
        }
    }
    thisNode->index= ind;
    thisNode->PID = pid;
    thisNode->startTime= start;
    thisNode->endTime=end;
    thisNode->active=0;
    thisNode->nextNodePtr= nextCmd;
    return;
}
void init(CommandNode** head)
{
    *head = NULL;
}
void printList(struct CommandNode* thisNode){
    while (thisNode!=NULL){
        printf("%d", thisNode->PID);
        thisNode= thisNode->nextNodePtr;
    }
}

void InsertCommandAfter(CommandNode* thisNode, CommandNode* newNode) {
    if(thisNode == NULL){
        printf("This node cannot be NULL");
    }
    CommandNode *tmpNext = NULL;
    tmpNext = (struct CommandNode*) malloc((sizeof(CommandNode)));
    tmpNext = thisNode->nextNodePtr;
    thisNode->nextNodePtr = newNode;
    newNode->nextNodePtr = tmpNext;
    return;
}
CommandNode* GetNextCommand(CommandNode* thisNode){
    return thisNode->nextNodePtr;
}

CommandNode* FindCommand(CommandNode* cmd, int pid){
    CommandNode *temp = cmd;
    while(temp !=NULL){
        if(temp->PID == pid)
        { return temp;}
        temp = temp->nextNodePtr;
    }
    return NULL;
}