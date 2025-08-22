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

#include "outpost/hal/gpio/input_pin.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <unittest/hal/can_bus_stub.h>
#include <unittest/hal/gpio_stub.h>
#include <unittest/harness.h>

using namespace outpost::hal;
using namespace ::testing;

// ----------------------------------------------------------------------------

namespace input_pin_test
{

class Handler : public outpost::Callable
{
public:
    explicit Handler(InterruptType type) :
        mHandler(type, this, &Handler::onInterrupt), mCalled(false)
    {
    }

    Handler(const Handler&) = delete;

    Handler&
    operator=(const Handler&) = delete;

    void
    onInterrupt(InterruptType /* type */)
    {
        mCalled = true;
    }

    outpost::hal::InterruptHandler mHandler;
    bool mCalled;
};

class Handler2 : public outpost::Callable
{
public:
    explicit Handler2(InterruptType type) :
        mHandler(type, this, &Handler2::onInterrupt), mCalled(false)
    {
    }

    Handler2(const Handler2&) = delete;

    Handler2&
    operator=(const Handler2&) = delete;

    void
    onInterrupt()
    {
        mCalled = true;
    }

    outpost::hal::InterruptHandler mHandler;
    bool mCalled;
};

}  // namespace input_pin_test

using namespace input_pin_test;

// ---------------------------------------------------------------------------
TEST(InterruptHandlerTest, shouldNotBeCalledInitially)
{
    Handler handler(outpost::hal::InterruptType::fallingEdge);

    ASSERT_THAT(handler.mCalled, IsFalse());
}

TEST(InterruptHandlerTest, shouldCallInterruptHandler)
{
    Handler handler(outpost::hal::InterruptType::fallingEdge);

    handler.mHandler.callHandler(outpost::hal::InterruptType::fallingEdge);

    ASSERT_THAT(handler.mCalled, IsTrue());
}

TEST(InterruptHandlerTest, shouldCallInterruptHandlerWithoutInterruptTypeAsArgument)
{
    Handler2 handler(outpost::hal::InterruptType::fallingEdge);

    handler.mHandler.callHandler(outpost::hal::InterruptType::fallingEdge);

    ASSERT_THAT(handler.mCalled, IsTrue());
}

TEST(InterruptHandlerTest, shouldNotCallInterruptHandler)
{
    Handler handler(outpost::hal::InterruptType::fallingEdge);

    handler.mHandler.callHandler(outpost::hal::InterruptType::risingEdge);

    ASSERT_THAT(handler.mCalled, IsFalse());
}

TEST(InterruptHandlerTest, shouldCallInterruptHandlerWhenListeningToBoth1)
{
    Handler handler(outpost::hal::InterruptType::bothEdges);

    handler.mHandler.callHandler(outpost::hal::InterruptType::risingEdge);

    ASSERT_THAT(handler.mCalled, IsTrue());
}

TEST(InterruptHandlerTest, shouldCallInterruptHandlerWhenListeningToBoth2)
{
    Handler handler(outpost::hal::InterruptType::bothEdges);

    handler.mHandler.callHandler(outpost::hal::InterruptType::fallingEdge);

    ASSERT_THAT(handler.mCalled, IsTrue());
}

TEST(InterruptHandlerTest, shouldCallInterruptHandlerWhenListeningToLowAndHighLevel1)
{
    Handler handler(outpost::hal::InterruptType::bothLevels);

    handler.mHandler.callHandler(outpost::hal::InterruptType::highLevel);

    ASSERT_THAT(handler.mCalled, IsTrue());
}

TEST(InterruptHandlerTest, shouldCallInterruptHandlerWhenListeningToLowAndHighLevel2)
{
    Handler handler(outpost::hal::InterruptType::bothLevels);

    handler.mHandler.callHandler(outpost::hal::InterruptType::lowLevel);

    ASSERT_THAT(handler.mCalled, IsTrue());
}

TEST(InterruptHandlerTest, shouldNotCallInterruptHandlerWhenListeningToLowAndHighLevel)
{
    Handler handler(outpost::hal::InterruptType::bothLevels);

    handler.mHandler.callHandler(outpost::hal::InterruptType::risingEdge);
    handler.mHandler.callHandler(outpost::hal::InterruptType::fallingEdge);

    ASSERT_THAT(handler.mCalled, IsFalse());
}

// ---------------------------------------------------------------------------
TEST(InputPinTest, shouldPerDefaultReturnFalse)
{
    InputPin pin;

    auto value = pin.read();
    ASSERT_THAT(value.error(), Eq(ReturnCode::failure));
}

TEST(InputPinTest, shouldFailToAddInterruptHandlerDuringOperation)
{
    Handler handler(outpost::hal::InterruptType::fallingEdge);
    InputPin pin;
    pin.initialized();
    EXPECT_OUTPOST_ASSERTION_DEATH(pin.registerInterruptHandler(handler.mHandler),
                                   "Can not add an interrupt handler during operation!");
}

TEST(InputPinTest, shouldNotAddHandlerAfterInitialization)
{
    Handler handler(outpost::hal::InterruptType::fallingEdge);
    InputPin pin;

    pin.initialized();

    EXPECT_OUTPOST_ASSERTION_DEATH(pin.registerInterruptHandler(handler.mHandler),
                                   "Can not add an interrupt handler during operation!");

    pin.callHandlers(InterruptType::fallingEdge);

    ASSERT_THAT(handler.mCalled, IsFalse());
}

TEST(InputPinTest, shouldCallHandlers)
{
    Handler handler(outpost::hal::InterruptType::fallingEdge);
    InputPin pin;
    pin.registerInterruptHandler(handler.mHandler);

    pin.callHandlers(InterruptType::fallingEdge);

    ASSERT_THAT(handler.mCalled, IsTrue());
}

TEST(InputPinTest, shouldNotCallHandlers)
{
    Handler handler(outpost::hal::InterruptType::fallingEdge);
    InputPin pin;
    pin.registerInterruptHandler(handler.mHandler);

    pin.callHandlers(InterruptType::risingEdge);

    ASSERT_THAT(handler.mCalled, IsFalse());
}

// ---------------------------------------------------------------------------
#if defined(OUTPOST_USE_ASSERT) && OUTPOST_USE_ASSERT == 1

TEST(SimpleInputPinTest, shouldDetectOutOfBounds)
{
    unittest::hal::GpiStub<uint32_t> stub;
    EXPECT_DEATH(SimpleInputPin<uint32_t> pin(stub, 32U), "Index out of range!");
}

#endif

TEST(SimpleInputPinTest, shouldRead0OnPin)
{
    Handler handler(outpost::hal::InterruptType::fallingEdge);
    unittest::hal::GpiStub<uint32_t> stub;
    SimpleInputPin<uint32_t> pin(stub, Index<0U>());
    pin.registerInterruptHandler(handler.mHandler);
    stub.set(0x10);

    auto value = *pin.read();

    ASSERT_THAT(value, IsFalse());
}

TEST(SimpleInputPinTest, shouldRead1OnPin)
{
    Handler handler(outpost::hal::InterruptType::fallingEdge);
    unittest::hal::GpiStub<uint32_t> stub;
    SimpleInputPin<uint32_t> pin(stub, 0);
    pin.registerInterruptHandler(handler.mHandler);
    stub.set(0x01);

    auto value = *pin.read();

    ASSERT_THAT(value, IsTrue());
}

TEST(SimpleInputPinTest, shouldCallHandlers)
{
    Handler handler(outpost::hal::InterruptType::fallingEdge);
    unittest::hal::GpiStub<uint32_t> stub;
    SimpleInputPin<uint32_t> pin(stub, 0);
    pin.registerInterruptHandler(handler.mHandler);

    pin.callHandlers(0x01, InterruptType::fallingEdge);

    ASSERT_THAT(handler.mCalled, IsTrue());
}

TEST(SimpleInputPinTest, shouldNotCallHandlers)
{
    Handler handler(outpost::hal::InterruptType::fallingEdge);
    unittest::hal::GpiStub<uint32_t> stub;
    SimpleInputPin<uint32_t> pin(stub, 0);
    pin.registerInterruptHandler(handler.mHandler);

    pin.callHandlers(0x01, InterruptType::risingEdge);

    ASSERT_THAT(handler.mCalled, IsFalse());
}

TEST(SimpleInputPinTest, shouldNotCallHandlersDueTheMask)
{
    Handler handler(outpost::hal::InterruptType::fallingEdge);
    unittest::hal::GpiStub<uint32_t> stub;
    SimpleInputPin<uint32_t> pin(stub, 0);
    pin.registerInterruptHandler(handler.mHandler);

    pin.callHandlers(0x02, InterruptType::fallingEdge);

    ASSERT_THAT(handler.mCalled, IsFalse());
}
