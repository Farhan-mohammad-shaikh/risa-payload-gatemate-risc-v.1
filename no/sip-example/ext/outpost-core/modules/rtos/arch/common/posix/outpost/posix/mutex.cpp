/*
 * Copyright (c) 2013-2017, 2019, Fabian Greif
 * Copyright (c) 2015, Jan Sommer
 * Copyright (c) 2021, Felix Passenberg
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "mutex.h"

#include "internal/time.h"

#include <outpost/rtos/failure_handler.h>

#include <time.h>

using namespace outpost::posix;

Mutex::Mutex()
{
    pthread_mutexattr_t attr;
    if (pthread_mutexattr_init(&attr) != 0)
    {
        rtos::FailureHandler::fatal(
                rtos::FailureCode::resourceAllocationFailed(rtos::Resource::mutex));
    }

    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mMutex, &attr);

    if (pthread_mutexattr_destroy(&attr) != 0)
    {
        rtos::FailureHandler::fatal(
                rtos::FailureCode::resourceAllocationFailed(rtos::Resource::mutex));
    }
}

bool
Mutex::acquire(outpost::time::Duration timeout)
{
    bool success = false;
    if (timeout >= time::Duration::myriad())
    {
        success = acquire();
    }
    else
    {
        timespec time = rtos::toAbsoluteTime(CLOCK_REALTIME, timeout);
        success = (pthread_mutex_timedlock(&mMutex, &time) == 0);
    }
    return success;
}
