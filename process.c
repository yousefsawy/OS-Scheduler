#include "headers.h"

/* Modify this file as needed*/
int remainingtime;
int process_id;
int time;
int sem1, sem2;

int main(int agrc, char * argv[])
{
    initClk();
    
    //TODO it needs to get the remaining time from somewhere
    
    remainingtime = atoi(argv[1]);
    process_id = atoi(argv[2]);


    //SIGSTOP Process when first forked to move to RDY Queue



    //Process Starting RUN

    printf("process [%d] starting at time [%d]\n", process_id, getClk()); //starting ack

    time = getClk();

    while (remainingtime > 0)
    {
        if (time != getClk())
        {
            remainingtime --;
            time = getClk();
        }
    }
    kill(getppid(), SIGUSR1); //sends termination signal to scheduler in order to handle it

    printf("process [%d] terminating at time [%d]\n", process_id, getClk()); //terminating ack

    destroyClk(false);
    exit(process_id);

    return 0;
}

