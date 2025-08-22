/*
 * Copyright (c) 2013-2017, 2019, Fabian Greif
 * Copyright (c) 2020, Jan Malburg
 * Copyright (c) 2021, Felix Passenberg
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2023, Passenberg, Felix Constantin
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "semaphore.h"

#include "internal/time.h"

#include <outpost/rtos/failure_handler.h>

#include <time.h>

using outpost::posix::BinarySemaphore;
using outpost::posix::Semaphore;

// ----------------------------------------------------------------------------
Semaphore::Semaphore(uint32_t count) : mSid()
{
    // shared semaphores are disabled
    if (sem_init(&mSid, 0, count) != 0)
    {
        rtos::FailureHandler::fatal(
                rtos::FailureCode::resourceAllocationFailed(rtos::Resource::semaphore));
    }
}

Semaphore::~Semaphore()
{
    sem_destroy(&mSid);
}

bool
Semaphore::acquire(time::Duration timeout)
{
    bool success = false;
    if (timeout >= time::Duration::myriad())
    {
        success = acquire();
    }
    else
    {
        timespec t = rtos::toAbsoluteTime(CLOCK_REALTIME, timeout);
        success = (sem_timedwait(&mSid, &t) == 0);
    }
    return success;
}

// ----------------------------------------------------------------------------
BinarySemaphore::BinarySemaphore() : mValue(BinarySemaphore::State::released)
{
    pthread_mutex_init(&mMutex, NULL);
    pthread_cond_init(&mSignal, NULL);
}

BinarySemaphore::BinarySemaphore(State::Type initial) : mValue(initial)
{
    pthread_mutex_init(&mMutex, NULL);
    pthread_cond_init(&mSignal, NULL);
}

BinarySemaphore::~BinarySemaphore()
{
    pthread_cond_destroy(&mSignal);
    pthread_mutex_destroy(&mMutex);
}

bool
BinarySemaphore::acquire()
{
    pthread_mutex_lock(&mMutex);
    pthread_cleanup_push(unlockMutex, &mMutex);
    while (mValue == State::acquired)
    {
        pthread_cond_wait(&mSignal, &mMutex);
    }
    mValue = State::acquired;
    pthread_cleanup_pop(1);
    return true;
}

bool
BinarySemaphore::acquire(time::Duration timeout)
{
    bool success = false;
    if (timeout >= time::Duration::myriad())
    {
        success = acquire();
    }
    else
    {
        timespec time = rtos::toAbsoluteTime(CLOCK_REALTIME, timeout);
        bool timedout = false;
        pthread_mutex_lock(&mMutex);
        pthread_cleanup_push(unlockMutex, &mMutex);
        while (mValue == State::acquired && !timedout)
        {
            if (pthread_cond_timedwait(&mSignal, &mMutex, &time) != 0)
            {
                // Timeout or other error has occurred
                // => semaphore can't be acquired
                timedout = true;
            }
        }
        if (!timedout)
        {
            mValue = State::acquired;
        }
        pthread_cleanup_pop(1);
        success = !timedout;
    }
    return success;
}

void
BinarySemaphore::release()
{
    pthread_mutex_lock(&mMutex);
    mValue = State::released;
    pthread_cond_signal(&mSignal);
    pthread_mutex_unlock(&mMutex);
}

void
BinarySemaphore::unlockMutex(void* mutex)
{
    pthread_mutex_unlock(reinterpret_cast<pthread_mutex_t*>(mutex));
}
