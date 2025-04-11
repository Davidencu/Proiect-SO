#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<fcntl.h>
#include<stdint.h>
#include<unistd.h>
#include<time.h>
#include<dirent.h>
#include <sys/sysmacros.h>
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
          //printf("%s - %s\n", current_pointer_subd->d_name, hunt_id);
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
  DIR *d=NULL;
  int directory_number=0;
  int treasure_hunt_found=0;
  if((d=opendir("."))==NULL) //if the Proiect_SO directory somehow does not exist
  {
    perror("Open the main directory");
    exit(-1);
  }
  else
  {
    printf("Entered main directory\n");
  }
  char relative_path[100];
  //char hunt_relative_path[400];
  char logged_hunt_relative_path[400];
  char symlink_name[100];
  int descr=0;
  find_treasure_hunt(d, &treasure_hunt_found, hunt_id, relative_path, &directory_number);
  if(treasure_hunt_found == 0)
  {
    sprintf(relative_path,"./%d",directory_number);
      if(mkdir(relative_path, 0777)==-1)
      {
        perror("Create the hunt directory");
        exit(-1);
      }
      else
      {
        printf("Succesfully created directory no %d\n", directory_number);
      }
      sprintf(logged_hunt_relative_path,"%s/logged_hunt.txt",relative_path);
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
      if((descr = open(logged_hunt_relative_path, O_CREAT | O_WRONLY, 0777))==-1)
      {
        printf("Cannot add the logged hunt:%s\n", strerror(errno));
        exit(-1);
      }
      else
      {
        printf("Succesfully added the logged hunt\n");
      }
      sprintf(symlink_name,"logged_hunt-<%s>",hunt_id);
      if(symlink(logged_hunt_relative_path,symlink_name)==-1)
      {
        perror("symlink failed");
        exit(-1);
      }
      if(close(descr)!=0)
      {
        perror("close");
        exit(-1);
      }
  }
    treasure buffer;
    strcat(relative_path,"/file.bin");
    descr=open(relative_path,O_CREAT | O_WRONLY | O_APPEND, 00755);
    if(descr == -1)
    {
      perror("open treasure file failed");
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
      perror("write treasure to the specified hunt");
      exit(-1);
    }
    if(close(descr)!=0)
    {
      perror("close the file containinng the treasures");
      exit(-1);
    }
  if(closedir(d)!=0)
  {
    perror("close the hunt directory");
    exit(-1);
  }
  else
  {
    printf("Succesfully exited the Proiect-SO directory\n");
  }
}

void treasure_hunt_file_operation(char *hunt_id,char *treasure_id, int operation)
{
  treasure treasures[MAX_TREASURES];
  int treasures_number=0;
  int directory_number=0;
  DIR *d=NULL;
  char relative_path[100];
  char symlink_relative_path[100];
  //char file_relative_path[400];
  struct stat statbuf;
  int treasure_hunt_found=0;
  if ((d=opendir("."))==NULL)
  {
    perror("Open the hunt directory");
    exit(-1);
  }
  find_treasure_hunt(d, &treasure_hunt_found, hunt_id, relative_path, &directory_number);
  if(treasure_hunt_found)
  {
    sprintf(symlink_relative_path,"logged_hunt-<%s>",hunt_id);
    strcat(relative_path,"/file.bin");
      int descr=open(relative_path, O_RDONLY);
      if(descr<0)
      {
        perror("Open the file for reading the treasures");
        exit(-1);
      }
      while((read(descr,&treasures[treasures_number], sizeof(treasures)/MAX_TREASURES)) > 0)
        {
          treasures_number++;
        }
        if(close(descr)!=0)
        {
          perror("close the treasures file");
          exit(-1);
        }
      if(operation == 2) //if op = 2 we remove the specified treasure
      {
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
        if((descr=open(relative_path, O_TRUNC)) < 0)
        {
          perror("truncate the file");
          exit(-1);
        }
        if(close(descr)!=0)
        {
          perror("close the treasures file");
          exit(-1);
        }
        if((descr=open(relative_path, O_WRONLY | O_APPEND)) < 0)
        {
          perror("open the treasures file for writing");
          exit(-1);
        }
        for(int i=0;i<treasures_number;i++)
        {
          if(write(descr,&treasures[i],sizeof(treasures)/MAX_TREASURES)==-1)
          {
            perror("write a treasure to the treasures file");
            exit(-1);
          }
        }
        if(close(descr)!=0)
        {
          perror("close the treasures file");
          exit(-1);
        }
      }
      if(operation == 0)
      {
        printf("Hunt name: %s\n",hunt_id);
        if(stat(relative_path,&statbuf)!=0)
        {
          perror("Get file status failed");
          exit(-1);
        }
        else
        {
          printf("File size:                %jd bytes\n",(intmax_t) statbuf.st_size);
          printf("Last file modification:   %s\n", ctime(&(statbuf.st_mtime)));
        }
        printf("%d\n",treasures_number);
        
        for(int i=0;i<treasures_number;i++)
        {
          printf("Treasure %d: %s - %s - %s - %f - %f\n",i,treasures[i].id,treasures[i].name,treasures[i].clue,treasures[i].c.x,treasures[i].c.y);
        }
      }
      if(operation==1)
      {
        for(int i=0;i<treasures_number;i++)
        {
          if(strcmp(treasures[i].id, treasure_id)==0)
          {
            printf("Treasure %d: %s - %s - %s - %f - %f\n",i,treasures[i].id,treasures[i].name,treasures[i].clue,treasures[i].c.x,treasures[i].c.y);
            break;
          }
        }
      }
      if (closedir(d)!=0)
      {
        perror("close the hunt directory");
        exit(-1);
      }
  }
}

void remove_hunt(char *hunt_id) //function for removing an entire hunt
{
  int directory_number=0;
  DIR *d=NULL;
  char relative_path[100];
  char file_relative_path[400];
  int treasure_hunt_found=0;
  if ((d=opendir("."))==NULL)
  {
    perror("Open the hunt directory");
    exit(-1);
  }
  find_treasure_hunt(d, &treasure_hunt_found, hunt_id, relative_path, &directory_number);

  if(treasure_hunt_found==1)
  {
    printf("Hunt found\n");
    DIR *hunt = opendir(relative_path);
    struct dirent *content=NULL;
    if(hunt==NULL)
    {
      perror("open hunt directory");
      exit(-1);
    }
    content=readdir(hunt);
    while(content!=NULL)
    {
      if(content->d_type == DT_REG)
      {
        sprintf(file_relative_path,"%s/%s",relative_path,content->d_name);
        printf("%s\n",file_relative_path);
        if(unlink(file_relative_path)==-1)
        {
          perror("Delete a treasure file failed");
          exit(-1);
        }
      }
      content=readdir(hunt);
    }
    if(closedir(hunt)!=0)
    {
      perror("close hunt directory");
      exit(-1);
    }
    if(rmdir(relative_path)==-1)
    {
      perror("Remove the hunt directory failed");
      exit(-1);
    }
  }
  if (closedir(d)!=0)
  {
    perror("close the hunt directory");
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
       treasure_hunt_file_operation(argv[2], " ", checkCommand(argv[1])-2);
       break;
     case 3:
        treasure_hunt_file_operation(argv[2], argv[3], checkCommand(argv[1])-2);
       break;
     case 4:
        treasure_hunt_file_operation(argv[2], argv[3], checkCommand(argv[1])-2);
       break;
     case 5:
        remove_hunt(argv[2]);
       break;
     case -1:
       printf("The first argument is not valid\n");
       break;
  }
  return 0;
}
