#include "headers.h"

void clearResources(int);

int main(int argc, char * argv[])
{
    //signal(SIGINT, clearResources);

    // TODO Initialization

    // 1. Read the input files.

    FILE* file = fopen("processes.txt", "r");
    if (!file)
    {
        printf("Error in opening file\n");
        return -1;
    }
    char c;
    int processes_count; //number of processes
    while ((c = fgetc(file)) != EOF)
    {
        if (c == '\n')
            processes_count++;
    }
    processes_count--;
    fclose(file);
    int ids[processes_count]; //array of ids
    int arrivals[processes_count]; //array of arrival times
    int runtimes[processes_count]; //array of runtimes
    int priorities[processes_count]; //array of priorities
    file = fopen("processes.txt", "r");
    if (!file)
    {
        printf("Error in opening file\n");
        return -1;
    }
    while ((c = fgetc(file)) != EOF)
    {
        if (c == '\n')
            break;
    }
    for (int i = 0; i < processes_count; i++)
    {
        fscanf(file, "%d\t%d\t%d\t%d", &ids[i], &arrivals[i], &runtimes[i], &priorities[i]);
    }

    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.

    int algo;
    int quantum = 0;
    printf("[1]HPF   [2]SRTN   [3]RR\n");
    printf("Please, choose a scheduling algo: ");
    scanf("%d", &algo);
    while (algo != 1 && algo != 2 && algo != 3) //checking for valid choice
    {
        printf("Error! Please, choose from [1-3]: ");
        scanf("%d", &algo);
    }
    if (algo == 3)
    {
        printf("Please, enter a valid quantum value: ");
        scanf("%d", &quantum);
        while (quantum < 1) //checking for quantum value
        {
            printf("Error! Please, enter a positive integer: ");
            scanf("%d", &quantum);
        }
    }

    // 3. Initiate and create the scheduler and clock processes.

    int clk_pid = fork(); //clock forking
    if (clk_pid == -1)
    {
        perror("Error in forking of clock");
        return -1;
    }
    else if (clk_pid == 0) //clock process
    {
        execl("./clk.out", "clk.out", NULL);
        perror("Error in excel of clock");
        return -1;
    }

    // int scheduler_pid = fork(); //scheduler forking
    // if (scheduler_pid == -1)
    // {
    //     perror("Error in forking of scheduler");
    //     return -1;
    // }
    // else if (scheduler_pid == 0) //scheduler process
    // {
    //     execl("./scheduler.out", "scheduler.out", NULL);
    //     perror("Error in excel of scheduler");
    //     return -1;
    // }

    // 4. Use this function after creating the clock process to initialize clock
    initClk();

    // To get time use this
    //int x = getClk();
    //printf("current time is %d\n", x);
    
    // 5. Create a data structure for processes and provide it with its parameters.

    PCB processes[processes_count];
    for (int i = 0; i < processes_count; i++)
    {
        processes[i].id = ids[i];
        processes[i].arrival_time = arrivals[i];
        processes[i].running_time = runtimes[i];
        processes[i].priority = priorities[i];
        //all other parameters are initialized with zero
        processes[i].pid = 0;
        processes[i].waiting_time = 0;
        processes[i].remaining_time = 0;
        processes[i].finish_time = 0;
        processes[i].state = READY;
    }

    // TODO Generation Main Loop

    // 6. Send the information to the scheduler at the appropriate time.

    int start = 0;
    while (true)
    {
        for (int i = start; i < processes_count; i++)
        {
            if (processes[i].arrival_time == getClk())
            {
                //needs a function that sends this process to the scheduler's ready list
                printf("process[%d] arrived at time %d\n", processes[i].id, getClk());
                start = i;
            }
        }
        sleep(1);
    }

    // 7. Clear clock resources
    destroyClk(true);
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
}