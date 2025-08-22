/*
 * Copyright (c) 2013-2017, Fabian Greif
 * Copyright (c) 2023-2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "mutex.h"

#include "rtems/interval.h"

#include <rtems.h>

#include <outpost/rtos/failure_handler.h>

// ----------------------------------------------------------------------------
outpost::rtos::Mutex::Mutex()
{
    rtems_name name = rtems_build_name('M', 'T', 'X', '-');

    rtems_status_code status = rtems_semaphore_create(
            name, 1, RTEMS_PRIORITY | RTEMS_BINARY_SEMAPHORE | RTEMS_INHERIT_PRIORITY, 1, &mId);
    if (status != RTEMS_SUCCESSFUL)
    {
        FailureHandler::fatal(FailureCode::resourceAllocationFailed(Resource::mutex));
    }
}

outpost::rtos::Mutex::~Mutex()
{
    rtems_semaphore_delete(mId);
}

// ----------------------------------------------------------------------------
bool
outpost::rtos::Mutex::acquire()
{
    // wait forever
    rtems_interval timeout = RTEMS_NO_TIMEOUT;

    return (rtems_semaphore_obtain(mId, RTEMS_WAIT, timeout) == RTEMS_SUCCESSFUL);
}

bool
outpost::rtos::Mutex::acquire(time::Duration timeout)
{
    rtems_option waitOption = RTEMS_WAIT;
    if (timeout == time::Duration::zero())
    {
        waitOption = RTEMS_NO_WAIT;
    }  // else use default value RTEMS_WAIT
    return (rtems_semaphore_obtain(mId, waitOption, rtems::getInterval(timeout))
            == RTEMS_SUCCESSFUL);
}
