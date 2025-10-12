#CMPT 360 – A2: CPU Scheduling Simulator

This program simulates two basic CPU scheduling algorithms — First-Come, First-Served (FCFS) and Round Robin (RR): using a single CPU and a discrete-time model. The simulator reads a workload file that lists processes by: PID, ARRIVALm CPU_TIME. Each tick of simulated time represents one unit of CPU work. The program keeps track of which process runs and computes these metrics:
- First run time – when a job first starts running
- Completion time – when a job finishes
- Turnaround time (TAT) – total time spent in the system
- Response time (RESP) – delay before the job first runs
- Context switches – how many times the CPU changed from one job to another

• parse_args(): Reads and validates the command-line arguments. Checks that the user includes the required flags: --policy=FCFS or --policy=RR ,--in=FILENAME, and --quantum=N (only required if policy is RR). If any argument is missing or invalid, it prints a one-line usage message and exits with a non-zero status. The function fills in the sim_cfg_t struct with the chosen policy and quantum value.

• load_workload(): Reads all the processes from the input text file and stores them in an array of job_t structs. Opens the given file for reading. Reads each line, skipping comment lines (starting with #). Each valid line must contain: PID, arrival time and CPU time. I Validates that pid >= 0, arrival >= 0, and cpu_time > 0. When done, the function returns: *jobs → a pointer to the job array  and *n → The number of valid jobs found  

• simulate(): Runs the CPU scheduling simulation in discrete time. This is where the main scheduling logic happens. It handles both FCFS and Round Robin depending on what the user selected. Uses arrays to track each process’s: Remaining CPU time, First run time, Completion time. Uses a simple queue to store processes waiting for the CPU. At each tick we add new jobs to the queue, If CPU is idle, we assigns the next job from the queue and it runs the job for one tick and records it in a timeline array. If using Round Robin, it checks if the process’s quantum expired, If so, and the process isn’t finished yet, it’s moved to the end of the queue. Thus continues until all jobs finish. When the simulation ends, it calculates each process’s turnaround time (completion - arrival), response time (first_run - arrival), counts the context switches  and Prints everything in the required format

• main(): Coordinates everything and ensures the simulation runs cleanly. It first calls parse_args() to process command-line options, then load_workload() to read the input files and load all processes, then it calls simulate() to run the scheduling simulation and print results and finally frees memory before exiting.


• A statement on the status of your submission:

--> Both FCFS and RR  are working, there are no warnings under -std=c11 -Wall -Wextra -pedantic, Valgrind command shows "All heap blocks were freed -- no leaks are possible" and I tested with all provided workloads from 1-4.

~ I certify that this submission represents entirely my own work.

