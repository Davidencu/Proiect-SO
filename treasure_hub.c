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

#define MAX_HUNTS 30

int child_sleeping=0;
int monitor_exists=0;
int pfd[2]; //the pipe file descriptors

void handle(int sig)
{
    char monitor_cmd[100];
    char actual_command[10];
    char hunt_id[12];
    char treasure_id[8];
    int status;
    int fd=open("cmd.txt",O_RDONLY); //monitor reads the command from the cmd.txt file created by the main process
    read(fd,monitor_cmd,sizeof(monitor_cmd));
    close(fd);
    int word_count=0;
    char *p=strtok(monitor_cmd," \n");
    while(p!=NULL) //it reads the contents of the file in a formatted manner
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
    if((strcmp(actual_command,"list_hunts")==0) || (strcmp(actual_command,"calculate_scores")==0))
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
            execl("./treasure_manager","./treasure_manager","list_hunts",NULL);
        }
        wait(&status);
        if(!WIFEXITED(status))
        {
            printf("Monitor child exited abnormally\n");
            exit(-1);
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
            execl("./treasure_manager","./treasure_manager","list",hunt_id,NULL);
        }
        wait(&status);
        if(!WIFEXITED(status))
        {
            printf("Monitor child exited abnormally\n");
            exit(-1);
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
            execl("./treasure_manager","./treasure_manager","view",hunt_id,treasure_id,NULL);
        }
        wait(&status);
        if(!WIFEXITED(status))
        {
            printf("Monitor child exited abnormally\n");
            exit(-1);
        }
    }

    if(strcmp(actual_command,"stop_monitor")==0)
    {
      kill(getppid(), SIGUSR1); //sends a signal to the main process announcing that the monitor is going to sleep
      usleep(15000000); //puts the monitor to sleep for 15 seconds
      kill(getppid(), SIGUSR2); //sends another signal to the main process announcing that the monitor woke up
      close(pfd[1]);
      exit(0); //ends the monitor
    }
}

void monitor()
{
    close(pfd[0]); //close the read descriptor of the pipe
    dup2(pfd[1], 1); //redirects the stdout of the monitor to the write end of the pipe
    struct sigaction monitor_actions;
    memset(&monitor_actions, 0x00, sizeof(struct sigaction));
    monitor_actions.sa_handler = handle; //if the monitor received SIGUSR1 then it will execute the instructions inside of the handle function
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

void write_to_file(char *cmd) //the function for writing the command details to the file
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
            if(strcmp(p, "calculate_scores")==0 && cnt==0)
            {
                return 7;
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
    monitor_exists=0;
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
    char buf[1000];
    char buf2[1000];
    int command_number;
    pid_t child_pid;
    struct sigaction main_actions;
    //FILE *stream=NULL;
    memset(&main_actions, 0x00, sizeof(struct sigaction));
    if(pipe(pfd)<0)
	{
		printf("Eroare la crearea pipe-ului\n");
		exit(1);
	}
    while(1)
    {
        usleep(1000);
        printf("Enter command: ");
        usleep(1000);
        fgets(cmd,100,stdin); //reads the current command from stdin
        usleep(1000);
        strcpy(cmd2, cmd);
        command_number=getCommandNumber(cmd2);
        if(child_sleeping) //if the monitor is sleeping, print an error message
        {
            fprintf(stderr, "Cannot give command, monitor is sleeping\n");
        }
        else{
        if(command_number==-1)
        {
            fprintf(stderr,"Invalid command, please insert a valid one\n");
        }
        if(command_number==1)
        {
            if(!monitor_exists)
                {
                    start_monitor(&child_pid);
                    monitor_exists=1;
                    close(pfd[1]); //close the write descriptor of the pipe
                }
                else
                {
                    fprintf(stderr,"monitor already exists\n");
                }
        }
        if(command_number>=2 && command_number<=4)
        {
            memset(buf,0,sizeof(buf));
            if (monitor_exists)
                {
                    write_to_file(cmd); //writes to the cmd.txt the details about the current command then sends SIGUSR1 to the monitor
                    kill(child_pid, SIGUSR1);
                    read(pfd[0],buf,sizeof(buf)); //the main process reads from the pipe then prints the result to stdout
                    printf("%s",buf);
                }
                else
                {
                    fprintf(stderr,"Monitor does not exist, please create it!\n");
                }
        }
        if(command_number==5)
        {
            if(monitor_exists)
            {
                write_to_file(cmd);
                kill(child_pid, SIGUSR1);
                close(pfd[0]);
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
                    perror("SIGCHLD received from child");
                    exit(-1);
                }
            }
            else
            {
                fprintf(stderr,"stop_monitor error: Monitor does not exist. Please create it!\n");
            }
        }
        if(command_number==6)
        {
            if(!monitor_exists)
            {
                exit(0);
            }
            else
            {
                fprintf(stderr,"Monitor still running! Please kill the process first\n");
            }
        }
        if(command_number==7)
        {
            memset(buf,0,sizeof(buf));
            if (monitor_exists)
            {
                int pfd2[2];
                write_to_file(cmd); //writes to the cmd.txt the details about the current command then sends SIGUSR1 to the monitor
                kill(child_pid, SIGUSR1);
                read(pfd[0],buf,sizeof(buf));
                int is_a_hunt_id=0;
                char *p=strtok(buf,",");
                is_a_hunt_id=1;
                if(pipe(pfd2)<0)
                {
                    fprintf(stderr,"Cannot create pipe\n");
                    exit(-1);
                }
                while(p!=NULL)
                {
                    if (is_a_hunt_id) 
                    {
                        int hunt_pid;
                        int status;
                        if((hunt_pid=fork())<0)
                        {
                            fprintf(stderr,"Cannot create a calculate score process\n");
                            exit(-1);
                        }
                        if(hunt_pid==0)
                        {
                            close(pfd2[0]);
                            dup2(pfd2[1],1); //redirects the stdout of a calculate_score process to the pipe
                            execl("./calculate_scores","./calculate_scores",p,NULL);
                        }
                        close(pfd2[1]);
                        memset(buf2,0,sizeof(buf));
                        read(pfd2[0],buf2,sizeof(buf2));
                        printf("%s",buf2);
                        p=strtok(NULL,"\n");
                        is_a_hunt_id=0;
                        wait(&status);
                        if(!WIFEXITED(status))
                        {
                            printf("Calculate scores process ended abnormally\n");
                            exit(-1);
                        }
                    }
                    else
                    {
                        p=strtok(NULL,",");
                        is_a_hunt_id=1;
                    }
                }
                close(pfd2[0]);
            }
            else
            {
                fprintf(stderr,"Monitor does not exist, please create it!\n");
            }
        }
        }
    }
}