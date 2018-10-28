#include <stdlib.h>
#include "./def.h"
#include "./externs.h"
#include "./init.h"
#include "./exam.h"
#include "./execute.h"

char cmdline[kLineSize];
char argsstore[kLineSize];
char infile[kCmdSize][kFileName];
char outfile[kCmdSize][kFileName];
char str[kLineSize];
char *lineptr;
char *argsptr;
// default both are false
bool isbackgnd;
int append, parsedcmd;
Command cmd[kCmdSize];
Command *curcmd, *cmdfree;

int main()
{
    SetUp();
    PrintWelcome();
    Loop();
    return 0;
}