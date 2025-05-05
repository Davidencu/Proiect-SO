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

int child_sleeping=0;

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
        //printf("We are in the child process\n");
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

void child_is_sleeping(int sig)
{
    child_sleeping=1;
}

void child_woke_up(int sig)
{
    child_sleeping=0;
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
    int monitor_exists=0;
    //int status;
    pid_t child_pid;
    struct sigaction main_actions;
    memset(&main_actions, 0x00, sizeof(struct sigaction));
    //printf("%d\n",getpid());
    while(1)
    {
        printf("-----------------------------\n");
        usleep(1000);
        printf("Enter command: ");
        usleep(1000);
        fgets(cmd,100,stdin);
        usleep(1000);
        printf("-----------------------------\n");
        strcpy(cmd2, cmd);
        command_number=getCommandNumber(cmd2);
        if(child_sleeping)
        {
            fprintf(stderr, "Cannot give command, child process is sleeping\n");
        }
        else{
        switch(command_number)
        {
            case 1:
                if(!monitor_exists)
                {
                    start_monitor(&child_pid);
                    monitor_exists=1;
                }
                else
                {
                    fprintf(stderr,"monitor already exists\n");
                }
                break;
            case 2:
                write_to_file(cmd);
                kill(child_pid, SIGUSR1);
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
                kill(child_pid, SIGUSR1);
                main_actions.sa_handler = child_is_sleeping;
                if (sigaction(SIGUSR1, &main_actions, NULL) < 0)
                {
                    perror("SIGUSR1 received from child");
                    exit(-1);
                }
                main_actions.sa_handler = child_woke_up;
                if (sigaction(SIGUSR2, &main_actions, NULL) < 0)
                {
                    perror("SIGUSR2 received from child");
                    exit(-1);
                }
                main_actions.sa_handler = monitor_terminated;
                if (sigaction(SIGCHLD, &main_actions, NULL) < 0)
                {
                    perror("SIGUSR2 received from child");
                    exit(-1);
                }
                monitor_exists=0;
                break;
            case 6:
                if(!monitor_exists)
                {
                    printf("Main process exited successfully\n");
                    exit(0);
                }
                else
                {
                    fprintf(stderr,"Monitor still running! Please kill the process first\n");
                }
                break;
            case -1:
                fprintf(stderr,"Invalid command, please insert a valid one\n");
                break;
        }
        }
    }
}