/*
 * Student Name: Amal Ali
 * Student ID:   3145040
 * Date:         MS1: 09/26/2025 | MS2: 10/2/2025 | Final submission: 10/11/2025
 * File:         sched.h
 * Description:  Header file for CPU FCFS and RR scheduling simulator
 */
#ifndef SCHED_H
#define SCHED_H

typedef struct
{
    int pid, arrival, cpu_time;
} job_t;
typedef enum
{
    POLICY_FCFS,
    POLICY_RR
} policy_t;

typedef struct
{
    policy_t policy; /* required */
    int quantum;     /* required iff RR */
} sim_cfg_t;

typedef struct
{
    int context_switches;
    double avg_tat;
    double avg_resp;
} sim_metrics_t;

int parse_args(int argc, char **argv, sim_cfg_t *cfg, const char **in_path);
int load_workload(const char *path, job_t **jobs, int *n);
int simulate(const job_t *jobs, int n, const sim_cfg_t *cfg, sim_metrics_t *out);

#endif
