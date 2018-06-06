#include <stdio.h>

#define MAX_ARG 7
#define MAX_INPUT 80
#define MAX_PROMPT 10
#define MAX_PATH 10
#define PATH_LENGTH 32

int main(int argc, char *argv[]) {
  char *path[MAX_PATH];
  int path_count;
  FILE* rc = fopen("./.sh360.rc", "r");
  
  path_count=0;
  while(fgets(path[path_count], PATH_LENGTH, rc)){
  printf("it gets to here\n");
    path_count++;
    // printf("%s",path[path_count]);
  }
  // printf("%s\n",line);
  return 0;
}