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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>

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

/* Results from a single measurement. */
typedef struct result_t {
    /* Timings from a nanosecond-resolution monotonic clock. */
    long long seconds, nanoseconds;
    /* Time spent in user and system mode, according to rusage. */
    struct timeval *user_time, *sys_time;
    /* Data from the operating system. */
    long int max_set_size, soft_fault, hard_fault, in_block, out_block, \
        vol_con_switches, invol_con_switches;
} result_t;

/* Allocate and free result types. */
result_t * result_new();
void result_free (result_t* result);

/* Print results of a single measurement. */
void print_result(result_t *result);

/* Summary of results from experiments. */
typedef struct statistics_t {
    /* Timings from a nanosecond-resolution monotonic clock. */
    long double seconds_mean, seconds_stdev, \
        nanoseconds_mean, nanoseconds_stdev;
    /* Time spent in user and system mode, according to rusage. */
    long double user_time_seconds_mean, user_time_seconds_stdev, \
        sys_time_seconds_mean, sys_time_seconds_stdev; /* TODO: Missing usec. */
    /* Data from the operating system. */
    long double max_set_size_mean, max_set_size_stdev, \
        soft_fault_mean, soft_fault_stdev, \
        hard_fault_mean, hard_fault_stdev, \
        in_block_mean, in_block_stdev, \
        out_block_mean, out_block_stdev, \
        vol_con_switches_mean, vol_con_switches_stdev, \
        invol_con_switches_mean, invol_con_switches_stdev;
} statistics_t;

/* Allocate and free statistics types. */
statistics_t * statistics_new();
void statistics_free (statistics_t* statistics);

/* Print summary of statistics. */
void print_statistics(statistics_t *stats);

/* Calculate means and standard deviations. */
void summarise_statistics(result_t **results,
                          statistics_t *stats,
                          int num_experiments);
/* TODO: Confidence intervals. */

/* The name of this program. */
const char *program_name;

/* Run in verbose, quiet or regular mode. */
int verbose, quiet;

/* Print a horizontal rule. */
void hrule();

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


/* Allocate memory for a result_t type. */
result_t * result_new() {
    result_t *result = (result_t*)malloc(sizeof(result_t));
    result->user_time = malloc(sizeof(struct timeval));
    result->sys_time = malloc(sizeof(struct timeval));
    return result;
}


/* Free the memory allocated to a result_t type. */
void result_free (result_t *result) {
    free(result->sys_time);
    free(result->user_time);
    free(result);
}


/* Allocate memory for a statistics_t type. */
statistics_t * statistics_new() {
    statistics_t *statistics = (statistics_t*)malloc(sizeof(statistics_t));
    return statistics;
}


/* Free the memory allocated to a statistics_t type. */
void statistics_free (statistics_t *statistics) {
    free(statistics);
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


/* Print a horizontal rule. */
void hrule() {
    printf("----------------------------------------------------------------\n");
}


/* Print results of a single measurement. */
void print_result(result_t *result) {
    long double wc_s = ( (long double)result->seconds +
                         ((long double)result->nanoseconds /
                          (long double)1000000000) );
    long double user_s = ( (long double)result->user_time->tv_sec +
                           ((long double)result->user_time->tv_usec /
                            (long double)1000000) );
    long double sys_s = ( (long double)result->sys_time->tv_sec +
                         ((long double)result->sys_time->tv_usec /
                          (long double)1000000) );

    printf("Wall clock time: %lld seconds %lld nanoseconds or %.9Lf seconds.\n",
           result->seconds, result->nanoseconds, wc_s);
    printf("User time: %ld second %ld microseconds or %.6Lf seconds.\n",
           result->user_time->tv_sec, result->user_time->tv_usec, user_s);
    printf("System time: %ld second %ld microseconds or %.6Lf seconds.\n",
           result->sys_time->tv_sec, result->sys_time->tv_usec, sys_s);
    printf("%-10ld Maximum resident set size (Kb).\n",
           result->max_set_size);
    printf("%-10ld Page reclaims (soft page faults).\n",
           result->soft_fault);
    printf("%-10ld Page faults (hard page faults).\n",
           result->hard_fault);
    printf("%-10ld Block input operations.\n",
           result->in_block);
    printf("%-10ld Block output operations.\n",
           result->out_block);
    printf("%-10ld Voluntary context switches.\n",
           result->vol_con_switches);
    printf("%-10ld Involuntary context switches.\n",
           result->invol_con_switches);
}


/* Print summary of statistics. */
void print_statistics(statistics_t *stats) {
    printf("\n");
    hrule();
    printf(" %-30s | %-15s | %-20s \n",
           "Measurement", "Mean", "Std. deviation");
    hrule();
    printf(" %-30s | %-15Lf | %-20Lf \n",
           "Wall clock time (s)",
           stats->seconds_mean, stats->seconds_stdev);
    printf(" %-30s | %-15Lf | %-20Lf \n",
           "Wall clock time (ns)",
           stats->nanoseconds_mean, stats->nanoseconds_stdev);
    printf(" %-30s | %-15Lf | %-20Lf \n",
           "User time (s)",
           stats->user_time_seconds_mean, stats->user_time_seconds_stdev);
    printf(" %-30s | %-15Lf | %-20Lf \n",
           "System time (us)",
           stats->sys_time_seconds_mean, stats->sys_time_seconds_stdev);
    printf(" %-30s | %-15Lf | %-20Lf \n",
           "Maximum resident size (KB)",
           stats->max_set_size_mean, stats->max_set_size_stdev);
    printf(" %-30s | %-15Lf | %-20Lf \n",
           "Soft page faults", stats->soft_fault_mean, stats->soft_fault_stdev);
    printf(" %-30s | %-15Lf | %-20Lf \n",
           "Hard page faults",
           stats->hard_fault_mean, stats->hard_fault_stdev);
    printf(" %-30s | %-15Lf | %-20Lf \n",
           "Number of input operations",
           stats->in_block_mean, stats->in_block_stdev);
    printf(" %-30s | %-15Lf | %-20Lf \n",
           "Number of output operations",
           stats->out_block_mean, stats->out_block_stdev);
    printf(" %-30s | %-15Lf | %-20Lf \n",
           "Voluntary context switches",
           stats->vol_con_switches_mean, stats->vol_con_switches_stdev);
    printf(" %-30s | %-15Lf | %-20Lf \n",
           "Involuntary context switches",
           stats->invol_con_switches_mean, stats->invol_con_switches_stdev);
    hrule();
    return;
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


/* Given an array of results, fill in the averages in a statistics summary.
 *
 * Note that the number of experiments is guaranteed to be >= 1.
 */
void summarise_statistics(result_t **results,
                          statistics_t *stats,
                          int num_experiments) {

    long double seconds_total = 0, /* Temporary total. */
            nanoseconds_total = 0,
            user_time_seconds_total = 0,
            sys_time_seconds_total = 0,
            max_set_size_total = 0,
            soft_fault_total = 0,
            hard_fault_total = 0,
            in_block_total = 0,
            out_block_total = 0,
            vol_con_switches_total = 0,
            invol_con_switches_total = 0,
            /* Temporary variances. */
            seconds_nvar = 0,
            nanoseconds_nvar = 0,
            user_time_seconds_nvar = 0,
            sys_time_seconds_nvar = 0,
            max_set_size_nvar = 0,
            soft_fault_nvar = 0,
            hard_fault_nvar = 0,
            in_block_nvar = 0,
            out_block_nvar = 0,
            vol_con_switches_nvar = 0,
            invol_con_switches_nvar = 0;
    int i = 0;
    const long double recip = 1.0 / (long double)num_experiments;

    /* Calculate totals for all values. */
    for (i = 0; i < num_experiments; i++) {
        seconds_total            += results[i]->seconds;
        nanoseconds_total        += results[i]->nanoseconds;
        user_time_seconds_total  += results[i]->user_time->tv_sec;
        sys_time_seconds_total   += results[i]->sys_time->tv_sec;
        max_set_size_total       += results[i]->max_set_size;
        soft_fault_total         += results[i]->soft_fault;
        hard_fault_total         += results[i]->hard_fault;
        in_block_total           += results[i]->in_block;
        out_block_total          += results[i]->out_block;
        vol_con_switches_total   += results[i]->vol_con_switches;
        invol_con_switches_total += results[i]->invol_con_switches;
    }

    /* Set means for all values. */
    stats->seconds_mean = (long double) seconds_total / num_experiments;
    stats->nanoseconds_mean = (long double) nanoseconds_total / num_experiments;
    stats->user_time_seconds_mean = (long double) (user_time_seconds_total /
                                                   num_experiments);
    stats->sys_time_seconds_mean = (long double) (sys_time_seconds_total /
                                                  num_experiments);
    stats->max_set_size_mean = (long double) (max_set_size_total /
                                              num_experiments);
    stats->soft_fault_mean = (long double) soft_fault_total / num_experiments;
    stats->hard_fault_mean = (long double) hard_fault_total / num_experiments;
    stats->in_block_mean = (long double) in_block_total / num_experiments;
    stats->out_block_mean = (long double) out_block_total / num_experiments;
    stats->vol_con_switches_mean = (long double) (vol_con_switches_total /
                                                  num_experiments);
    stats->invol_con_switches_mean = (long double) (invol_con_switches_total /
                                                    num_experiments);

    /* Calculate n * variance for all values. */
    for (i = 0; i < num_experiments; i++) {
        seconds_nvar            += pow(results[i]->seconds -
                                       stats->seconds_mean, 2);
        nanoseconds_nvar        += pow(results[i]->nanoseconds -
                                       stats->nanoseconds_mean, 2);
        user_time_seconds_nvar  += pow(results[i]->user_time->tv_sec -
                                       stats->user_time_seconds_mean, 2);
        sys_time_seconds_nvar   += pow(results[i]->sys_time->tv_sec -
                                       stats->sys_time_seconds_mean, 2);
        max_set_size_nvar       += pow(results[i]->max_set_size -
                                       stats->max_set_size_mean, 2);
        soft_fault_nvar         += pow(results[i]->soft_fault -
                                       stats->soft_fault_mean, 2);
        hard_fault_nvar         += pow(results[i]->hard_fault -
                                       stats->hard_fault_mean, 2);
        in_block_nvar           += pow(results[i]->in_block -
                                       stats->in_block_mean, 2);
        out_block_nvar          += pow(results[i]->out_block -
                                       stats->out_block_mean, 2);
        vol_con_switches_nvar   += pow(results[i]->vol_con_switches -
                                       stats->vol_con_switches_mean, 2);
        invol_con_switches_nvar += pow(results[i]->invol_con_switches -
                                       stats->invol_con_switches_mean, 2);
    }

    /* Set standard deviations for all values. */
    stats->seconds_stdev            = sqrt(recip * seconds_nvar);
    stats->nanoseconds_stdev        = sqrt(recip * nanoseconds_nvar);
    stats->user_time_seconds_stdev  = sqrt(recip * user_time_seconds_nvar);
    stats->sys_time_seconds_stdev   = sqrt(recip * sys_time_seconds_nvar);
    stats->max_set_size_stdev       = sqrt(recip * max_set_size_nvar);
    stats->soft_fault_stdev         = sqrt(recip * soft_fault_nvar);
    stats->hard_fault_stdev         = sqrt(recip * hard_fault_nvar);
    stats->in_block_stdev           = sqrt(recip * in_block_nvar);
    stats->out_block_stdev          = sqrt(recip * out_block_nvar);
    stats->vol_con_switches_stdev   = sqrt(recip * vol_con_switches_nvar);
    stats->invol_con_switches_stdev = sqrt(recip * invol_con_switches_nvar);

    return;
}

/* TODO: Implement confidence intervals. */
