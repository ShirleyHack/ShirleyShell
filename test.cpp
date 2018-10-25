#include <stdio.h>
#include <string.h>
void print(int& tmp)
{
    printf("%d\n", tmp);
    tmp = 6;
    printf("%s\n", "helloworld");
}
int main(void)
{
    int n = 2;
    // int* p = &n;
    print(n);
    print(n);
    return 0;
}
