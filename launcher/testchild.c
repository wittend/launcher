//======================================================================
// testchild.c
//
// Child executable for testing spawned process access to pigpio interface
// library on Raspberry Pi.
//
// This is meant to simulate the main component in the HamSci Grape2 
// system.  As the main program uses the pigpio library with the system
// pigpio daemon shutdown, it 'becomes the daemon' and any other process
// must make calls through the pigpio_if2 library (which use sockets or 
// pipes) to interact with GPIO on the local Pi.
//
//  
// Dave Witten, 2024-1-9
//======================================================================
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
//#include <pigpio.h>
#include <pigpiod_if2.h>

//------------------------------------------
// Macros
//------------------------------------------
#define BCM_GPIO_PIN    27
#define OUTPUT_PRINT    stdout

//------------------------------------------
// Static variables
//------------------------------------------
int killflag = 0;

//------------------------------------------
// prototypes
//------------------------------------------
void onEdge(void);
void sigHandler(int signo);
int main(int argc, char *argv[]);

//---------------------------------------------------------
// int main()
//---------------------------------------------------------
int main(int argc, char *argv[])
{
    int pi = 0;
    int rv = 0;
    unsigned edge_cb_id = 0;
    
    fprintf(OUTPUT_PRINT, "\nIn %s child process.\n", argv[0]);

    // setup simple ^C handler
    signal(SIGABRT, sigHandler);
  
    // Start interaction with pigpio provider.
    fprintf(OUTPUT_PRINT, "Starting pigpio.\n");
    if((pi = pigpio_start(NULL, NULL) < 0))
    {
        fprintf(OUTPUT_PRINT, "pigpio initialization failed!\n");
        exit(pi);
    }
    else
    {
        // setup rising edge detection handler
        fprintf(OUTPUT_PRINT, "Setting child pin: %i mode to: %i.\n", BCM_GPIO_PIN, PI_INPUT);
        if((rv = set_mode(pi, (unsigned) BCM_GPIO_PIN, PI_INPUT) == 0))
        {
            fprintf(OUTPUT_PRINT, "Setting up edge detection callback.\n\n");
            if((edge_cb_id = callback(pi, (unsigned) BCM_GPIO_PIN, RISING_EDGE, (CBFunc_t) onEdge)) == 0)       // RISING_EDGE, FALLING_EDGE, or EITHER_EDGE
            {
                // Loop until SIGABRT
                while(1)
                {
                    sleep(1);
                    if(killflag)
                    {
                        break;
                    }
                }
                fprintf(OUTPUT_PRINT, "Ending child process.\n");
                fflush(OUTPUT_PRINT);
            }
            else
            {
                fprintf(OUTPUT_PRINT, "Failed to get callback for pin %i. Return: %i\n", BCM_GPIO_PIN, edge_cb_id);
                fflush(OUTPUT_PRINT);
                exit(rv);
            }
        }
        else
        {
            fprintf(OUTPUT_PRINT, "Failed to set mode of GPIO pin %i. Return: %i\n", BCM_GPIO_PIN, rv);
            fflush(OUTPUT_PRINT);
            exit(rv);
        }
    }
    // Cleanup event handlers.
    rv = event_callback_cancel(edge_cb_id);
    pigpio_stop(pi);
    exit(rv);
}

//---------------------------------------------------------------
// void onEdge(void)
//---------------------------------------------------------------
void onEdge(void)
{
    fputs("_|\0x250C\0x2510|_", OUTPUT_PRINT);
    fflush(OUTPUT_PRINT);
}

//---------------------------------------------------------------
// void sigHandler(void)
//---------------------------------------------------------------
void sigHandler(int signo)
{
    if(signo == SIGABRT)
    {
        fputs("received SIGINT\n", OUTPUT_PRINT);
        fflush(OUTPUT_PRINT);
        killflag = 1;
    }
}
