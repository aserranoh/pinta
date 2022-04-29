
#include "pinta.h"

#include <time.h>

#define CLOCK               CLOCK_MONOTONIC
#define NSEC_PER_SEC        1000000000L
#define as_nanoseconds(ts)  \
    ((ts).tv_sec * (uint64_t)NSEC_PER_SEC + (ts).tv_nsec)

void
pinta_clock_tick(struct pinta_clock *clk)
{
    // Obtain the first time reference
    if (clk->time == 0) {
        struct timespec current_time_ts;
        clock_gettime(CLOCK, &current_time_ts);
        clk->time = as_nanoseconds(current_time_ts);
    }

    // Compute the next waking up time
    uint64_t next_time = clk->time + clk->interval;
    struct timespec target_time;
    target_time.tv_sec = next_time / NSEC_PER_SEC;
    target_time.tv_nsec = next_time % NSEC_PER_SEC;

    // Sleep until next waking up time
    clock_nanosleep(CLOCK, TIMER_ABSTIME, &target_time, NULL);

    // Update the current time
    clk->time = next_time;
}

void
pinta_clock_init(struct pinta_clock *clk, double fps)
{
    clk->time = 0;
    clk->interval = (uint64_t)(NSEC_PER_SEC/fps);
}

