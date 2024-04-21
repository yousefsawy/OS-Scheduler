#include "headers.h"


int msgq_id;
int main(int argc, char * argv[])
{

    initClk();
    int process_Count = atoi(argv[1]);
    int processes_done = 0;

    PCB Processes[process_Count];

    SchedulingALgo Algo = atoi(argv[2]);

    
    //TODO implement the scheduler :)
    key_t key_id;
    int rec_val, send_val;

    key_id = ftok("processes.txt", 65);               //create unique key
    msgq_id = msgget(key_id, 0666 | IPC_CREAT); //create message queue and return id

    if (msgq_id == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    printf("Message Queue ID = %d\n", msgq_id);

    struct msgbuff message;

    int i = 0;
    while(processes_done < process_Count)
    {
        rec_val = msgrcv(msgq_id, &message, sizeof(message.Process), 1, !IPC_NOWAIT);


            if (rec_val == -1)
                perror("Error in receive scheduler");
            else
                {
                    printf("\nMessage received: %d %d %d %d %d %d %d %d %d\n", message.Process.id, message.Process.arrival_time, message.Process.running_time, message.Process.priority, message.Process.pid, message.Process.waiting_time,
                    message.Process.remaining_time,
                    message.Process.finish_time,
                    message.Process.state);

                            //Initalize the PCB
                            Processes[i]= message.Process;
                }
        processes_done++;
    }

    
    //delete the message queue:
    msgctl(msgq_id, IPC_RMID, (struct msqid_ds *)0);
    printf("Message Queue terminated\n");
    //upon termination release the clock resources.
    
    destroyClk(true);
}
