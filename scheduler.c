#include "headers.h"

bool p_terminated = false;

void handler();

int main(int argc, char * argv[])
{
    initClk();

    signal(SIGCHLD, handler);

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

    int time = getClk();
    int queue_size = 0;
    int terminate_count = 0;
    bool update = true;
    while(true)
    {
        //receiving from pg

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
            enqueue_PriorityQueue(&HPF_queue, processes[message.process.id - 1], message.process.priority);
            queue_size ++;
            continue;
        }

        //hpf scheduling

    //     if (p_terminated)
    //     {
    //         processes[terminate_count] = dequeue_PriorityQueue(&HPF_queue);
    //         processes[terminate_count].finish_time = getClk();
    //         processes[terminate_count].state = TERMINATED;
    //         terminate_count ++;
    //         p_terminated = false;
    //         if (isEmpty_PriorityQueue(&HPF_queue)) {break;}
    //     }
    //     if (HPF_queue.head->process.state != RUNNING)
    //     {
    //         HPF_queue.head->process.state = RUNNING;
    //         HPF_queue.head->process.start_time = getClk();
    //         char s_running_time[5], s_id[5];
    //         sprintf(s_running_time, "%d", HPF_queue.head->process.running_time);
    //         sprintf(s_id, "%d", HPF_queue.head->process.id);
    //         int pid = fork();
    //         if (pid == -1)
    //         {
    //             perror("Error in fork");
    //             return -1;
    //         }
    //         else if (pid == 0)
    //         {
    //             execl("./process.out", "process.out", s_running_time, s_id, NULL);
    //         }
    //         else
    //         {
    //             HPF_queue.head->process.pid = pid;
    //         }
    //     }
    //     if (update) //update PCB
    //     {
    //         for (int i = 0; i < queue_size; i++)
    //         {
    //             if (processes[i].state == RUNNING) {processes[i].remaining_time --;}
    //             else if (processes[i].state == READY) {processes[i].waiting_time ++;}
    //         }

    //         update = false;
    //         printf("current time is %d\n", getClk());
    //     }
    //     if (time != getClk())
    //     {
    //         update = true;
    //         time = getClk();
    //     }
     }

    destroy_PriorityQueue(&HPF_queue);

    //upon termination release the clock resources.

    destroyClk(true);
}

void handler()
{
    p_terminated = true;
}