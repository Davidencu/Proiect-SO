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
#include <sys/sysmacros.h>
#include<sys/stat.h>

#define MAX_TREASURES 100

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

void write_to_logged_hunt(char *symlink_name,char *message) //function for writing a message to the logged_hunt
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

void find_treasure_hunt(DIR *d,int *treasure_hunt_found,char *hunt_id, char *relative_path, char *symlink_name)
{                                                                               //function for searching for a specific hunt and its symlink
  relative_path[0]='\0';
  char name[100];
  int symlink_found=0;
  sprintf(name,"./logged_hunt-%s",hunt_id);
  struct dirent *current_pointer=readdir(d);
  while(current_pointer != NULL) //while there are still elements in the main directory
  {
    if (current_pointer->d_type == DT_DIR && strcmp(current_pointer->d_name, ".git")!=0 && strcmp(current_pointer->d_name, ".")!=0 && strcmp(current_pointer->d_name, "..")!=0)
      {
          //printf("%s - %s\n", current_pointer_subd->d_name, hunt_id);
          if (strcmp(current_pointer->d_name, hunt_id)==0)
          {
            *treasure_hunt_found=1;
            sprintf(relative_path,"./%s",hunt_id);
          }
      }
    else if(symlink_found == 0)
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
}

void add_function(char *hunt_id) //adds a treasure in a specified hunt. If the hunt doesn't exist, it will be created and then a treasure will be added to it
{
  DIR *d=NULL;
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
  find_treasure_hunt(d, &treasure_hunt_found, hunt_id, relative_path,symlink_name);
  if(treasure_hunt_found == 0) //if the treasure hunt does not exist, we will add it
  {
    printf("Hunt not found, adding the hunt...\n");
    sprintf(relative_path,"./%s",hunt_id);
      if(mkdir(relative_path, 0777)==-1)
      {
        perror("Create the hunt directory");
        exit(-1);
      }
      else
      {
        printf("Succesfully created the hunt directory\n");
      }
      //sprintf(logged_hunt_relative_path,"%s/logged_hunt.txt",relative_path);
      sprintf(logged_hunt_relative_path,"%s/logged_hunt.txt",relative_path); //adding the logged hunt text file
      if((descr = open(logged_hunt_relative_path, O_CREAT | O_WRONLY | O_APPEND, 00777))==-1)
      {
        printf("Cannot add the logged hunt:%s\n", strerror(errno));
        exit(-1);
      }
      else
      {
        printf("Succesfully added the logged hunt\n");
      }
      sprintf(symlink_name,"logged_hunt-%s",hunt_id);
      if(symlink(logged_hunt_relative_path,symlink_name)==-1) //creating the symbolic link
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
    treasure buffer; //now we will add the treasure to the hunt
    strcat(relative_path,"/file.bin");
    descr=open(relative_path,O_CREAT | O_WRONLY | O_APPEND, 00777); //creating a binary file inside the unique hunt directory where the treasures are stored
    if(descr == -1)
    {
      perror("open treasure file failed");
      exit(-1);
    }
    printf("Treasure ID: "); //reading the treasure elements from the stdin
    scanf("%7s",buffer.id);
    printf("Username: ");
    scanf("%15s",buffer.username);
    getchar();
    printf("Clue: ");
    fgets(buffer.clue,64,stdin);
    buffer.clue[strlen(buffer.clue)-1]='\0';
    printf("Coordinate x: ");
    scanf("%f",&buffer.c.x);
    printf("Coordinate y: ");
    scanf("%f",&buffer.c.y);
    printf("Value: ");
    scanf("%d",&buffer.value);
    if(write(descr,&buffer,sizeof(buffer))==-1) //writing the treasure to the binary file
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
    sprintf(message,"add %s\n",hunt_id);
    write_to_logged_hunt(symlink_name,message); //printing the corresponding message to the logged_hunt
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
  treasure treasures[MAX_TREASURES]; //the array of treasures
  int treasures_number=0;
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
  find_treasure_hunt(d, &treasure_hunt_found, hunt_id, relative_path,symlink_name);
  if(treasure_hunt_found)
  {
    
    strcat(relative_path,"/file.bin");
      int descr=open(relative_path, O_RDONLY);
      if(descr<0)
      {
        perror("Open the file for reading the treasures");
        exit(-1);
      }
      while((read(descr,&treasures[treasures_number], sizeof(treasures)/MAX_TREASURES)) > 0) //reading the treasures from the file in the array called "treasures"
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
        for(int i=0;i<treasures_number;i++) //removing a treasure from the array
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
        if((descr=open(relative_path, O_TRUNC)) < 0) //deleting all the items inside the file in order to rewrite it with the remaining treasures in the array
        {
          perror("truncate the file");
          exit(-1);
        }
        if(close(descr)!=0)
        {
          perror("close the treasures file");
          exit(-1);
        }
        if((descr=open(relative_path, O_WRONLY | O_APPEND)) < 0) //now we will rewrite the binary file
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
        sprintf(message,"remove_treasure %s %s\n",hunt_id, treasure_id);
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
          printf("Treasure %d: %s - %s - %s - %.2f - %.2f - %d\n",i,treasures[i].id,treasures[i].username,treasures[i].clue,treasures[i].c.x,treasures[i].c.y,treasures[i].value);
        }
        
        sprintf(message,"list %s\n",hunt_id);
        write_to_logged_hunt(symlink_name,message); //printing the corresponding message to the lgged_hunt
      }
      if(operation==1) //print a specific treasure by its id
      {
        if(treasure_id==NULL)
        {
          printf("Treasure id not given\n");
          return;
        }
        int treasure_found=0;
        for(int i=0;i<treasures_number;i++)
        {
          if(strcmp(treasures[i].id, treasure_id)==0)
          {
            treasure_found++;
            printf("Treasure %d: %s - %s - %s - %f - %f - %d\n",i,treasures[i].id,treasures[i].username,treasures[i].clue,treasures[i].c.x,treasures[i].c.y,treasures[i].value);
            break;
          }
        }
        if(treasure_found==0)
        {
          printf("Treasure not found\n");
        }
        sprintf(message,"view %s %s\n",hunt_id, treasure_id);
        write_to_logged_hunt(symlink_name,message); //printing the corresponding message to the logged_hunt file
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
  find_treasure_hunt(d, &treasure_hunt_found, hunt_id, relative_path,symlink_name);

  if(treasure_hunt_found==1) //we only delete the hunt if we found it
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
        if(unlink(file_relative_path)==-1) //we delete all the files inside the hunt directory
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
    if(rmdir(relative_path)==-1) //removing the hunt directory
    {
      perror("Remove the hunt directory failed");
      exit(-1);
    }
    printf("Done deleting the hunt directory\n");
    if (unlink(symlink_name)==-1) //deleting the symbolic link
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

void list_all_hunts()
{
  DIR *d;
  DIR *d2;
  treasure treasures[100];
  char path[300];
  if ((d=opendir("."))==NULL)
  {
    perror("Open the hunt directory");
    exit(-1);
  }
  struct dirent *current_pointer=readdir(d);
  while(current_pointer != NULL) //while there are still elements in the main directory
  {
    if (current_pointer->d_type == DT_DIR && strcmp(current_pointer->d_name, ".git")!=0 && strcmp(current_pointer->d_name, ".")!=0 && strcmp(current_pointer->d_name, "..")!=0)
      {
          //printf("%s - %s\n", current_pointer_subd->d_name, hunt_id);
          printf("%s,",current_pointer->d_name);
          sprintf(path, "./%s", current_pointer->d_name);
          if ((d2=opendir(path))==NULL)
          {
            perror("Open the hunt failed");
            exit(-1);
          }
          sprintf(path, "./%s/file.bin", current_pointer->d_name);
          int descr=open(path, O_RDONLY);
          if(descr<0)
          {
              perror("Open the file for reading the treasures");
              exit(-1);
          }
          int treasures_number=0;
          while((read(descr,&treasures[treasures_number], sizeof(treasures)/MAX_TREASURES)) > 0) //reading the treasures from the file in the array called "treasures"
          {
            treasures_number++;
          }
          printf("%d\n",treasures_number);
          if(close(descr)!=0)
          {
            perror("Close the treasures file");
            exit(-1);
          }
          if (closedir(d2)!=0)
          {
            perror("Close the hunt failed");
            exit(-1);
          }
      }
      current_pointer=readdir(d);
  }
  if (closedir(d)!=0)
  {
    perror("Close the hunt directory failed");
    exit(-1);
  }
}


int checkCommand(char *command) //function that returns a value according to the given command
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
  if(strcmp(command,"list_hunts")==0)
    {
      return 6;
    }
  return -1; //if the word is anything else
}

int main(int argc,char **argv) //the main function
{
  if ( argc < 2 || argc > 4 )
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
      case 6:
        list_all_hunts();
        break;
     case -1:
       printf("The first argument is not valid\n");
       break;
  }
  return 0;
}
