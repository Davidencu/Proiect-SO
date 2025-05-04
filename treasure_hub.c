#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<fcntl.h>
#include<stdint.h>
#include<unistd.h>
#include<time.h>
#include<pwd.h>
#include<dirent.h>
#include<signal.h>
#include<sys/sysmacros.h>
#include<sys/stat.h>

void start_monitor(pid_t *child_pid)
{
    //char *args[]={"<","cmd.txt",NULL};
    if((*child_pid = fork()) < 0)
    {
        perror("Failed creating the monitor process");
        exit(-1);
    }
    else 
    if (*child_pid == 0)
    {
        printf("We are in the child process\n");
        execl("./tm","./tm",NULL);
    }
}

void write_to_file(char *cmd)
{
    int descr;
    if((descr=open("./cmd.txt",O_CREAT | O_WRONLY | O_TRUNC, 00777))<0)
    {
        perror("Failed creating the file");
        exit(-1);
    }
    write(descr,cmd,strlen(cmd));

}

int getCommandNumber(char *cmd)
{
    char *p=strtok(cmd," \n");
    int cnt=0;
        while(p!=NULL)
        {
            if(strcmp(p,"start_monitor")==0 && cnt==0)
            {
                return 1;
            }
            if(strcmp(p,"list_hunts")==0 && cnt==0)
            {
                return 2;
            }
            if(strcmp(p,"list_treasures")==0 && cnt==0)
            {
                return 3;
            }
            if(strcmp(p, "view_treasure")==0 && cnt==0)
            {
                return 4;
            }
            if(strcmp(p, "stop_monitor")==0 && cnt==0)
            {
                return 5;
            }
            if(strcmp(p, "exit")==0 && cnt==0)
            {
                return 6;
            }
            else
            {
                return -1;
            }
            p=strtok(NULL," \n");
        }
    return 0;
}

void monitor_terminated(int sig)
{
    printf("Monitor terminated\n");
}

int main(void)
{
    char cmd[100]; //string used to write data into a file that will be read by the monitor
    char cmd2[100];
    int command_number;
    pid_t child_pid;
    struct sigaction main_actions;
    memset(&main_actions, 0x00, sizeof(struct sigaction));
    //printf("%d\n",getpid());
    while(fgets(cmd,100,stdin)!=NULL)
    {
        strcpy(cmd2, cmd);
        command_number=getCommandNumber(cmd2);
        switch(command_number)
        {
            case 1:
                start_monitor(&child_pid);
                break;
            case 2:
                write_to_file(cmd);
                kill(child_pid, SIGUSR1);
                printf("SIGUSR1 sent\n");
                break;
            case 3:
                write_to_file(cmd);
                kill(child_pid, SIGUSR1);
                break;
            case 4:
                write_to_file(cmd);
                kill(child_pid, SIGUSR1);
                break;
            case 5:
                write_to_file(cmd);
                kill(child_pid, SIGUSR2);
                main_actions.sa_handler = monitor_terminated;
                if (sigaction(SIGCHLD, &main_actions, NULL) < 0)
                {
                    perror("sigaction SIGCHLD");
                    exit(-1);
                }
                break;
            case 6:
                exit(0);
                break;
            case -1:
                printf("Invalid command\n");
                exit(-1);
                break;
        }
    }
}