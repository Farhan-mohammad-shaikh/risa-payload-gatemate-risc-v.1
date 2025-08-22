/*
 * Copyright (c) 2023, Janosch Reinking
 * Copyright (c) 2023, Pascal Pieper
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "outpost/hal/gpio/interrupt_distribution.h"

#include "outpost/hal/gpio/output_pin.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <unittest/hal/can_bus_stub.h>
#include <unittest/hal/gpio_stub.h>
#include <unittest/harness.h>

using namespace outpost::hal;
using namespace ::testing;

// ----------------------------------------------------------------------------
namespace interrupt_distribution_test
{

class Handler : public outpost::Callable
{
public:
    explicit Handler(InterruptType type) :
        mHandler(type, this, &Handler::onInterrupt), mCalled(false)
    {
    }

    void
    onInterrupt(outpost::hal::InterruptType /* type */)
    {
        mCalled = true;
    }

    outpost::hal::InterruptHandler mHandler;
    bool mCalled;
};

}  // namespace interrupt_distribution_test

using namespace interrupt_distribution_test;

// ---------------------------------------------------------------------------
TEST(InterruptDistributionTest, shouldCallFirstHandler)
{
    Handler handler1(outpost::hal::InterruptType::fallingEdge);
    Handler handler2(outpost::hal::InterruptType::fallingEdge);
    InputPin pin1(0);
    InputPin pin2(1);
    pin1.registerInterruptHandler(handler1.mHandler);
    pin2.registerInterruptHandler(handler2.mHandler);
    InterruptDistribution interruptManager;
    interruptManager.registerPin(pin1);
    interruptManager.registerPin(pin2);

    interruptManager.callHandlers(outpost::hal::InterruptType::fallingEdge, 0U);

    ASSERT_THAT(handler1.mCalled, IsTrue());
    ASSERT_THAT(handler2.mCalled, IsFalse());
}

TEST(InterruptDistributionTest, shouldFailToAddPinDuringOperation)
{
    InputPin pin1(0);
    InterruptDistribution interruptManager;
    interruptManager.initialize();
    EXPECT_OUTPOST_ASSERTION_DEATH(interruptManager.registerPin(pin1),
                                   "Can not add an interrupt handler during operation!");
}

TEST(InterruptDistributionTest, shouldCallSecondHandler)
{
    Handler handler1(outpost::hal::InterruptType::fallingEdge);
    Handler handler2(outpost::hal::InterruptType::fallingEdge);
    InputPin pin1(0);
    InputPin pin2(1);
    pin1.registerInterruptHandler(handler1.mHandler);
    pin2.registerInterruptHandler(handler2.mHandler);
    InterruptDistribution interruptManager;
    interruptManager.registerPin(pin1);
    interruptManager.registerPin(pin2);

    interruptManager.callHandlers(outpost::hal::InterruptType::fallingEdge, 1U);

    ASSERT_THAT(handler1.mCalled, IsFalse());
    ASSERT_THAT(handler2.mCalled, IsTrue());
}

// ---------------------------------------------------------------------------
TEST(SimpleInterruptDistributionTest, shouldCallFirstHandler)
{
    Handler handler1(outpost::hal::InterruptType::fallingEdge);
    Handler handler2(outpost::hal::InterruptType::fallingEdge);
    unittest::hal::GpiStub<uint32_t> stub;
    SimpleInputPin<uint32_t> pin1(stub, 0);
    SimpleInputPin<uint32_t> pin2(stub, 1);
    pin1.registerInterruptHandler(handler1.mHandler);
    pin2.registerInterruptHandler(handler2.mHandler);
    SimpleInterruptDistribution<uint32_t> interruptManager(stub,
                                                           outpost::hal::InterruptType::bothEdges);
    interruptManager.registerPin(pin1);
    interruptManager.registerPin(pin2);
    interruptManager.initialize();

    stub.callHandler(InterruptType::fallingEdge, 0x01);

    ASSERT_THAT(handler1.mCalled, IsTrue());
    ASSERT_THAT(handler2.mCalled, IsFalse());
}

TEST(SimpleInterruptDistributionTest, shouldCallSecondHandler)
{
    Handler handler1(outpost::hal::InterruptType::fallingEdge);
    Handler handler2(outpost::hal::InterruptType::fallingEdge);
    unittest::hal::GpiStub<uint32_t> stub;
    SimpleInputPin<uint32_t> pin1(stub, 0);
    SimpleInputPin<uint32_t> pin2(stub, 1);
    pin1.registerInterruptHandler(handler1.mHandler);
    pin2.registerInterruptHandler(handler2.mHandler);
    SimpleInterruptDistribution<uint32_t> interruptManager(stub,
                                                           outpost::hal::InterruptType::bothEdges);
    interruptManager.registerPin(pin1);
    interruptManager.registerPin(pin2);
    interruptManager.initialize();

    stub.callHandler(InterruptType::fallingEdge, 0x02);

    ASSERT_THAT(handler1.mCalled, IsFalse());
    ASSERT_THAT(handler2.mCalled, IsTrue());
}

TEST(SimpleInterruptDistributionTest, shouldCallBothHandler)
{
    Handler handler1(outpost::hal::InterruptType::fallingEdge);
    Handler handler2(outpost::hal::InterruptType::fallingEdge);
    unittest::hal::GpiStub<uint32_t> stub;
    SimpleInputPin<uint32_t> pin1(stub, 0);
    SimpleInputPin<uint32_t> pin2(stub, 1);
    pin1.registerInterruptHandler(handler1.mHandler);
    pin2.registerInterruptHandler(handler2.mHandler);
    SimpleInterruptDistribution<uint32_t> interruptManager(stub,
                                                           outpost::hal::InterruptType::bothEdges);
    interruptManager.registerPin(pin1);
    interruptManager.registerPin(pin2);
    interruptManager.initialize();

    stub.callHandler(InterruptType::fallingEdge, 0x03);

    ASSERT_THAT(handler1.mCalled, IsTrue());
    ASSERT_THAT(handler2.mCalled, IsTrue());
}

TEST(SimpleInterruptDistributionTest, shouldNotCallAnyHandler)
{
    Handler handler1(outpost::hal::InterruptType::fallingEdge);
    Handler handler2(outpost::hal::InterruptType::fallingEdge);
    unittest::hal::GpiStub<uint32_t> stub;
    SimpleInputPin<uint32_t> pin1(stub, 0);
    SimpleInputPin<uint32_t> pin2(stub, 1);
    pin1.registerInterruptHandler(handler1.mHandler);
    pin2.registerInterruptHandler(handler2.mHandler);

    SimpleInterruptDistribution<uint32_t> interruptManager(stub,
                                                           outpost::hal::InterruptType::bothEdges);
    interruptManager.registerPin(pin1);
    interruptManager.registerPin(pin2);
    interruptManager.initialize();

    stub.callHandler(InterruptType::fallingEdge, 0x04);

    ASSERT_THAT(handler1.mCalled, IsFalse());
    ASSERT_THAT(handler2.mCalled, IsFalse());
}

// ---------------------------------------------------------------------------
TEST(SimpleInterruptDistributionTest, shouldNotAddPins)
{
    Handler handler1(outpost::hal::InterruptType::fallingEdge);
    Handler handler2(outpost::hal::InterruptType::fallingEdge);
    unittest::hal::GpiStub<uint32_t> stub;
    SimpleInputPin<uint32_t> pin1(stub, 0);
    SimpleInputPin<uint32_t> pin2(stub, 1);
    pin1.registerInterruptHandler(handler1.mHandler);
    pin2.registerInterruptHandler(handler2.mHandler);

    SimpleInterruptDistribution<uint32_t> interruptManager(stub,
                                                           outpost::hal::InterruptType::bothEdges);
    interruptManager.initialize();
    EXPECT_OUTPOST_ASSERTION_DEATH(interruptManager.registerPin(pin1),
                                   "Can not add an interrupt handler during operation");
    EXPECT_OUTPOST_ASSERTION_DEATH(interruptManager.registerPin(pin2),
                                   "Can not add an interrupt handler during operation");

    stub.callHandler(InterruptType::fallingEdge, 0xFF);

    ASSERT_THAT(handler1.mCalled, IsFalse());
    ASSERT_THAT(handler2.mCalled, IsFalse());
}
