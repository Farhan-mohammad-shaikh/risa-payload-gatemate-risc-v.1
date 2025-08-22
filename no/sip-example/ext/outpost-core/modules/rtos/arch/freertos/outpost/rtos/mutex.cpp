/*
 * Copyright (c) 2013-2017, Fabian Greif
 * Copyright (c) 2015, Jan Sommer
 * Copyright (c) 2021, Felix Passenberg
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "mutex.h"

#include <outpost/rtos/failure_handler.h>

// ----------------------------------------------------------------------------
outpost::rtos::Mutex::Mutex()
{
    mHandle = xSemaphoreCreateRecursiveMutex();

    if (mHandle == 0)
    {
        FailureHandler::fatal(FailureCode::resourceAllocationFailed(Resource::mutex));
    }
}

outpost::rtos::Mutex::~Mutex()
{
    vQueueDelete(mHandle);
}

// ----------------------------------------------------------------------------
bool
outpost::rtos::Mutex::acquire()
{
    // wait indefinitely
    return (xSemaphoreTakeRecursive(mHandle, portMAX_DELAY) == pdTRUE);
}

bool
outpost::rtos::Mutex::acquire(time::Duration timeout)
{
    if (timeout > outpost::time::Duration::myriad())
    {
        // prevent calculations if delay is large to prevent overflows
        return acquire();
    }
    return (xSemaphoreTakeRecursive(mHandle, timeout.milliseconds() * configTICK_RATE_HZ / 1000)
            == pdTRUE);
}

void
outpost::rtos::Mutex::release()
{
    xSemaphoreGiveRecursive(mHandle);
}
