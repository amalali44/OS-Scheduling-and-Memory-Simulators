#CMPT 360 – A3: Address Translation in Practice

This program simulates two fundamental virtual memory translation schemes used in operating systems: Base & Bounds (BB) and Segmentation (SEG). The simulator reads one or more configuration files and a trace file that lists memory accesses. Each trace record represents a logical address access (read, write, or execute) and the simulator translates it into a physical address (PA) or reports a fault.

The simulator tracks different fault types, verifies permissions, and prints per-access results along with a summary of system statistics


• parse_args(): Reads and validates the command-line arguments. Checks that the user includes the required flags:

  --mode=bb, In BB mode, it requires: --base=N, --limit=N, and --trace=FILE. 
  Or 
  --mode=seg , In SEG mode, it requires: --config=FILE and --trace=FILE.
  
All valid arguments are stored in a sim_opts_t struct which contains the selected mode, file paths, base, and limit values.

• run_bb(): Implements the Base & Bounds translation scheme. Opens and reads the trace file line by line. Each valid line has an operation (R or W) and a logical address. For every access, it checks that the address is within the valid range: { 0 ≤ offset < limit }. If valid, it computes the physical address using: PA = base + offset and prints the result with ; ok. If not valid, it prints fault: BOUNDS. Malformed lines (missing arguments, bad op, or non-decimal addresses) print a descriptive error showing the file name, line number, and cause. At the end, it prints statistics that include total accesses, successful translations, and bounds faults.

• run_seg(): Implements the Segmentation translation scheme. Reads the segment configuration file, where each line defines: SEGMENT_NAME BASE LIMIT PERMISSIONS. Builds a segment table in memory, with each segment’s base address, size (limit), and permissions. Then it reads the trace file, 
Locates the segment by name, and checks if the offset is within bounds:

    Non-stack segments: valid if 0 ≤ offset < limit, PA = base + offset

    Stack segments: downward growth, so
    offsigned = offset − limit,
    valid if |offsigned| ≤ limit,
    PA = base + offsigned

permissions are verified based on the operation and Prints either the translated PA with 'ok' or the appropriate fault message (BOUNDS, PROTECTION, or NOSEG).


• main(): coordinates everything, calls parse_args() to process and validate the command-line inputs. Initializes a stats_t structure to track system totals and depending on the mode, calls either run_bb() or run_seg().


• A statement on the status of your submission:

--> Both bb and seg are functional, there are no warnings under -std=c11 -Wall -Wextra -pedantic, Valgrind command shows "All heap blocks were freed -- no leaks are possible". However, I do know at the time of submission some of the error checking and messages for a bad config file is not implemented, unfortuantely I ran out of time to write those.

~ I certify that this submission represents entirely my own work.
