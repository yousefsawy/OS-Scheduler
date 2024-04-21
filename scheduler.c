#include "headers.h"


int main(int argc, char * argv[])
{
    initClk();

    //Reading file parameters
    int processes_count = atoi(argv[1]);
    int algo = atoi(argv[2]);
    int quantum = atoi(argv[3]);
    PCB process;

    //Initiating Message Queue

    key_t key_pg_s = ftok("keyfile", 1);
    int pg_s_id = msgget(key_pg_s, IPC_CREAT | 0666);
    if (pg_s_id == -1)
    {
        perror("Error in creating pg_s msg queue");
        return -1;
    }
    printf("PGenerator-Scheduler Message Queue ID = %d\n", pg_s_id);

    while(true)
    {
        int rec_val = msgrcv(pg_s_id, &process, sizeof(process), 0, !IPC_NOWAIT);
        if (rec_val == -1)
        {
            perror("Error in receiving pg_s");
            exit(-1);
        }
        else
        {
            printf("process[%d] arrived at time %d\n", process.id, getClk());
        }
    }

    //TODO implement the scheduler :)
    //upon termination release the clock resources.

    destroyClk(true);
}
