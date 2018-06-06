/*
 * Appendix B program.
 * CSC 360, Summer 2017
 *
 * In this example, fork & execve are used to create a child process
 * that runs the "ls" command with one argument given to the "ls" command
 * (i.e., "-1" which lists all of the files in a single column).
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    /* terminated by null pointers */
    char *args[] = { "/bin/ls", "-1", 0 };
    char *envp[] = { 0 };
    int pid;
    int status;

    if ((pid = fork()) == 0) {
        printf("child: about to start with pid %d...\n",pid);
        /* filename, arguements, environment variables */
        execve(args[0], args, envp);
        printf("child: SHOULDN'T BE HERE.\n");
    }

    printf("parent: waiting for child to finish...\n");
    /* parent waits for any child to finish execution and
        returns it's pid.
        Statuses address is passed in*/
    /* This while loop makes little sense
       It fails the second time around because
       there are no child processes left and it returns -1 */
    while (wait(&status) > 0) {
        printf("parent: child is finished with status %d.\n",status);
    } 
}
