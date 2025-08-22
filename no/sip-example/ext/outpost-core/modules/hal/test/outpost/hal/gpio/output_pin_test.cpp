/*
 * Copyright (c) 2023-2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "outpost/hal/gpio/output_pin.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "unittest/hal/gpio/output_pin_stub.h"
#include <unittest/hal/can_bus_stub.h>
#include <unittest/hal/gpio_stub.h>
#include <unittest/harness.h>

using namespace outpost::hal;
using namespace ::testing;

// ---------------------------------------------------------------------------
TEST(SimpleOutputPinTest, shouldForwardErrorCodeOnSet)
{
    unittest::hal::GpoStub<uint32_t> stub;
    outpost::hal::SimpleOutputPin<uint32_t> pin(stub, outpost::hal::Index<0U>());
    stub.mSet = [](uint32_t) { return outpost::hal::ReturnCode::failure; };

    auto code = pin.set();

    ASSERT_THAT(code, Eq(outpost::hal::ReturnCode::failure));
}

TEST(SimpleOutputPinTest, shouldForwardErrorCodeOnClear)
{
    unittest::hal::GpoStub<uint32_t> stub;
    outpost::hal::SimpleOutputPin<uint32_t> pin(stub, outpost::hal::Index<0U>());
    stub.mClear = [](uint32_t) { return outpost::hal::ReturnCode::failure; };

    auto code = pin.clear();

    ASSERT_THAT(code, Eq(outpost::hal::ReturnCode::failure));
}

TEST(SimpleOutputPinTest, shouldForwardErrorCodeOnToggle)
{
    unittest::hal::GpoStub<uint32_t> stub;
    outpost::hal::SimpleOutputPin<uint32_t> pin(stub, outpost::hal::Index<0U>());
    stub.mToggle = [](uint32_t) { return outpost::hal::ReturnCode::failure; };

    auto code = pin.toggle();

    ASSERT_THAT(code, Eq(outpost::hal::ReturnCode::failure));
}

// ---------------------------------------------------------------------------
TEST(SimpleOutputPinTest, shouldSetPin)
{
    unittest::hal::GpoStub<uint32_t> stub;
    outpost::hal::SimpleOutputPin<uint32_t> pin(stub, outpost::hal::Index<0U>());

    auto code = pin.set();

    ASSERT_THAT(stub.get(), Eq(1U));
    ASSERT_THAT(code, Eq(outpost::hal::ReturnCode::success));
}

TEST(SimpleOutputPinTest, shouldClearPin)
{
    unittest::hal::GpoStub<uint32_t> stub;
    outpost::hal::SimpleOutputPin<uint32_t> pin(stub, 0);

    pin.set();
    auto code = pin.clear();

    ASSERT_THAT(stub.get(), Eq(0U));
    ASSERT_THAT(code, Eq(outpost::hal::ReturnCode::success));
}

TEST(SimpleOutputPinTest, shouldTogglePin)
{
    unittest::hal::GpoStub<uint32_t> stub;
    outpost::hal::SimpleOutputPin<uint32_t> pin(stub, 0);

    auto code = pin.toggle();

    ASSERT_THAT(stub.get(), Eq(1U));
    ASSERT_THAT(code, Eq(outpost::hal::ReturnCode::success));
}

TEST(SimpleOutputPinTest, shouldTogglePinTwice)
{
    unittest::hal::GpoStub<uint32_t> stub;
    outpost::hal::SimpleOutputPin<uint32_t> pin(stub, 0);

    pin.toggle();
    auto code = pin.toggle();

    ASSERT_THAT(stub.get(), Eq(0U));
    ASSERT_THAT(code, Eq(outpost::hal::ReturnCode::success));
}

// ---------------------------------------------------------------------------
#if defined(OUTPOST_USE_ASSERT) && OUTPOST_USE_ASSERT == 1

TEST(SimpleOutputPinTest, shouldDetectOutOfBounds)
{
    unittest::hal::GpoStub<uint32_t> stub;
    EXPECT_DEATH(SimpleOutputPin<uint32_t> pin(stub, 32U), "Index out of range!");
}

#endif

TEST(SimpleOutputPinTest, shouldSetPinOnSecondPin)
{
    unittest::hal::GpoStub<uint32_t> stub;
    outpost::hal::SimpleOutputPin<uint32_t> pin(stub, 1);

    pin.set();

    ASSERT_THAT(stub.get(), Eq(2U));
}

TEST(SimpleOutputPinTest, shouldClearPinOnSecondPin)
{
    unittest::hal::GpoStub<uint32_t> stub;
    outpost::hal::SimpleOutputPin<uint32_t> pin(stub, 1);

    pin.set();
    pin.clear();

    ASSERT_THAT(stub.get(), Eq(0U));
}

TEST(SimpleOutputPinTest, shouldTogglePinOnSecondPin)
{
    unittest::hal::GpoStub<uint32_t> stub;
    outpost::hal::SimpleOutputPin<uint32_t> pin(stub, 1);

    pin.toggle();

    ASSERT_THAT(stub.get(), Eq(2U));
}

TEST(SimpleOutputPinTest, shouldTogglePinTwiceOnSecondPin)
{
    unittest::hal::GpoStub<uint32_t> stub;
    outpost::hal::SimpleOutputPin<uint32_t> pin(stub, 1);

    pin.toggle();
    pin.toggle();

    ASSERT_THAT(stub.get(), Eq(0U));
}

TEST(SimpleOutputPinTest, shouldStubOutputPin)
{
    unittest::hal::OutputPinStub outputPin;

    ASSERT_THAT(outputPin.getValue(), IsFalse());

    {
        auto res = outputPin.set();
        ASSERT_THAT(outputPin.getValue(), IsTrue());
        ASSERT_THAT(res, Eq(decltype(res)::success));
    }

    {
        auto res = outputPin.clear();
        ASSERT_THAT(outputPin.getValue(), IsFalse());
        ASSERT_THAT(res, Eq(decltype(res)::success));
    }

    {
        auto res = outputPin.toggle();
        ASSERT_THAT(outputPin.getValue(), IsTrue());
        ASSERT_THAT(res, Eq(decltype(res)::success));
    }

    outputPin.set(decltype(outputPin)::ReturnCode::failure);

    {
        auto res = outputPin.set();
        ASSERT_THAT(outputPin.getValue(), IsTrue());
        ASSERT_THAT(res, Eq(decltype(res)::failure));
    }

    {
        auto res = outputPin.clear();
        ASSERT_THAT(outputPin.getValue(), IsTrue());
        ASSERT_THAT(res, Eq(decltype(res)::failure));
    }

    {
        auto res = outputPin.toggle();
        ASSERT_THAT(outputPin.getValue(), IsTrue());
        ASSERT_THAT(res, Eq(decltype(res)::failure));
    }
}
