//
// Created by Giang Duong on 3/26/20.
//
#ifndef PROJ3_COMMANDNODE_H
#define PROJ3_COMMANDNODE_H
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
typedef struct CommandNode{
    int index;
    int PID;
    char cmd[20][20];
    int startTime;
    int endTime;
    bool active;
    struct CommandNode *nextNodePtr;
}CommandNode;
//create a new command node, nextCmd can be NULL and function InsertCommandAfter can be call to insert
//after the head node
void CreateCommandNode(struct CommandNode* thisNode, char cmd[20][20], int ind,int pid, int start, int end, bool act, CommandNode* nextCmd);
// insert node newNode after thisNode
void InsertCommandAfter(struct CommandNode* thisNode, struct CommandNode* newNode);
// get next command node in the linked list
struct CommandNode* GetNextCommand(CommandNode* thisNode);
// find a command based on the pid
struct CommandNode* FindCommand(CommandNode* cmd,int pid);

#endif //PROJ3_COMMANDNODE_H

