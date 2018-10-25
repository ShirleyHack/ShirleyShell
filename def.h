#ifndef _DEF_H_
#define _DEF_H_
#include <stdio.h>

typedef void (*CmdHandler)();
typedef struct BuiltinCmd
{
    char* name;
    CmdHandler handler;
}BuiltinCmd;

void exit();
void cd();
void type();
void about();
void jobs();
void kill();

const BuiltinCmd builtin_cmd[] =
{
    {"exit", exit},
    {"cd", cd},
    {"type", type},
    {"about", about},
    {"jobs", jobs},
    {"kill", kill}
};
const int kBuiltinSize = 6;
const int kPathBufSize = 1024;
const int kLineSize = 1024;
const int kFileName = 100;
const int kPipeSize = 5;
const int kArgsSize = 20;
const int kCmdSize = 5;
typedef struct Command
{
    char* args[kArgsSize];
    bool append;
    int infd, outfd;
    struct Command* next;
}Command;
void InitCmd(Command* cmd)
{
    cmd->args[0] = NULL;
    cmd->append = false;
    cmd->next = NULL;
    cmd->infd = 0;
    cmd->outfd = 1;
}
#endif