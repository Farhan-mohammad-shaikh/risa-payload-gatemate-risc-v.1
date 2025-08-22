/*
 * Copyright (c) 2024, Janosch Reinking
 * Copyright (c) 2024, Cold, Erin Cynthia
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "outpost/rtos/semaphore.h"

#include "unittest/rtos/semaphore_mock.h"

using outpost::rtos::BinarySemaphore;
using outpost::rtos::Semaphore;

// ----------------------------------------------------------------------------
Semaphore::Semaphore(uint32_t count) : mImplementation(nullptr)
{
    auto* sema = new unittest::rtos::SemaphoreImplementation(this, count);
    mImplementation = sema;
}

Semaphore::~Semaphore()
{
    auto* sema = reinterpret_cast<unittest::rtos::SemaphoreImplementation*>(mImplementation);
    delete sema;
    mImplementation = nullptr;
}

// ---------------------------------------------------------------------------
bool
Semaphore::acquire()
{
    auto* sema = reinterpret_cast<unittest::rtos::SemaphoreImplementation*>(mImplementation);
    return sema->acquire();
}

bool
Semaphore::acquire(time::Duration timeout)
{
    auto* sema = reinterpret_cast<unittest::rtos::SemaphoreImplementation*>(mImplementation);
    return sema->acquire(timeout);
}

bool
Semaphore::acquireFromISR(bool& hasWokenTask)
{
    auto* sema = reinterpret_cast<unittest::rtos::SemaphoreImplementation*>(mImplementation);
    return sema->acquireFromISR(hasWokenTask);
}

void
Semaphore::release()
{
    auto* sema = reinterpret_cast<unittest::rtos::SemaphoreImplementation*>(mImplementation);
    return sema->release();
}

void
Semaphore::releaseFromISR(bool& hasWokenTask)
{
    auto* sema = reinterpret_cast<unittest::rtos::SemaphoreImplementation*>(mImplementation);
    return sema->releaseFromISR(hasWokenTask);
}

// ----------------------------------------------------------------------------
BinarySemaphore::BinarySemaphore() : mImplementation(nullptr)
{
    auto* sema = new unittest::rtos::BinarySemaphoreImplementation(this);
    mImplementation = sema;
}

BinarySemaphore::BinarySemaphore(State::Type initial)
{
    auto* sema = new unittest::rtos::BinarySemaphoreImplementation(this, initial);
    mImplementation = sema;
}

BinarySemaphore::~BinarySemaphore()
{
    auto* sema = reinterpret_cast<unittest::rtos::BinarySemaphoreImplementation*>(mImplementation);
    delete sema;
    mImplementation = nullptr;
}

// ---------------------------------------------------------------------------
bool
BinarySemaphore::acquire()
{
    auto* sema = reinterpret_cast<unittest::rtos::BinarySemaphoreImplementation*>(mImplementation);
    return sema->acquire();
}

bool
BinarySemaphore::acquire(time::Duration timeout)
{
    auto* sema = reinterpret_cast<unittest::rtos::BinarySemaphoreImplementation*>(mImplementation);
    return sema->acquire(timeout);
}

bool
BinarySemaphore::acquireFromISR(bool& hasWokenTask)
{
    auto* sema = reinterpret_cast<unittest::rtos::BinarySemaphoreImplementation*>(mImplementation);
    return sema->acquireFromISR(hasWokenTask);
}

void
BinarySemaphore::release()
{
    auto* sema = reinterpret_cast<unittest::rtos::BinarySemaphoreImplementation*>(mImplementation);
    return sema->release();
}

void
BinarySemaphore::releaseFromISR(bool& hasWokenTask)
{
    auto* sema = reinterpret_cast<unittest::rtos::BinarySemaphoreImplementation*>(mImplementation);
    return sema->releaseFromISR(hasWokenTask);
}
