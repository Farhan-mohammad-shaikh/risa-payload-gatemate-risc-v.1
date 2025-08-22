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

#include <unittest/rtos/barrier_mock.h>

#include <thread>

using namespace outpost::rtos;
using namespace ::testing;

// ---------------------------------------------------------------------------
namespace unittest
{
namespace rtos
{

struct BarrierUser
{
    BarrierUser() : mBarrier(1)
    {
    }

    outpost::rtos::Barrier mBarrier;
};

struct BarrierUser2
{
    BarrierUser2() : mBarrier(2)
    {
    }

    outpost::rtos::Barrier mBarrier;
};

}  // namespace rtos
}  // namespace unittest

// ---------------------------------------------------------------------------
/**
 * Tests for \c BarrierMock
 *
 * function:
 * - 'now'
 */

TEST(BarrierMockTest, wait_shouldMock)
{
    unittest::rtos::BarrierUser user;
    unittest::rtos::BarrierMock mock;
    unittest::rtos::injectMock(user, mock);

    {
        EXPECT_CALL(mock, wait()).Times(1U);
    }

    user.mBarrier.wait();
}

TEST(BarrierMockTest, underlyingObjectShouldExist)
{
    unittest::rtos::BarrierUser user;
    unittest::rtos::BarrierMock mock;
    unittest::rtos::injectMock(user, mock);

    const outpost::posix::Barrier* barrier = mock.getUnderlyingObject();

    ASSERT_NE(nullptr, barrier);
}

TEST(BarrierMockTest, underlyingObjectWorks)
{
    unittest::rtos::BarrierUser2 user;
    unittest::rtos::BarrierMock mock;
    unittest::rtos::injectMock(user, mock);

    outpost::posix::Barrier* barrier = mock.getUnderlyingObject();
    int barrierWorkCounter = 0;

    std::thread thd1([&]() {
        barrier->wait();
        barrierWorkCounter++;
    });
    std::thread thd2([&]() {
        barrier->wait();
        barrierWorkCounter++;
    });
    thd1.join();
    thd2.join();
    // if control flow reaches here, the barrier has resolved and threads have exited
    ASSERT_THAT(barrierWorkCounter, Gt(0));
}
