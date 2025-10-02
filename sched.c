/*
 * Student Name: Amal Ali
 * Student ID: 3145040
 * Date: MS1: 09/26/2025 | MS2: 10/2/2025 | Final submission: 10/11/2025
 * File: sched.c
 * Description:  CPU FCFS and RR scheduling simulator
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sched.h"

static void usage(const char *prog)
{
    /* one-line usage only (per spec) */
    fprintf(stderr, "Usage: %s --policy=FCFS|RR [--quantum=N] --in=FILE\n", prog);
}

// /* TODO: strict CLI
//    Required: --policy=FCFS|RR, --in=FILE
//    Optional: --quantum=N (required iff RR; N>0)
//    On error: print usage() and return non-zero.
// */
int parse_args(int argc, char **argv, sim_cfg_t *cfg, const char **input_path)
{
    int policy = 0, in = 0, quantum = 0, quantum_val = 0; // set up counters for terminal inputs
    for (int i = 1; i < argc; i++)                        // while there are arguments in the count
    {
        if (strncmp(argv[i], "--policy=", 9) == 0)
        {
            const char *user_input = argv[i] + 9; // check the input after 9 characters (after --policy=)
            if (strcmp(user_input, "FCFS") == 0)  // check if they put valid policy input 1: FCFS
            {
                cfg->policy = POLICY_FCFS;
                policy = 1; // set policy counter to 1 if valid input
            }
            else if (strcmp(user_input, "RR") == 0) // Check if they put valid policy input 2: RR
            {
                cfg->policy = POLICY_RR;
                policy = 2; // set policy counter to 1 if valid input
            }
            else
            {
                usage(argv[0]); // else print usage and return 1
                return 1;
            }
        }
        else if (strncmp(argv[i], "--in=", 5) == 0) // check if they put valid input for file path
        {
            *input_path = argv[i] + 5; // set the input path to the user input after --in=
            in = 1;
        }
        else if ((strncmp(argv[i], "--quantum=", 10) == 0) && policy == 2) // check if the polciy is RR and there is valid input for quantum
        {
            int q = 0;
            const char *quantum_check = argv[i] + 10; // check the input after 10 characters (after --quantum=)

            for (; *quantum_check; ++quantum_check) //
            {
                if (*quantum_check < '0') // make sure the input is a valid int
                {
                    usage(argv[0]);
                    return 1;
                }
                q = q * 10 + (*quantum_check - '0'); // convert string to int
            }
            quantum = 1;
            quantum_val = q;
        }
        else
        {
            usage(argv[0]);
            return 1;
        }
    }
    if (!policy || !in) // if either policy or the input path counters are 0, print usage & return 1
    {
        usage(argv[0]);
        return 1;
    }
    if (cfg->policy == POLICY_RR) // require quantum if policy is RR
    {
        if (!quantum)
        {
            usage(argv[0]);
            return 1;
        }
        cfg->quantum = quantum_val;
    }
    return 0;
}

/* TODO: read "PID ARRIVAL CPU_TIME", ignore lines starting with '#', validate:
   pid>=0, arrival>=0, cpu>0. Sort by (arrival, pid). Return 0 on success.
*/

int load_workload(const char *path, job_t **jobs, int *n)
{
    (void)path;
    (void)jobs;
    (void)n;

    FILE *file = fopen(path, "r"); // open the file in read mode
    if (file == NULL)              // check if the file opened successfully
    {
        fprintf(stderr, "Error: Could not open file %s\n", path);
        return 1;
    }

    int count = 0;
    static job_t job_list[100]; // allocate a fixed array for 100 jobs initially

    char line[256];

    while (fgets(line, sizeof(line), file)) // read each line from the file
    {
        if (line[0] == '#') // ignore lines starting with '#'
            continue;
        int pid, arrival, cpu_time;

        if (sscanf(line, "%d %d %d", &pid, &arrival, &cpu_time) != 3 || pid < 0 || arrival < 0 || cpu_time <= 0) // parse the line for pid, arrival, cpu_time (add for unique pid as well)
        {
            fprintf(stderr, "Error: Invalid line format");
            fclose(file);
            return 1;
        }

        // assign valies to job struct
        job_list[count].pid = pid;
        job_list[count].arrival = arrival;
        job_list[count].cpu_time = cpu_time;
        count++; // increment the job count
    }
    fclose(file); // close the file

    *jobs = malloc(count * sizeof(job_t)); // allocate memory for the output array
    if (!*jobs)
    {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return 1;
    }
    for (int i = 0; i < count; i++) // copy jobs to the output array
    {
        (*jobs)[i] = job_list[i];
    }

    *n = count;
    return 0;
}

/* TODO: discrete-time CPU simulation with FIFO ready queue.
   FCFS: never preempt. RR: quantum countdown; when it hits 0 and job not finished, enqueue at tail.
   Track: first run (first time scheduled) and completion (t+1 on finish).
   Build test_workloads run[t]=pid or -1 (idle). After finishing:
     - context_switches = count PID->PID changes (ignore idle)
     - TAT = completion-arrival; RESP = first run-arrival
   Print exactly:
     time: 0 1 2 ... T-1
     run : <pid or -> per tick
     Pk: first run=... completion=... TAT=... RESP=...
     System: context_switches_switches=..., avgTAT=..., avgRESP=...
*/
int simulate(const job_t *jobs, int n, const sim_cfg_t *cfg, sim_metrics_t *out)
{
    (void)jobs;
    (void)n;
    (void)cfg;
    (void)out;

    // Tracking arrays
    int remaining_time[100];
    int first_run[100];
    int completion[100];

    for (int i = 0; i < n; i++) // initialize arrays that keep track of each job's remaining time, first run time, and completion time
    {
        remaining_time[i] = jobs[i].cpu_time;
        first_run[i] = -1;
        completion[i] = -1;
    }

    int test_workloads[10000]; // large enough for test workloads
    int t = 0;                 // current time tick
    int done = 0;              // how many jobs finished
    int prev_pid = -1;         // for context switch counting
    int context_switches = 0;  // context switch counter

    int queue[100]; // The ready queue (stores indices of jobs)
    int qhead = 0;
    int qtail = 0;
    int running = -1; // index of job currently on CPU

    // FCFS simulation loop
    while (done < n) // while not all jobs are done. (n is the number of jobs found in the input file)
    {
        for (int i = 0; i < n; i++) // Step 1: enqueue any jobs that arrive at this tick
        {
            if (jobs[i].arrival == t)
                queue[qtail++] = i; // put the job in the ready queue
        }

        if (running == -1) // Step 2: if CPU idle, grab the next job from the 'ready' queue
        {
            if (qhead < qtail)
                running = queue[qhead++];
            else
                running = -1;
        }

        if (running == -1) // Step 3: execute for this tick
        {
            test_workloads[t] = -1; // CPU idle
        }
        else
        {
            if (first_run[running] == -1) // if the job has never ran before
                first_run[running] = t;   // record first run

            test_workloads[t] = jobs[running].pid;
            remaining_time[running]--;

            if (remaining_time[running] == 0) // job finished
            {
                completion[running] = t + 1; // record finish time, the last 1
                done++;
                running = -1; // CPU is now free
            }
        }
        t++; // move to the next time tick
    }

    //  for counting amount of context switches
    for (int i = 0; i < t; i++)
    {
        int current_pid = test_workloads[i];
        if (current_pid >= 0 && prev_pid >= 0 && current_pid != prev_pid) // count context switch if current pid and previous pid is -1, and if current pid is different from previous pid
            context_switches++;
        if (current_pid >= 0)       // update previous pid if current pid is not -1
            prev_pid = current_pid; // update previous pid
    }

    //  Print time and run metrics
    printf("time:");
    for (int i = 0; i < t; i++) // print time ticks from 0 to t-1
        printf(" %d", i);
    printf("\nrun :");

    for (int i = 0; i < t; i++)
    {
        if (test_workloads[i] < 0)
            printf(" -"); // for when its idle
        else
            printf(" %d", test_workloads[i]);
    }
    printf("\n");

    int total_tat = 0, total_resp = 0;
    for (int i = 0; i < n; i++)
    {
        int tat = completion[i] - jobs[i].arrival;
        int resp = first_run[i] - jobs[i].arrival;
        total_tat += tat;
        total_resp += resp;

        printf("P%d: first run= %d completion= %d TAT= %d RESP= %d\n", jobs[i].pid, first_run[i], completion[i], tat, resp); // each job's metrics
    }

    // Save results for the last system line
    (*out).context_switches = context_switches;
    (*out).avg_tat = total_tat / n;
    (*out).avg_resp = total_resp / n;

    printf("System: ctx_switches= %d, avgTAT= %d, avgRESP= %d\n", context_switches, (total_tat / n), total_resp / n);

    return 0;
}

// the main you provided for us (thank you!)
int main(int argc, char **argv)
{
    sim_cfg_t cfg;
    const char *input_path = NULL;
    int pr = parse_args(argc, argv, &cfg, &input_path);
    if (pr != 0)
        return 1;

    job_t *jobs = NULL;
    int n = 0;
    if (load_workload(input_path, &jobs, &n) != 0)
        return 2;

    sim_metrics_t m;
    if (simulate(jobs, n, &cfg, &m) != 0)
    {
        // free(jobs);
        return 3;
    }

    // free(jobs);
    return 0;
}
