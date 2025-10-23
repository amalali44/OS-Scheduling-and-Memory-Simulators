/* ID Header:
 Student Name:
 Student ID:
 Submission Date:
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

int run_bb(const sim_opts_t *o, stats_t *st)
{
    FILE *fp = fopen(o->trace_path, "r");
    if (!fp)
    {
        fprintf(stderr, "Error: cannot open trace file %s\n", o->trace_path);
        return 2;
    }

    char op;
    long va;
    char line[256]; // buffer for each line

    while (fgets(line, sizeof(line), fp))
    {
        // skip comments and blank lines
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r')
            continue;

        if (sscanf(line, " %c %ld", &op, &va) != 2) // parse op (operation) and va (virtual address)
            continue;

        if (op != 'R' && op != 'W' && op != 'X') // invalid operation
            continue;

        st->accesses++; // increment total accesses

        if (va < 0 || va >= o->limit)
        {
            st->faults_bounds++; // increment bounds faults
            printf("%c %ld -> fault: BOUNDS\n", op, va);
        }
        else
        {
            int pa = o->base + va;
            st->ok++; // increment successful accesses
            printf("%c %ld -> PA %d ; ok\n", op, va, pa);
        }
    }

    fclose(fp);

    printf("\n== stats ==\n");
    printf("accesses=%lu, ok=%lu, faults.bounds=%lu\n",
           st->accesses, st->ok, st->faults_bounds);

    return 0;
}

// seg
int run_seg(const sim_opts_t *o, stats_t *st)
{
    (void)o;
    (void)st;
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
        printf("%s", line); // print line by line
    }
    fclose(fp);
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
