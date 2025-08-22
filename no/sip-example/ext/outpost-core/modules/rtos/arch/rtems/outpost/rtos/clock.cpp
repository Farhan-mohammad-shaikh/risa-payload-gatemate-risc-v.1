/*
 * Copyright (c) 2013-2017, 2019, Fabian Greif
 * Copyright (c) 2019, Jan-Gerd Mess
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "clock.h"

#include <rtems.h>

namespace outpost
{
namespace rtos
{
static constexpr int64_t microsecondsPerSecond =
        time::Duration::microsecondsPerMillisecond * time::Duration::millisecondsPerSecond;

outpost::time::SpacecraftElapsedTime
outpost::rtos::SystemClock::now() const
{
    timespec time;
    rtems_clock_get_uptime(&time);

    // convert to microseconds
    int64_t microseconds = (time.tv_nsec / time::Duration::nanosecondsPerMicrosecond)
                           + (time.tv_sec * microsecondsPerSecond);

    return outpost::time::SpacecraftElapsedTime::afterEpoch(
            outpost::time::Microseconds(microseconds));
}

}  // namespace rtos
}  // namespace outpost
