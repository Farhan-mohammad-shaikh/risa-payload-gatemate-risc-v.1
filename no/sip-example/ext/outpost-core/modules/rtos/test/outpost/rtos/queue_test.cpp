/*
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

#include <unittest/rtos/queue_mock.h>

using namespace outpost::rtos;
using namespace ::testing;

namespace unittest
{
namespace rtos
{

struct TestItem
{
    int intMember;
    bool boolMember;

    bool
    operator==(const TestItem& other) const
    {
        return boolMember == other.boolMember && intMember == other.intMember;
    }
};

struct DurationQueueUser
{
    static constexpr size_t queueSize = 10U;

    DurationQueueUser() : mQueue(queueSize)
    {
    }

    outpost::rtos::Queue<TestItem> mQueue;
};

}  // namespace rtos
}  // namespace unittest

TEST(QueueMockTest, shouldSendAndRecvTriviallyCopyableItems)
{
    unittest::rtos::TestItem item;
    item.intMember = 1;
    item.boolMember = false;

    unittest::rtos::DurationQueueUser user;
    unittest::rtos::QueueMock<unittest::rtos::TestItem> mock;
    unittest::rtos::injectMock(user, mock);

    {
        InSequence seq;
        EXPECT_CALL(mock, send(Eq(item))).WillOnce(Return(true));
        EXPECT_CALL(mock, send(Eq(item))).WillOnce(Return(false));
    }

    const bool val1 = user.mQueue.send(item);
    const bool val2 = user.mQueue.send(item);

    ASSERT_THAT(val1, IsTrue());
    ASSERT_THAT(val2, IsFalse());
}
