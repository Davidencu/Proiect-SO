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
#include <sys/sysmacros.h>
#include<sys/stat.h>

#define MAX_TREASURES 100

typedef struct coordinates{
  float x,y;
}coordinates;

typedef struct treasure{
  char id[8];
  char username[33];
  char clue[64];
  coordinates c;
  int value;
}treasure;

void write_to_logged_hunt(char *symlink_name,char *message)
{
  int descr=0;
  if((descr=open(symlink_name, O_WRONLY | O_APPEND))==-1)
    {
      perror("open symlink failed");
      exit(-1);
    }
    write(descr,message,strlen(message)); //+1
    if(close(descr)!=0)
    {
      perror("close symlink failed");
      exit(-1);
    }
}

void find_treasure_hunt(DIR *d,int *treasure_hunt_found,char *hunt_id, char *relative_path, int *directory_number,char *symlink_name)
{
  *directory_number=0;
  relative_path[0]='\0';
  char name[100];
  int symlink_found=0;
  sprintf(name,"./logged_hunt-<%s>",hunt_id);
  struct dirent *current_pointer=readdir(d);
  while(current_pointer != NULL) //If there is no directory inside the main directory
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
    else if((current_pointer->d_type = DT_REG) && (symlink_found == 0))
    {
      //struct stat statbuf;
      sprintf(symlink_name,"./%s",current_pointer->d_name);
      if(strcmp(name,symlink_name)==0)
      {
        symlink_found++;
      }
    }
    current_pointer=readdir(d);
  }
  //printf("%s\n",relative_path);
}

void add_function(char *hunt_id)
{
  DIR *d=NULL;
  int directory_number=0;
  int treasure_hunt_found=0;
  if((d=opendir("."))==NULL) //if the main directory cannot be opened
  {
    perror("Open the main directory");
    exit(-1);
  }
  else
  {
    printf("Entered main directory\n");
  }
  char relative_path[100];
  char logged_hunt_relative_path[400];
  char symlink_name[100];
  char message[100];
  int descr=0;
  find_treasure_hunt(d, &treasure_hunt_found, hunt_id, relative_path, &directory_number,symlink_name);
  if(treasure_hunt_found == 0)
  {
    printf("Hunt not found, adding the hunt...\n");
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
      //sprintf(logged_hunt_relative_path,"%s/logged_hunt.txt",relative_path);
      strcat(relative_path,"/");
      strcat(relative_path,hunt_id);
      sprintf(logged_hunt_relative_path,"%s/logged_hunt.txt",relative_path);
      if(mkdir(relative_path, 0777)==-1)
      {
        printf("Cannot add the hunt:%s\n", strerror(errno));
        exit(-1);
      }
      else
      {
        printf("Succesfully added the hunt\n");
      }
      if((descr = open(logged_hunt_relative_path, O_CREAT | O_WRONLY | O_APPEND, 0777))==-1)
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
    printf("Treasure ID: ");
    scanf("%7s",buffer.id);
    strcpy(buffer.username,getpwuid(getuid())->pw_name);
    printf("The username was automatically recorded\n");
    printf("Clue (you cannot use spaces): ");
    scanf("%63s",buffer.clue);
    printf("Coordinate x: ");
    scanf("%f",&buffer.c.x);
    printf("Coordinate y: ");
    scanf("%f",&buffer.c.y);
    printf("Value: ");
    scanf("%d",&buffer.value);
    if(write(descr,&buffer,sizeof(buffer))==-1)
    {
      perror("write treasure to the specified hunt");
      exit(-1);
    }
    if(close(descr)!=0)
    {
      perror("close the file containing the treasures");
      exit(-1);
    }
    //sprintf(symlink_name,"logged_hunt-<%s>",hunt_id);
    sprintf(message,"[%s]: add_treasure %s\n",getpwuid(getuid())->pw_name,hunt_id);
    write_to_logged_hunt(symlink_name,message);
  if(closedir(d)!=0)
  {
    perror("close the hunt directory");
    exit(-1);
  }
  else
  {
    printf("Succesfully exited the main directory\n");
  }
}

void treasure_hunt_file_operation(char *hunt_id,char *treasure_id, int operation) //the operation field selects what to be executed
{
  treasure treasures[MAX_TREASURES];
  int treasures_number=0;
  int directory_number=0;
  DIR *d=NULL;
  char relative_path[100];
  char symlink_name[100];
  char message[100];
  struct stat statbuf;
  int treasure_hunt_found=0;
  if ((d=opendir("."))==NULL)
  {
    perror("Open the hunt directory");
    exit(-1);
  }
  find_treasure_hunt(d, &treasure_hunt_found, hunt_id, relative_path, &directory_number,symlink_name);
  if(treasure_hunt_found)
  {
    
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
        int treasure_found=0;
        for(int i=0;i<treasures_number;i++)
        {
          if(strcmp(treasures[i].id, treasure_id)==0)
          {
            treasure_found++;
            for (int j=i;j<treasures_number-1;j++)
            {
              treasures[j]=treasures[j+1];
            }
            treasures_number--;
            i--;
          }
          
        }
        if(treasure_found!=0)
        {
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
        else
        {
          printf("Treasure not found\n");
        }
        sprintf(message,"[%s]: remove_treasure %s %s\n",getpwuid(getuid())->pw_name,hunt_id, treasure_id);
        write_to_logged_hunt(symlink_name,message);
      }
      if(operation == 0) //print all the treasures in a hunt
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
        for(int i=0;i<treasures_number;i++)
        {
          printf("Treasure %d: %s - %s - %s - %f - %f - %d\n",i,treasures[i].id,treasures[i].username,treasures[i].clue,treasures[i].c.x,treasures[i].c.y,treasures[i].value);
        }
        
        sprintf(message,"[%s]: list %s\n",treasures[0].username,hunt_id);
        write_to_logged_hunt(symlink_name,message);
      }
      if(operation==1) //print a specific treasure by its id
      {
        int treasure_found=0;
        for(int i=0;i<treasures_number;i++)
        {
          if(strcmp(treasures[i].id, treasure_id)==0)
          {
            treasure_found++;
            printf("Treasure %d: %s - %s - %s - %f - %f\n",i,treasures[i].id,treasures[i].username,treasures[i].clue,treasures[i].c.x,treasures[i].c.y);
            break;
          }
        }
        if(treasure_found==0)
        {
          printf("Treasure not found\n");
        }
        sprintf(message,"[%s]: view %s %s\n",getpwuid(getuid())->pw_name,hunt_id, treasure_id);
        write_to_logged_hunt(symlink_name,message);
      }
      
  }
  else
  {
    printf("Hunt not found\n");
  }
  if (closedir(d)!=0)
  {
    perror("close the hunt directory");
    exit(-1);
  }
}

void remove_hunt(char *hunt_id) //function for removing an entire hunt
{
  int directory_number=0;
  DIR *d=NULL;
  char relative_path[100];
  char file_relative_path[400];
  int treasure_hunt_found=0;
  char symlink_name[100];
  if ((d=opendir("."))==NULL)
  {
    perror("Open the hunt directory");
    exit(-1);
  }
  find_treasure_hunt(d, &treasure_hunt_found, hunt_id, relative_path, &directory_number,symlink_name);

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
        printf("File deletion done\n");
      }
      content=readdir(hunt);
    }
    if(closedir(hunt)!=0)
    {
      perror("close hunt directory");
      exit(-1);
    }
    printf("Done closing the hunt directory\n");
    if(rmdir(relative_path)==-1)
    {
      perror("Remove the hunt directory failed");
      exit(-1);
    }
    printf("Done deleting the hunt directory\n");
    if (unlink(symlink_name)==-1)
    {
      perror("Failed deleting the symlink");
      exit(-1);
    }
    printf("Done deleting the symlink\n");
  }
  else
  {
    printf("Hunt not found\n");
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
       add_function(argv[2]); //if the hunt does not exist, it will be added in a new directory, a logged hunt will also be created
                              ///and a symbolic link will point to the logged hunt
       break;
     case 2:
       treasure_hunt_file_operation(argv[2], " ", checkCommand(argv[1])-2); //list all the treasures
       break;
     case 3:
        treasure_hunt_file_operation(argv[2], argv[3], checkCommand(argv[1])-2); //list one specific treasure
       break;
     case 4:
        treasure_hunt_file_operation(argv[2], argv[3], checkCommand(argv[1])-2); //remove a specific treasure
       break;
     case 5:
        remove_hunt(argv[2]); //when we delete a hunt, we will also delete its logged hunt and, of course, 
                              //the symlink associated with it from the root directory
       break;
     case -1:
       printf("The first argument is not valid\n");
       break;
  }
  return 0;
}
