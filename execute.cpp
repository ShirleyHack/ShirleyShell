#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include "def.h"
#include "exam.h"
#include "execute.h"
#include "externs.h"
#include "init.h"

void GetName(char* filename)
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

void GetCommand(int i)
{
    int j = 0, k = 0;
    bool isend = false;
    Command* command = (Command*)malloc(sizeof(Command));
    InitCmd(command);
    curcmd = &cmd[parsedcmd];
    for(; k < i; k++)
    {
        if(curcmd == NULL)
        {
            fprintf(stderr, "Getcommand error!");
            return;
        }
        if(curcmd->next != NULL)
            curcmd = curcmd->next;
        else
        {
            curcmd->next = command;
            break;
        }
    }
    while(*lineptr != '\0')
    {
        while(*lineptr == ' ' || *lineptr == '\t')
            lineptr++;
        curcmd->args[j] = argsptr;
        while(*lineptr != '\0' && *lineptr != '\t'
            && *lineptr != ' ' && *lineptr != '\n'
            && *lineptr != '<' && *lineptr != '>'
            && *lineptr != '|' && *lineptr != '&')
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
                if(isend == true)
                    curcmd->args[j] = NULL;
            default:
                return;
        }
    }
}

void Loop()
{
    while(true)
    {
        init();
        if(ReadCommand() == -1)
        {
            printf("Failed to read command\n.");
            break;
        }
        if(ParseCommand() == -1)
        {
            printf("Failed to parse command\n.");
            break;
        }
        if(ExecuteCommand() == -1)
        {
            printf("Failed to execute command\n.");
            break;
        }
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
    // 无命令
    if(check("\n"))
        return 0;
    // 判断是否内部命令
    if(IsBuiltin())
        return 0;
    GetCommand(0);
    // 判断是否有输入重定向
    if(check("<"))
    {
        if(check("<"))
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
        if(check("|"))
        {
            if(check("|"))
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
    if(check(">"))
    {
        if(check(">"))
            cmd[parsedcmd].append = true;
        GetName(outfile[parsedcmd]);
    }
    // 判断是否后台作业--对应着最后一条command
    if(check("&"))
    {
        if(check("\n"))
        {
            parsedcmd++;
            isbackgnd = true;
            return parsedcmd;
        }
        else
        {
            fprintf(strerr, "Syntax error: &");
        }
    }
    // 判断命令结束
    if(check("\n"))
    {

    }
    else
    {
        if(check(";") || check("&&"))
        {
            parsedcmd++;
            ParseCommand();
        }
        // else if(check("&&"))
        //     ParseCommand();
        // else if(check("||"))
        //     ParseCommand("||");
        else
        {
            fprintf(stderr, "Command line syntax error\n");
            return -1;
        }
    }
}

int ExecuteCommand()
{
    int numcmd = 0;
    while(numcmd < parsedcmd)
    {
        // 没有命令
        curcmd = &cmd[numcmd];
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
                curcmd->outfd = open(outfile[numcmd], OWRONLY | O_CREAT
                    | O_AAPPEND, 0666);
            else
                curcmd->outfd = open(outfile[numcmd], O_WRONLY | O_CREAT
                    | O_TRUNC, 0666);
        }

        if(numcmd == parsedcmd - 1 && isbackgnd)
            signal(SIGCHLD, SIG_IGN);
        else
            signal(SIGCHLD, SIG_DFL);

        int fds[2];
        while(curcmd->args[0] != NULL)
        {
            if(curcmd->next != NULL)
            {
                pipe(fds);
                curcmd->infd = fds[0];
                curcmd->outfd = fds[1];
            }

            forkexec(curcmd);

            if(curcmd->infd != 0)
                close(curcmd);

            if(curcmd->outfd != 1)
                close(outfd);
        }

        if(isbackgnd)
        {
            while(wait(NULL) != lastpid)
            {}
        }
        curcmd = curcmd->next;
    }
    return 0;
}

void forkexec(Command* cmd)
{
    int status = 1;
    pid_t pid;
    pid = fork();
    if(pid < 0)
        fprintf(strerr, "Fork failed\n");

    else if(pid == 0)
    {
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGCONT, SIG_DFL);
        dup2(cmd->infd, 0);
        dup2(cmd->outfd, 1);
        if(cmd->infd != 0)
            close(cmd->infd);
        if(cmd->outfd != 0)
            close(cmd->outfd);
        if(execvp(cmd->args[0], cmd->args) < 0)
            fprintf(strerr, " *** ERROR: exec %s failed\n", segment->args[0]);
    }
    // 父进程
    else
    {
        signal(SIGINT, SIG_IGN);
        signal(SIGTSTP, SIG_IGN);
        signal(SIGCONT, SIG_DFL);
        if(isbackgnd)
            signal(SIGCHLD, SIG_IGN);
        else
            waitpid(pid, &status, WUNTRACED);
        if(cmd->infd != 0){
            close(cmd->infd);
        }
        if(cmd->outfd != 1){
            close(cmd->outfd);
        }
    }
    return 1;

}