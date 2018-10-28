#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "./def.h"
#include "./externs.h"
#include "./init.h"

void SIGINTHandler(int sig)
{
    char pathbuf[kPathBufSize];
    getcwd(pathbuf, sizeof(pathbuf));
    printf("\e[1;32m[ShirleyShell]:\e[1;34m%s$ \033[0m", pathbuf);
    fflush(stdout);
}
void SetUp()
{
    signal(SIGINT, SIGINTHandler);
    signal(SIGQUIT, SIGINTHandler);
}
extern void InitCmd(Command* cmd)
{
    int i = 0;
    while(cmd->args[i] != NULL)
    {
        cmd->args[i] = NULL;
        i++;
    }
    cmd->append = false;
    cmd->next = NULL;
    cmd->infd = 0;
    cmd->outfd = 1;
}
void init()
{
    int i, j;
    SIGINTHandler(0);
    fflush(stdout);
    memset(cmdline, 0, sizeof(cmdline));
    memset(argsstore, 0, sizeof(argsstore));
    memset(infile, 0, sizeof(infile));
    memset(outfile, 0, sizeof(outfile));
    lineptr = cmdline;
    argsptr = argsstore;
    append = -1;
    isbackgnd = false;
    parsedcmd = 0;
    for(i = 0; i < kCmdSize; i++)
    {
        InitCmd(&cmd[i]);
    }
    curcmd = NULL;
    cmdfree = NULL;
}
void PrintWelcome()
{
    printf("\nWelcome to ShirleyShell~\n");
}
