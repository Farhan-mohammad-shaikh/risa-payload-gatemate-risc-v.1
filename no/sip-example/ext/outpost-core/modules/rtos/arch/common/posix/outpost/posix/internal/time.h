/*
 * Copyright (c) 2014-2017, 2019, Fabian Greif
 * Copyright (c) 2021, Jan Malburg
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_RTOS_POSIX_TIME_H
#define OUTPOST_RTOS_POSIX_TIME_H

#include <outpost/time/duration.h>

#include <time.h>

namespace outpost
{
namespace rtos
{
/**
 * Query the current POSIX time.
 */
timespec
getTime(clockid_t clock);

/**
 * Convert a duration to a C `timespec` type.
 *
 * \warning
 *      For negative durations, `tv_sec` and `tv_nsec`
 *      will be negative.
 */
timespec
toRelativeTime(time::Duration duration);

/**
 * Convert a duration to an absolute time point.
 *
 * Uses the current time as a reference point for the given duration.
 */
timespec
toAbsoluteTime(clockid_t clock, time::Duration duration);

/**
 * Add two `timespec` values.
 *
 * \param[inout] result
 * \param[in]    increment
 *      Relative time which is added to the result
 */
void
addTime(timespec& result, const timespec& increment);

/**
 * Compare two times.
 *
 * \param time
 * \param other
 *
 * \retval true time is bigger or equal than other
 * \retval false time is lower than other
 */
static inline bool
isBigger(const timespec& time, const timespec& other)
{
    if (time.tv_sec > other.tv_sec)
    {
        return true;
    }
    else if (time.tv_sec == other.tv_sec)
    {
        if (time.tv_nsec >= other.tv_nsec)
        {
            return true;
        }
    }

    return false;
}

/**
 * Sleep until the supplied absolute time.
 *
 * Handles interruptions through signal by restarting
 * the sleep.
 */
void
sleepUntilAbsoluteTime(clockid_t clock, const timespec& deadline);

}  // namespace rtos
}  // namespace outpost

#endif
