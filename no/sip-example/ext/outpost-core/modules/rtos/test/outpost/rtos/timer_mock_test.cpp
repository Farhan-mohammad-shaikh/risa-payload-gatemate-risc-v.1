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

#include <outpost/rtos/timer.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <unittest/rtos/timer_mock.h>

using namespace outpost::rtos;
using namespace ::testing;

// ---------------------------------------------------------------------------
namespace unittest
{
namespace rtos
{

struct TimerUser : public outpost::rtos::Timer
{
    TimerUser() : outpost::rtos::Timer(this, &TimerUser::callback, "test"), mCounter(0)
    {
    }

    void
    callback(outpost::rtos::Timer* timer)
    {
        auto* base = dynamic_cast<outpost::rtos::Timer*>(this);
        EXPECT_THAT(timer, Eq(base));
        mCounter++;
    }

    int mCounter;
};

}  // namespace rtos
}  // namespace unittest

// ---------------------------------------------------------------------------
/**
 * Tests for \c TimerMock
 *
 * - retrieve constructor arguments
 * - execute timer manually
 * - 'start'
 * - 'reset'
 * - 'cancel'
 * - 'isRunning'
 * - 'startTimerDaemonThread'
 */

// ---------------------------------------------------------------------------
// retrieve constructor arguments
TEST(TimerMockTest, shouldRetrieveName)
{
    unittest::rtos::TimerMock mock;
    unittest::rtos::TimerUser user;
    unittest::rtos::injectMock(user, mock);

    auto* timer = mock.getMockable();
    const char* name = timer->getName();

    ASSERT_THAT(name, StrEq("test"));
}

// ---------------------------------------------------------------------------
// execute timer manually
TEST(TimerMockTest, shouldExecuteTimer)
{
    unittest::rtos::TimerMock mock;
    unittest::rtos::TimerUser user;
    unittest::rtos::injectMock(user, mock);

    auto* timer = mock.getMockable();
    timer->executeTimer();

    ASSERT_THAT(user.mCounter, Eq(1));
}

// ---------------------------------------------------------------------------
// start
TEST(TimerMockTest, start_shouldMock)
{
    static constexpr outpost::time::Milliseconds anyDuration(100U);

    unittest::rtos::TimerMock mock;
    unittest::rtos::TimerUser user;
    unittest::rtos::injectMock(user, mock);

    {
        EXPECT_CALL(mock, start(anyDuration)).Times(1U);
    }

    auto* timer = dynamic_cast<outpost::rtos::Timer*>(&user);
    timer->start(anyDuration);
}

// reset
TEST(TimerMockTest, reset_shouldMock)
{
    unittest::rtos::TimerMock mock;
    unittest::rtos::TimerUser user;
    unittest::rtos::injectMock(user, mock);

    {
        EXPECT_CALL(mock, reset()).Times(1U);
    }

    auto* timer = dynamic_cast<outpost::rtos::Timer*>(&user);
    timer->reset();
}

// cancel
TEST(TimerMockTest, cancel_shouldMock)
{
    unittest::rtos::TimerMock mock;
    unittest::rtos::TimerUser user;
    unittest::rtos::injectMock(user, mock);

    {
        EXPECT_CALL(mock, cancel()).Times(1U);
    }

    auto* timer = dynamic_cast<outpost::rtos::Timer*>(&user);
    timer->cancel();
}

// isRunning
TEST(TimerMockTest, isRunning_shouldMock)
{
    unittest::rtos::TimerMock mock;
    unittest::rtos::TimerUser user;
    unittest::rtos::injectMock(user, mock);

    {
        InSequence seq;
        EXPECT_CALL(mock, isRunning()).WillOnce(Return(true));
        EXPECT_CALL(mock, isRunning()).WillOnce(Return(false));
    }

    auto* timer = dynamic_cast<outpost::rtos::Timer*>(&user);
    bool val1 = timer->isRunning();
    bool val2 = timer->isRunning();

    ASSERT_THAT(val1, IsTrue());
    ASSERT_THAT(val2, IsFalse());
}

// startTimerDaemonThread
TEST(TimerMockTest, startTimerDaemonThread_shouldMock)
{
    unittest::rtos::TimerMock mock;
    unittest::rtos::TimerUser user;
    unittest::rtos::injectMock(user, mock);

    {
        InSequence seq;
        EXPECT_CALL(mock,
                    startTimerDaemonThread(Eq(0U), Eq(0U), Eq(outpost::rtos::inheritFromCaller)))
                .Times(1);
    }

    auto* timer = dynamic_cast<outpost::rtos::Timer*>(&user);
    timer->startTimerDaemonThread(0, 0, outpost::rtos::inheritFromCaller);
}

// startTimerDaemonThread
TEST(TimerMockTest, startTimerDaemonThread_shouldUseFirstTimerToMockStaticFunction)
{
    unittest::rtos::TimerMock mock;
    unittest::rtos::TimerUser user1;
    unittest::rtos::TimerUser user2;
    unittest::rtos::injectMock(user1, mock);

    {
        InSequence seq;
        EXPECT_CALL(mock,
                    startTimerDaemonThread(Eq(0U), Eq(0U), Eq(outpost::rtos::inheritFromCaller)))
                .Times(1);
    }

    outpost::rtos::Timer::startTimerDaemonThread(0, 0, outpost::rtos::inheritFromCaller);
}

TEST(TimerMockTest, underlyingObjectShouldExist)
{
    unittest::rtos::TimerMock mock;
    unittest::rtos::TimerUser user;
    unittest::rtos::injectMock(user, mock);

    outpost::posix::TimerRaw* timer = mock.getUnderlyingObject();

    ASSERT_NE(nullptr, timer);

    timer->start(outpost::time::Duration::myriad());
    ASSERT_TRUE(timer->isRunning());
    timer->cancel();
    ASSERT_FALSE(timer->isRunning());
}
