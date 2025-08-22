/*
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "outpost/rtos/mutex.h"

#include "unittest/rtos/mutex_mock.h"

using namespace outpost::rtos;

// ---------------------------------------------------------------------------
Mutex::Mutex() : mImplementation(nullptr)
{
    auto* mutex = new unittest::rtos::MutexImplementation(this);
    mImplementation = mutex;
}

Mutex::~Mutex()
{
    auto* mutex = reinterpret_cast<unittest::rtos::MutexImplementation*>(mImplementation);
    delete mutex;
    mImplementation = nullptr;
}

// ---------------------------------------------------------------------------
bool
Mutex::acquire()
{
    auto* mutex = reinterpret_cast<unittest::rtos::MutexImplementation*>(mImplementation);
    return mutex->acquire();
}

bool
Mutex::acquire(outpost::time::Duration timeout)
{
    auto* mutex = reinterpret_cast<unittest::rtos::MutexImplementation*>(mImplementation);
    return mutex->acquire(timeout);
}

bool
Mutex::acquireFromISR(bool& hasWokenThread)
{
    auto* mutex = reinterpret_cast<unittest::rtos::MutexImplementation*>(mImplementation);
    return mutex->acquireFromISR(hasWokenThread);
}

void
Mutex::release()
{
    auto* mutex = reinterpret_cast<unittest::rtos::MutexImplementation*>(mImplementation);
    return mutex->release();
}

void
Mutex::releaseFromISR(bool& hasWokenThread)
{
    auto* mutex = reinterpret_cast<unittest::rtos::MutexImplementation*>(mImplementation);
    return mutex->releaseFromISR(hasWokenThread);
}
