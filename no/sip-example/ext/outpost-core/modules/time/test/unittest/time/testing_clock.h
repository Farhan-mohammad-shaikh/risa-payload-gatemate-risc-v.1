/*
 * Copyright (c) 2014-2018, Fabian Greif
 * Copyright (c) 2021, Jan Malburg
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef UNITTEST_TIME_TESTING_CLOCK_H
#define UNITTEST_TIME_TESTING_CLOCK_H

#include <outpost/time/clock.h>

namespace unittest
{
namespace time
{
/**
 * Clock interface.
 *
 * \author  Fabian Greif
 */
class TestingClock : public outpost::time::Clock
{
public:
    TestingClock();

    virtual ~TestingClock() = default;

    outpost::time::SpacecraftElapsedTime
    now() const override;

    /**
     * Set the current time to an absolute value.
     */
    void
    setTime(outpost::time::SpacecraftElapsedTime timePoint);

    /**
     * Increment the current time by the given duration.
     *
     * The duration can be negative.
     */
    void
    incrementBy(outpost::time::Duration time);

private:
    outpost::time::SpacecraftElapsedTime currentTime;
};

}  // namespace time
}  // namespace unittest

#endif  // UNITTEST_TIME_TESTING_CLOCK_H
