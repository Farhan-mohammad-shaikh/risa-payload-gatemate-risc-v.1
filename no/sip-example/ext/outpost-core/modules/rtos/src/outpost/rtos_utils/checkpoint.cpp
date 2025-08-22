/*
 * Copyright (c) 2016-2017, Fabian Greif
 * Copyright (c) 2024, Reinking, Janosch
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "checkpoint.h"

using outpost::rtos_utils::Checkpoint;

Checkpoint::Checkpoint(State initialState) :
    mMutex(),
    mSemaphore((initialState == running) ? outpost::rtos::BinarySemaphore::State::released
                                         : outpost::rtos::BinarySemaphore::State::acquired),
    mState((initialState == running) ? running : suspending)
{
}

Checkpoint::~Checkpoint()
{
}

void
Checkpoint::resume()
{
    mMutex.acquire();
    if (mState != running)
    {
        mState = running;
        mSemaphore.release();
    }
    mMutex.release();
}

void
Checkpoint::suspend()
{
    mMutex.acquire();
    if (mState == running)
    {
        mState = suspending;
        mSemaphore.acquire();
    }
    mMutex.release();
}

Checkpoint::State
Checkpoint::getState() const
{
    outpost::rtos::MutexGuard lock(mMutex);
    return mState;
}

bool
Checkpoint::shouldSuspend() const
{
    outpost::rtos::MutexGuard lock(mMutex);
    return (mState == suspending);
}

void
Checkpoint::pass()
{
    bool waiting = true;
    do
    {
        mMutex.acquire();
        if (mState == running)
        {
            mMutex.release();
            waiting = false;
        }
        else
        {
            mState = suspended;
            mMutex.release();

            mSemaphore.acquire();
            mSemaphore.release();
        }
    } while (waiting);
}
