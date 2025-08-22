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

#ifndef UNITTEST_RTOS_CLOCK_MOCK_H
#define UNITTEST_RTOS_CLOCK_MOCK_H

#include <outpost/posix/clock.h>
#include <outpost/rtos/clock.h>
#include <outpost/time.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "unittest/rtos/mock.h"

namespace unittest
{
namespace rtos
{

class ClockImplementation;

// ---------------------------------------------------------------------------
/**
 * \class ClockInterface
 */
class ClockInterface
{
public:
    explicit ClockInterface() = default;

    ClockInterface(const ClockInterface&) = delete;

    ClockInterface&
    operator=(const ClockInterface&) = delete;

    virtual ~ClockInterface() = default;

    virtual outpost::time::SpacecraftElapsedTime
    now() const = 0;

private:
};

// ---------------------------------------------------------------------------
/**
 * \class ClockMock
 */
class ClockMock : public ClockInterface, public Mock<ClockImplementation>
{
public:
    explicit ClockMock(MockMode mode = defaultMode) : Mock<ClockImplementation>(mode)
    {
    }

    MOCK_METHOD(outpost::time::SpacecraftElapsedTime, now, (), (const, override));
};

// ---------------------------------------------------------------------------
/**
 * \class ClockImplementation
 */
class ClockImplementation : public Mockable<ClockMock, outpost::posix::SystemClock>
{
public:
    explicit ClockImplementation(outpost::rtos::SystemClock* clock);

    ClockImplementation(const ClockImplementation&) = delete;

    ClockImplementation&
    operator=(const ClockImplementation&) = delete;

    ~ClockImplementation();

    outpost::time::SpacecraftElapsedTime
    now() const;

private:
    outpost::posix::SystemClock mSystemClock;
};

}  // namespace rtos
}  // namespace unittest

#endif  // UNITTEST_RTOS_CLOCK_MOCK_H
