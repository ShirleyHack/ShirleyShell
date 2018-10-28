#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include "./def.h"
#include "./externs.h"
#include "./init.h"
#include "./exam.h"

void exit();
void cd();
// void type();
void about();
// void jobs();
void kill();
void cmdcpy(Command *dest, Command *src);

const BuiltinCmd builtin_cmd[] =
{
    {"exit", exit},
    {"cd", cd},
    // {"type", type},
    {"about", about},
    // {"jobs", jobs},
    {"kill", kill}
};

extern int check(const char *str)
{
    char *p;
    while(*lineptr == '\t' || *lineptr == ' ')
        lineptr++;

    p = lineptr;
    while(*str != '0' && *str == *p)
    {
        str++;
        p++;
    }
    if(*str == '\0')
    {
        lineptr = p;
        return 1;
    }
    return 0;
}

extern void GetCommand(int i)
{
    // printf("GetCommand: %d\n", i);
    int j = 0, k = 0;
    bool isend = false;
    Command *command = (Command*)malloc(sizeof(Command));
    InitCmd(command);
    curcmd = &cmd[parsedcmd];

    if(curcmd == NULL)
    {
        fprintf(stderr, "Get command error!\n");
        return;
    }
    if(i != 0)
    {
        while(curcmd->next != NULL)
            curcmd = curcmd->next;
        curcmd->next = command;
        curcmd = curcmd->next;
    }

    while(*lineptr != '\0')
    {
        while(*lineptr == ' ' || *lineptr == '\t')
            lineptr++;
        curcmd->args[j] = argsptr;
        while(*lineptr != '\0' && *lineptr != '\t'
            && *lineptr != ' ' && *lineptr != '\n'
            && *lineptr != '<' && *lineptr != '>'
            && *lineptr != '|' && *lineptr != '&'
            && *lineptr != ';')
        {
            *argsptr++ = *lineptr++;
            isend = true;
        }
        *argsptr++ = '\0';
        switch(*lineptr)
        {
            case ' ':
            case '\t':
                isend = false;
                j++;
                break;
            case '<':
            case '>':
            case '|':
            case '&':
            case '\n':
            case ';':
                if(isend == false)
                    curcmd->args[j] = NULL;
            default:
                return;
        }
    }
    printf("insert command:\ncmd[0].args[0]:%s    \n", cmd[0].args[0]);
}

void cmdcpy(Command *dest, Command *src)
{
    int i = 0;
    printf("copy args: \n");
    while(src->args[i] != NULL)
    {
        dest->args[i] = src->args[i];
        printf("%s ", dest->args[i]);
        i++;
    }
    printf("\n");
    dest->append = src->append;
    dest->next = src->next;
    dest->infd = src->infd;
    dest->outfd = src->outfd;
}

int IsBuiltin()
{
    int i;
    for(i = 0; i < kBuiltinSize; i++)
    {
        if(check(builtin_cmd[i].name))
        {
            builtin_cmd[i].handler();
            return 1;
        }
    }
    return 0;
}

void exit()
{
    // int pnum = 0;
    // ProcessNode *tmp = head->next;
    printf("%s\n[pid]:%d\n", "exit", getpid());
    exit(EXIT_SUCCESS);
}

void cd()
{
    int fd;
    GetCommand(parsedcmd);
    fd = open(*(cmd[parsedcmd].args), O_RDONLY);
    fchdir(fd);
    close(fd);
}

void kill()
{
    GetCommand(parsedcmd);
    int num = atoi(cmd[parsedcmd].args[1]);
    signal(SIGQUIT, SIG_DFL);
    if(kill(num, SIGQUIT) < 0)
    {
        fprintf(stderr, "Failed to kill %d\n", num);
        return;
    }
    signal(SIGQUIT, SIG_IGN);
}

void about()
{
    printf("Shell created by Shirley!\n");
}