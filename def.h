#ifndef _DEF_H_
#define _DEF_H_
#include <stdio.h>
#include <stdbool.h>

#define kBuiltinSize 6
#define kPathBufSize 1024
#define kLineSize 1024
#define kFileName 100
#define kPipeSize 5
#define kArgsSize 20
#define kCmdSize 5
#define kOpenMax 100
typedef void (*CmdHandler)();
typedef struct BuiltinCmd
{
    char name[kBuiltinSize];
    CmdHandler handler;
}BuiltinCmd;

// typedef struct ProcessNode
// {
//     pid_t pid;
//     struct ProcessNode *next;
// }ProcessNode;

typedef struct Command
{
    char *args[kArgsSize];
    bool append;
    int infd, outfd;
    struct Command* next;
}Command;

// void InitCmd(Command* cmd)
// {
//     cmd->args[0] = NULL;
//     cmd->append = false;
//     cmd->next = NULL;
//     cmd->infd = 0;
//     cmd->outfd = 1;
// }
#endif