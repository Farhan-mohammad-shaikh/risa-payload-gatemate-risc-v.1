/*
 * Copyright (c) 2014-2017, Fabian Greif
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "../reference/consumer.h"

#include <outpost/rtos/mutex_guard.h>

Consumer::Consumer(outpost::rtos::Queue<uint32_t>& queue) :
    Thread(40, defaultStackSize, "Consumer"),
    mQueue(queue),
    mCurrentValue(0),
    mEvent(outpost::rtos::BinarySemaphore::State::acquired)
{
}

uint32_t
Consumer::getCurrentValue() const
{
    outpost::rtos::MutexGuard lock(mMutex);
    return mCurrentValue;
}

void
Consumer::waitForNewValue()
{
    mEvent.acquire();
}

void
Consumer::run()
{
    while (1)
    {
        uint32_t value = 0;
        if (mQueue.receive(value, outpost::time::Milliseconds(500)))
        {
            outpost::rtos::MutexGuard lock(mMutex);
            mCurrentValue = value;

            mEvent.release();
        }
    }
}
