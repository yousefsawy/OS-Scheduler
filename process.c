#include "headers.h"

/* Modify this file as needed*/
int remainingtime;
int process_id;
int time;
int sem1, sem2;

void handler();

int main(int agrc, char * argv[])
{
    remainingtime = atoi(argv[1]);
    process_id = atoi(argv[2]);
    initClk();
    signal(SIGUSR2, handler);
    //TODO it needs to get the remaining time from somewhere
    


    //SIGSTOP Process when first forked to move to RDY Queue



    //Process Starting RUN



    time = getClk();
    int rem = remainingtime;
    while (remainingtime > 0)
    {
        if(rem != remainingtime)
        {
            rem = remainingtime;
        }
    }
    kill(getppid(), SIGUSR1); //sends termination signal to scheduler in order to handle it

    printf("process [%d] terminating at time [%d]\n", process_id, getClk()); //terminating ack

    destroyClk(false);
    exit(process_id);

    return 0;
}

void handler()
{
    remainingtime--;
    signal(SIGUSR2, handler);
}