#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<fcntl.h>
#include<stdint.h>
#include<unistd.h>
#include<pwd.h>
#include<dirent.h>
#include<signal.h>
#include<sys/sysmacros.h>
#include<sys/wait.h>

#define MAX_USERS 10

typedef struct coordinates{
    float x,y;
}coordinates;
  
typedef struct treasure{
    char id[8];
    char username[16];
    char clue[64];
    coordinates c;
    int value;
}treasure;

typedef struct score{
    char username[33];
    int score;
}score;

int main(int argc,char **argv)
{
    treasure buffer={0};
    score scores[MAX_USERS]={0};
    int number_of_users=0;
    if(argc!=2)
    {
        fprintf(stderr,"Not enough arguments\n");
        exit(-1);
    }
    int fd;
    char file_relative_path[100];
    sprintf(file_relative_path,"./%s/file.bin",argv[1]);
    if ((fd=open(file_relative_path,O_RDONLY))==-1)
    {
        perror("Open the treasures file\n");
        exit(-1);
    }
    while(read(fd,&buffer,sizeof(buffer)))
    {
        int position=-1;
        for(int i=0;i<number_of_users;i++)
        {
            if(!strcmp(buffer.username,scores[i].username))
            {
                position=i;
            }
        }
        if (position!=-1) scores[position].score+=buffer.value;
        else 
        {
            strcpy(scores[number_of_users].username,buffer.username);
            scores[number_of_users].score=buffer.value;
            number_of_users++;
        }
    }
    printf("%s\n",argv[1]);
    for(int i=0;i<number_of_users;i++)
    {
        printf("%s,%d\n",scores[i].username,scores[i].score);
    }
    return 0;
}