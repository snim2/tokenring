/* Allocate, free, print and manipulate data related to benchmark results.
 *
 * (c) Sarah Mount <s.mount@wlv.ac.uk> 2014
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "timer_data.h"

/* Print a horizontal rule. */
void hrule();


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


/* Write out an array of result_ts to a CSV file. */
int result_write_csv(result_t **result, char *filename, int num_experiments) {
    unsigned int i = 0;
    FILE *fp;
    fp = fopen(filename,"w+");
    /* Write header. */
    fprintf(fp, "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
            "Experiment",
            "Wall clock time (s)",
            "Wall clock time (ns)",
            "User time (s)",
            "System time (s)",
            "Maximum resident set size (KB)",
            "Page reclaims (soft page faults)",
            "Page faults (hard page faults)",
            "Block input operations",
            "Block output operations",
            "Voluntary context switches",
            "Involuntary context switches");
    /* Write data. */
    for (i = 0; i < num_experiments; i++) {
        fprintf(fp,
                "%d,%lld,%lld,%lld,%lld,%ld,%ld,%ld,%ld,%ld,%ld,%ld\n",
                i,
                result[i]->seconds,
                result[i]->nanoseconds,
                (long long int)result[i]->user_time->tv_sec,
                (long long int)result[i]->sys_time->tv_sec,
                result[i]->max_set_size,
                result[i]->soft_fault,
                result[i]->hard_fault,
                result[i]->in_block,
                result[i]->out_block,
                result[i]->vol_con_switches,
                result[i]->invol_con_switches);
    }
    fclose(fp);
    return EXIT_SUCCESS;
}


/* Write out an array of result_ts to a JSON file. */
int result_write_json(result_t **result, char *filename, int num_experiments) {
    printf("Not implemented.\n");
    return EXIT_FAILURE;
}


/* Write out an array of result_ts to a LaTeX file. */
int result_write_latex(result_t **result, char *filename, int num_experiments) {
    printf("Not implemented.\n");
    return EXIT_FAILURE;
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


/* Print a horizontal rule. */
void hrule() {
    printf("----------------------------------------------------------------\n");
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


/* Write out a statistics_t struct to a CSV file. */
int statistics_write_csv(statistics_t *stats, char *filename, int num_experiments) {
    FILE *fp;
    fp = fopen(filename,"w+");
    /* Write header. */
    fprintf(fp,
            "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
/*            "Number of experiments", */
            "Mean wall clock time (s)",
            "Std. dev. wall clock time (s)",
            "Mean wall clock time (ns)",
            "Std. dev. wall clock time (ns)",
            "Mean user time (s)",
            "Std. dev. user time (s)",
            "Mean system time (s)",
            "Std. dev. system time (s)",
            "Mean maximum resident set size (KB)",
            "Std. dev. maximum resident set size (KB)",
            "Mean page reclaims (soft page faults)",
            "Std. dev. page reclaims (soft page faults)",
            "Mean page faults (hard page faults)",
            "Std. dev. page faults (hard page faults)",
            "Mean block input operations",
            "Std. dev. block input operations",
            "Mean block output operations",
            "Std. dev. block output operations",
            "Mean voluntary context switches",
            "Std. dev. voluntary context switches",
            "Mean involuntary context switches",
            "Std. dev. involuntary context switches");
    /* Write data. */
    fprintf(fp,
            "%Lf,%Lf,%Lf,%Lf,%Lf,%Lf,%Lf,%Lf,%Lf,%Lf,%Lf,%Lf,%Lf,%Lf,%Lf,%Lf,%Lf,%Lf,%Lf,%Lf,%Lf,%Lf\n",
            stats->seconds_mean,
            stats->seconds_stdev,
            stats->nanoseconds_mean,
            stats->nanoseconds_stdev,
            stats->user_time_seconds_mean,
            stats->user_time_seconds_stdev,
            stats->sys_time_seconds_mean,
            stats->sys_time_seconds_stdev,
            stats->max_set_size_mean,
            stats->max_set_size_stdev,
            stats->soft_fault_mean,
            stats->soft_fault_stdev,
            stats->hard_fault_mean,
            stats->hard_fault_stdev,
            stats->in_block_mean,
            stats->in_block_stdev,
            stats->out_block_mean,
            stats->out_block_stdev,
            stats->vol_con_switches_mean,
            stats->vol_con_switches_stdev,
            stats->invol_con_switches_mean,
            stats->invol_con_switches_stdev);

    fclose(fp);
    return EXIT_SUCCESS;


    return EXIT_SUCCESS;
}


/* Write out a statistics_t struct to a CSV file. */
int statistics_write_json(statistics_t *stats, char *filename, int num_experiments) {
    printf("Not implemented.\n");
    return EXIT_FAILURE;
}


/* Write out a statistics_t struct to a CSV file. */
int statistics_write_latex(statistics_t *stats, char *filename, int num_experiments) {
    printf("Not implemented.\n");
    return EXIT_FAILURE;
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
