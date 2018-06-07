//V00864456
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>


#define MAX_ARG 7
#define MAX_INPUT 80
#define MAX_PROMPT 10
#define MAX_PATH 10

int findPath(char path[][MAX_INPUT],char command[],int path_count) {
  struct stat file_stat;
  char filename[MAX_INPUT*2];

  for(int i =0; i < path_count; i++){
    strncpy(filename,path[i],MAX_INPUT);
    strcat(filename,command);
    // check if it exists
    printf("checking path_count: %d with path: %s\n",i,filename);
    if (stat(filename, &file_stat) != 0) {
      printf("cannot find %s\n",
          filename);
    } else {
      printf("%s exists\n",
          filename);
      if(!(file_stat.st_mode & S_IXOTH)){
        printf("%s is not executable\n",filename);
        return -1;
      }
      if(!(file_stat.st_mode & S_IROTH)){
        printf("%s is not readable\n",filename);
        return -1;
      }
      return i;
    }
  }
  
  printf("Unable to find command\n");
  return -1;
}

void single(char path[][MAX_INPUT],char input[],int path_count) {
  int pid;
  char *args[MAX_ARG+1];
  char *envp[] = {0};
  int status;
  char *t;
  char command[MAX_INPUT];
  int correct_path;

  int num_tokens = 0;
  t = strtok(input, " ");

  if(t == NULL){
    printf("No command provided\n");
    return;
  }
  strncpy(command,t,MAX_INPUT);
  printf("command is: %s\n",command);
  correct_path = findPath(path, command,path_count);
  if(correct_path==-1){
    return;
  }
  
  while (t != NULL && num_tokens < MAX_ARG) {
    if(num_tokens == 0){
      char pre[MAX_INPUT*2];
      strncpy(pre,path[correct_path],MAX_INPUT);
      strcat(pre,t);
      args[num_tokens] = pre;
      printf("args: %s\n",args[num_tokens]);
      num_tokens++;
      t = strtok(NULL, " ");
    } else {
      args[num_tokens] = t;
      printf("args: %s\n",args[num_tokens]);
      num_tokens++;
      t = strtok(NULL, " ");
    }
  }
  args[num_tokens] = 0;

  if((pid = fork()) ==0) {
    printf("child starting\n");
    execve(args[0], args, envp);
  }
  waitpid(pid, &status, 0);
}

void pp(char path[][MAX_INPUT],char input[],int path_count) {
  int status;
  char *args_head[MAX_ARG+1];
  char *args_tail[MAX_ARG+1];
  char *envp[] = {0};
  char *t;
  char command[MAX_INPUT];
  int pid_head, pid_tail;
  int fd[2];
  int correct_path;
  int miss = 1;
  char head_path[MAX_INPUT*2];
  char tail_path[MAX_INPUT*2];

  pipe(fd);

  int num_tokens = 0;
  t = strtok(input, " ");

  if(t == NULL || strcmp(t, "->")==0){
    printf("No command provided\n");
    return;
  }
  strncpy(command,t,MAX_INPUT);
  printf("command is: %s\n",command);
  correct_path = findPath(path, command,path_count);
  if(correct_path==-1){
    return;
  }

  while (t != NULL && num_tokens < MAX_ARG) {
    if(num_tokens == 0){
      strncpy(head_path,path[correct_path],MAX_INPUT);
      strcat(head_path,t);
      args_head[num_tokens] = head_path;
      printf("args_head: %s\n",args_head[num_tokens]);
      num_tokens++;
      t = strtok(NULL, " ");
    } else {
      // If -> is encountered
      if(strcmp(t, "->")==0){
        t = strtok(NULL, " ");
        if(t == NULL){
          printf("second command not provided\n");
          return;
        }
        miss = 0;
        args_head[num_tokens] = 0;
        break;
      }

      args_head[num_tokens] = t;
      printf("args_head: %s\n",args_head[num_tokens]);
      num_tokens++;
      t = strtok(NULL, " ");
    }
  }
  if(miss){
    printf("'->' not found\n");
    return;
  }

  strncpy(command,t,MAX_INPUT);
  printf("command 2 is: %s\n",command);
  printf("command 1 is: %s\n",args_head[0]);
  correct_path = findPath(path, command,path_count);
  if(correct_path==-1){
    return;
  }

  num_tokens = 0;
  while (t != NULL && num_tokens < MAX_ARG) {
    if(num_tokens == 0){
      strncpy(tail_path,path[correct_path],MAX_INPUT);
      printf("command head is: %s\n",args_head[0]);
      strcat(tail_path,t);
      args_tail[num_tokens] = tail_path;
      printf("args_tail: %s\n",args_tail[num_tokens]);
      num_tokens++;
      t = strtok(NULL, " ");
    } else {
      args_tail[num_tokens] = t;
      printf("args_tail: %s\n",args_tail[num_tokens]);
      num_tokens++;
      t = strtok(NULL, " ");
    }
  }
  args_tail[num_tokens] = 0;

  if ((pid_head = fork()) == 0) {
    dup2(fd[1], 1);
    close(fd[0]);
    execve(args_head[0], args_head, envp);
  }

  if ((pid_tail = fork()) == 0) {
    dup2(fd[0], 0);
    close(fd[1]);
    execve(args_tail[0], args_tail, envp);
  }

  close(fd[0]);
  close(fd[1]);

  printf("waiting for head to finish\n");
  waitpid(pid_head, &status, 0);
  printf("head is finished.\n");

  printf("waiting for tail to finish\n");
  waitpid(pid_tail, &status, 0); 
  printf("tail is finished.\n");
}

void or(char path[][MAX_INPUT],char input[],int path_count){
  int pid, fd;
  char *args[MAX_ARG+1];
  char *envp[] = {0};
  int status;
  char *t,*o;
  char command[MAX_INPUT];
  int correct_path;
  int miss = 1;

  int num_tokens = 0;
  t = strtok(input, " ");

  if(t == NULL || strcmp(t, "->")==0){
    printf("No command provided\n");
    return;
  }
  strncpy(command,t,MAX_INPUT);
  printf("command is: %s\n",command);

  correct_path = findPath(path, command,path_count);
  if(correct_path==-1){
    return;
  }
  
  while (t != NULL && num_tokens < MAX_ARG) {
    if(num_tokens == 0){
      char pre[MAX_INPUT*2];
      strncpy(pre,path[correct_path],MAX_INPUT);
      strcat(pre,t);
      args[num_tokens] = pre;
      printf("args: %s\n",args[num_tokens]);
      num_tokens++;
      t = strtok(NULL, " ");
    } else {
      // If -> is encountered
      if(strcmp(t, "->")==0){
        t = strtok(NULL, " ");
        if(t == NULL){
          printf("No ouput file provided\n");
          return;
        }
        miss = 0;
        args[num_tokens] = 0;
        break;
      }

      args[num_tokens] = t;
      printf("args: %s\n",args[num_tokens]);
      num_tokens++;
      t = strtok(NULL, " ");
    }
  }
  if(miss){
    printf("'->' not found\n");
    return;
  }
  o = t;
  printf("output file: %s\n",o);

  if((pid = fork()) ==0) {
    printf("child starting\n");
    fd = open(o, O_CREAT|O_TRUNC|O_RDWR, S_IRUSR|S_IWUSR);
    if (fd == -1) {
      fprintf(stderr, "Failed to open %s\n",o);
      exit(1);
    }
    dup2(fd, 1);
    dup2(fd, 2);
    execve(args[0], args, envp);
  }
  waitpid(pid, &status, 0);
}

int main(int argc, char *argv[]) {
  char path[MAX_PATH][MAX_INPUT];
  int path_count;
  char prompt[MAX_PROMPT];
  FILE* rc = fopen("./.sh360.rc", "r");
  
  //Read Prompt and Paths and get rid of the newline character
  path_count=0;
  char line[MAX_INPUT];
  fgets(prompt, MAX_PROMPT, rc);
  if (prompt[strlen(prompt)-1] == '\n'){
      prompt[strlen(prompt)-1] = '\0';
  }
  while(fgets(line, MAX_INPUT, rc)){
    if (line[strlen(line)-1] == '\n'){
      line[strlen(line)-1] = '\0';
    }
    //Add forward slash to end of path if it doesn't exist
    if (line[strlen(line)-1] != '/'){
      line[strlen(line)] = '/';
    }
    strncpy(path[path_count], line, MAX_INPUT);
    printf("%s\n",path[path_count]);
    path_count++;
  }

  for(int i =path_count;i < MAX_PATH;i++){
    path[i][0] = '\0';
  }

  for(;;) {  
    char input[MAX_INPUT];

    fprintf(stdout, "%s", prompt);
    fflush(stdout);
    fgets(input, MAX_INPUT, stdin);
    if (input[strlen(input) - 1] == '\n') {
      input[strlen(input) - 1] = '\0';
    }
    // get rid of leading whitespace
    char * cut = input;
    while(isspace(cut[0])) {
      cut++;
    }
  
    if(strcmp(cut, "exit")==0){
      exit(0);
    }
    printf("input: %s\n",cut);

    if(strlen(cut) > 2 && strncmp(cut, "OR ", 3) == 0){
      cut += 2;
      printf("OR MATCHED\n");
      or(path,cut,path_count);
    } else if(strlen(cut) > 2 && strncmp(cut, "PP ", 3) == 0) {
      cut += 2;
      printf("PP MATCHED\n");
      pp(path,cut,path_count);
    } else {
      single(path,cut,path_count);
    }

  }
}
