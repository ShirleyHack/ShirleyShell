#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "def.h"
void sigint_handler(int sig)
{
    char pathbuf[PATH_BUFSIZE];
    getcwd(pathbuf, sizeof(pathbuf));
    printf("\n[ShirleyShell]:\e[34m%s$", pathbuf);
}
void SetUp()
{
    signal(SIGINT, sigint_handler);
    signal(SIGQUIT, SIG_IGN);
}
void PrintWelcome()
{
    printf("")
}