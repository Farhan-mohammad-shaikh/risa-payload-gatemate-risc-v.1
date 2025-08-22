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

#include <unittest/rtos/periodic_task_manager_mock.h>
#include <unittest/rtos/thread_mock.h>

using namespace outpost::rtos;
using namespace ::testing;

// ---------------------------------------------------------------------------
static constexpr outpost::time::Milliseconds anyDuration(1000U);

// ---------------------------------------------------------------------------
namespace unittest
{
namespace rtos
{

struct PeriodicTaskManagerUser
{
    static constexpr size_t queueSize = 10U;

    PeriodicTaskManagerUser()
    {
    }

    static void
    run1()
    {
        outpost::rtos::PeriodicTaskManager ptm;
        while (true)
        {
            auto error = ptm.nextPeriod(anyDuration);
            if (error == outpost::rtos::PeriodicTaskManager::Status::timeout)
            {
                break;
            }
        }
    }

    static void
    run2()
    {
        outpost::rtos::PeriodicTaskManager ptm;
        while (true)
        {
            auto status = ptm.status();
            if (status == outpost::rtos::PeriodicTaskManager::Status::timeout)
            {
                break;
            }
        }
    }

    static void
    run3()
    {
        outpost::rtos::PeriodicTaskManager ptm;
        ptm.cancel();
    }
};

}  // namespace rtos
}  // namespace unittest

// ---------------------------------------------------------------------------
/**
 * Tests for \c PeriodicTaskManagerMock
 *
 * function:
 * - 'nextPeriod'
 * - 'status'
 * - 'cancel'
 * - should raise assertion if mock was not used but registered
 * - multiple periodic task manager
 * - should not register twice
 */

// ---------------------------------------------------------------------------
// send
TEST(PeriodicTaskManagerMockTest, nextPeriod_shouldMock)
{
    unittest::rtos::PeriodicTaskManagerUser user;
    unittest::rtos::PeriodicTaskManagerMock mock;
    unittest::rtos::registerMock(mock);

    {
        EXPECT_CALL(mock, nextPeriod(anyDuration))
                .WillOnce(Return(PeriodicTaskManager::Status::timeout));
    }

    user.run1();
}

// status
TEST(PeriodicTaskManagerMockTest, status_shouldMock)
{
    unittest::rtos::PeriodicTaskManagerUser user;
    unittest::rtos::PeriodicTaskManagerMock mock;
    unittest::rtos::registerMock(mock);

    {
        EXPECT_CALL(mock, status()).WillOnce(Return(PeriodicTaskManager::Status::timeout));
    }

    user.run2();
}

// cancel
TEST(PeriodicTaskManagerMockTest, cancel_shouldMock)
{
    unittest::rtos::PeriodicTaskManagerUser user;
    unittest::rtos::PeriodicTaskManagerMock mock;
    unittest::rtos::registerMock(mock);

    {
        EXPECT_CALL(mock, cancel()).Times(1U);
    }

    user.run3();
}

// ---------------------------------------------------------------------------
// assertion
TEST(PeriodicTaskManagerMockTest, shouldRaiseAssertionWhenMockWasNotUsed)
{
    unittest::rtos::PeriodicTaskManagerMock mock;
    unittest::rtos::registerMock(mock);

    ASSERT_DEATH(mock.~PeriodicTaskManagerMock(), _);

    // use the mock, otherwise the test will fail when the mock really gets
    // destroyed.
    PeriodicTaskManager ptm;
}

// multiple instances
TEST(PeriodicTaskManagerMockTest, shouldUseMultipleInstances)
{
    unittest::rtos::PeriodicTaskManagerUser user;
    unittest::rtos::PeriodicTaskManagerMock mock1;
    unittest::rtos::PeriodicTaskManagerMock mock2;

    unittest::rtos::registerMock(mock1);
    unittest::rtos::registerMock(mock2);

    {
        EXPECT_CALL(mock1, nextPeriod(anyDuration))
                .WillOnce(Return(PeriodicTaskManager::Status::timeout));
        EXPECT_CALL(mock2, status()).WillOnce(Return(PeriodicTaskManager::Status::timeout));
    }

    user.run1();  // use mock1
    user.run2();  // use mock2
}

// register twice
TEST(PeriodicTaskManagerMockTest, shouldNotRegisterMockTwice)
{
    unittest::rtos::PeriodicTaskManagerMock mock;

    unittest::rtos::registerMock(mock);
    ASSERT_DEATH(unittest::rtos::registerMock(mock), _);

    // use mock, otherwise the constructor will raise an assertion.
    PeriodicTaskManager ptm;
}

TEST(PeriodicTaskManager, underlyingObjectShouldExist)
{
    unittest::rtos::PeriodicTaskManagerMock mock;
    unittest::rtos::registerMock(mock);
    outpost::rtos::PeriodicTaskManager ptm;
    const outpost::posix::PeriodicTaskManager* taskManager = mock.getUnderlyingObject();
    ASSERT_NE(nullptr, taskManager);
}
