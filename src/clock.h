#ifndef PINTA_CLOCK_H
#define PINTA_CLOCK_H

#include <cstdint>
#include <time.h>

namespace pinta {

class Clock {

public:

    Clock(double fps);

    void tick();

private:

    static const uint32_t NANOSECONDS_PER_SECOND;
    static const clockid_t CLOCK;

    static inline uint64_t asNanoseconds(struct timespec ts) {return ts.tv_sec * NANOSECONDS_PER_SECOND + ts.tv_nsec;}

    uint64_t interval;
    uint64_t lastTimestamp;

};

}

#endif
