#include "headers.h"

bool p_terminated = false; //flag for indicating that a process terminated in current timestep

void handler();

int main(int argc, char * argv[])
{
    initClk();

    signal(SIGUSR1, handler);

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

    //Initiating 2 Semaphores for process synch
    key_t key_id1, key_id2;
    union Semun semun;

    key_id1 = ftok("keyfile", 65);
    key_id2 = ftok("keyfile", 64);

    int sem1 = semget(key_id1, 1, 0666 | IPC_CREAT); //SEM 1 -> For client write
    int sem2 = semget(key_id2, 1, 0666 | IPC_CREAT); //SEM 2 -> For Server Read

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

    //TODO implement the scheduler :)

    /////////////////////// flags and counters for sync /////////////////////
    int time = getClk();                                                   //
    int queue_size = 0;                                                    //
    int terminate_count = 0;                                               // add more if needed
    bool update = true;                                                    //
    bool CPU_available = true;                                             //
    /////////////////////////////////////////////////////////////////////////
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
            printf("process[%d] arrived\n", message.process.id);
            message.process.state = READY;
            processes[message.process.id - 1] = message.process; //TO BE DELETED
            if (algo == 1) //HPF
            {
                enqueue_PriorityQueue(&HPF_queue, processes[message.process.id - 1], message.process.priority);
                queue_size ++;
            }

            //For & make sure process is Stopped before continuing code
            char s_running_time[5], s_id[5];
            sprintf(s_running_time, "%d", HPF_queue.head->process.running_time);
            sprintf(s_id, "%d", HPF_queue.head->process.id);
            int pid = fork();
            if (pid == -1)
            {
                perror("Error in fork");
                return -1;
            }
            else if (pid == 0)
            {
                execl("./process.out", "process.out", s_running_time, s_id, NULL);
            }
            kill(pid,SIGSTOP);
            processes[message.process.id - 1].pid = pid;

            continue;
        }

        if (update)
        {
            printf("current time is [%d]\n", getClk());
        }

        usleep(200000); //sleeps for 0.2 seconds

        if (algo == 1) //HPF
        {
            if (update && p_terminated) //at first check if a process terminated
            {
                processes[terminate_count].finish_time = getClk();
                processes[terminate_count].state = TERMINATED;
                terminate_count ++;
                p_terminated = false;
                CPU_available = true;
                if (terminate_count == processes_count) {break;}
            }
            if (update && CPU_available && !isEmpty_PriorityQueue(&HPF_queue)) //then check if there are no processes running
            {
                HPF_queue.head->process.state = RUNNING;
                HPF_queue.head->process.start_time = getClk();
                kill(HPF_queue.head->process.pid, SIGCONT);
                processes[terminate_count] = dequeue_PriorityQueue(&HPF_queue);
                CPU_available = false;
            }
        }
        
        if(algo == 2) //SRTN
        {
            
        } 

        usleep(200000); //sleeps for 0.2 seconds

        //update PCB blocks
        if (update)
        {
            for (int i = 0; i < queue_size; i++)
            {
                if (processes[i].state == RUNNING) {processes[i].remaining_time --;}
                else if (processes[i].state == READY) {processes[i].waiting_time ++;}
            }

            printf("============================\n");
            update = false;
        }

        //handles the update flag in order to control operations which are done only once every timestep
        if (time != getClk())
        {
            update = true;
            time = getClk();
        }
     }

    destroy_PriorityQueue(&HPF_queue);

    //upon termination release the clock resources.

    destroyClk(true);
}

void handler()
{
    p_terminated = true;
}