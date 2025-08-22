/*
 * Copyright (c) 2020, Jan Malburg
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "barrier.h"

outpost::rtos::Barrier::Barrier(uint32_t numberOfThreads) :
    mWaitSemaphore(::outpost::rtos::BinarySemaphore::State::acquired),
    mReleaseSemaphore(::outpost::rtos::BinarySemaphore::State::released),
    mMaxWaiting(numberOfThreads),
    mCurrentlyWaiting(0){};

void
outpost::rtos::Barrier::wait()
{
    // To ensure that on does not start waiting while the rest is not done releasing.
    mReleaseSemaphore.acquire();

    mCounterMutex.acquire();
    uint32_t myNumber = ++mCurrentlyWaiting;
    mCounterMutex.release();

    if (myNumber < mMaxWaiting)
    {
        mReleaseSemaphore.release();

        // wait till we start releasing
        mWaitSemaphore.acquire();

        // release the next one
        mWaitSemaphore.release();

        mCounterMutex.acquire();
        myNumber = --mCurrentlyWaiting;
        mCounterMutex.release();

        if (myNumber == 0)
        {
            // you are the last
            // get the Semaphore values to initial value
            mWaitSemaphore.acquire();
            mReleaseSemaphore.release();
        }

        return;
    }
    else
    {
        // release the next one
        mWaitSemaphore.release();

        mCounterMutex.acquire();
        myNumber = --mCurrentlyWaiting;
        mCounterMutex.release();

        // for the special case maxWaiting == 1
        if (myNumber == 0)
        {
            // you are the last
            // get the Semaphore values to initial value
            mWaitSemaphore.acquire();
            mReleaseSemaphore.release();
        }

        return;
    }
}
