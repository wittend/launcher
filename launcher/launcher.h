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
#ifndef SWXLAUNCHER_h
#define SWXLAUNCHER_h
#include <stdint.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <memory.h>

//---------------------------------------------------------------
// macros
//---------------------------------------------------------------
#define FALSE               0
#define TRUE                1

#define _DEBUG              0

#define USE_PIPES           TRUE

#define LAUNCHER_VERSION    "0.0.1"
#define UTCBUFLEN           64
#define MAXPATHBUFLEN       1025
#define SITEPREFIXLEN       32
#define TIMEOUT             10000.00            // double 10 seconds

#define OUTPUT_PRINT        stdout
#define OUTPUT_ERROR        stderr

//------------------------------------------
// Parameter List struct
//------------------------------------------
typedef struct tag_pList
{
    int  outDelay;
    int  quietFlag;
    int  showParameters;
    int  tsMilliseconds;
    int  verboseFlag;
    int  usePipes;
    char *pipeInPath;
    char *pipeOutPath;
    char *Version;
} pList;

//------------------------------------------
// Prototypes
//------------------------------------------
int  main(int argc, char *argv[]);
void child_process_ret_handle(int sigval);
void child_process_term_handle(int sigval);
void spawn(char *program, char *argv[]);
void onEdge(void);

#endif // SWXLAUNCHER_h
