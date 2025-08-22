/*
 * Copyright (c) 2013-2017, Fabian Greif
 * Copyright (c) 2015, Jan Sommer
 * Copyright (c) 2020, Jan Malburg
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "semaphore.h"

#include "rtems/interval.h"

#include <rtems.h>

#include <outpost/rtos/failure_handler.h>

// ----------------------------------------------------------------------------
outpost::rtos::Semaphore::Semaphore(uint32_t count)
{
    rtems_name name = rtems_build_name('C', 'S', 'E', 'M');

    if (rtems_semaphore_create(name, count, RTEMS_PRIORITY | RTEMS_COUNTING_SEMAPHORE, 1, &mId)
        != RTEMS_SUCCESSFUL)
    {
        FailureHandler::fatal(FailureCode::resourceAllocationFailed(Resource::semaphore));
    }
}

outpost::rtos::Semaphore::~Semaphore()
{
    rtems_semaphore_delete(mId);
}

// ----------------------------------------------------------------------------
bool
outpost::rtos::Semaphore::acquire()
{
    // wait forever
    rtems_interval timeout = RTEMS_NO_TIMEOUT;

    return (rtems_semaphore_obtain(mId, RTEMS_WAIT, timeout) == RTEMS_SUCCESSFUL);
}

bool
outpost::rtos::Semaphore::acquire(time::Duration timeout)
{
    rtems_option waitOption = RTEMS_WAIT;
    if (timeout == time::Duration::zero())
    {
        waitOption = RTEMS_NO_WAIT;
    }  // else use default value RTEMS_WAIT
    return (rtems_semaphore_obtain(mId, waitOption, rtems::getInterval(timeout))
            == RTEMS_SUCCESSFUL);
}

// ----------------------------------------------------------------------------
outpost::rtos::BinarySemaphore::BinarySemaphore()
{
    rtems_name name = rtems_build_name('B', 'S', 'E', 'M');

    if (rtems_semaphore_create(name, 1, RTEMS_PRIORITY | RTEMS_SIMPLE_BINARY_SEMAPHORE, 1, &mId)
        != RTEMS_SUCCESSFUL)
    {
        FailureHandler::fatal(FailureCode::resourceAllocationFailed(Resource::semaphore));
    }
}

// ----------------------------------------------------------------------------
outpost::rtos::BinarySemaphore::BinarySemaphore(State::Type initial)
{
    rtems_name name = rtems_build_name('B', 'S', 'E', 'M');

    if (rtems_semaphore_create(name,
                               (initial == State::acquired) ? 0 : 1,
                               RTEMS_PRIORITY | RTEMS_SIMPLE_BINARY_SEMAPHORE,
                               1,
                               &mId)
        != RTEMS_SUCCESSFUL)
    {
        FailureHandler::fatal(FailureCode::resourceAllocationFailed(Resource::semaphore));
    }
}

outpost::rtos::BinarySemaphore::~BinarySemaphore()
{
    rtems_semaphore_delete(mId);
}

// ----------------------------------------------------------------------------
bool
outpost::rtos::BinarySemaphore::acquire()
{
    // wait forever
    rtems_interval timeout = RTEMS_NO_TIMEOUT;
    return (rtems_semaphore_obtain(mId, RTEMS_WAIT, timeout) == RTEMS_SUCCESSFUL);
}

bool
outpost::rtos::BinarySemaphore::acquire(time::Duration timeout)
{
    rtems_option waitOption = RTEMS_WAIT;
    if (timeout == time::Duration::zero())
    {
        waitOption = RTEMS_NO_WAIT;
    }  // else use default value RTEMS_WAIT
    return (rtems_semaphore_obtain(mId, waitOption, rtems::getInterval(timeout))
            == RTEMS_SUCCESSFUL);
}
