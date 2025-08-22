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

#include <unittest/harness.h>
#include <unittest/rtos/queue_mock.h>

using namespace outpost::rtos;
using namespace ::testing;

// ---------------------------------------------------------------------------
static constexpr outpost::time::Milliseconds anyDuration(1000U);

// ---------------------------------------------------------------------------
namespace unittest
{
namespace rtos
{

struct QueueUser
{
    static constexpr size_t queueSize = 10U;

    QueueUser() : mQueue(queueSize)
    {
    }

    outpost::rtos::Queue<int> mQueue;
};

}  // namespace rtos
}  // namespace unittest

// ---------------------------------------------------------------------------
/**
 * Tests for \c QueueMock
 *
 * function:
 * - 'send'
 * - 'sendFromISR'
 * - 'receive'
 * - 'receiveFromISR'
 * - 'clear'
 * - 'getNumberOfPendingMessages'
 */

// ---------------------------------------------------------------------------
// injectMock
TEST(QueueMockTest, send_notInjectMockOfDifferentType)
{
    unittest::rtos::QueueUser user;
    unittest::rtos::QueueMock<double /* int */> mock;

    EXPECT_OUTPOST_ASSERTION_DEATH(unittest::rtos::injectMock(user, mock),
                                   "Can not inject QueueMock<T> into Queue<U> with U != T");
}

// send
TEST(QueueMockTest, send_shouldMock)
{
    unittest::rtos::QueueUser user;
    unittest::rtos::QueueMock<int> mock;
    unittest::rtos::injectMock(user, mock);

    {
        InSequence seq;
        EXPECT_CALL(mock, send(Eq(42))).WillOnce(Return(true));
        EXPECT_CALL(mock, send(Eq(42))).WillOnce(Return(false));
    }

    const bool val1 = user.mQueue.send(42);
    const bool val2 = user.mQueue.send(42);

    ASSERT_THAT(val1, IsTrue());
    ASSERT_THAT(val2, IsFalse());
}

// sendFromISR
TEST(QueueMockTest, sendFromISR_shouldMock)
{
    unittest::rtos::QueueUser user;
    unittest::rtos::QueueMock<int> mock;
    unittest::rtos::injectMock(user, mock);

    bool hasWokenThread = false;

    {
        InSequence seq;
        EXPECT_CALL(mock, sendFromISR(Eq(42), Eq(std::cref(hasWokenThread))))
                .WillOnce(Return(true));
        EXPECT_CALL(mock, sendFromISR(Eq(42), Eq(std::cref(hasWokenThread))))
                .WillOnce(Return(false));
    }

    const bool val1 = user.mQueue.sendFromISR(42, hasWokenThread);
    const bool val2 = user.mQueue.sendFromISR(42, hasWokenThread);

    ASSERT_THAT(val1, IsTrue());
    ASSERT_THAT(val2, IsFalse());
}

// receive
TEST(QueueMockTest, receive_shouldMock)
{
    unittest::rtos::QueueUser user;
    unittest::rtos::QueueMock<int> mock;
    unittest::rtos::injectMock(user, mock);

    int value = 0;

    {
        InSequence seq;
        EXPECT_CALL(mock, receive(Eq(std::cref(value)), Eq(anyDuration)))
                .WillOnce(DoAll(unittest::rtos::ReturnElem<int>(31), Return(true)));
    }

    const bool success = user.mQueue.receive(value, anyDuration);

    ASSERT_THAT(success, IsTrue());
    ASSERT_THAT(value, Eq(31));
}

// receiveFromISR
TEST(QueueMockTest, receiveFromISR_shouldMock)
{
    unittest::rtos::QueueUser user;
    unittest::rtos::QueueMock<int> mock;
    unittest::rtos::injectMock(user, mock);

    bool hasWokenThread = false;
    int value = 0;

    {
        InSequence seq;
        EXPECT_CALL(mock, receiveFromISR(Eq(std::cref(value)), Eq(std::cref(hasWokenThread))))
                .WillOnce(DoAll(unittest::rtos::ReturnElem<int>(31), Return(true)));
    }

    const bool success = user.mQueue.receiveFromISR(value, hasWokenThread);

    ASSERT_THAT(success, IsTrue());
    ASSERT_THAT(value, Eq(31));
}

// clear
TEST(QueueMockTest, clear_shouldMock)
{
    unittest::rtos::QueueUser user;
    unittest::rtos::QueueMock<int> mock;
    unittest::rtos::injectMock(user, mock);

    {
        EXPECT_CALL(mock, clear()).Times(1U);
    }

    user.mQueue.clear();
}

// getNumberOfPendingMessages
TEST(QueueMockTest, getNumberOfPendingMessages_shouldMock)
{
    unittest::rtos::QueueUser user;
    unittest::rtos::QueueMock<int> mock;
    unittest::rtos::injectMock(user, mock);

    {
        EXPECT_CALL(mock, getNumberOfPendingMessages()).WillOnce(Return(3U));
    }

    const size_t val = user.mQueue.getNumberOfPendingMessages();

    ASSERT_THAT(val, Eq(3U));
}

TEST(QueueMockTest, underlyingObjectShouldExist)
{
    unittest::rtos::QueueUser user;
    unittest::rtos::QueueMock<int> mock;
    unittest::rtos::injectMock(user, mock);

    outpost::posix::QueueRaw* queue = mock.getUnderlyingObject();

    ASSERT_NE(nullptr, queue);

    ASSERT_EQ(0U, queue->getNumberOfPendingMessages());
}
