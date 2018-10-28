#ifndef _EXTERNS_H_
#define _EXTERNS_H_

#include "def.h"

extern char cmdline[kLineSize];
extern char argsstore[kLineSize];
extern char infile[kCmdSize][kFileName];
extern char outfile[kCmdSize][kFileName];
extern char str[kLineSize];
extern char *lineptr;
extern char *argsptr;
// default both are false
extern bool isbackgnd;
extern int append, parsedcmd;
extern Command cmd[kCmdSize];
extern Command *curcmd, *cmdfree;

// extern ProcessNode *head;
// extern ProcessNode *cur;
// extern void InitCmd(Command* cmd)
// {
//     cmd->args[0] = NULL;
//     cmd->append = false;
//     cmd->next = NULL;
//     cmd->infd = 0;
//     cmd->outfd = 1;
// }
#endif