#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<fcntl.h>
#include<stdint.h>
#include<unistd.h>
#include<dirent.h>
#include<sys/stat.h>

#define MAX_TREASURES 100

typedef struct coordinates{
  float x,y;
}coordinates;

typedef struct treasure{
  char id[8];
  char name[16];
  char clue[64];
  coordinates c;
}treasure;

void get_cwd_name(char *cwd,char p2[][100])
{
  char *p=strtok(cwd,"/");
  while(p!=NULL)
  {
    strcpy(*p2,p);
    p=strtok(NULL,"/");
  }
}

void find_treasure_hunt(DIR *d,int *treasure_hunt_found,char *hunt_id, char *relative_path, int *directory_number)
{
  *directory_number=0;
  relative_path[0]='\0';
  struct dirent *current_pointer=readdir(d);
  while(current_pointer != NULL) //If there is no directory inside Proiect_SO
  {
    if (current_pointer->d_type == DT_DIR && strcmp(current_pointer->d_name, ".git")!=0 && strcmp(current_pointer->d_name, ".")!=0 && strcmp(current_pointer->d_name, "..")!=0)
    {
      DIR *subd=opendir(current_pointer->d_name);
      struct dirent *current_pointer_subd=readdir(subd);
      
      if(subd!=NULL)
      {
        while(current_pointer_subd != NULL)
        {
          printf("%s - %s\n", current_pointer_subd->d_name, hunt_id);
          if (current_pointer_subd->d_type == DT_DIR && strcmp(current_pointer_subd->d_name, hunt_id)==0)
          {
            *treasure_hunt_found=1;
            sprintf(relative_path,"./%s/%s",current_pointer->d_name,hunt_id);
            break;
          }
          current_pointer_subd = readdir(subd);
        }
      }
      *directory_number=*directory_number+1;
      if(closedir(subd)!=0)
      {
        perror("closedir");
        exit(-1);
      }
    }
    current_pointer=readdir(d);
  }
  printf("%s\n",relative_path);
}

void add_function(char *hunt_id)
{
  char p2[100];
  char cwd[1000];
  //struct dirent *current_pointer=NULL;
  getcwd(cwd, sizeof(cwd));
  get_cwd_name(cwd,&p2);
  printf("%s\n",p2);
  DIR *d=NULL;
  int directory_number=0;
  int treasure_hunt_found=0;
  if((d=opendir("."))==NULL) //if the Proiect_SO directory somehow does not exist
  {
    if(mkdir("Proiect_SO", 0777)==-1)
    {
      perror("mkdir");
      exit(-1);
    }
    else
    {
      printf("Created SO directory\n");
    }
  }
  else
  {
    printf("Entered Proiect_SO directory\n");
  }
  char relative_path[100];
  find_treasure_hunt(d, &treasure_hunt_found, hunt_id, relative_path, &directory_number);
  if(treasure_hunt_found == 0)
  {
    sprintf(relative_path,"./%d",directory_number);
      if(mkdir(relative_path, 0777)==-1)
      {
        perror("mkdir");
        exit(-1);
      }
      else
      {
        printf("Succesfully created directory no %d\n", directory_number);
      }
      strcat(relative_path,"/");
      strcat(relative_path,hunt_id);
      if(mkdir(relative_path, 0777)==-1)
      {
        printf("Cannot add the hunt:%s\n", strerror(errno));
        exit(-1);
      }
      else
      {
        printf("Succesfully added the hunt\n");
      }
  }
    treasure buffer;
    strcat(relative_path,"/file.bin");
    int descr=open(relative_path,O_CREAT | O_WRONLY | O_APPEND, 00755);
    if(descr == -1)
    {
      perror("open");
      exit(-1);
    }
    printf("%d\n",descr);
    scanf("%7s",buffer.id);
    scanf("%15s",buffer.name);
    scanf("%63s",buffer.clue);
    scanf("%f",&buffer.c.x);
    scanf("%f",&buffer.c.y);
    if(write(descr,&buffer,sizeof(buffer))==-1)
    {
      perror("write");
      exit(-1);
    }
    if(close(descr)!=0)
    {
      perror("close");
      exit(-1);
    }
  if(closedir(d)!=0)
  {
    perror("closedir");
    exit(-1);
  }
  else
  {
    printf("Succesfully exited the Proiect-SO directory\n");
  }
}

void re_move(char *hunt_id,char *treasure_id)
{
  treasure treasures[MAX_TREASURES];
  printf("%ld\n",sizeof(treasures)/MAX_TREASURES);
  int treasures_number=0;
  int directory_number=0;
  DIR *d=NULL;
  char relative_path[100];
  int treasure_hunt_found=0;
  if ((d=opendir("."))==NULL)
  {
    perror("opendir");
    exit(-1);
  }
  find_treasure_hunt(d, &treasure_hunt_found, hunt_id, relative_path, &directory_number);
  if(treasure_hunt_found)
  {
    strcat(relative_path,"/file.bin");
    int descr=open(relative_path, O_RDONLY);
    if(descr<0)
    {
      perror("open");
      exit(-1);
    }
    else
    {
      while((read(descr,&treasures[treasures_number], sizeof(treasures)/MAX_TREASURES)) > 0)
      {
        treasures_number++;
      }
      for(int i=0;i<treasures_number;i++)
      {
        if(strcmp(treasures[i].id, treasure_id)==0)
        {
          for (int j=i;j<treasures_number-1;j++)
          {
            treasures[i]=treasures[i+1];
          }
          treasures_number--;
        }
      }
    }
    if(close(descr)!=0)
    {
      perror("close");
      exit(-1);
    }
    if(open(relative_path, O_TRUNC) < 0)
    {
      perror("open");
      exit(-1);
    }
    if(close(descr)!=0)
    {
      perror("close");
      exit(-1);
    }
    if(open(relative_path, O_WRONLY | O_APPEND) < 0)
    {
      perror("open");
      exit(-1);
    }
    for(int i=0;i<treasures_number;i++)
    {
      if(write(descr,&treasures[i],sizeof(treasures)/MAX_TREASURES)==-1)
      {
        perror("write");
        exit(-1);
      }
    }
    if(close(descr)!=0)
    {
      perror("close");
      exit(-1);
    }
  }
  if (closedir(d)!=0)
  {
    perror("closedir");
    exit(-1);
  }
}

int checkCommand(char *command)
{
  if(strcmp(command,"add")==0)
    {
      return 1;
    }
  if(strcmp(command,"list")==0)
    {
      return 2;
    }
  if(strcmp(command,"view")==0)
    {
      return 3;
    }
  if(strcmp(command,"remove_treasure")==0)
    {
      return 4;
    }
  if(strcmp(command,"remove_hunt")==0)
    {
      return 5;
    }
  return -1; //daca cuvantul e orice altceva
}

int main(int argc,char **argv)
{
  if ( argc < 3 || argc > 4 )
    {
      printf("Not enough arguments\n");
      exit(-1);
    }
  switch(checkCommand(argv[1]))
  {
     case 1:
       add_function(argv[2]);
       break;
     case 2:
       break;
     case 3:
       break;
     case 4:
        re_move(argv[2], argv[3]);
       break;
     case 5:
       break;
     case -1:
       printf("The first argument is not valid\n");
       break;
  }
  return 0;
}
