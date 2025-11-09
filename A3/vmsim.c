/* ID Header:
 Student Name: Amal Ali
 Student ID: 3145040
 Submission Date: 11/08/2025
 File:
*/

#include "vmsim.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

// usage
static void usage(const char *prog)
{
    fprintf(stderr, "Usage:\n  %s --mode=bb  --base=N --limit=N --trace=FILE \n  %s --mode=seg --config=FILE --trace=FILE \n", prog, prog);
}

// CLI
bool parse_args(int argc, char **argv, sim_opts_t *o)
{
    (void)argc;
    (void)argv;
    if (!o) // null pointer
        return false;

    for (int i = 1; i < argc; i++) // start from 1 to skip program name and go through all arguments in the command line
    {
        if (strncmp(argv[i], "--mode=", 7) == 0) // check if the argument starts with --mode=
        {
            const char *m = argv[i] + 7;
            if (strcmp(m, "bb") == 0)
                o->mode = MODE_BB;
            else if (strcmp(m, "seg") == 0)
                o->mode = MODE_SEG;
            else
            {
                return false;
            }
        }
        else if (strncmp(argv[i], "--base=", 7) == 0) // check if the argument starts with --base=
        {
            const char *p = argv[i] + 7; // pointer to the character after =
            if (*p == '\0')
                return false;
            o->base = atoi(p); // convert string to int
        }
        else if (strncmp(argv[i], "--limit=", 8) == 0) // check if the argument starts with --limit=
        {
            const char *p = argv[i] + 8;
            if (*p == '\0')
                return false;
            o->limit = atoi(p); // convert string to int
        }

        else if (strncmp(argv[i], "--config=", 9) == 0) // check if the argument starts with --config=
        {
            o->config_path = argv[i] + 9; // saving the config path
            if (*o->config_path == '\0')
                return false;
        }
        else if (strncmp(argv[i], "--trace=", 8) == 0) // check if the argument starts with --trace=
        {
            o->trace_path = argv[i] + 8; // saving the trace path
            if (*o->trace_path == '\0')
                return false;
        }
        else
            return false;
    }

    if (o->mode == MODE_BB)
    {
        if (!o->trace_path || o->base < 0 || o->limit <= 0) // base must be >=0, limit must be >0
            return false;
    }
    else if (o->mode == MODE_SEG)
    {
        if (!o->trace_path || !o->config_path) // both trace and config paths must be provided
            return false;
    }
    else
    {
        return false;
    }

    return true;
}

// bb
int run_bb(const sim_opts_t *o, stats_t *st)
{
    FILE *file = fopen((*o).trace_path, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Error: cannot open trace file %s\n", (*o).trace_path);
        return 1;
    }

    char line[256];      // buffer to hold each line from the trace file
    int line_number = 0; // line number for error reporting

    while (fgets(line, sizeof(line), file))
    {
        line_number++; // increment line number because we're on a new line

        // Ignore comments and blank lines
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r')
            continue;

        char op; // operation: R, W, X
        char adress_array[128] = {0};
        int parts = sscanf(line, " %c %s", &op, adress_array); // read ops (operation) and addr (address)

        // Invalid operation
        if (!(op == 'R' || op == 'W'))
        {
            fprintf(stderr, "trace: %s:%d: malformed: op must be R/W, we got \"%c\"\n", (*o).trace_path, line_number, op);
            continue;
        }

        // If the part after the opperation is a comment '#', dont count it as an address and return an error message
        if ((parts == 2 && adress_array[0] == '#') || parts == 1)
        {
            fprintf(stderr, "trace: %s:%d: malformed: expected \"OP ADDR\"\n", (*o).trace_path, line_number);
            continue;
        }

        // Validate address_array is all decimal digits
        int all_digits = 1;
        for (const char *p = adress_array; *p; ++p)
        {
            if (!isdigit((unsigned char)*p))
            {
                all_digits = 0;
                break;
            }
        }

        if (all_digits == 0) // check if all address characters are digits, if not, print error and continue
        {
            fprintf(stderr, "trace: %s:%d: bad address \"%s\" (not a decimal)\n", (*o).trace_path, line_number, adress_array);
            continue;
        }

        int addr = atoi(adress_array); // use ascii to integer conversion function to get the address in decimal form for the bounds check

        st->accesses++; // increment total accesses

        if (addr >= 0 && addr < (*o).limit) // check bounds
        {
            int pa = o->base + addr;
            st->ok++;
            printf("%c %d -> PA %d ; ok\n", op, addr, pa);
        }

        else
        {
            st->faults_bounds++;
            printf("%c %d -> fault: BOUNDS\n", op, addr);
        }
    }

    fclose(file);

    // Print summary
    printf("\n== stats ==\n");
    printf("accesses=%lu, ok=%lu, faults.bounds=%lu\n", (*st).accesses, (*st).ok, (*st).faults_bounds);

    return 0;
}

// seg
int run_seg(const sim_opts_t *o, stats_t *st)
{
    (void)o;
    (void)st;
    seg_table_t table = {0}; // initialize segment table
    segment_t seg = {0};     // temporary segment to hold parsed data

    // --- Read config file ---
    FILE *fp = fopen((*o).config_path, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Error: Could not open file %s\n", (*o).config_path);
        return 1;
    }
    char line[256];
    while (fgets(line, sizeof(line), fp)) // read line by line
    {
        if (line[0] == '#' || isspace(line[0])) // ignore lines starting with '#'
            continue;

        if (sscanf(line, "%s %ld %ld %s", seg.name, &seg.base, &seg.limit, seg.perms) == 4) // check if all 4 items are read
        {
            seg.in_use = true;               // mark segment as in use
            table.segs[table.nsegs++] = seg; // add segment to table and increment segment count
        }
    }
    fclose(fp);

    // --- Process the trace file ---
    FILE *trace = fopen(o->trace_path, "r");
    if (!trace)
    {
        fprintf(stderr, "Error: cannot open trace file %s\n", o->trace_path);
        return 1;
    }

    int line_num = 0;
    while (fgets(line, sizeof(line), trace))
    {
        // line_num++;

        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') // ignore comments and blank lines
            continue;

        char op;
        char segname[32];
        int offset;

        int matched = sscanf(line, " %c %s %d", &op, segname, &offset); // read operati

        if (matched != 3 || offset < 0) // if there isnt opperation, seg, or offset
        {
            fprintf(stderr, "trace: %s:%d: malformed: expected \"OP SEG OFFSET\"\n", o->trace_path, line_num);
            continue;
        }
        st->accesses++;

        // --- check segments ---
        segment_t *seg = NULL;
        for (size_t i = 0; i < table.nsegs; i++) // iterate through segments in the table
        {
            if (strcmp(table.segs[i].name, segname) == 0) // if the segment name matches with one in the table
            {
                seg = &table.segs[i]; // point to that segment
                break;
            }
        }

        if (!seg) // if segment not found
        {
            st->faults_noseg++;
            printf("%c %s %d -> fault: NOSEG\n", op, segname, offset);
            continue;
        }

        // --- Bounds check ---
        if (offset < 0 || offset >= seg->limit)
        {
            st->faults_bounds++;
            printf("%c %s %d -> fault: BOUNDS\n", op, segname, offset);
            continue;
        }

        // --- Permission check ---
        int allowed = 0;

        for (int i = 0; seg->perms[i] != '\0'; i++)
        {
            if ((op == 'R' && seg->perms[i] == 'r') || (op == 'W' && seg->perms[i] == 'w') || (op == 'X' && seg->perms[i] == 'x'))
            {
                allowed = 1;
                break;
            }
        }

        if (!allowed)
        {
            st->faults_prot++;
            printf("%c %s %d -> fault: PROTECTION\n", op, segname, offset);
            continue;
        }

        // --- Address translation ---
        long pa;
        int valid = 0;

        if (strcmp(seg->name, "stack") == 0)
        {
            long offsigned = offset - seg->limit;           // calculate signed offset for stack segment
            if (offsigned >= -seg->limit && offsigned <= 0) // check bounds for stack segment
            {
                pa = seg->base + offsigned; // calculate physical address
                valid = 1;                  // mark as valid
            }
            else
            {
                st->faults_bounds++;                                        // else increment bounds fault counter
                printf("%c %s %d -> fault: BOUNDS\n", op, segname, offset); // print fault message
                continue;
            }
        }
        else
        {
            if (offset >= 0 && offset < seg->limit) // for non-stack segments, check bounds
            {
                pa = seg->base + offset; // calculate physical address
                valid = 1;
            }
            else
            {
                st->faults_bounds++;
                printf("%c %s %d -> fault: BOUNDS\n", op, segname, offset); // else print bounds fault message
                continue;
            }
        }

        if (valid)
        {
            st->ok++;
            seg->hits++;
            printf("%c %s %d -> PA %ld ; ok\n", op, segname, offset, pa);
        }
    }
    fclose(trace);

    // --- Summary ---
    printf("== stats ==\n");
    printf("accesses=%lu, ok=%lu, faults.bounds=%lu, faults.prot=%lu, faults.noseg=%lu\n",
           st->accesses, st->ok, st->faults_bounds, st->faults_prot, st->faults_noseg);

    printf("seg hits:");
    for (size_t i = 0; i < table.nsegs; i++)
    {
        printf(" %s=%lu", table.segs[i].name, table.segs[i].hits);
        if (i + 1 < table.nsegs)
            printf(",");
    }
    printf("\n");
    return 0;
}

// main()
int main(int argc, char **argv)
{
    sim_opts_t opts;
    if (!parse_args(argc, argv, &opts))
    {
        usage(argv[0]);
        return 1;
    }
    stats_t st = (stats_t){0};
    if (opts.mode == MODE_BB)
        return run_bb(&opts, &st);
    else
        return run_seg(&opts, &st);
}
