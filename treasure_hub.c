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

void handle(int sig)
{
    char monitor_cmd[100];
    char actual_command[10];
    char hunt_id[12];
    char treasure_id[8];
    int fd=open("cmd.txt",O_RDONLY); //monitor reads the command from the cmd.txt file created by the main process
    read(fd,monitor_cmd,sizeof(monitor_cmd));
    close(fd);
    int word_count=0;
    char *p=strtok(monitor_cmd," \n");
    while(p!=NULL)
    {
      switch(word_count)
      {
        case 0:
          strcpy(actual_command,p);
          break;
        case 1:
          strcpy(hunt_id,p);
          break;
        case 2:
          strcpy(treasure_id,p);
          break;
      }
      word_count++;
      p=strtok(NULL," \n");
    }
    if(strcmp(actual_command,"list_hunts")==0)
    {
        int child_pid2;
        if((child_pid2 = fork()) < 0) //forking the monitor to execute the treasure manager with the command line arguments read from the file
        {
            perror("Failed creating the monitor process");
            exit(-1);
        }
        else 
        if (child_pid2 == 0) //executing the treasure manager in the child monitor process
        {
            execl("./tm","./tm","list_hunts",NULL);
        }
    }
    if(strcmp(actual_command,"list_treasures")==0)
    {
        int child_pid2;
        if((child_pid2 = fork()) < 0)
        {
            perror("Failed creating the monitor process");
            exit(-1);
        }
        else 
        if (child_pid2 == 0)
        {
            execl("./tm","./tm","list",hunt_id,NULL);
        }
    }
    if(strcmp(actual_command,"view_treasure")==0)
    {
        int child_pid2;
        if((child_pid2 = fork()) < 0)
        {
            perror("Failed creating the monitor process");
            exit(-1);
        }
        else 
        if (child_pid2 == 0)
        {
            execl("./tm","./tm","view",hunt_id,treasure_id,NULL);
        }
    }
    if(strcmp(actual_command,"stop_monitor")==0)
    {
      printf("SIGUSR1 received from parent\n");
      kill(getppid(), SIGUSR1); //sends a signal to the main process announcing that the monitor is going to sleep
      usleep(20000000); //puts the monitor to sleep for 20 seconds
      kill(getppid(), SIGUSR2); //sends another signal to the main process announcing that the monitor woke up
      exit(0); //ends the monitor
    }
}

void monitor()
{
    struct sigaction monitor_actions;
    memset(&monitor_actions, 0x00, sizeof(struct sigaction));
    monitor_actions.sa_handler = handle;
    if (sigaction(SIGUSR1, &monitor_actions, NULL) < 0)
    {
      perror("sigaction SIGUSR1 handle");
      exit(-1);	     
    }
  while(1);
}

void start_monitor(pid_t *child_pid)
{
    if((*child_pid = fork()) < 0)
    {
        perror("Failed creating the monitor process");
        exit(-1);
    }
    else 
    if (*child_pid == 0)
    {
        monitor(); //executes the monitor code
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
    int status;
    pid_t pid;

    if ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status)) {
            printf("\nMonitor exited with status %d\n", WEXITSTATUS(status));
        } else {
            printf("\nMonitor terminated abnormally\n");
        }
    }
}

int main(void)
{
    char cmd[100]; //string used to write data into a file that will be read by the monitor
    char cmd2[100];
    int command_number;
    int monitor_exists=0;
    pid_t child_pid;
    struct sigaction main_actions;
    memset(&main_actions, 0x00, sizeof(struct sigaction));
    //printf("%d\n",getpid());
    while(1)
    {
        //printf("-----------------------------\n");
        usleep(1000);
        printf("Enter command: ");
        usleep(1000);
        fgets(cmd,100,stdin); //reads the current command from stdin
        usleep(1000);
        //printf("-----------------------------\n");
        strcpy(cmd2, cmd);
        command_number=getCommandNumber(cmd2);
        if(child_sleeping) //if the monitor is sleeping, print an error message
        {
            fprintf(stderr, "Cannot give command, monitor is sleeping\n");
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
                if (monitor_exists)
                {
                    write_to_file(cmd); //writes to the cmd.txt the details about the current command then sends SIGUSR1 to the monitor
                    kill(child_pid, SIGUSR1);
                }
                else
                {
                    fprintf(stderr,"Monitor does not exist, please create it!\n");
                }
                break;
            case 3:
                if(monitor_exists)
                {
                    write_to_file(cmd);
                    kill(child_pid, SIGUSR1);
                }
                else
                {
                    fprintf(stderr,"Monitor does not exist, please create it!\n");
                }
                break;
            case 4:
                if(monitor_exists)
                {
                    write_to_file(cmd);
                    kill(child_pid, SIGUSR1);
                }
                else
                {
                    fprintf(stderr,"Monitor does not exist, please create it!\n");
                }
                break;
            case 5:
                if(monitor_exists)
                {
                    write_to_file(cmd);
                    kill(child_pid, SIGUSR1);
                    main_actions.sa_handler = child_is_sleeping; //if main process receives SIGUSR1 from monitor, it will set the child_is_sleeping variable to 1
                    if (sigaction(SIGUSR1, &main_actions, NULL) < 0)
                    {
                        perror("SIGUSR1 received from child");
                        exit(-1);
                    }
                    main_actions.sa_handler = child_woke_up; //if main process receives SIGUSR2 from monitor, it will set the child_is_sleeping variable to 0
                    if (sigaction(SIGUSR2, &main_actions, NULL) < 0)
                    {
                        perror("SIGUSR2 received from child");
                        exit(-1);
                    }
                    main_actions.sa_handler = monitor_terminated; //if main process receives SIGCHLD from monitor, it will acknowledge the user that the monitor process is finished
                    if (sigaction(SIGCHLD, &main_actions, NULL) < 0)
                    {
                        perror("SIGUSR2 received from child");
                        exit(-1);
                    }
                    
                    monitor_exists=0;
                }
                else
                {
                    printf("Monitor does not exist. Please create it!\n");
                }
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