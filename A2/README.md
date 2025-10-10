# CMPT 360 – A2: CPU Scheduling Simulator

## Overview

This program simulates two basic CPU scheduling algorithms — irst-Come, First-Served (FCFS) and Round Robin (RR): using a single CPU and a discrete-time model.


---

## How It Works

The simulator reads a workload file that lists processes by:
```
PID ARRIVAL CPU_TIME
```

Each tick of simulated time represents one unit of CPU work.  
The program keeps track of which process runs at each tick and computes these metrics:
- **First run time** – when a job first starts running
- **Completion time** – when a job finishes
- **Turnaround time (TAT)** – total time spent in the system
- **Response time (RESP)** – delay before the job first runs
- **Context switches** – how many times the CPU changed from one job to another

---

## Supported Policies

### FCFS (First-Come, First-Served)
Jobs are handled strictly in arrival order.  
Once a job starts, it runs until completion.

### RR (Round Robin)
Each job runs for a **time quantum** before being preempted and sent to the back of the queue.  
This ensures every process gets a fair share of CPU time, improving responsiveness.






## Design Choices

- Used simple arrays (no structs with extra fields) for clarity.  
- `simulate()` uses parallel arrays to track `remaining_time`, `first_run`, and `completion`.  
- Input reading assumes workloads are already sorted by arrival time.  
- The output format exactly follows the spec, with no extra text.  

---

• A statement on the status of your submission:

--> Both FCFS and RR  are working, there are no warnings under `-std=c11 -Wall -Wextra -pedantic` and I tested with all provided workloads from 1-4

~ I certify that this submission represents entirely my own work.

