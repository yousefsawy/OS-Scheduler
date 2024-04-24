#include "headers.h"


int main(int argc, char * argv[])
{
    initClk();

    //Reading file parameters
    int processes_count = atoi(argv[1]);
    int algo = atoi(argv[2]);
    int quantum = atoi(argv[3]);

    //Some instantiations
    PCB processes[processes_count];
    for (int i = 0; i < processes_count; i++)
    {
        processes[i].id = 0;
        processes[i].arrival_time = 0;
        processes[i].running_time = 0;
        processes[i].priority = 0;
        processes[i].pid = 0;
        processes[i].waiting_time = 0;
        processes[i].remaining_time = 0;
        processes[i].finish_time = 0;
        processes[i].state = UNKNOWN;
    }
    msgbuff message;
    message.mtype = getppid();

    //HPF queue
    PriorityQueue HPF_queue;
    init_PriorityQueue(&HPF_queue);

    //Initiating Message Queue

    key_t key_pg_s = ftok("keyfile", 1);
    int pg_s_id = msgget(key_pg_s, IPC_CREAT | 0666);
    if (pg_s_id == -1)
    {
        perror("Error in creating pg_s msg queue");
        return -1;
    }

    //TODO implement the scheduler :)

    while(true)
    {
        int rec_val = msgrcv(pg_s_id, &message, sizeof(message.process), message.mtype, IPC_NOWAIT);
        if (rec_val == -1)
        {
            if (errno == ENOMSG) {} //continue code normally
            else
            {
                perror("Error in receiving pg_s");
                exit(-1);
            }
        }
        else
        {
            printf("process[%d] arrived at time %d\n", message.process.id, getClk());
            message.process.state = READY;
            processes[message.process.id - 1] = message.process;
        }

        if (algo == 1) //HPF
        {
            enqueue_PriorityQueue(&HPF_queue, message.process, message.process.priority);
        }
        else if (algo == 2) //SRTN
        {

        }
        else //RR
        {

        }
    }

    //upon termination release the clock resources.

    destroyClk(true);
}
