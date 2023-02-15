
#include "pinta/clock.h"

namespace pinta {

const uint32_t Clock::NANOSECONDS_PER_SECOND = 1000000000;
const clockid_t Clock::CLOCK = CLOCK_MONOTONIC;

Clock::Clock(double fps):
    interval(static_cast<uint64_t>(NANOSECONDS_PER_SECOND/fps)), lastTimestamp(0)
{
}

void Clock::tick()
{
    // Obtain the first time reference
    if (lastTimestamp == 0) {
        struct timespec currentTimeTs;
        clock_gettime(CLOCK, &currentTimeTs);
        lastTimestamp = asNanoseconds(currentTimeTs);
    }

    // Compute the next waking up time
    uint64_t nextTime = lastTimestamp + interval;
    struct timespec targetTime;
    targetTime.tv_sec = nextTime / NANOSECONDS_PER_SECOND;
    targetTime.tv_nsec = nextTime % NANOSECONDS_PER_SECOND;

    // Sleep until next waking up time
    clock_nanosleep(CLOCK, TIMER_ABSTIME, &targetTime, 0);

    // Update the current time
    lastTimestamp = nextTime;
}

}
