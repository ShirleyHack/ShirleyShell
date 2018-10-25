#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "def.h"
void SIGINTHandler(int sig)
{
    char pathbuf[kPathBufSize];
    getcwd(pathbuf, sizeof(pathbuf));
    printf("\n[ShirleyShell]:\e[34m%s$ ", pathbuf);
}
void SetUp()
{
    signal(SIGINT, SIGINTHandler);
    signal(SIGQUIT, SIG_IGN);
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
    lastpid = 0;
}
void PrintWelcome()
{
    printf("Welcome to ShirleyShell~\n");
}