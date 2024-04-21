#include "headers.h"


int msgq_id;
int main(int argc, char * argv[])
{
    initClk();
    int processes = atoi(argv[1]);
    printf("processes");
    int processes_done = 0;
    
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

    while(true)
    {
        rec_val = msgrcv(msgq_id, &message, sizeof(message), 1, !IPC_NOWAIT);


            if (rec_val == -1)
                perror("Error in receive scheduler");
            else
                {
                    printf("\nMessage received: %d %d %d %d %d %d %d %d %d\n", message.id, message.arrival_time, message.running_time, message.priority, message.pid, message.waiting_time,
                    message.remaining_time,
                    message.finish_time,
                    message.state);
                }
        message.mtype = 2;
        send_val = msgsnd(msgq_id, &message, sizeof(message), !IPC_NOWAIT);
    }

    //delete the message queue:
    msgctl(msgq_id, IPC_RMID, (struct msqid_ds *)0);
    //upon termination release the clock resources.
    
    //destroyClk(true);
}
