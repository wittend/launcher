//=========================================================================
// launcher.c
//
// Simulator to test launching and communication with Raspberry Pi 3/4
// programs that use the pigpio library.
//
// Author:      David Witten, KD0EAG
// Date:        January 3, 2024
// License:     GPL 3.0
// Note:        
//              
//              
//=========================================================================
#include "launcher.h"

extern char version[];

//------------------------------------------
// currentTimeMillis()
//------------------------------------------
long currentTimeMillis()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    return time.tv_sec * 1000 + time.tv_usec / 1000;
}

//------------------------------------------
// getUTC()
//------------------------------------------
struct tm *getUTC()
{
    time_t now = time(&now);
    if(now == -1)
    {
        puts("The time() function failed");
    }
    struct tm *ptm = gmtime(&now);
    if(ptm == NULL)
    {
        puts("The gmtime() function failed");
    }
    return ptm;
}

//------------------------------------------
// showSettings()
//------------------------------------------
void showSettings(pList *p)
{
    //char pathStr[128] = "";
    fprintf(stdout, "\nVersion = %s\n", LAUNCHER_VERSION);

//#if (USE_PIPES)
    fprintf(stdout, "   Log output to pipes:                        %s\n",          p->usePipes ? "TRUE" : "FALSE");
    fprintf(stdout, "   Input file path:                            %s\n",          p->pipeInPath);
    fprintf(stdout, "   Output file path:                           %s\n",          p->pipeOutPath);
//#endif
//    fprintf(stdout, "   Built in self test (BIST) value:            %02X (hex)\n",  p->doBistMask);
    fprintf(stdout, "   Show parameters:                            %s\n",          p->showParameters   ? "TRUE" : "FALSE");
    fprintf(stdout, "   Quiet mode:                                 %s\n",          p->quietFlag        ? "TRUE" : "FALSE");
    fprintf(stdout, "\n\n");
}

//------------------------------------------
// getCommandLine()
//------------------------------------------
int getCommandLine(int argc, char** argv, pList *p)
{
    int c;
//    int NOSval = 0;
//    int magAddr = 0;
//    int lTmpAddr = 0;
//    int rTmpAddr = 0;

    if(p != NULL)
    {
        memset(p, 0, sizeof(pList));
    }
    p->Version = version;

    while((c = getopt(argc, argv, "?jPqvV")) != -1)
    {
         switch(c)
        {
            case 'P':
                p->showParameters = TRUE;
                break;
            case 'q':
                p->quietFlag = TRUE;
                p->verboseFlag = FALSE;
                break;
            case 'V':
                fprintf(stdout, "\nVersion: %s\n", p->Version);
                exit(0);
                break;
            case 'v':
                p->verboseFlag = TRUE;
                p->quietFlag = FALSE;
                break;
            case 'h':
            case '?':
                fprintf(stdout, "\n%s Version = %s\n", argv[0], version);
                fprintf(stdout, "\nParameters:\n\n");
                fprintf(stdout, "   -P                     :  Show Parameters.\n");
                fprintf(stdout, "   -q                     :  Quiet mode.                           [ partial ]\n");
                fprintf(stdout, "   -v                     :  Verbose output.\n");
                fprintf(stdout, "   -V                     :  Display software version and exit.\n");
                fprintf(stdout, "   -h or -?               :  Display this help.\n\n");
                return 1;
                break;
            default:
                fprintf(stdout, "\n?? getopt returned character code 0x%2X ??\n", c);
                break;
        }
    }
    if(optind < argc)
    {
        printf("non-option ARGV-elements: ");
        while(optind < argc)
        {
            printf("%s ", argv[optind++]);
        }
        printf("\n");
    }
    return 0;
}
