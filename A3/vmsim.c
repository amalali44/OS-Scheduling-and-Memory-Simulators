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
    fprintf(stderr,
            "Usage:\n"
            "  %s --mode=bb  --base=N --limit=N --trace=FILE \n"
            "  %s --mode=seg --config=FILE --trace=FILE \n",
            prog, prog);
}

// CLI
bool parse_args(int argc, char **argv, sim_opts_t *o)
{
    (void)argc;
    (void)argv;
    if (o)
        memset(o, 0, sizeof(*o));

    for (int i = 1; i < argc; i++) // while there are arguments in the count
    {
        if (strncmp(argv[i], "--mode=", 7) == 0)
        {
            const char *user_input = argv[i] + 7; // check the input after 7 characters (after --mode=)
            if (strcmp(user_input, "bb") == 0)    // check if they put valid policy input 1: bb
            {
                o->mode = MODE_BB;
            }
            if (strncmp(argv[i], "--base=", 7) == 0) // check if they put valid input for base
            {
                char *endptr;
                errno = 0;                                    // reset errno before call
                long val = strtol(argv[i] + 7, &endptr, 10);  // convert string to long
                if (errno != 0 || *endptr != '\0' || val < 0) // check for conversion errors or negative values
                {
                    usage(argv[0]);
                    return 1;
                }
                o->base = val;
            }
            else if (strncmp(argv[i], "--limit=", 8) == 0) // check if they put valid input for limit
            {
                char *endptr;
                errno = 0;                                     // reset errno before call
                long val = strtol(argv[i] + 8, &endptr, 10);   // convert string to long
                if (errno != 0 || *endptr != '\0' || val <= 0) // check for conversion errors or non-positive values
                {
                    usage(argv[0]);
                    return 1;
                }
                o->limit = val;
            }
            else if (strncmp(argv[i], "--trace=", 8) == 0) // check if they put valid input for trace file path
            {
                o->trace_path = argv[i] + 8; // set the trace path to the user input after --trace=
            }
            else if (strncmp(argv[i], "--config=", 9) == 0) // check if they put valid input for config file path
            {
                o->config_path = argv[i] + 9; // set the config path to the user input after --config=
            }
            else if (strcmp(user_input, "seg") == 0) // Check if they put valid policy input 2: seg
            {
                o->mode = MODE_SEG;
            }
            else
            {
                usage(argv[0]); // else print usage and return 1
                return 1;
            }
        }
    }
}
// bb
int run_bb(const sim_opts_t *o, stats_t *st)
{
    (void)o;
    (void)st;
    fprintf(stderr, "TODO: run_bb()\n");
    return 0;
}

// seg
int run_seg(const sim_opts_t *o, stats_t *st)
{
    (void)o;
    (void)st;
    fprintf(stderr, "TODO: run_seg()\n");
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
