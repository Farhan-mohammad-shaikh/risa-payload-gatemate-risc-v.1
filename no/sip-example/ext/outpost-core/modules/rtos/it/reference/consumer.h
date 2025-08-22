/*
 * Copyright (c) 2014-2017, Fabian Greif
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef CONSUMER_H
#define CONSUMER_H

#include <outpost/rtos/mutex.h>
#include <outpost/rtos/queue.h>
#include <outpost/rtos/semaphore.h>
#include <outpost/rtos/thread.h>

class Consumer : public outpost::rtos::Thread
{
public:
    Consumer(outpost::rtos::Queue<uint32_t>& queue);

    uint32_t
    getCurrentValue() const;

    void
    waitForNewValue();

private:
    virtual void
    run();

    outpost::rtos::Queue<uint32_t>& mQueue;

    mutable outpost::rtos::Mutex mMutex;
    uint32_t mCurrentValue;

    outpost::rtos::BinarySemaphore mEvent;
};

#endif  // CONSUMER_H
