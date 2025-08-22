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

#include <unittest/rtos/mutex_mock.h>

using namespace outpost::rtos;
using namespace ::testing;

// ---------------------------------------------------------------------------
static constexpr outpost::time::Milliseconds anyDuration(1000U);

// ---------------------------------------------------------------------------
namespace unittest
{
namespace rtos
{

struct MutexUser
{
    outpost::rtos::Mutex mMutex;
};

}  // namespace rtos
}  // namespace unittest

// ---------------------------------------------------------------------------
/**
 * Tests for \c MutexMock
 *
 * function:
 * - 'acquire'
 * - 'acquire'
 * - 'acquireFromISR'
 * - 'release'
 * - 'releaseFromISR'
 */

// ---------------------------------------------------------------------------
// acquire
TEST(MutexMockTest, acquire_shouldMock)
{
    unittest::rtos::MutexUser user;
    unittest::rtos::MutexMock mock;
    unittest::rtos::injectMock(user, mock);

    {
        InSequence seq;
        EXPECT_CALL(mock, acquire()).WillOnce(Return(true));
        EXPECT_CALL(mock, acquire()).WillOnce(Return(false));
    }

    const bool val1 = user.mMutex.acquire();
    const bool val2 = user.mMutex.acquire();

    ASSERT_THAT(val1, IsTrue());
    ASSERT_THAT(val2, IsFalse());
}

// acquire duration
TEST(MutexMockTest, acquire_duration_shouldMock)
{
    unittest::rtos::MutexUser user;
    unittest::rtos::MutexMock mock;
    unittest::rtos::injectMock(user, mock);

    {
        InSequence seq;
        EXPECT_CALL(mock, acquire(anyDuration)).WillOnce(Return(true));
        EXPECT_CALL(mock, acquire(anyDuration)).WillOnce(Return(false));
    }

    const bool val1 = user.mMutex.acquire(anyDuration);
    const bool val2 = user.mMutex.acquire(anyDuration);

    ASSERT_THAT(val1, IsTrue());
    ASSERT_THAT(val2, IsFalse());
}

// acquireFromISR
TEST(MutexMockTest, acquireFromISR_shouldMock)
{
    unittest::rtos::MutexUser user;
    unittest::rtos::MutexMock mock;
    unittest::rtos::injectMock(user, mock);

    bool hasWokenThread = false;

    {
        InSequence seq;
        EXPECT_CALL(mock, acquireFromISR(Eq(std::cref(hasWokenThread)))).WillOnce(Return(true));
        EXPECT_CALL(mock, acquireFromISR(Eq(std::cref(hasWokenThread)))).WillOnce(Return(false));
    }

    const bool val1 = user.mMutex.acquireFromISR(hasWokenThread);
    const bool val2 = user.mMutex.acquireFromISR(hasWokenThread);

    ASSERT_THAT(val1, IsTrue());
    ASSERT_THAT(val2, IsFalse());
}

// release
TEST(MutexMockTest, release_shouldMock)
{
    unittest::rtos::MutexUser user;
    unittest::rtos::MutexMock mock;
    unittest::rtos::injectMock(user, mock);

    {
        InSequence seq;
        EXPECT_CALL(mock, release()).Times(1);
    }

    user.mMutex.release();
}

// releaseFromISR
TEST(MutexMockTest, releaseFromISR_shouldMock)
{
    unittest::rtos::MutexUser user;
    unittest::rtos::MutexMock mock;
    unittest::rtos::injectMock(user, mock);

    bool hasWokenThread = false;

    {
        InSequence seq;
        EXPECT_CALL(mock, releaseFromISR(Eq(std::cref(hasWokenThread)))).Times(1);
    }

    user.mMutex.releaseFromISR(hasWokenThread);
}

TEST(MutexMockTest, underlyingObjectShouldExist)
{
    unittest::rtos::MutexUser user;
    unittest::rtos::MutexMock mock;
    unittest::rtos::injectMock(user, mock);

    outpost::posix::Mutex* mutex = mock.getUnderlyingObject();

    ASSERT_NE(nullptr, mutex);

    ASSERT_TRUE(mutex->acquire());
}
