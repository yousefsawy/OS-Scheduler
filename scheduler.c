#include "headers.h"
#include <math.h>

bool p_terminated = false; // flag for indicating that a process terminated in current timestep
int p_terminated_id = -1;
void handler();
int ShortestRemaining(PCB *Processes, int count, int *index);
int main(int argc, char *argv[])
{
    char LogStr[10000] = "";

    // Opening log file
    FILE *fileLog = fopen("scheduler.log", "w");
    if (fileLog == NULL)
    {
        printf("Error opening file!\n");
        return 1;
    }

    initClk();

    signal(SIGUSR1, handler);



    // Reading file parameters
    int Process_Arrival[atoi(argv[1])];
    int processes_count = atoi(argv[1]);
    int algo = atoi(argv[2]);
    int quantum = atoi(argv[3]);
    char* p_arrival = argv[4];


    for(int i = 0; i < processes_count; i++)
    {
        sscanf(p_arrival, "%d", &(Process_Arrival[i]));

        while (*p_arrival && *p_arrival != ' ')
            p_arrival++;
        p_arrival++;
    }

    for(int i = 0; i < processes_count; i++)
    {
        printf("%d \n", Process_Arrival[i]);;
    }

    // Some instantiations
    PCB processes[processes_count];
    printf("Prcs count %d", processes_count);
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
        processes[i].start_time = 0;
    }
    msgbuff message;
    message.mtype = getppid();

    // HPF queue
    PriorityQueue HPF_queue;
    init_PriorityQueue(&HPF_queue);

    // RR queue
    CircularQueue RR_queue;
    init_CircularQueue(&RR_queue);
    CQ_Node *RR_current_process = NULL;
    int RR_current_quantum = quantum;

    // Initiating Message Queue

    key_t key_pg_s = ftok("keyfile", 1);
    int pg_s_id = msgget(key_pg_s, IPC_CREAT | 0666);
    if (pg_s_id == -1)
    {
        perror("Error in creating pg_s msg queue");
        return -1;
    }

    // TODO implement the scheduler :)

    /////////////////////// flags and counters for sync /////////////////////
    int time = getClk(); 
    int idleTime = 0;      //
    int totalTime = 0;
    int queue_size = 0;        //
    int terminate_count = 0;   // add more if needed
    bool update = true;        //
    bool CPU_available = true; //
    int Process_arrived = 0;   //
    int running_pid = -1;      //
    int received = 0;          //
    int stalled = 0;
    int *index = (int *)malloc(sizeof(int));
    *index = 0; //
    /////////////////////////////////////////////////////////////////////////
    while (true)
    {
        // receiving from pg
        if(Process_Arrival[Process_arrived] == getClk() && !received)
        {
            usleep(200000); // sleeps for 0.2 seconds
        }

        int rec_val = msgrcv(pg_s_id, &message, sizeof(message.process), message.mtype, IPC_NOWAIT);


        if (rec_val == -1)
        {
            if (errno == ENOMSG)
            {
            } // continue code normally
            else
            {
                perror("Error in receiving pg_s");
                exit(-1);
            }
        }
        else
        {
            received = 1;
            printf("process[%d] arrived\n", message.process.id);
            Process_arrived++;
            message.process.state = READY;
            processes[message.process.id - 1] = message.process;

            // For & make sure process is Stopped before continuing code
            char s_running_time[5], s_id[5];
            sprintf(s_running_time, "%d", message.process.running_time);
            sprintf(s_id, "%d", message.process.id);
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

            processes[message.process.id - 1].pid = pid;

            if (algo == 1) // HPF
            {
                enqueue_PriorityQueue(&HPF_queue, processes[message.process.id - 1], message.process.priority);
                queue_size++;
            }
            else if (algo == 3) // RR
            {
                enqueue_CircularQueue(&RR_queue, processes[message.process.id - 1]);
                queue_size++;
            }
            continue;
        }


        if (update)
        {
            printf("current time is [%d]\n", getClk());
            totalTime++;
            usleep(200000); // sleeps for 0.2 seconds
        }

        

        if (algo == 1) // HPF
        {
            if (update && p_terminated) // at first check if a process terminated
            {
                processes[p_terminated_id-1].finish_time = getClk();
                processes[p_terminated_id-1].state = TERMINATED;
                terminate_count++;
                p_terminated = false;
                CPU_available = true;
                int TA = (processes[p_terminated_id-1].finish_time - processes[p_terminated_id-1].arrival_time);
                int WTA = TA / processes[p_terminated_id - 1].running_time;
                char line[100];
                sprintf(line, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %d\n", getClk(), p_terminated_id, processes[p_terminated_id - 1].arrival_time, processes[p_terminated_id - 1].running_time, 0, processes[p_terminated_id - 1].waiting_time, TA, WTA);
                strcat(LogStr, line);
                if (terminate_count == processes_count)
                {
                    break;
                }
            }

            if (update && CPU_available && !isEmpty_PriorityQueue(&HPF_queue)) // then check if there are no processes running
            {
                int P_id;
                P_id = HPF_queue.head->process.id;
                HPF_queue.head->process.state = RUNNING;
                HPF_queue.head->process.start_time = getClk();
                HPF_queue.head->process.waiting_time = processes[P_id].waiting_time;
                kill(HPF_queue.head->process.pid, SIGCONT);
                processes[P_id] = dequeue_PriorityQueue(&HPF_queue);
                CPU_available = false;

                char line[100];
                sprintf(line, "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk(), P_id, processes[P_id].arrival_time, processes[P_id].running_time, processes[P_id].remaining_time, processes[P_id].waiting_time);
                strcat(LogStr, line);
            }

            if(update && CPU_available)
            {
                idleTime++;
            }
        }
        else if (algo == 2) // SRTN
        {
            if (update && p_terminated) // Terminated
            {
                processes[p_terminated_id - 1].finish_time = getClk();
                processes[p_terminated_id - 1].state = TERMINATED;
                terminate_count++;
                p_terminated = false;
                running_pid = -1;

                // Writing to log Terminated
                int TA = (processes[p_terminated_id - 1].finish_time - processes[p_terminated_id - 1].arrival_time);
                int WTA = TA / processes[p_terminated_id - 1].running_time;

                char line[100];
                sprintf(line, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %d\n", getClk(), p_terminated_id, processes[p_terminated_id - 1].arrival_time, processes[p_terminated_id - 1].running_time, 0, processes[p_terminated_id - 1].waiting_time, TA, WTA);
                strcat(LogStr, line);
                if (terminate_count == processes_count)
                {
                    break;
                }
            }

            if (update)
            {
                int prev_pid = running_pid;
                int prev_index = *index;
                running_pid = ShortestRemaining(processes, Process_arrived, index);
                printf("run pid %d\n",running_pid);

                if (prev_pid != -1 && running_pid != prev_pid) // Stopped
                {
                    processes[prev_index].state = READY;

                    char line[100];
                    sprintf(line, "At time %d process %d stopped arr %d total %d remain %d wait %d\n", getClk(), prev_index + 1, processes[prev_index].arrival_time, processes[prev_index].running_time, processes[prev_index].remaining_time, processes[prev_index].waiting_time);
                    strcat(LogStr, line);
                }

                received = 0;
                if (running_pid != -1 && running_pid != prev_pid)
                {
                    processes[*index].state = RUNNING;
                    if (processes[*index].start_time == 0) // Started
                    {
                        processes[*index].start_time = getClk();
                        char line[100];
                        printf("process [%d] starting at time [%d]\n", *index + 1, getClk()); //starting ack
                        sprintf(line, "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk(), *index + 1, processes[*index].arrival_time, processes[*index].running_time, processes[*index].remaining_time, processes[*index].waiting_time);
                        strcat(LogStr, line);
                    }
                    else // Resumed
                    {
                        char line[100];
                        sprintf(line, "At time %d process %d resumed arr %d total %d remain %d wait %d\n", getClk(), *index + 1, processes[*index].arrival_time, processes[*index].running_time, processes[*index].remaining_time, processes[*index].waiting_time);
                        strcat(LogStr, line);
                    }
                }

                if(running_pid == -1)
                {
                    idleTime++;
                }
                else 
                {
                    kill(running_pid,SIGSTOP);
                    kill(running_pid,SIGUSR2);
                    kill(running_pid,SIGCONT);
                    printf("here 281\n");
                }
            }
        }
        else if (algo == 3) // RR
        {
            if (update)
            {
                if (RR_current_process)
                {
                    RR_current_quantum--;
                    RR_current_process->process.remaining_time--;
                }

                if (p_terminated) // TERMINATED
                {
                    RR_current_process->process.finish_time = getClk();
                    RR_current_process->process.state = TERMINATED;
                    processes[terminate_count] = RR_current_process->process;
                    terminate_count++;
                    p_terminated = false;

                    int terminatedPID = RR_current_process->process.pid;
                    RR_current_process = RR_current_process->next;
                    RR_current_quantum = quantum;
                    deleteNode_CircularQueue(&RR_queue, terminatedPID);

                    if (isEmpty_CircularQueue(&RR_queue))
                    {
                        init_CircularQueue(&RR_queue);
                        RR_current_process = NULL;
                    }

                    int TA = (processes[p_terminated_id - 1].finish_time - processes[p_terminated_id - 1].arrival_time);
                    int WTA = TA / processes[p_terminated_id - 1].running_time;

                    char line[100];
                    sprintf(line, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %d\n", getClk(), p_terminated_id, processes[p_terminated_id - 1].arrival_time, processes[p_terminated_id - 1].running_time, 0, processes[p_terminated_id - 1].waiting_time, TA, WTA);
                    strcat(LogStr, line);
                    if (terminate_count == processes_count)
                    {
                        break;
                    }
                }

                if (!isEmpty_CircularQueue(&RR_queue) && RR_current_process == NULL) // First process to run
                {
                    RR_current_process = RR_queue.front;
                    RR_current_quantum = quantum;
                    RR_current_process->process.start_time = getClk();
                    RR_current_process->process.state = RUNNING;
                    kill(RR_current_process->process.pid, SIGCONT); // Started
                    printf("Process %d first run\n", RR_current_process->process.id);

                    char line[100];
                    sprintf(line, "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk(), RR_current_process->process.id, RR_current_process->process.arrival_time, RR_current_process->process.running_time, RR_current_process->process.remaining_time, RR_current_process->process.waiting_time);
                    strcat(LogStr, line);
                }
                else if (RR_current_quantum == 0 && RR_current_process) // Process has finished its allowed time
                {
                    RR_current_process->process.state = READY;
                    kill(RR_current_process->process.pid, SIGSTOP); // Stopped
                    printf("Process %d stoped\n", RR_current_process->process.id);

                    char line[100];
                    sprintf(line, "At time %d process %d stoped arr %d total %d remain %d wait %d\n", getClk(), RR_current_process->process.id, RR_current_process->process.arrival_time, RR_current_process->process.running_time, RR_current_process->process.remaining_time, RR_current_process->process.waiting_time);
                    strcat(LogStr, line);

                    RR_current_process = RR_current_process->next;
                    RR_current_process->process.state = RUNNING;
                    RR_current_quantum = quantum;
                    if (RR_current_process->process.start_time == 0) // Started
                    {
                        RR_current_process->process.start_time = getClk();

                        char line[100];
                        sprintf(line, "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk(), RR_current_process->process.id, RR_current_process->process.arrival_time, RR_current_process->process.running_time, RR_current_process->process.remaining_time, RR_current_process->process.waiting_time);
                        strcat(LogStr, line);
                    }
                    else // Resumed
                    {
                        char line[100];
                        sprintf(line, "At time %d process %d resumed arr %d total %d remain %d wait %d\n", getClk(), RR_current_process->process.id, RR_current_process->process.arrival_time, RR_current_process->process.running_time, RR_current_process->process.remaining_time, RR_current_process->process.waiting_time);
                        strcat(LogStr, line);
                    }
                    kill(RR_current_process->process.pid, SIGCONT);
                    printf("process %d continue after a process stoped\n", RR_current_process->process.id);
                }
                else if (RR_current_process && RR_current_quantum == quantum) // process after terminated one
                {
                    RR_current_process->process.state = RUNNING;
                    RR_current_quantum = quantum;
                    if (RR_current_process->process.start_time == 0) // Started
                    {
                        RR_current_process->process.start_time = getClk();

                        char line[100];
                        sprintf(line, "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk(), RR_current_process->process.id, RR_current_process->process.arrival_time, RR_current_process->process.running_time, RR_current_process->process.remaining_time, RR_current_process->process.waiting_time);
                        strcat(LogStr, line);
                    }
                    else // Resumed
                    {
                        char line[100];
                        sprintf(line, "At time %d process %d resumed arr %d total %d remain %d wait %d\n", getClk(), RR_current_process->process.id, RR_current_process->process.arrival_time, RR_current_process->process.running_time, RR_current_process->process.remaining_time, RR_current_process->process.waiting_time);
                        strcat(LogStr, line);
                    }
                    kill(RR_current_process->process.pid, SIGCONT);
                    printf("process %d continue after a process terminated\n", RR_current_process->process.id);
                }
            }
        }

        if(update && algo == 3 && isEmpty_CircularQueue(&RR_queue))
        {
            idleTime++;
        }

        //usleep(200000); // sleeps for 0.2 seconds

        // update PCB blocks
        if (update)
        {
            if (algo == 1 || algo == 2)
            {
                for (int i = 0; i < processes_count; i++)
                {
                    if (processes[i].state == RUNNING)
                    {
                        processes[i].remaining_time--;
                    }
                    else if (processes[i].state == READY)
                    {
                        processes[i].waiting_time++;
                    }
                }
            }
            else if (algo == 3 && RR_current_process)
            {
                CQ_Node *current = RR_queue.front;
                if (current->process.state == READY)
                {
                    current->process.waiting_time++;
                }
                current = current->next;
                while (current != RR_queue.front)
                {
                    if (current->process.state == READY)
                    {
                        current->process.waiting_time++;
                    }
                    current = current->next;
                }
            }

            printf("============================\n");
            
            if (received)
            {
                if(Process_Arrival[Process_arrived-1] != getClk())
                {
                    received = 0;
                }
            }

            update = false;
        }

        // handles the update flag in order to control operations which are done only once every timestep
        if (time != getClk())
        {
            update = true;
            time = getClk();
        }
    }

    destroy_PriorityQueue(&HPF_queue);
    destroy_CircularQueue(&RR_queue);

    // Performance file
    float avgWTA = 0, avgWaiting = 0, stdWTA = 0;
    float utilization = (1 - (float)idleTime/totalTime) * 100;

    for (int i = 0; i < processes_count; i++)
    {
        avgWaiting += processes[i].waiting_time;
        avgWTA += (processes[i].finish_time - processes[i].arrival_time) / (float)processes[i].running_time;
    }
    
    avgWTA /= processes_count;

    for (int i = 0; i < processes_count; i++)
    {
        float WTA_ = (processes[i].finish_time - processes[i].arrival_time) / (float)processes[i].running_time;
        stdWTA += pow(WTA_ - avgWTA, 2);
    }

    stdWTA /= processes_count;
    stdWTA = sqrt(stdWTA);

    avgWaiting /= processes_count;

    FILE *file = fopen("scheduler.perf", "w");
    if (file == NULL)
    {
        printf("Error opening file!\n");
        return 1;
    }
    fprintf(file, "CPU utilization = %.f%%\n", utilization);
    fprintf(file, "Avg WTA = %.2f\n", avgWTA);
    fprintf(file, "Avg Waiting = %.2f\n", avgWaiting);
    fprintf(file, "Std WTA = %.2f\n", stdWTA);
    fclose(file);

    //WRITING LOG FILE
    fprintf(fileLog, "%s", LogStr);
    fclose(fileLog);

    // upon termination release the clock resources.

    destroyClk(true);
}

void handler()
{
    wait(&p_terminated_id);
    p_terminated_id = WEXITSTATUS(p_terminated_id);
    p_terminated = true;

    // signal(SIGUSR1, handler);
}

int ShortestRemaining(PCB *Processes, int count, int *index)
{
    int shortest = INT_MAX;
    int pid = -1;
    for (int i = 0; i < count; i++)
    {
        if (Processes[i].state == READY || Processes[i].state == RUNNING)
        {
            if (Processes[i].remaining_time < shortest)
            {
                pid = Processes[i].pid;
                shortest = Processes[i].remaining_time;
                *index = i;
            }
        }
    }
    return pid;
}