#include "def.h"
#include "exam.h"
#include "externs.h"

int check(const char* str)
{
    char* p;
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