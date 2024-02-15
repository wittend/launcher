//======================================================================
// launcher.c
//
// Outer shell for testing spawned process access to pigpio interface
// library on Raspberry Pi.
//
// This is meant to simulate the main component in the HamSci Grape2 
// system.  As the main program uses the pigpio library with the system
// pigpio daemon shutdown, it 'becomes the daemon' and any other process
// must make calls through the pigpio_if2 library (which use sockets) to
// interact with GPIO on the local Pi.
//
//  
// Dave Witten, 2024-1-3
//======================================================================
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <pigpio.h>
#include <launcher.h>

//------------------------------------------
// Static variables
//------------------------------------------
char version[sizeof LAUNCHER_VERSION] = LAUNCHER_VERSION;
char filename[FILENAME_MAX] = "";
char program[PATH_MAX] = "";

char fifoCtrl[] = "/home/pi/PSWS/Sstat/magctl.fifo";
char fifoData[] = "/home/pi/PSWS/Sstat/magdata.fifo";
int PIPEIN  = -1;
int PIPEOUT = -1;

//---------------------------------------------------------
// Global return status 
//---------------------------------------------------------
sig_atomic_t child_process_ret_status;
sig_atomic_t child_process_term_status;
int wstatus = 0;

//---------------------------------------------------------
// int main()
//---------------------------------------------------------
int main(int argc, char *argv[])
{
    pList   ctl;
    pList   *p = &ctl;

    //-----------------------------------------
    //  Setup magnetometer parameter defaults.
    //-----------------------------------------
    if(p != NULL)
    {
        memset(p, 0, sizeof(pList));
    }

    p->outDelay        = 0;
    p->quietFlag       = 0;
    p->showParameters  = FALSE;
    p->tsMilliseconds  = 0;
    p->verboseFlag     = FALSE;
    p->usePipes        = USE_PIPES;
    p->pipeInPath      = fifoData;
    p->pipeOutPath     = fifoCtrl;
    p->Version         = LAUNCHER_VERSION;

    if(argc >  1)
    {
        fprintf(OUTPUT_PRINT, "[PARENT] Parent: %s: \n", argv[0]);
        fprintf(OUTPUT_PRINT, "[PARENT]     Child:     %s\n", argv[1]);
        fprintf(OUTPUT_PRINT, "[PARENT]     Arguments: %s\n\n", argv[2]);
        fflush(OUTPUT_PRINT);
    }
 
#if(USE_PIPES)
    //-----------------------------------------
    //  Setup the I/O pipes
    //-----------------------------------------
    int  fdPipeIn;
    int  fdPipeOut;

    if(p->usePipes == TRUE)
    {
        // Notice that fdPipeOut and fdPipeIn are intentionally reversed.
        if(!(fdPipeOut = open(p->pipeInPath, O_WRONLY | O_CREAT)))
        {
            perror("[PARENT] Open PIPE Out failed: ");
            fprintf(stderr, p->pipeInPath);
            fflush(OUTPUT_ERROR);
            exit(1);
        }
        else
        {
            fprintf(OUTPUT_ERROR, "[PARENT] Open PIPE Out OK.\n");
            fflush(OUTPUT_ERROR);
            PIPEIN = fdPipeOut;
        }

        if(!(fdPipeIn = open(p->pipeOutPath, O_RDONLY | O_CREAT)))
        {    
            perror("[PARENT] Open PIPE In failed: ");
            fprintf(OUTPUT_ERROR, p->pipeInPath);
            fflush(OUTPUT_ERROR);
            exit(1);
        }
        else
        {
            fprintf(OUTPUT_ERROR, "[PARENT] Open PIPE In OK.\n");
            fflush(OUTPUT_ERROR);
            PIPEOUT = fdPipeIn;
       }
    }

#endif // USE_PIPES

    // Initialize the pigpio  interface
    if(gpioInitialise() < 0)
    {
        // pigpio initialisation failed.
        fprintf(OUTPUT_PRINT, "\n");
        fprintf(OUTPUT_PRINT, "+--------------------------------------------------------------+\n");
        fprintf(OUTPUT_PRINT, "| [PARENT] %s: pigpio Initialization Failed!\n", argv[0]);
        fprintf(OUTPUT_PRINT, "+--------------------------------------------------------------+\n");
        fprintf(OUTPUT_PRINT, "\n");
        fflush(OUTPUT_PRINT);
        exit(1);
    }
    else
    {
        // life is good!
        fprintf(OUTPUT_PRINT, "\n");
        fprintf(OUTPUT_PRINT, "+--------------------------------------------------------------+\n");
        fprintf(OUTPUT_PRINT, "| [PARENT] %s: Now executing child processes...\n", argv[0]);
        fprintf(OUTPUT_PRINT, "+--------------------------------------------------------------+\n");
        fprintf(OUTPUT_PRINT, "\n");
        fflush(OUTPUT_PRINT);
        
        signal(SIGCHLD, child_process_ret_handle);
        signal(SIGTERM, child_process_term_handle);
        
        char *program;

        if(argv[1] == NULL)
        {
            program = "./testchild";
        }
        else
        {
            program = argv[1];
        }

        spawn(program, argv);

        //if(WIFEXITED(child_process_ret_status))
        //if(!(WIFEXITED(child_process_ret_status) && (WEXITSTATUS(child_process_ret_status) == 0))) 
        if((WIFEXITED(child_process_ret_status) && (WEXITSTATUS(child_process_ret_status) == 0))) 
        {
            fprintf(OUTPUT_PRINT, "\n");
            fprintf(OUTPUT_PRINT, "    +--------------------------------------------------------------+\n");
            fprintf(OUTPUT_PRINT, "    | [PARENT] %s: child process exited successfully with %d.\n", argv[0], WEXITSTATUS(child_process_ret_status));                
            fprintf(OUTPUT_PRINT, "    +--------------------------------------------------------------+\n");
            fprintf(OUTPUT_PRINT, "\n");
            fflush(OUTPUT_PRINT);
        }
        else
        {
            fprintf(OUTPUT_PRINT,"\n");
            fprintf(OUTPUT_PRINT, "    +--------------------------------------------------------------+\n");
            fprintf(OUTPUT_PRINT, "    | [PARENT] %s: the child process exited abnormally with %d!\n", argv[0], WEXITSTATUS(child_process_ret_status));
            fprintf(OUTPUT_PRINT, "    +--------------------------------------------------------------+\n");
            fprintf(OUTPUT_PRINT,"\n");
            fflush(OUTPUT_PRINT);
        }
        while(wait(&wstatus))
        {
            ;
        }
        gpioTerminate();

#if(USE_PIPES)
        if(p->usePipes == TRUE)
        {
            close(fdPipeOut);
            fprintf(OUTPUT_ERROR, "[PARENT] Close PIPE Out OK.\n");
            fflush(OUTPUT_ERROR);

            close(fdPipeIn);
            fprintf(OUTPUT_ERROR, "[PARENT] Close PIPE In OK.\n");
            fflush(OUTPUT_ERROR);
        }
#endif // (USE_PIPES)

        return 0;
    }
}

//---------------------------------------------------------
// void spawn()
//---------------------------------------------------------
void spawn(char *program, char *argv[])
{
    char *args[3];

    args[0] = program;
    args[1] = argv[2];
    args[2] = NULL;

//    pid_t child_pid = vfork();
    pid_t child_pid = fork();

    if(child_pid != 0)                  // Code executed by parent
    {
        fprintf(OUTPUT_PRINT, "\n");
        fprintf(OUTPUT_PRINT, "    +..............................................................+\n");
        fprintf(OUTPUT_PRINT, "    | [OLD PARENT] Parent process pid: %d\n", (int)getpid());
        fprintf(OUTPUT_PRINT, "    | child_pid: %i (i.e. > 0)\n", child_pid);
        fprintf(OUTPUT_PRINT, "    +..............................................................+\n");
        fprintf(OUTPUT_PRINT, "\n");
        fflush(OUTPUT_PRINT);
        while(1)
        {
            int next;
            int i;

            ssize_t count = read(fd, &next, sizeof(int));
            if (0 == count)
            {
                break;                  /* end of stream */
            }
        }
    }
    else                                // Code executed by child
    {
        fprintf(OUTPUT_PRINT, "\n");
        fprintf(OUTPUT_PRINT, "    +..............................................................+\n");
        fprintf(OUTPUT_PRINT, "    | [NEW CHILD] Parent process pid: %d\n", (int)getpid());
        fprintf(OUTPUT_PRINT, "    | child_pid: %i (i.e. == 0)\n", child_pid);
        fprintf(OUTPUT_PRINT, "    | Using: execvp(\"%s\", \"%s\")\n", program, args[1]);
        fprintf(OUTPUT_PRINT, "    +..............................................................+\n");
        fprintf(OUTPUT_PRINT, "\n");
        fflush(OUTPUT_PRINT);

        int rv = execv(program, args);
        if(rv == -1) 
        {
            fprintf(OUTPUT_PRINT, "    [NEW CHILD] Process did not terminate correctly. Exiting...\n");
            fflush(OUTPUT_PRINT);
            exit(1);
        }
        // Wait for child process
//        waitpid(child_pid, &wstatus, 0);
//        waitpid(child_pid, &wstatus, WNOHANG);
//        wait(&wstatus);
    }
}

//---------------------------------------------------------
// void child_process_ret_handle()
//---------------------------------------------------------
void child_process_ret_handle(int sigval)
{
    if(sigval == SIGCHLD)
    {
        fprintf(OUTPUT_PRINT, "\n");
        fprintf(OUTPUT_PRINT, "+-----------------------------------+\n");
        fprintf(OUTPUT_PRINT, "| [Parent] SIGCHLD received!\n");
        fprintf(OUTPUT_PRINT, "+-----------------------------------+\n");
        fprintf(OUTPUT_PRINT, "\n");
        fflush(stdout);
        wait(&child_process_ret_status);
    }
}

//---------------------------------------------------------
// void child_process_term_handle()
//---------------------------------------------------------
void child_process_term_handle(int sigval)
{
    if(sigval == SIGTERM)
    {
        fprintf(OUTPUT_PRINT, "\n");
        fprintf(OUTPUT_PRINT, "+-----------------------------------+\n");
        fprintf(OUTPUT_PRINT, "| [Parent] SIGTERM received!\n");
        fprintf(OUTPUT_PRINT, "+-----------------------------------+\n");
        fprintf(OUTPUT_PRINT, "\n");
        fflush(stdout);
//        wait(&child_process_ret_status);
    }
}
