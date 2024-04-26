#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

int main(int agrc, char * argv[])
{
    initClk();

    //Initiating 2 Semaphores for Scheduler Synchronization
    key_t key_id1, key_id2;
    union Semun semun;

    key_id1 = ftok("keyfile", 65);
    key_id2 = ftok("keyfile", 64);

    int sem1 = semget(key_id1, 1, 0666 | IPC_CREAT); //SEM 1 -> For Process Raise SIGSTOP
    int sem2 = semget(key_id2, 1, 0666 | IPC_CREAT); //SEM 2 -> For Process Decrement Remaining Time

    semun.val = 1;
    if (semctl(sem1, 0, SETVAL, semun) == -1)
    {
        perror("Error in semctl");
        exit(-1);
    }

    if (semctl(sem2, 0, SETVAL, semun) == -1)
    {
        perror("Error in semctl");
        exit(-1);
    }

    //TODO it needs to get the remaining time from somewhere
    
    remainingtime = atoi(argv[1]);
    int process_id = atoi(argv[2]);


    //SIGSTOP Process when first forked to move to RDY Queue



    //Process Starting RUN

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
    kill(getppid(), SIGUSR1); //sends termination signal to scheduler in order to handle it

    printf("process [%d] terminating at time [%d]\n", process_id, getClk()); //terminating ack

    destroyClk(false);

    return 0;
}

//Fork Process 
//Add semaphore to make sure process raises SIGSTOP first, Sem1 Process(UP),Sem1 Scheduler(Down) 
//TO run process -> SIGCONT
//Add semaphore to decrement the remaining time inside running process before removing from run
