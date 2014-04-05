/* Print a table of available timer frequencies. Linux only.
 *
 * (c) Sarah Mount <s.mount@wlv.ac.uk> 2014.
 */
#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>
#include <time.h>


/* Print a horizontal rule. */
void hrule();

/* Print a fancy header above the table of results. */
void print_header();

/* Print resolutions of all clocks in the lookup table. */
void print_resolutions();

/* Lookup table containing clock IDs and readable names. */
typedef struct lut_t { clockid_t id; const char *name; } lut_t;


/* All available clocks listed in the man page for clock_getres. */
lut_t clocks[] = {
    { CLOCK_REALTIME,           "CLOCK_REALTIME" },
    { CLOCK_REALTIME_COARSE,    "CLOCK_REALTIME_COARSE" },
    { CLOCK_MONOTONIC,          "CLOCK_MONOTONIC" },
    { CLOCK_MONOTONIC_COARSE,   "CLOCK_MONOTONIC_COARSE" },
    { CLOCK_MONOTONIC_RAW,      "CLOCK_MONOTONIC_RAW" },
    { CLOCK_BOOTTIME,           "CLOCK_BOOTTIME" },
    { CLOCK_PROCESS_CPUTIME_ID, "CLOCK_PROCESS_CPUTIME_ID" },
    { CLOCK_THREAD_CPUTIME_ID,  "CLOCK_THREAD_CPUTIME_ID" },
    { (clockid_t) -1,           NULL },
};


int main(int argc, char** argv) {
    print_header();
    print_resolutions();
    hrule();
    printf("\n");
    return 0;
}


/* Print resolutions of all clocks in the lookup table. */
void print_resolutions() {
    struct timespec spec;
    struct lut_t *p = NULL;
    for (p = clocks; p->id != -1; ++p) {
        if (clock_getres(p->id, &spec) != 0) {
            printf("%s is not supported on this platform.\n", p->name);
        }
        else {
            printf(" %-25s | %-10ld | %-10ld (%g secs)\n",
                   p->name,
                   spec.tv_sec,
                   spec.tv_nsec,
                   (double)spec.tv_nsec / (double)1000000000.0);
        }
    }
}


/* Print a fancy header above the table of results. */
void print_header() {
    struct utsname sysinfo;
    printf("\n");
    hrule();
    if (uname(&sysinfo) == 0) {
        printf("\t%s system clock resolutions\n",
        strcat(sysinfo.sysname, sysinfo.release));
    } else {
        printf("\tSystem clock resolutions\n");
    }
    hrule();
    printf(" %-25s | %-10s | %-10s \n", "Clock", "Seconds", "Nanoseconds");
    hrule();
}


/* Print a horizontal rule. */
void hrule() {
    printf("----------------------------------------------------------------\n");
}
