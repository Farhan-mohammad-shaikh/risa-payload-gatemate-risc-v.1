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

#ifndef UNITTEST_RTOS_MUTEX_MOCK_H
#define UNITTEST_RTOS_MUTEX_MOCK_H

#include <outpost/posix/mutex.h>
#include <outpost/rtos/mutex.h>
#include <outpost/time.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "unittest/rtos/mock.h"

namespace unittest
{
namespace rtos
{

class MutexImplementation;

// ---------------------------------------------------------------------------
/**
 * \class MutexInterface
 */
class MutexInterface
{
public:
    explicit MutexInterface() = default;

    MutexInterface(const MutexInterface&) = delete;

    MutexInterface&
    operator=(const MutexInterface&) = delete;

    virtual ~MutexInterface() = default;

    virtual bool
    acquire() = 0;

    virtual bool
    acquire(outpost::time::Duration timeout) = 0;

    virtual bool
    acquireFromISR(bool& hasWokenThread) = 0;

    virtual void
    release() = 0;

    virtual void
    releaseFromISR(bool& hasWokenThread) = 0;

private:
};

// ---------------------------------------------------------------------------
/**
 * \class MutexMock
 */
class MutexMock : public MutexInterface, public Mock<MutexImplementation>
{
public:
    explicit MutexMock(MockMode mode = defaultMode) : Mock<MutexImplementation>(mode)
    {
    }

    MOCK_METHOD(bool, acquire, (), (override));
    MOCK_METHOD(bool, acquire, (outpost::time::Duration timeout), (override));
    MOCK_METHOD(bool, acquireFromISR, (bool& hasWokenThread), (override));
    MOCK_METHOD(void, release, (), (override));
    MOCK_METHOD(void, releaseFromISR, (bool& hasWokenThread), (override));
};

// ---------------------------------------------------------------------------
/**
 * \class MutexImplementation
 */
class MutexImplementation : public Mockable<MutexMock, outpost::posix::Mutex>
{
public:
    explicit MutexImplementation(outpost::rtos::Mutex* mutex);

    MutexImplementation(const MutexImplementation&) = delete;

    MutexImplementation&
    operator=(const MutexImplementation&) = delete;

    ~MutexImplementation();

    bool
    acquire();

    bool
    acquire(outpost::time::Duration timeout);

    bool
    acquireFromISR(bool& hasWokenThread);

    void
    release();

    void
    releaseFromISR(bool& hasWokenThread);

private:
    outpost::posix::Mutex mMutex;
};

}  // namespace rtos
}  // namespace unittest

#endif  // UNITTEST_RTOS_MUTEX_MOCK_H
