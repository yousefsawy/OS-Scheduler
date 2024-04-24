#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

int main(int agrc, char * argv[])
{
    initClk();

    //TODO it needs to get the remaining time from somewhere
    
    remainingtime = atoi(argv[1]);
    int process_id = atoi(argv[2]);

    printf("process [%d] starting at time [%d]\n", process_id, getClk()); //starting ack

    int time = getClk();
    while (remainingtime > 0)
    {
        if (time != getClk())
        {
            remainingtime --;
            time = getClk();
        }
    }

    kill(getppid(), SIGCHLD); //sends termination signal to scheduler in order to handle it

    printf("process [%d] terminating at time [%d]\n", process_id, getClk()); //terminating ack

    destroyClk(false);

    return 0;
}
