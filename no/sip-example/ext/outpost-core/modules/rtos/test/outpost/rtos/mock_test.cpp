/*
 * Copyright (c) 2024, Janosch Reinking
 * Copyright (c) 2024, Tepe, Alexander
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
#include <unittest/rtos/queue_mock.h>

using namespace outpost::time::literals;
using namespace unittest::rtos;
using namespace outpost::rtos;
using namespace ::testing;

// ---------------------------------------------------------------------------
namespace unittest
{
namespace rtos
{

struct MutexUser1
{
    outpost::rtos::Mutex mMutex;
};

struct MutexUser2
{
};

struct MutexUser3
{
    outpost::rtos::Mutex mMutex1;
    outpost::rtos::Mutex mMutex2;
};

}  // namespace rtos
}  // namespace unittest

// ---------------------------------------------------------------------------
/**
 * Tests for \c Mock
 *
 * function:
 * - should not inject mock twice
 * - should raise assertion if mock could not be injected
 * - should inject multiple mocks
 */

TEST(MockTest, shouldNotInjectMockTwice)
{
    unittest::rtos::MutexUser1 user;
    unittest::rtos::MutexMock mock;

    unittest::rtos::injectMock(user, mock);

    // inject mock twice -> error
    ASSERT_DEATH(unittest::rtos::injectMock(user, mock), _);
}

TEST(MockTest, shouldRaiseAssertionIfMockCouldNotBeInjected)
{
    unittest::rtos::MutexUser2 user;
    unittest::rtos::MutexMock mock;

    // user has no mockable -> error
    ASSERT_DEATH(unittest::rtos::injectMock(user, mock), _);
}

TEST(MockTest, shouldInjectMultipleMocks)
{
    unittest::rtos::MutexUser3 user;
    unittest::rtos::MutexMock mock1;
    unittest::rtos::MutexMock mock2;

    unittest::rtos::injectMock(user, mock1);
    unittest::rtos::injectMock(user, mock2);

    {
        InSequence seq;
        EXPECT_CALL(mock1, acquire()).WillOnce(Return(false));
        EXPECT_CALL(mock2, acquire()).WillOnce(Return(true));
    }

    user.mMutex1.acquire();
    user.mMutex2.acquire();
}

TEST(MockTest, shouldInjectMultipleMocksWithIndex)
{
    unittest::rtos::MutexUser3 user;
    unittest::rtos::MutexMock mock1;
    unittest::rtos::MutexMock mock2;

    unittest::rtos::injectMock(user, mock1, 0);
    unittest::rtos::injectMock(user, mock2, 1);

    {
        InSequence seq;
        EXPECT_CALL(mock1, acquire()).WillOnce(Return(false));
        EXPECT_CALL(mock2, acquire()).WillOnce(Return(true));
    }

    user.mMutex1.acquire();
    user.mMutex2.acquire();
}

TEST(MockTest, shouldInjectMultipleMocksWithMixedIndex)
{
    unittest::rtos::MutexUser3 user;
    unittest::rtos::MutexMock mock1;
    unittest::rtos::MutexMock mock2;

    unittest::rtos::injectMock(user, mock1);
    // Offset zero should have been already taken by default `injectMock(..)`
    EXPECT_DEATH(unittest::rtos::injectMock(user, mock2, 0), _);
    unittest::rtos::injectMock(user, mock2, 1);

    {
        InSequence seq;
        EXPECT_CALL(mock1, acquire()).WillOnce(Return(false));
        EXPECT_CALL(mock2, acquire()).WillOnce(Return(true));
    }

    user.mMutex1.acquire();
    user.mMutex2.acquire();
}

TEST(MockTest, shouldInjectMultipleMocksInDifferentUsers)
{
    unittest::rtos::MutexUser3 user1;
    unittest::rtos::MutexUser3 user2;
    unittest::rtos::MutexMock mock1;
    unittest::rtos::MutexMock mock2;
    unittest::rtos::MutexMock mock3;
    unittest::rtos::MutexMock mock4;

    unittest::rtos::injectMock(user1, mock1);
    unittest::rtos::injectMock(user1, mock2);
    unittest::rtos::injectMock(user2, mock3);
    unittest::rtos::injectMock(user2, mock4);

    {
        InSequence seq;
        EXPECT_CALL(mock1, acquire()).WillOnce(Return(false));
        EXPECT_CALL(mock2, acquire()).WillOnce(Return(true));
        EXPECT_CALL(mock3, acquire()).WillOnce(Return(true));
        EXPECT_CALL(mock4, acquire()).WillOnce(Return(false));

        EXPECT_CALL(mock1, acquire()).WillOnce(Return(true));
        EXPECT_CALL(mock2, acquire()).WillOnce(Return(true));
        EXPECT_CALL(mock3, acquire()).WillOnce(Return(false));
        EXPECT_CALL(mock4, acquire()).WillOnce(Return(false));
    }

    ASSERT_THAT(user1.mMutex1.acquire(), IsFalse());
    ASSERT_THAT(user1.mMutex2.acquire(), IsTrue());
    ASSERT_THAT(user2.mMutex1.acquire(), IsTrue());
    ASSERT_THAT(user2.mMutex2.acquire(), IsFalse());

    ASSERT_THAT(user1.mMutex1.acquire(), IsTrue());
    ASSERT_THAT(user1.mMutex2.acquire(), IsTrue());
    ASSERT_THAT(user2.mMutex1.acquire(), IsFalse());
    ASSERT_THAT(user2.mMutex2.acquire(), IsFalse());
}

TEST(MockTest, shouldInspectMockable)
{
    outpost::rtos::Queue<int> queue(10);
    NiceMock<unittest::rtos::QueueMock<int>> queueMock(MockMode::inspect);
    unittest::rtos::injectMock(queue, queueMock);

    int callCounter = 0;

    EXPECT_CALL(queueMock, send(_)).WillRepeatedly([&]() {
        callCounter++;
        return Ignore();  // return value will be ignored
    });

    queue.send(1);
    queue.send(2);
    queue.send(3);

    int value = 0;
    ASSERT_THAT(queue.receive(value, 0_s), IsTrue());
    ASSERT_THAT(value, Eq(1));
    ASSERT_THAT(queue.receive(value, 0_s), IsTrue());
    ASSERT_THAT(value, Eq(2));
    ASSERT_THAT(queue.receive(value, 0_s), IsTrue());
    ASSERT_THAT(value, Eq(3));

    ASSERT_THAT(callCounter, Eq(3));
}

TEST(MockTest, negativeIndexShouldFail)
{
    unittest::rtos::MutexUser1 user;
    unittest::rtos::MutexMock mock;

    // inject at negative index -> error
    ASSERT_DEATH(unittest::rtos::injectMock(user, mock, -1), _);
}

TEST(MockTest, underlyingObjectShouldAcquire)
{
    unittest::rtos::MutexUser1 user;
    unittest::rtos::MutexMock mock;
    unittest::rtos::injectMock(user, mock);

    outpost::posix::Mutex* mutex = mock.getUnderlyingObject();

    ASSERT_TRUE(mutex->acquire());
}
