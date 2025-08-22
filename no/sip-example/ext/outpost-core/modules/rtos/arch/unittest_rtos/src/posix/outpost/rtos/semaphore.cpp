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

#include "outpost/posix/semaphore.h"

using outpost::rtos::BinarySemaphore;
using outpost::rtos::Semaphore;

// ----------------------------------------------------------------------------
Semaphore::Semaphore(uint32_t count) : mImplementation(nullptr)
{
    auto* semaphore = new outpost::posix::Semaphore(count);
    mImplementation = semaphore;
}

Semaphore::~Semaphore()
{
    auto* semaphore = reinterpret_cast<outpost::posix::Semaphore*>(mImplementation);
    delete semaphore;
    mImplementation = nullptr;
}

// ---------------------------------------------------------------------------
bool
Semaphore::acquire()
{
    auto* semaphore = reinterpret_cast<outpost::posix::Semaphore*>(mImplementation);
    return semaphore->acquire();
}

bool
Semaphore::acquire(time::Duration timeout)
{
    auto* semaphore = reinterpret_cast<outpost::posix::Semaphore*>(mImplementation);
    return semaphore->acquire(timeout);
}

bool
Semaphore::acquireFromISR(bool& hasWokenTask)
{
    auto* semaphore = reinterpret_cast<outpost::posix::Semaphore*>(mImplementation);
    return semaphore->acquireFromISR(hasWokenTask);
}

void
Semaphore::release()
{
    auto* semaphore = reinterpret_cast<outpost::posix::Semaphore*>(mImplementation);
    return semaphore->release();
}

void
Semaphore::releaseFromISR(bool& hasWokenTask)
{
    auto* semaphore = reinterpret_cast<outpost::posix::Semaphore*>(mImplementation);
    return semaphore->releaseFromISR(hasWokenTask);
}

// ----------------------------------------------------------------------------
BinarySemaphore::BinarySemaphore() : mImplementation(nullptr)
{
    auto* semaphore = new outpost::posix::BinarySemaphore();
    mImplementation = semaphore;
}

BinarySemaphore::BinarySemaphore(State::Type initial)
{
    auto* semaphore = new outpost::posix::BinarySemaphore(
            static_cast<outpost::posix::BinarySemaphore::State::Type>(initial));
    mImplementation = semaphore;
}

BinarySemaphore::~BinarySemaphore()
{
    auto* semaphore = reinterpret_cast<outpost::posix::BinarySemaphore*>(mImplementation);
    delete semaphore;
    mImplementation = nullptr;
}

// ---------------------------------------------------------------------------
bool
BinarySemaphore::acquire()
{
    auto* semaphore = reinterpret_cast<outpost::posix::BinarySemaphore*>(mImplementation);
    return semaphore->acquire();
}

bool
BinarySemaphore::acquire(time::Duration timeout)
{
    auto* semaphore = reinterpret_cast<outpost::posix::BinarySemaphore*>(mImplementation);
    return semaphore->acquire(timeout);
}

bool
BinarySemaphore::acquireFromISR(bool& hasWokenTask)
{
    auto* semaphore = reinterpret_cast<outpost::posix::BinarySemaphore*>(mImplementation);
    return semaphore->acquireFromISR(hasWokenTask);
}

void
BinarySemaphore::release()
{
    auto* semaphore = reinterpret_cast<outpost::posix::BinarySemaphore*>(mImplementation);
    return semaphore->release();
}

void
BinarySemaphore::releaseFromISR(bool& hasWokenTask)
{
    auto* semaphore = reinterpret_cast<outpost::posix::BinarySemaphore*>(mImplementation);
    return semaphore->releaseFromISR(hasWokenTask);
}
