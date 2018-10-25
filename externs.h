#ifndef _EXTERNS_H_
#define _EXTERNS_H_

#include "def.h"

extern char cmdline[kLineSize];
extern char argsstore[kLineSize];
extern char infile[kCmdSize][kFileName];
extern char outfile[kCmdSize][kFileName];
extern char* lineptr;
extern char* argsptr;
// default both are false
extern bool isbackgnd;
extern int append, parsedcmd, lastpid;
extern Command cmd[kCmdSize];
extern Command* curcmd;
extern void InitCmd(Command& cmd);
#endif