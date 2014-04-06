/* Allocate, free, print and manipulate data related to benchmark results.
 *
 * (c) Sarah Mount <s.mount@wlv.ac.uk> 2014
 */

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


/* Allocate and free result types. */
result_t * result_new();
void result_free (result_t* result);


/* Print results of a single measurement. */
void print_result(result_t *result);


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
