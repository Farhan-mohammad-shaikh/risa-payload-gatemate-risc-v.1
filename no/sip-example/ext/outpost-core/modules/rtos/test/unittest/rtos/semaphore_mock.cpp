/*
 * Copyright (c) 2024, Janosch Reinking
 * Copyright (c) 2024, Cold, Erin Cynthia
 * Copyright (c) 2024, Reinking, Janosch
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "unittest/rtos/semaphore_mock.h"

using namespace outpost::posix;
using namespace unittest::rtos;
using namespace outpost::time;

// ---------------------------------------------------------------------------
SemaphoreImplementation::SemaphoreImplementation(outpost::rtos::Semaphore* sema, uint32_t count) :
    Mockable<SemaphoreMock, outpost::posix::Semaphore>(
            this, reinterpret_cast<intptr_t>(sema), &mSemaphore),
    mSemaphore(count)
{
}

bool
SemaphoreImplementation::acquire()
{
    std::function<bool()> mockFunction =
            std::bind(static_cast<bool (SemaphoreMock::*)()>(&SemaphoreMock::acquire), mMock);
    std::function<bool()> realFunction =
            std::bind(static_cast<bool (Semaphore::*)()>(&Semaphore::acquire), &mSemaphore);
    return handleMethodCall(mMock, mockFunction, realFunction);
}

bool
SemaphoreImplementation::acquire(outpost::time::Duration timeout)
{
    std::function<bool()> mockFunction =
            std::bind(static_cast<bool (SemaphoreMock::*)(Duration)>(&SemaphoreMock::acquire),
                      mMock,
                      timeout);
    std::function<bool()> realFunction = std::bind(
            static_cast<bool (Semaphore::*)(Duration)>(&Semaphore::acquire), &mSemaphore, timeout);
    return handleMethodCall(mMock, mockFunction, realFunction);
}

bool
SemaphoreImplementation::acquireFromISR(bool& hasWokenTask)
{
    std::function<bool()> mockFunction =
            std::bind(&SemaphoreMock::acquireFromISR, mMock, hasWokenTask);
    std::function<bool()> realFunction =
            std::bind(&Semaphore::acquireFromISR, &mSemaphore, hasWokenTask);
    return handleMethodCall(mMock, mockFunction, realFunction);
}

void
SemaphoreImplementation::release()
{
    std::function<void()> mockFunction = std::bind(&SemaphoreMock::release, mMock);
    std::function<void()> realFunction = std::bind(&Semaphore::release, &mSemaphore);
    return handleMethodCall(mMock, mockFunction, realFunction);
}

void
SemaphoreImplementation::releaseFromISR(bool& hasWokenTask)
{
    std::function<void()> mockFunction =
            std::bind(&SemaphoreMock::releaseFromISR, mMock, hasWokenTask);
    std::function<void()> realFunction =
            std::bind(&Semaphore::releaseFromISR, &mSemaphore, hasWokenTask);
    return handleMethodCall(mMock, mockFunction, realFunction);
}

// ---------------------------------------------------------------------------
BinarySemaphoreImplementation::BinarySemaphoreImplementation(outpost::rtos::BinarySemaphore* sema) :
    Mockable<BinarySemaphoreMock, outpost::posix::BinarySemaphore>(
            this, reinterpret_cast<intptr_t>(sema), &mSemaphore),
    mSemaphore()
{
}

BinarySemaphoreImplementation::BinarySemaphoreImplementation(
        outpost::rtos::BinarySemaphore* sema, outpost::rtos::BinarySemaphore::State::Type initial) :
    Mockable<BinarySemaphoreMock, outpost::posix::BinarySemaphore>(
            this, reinterpret_cast<intptr_t>(sema), &mSemaphore),
    mSemaphore(static_cast<outpost::posix::BinarySemaphore::State::Type>(initial))
{
}

bool
BinarySemaphoreImplementation::acquire()
{
    std::function<bool()> mockFunction = std::bind(
            static_cast<bool (BinarySemaphoreMock::*)()>(&BinarySemaphoreMock::acquire), mMock);
    std::function<bool()> realFunction = std::bind(
            static_cast<bool (BinarySemaphore::*)()>(&BinarySemaphore::acquire), &mSemaphore);
    return handleMethodCall(mMock, mockFunction, realFunction);
}

bool
BinarySemaphoreImplementation::acquire(outpost::time::Duration timeout)
{
    std::function<bool()> mockFunction = std::bind(
            static_cast<bool (BinarySemaphoreMock::*)(Duration)>(&BinarySemaphoreMock::acquire),
            mMock,
            timeout);
    std::function<bool()> realFunction =
            std::bind(static_cast<bool (BinarySemaphore::*)(Duration)>(&BinarySemaphore::acquire),
                      &mSemaphore,
                      timeout);
    return handleMethodCall(mMock, mockFunction, realFunction);
}

bool
BinarySemaphoreImplementation::acquireFromISR(bool& hasWokenTask)
{
    std::function<bool()> mockFunction =
            std::bind(&BinarySemaphoreMock::acquireFromISR, mMock, hasWokenTask);
    std::function<bool()> realFunction =
            std::bind(&BinarySemaphore::acquireFromISR, &mSemaphore, hasWokenTask);
    return handleMethodCall(mMock, mockFunction, realFunction);
}

void
BinarySemaphoreImplementation::release()
{
    std::function<void()> mockFunction = std::bind(&BinarySemaphoreMock::release, mMock);
    std::function<void()> realFunction = std::bind(&BinarySemaphore::release, &mSemaphore);
    return handleMethodCall(mMock, mockFunction, realFunction);
}

void
BinarySemaphoreImplementation::releaseFromISR(bool& hasWokenTask)
{
    std::function<void()> mockFunction =
            std::bind(&BinarySemaphoreMock::releaseFromISR, mMock, hasWokenTask);
    std::function<void()> realFunction =
            std::bind(&BinarySemaphore::releaseFromISR, &mSemaphore, hasWokenTask);
    return handleMethodCall(mMock, mockFunction, realFunction);
}
