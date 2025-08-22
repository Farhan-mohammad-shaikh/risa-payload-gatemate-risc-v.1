/*
 * Copyright (c) 2024, Janosch Reinking
 * Copyright (c) 2024, Reinking, Janosch
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "unittest/rtos/mutex_mock.h"

using namespace outpost::posix;
using namespace unittest::rtos;
using namespace outpost::time;

// ---------------------------------------------------------------------------
MutexImplementation::MutexImplementation(outpost::rtos::Mutex* mutex) :
    Mockable<MutexMock, outpost::posix::Mutex>(this, reinterpret_cast<intptr_t>(mutex), &mMutex)
{
}

MutexImplementation::~MutexImplementation()
{
}

// ---------------------------------------------------------------------------
bool
MutexImplementation::acquire()
{
    std::function<bool()> mockFunction =
            std::bind(static_cast<bool (MutexMock::*)()>(&MutexMock::acquire), mMock);
    std::function<bool()> realFunction =
            std::bind(static_cast<bool (Mutex::*)()>(&Mutex::acquire), &mMutex);
    return handleMethodCall(mMock, mockFunction, realFunction);
}

bool
MutexImplementation::acquire(outpost::time::Duration timeout)
{
    std::function<bool()> mockFunction = std::bind(
            static_cast<bool (MutexMock::*)(Duration)>(&MutexMock::acquire), mMock, timeout);
    std::function<bool()> realFunction =
            std::bind(static_cast<bool (Mutex::*)(Duration)>(&Mutex::acquire), &mMutex, timeout);
    return handleMethodCall(mMock, mockFunction, realFunction);
}

bool
MutexImplementation::acquireFromISR(bool& hasWokenThread)
{
    std::function<bool()> mockFunction =
            std::bind(&MutexMock::acquireFromISR, mMock, hasWokenThread);
    std::function<bool()> realFunction = std::bind(&Mutex::acquireFromISR, &mMutex, hasWokenThread);
    return handleMethodCall(mMock, mockFunction, realFunction);
}

void
MutexImplementation::release()
{
    std::function<void()> mockFunction = std::bind(&MutexMock::release, mMock);
    std::function<void()> realFunction = std::bind(&Mutex::release, &mMutex);
    return handleMethodCall(mMock, mockFunction, realFunction);
}

void
MutexImplementation::releaseFromISR(bool& hasWokenThread)
{
    std::function<void()> mockFunction =
            std::bind(&MutexMock::releaseFromISR, mMock, hasWokenThread);
    std::function<void()> realFunction = std::bind(&Mutex::releaseFromISR, &mMutex, hasWokenThread);
    return handleMethodCall(mMock, mockFunction, realFunction);
}
