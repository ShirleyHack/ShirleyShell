#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>
#include <sys/wait.h>
#include <string.h>
#include "./def.h"
#include "./externs.h"
#include "./init.h"
#include "./exam.h"
#include "./execute.h"

void ForkExec(Command* cmd);
int FreeCmd(Command* cmd, int index);
void GetName(char *filename)
{
    while(*lineptr == ' ' || *lineptr == '\t')
        lineptr++;
    
    while(*lineptr != '\0' && *lineptr != '\n'
        && *lineptr != ' '  && *lineptr != '\t'
        && *lineptr != '<'  && *lineptr != '>'
        && *lineptr != '$'  && *lineptr != '|')
    {
        *filename++ = *lineptr++;
    }
    *filename = '\0';
}

void Loop()
{
    while(true)
    {
        init();
        if(ReadCommand() == -1)
            break;
        if(ParseCommand() == -1)
            printf("Failed to parse command.\n");
        if(ExecuteCommand() == -1)
            printf("Failed to execute command.\n");
    }
    printf("\nexit[ShirleyShell]\n");
}
int ReadCommand()
{
    if(fgets(cmdline, kLineSize, stdin) == NULL)
        return -1;
    return 0;
}
int ParseCommand()
{
    strcpy(str, "\n");
    // 无命令
    if(check(str))
        return 0;
    // 判断是否内部命令
    if(IsBuiltin())
        return 0;
    GetCommand(0);
    // 判断是否有输入重定向
    strcpy(str, "<");
    if(check(str))
    {
        if(check(str))
        {
            /*wait for supplementation*/
        }
        else
            GetName(infile[parsedcmd]);
    }
    // 判断是否有管道
    int i;
    for(i = 1; i < kPipeSize; i++)
    {
        strcpy(str, "|");
        if(check(str))
        {
            if(check(str))
            {
                /*wait for supplementation*/
            }
            else
                GetCommand(i);
        }
        else
            break;
    }

    // 判断是否有输出重定向
    strcpy(str, ">");
    if(check(str))
    {
        if(check(str))
            cmd[parsedcmd].append = true;
        GetName(outfile[parsedcmd]);
    }
    // 判断是否后台作业--对应着最后一条command
    strcpy(str, "&");
    if(check(str))
    {
        strcpy(str, "\n");
        if(check(str))
        {
            parsedcmd++;
            isbackgnd = true;
            return parsedcmd;
        }
        else
        {
            strcpy(str, "&");
            if(check(str))
            {
                parsedcmd++;
                ParseCommand();
            }
            else
                fprintf(stderr, "Syntax error: &\n");
        }
    }
    // 判断命令结束
    strcpy(str, ";");
    if(check(str))
    {
        parsedcmd++;
        ParseCommand();
    }
    strcpy(str, "\n");
    if(check(str))
    {
        parsedcmd++;
        return parsedcmd;
    }
    return 0;
}

int ExecuteCommand()
{
    int numcmd = 0;
    while(numcmd < parsedcmd)
    {
        // 没有命令
        curcmd = &cmd[numcmd];
        // printf("first:%d\n", curcmd->args[0] == NULL);
        if(curcmd->args[0] == NULL)
        {
            numcmd++;
            continue;
        }
        if(infile[numcmd][0] != '\0')
            curcmd->infd = open(infile[numcmd], O_RDONLY);

        if(outfile[numcmd][0] != '\0')
        {
            if(curcmd->append)
                curcmd->outfd = open(outfile[numcmd], O_WRONLY | O_CREAT
                    | O_APPEND, 0666);
            else
                curcmd->outfd = open(outfile[numcmd], O_WRONLY | O_CREAT
                    | O_TRUNC, 0666);
        }

        if(numcmd == parsedcmd - 1 && isbackgnd)
            signal(SIGCHLD, SIG_IGN);
        else
            signal(SIGCHLD, SIG_DFL);

        int fd;
        int fds[2];
        while(curcmd != NULL && curcmd->args[0] != NULL)
        {
            if(curcmd->next != NULL)
            {
                pipe(fds);
                curcmd->outfd = fds[1];
                // !curcmd->next
                curcmd->next->infd = fds[0];
            }

            ForkExec(curcmd);
            // printf("curcmd->infd: %d\ncurcmd->outfd", curcmd->infd, curcmd->outfd);
            if((fd = curcmd->infd) != 0)
                close(fd);

            if((fd = curcmd->outfd) != 1)
                close(fd);
            curcmd = curcmd->next;
        }
        // curcmd = &cmd[numcmd];
        if(FreeCmd(cmd[numcmd].next, 0) < 0)
        {
            printf("Failed to free Command.\n");
        }
        numcmd++;
    }
    return 0;
}

int FreeCmd(Command* cmd, int index)
{
    if(cmd == NULL)
    {
        if(index == 0)
            return 0;
        return -1;
    }
    if(cmd->next != NULL)
        FreeCmd(cmd->next, index+1);
    free(cmd);
    return 0;
}

void ForkExec(Command* cmd)
{
    int status = 1;
    pid_t pid;

    int k = 0;
    while(cmd->args[k] != NULL)
    {
        printf("args[%d]:%s  ", k, cmd->args[k]);
        k++;
    }
    printf("\n");   
 
    pid = fork();
    // printf("fork command:\ncmd[0].args[0]:%s    \n", cmd[0].args[0]);
    // printf("pid_origin: %lu\n", pid);
    // printf("getpid_origin: %lu\n", getpid());

    if(pid < 0)
        fprintf(stderr, "Fork failed\n");
    else if(pid == 0)
    {
        // if(cmd->infd == 0 && isbackgnd)
        //     cmd->infd = open("/dev/null", O_RDONLY);

        // printf("child: %lu\n", getpid());
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGCONT, SIG_DFL);
        if(cmd->infd != 0)
        {
            close(0);
            dup(cmd->infd);
        }
        // !!!Wrong condiction lead to wrongly closing!!!
        if(cmd->outfd != 1)
        {
            close(1);
            dup(cmd->outfd);
        }
        // printf("child continue1\n");

        int j;
        for(j = 3; j < kOpenMax; j++)
            close(j);

        // if(!isbackgnd)
        // {
        //     printf("isbackgnd: %d\n", isbackgnd);
        //     signal(SIGINT, SIG_DFL);
        //     signal(SIGQUIT, SIG_DFL);
        // }

        // printf("child continue2\n");

        // printf("[%d]end\n", k);
        // printf("child continue3\n");
        if(execvp(cmd->args[0], cmd->args) < 0)
            fprintf(stderr, " *** ERROR: exec %s failed\n", cmd->args[0]);
        // printf("child continue4\n");
        exit(EXIT_SUCCESS);
    }
    // 父进程
    else
    {
        // printf("parent: %lu\n", getpid());
        // printf("isback: %d, cur_pid: %lu\n", isbackgnd, pid);        
        signal(SIGINT, SIG_IGN);
        signal(SIGTSTP, SIG_IGN);
        signal(SIGCONT, SIG_DFL);
        if(isbackgnd)
        {
            signal(SIGCHLD, SIG_IGN);
            // ProcessNode *p = (ProcessNode*)malloc(sizeof(ProcessNode));
            // p->pid = pid;
            // ProcessNode *tmp = head->next;
            // head->next = p;
            // p->next = tmp;
        }
        else
            waitpid(pid, &status, WUNTRACED);
    }
}