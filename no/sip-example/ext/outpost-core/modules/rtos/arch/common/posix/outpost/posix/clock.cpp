/*
 * Copyright (c) 2013-2017, 2019, Fabian Greif
 * Copyright (c) 2019, Ulmer, Moritz Christian
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "clock.h"

#include "internal/time.h"

#include <time.h>

using namespace outpost::time;
using namespace outpost::posix;

// ---------------------------------------------------------------------------
static constexpr int64_t microsecondsPerSecond =
        Duration::microsecondsPerMillisecond * Duration::millisecondsPerSecond;

SpacecraftElapsedTime
SystemClock::now() const
{
    timespec time = rtos::getTime(CLOCK_MONOTONIC);

    // Convert to microseconds. Store seconds in a uint64_t to avoid an overflow when converting
    // to microseconds on 32-bit systems.
    uint64_t secondsInMicroseconds = time.tv_sec;
    secondsInMicroseconds *= microsecondsPerSecond;
    uint64_t nanosecondsInMicroseconds = time.tv_nsec;
    nanosecondsInMicroseconds /= time::Duration::nanosecondsPerMicrosecond;
    uint64_t totalMicroseconds = secondsInMicroseconds + nanosecondsInMicroseconds;

    return outpost::time::SpacecraftElapsedTime::afterEpoch(
            outpost::time::Microseconds(totalMicroseconds));
}
