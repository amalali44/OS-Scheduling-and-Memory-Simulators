/*
 * Student Name: Amal Ali
 * Student ID:   3145040
 * Date:         MS1:
 * File:         ____________________________________________
 * Description:  ____________________________________________
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
int parse_args(int argc, char **argv, sim_cfg_t *cfg, const char **in_path)
{
    int policy = 0, in = 0, quantum = 0, quantum_val = 0;
    for (int i = 1; i < argc; i++)
    {
        if (strncmp(argv[i], "--policy=", 9) == 0)
        {
            const char *user_input = argv[i] + 9;
            if (strcmp(user_input, "FCFS") == 0)
            {
                cfg->policy = POLICY_FCFS;
                policy = 1;
            }
            else if (strcmp(user_input, "RR") == 0)
            {
                cfg->policy = POLICY_RR;
                policy = 1;
            }
            else
            {
                usage(argv[0]);
                return 1;
            }
        }
        else if (strncmp(argv[i], "--in=", 5) == 0)
        {
            *in_path = argv[i] + 5;
            in = 1;
        }
        else if (strncmp(argv[i], "--quantum=", 10) == 0)
        {
            int q = 0;
            const char *quantum_check = argv[i] + 10;
            if (*quantum_check == '\0')
            {
                usage(argv[0]);
                return 1;
            }
            for (; *quantum_check; ++quantum_check)
            {
                if (*quantum_check < '0' || *quantum_check > '9')
                {
                    usage(argv[0]);
                    return 1;
                }
                q = q * 10 + (*quantum_check - '0');
            }
            if (q <= 0)
            {
                usage(argv[0]);
                return 1;
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
    if (!policy || !in)
    {
        usage(argv[0]);
        return 1;
    }
    if (cfg->policy == POLICY_RR)
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
    const char *in_path = NULL;
    int pr = parse_args(argc, argv, &cfg, &in_path);
    if (pr != 0)
        return 1;

    job_t *jobs = NULL;
    int n = 0;
    if (load_workload(in_path, &jobs, &n) != 0)
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
