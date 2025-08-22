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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <unittest/rtos/clock_mock.h>

using namespace outpost::rtos;
using namespace outpost::time;
using namespace ::testing;

// ---------------------------------------------------------------------------
static constexpr Milliseconds anyDuration(1000U);

// ---------------------------------------------------------------------------
namespace unittest
{
namespace rtos
{

struct ClockUser
{
    outpost::rtos::SystemClock mClock;
};

}  // namespace rtos
}  // namespace unittest

// ---------------------------------------------------------------------------
/**
 * Tests for \c ClockMock
 *
 * function:
 * - 'now'
 */

TEST(ClockMockTest, now_shouldMock)
{
    unittest::rtos::ClockUser user;
    unittest::rtos::ClockMock mock;
    unittest::rtos::injectMock(user, mock);

    {
        EXPECT_CALL(mock, now())
                .WillOnce(Return(SpacecraftElapsedTime::startOfEpoch() + anyDuration));
    }

    const auto now = user.mClock.now();

    ASSERT_THAT(now.timeSinceEpoch(), Eq(anyDuration));
}

TEST(ClockMockTest, underlyingObjectShouldExist)
{
    unittest::rtos::ClockUser user;
    unittest::rtos::ClockMock mock;
    unittest::rtos::injectMock(user, mock);

    const outpost::posix::SystemClock* clock = mock.getUnderlyingObject();

    ASSERT_NE(nullptr, clock);

    auto nowEarly = clock->now();
    auto nowLate = clock->now() + outpost::time::Duration(Seconds(1));

    ASSERT_THAT(nowLate, Ge(nowEarly));
}
