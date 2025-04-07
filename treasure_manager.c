#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>

typedef struct coordinates{
  float x,y;
}coordinates;

typedef struct treasure{
  char id[16];
  char name[21];
  char clue[101];
  coordinates c;
}treasure;

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
  if ( argc < 2 || argc > 3 )
    {
      printf("Not enough arguments\n");
      exit(-1);
    }
  switch(checkCommand(argv[1]))
  {
     case 1:
       
       break;
     case 2:
       break;
     case 3:
       break;
     case 4:
       break;
     case 5:
       break;
     case -1:
       printf("The first argument is not valid\n");
       break;
  }
  return 0;
}
