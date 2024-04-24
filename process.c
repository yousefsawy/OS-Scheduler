#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

int main(int agrc, char * argv[])
{
    initClk();

    //TODO it needs to get the remaining time from somewhere
    remainingtime = atoi(argv[1]);
    int time = getClk();
    printf("process %d starting at time %d\n", atoi(argv[2]), getClk());
    while (remainingtime > 0)
    {
        if (time != getClk())
        {
            remainingtime --;
            time = getClk();
        }
    }

    kill(getppid(), SIGCHLD);

    printf("process %d terminating at time %d\n", atoi(argv[2]), getClk());

    destroyClk(false);

    return 0;
}
