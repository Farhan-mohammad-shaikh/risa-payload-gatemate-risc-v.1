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

#ifndef UNITTEST_RTOS_BARRIER_MOCK_H
#define UNITTEST_RTOS_BARRIER_MOCK_H

#include <outpost/posix/barrier.h>
#include <outpost/rtos/barrier.h>
#include <outpost/time.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "unittest/rtos/mock.h"

namespace unittest
{
namespace rtos
{

class BarrierImplementation;

// ---------------------------------------------------------------------------
/**
 * \class BarrierInterface
 */
class BarrierInterface
{
public:
    explicit BarrierInterface() = default;

    BarrierInterface(const BarrierInterface&) = delete;

    BarrierInterface&
    operator=(const BarrierInterface&) = delete;

    virtual ~BarrierInterface() = default;

    virtual void
    wait() = 0;

private:
};

// ---------------------------------------------------------------------------
/**
 * \class BarrierMock
 */
class BarrierMock : public BarrierInterface, public Mock<BarrierImplementation>
{
public:
    explicit BarrierMock(MockMode mode = defaultMode) : Mock<BarrierImplementation>(mode)
    {
    }

    MOCK_METHOD(void, wait, (), (override));
};

// ---------------------------------------------------------------------------
/**
 * \class BarrierImplementation
 */
class BarrierImplementation : public Mockable<BarrierMock, outpost::posix::Barrier>
{
public:
    explicit BarrierImplementation(outpost::rtos::Barrier* barrier, uint32_t numberOfThreads);

    BarrierImplementation(const BarrierImplementation&) = delete;

    BarrierImplementation&
    operator=(const BarrierImplementation&) = delete;

    ~BarrierImplementation();

    void
    wait();

private:
    outpost::posix::Barrier mBarrier;
};

}  // namespace rtos
}  // namespace unittest

#endif  // UNITTEST_RTOS_BARRIER_MOCK_H
