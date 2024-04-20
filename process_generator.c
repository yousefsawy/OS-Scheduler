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

    int scheduler_pid = fork(); //scheduler forking
    if (scheduler_pid == -1)
    {
        perror("Error in forking of scheduler");
        return -1;
    }
    else if (scheduler_pid == 0) //scheduler process
    {
        execl("./scheduler.out", "scheduler.out", NULL);
        perror("Error in excel of scheduler");
        return -1;
    }

    // 4. Use this function after creating the clock process to initialize clock
    initClk();
    sleep(1);
    //sleep(1);
    // To get time use this
    int x = getClk();
    printf("current time is %d\n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
    destroyClk(true);
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
}