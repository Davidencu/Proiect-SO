#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
int main()
{
    char s[100];
    fgets(s,100,stdin);
    printf("%s",s);
    return 0;
}