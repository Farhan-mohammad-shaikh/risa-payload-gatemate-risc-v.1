/*
 * Copyright (c) 2016-2017, Fabian Greif
 * Copyright (c) 2021, Felix Passenberg
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_RTOS_RTEMS_INTERVAL_H
#define OUTPOST_RTOS_RTEMS_INTERVAL_H

#include <rtems.h>

#include <outpost/time/duration.h>

namespace outpost
{
namespace rtos
{
namespace rtems
{
/**
 * Converts a duration into a RTEMS interval.
 *
 * An RTEMS interval needs to be at least 1 tick long, otherwise it would
 * specify an infinite interval.
 */
static inline rtems_interval
getInterval(outpost::time::Duration duration)
{
    rtems_interval interval;
    if (duration >= outpost::time::Duration::myriad())
    {
        interval = 0;
    }
    else if (duration < outpost::time::Duration::zero())
    {
        // Negative intervals are invalid and are rounded up to the
        // smallest positive interval.
        interval = 1;
    }
    else
    {
        interval = duration.microseconds() / rtems_configuration_get_microseconds_per_tick();
        if (interval < 1)
        {
            interval = 1;
        }
    }
    return interval;
}

}  // namespace rtems
}  // namespace rtos
}  // namespace outpost

#endif
