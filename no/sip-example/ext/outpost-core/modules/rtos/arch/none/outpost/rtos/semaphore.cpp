/*
 * Copyright (c) 2013-2017, Fabian Greif
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

// ----------------------------------------------------------------------------
outpost::rtos::Semaphore::Semaphore(uint32_t count)
{
    (void) count;
}

outpost::rtos::Semaphore::~Semaphore()
{
}

// ----------------------------------------------------------------------------
bool
outpost::rtos::Semaphore::acquire()
{
    return true;
}

bool
outpost::rtos::Semaphore::acquire(time::Duration timeout)
{
    (void) timeout;

    return true;
}

void
outpost::rtos::Semaphore::release()
{
}

// ----------------------------------------------------------------------------
outpost::rtos::BinarySemaphore::BinarySemaphore()
{
}

outpost::rtos::BinarySemaphore::BinarySemaphore(State::Type initial)
{
    (void) initial;
}

outpost::rtos::BinarySemaphore::~BinarySemaphore()
{
}

// ----------------------------------------------------------------------------
bool
outpost::rtos::BinarySemaphore::acquire()
{
    return true;
}

bool
outpost::rtos::BinarySemaphore::acquire(time::Duration timeout)
{
    (void) timeout;

    return true;
}

void
outpost::rtos::BinarySemaphore::release()
{
}
