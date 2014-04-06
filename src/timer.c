/* Time a number of iterations of a command and report statistics.
 *
 * Usage: timer options
 * -h --help Display this usage information.
 * -i --iterations Number of iterations to run COMMAND.
 * -c --command COMMAND to be measured.
 * -l --latex Save results as a LaTeX table named results.tex.
 * -j --json Save results as a JSON file named results.json.
 * -s --csv Save results as a CSV file named results.csv.
 * -q --quiet Run in quiet mode.
 * -v --verbose Run in verbose mode.
 *
 * TODO: LaTeX output, JSON output, CSV output, confidence intervals.
 *
 * (c) Sarah Mount <s.mount@wlv.ac.uk> 2014
 */

#include <errno.h>
#include <getopt.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>

#include "timer_data.h"

#define DEFAULT_ITERATIONS 10

#define MAX_ARGS 64

/* Which timer should we use? Options are:
 *
 * CLOCK_REALTIME
 * CLOCK_REALTIME_COARSE
 * CLOCK_MONOTONIC
 * CLOCK_MONOTONIC_COARSE
 * CLOCK_MONOTONIC_RAW
 * CLOCK_BOOTTIME
 * CLOCK_PROCESS_CPUTIME_ID
 * CLOCK_THREAD_CPUTIME_ID
 *
 * The program clock_res.c can be used to determine the resolution of these
 * timers on a particular platform. COARSE timers have lower resolutions.
 */
#define TIMER CLOCK_MONOTONIC

/* Filenames of different types. */
#define CSV_FILENAME   "results.csv"
#define JSON_FILENAME  "results.json"
#define LATEX_FILENAME "results.tex"

/* The name of this program. */
const char *program_name;

/* Run in verbose, quiet or regular mode. */
int verbose, quiet;

/* Prints usage information for this program exit. */
void print_usage (FILE *stream, int exit_code);

/* Parse a command from the user into a format suitable for execvp. */
void parse_command(char *line, char **argv);

/* Execute and time the command the user wishes to measure. */
int execute(char **argv, const int iterations, result_t *result);

/* Calculate the difference between two points in time. */
struct timespec diff(struct timespec start, struct timespec end);


int main(int argc, char **argv) {
    /* Iterations to measure. */
    int iterations = DEFAULT_ITERATIONS;

    /* Command (and arguments) to be measured. */
    char *command = NULL;
    char *args[MAX_ARGS];

    /* Output type. Not implemented yet. TODO.
    int latex, csv, json;
    */

    /* Valid short options. */
    const char *short_options = "hc:i:ljsvq";
    int next_opt, i;

    /* Valid long options. */
    const struct option long_options[] = {
        { "help",       0, NULL, 'h' },
        { "command",    1, NULL, 'c' },
        { "iterations", 1, NULL, 'i' },
        { "latex",      0, NULL, 'l' },
        { "json",       0, NULL, 'j' },
        { "csv",        0, NULL, 's' },
        { "verbose",    0, NULL, 'v' },
        { "quiet",      0, NULL, 'q' },
        { NULL, 0, NULL, 0 }
    };

    program_name = argv[0]; /* Global. */

    do {
        next_opt = getopt_long(argc, argv, short_options, long_options, NULL);
        switch (next_opt) {
            case 'h': /* -h or --help */
               print_usage (stdout, 0);
               break;
            case 'c': /* -c or --command */
               command = optarg;
               break;
            case 'i': /* -i or --iterations */
               iterations = atoi(optarg);
               break;
            case 'l': /* -l or --latex */
               /* latex = 1; */
               printf("LaTeX output not implemented.\n"); /* TODO */
               break;
            case 'j': /* -j or --json */
               /* json = 1; */
               printf("JSON output not implemented.\n"); /* TODO */
               break;
            case 's': /* -s or --csv */
               /* csv = 1; */
               printf("CSV output not implemented.\n"); /* TODO */
               break;
            case 'v': /* -v or --verbose */
               verbose = 1; /* Global. */
               break;
            case 'q': /* -q or --quiet */
               quiet = 1; /* Global. */
               break;
            case -1:
                break;
            default:
                print_usage (stderr, 1);
                return 1;
        }
    } while (next_opt != -1);

    if (verbose && quiet) {
        errno = EINVAL;
        perror("Cannot be both verbose and quiet");
        exit(EXIT_FAILURE);
        return 1;
    }

    if (iterations < 1) {
        errno = EINVAL;
        perror("Must perform at least one experiment");
        exit(EXIT_FAILURE);
        return 1;
    }

    if (command == NULL) {
        errno = EINVAL;
        perror("Must specify a command to measure");
        exit(EXIT_FAILURE);
        return 1;
    }

    /* Allocate an array of results. */
    result_t **results = malloc(sizeof(result_t*) * (iterations + 1));
    for (i = 0; i < iterations; i++) {
        results[i] = result_new();
    }
    results[i] = (result_t*)NULL;

    /* Parse the command we are going to execute. */
    parse_command(command, args);

    /* Run experiments. */
    for (i = 0; i < iterations; i++) {
        if (verbose) {
            printf("\nRunning experiment: %d.\n", i);
        }
        if (execute(args, iterations, results[i]) != 0) {
            fprintf(stderr,
                    "COMMAND ( %s ) failed: %s\n",
                    command,
                    strerror(EPIPE));
            exit(EXIT_FAILURE);
            return 1;
        }
    }

    /* Allocate memory for a summary of the results. */
    statistics_t *stats = statistics_new();

    /* Summarise results statistics. */
    summarise_statistics(results, stats, iterations);
    if (verbose) {
        print_statistics(stats);
    }

    /* TODO: Write results summary to file. */

    /* Deallocate array of results. */
    for (i = 0; i < iterations; i++) {
        result_free(results[i]);
    }

    free(results);
    statistics_free(stats);
    return 0;
}


/* Prints usage information for this program exit. */
void print_usage (FILE *stream, int exit_code) {
    fprintf (stream, "Usage: %s options \n", program_name);
    fprintf (stream,
             " -h --help Display this usage information.\n"
             " -i --iterations Number of iterations to run COMMAND.\n"
             " -c --command COMMAND to be measured.\n"
             " -l --latex Save results as a LaTeX table named results.tex. (not implemented)\n"
             " -j --json Save results as a JSON file named results.json. (not implemented)\n"
             " -s --csv Save results as a CSV file named results.csv. (not implemented)\n"
             " -q --quiet Run in quiet mode. (not implemented)\n"
             " -v --verbose Run in verbose mode.\n\n"
             "Example: Time 100 verbose runs of the command 'sleep 2':\n"
             "   timer -v -i 100 -c 'sleep 2'\n");
    exit (exit_code);
}


/* Parse a command from the user into a format suitable for execvp.
 * TODO: Refactor to use strtok.
 * TODO: Check that the user is not trying to time exit.
 */
void parse_command(char *line, char **argv) {
    if (verbose) {
        printf("Parsing: %s.\n", line);
    }
     while (*line != '\0') {
          while (*line == ' ' || *line == '\t' || *line == '\n')
               *line++ = '\0';
          *argv++ = line;
          while (*line != '\0' && *line != ' ' &&
                 *line != '\t' && *line != '\n')
               line++;
     }
     *argv = '\0';
}


/* Execute and time the command the user wishes to measure. */
int execute(char **argv, const int iterations, result_t *result) {
    struct timespec time_start, time_end, time_diff;
    struct rusage *ru = NULL;
    pid_t pid = NULL;
    int status;

    if (verbose) {
        printf("Executing %s in child process.\n", *argv);
    }

    clock_gettime(TIMER, &time_start);

    /* Execute the command we are measuring. */
    pid = fork();
    if (pid < 0) {
        perror("Could not fork child process.");
        return 1;
    }

    if (pid == 0) { /* Child process. */
        if (quiet) {
            /* TODO: Redirect stdout and stderr. */
        }
        execvp(*argv, argv);
        exit(EXIT_SUCCESS);
    }

    /* Parent process. */
    ru = malloc(sizeof(struct rusage));
    wait4(pid, &status, 0, ru);
    clock_gettime(TIMER, &time_end);

    if (status != 0) {
        free(ru);
        perror(strcat("Error when running ", *argv));
        return 1;
    }

    /* Calculate wall clock time and copy results into a results_t. */
    time_diff = diff(time_start, time_end);
    result->seconds = time_diff.tv_sec;
    result->nanoseconds = time_diff.tv_nsec;
    *result->user_time = ru->ru_utime;
    *result->sys_time = ru->ru_stime;
    result->max_set_size = ru->ru_maxrss;
    result->soft_fault = ru->ru_minflt;
    result->hard_fault = ru->ru_majflt;
    result->in_block = ru->ru_inblock;
    result->out_block = ru->ru_oublock;
    result->vol_con_switches = ru->ru_nvcsw;
    result->invol_con_switches = ru->ru_nivcsw;

    /* Once the results of getrusage() are in the result_t we do not need the
     * original copy of that data.
     */
    free(ru);

    if (verbose) {
        print_result(result);
    }

    return 0;
}


/* Calculate the difference between two points in time. */
struct timespec diff(struct timespec start, struct timespec end) {
    struct timespec temp;
    if ((end.tv_nsec-start.tv_nsec) < 0) {
        temp.tv_sec = end.tv_sec-start.tv_sec - 1;
        temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return temp;
}
