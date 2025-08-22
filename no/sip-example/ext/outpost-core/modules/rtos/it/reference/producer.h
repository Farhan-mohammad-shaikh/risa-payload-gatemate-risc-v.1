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

#ifndef PRODUCER_H
#define PRODUCER_H

#include <outpost/rtos/queue.h>
#include <outpost/rtos/thread.h>

class Producer : public outpost::rtos::Thread
{
public:
    Producer(outpost::rtos::Queue<uint32_t>& queue);

private:
    virtual void
    run();

    outpost::rtos::Queue<uint32_t>& mQueue;
};

#endif  // PRODUCER_H
