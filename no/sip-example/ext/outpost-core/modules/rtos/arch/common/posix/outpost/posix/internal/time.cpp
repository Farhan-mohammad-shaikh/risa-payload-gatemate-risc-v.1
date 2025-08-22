/*
 * Copyright (c) 2019, Fabian Greif
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "time.h"

#include <unistd.h>

#include <outpost/rtos/failure_handler.h>

#include <errno.h>

namespace outpost
{
namespace rtos
{
static constexpr int32_t nanosecondsPerSecond = time::Duration::nanosecondsPerMicrosecond
                                                * time::Duration::microsecondsPerMillisecond
                                                * time::Duration::millisecondsPerSecond;

timespec
getTime(clockid_t clock)
{
    timespec time;
    int result = clock_gettime(clock, &time);
    if (result != 0)
    {
        FailureHandler::fatal(FailureCode::genericRuntimeError(Resource::clock));
    }
    return time;
}

timespec
toRelativeTime(time::Duration duration)
{
    // Split into seconds and sub-seconds to avoid an overflow
    // when converting the sub-seconds to nanoseconds.
    int64_t seconds = duration.seconds();
    auto subSeconds = duration - time::Seconds(seconds);

    // Convert to sub-seconds nanoseconds, this will never exceed a
    // value of 1,000,000,000
    int32_t nanoseconds = subSeconds.microseconds() * time::Duration::nanosecondsPerMicrosecond;

    timespec relativeTime = {
            // seconds
            static_cast<time_t>(seconds),

            // remaining nanoseconds
            static_cast<long int>(nanoseconds),
    };

    return relativeTime;
}

timespec
toAbsoluteTime(clockid_t clock, time::Duration duration)
{
    timespec absoluteTime = getTime(clock);
    timespec relative = toRelativeTime(duration);

    addTime(absoluteTime, relative);

    return absoluteTime;
}

void
addTime(timespec& result, const timespec& increment)
{
    result.tv_nsec += increment.tv_nsec;
    if (result.tv_nsec >= nanosecondsPerSecond)
    {
        result.tv_sec += 1;
        result.tv_nsec -= nanosecondsPerSecond;
    }
    else if (result.tv_nsec < 0)
    {
        result.tv_sec -= 1;
        result.tv_nsec += nanosecondsPerSecond;
    }
    result.tv_sec += increment.tv_sec;
}

void
sleepUntilAbsoluteTime(clockid_t clock, const timespec& deadline)
{
    int result;
    do
    {
        result = clock_nanosleep(clock, TIMER_ABSTIME, &deadline, nullptr);

        // EINTR is returned when the sleep is interrupted by a signal
        // handler. In this case the sleep can be restarted.
        //
        // Any other result unequal zero is an failure which can not be
        // resolved here and therefore triggers the fatal error handler.
        if (result != 0 && result != EINTR)
        {
            FailureHandler::fatal(FailureCode::genericRuntimeError());
        }
    } while (result != 0);
}

}  // namespace rtos
}  // namespace outpost
