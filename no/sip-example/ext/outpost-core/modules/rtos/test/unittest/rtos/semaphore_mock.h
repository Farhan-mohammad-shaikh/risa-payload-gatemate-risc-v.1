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

#ifndef UNITTEST_RTOS_SEMAPHORE_MOCK_H
#define UNITTEST_RTOS_SEMAPHORE_MOCK_H

#include <outpost/posix/semaphore.h>
#include <outpost/rtos/semaphore.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "unittest/rtos/mock.h"

namespace unittest
{
namespace rtos
{

class SemaphoreImplementation;

// ---------------------------------------------------------------------------
/**
 * \class SemaphoreInterface
 */
class SemaphoreInterface
{
public:
    explicit SemaphoreInterface() = default;

    SemaphoreInterface(const SemaphoreInterface&) = delete;

    SemaphoreInterface&
    operator=(const SemaphoreInterface&) = delete;

    virtual ~SemaphoreInterface() = default;

    virtual bool
    acquire() = 0;

    virtual bool
    acquire(outpost::time::Duration timeout) = 0;

    virtual bool
    acquireFromISR(bool& hasWokenTask) = 0;

    virtual void
    release() = 0;

    virtual void
    releaseFromISR(bool& hasWokenTask) = 0;

private:
};

// ---------------------------------------------------------------------------
/**
 * \class SemaphoreMock
 */
class SemaphoreMock : public SemaphoreInterface, public Mock<SemaphoreImplementation>
{
public:
    explicit SemaphoreMock(MockMode mode = defaultMode) : Mock<SemaphoreImplementation>(mode)
    {
    }

    MOCK_METHOD(bool, acquire, (), (override));
    MOCK_METHOD(bool, acquire, (outpost::time::Duration), (override));
    MOCK_METHOD(bool, acquireFromISR, (bool&), (override));
    MOCK_METHOD(void, release, (), (override));
    MOCK_METHOD(void, releaseFromISR, (bool&), (override));

private:
};

// ---------------------------------------------------------------------------
/**
 * \class SemaphoreImplementation
 */
class SemaphoreImplementation : public Mockable<SemaphoreMock, outpost::posix::Semaphore>
{
public:
    explicit SemaphoreImplementation(outpost::rtos::Semaphore* sema, uint32_t count);

    SemaphoreImplementation(const SemaphoreImplementation&) = delete;

    SemaphoreImplementation&
    operator=(const SemaphoreImplementation&) = delete;

    ~SemaphoreImplementation() = default;

    bool
    acquire();

    bool
    acquire(outpost::time::Duration timeout);

    bool
    acquireFromISR(bool& hasWokenTask);

    void
    release();

    void
    releaseFromISR(bool& hasWokenTask);

private:
    outpost::posix::Semaphore mSemaphore;
};

// ---------------------------------------------------------------------------

class BinarySemaphoreImplementation;

// ---------------------------------------------------------------------------
/**
 * \class BinarySemaphoreInterface
 */
class BinarySemaphoreInterface
{
public:
    explicit BinarySemaphoreInterface() = default;

    BinarySemaphoreInterface(const BinarySemaphoreInterface&) = delete;

    BinarySemaphoreInterface&
    operator=(const BinarySemaphoreInterface&) = delete;

    virtual ~BinarySemaphoreInterface() = default;

    virtual bool
    acquire() = 0;

    virtual bool
    acquire(outpost::time::Duration timeout) = 0;

    virtual bool
    acquireFromISR(bool& hasWokenTask) = 0;

    virtual void
    release() = 0;

    virtual void
    releaseFromISR(bool& hasWokenTask) = 0;

private:
};

// ---------------------------------------------------------------------------
/**
 * \class BinarySemaphoreMock
 */
class BinarySemaphoreMock : public BinarySemaphoreInterface,
                            public Mock<BinarySemaphoreImplementation>
{
public:
    explicit BinarySemaphoreMock(MockMode mode = defaultMode) :
        Mock<BinarySemaphoreImplementation>(mode)
    {
    }

    MOCK_METHOD(bool, acquire, (), (override));
    MOCK_METHOD(bool, acquire, (outpost::time::Duration), (override));
    MOCK_METHOD(bool, acquireFromISR, (bool&), (override));
    MOCK_METHOD(void, release, (), (override));
    MOCK_METHOD(void, releaseFromISR, (bool&), (override));

private:
};

// ---------------------------------------------------------------------------
/**
 * \class BinarySemaphoreImplementation
 */
class BinarySemaphoreImplementation
    : public Mockable<BinarySemaphoreMock, outpost::posix::BinarySemaphore>
{
public:
    explicit BinarySemaphoreImplementation(outpost::rtos::BinarySemaphore* sema);
    explicit BinarySemaphoreImplementation(outpost::rtos::BinarySemaphore* sema,
                                           outpost::rtos::BinarySemaphore::State::Type initial);

    BinarySemaphoreImplementation(const BinarySemaphoreImplementation&) = delete;

    BinarySemaphoreImplementation&
    operator=(const BinarySemaphoreImplementation&) = delete;

    ~BinarySemaphoreImplementation() = default;

    bool
    acquire();

    bool
    acquire(outpost::time::Duration timeout);

    bool
    acquireFromISR(bool& hasWokenTask);

    void
    release();

    void
    releaseFromISR(bool& hasWokenTask);

private:
    outpost::posix::BinarySemaphore mSemaphore;
};

}  // namespace rtos
}  // namespace unittest

#endif  // UNITTEST_RTOS_SEMAPHORE_MOCK_H
