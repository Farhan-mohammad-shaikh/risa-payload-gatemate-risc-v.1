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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <unittest/rtos/semaphore_mock.h>

using namespace outpost::rtos;
using namespace ::testing;

// ---------------------------------------------------------------------------
static constexpr outpost::time::Milliseconds anyDuration(1000U);

// ---------------------------------------------------------------------------
namespace unittest
{
namespace rtos
{

struct SemaphoreUser
{
    SemaphoreUser() :
        mSemaphore(0U), mBinarySemaphore(outpost::rtos::BinarySemaphore::State::Type::acquired)
    {
    }

    outpost::rtos::Semaphore mSemaphore;
    outpost::rtos::BinarySemaphore mBinarySemaphore;
};

}  // namespace rtos
}  // namespace unittest

// ---------------------------------------------------------------------------
/**
 * Tests for \c SemaphoreMock
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
TEST(SemaphoreMockTest, acquire_shouldMock)
{
    unittest::rtos::SemaphoreUser user;
    unittest::rtos::SemaphoreMock mock;
    unittest::rtos::injectMock(user, mock);

    {
        InSequence seq;
        EXPECT_CALL(mock, acquire()).WillOnce(Return(true));
        EXPECT_CALL(mock, acquire()).WillOnce(Return(false));
    }

    const bool val1 = user.mSemaphore.acquire();
    const bool val2 = user.mSemaphore.acquire();

    ASSERT_THAT(val1, IsTrue());
    ASSERT_THAT(val2, IsFalse());
}

// acquire
TEST(SemaphoreMockTest, acquire_duration_shouldMock)
{
    unittest::rtos::SemaphoreUser user;
    unittest::rtos::SemaphoreMock mock;
    unittest::rtos::injectMock(user, mock);

    {
        InSequence seq;
        EXPECT_CALL(mock, acquire(Eq(anyDuration))).WillOnce(Return(true));
        EXPECT_CALL(mock, acquire(Eq(anyDuration))).WillOnce(Return(false));
    }

    const bool val1 = user.mSemaphore.acquire(anyDuration);
    const bool val2 = user.mSemaphore.acquire(anyDuration);

    ASSERT_THAT(val1, IsTrue());
    ASSERT_THAT(val2, IsFalse());
}

// acquireFromISR
TEST(SemaphoreMockTest, acquireFromISR_shouldMock)
{
    unittest::rtos::SemaphoreUser user;
    unittest::rtos::SemaphoreMock mock;
    unittest::rtos::injectMock(user, mock);

    bool hasWokenTask = false;

    {
        InSequence seq;
        EXPECT_CALL(mock, acquireFromISR(Eq(std::cref(hasWokenTask)))).WillOnce(Return(true));
        EXPECT_CALL(mock, acquireFromISR(Eq(std::cref(hasWokenTask)))).WillOnce(Return(false));
    }

    const bool val1 = user.mSemaphore.acquireFromISR(hasWokenTask);
    const bool val2 = user.mSemaphore.acquireFromISR(hasWokenTask);

    ASSERT_THAT(val1, IsTrue());
    ASSERT_THAT(val2, IsFalse());
}

// release
TEST(SemaphoreMockTest, release_shouldMock)
{
    unittest::rtos::SemaphoreUser user;
    unittest::rtos::SemaphoreMock mock;
    unittest::rtos::injectMock(user, mock);

    {
        InSequence seq;
        EXPECT_CALL(mock, release()).Times(1U);
    }

    user.mSemaphore.release();
}

// releaseFromISR
TEST(SemaphoreMockTest, releaseFromISR_shouldMock)
{
    unittest::rtos::SemaphoreUser user;
    unittest::rtos::SemaphoreMock mock;
    unittest::rtos::injectMock(user, mock);

    bool hasWokenTask = false;

    {
        InSequence seq;
        EXPECT_CALL(mock, releaseFromISR(Eq(std::cref(hasWokenTask)))).Times(1U);
    }

    user.mSemaphore.releaseFromISR(hasWokenTask);
}

// ---------------------------------------------------------------------------
/**
 * Tests for \c BinarySemaphore
 *
 * function:
 *  - 'acquire'
 *  - 'acquire duration'
 *  - 'acquireFromISR'
 *  - 'release'
 *  - 'releaseFromISR'
 */
// acquire
TEST(BinarySemaphoreMockTest, acquire_shouldMock)
{
    unittest::rtos::SemaphoreUser user;
    unittest::rtos::BinarySemaphoreMock mock;
    unittest::rtos::injectMock(user, mock);

    {
        InSequence seq;
        EXPECT_CALL(mock, acquire()).WillOnce(Return(true));
        EXPECT_CALL(mock, acquire()).WillOnce(Return(false));
    }

    const bool val1 = user.mBinarySemaphore.acquire();
    const bool val2 = user.mBinarySemaphore.acquire();

    ASSERT_THAT(val1, IsTrue());
    ASSERT_THAT(val2, IsFalse());
}

// acquire
TEST(BinarySemaphoreMockTest, acquire_duration_shouldMock)
{
    unittest::rtos::SemaphoreUser user;
    unittest::rtos::BinarySemaphoreMock mock;
    unittest::rtos::injectMock(user, mock);

    {
        InSequence seq;
        EXPECT_CALL(mock, acquire(Eq(anyDuration))).WillOnce(Return(true));
        EXPECT_CALL(mock, acquire(Eq(anyDuration))).WillOnce(Return(false));
    }

    const bool val1 = user.mBinarySemaphore.acquire(anyDuration);
    const bool val2 = user.mBinarySemaphore.acquire(anyDuration);

    ASSERT_THAT(val1, IsTrue());
    ASSERT_THAT(val2, IsFalse());
}

// acquireFromISR
TEST(BinarySemaphoreMockTest, acquireFromISR_shouldMock)
{
    unittest::rtos::SemaphoreUser user;
    unittest::rtos::BinarySemaphoreMock mock;
    unittest::rtos::injectMock(user, mock);

    bool hasWokenTask = false;

    {
        InSequence seq;
        EXPECT_CALL(mock, acquireFromISR(Eq(std::cref(hasWokenTask)))).WillOnce(Return(true));
        EXPECT_CALL(mock, acquireFromISR(Eq(std::cref(hasWokenTask)))).WillOnce(Return(false));
    }

    const bool val1 = user.mBinarySemaphore.acquireFromISR(hasWokenTask);
    const bool val2 = user.mBinarySemaphore.acquireFromISR(hasWokenTask);

    ASSERT_THAT(val1, IsTrue());
    ASSERT_THAT(val2, IsFalse());
}

// release
TEST(BinarySemaphoreMockTest, release_shouldMock)
{
    unittest::rtos::SemaphoreUser user;
    unittest::rtos::BinarySemaphoreMock mock;
    unittest::rtos::injectMock(user, mock);

    {
        InSequence seq;
        EXPECT_CALL(mock, release()).Times(1U);
    }

    user.mBinarySemaphore.release();
}

// releaseFromISR
TEST(BinarySemaphoreMockTest, releaseFromISR_shouldMock)
{
    unittest::rtos::SemaphoreUser user;
    unittest::rtos::BinarySemaphoreMock mock;
    unittest::rtos::injectMock(user, mock);

    bool hasWokenTask = false;

    {
        InSequence seq;
        EXPECT_CALL(mock, releaseFromISR(Eq(std::cref(hasWokenTask)))).Times(1U);
    }

    user.mBinarySemaphore.releaseFromISR(hasWokenTask);
}

TEST(BinarySemaphoreMockTest, underlyingObjectShouldExist)
{
    unittest::rtos::SemaphoreUser user;
    unittest::rtos::BinarySemaphoreMock mock;
    unittest::rtos::injectMock(user, mock);

    outpost::posix::BinarySemaphore* sema = mock.getUnderlyingObject();

    ASSERT_NE(nullptr, sema);

    sema->release();
    ASSERT_TRUE(sema->acquire());
}
