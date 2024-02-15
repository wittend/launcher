//============================================================
// exec_and_pipe.c
//
// example code from:
//    Stack Overflow -
//      exec() and pipe() between child process in C.
//============================================================
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    char *param1[] = {"ls", NULL};
    char *param2[] = {"wc", NULL};

    int p[2];
    pipe(p);

    pid_t process1;
    pid_t process2;

    // Create child 1
    //--------------------------
    if((process1 = fork()) == 0)
    {
        dup2(p[1], 1);           // redirect stdout to pipe
        close(p[0]);
        execvp("ls", param1);
        perror("execvp ls failed");
    }
    else if(process1 == -1)
    {
        // fork failed
        printf("\n1st fork() failed.\n");
        exit(1);
    }
    close(p[1]);                // no need for writing in the parent

    // Create child 2
    //--------------------------
    if((process2 = fork()) == 0)
    {
        dup2(p[0],0);           // get stdin from pipe

        char buff[1000] = {0};
        read(STDIN_FILENO, buff, 250);
        printf("---- in process2 -----\nls:\n%s\n", buff);
    }
    else if(process2 == -1)
    {
        // second fork failed
        printf("\n2nd fork() failed.\n");
        close(p[0]);            // ensure there is no reader to the pipe
        wait(NULL);             // wait for first chidren
        exit(1);
    }
    close(p[0]);                // no need for reading in the parent

    wait(NULL);
    wait(NULL);                 // wait for the two children
}
