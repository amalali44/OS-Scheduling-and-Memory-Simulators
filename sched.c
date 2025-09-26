/*
 * Student Name: Amal Ali
 * Student ID:   3145040
 * Date:         MS1: 09/26/2025
 * File:         sched.c
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

/* TODO: strict CLI
   Required: --policy=FCFS|RR, --in=FILE
   Optional: --quantum=N (required iff RR; N>0)
   On error: print usage() and return non-zero.
*/
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
    fprintf(stderr, "TODO: load_workload() not implemented\n");
    return 1;
}

/* TODO: discrete-time CPU simulation with FIFO ready queue.
   FCFS: never preempt. RR: quantum countdown; when it hits 0 and job not finished, enqueue at tail.
   Track: first run (first time scheduled) and completion (t+1 on finish).
   Build timeline run[t]=pid or -1 (idle). After finishing:
     - ctx = count PID->PID changes (ignore idle)
     - TAT = completion-arrival; RESP = first run-arrival
   Print exactly:
     time: 0 1 2 ... T-1
     run : <pid or -> per tick
     Pk: first run=... completion=... TAT=... RESP=...
     System: ctx_switches=..., avgTAT=..., avgRESP=...
*/
int simulate(const job_t *jobs, int n, const sim_cfg_t *cfg, sim_metrics_t *out)
{
    (void)jobs;
    (void)n;
    (void)cfg;
    (void)out;
    fprintf(stderr, "TODO: simulate() not implemented\n");
    return 1;
}

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
        free(jobs);
        return 3;
    }

    free(jobs);
    return 0;
}
