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

#include <outpost/rtos/thread.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <unittest/rtos/mock.h>
#include <unittest/rtos/thread_mock.h>

using namespace outpost::rtos;
using namespace ::testing;

// ---------------------------------------------------------------------------
static constexpr outpost::time::Milliseconds anyDuration(1000U);
static constexpr outpost::rtos::Thread::Identifier anyID = 42U;
static constexpr uint8_t anyPrio = 43U;

// ---------------------------------------------------------------------------
namespace unittest
{
namespace rtos
{

struct ThreadUser : public outpost::rtos::Thread
{
    ThreadUser() : outpost::rtos::Thread(0, Thread::defaultStackSize, "thd1")
    {
    }

    void
    run() override
    {
        while (true)
        {
            outpost::rtos::Thread::sleep(anyDuration);
        }
    }
};

}  // namespace rtos
}  // namespace unittest

// ---------------------------------------------------------------------------
/**
 * Tests for \c ThreadMock
 *
 * function:
 * - dummy thread
 * - retrieve constructor arguments
 * - execute run
 * - 'start'
 * - 'getIdentifier'
 * - 'getCurrentThreadIdentifier'
 * - 'setPriority'
 * - 'getPriority'
 * - 'yield'
 * - 'sleep'
 *
 * variation:
 * - ExecuteAs for
 *   - 'getCurrentThreadIdentifier'
 *   - 'yield'
 *   - 'sleep'
 */

// ---------------------------------------------------------------------------
// dummy thread
static void
sleepAndYield()
{
    outpost::rtos::Thread::sleep(anyDuration);
    outpost::rtos::Thread::yield();
}

TEST(ThreadMockTest, shouldCreateDummyThread)
{
    unittest::rtos::DummyThread user;
    unittest::rtos::ThreadMock mock;
    unittest::rtos::injectMock(user, mock);

    {
        InSequence seq;
        EXPECT_CALL(mock, sleep(Eq(anyDuration))).Times(1);
        EXPECT_CALL(mock, yield()).Times(1U);
    }

    sleepAndYield();
}

// ---------------------------------------------------------------------------
// retrieve constructor arguments
TEST(ThreadMockTest, shouldRetrieveName)
{
    unittest::rtos::ThreadUser user;
    unittest::rtos::ThreadMock mock;
    unittest::rtos::injectMock(user, mock);

    auto* thread = mock.getMockable();
    const char* name = thread->getName();

    ASSERT_THAT(name, StrEq("thd1"));
}

// ---------------------------------------------------------------------------
// execute run
TEST(ThreadMockTest, shouldExecuteThread)
{
    unittest::rtos::ThreadUser user;
    unittest::rtos::ThreadMock mock;
    unittest::rtos::injectMock(user, mock);

    {
        EXPECT_CALL(mock, sleep(Eq(anyDuration))).WillOnce(Throw(std::exception()));
    }

    try
    {
        auto* thread = mock.getMockable();
        thread->executeThread();
    }
    catch (...)
    {
    }
}

TEST(ThreadMockTest, shouldExecuteCorrectThread)
{
    unittest::rtos::ThreadUser user1;
    unittest::rtos::ThreadMock mock1;

    unittest::rtos::ThreadUser user2;
    unittest::rtos::ThreadMock mock2;

    unittest::rtos::injectMock(user1, mock1);
    unittest::rtos::injectMock(user2, mock2);

    {
        EXPECT_CALL(mock1, sleep(Eq(anyDuration))).WillRepeatedly(Throw(std::exception()));
        EXPECT_CALL(mock2, sleep(Eq(anyDuration))).Times(1).WillRepeatedly(Throw(std::exception()));
    }

    try
    {
        auto* thread = mock1.getMockable();
        thread->executeThread();
    }
    catch (...)
    {
    }

    try
    {
        auto* thread = mock2.getMockable();
        thread->executeThread();
    }
    catch (...)
    {
    }
}

// ---------------------------------------------------------------------------
// start
TEST(ThreadMockTest, start_shouldMock)
{
    unittest::rtos::ThreadUser user;
    unittest::rtos::ThreadMock mock;
    unittest::rtos::injectMock(user, mock);

    {
        EXPECT_CALL(mock, start()).Times(1);
    }

    auto* thread = dynamic_cast<outpost::rtos::Thread*>(&user);
    thread->start();
}

// getIdentifier
TEST(ThreadMockTest, getIdentifier_shouldMock)
{
    unittest::rtos::ThreadUser user;
    unittest::rtos::ThreadMock mock;
    unittest::rtos::injectMock(user, mock);

    {
        EXPECT_CALL(mock, getIdentifier()).WillOnce(Return(anyID));
    }

    auto* thread = dynamic_cast<outpost::rtos::Thread*>(&user);
    auto id = thread->getIdentifier();

    ASSERT_THAT(id, Eq(anyID));
}

// getCurrentThreadIdentifier
TEST(ThreadMockTest, getCurrentThreadIdentifier_shouldMock)
{
    unittest::rtos::ThreadUser user;
    unittest::rtos::ThreadMock mock;
    unittest::rtos::injectMock(user, mock);

    {
        EXPECT_CALL(mock, getCurrentThreadIdentifier()).WillOnce(Return(anyID));
    }

    auto* thread = dynamic_cast<outpost::rtos::Thread*>(&user);
    auto id = thread->getCurrentThreadIdentifier();

    ASSERT_THAT(id, Eq(anyID));
}

// getCurrentThreadIdentifier ExecuteAs
TEST(ThreadMockTest, getCurrentThreadIdentifier_shouldMockCorrectThread)
{
    unittest::rtos::ThreadUser user1;
    unittest::rtos::ThreadUser user2;
    unittest::rtos::ThreadMock mock1;
    unittest::rtos::ThreadMock mock2;

    unittest::rtos::injectMock(user1, mock1);
    unittest::rtos::injectMock(user2, mock2);

    {
        InSequence seq;
        EXPECT_CALL(mock1, getCurrentThreadIdentifier()).WillOnce(Return(0U));
        EXPECT_CALL(mock2, getCurrentThreadIdentifier()).WillOnce(Return(anyID));
        EXPECT_CALL(mock1, getCurrentThreadIdentifier()).WillOnce(Return(3U));
    }

    outpost::rtos::Thread::Identifier id1;
    {
        // first created -> 0
        id1 = outpost::rtos::Thread::getCurrentThreadIdentifier();
    }

    outpost::rtos::Thread::Identifier id2;
    {
        // execute as user2 -> anyID
        unittest::rtos::ExecuteAs executeAs(user2);
        id2 = outpost::rtos::Thread::getCurrentThreadIdentifier();
    }

    // again first created -> 3
    auto id3 = outpost::rtos::Thread::getCurrentThreadIdentifier();

    ASSERT_THAT(id1, Eq(0U));
    ASSERT_THAT(id2, Eq(anyID));
    ASSERT_THAT(id3, Eq(3U));
}

// setPriority
TEST(ThreadMockTest, setPriority_shouldMock)
{
    unittest::rtos::ThreadUser user;
    unittest::rtos::ThreadMock mock;
    unittest::rtos::injectMock(user, mock);

    {
        EXPECT_CALL(mock, setPriority(Eq(anyPrio))).Times(1U);
    }

    auto* thread = dynamic_cast<outpost::rtos::Thread*>(&user);
    thread->setPriority(anyPrio);
}

// getPriority
TEST(ThreadMockTest, getPriority_shouldMock)
{
    unittest::rtos::ThreadUser user;
    unittest::rtos::ThreadMock mock;
    unittest::rtos::injectMock(user, mock);

    {
        EXPECT_CALL(mock, getPriority()).WillOnce(Return(anyPrio));
    }

    auto* thread = dynamic_cast<outpost::rtos::Thread*>(&user);
    auto prio = thread->getPriority();

    ASSERT_THAT(prio, Eq(anyPrio));
}

// yield
TEST(ThreadMockTest, yield_shouldMock)
{
    unittest::rtos::ThreadUser user;
    unittest::rtos::ThreadMock mock;
    unittest::rtos::injectMock(user, mock);

    {
        EXPECT_CALL(mock, yield()).Times(1U);
    }

    auto* thread = dynamic_cast<outpost::rtos::Thread*>(&user);
    thread->yield();
}

// yield ExecuteAs
TEST(ThreadMockTest, yield_shouldMockCorrectThread)
{
    unittest::rtos::ThreadUser user1;
    unittest::rtos::ThreadUser user2;
    unittest::rtos::ThreadMock mock1;
    unittest::rtos::ThreadMock mock2;
    unittest::rtos::injectMock(user1, mock1);
    unittest::rtos::injectMock(user2, mock2);

    {
        InSequence seq;
        EXPECT_CALL(mock1, yield()).Times(1U);
        EXPECT_CALL(mock2, yield()).Times(1U);
        EXPECT_CALL(mock1, yield()).Times(1U);
    }

    {
        unittest::rtos::ExecuteAs executeAs1(user1);
        outpost::rtos::Thread::yield();

        {
            unittest::rtos::ExecuteAs executeAs2(user2);
            outpost::rtos::Thread::yield();
        }

        outpost::rtos::Thread::yield();
    }
}

// sleep
TEST(ThreadMockTest, sleep_shouldMock)
{
    unittest::rtos::ThreadUser user;
    unittest::rtos::ThreadMock mock;
    unittest::rtos::injectMock(user, mock);

    {
        EXPECT_CALL(mock, sleep(Eq(anyDuration))).Times(1U);
    }

    outpost::rtos::Thread::sleep(anyDuration);
}

// sleep executeAs
TEST(ThreadMockTest, sleep_shouldMockCorrectThread)
{
    unittest::rtos::ThreadUser user1;
    unittest::rtos::ThreadUser user2;
    unittest::rtos::ThreadMock mock1;
    unittest::rtos::ThreadMock mock2;
    unittest::rtos::injectMock(user1, mock1);
    unittest::rtos::injectMock(user2, mock2);

    {
        InSequence seq;
        EXPECT_CALL(mock1, sleep(Eq(anyDuration))).Times(1U);
        EXPECT_CALL(mock2, sleep(Eq(anyDuration * 2))).Times(1U);
        EXPECT_CALL(mock1, sleep(Eq(anyDuration * 3))).Times(1U);
    }

    {
        unittest::rtos::ExecuteAs executeAs1(user1);
        outpost::rtos::Thread::sleep(anyDuration);

        {
            unittest::rtos::ExecuteAs executeAs2(user2);
            outpost::rtos::Thread::sleep(anyDuration * 2);
        }

        outpost::rtos::Thread::sleep(anyDuration * 3);
    }
}

TEST(ThreadMockTest, underlyingObjectShouldExist)
{
    unittest::rtos::ThreadUser user;
    unittest::rtos::ThreadMock mock;
    unittest::rtos::injectMock(user, mock);

    const outpost::posix::Thread* threadObj = mock.getUnderlyingObject();
    ASSERT_NE(nullptr, threadObj);

    auto threadId = threadObj->getCurrentThreadIdentifier();

    // threadId must be set here
    ASSERT_NE(0xFFFFFFFF, threadId);
}
