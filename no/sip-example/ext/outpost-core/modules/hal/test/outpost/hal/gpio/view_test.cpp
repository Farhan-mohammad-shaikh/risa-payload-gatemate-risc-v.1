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

#include "outpost/hal/gpio/view.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <unittest/hal/can_bus_stub.h>
#include <unittest/hal/gpio_stub.h>
#include <unittest/harness.h>

using namespace outpost::hal;
using namespace ::testing;

// ---------------------------------------------------------------------------
TEST(GpiViewTest, shouldReturnPin)
{
    unittest::hal::GpiStub<uint8_t> stub;
    SimpleInputPin<uint8_t> pin1(stub, 0);
    SimpleInputPin<uint8_t> pin2(stub, 1);
    SimpleInputPin<uint8_t> pin3(stub, 2);
    outpost::hal::GpiView<3> view(pin1, pin2, pin3);

    auto& getPinResult0 = view.getPin(0U);
    auto& getPinResult1 = view.getPin(1U);
    auto& getPinResult2 = view.getPin(2U);

    auto& pinViaIndex0 = view.getPin<0>();
    auto& pinViaIndex1 = view.getPin<1>();
    auto& pinViaIndex2 = view.getPin<2>();

    ASSERT_THAT(&getPinResult0, Eq(&pin1));
    ASSERT_THAT(&getPinResult1, Eq(&pin2));
    ASSERT_THAT(&getPinResult2, Eq(&pin3));
    ASSERT_THAT(&pinViaIndex0, Eq(&pin1));
    ASSERT_THAT(&pinViaIndex1, Eq(&pin2));
    ASSERT_THAT(&pinViaIndex2, Eq(&pin3));
}

TEST(GpiViewTest, shouldCreatePortDirectlyFromPins)
{
    unittest::hal::GpiStub<uint8_t> stub;
    SimpleInputPin<uint8_t> pin1(stub, 0);
    SimpleInputPin<uint8_t> pin2(stub, 1);
    SimpleInputPin<uint8_t> pin3(stub, 2);
    outpost::hal::GpiView<3> view(pin1, pin2, pin3);

    auto& getPinResult0 = view.getPin(0U);
    auto& getPinResult1 = view.getPin(1U);
    auto& getPinResult2 = view.getPin(2U);

    ASSERT_THAT(&getPinResult0, Eq(&pin1));
    ASSERT_THAT(&getPinResult1, Eq(&pin2));
    ASSERT_THAT(&getPinResult2, Eq(&pin3));
}

TEST(GpiViewTest, shouldNotAnyPin)
{
    unittest::hal::GpiStub<uint8_t> stub;
    SimpleInputPin<uint8_t> pin1(stub, 0);
    SimpleInputPin<uint8_t> pin2(stub, 1);
    SimpleInputPin<uint8_t> pin3(stub, 2);

    outpost::hal::GpiView<3> view(pin1, pin2, pin3);

    auto value = *view.read();

    ASSERT_THAT(value, Eq(0U));
}

TEST(GpiViewTest, shouldReadFirstPin)
{
    unittest::hal::GpiStub<uint8_t> stub;
    SimpleInputPin<uint8_t> pin1(stub, 0);
    SimpleInputPin<uint8_t> pin2(stub, 1);
    SimpleInputPin<uint8_t> pin3(stub, 2);

    outpost::hal::GpiView<3> view(pin1, pin2, pin3);

    stub.set(0x01);
    auto value = *view.read();

    ASSERT_THAT(value, Eq(0x01));
}

TEST(GpiViewTest, shouldReadSecondPin)
{
    unittest::hal::GpiStub<uint8_t> stub;
    SimpleInputPin<uint8_t> pin1(stub, 0);
    SimpleInputPin<uint8_t> pin2(stub, 1);
    SimpleInputPin<uint8_t> pin3(stub, 2);

    outpost::hal::GpiView<3> view(pin1, pin2, pin3);

    stub.set(0x02);
    auto value = *view.read();

    ASSERT_THAT(value, Eq(0x02));
}

TEST(GpiViewTest, shouldReadThirdPin)
{
    unittest::hal::GpiStub<uint8_t> stub;
    SimpleInputPin<uint8_t> pin1(stub, 0);
    SimpleInputPin<uint8_t> pin2(stub, 1);
    SimpleInputPin<uint8_t> pin3(stub, 2);

    outpost::hal::GpiView<3> view(pin1, pin2, pin3);

    stub.set(0x04);  // 3
    auto value = *view.read();

    ASSERT_THAT(value, Eq(0x04));
}

TEST(GpiViewTest, shouldNotReadFourthPin)
{
    unittest::hal::GpiStub<uint8_t> stub;
    SimpleInputPin<uint8_t> pin1(stub, 0);
    SimpleInputPin<uint8_t> pin2(stub, 1);
    SimpleInputPin<uint8_t> pin3(stub, 2);

    outpost::hal::GpiView<3> view(pin1, pin2, pin3);

    stub.set(0x08);
    auto value = *view.read();

    ASSERT_THAT(value, Eq(0x00));
}

TEST(GpiViewTest, shouldReversePins)
{
    unittest::hal::GpiStub<uint8_t> stub;
    SimpleInputPin<uint8_t> pin1(stub, 0);
    SimpleInputPin<uint8_t> pin2(stub, 1);
    SimpleInputPin<uint8_t> pin3(stub, 2);

    outpost::hal::GpiView<3> view(pin3, pin2, pin1);

    stub.set(0x01);
    auto value = *view.read();

    ASSERT_THAT(value, Eq(0x04));
}

// ---------------------------------------------------------------------------
TEST(SimpleAtomicGpiViewTest, shouldMapPinsOnRead1)
{
    unittest::hal::GpiStub<uint16_t> stub;
    SimpleInputPin<uint16_t> pin1(stub, 0);
    SimpleInputPin<uint16_t> pin2(stub, 1);
    SimpleInputPin<uint16_t> pin3(stub, 3);

    outpost::hal::SimpleAtomicGpiView<uint16_t, 3> view(stub, pin1, pin2, pin3);

    stub.set(4U + 2U);

    auto value = *view.read();

    ASSERT_THAT(value, Eq(2U));
}

TEST(SimpleAtomicGpiViewTest, shouldMapPinsOnRead2)
{
    unittest::hal::GpiStub<uint16_t> stub;
    SimpleInputPin<uint16_t> pin1(stub, 0);
    SimpleInputPin<uint16_t> pin2(stub, 1);
    SimpleInputPin<uint16_t> pin3(stub, 3);

    outpost::hal::SimpleAtomicGpiView<uint16_t, 3> view(stub, pin1, pin2, pin3);

    stub.set(8U + 4U + 2U);

    auto value = *view.read();

    ASSERT_THAT(value, Eq(4U + 2U));
}

TEST(SimpleAtomicGpiViewTest, shouldMapPinsOnRead3)
{
    unittest::hal::GpiStub<uint16_t> stub;
    SimpleInputPin<uint16_t> pin1(stub, 0);
    SimpleInputPin<uint16_t> pin2(stub, 1);
    SimpleInputPin<uint16_t> pin3(stub, 3);

    outpost::hal::SimpleAtomicGpiView<uint16_t, 3> view(stub, pin1, pin2, pin3);

    stub.set(8U + 4U + 2U);

    auto value = *view.read();

    ASSERT_THAT(value, Eq(4U + 2U));
}

TEST(SimpleAtomicGpiViewTest, shouldExcludePinsThatAreNotPartOfTheView)
{
    unittest::hal::GpiStub<uint16_t> stub;
    SimpleInputPin<uint16_t> pin1(stub, 0);
    SimpleInputPin<uint16_t> pin2(stub, 1);
    SimpleInputPin<uint16_t> pin3(stub, 3);

    outpost::hal::SimpleAtomicGpiView<uint16_t, 3> view(stub, pin1, pin2, pin3);

    // all pins are set ...
    stub.set(0xFFFF);

    auto value = *view.read();

    // ... but only the pins that are in the view should be set
    ASSERT_THAT(value, Eq(4U + 2U + 1U));
}

// ---------------------------------------------------------------------------
TEST(GpoViewTest, shouldSetAllPins)
{
    unittest::hal::GpoStub<uint8_t> stub;
    SimpleOutputPin<uint8_t> pin1(stub, 0);
    SimpleOutputPin<uint8_t> pin2(stub, 1);
    SimpleOutputPin<uint8_t> pin3(stub, 2);

    outpost::hal::GpoView<3> view(pin1, pin2, pin3);

    view.set();

    ASSERT_THAT(stub.get(), Eq(0x07));
}

TEST(GpoViewTest, shouldCreatePortDirectlyFromPins)
{
    unittest::hal::GpoStub<uint8_t> stub;
    SimpleOutputPin<uint8_t> pin1(stub, 0);
    SimpleOutputPin<uint8_t> pin2(stub, 1);
    SimpleOutputPin<uint8_t> pin3(stub, 2);

    outpost::hal::GpoView<3> view(pin1, pin2, pin3);

    auto& getPinResult0 = view.getPin(0U);
    auto& getPinResult1 = view.getPin(1U);
    auto& getPinResult2 = view.getPin(2U);

    ASSERT_THAT(&getPinResult0, Eq(&pin1));
    ASSERT_THAT(&getPinResult1, Eq(&pin2));
    ASSERT_THAT(&getPinResult2, Eq(&pin3));
}

TEST(GpoViewTest, shouldClearAllPins)
{
    unittest::hal::GpoStub<uint8_t> stub;
    SimpleOutputPin<uint8_t> pin1(stub, 0);
    SimpleOutputPin<uint8_t> pin2(stub, 1);
    SimpleOutputPin<uint8_t> pin3(stub, 2);

    outpost::hal::GpoView<3> view(pin1, pin2, pin3);

    view.set();
    view.clear();

    ASSERT_THAT(stub.get(), Eq(0x00));
}

TEST(GpoViewTest, shouldToggleAllPins)
{
    unittest::hal::GpoStub<uint8_t> stub;
    SimpleOutputPin<uint8_t> pin1(stub, 0);
    SimpleOutputPin<uint8_t> pin2(stub, 1);
    SimpleOutputPin<uint8_t> pin3(stub, 2);

    outpost::hal::GpoView<3> view(pin1, pin2, pin3);
    pin1.set();

    view.toggle();

    ASSERT_THAT(stub.get(), Eq(6U));
}

TEST(GpoViewTest, shouldReversePins)
{
    unittest::hal::GpoStub<uint8_t> stub;
    SimpleOutputPin<uint8_t> pin1(stub, 0);
    SimpleOutputPin<uint8_t> pin2(stub, 1);
    SimpleOutputPin<uint8_t> pin3(stub, 2);

    outpost::hal::GpoView<3> view(pin3, pin2, pin1);

    view.write(0x03);

    ASSERT_THAT(stub.get(), Eq(0x06));
}

TEST(GpoViewTest, shouldReturnPin)
{
    unittest::hal::GpoStub<uint8_t> stub;
    SimpleOutputPin<uint8_t> pin1(stub, 0);
    SimpleOutputPin<uint8_t> pin2(stub, 1);
    SimpleOutputPin<uint8_t> pin3(stub, 3);

    outpost::hal::GpoView<3> view(pin1, pin2, pin3);

    auto& resPin1 = view.getPin<0>();
    auto& resPin2 = view.getPin<1>();
    auto& resPin3 = view.getPin<2>();

    ASSERT_THAT(&resPin1, Eq(&pin1));
    ASSERT_THAT(&resPin2, Eq(&pin2));
    ASSERT_THAT(&resPin3, Eq(&pin3));
}

// ---------------------------------------------------------------------------
TEST(SimpleAtomicGpoViewTest, shouldForwardErrorCodeOnToggle)
{
    unittest::hal::GpoStub<uint16_t> stub;
    SimpleOutputPin<uint16_t> pin1(stub, 0);
    SimpleOutputPin<uint16_t> pin2(stub, 1);
    SimpleOutputPin<uint16_t> pin3(stub, 3);
    outpost::hal::SimpleAtomicGpoView<uint16_t, 3> view(stub, pin1, pin2, pin3);

    stub.mToggle = [](uint16_t) { return outpost::hal::ReturnCode::failure; };

    auto ret = view.toggle();

    ASSERT_THAT(ret, Eq(outpost::hal::ReturnCode::failure));
}

TEST(SimpleAtomicGpoViewTest, shouldForwardErrorCodeOnSet)
{
    unittest::hal::GpoStub<uint16_t> stub;
    SimpleOutputPin<uint16_t> pin1(stub, 0);
    SimpleOutputPin<uint16_t> pin2(stub, 1);
    SimpleOutputPin<uint16_t> pin3(stub, 3);
    outpost::hal::SimpleAtomicGpoView<uint16_t, 3> view(stub, pin1, pin2, pin3);

    stub.mSet = [](uint16_t) { return outpost::hal::ReturnCode::failure; };

    auto ret = view.set();

    ASSERT_THAT(ret, Eq(outpost::hal::ReturnCode::failure));
}

TEST(SimpleAtomicGpoViewTest, shouldForwardErrorCodeOnClear)
{
    unittest::hal::GpoStub<uint16_t> stub;
    SimpleOutputPin<uint16_t> pin1(stub, 0);
    SimpleOutputPin<uint16_t> pin2(stub, 1);
    SimpleOutputPin<uint16_t> pin3(stub, 3);
    outpost::hal::SimpleAtomicGpoView<uint16_t, 3> view(stub, pin1, pin2, pin3);

    stub.mClear = [](uint16_t) { return outpost::hal::ReturnCode::failure; };

    auto ret = view.clear();

    ASSERT_THAT(ret, Eq(outpost::hal::ReturnCode::failure));
}

TEST(SimpleAtomicGpoViewTest, shouldForwardErrorCodeOnWrite)
{
    unittest::hal::GpoStub<uint16_t> stub;
    SimpleOutputPin<uint16_t> pin1(stub, 0);
    SimpleOutputPin<uint16_t> pin2(stub, 1);
    SimpleOutputPin<uint16_t> pin3(stub, 3);
    outpost::hal::SimpleAtomicGpoView<uint16_t, 3> view(stub, pin1, pin2, pin3);

    stub.mWrite = [](uint16_t, uint16_t) { return outpost::hal::ReturnCode::failure; };

    auto ret = view.write(0x0);

    ASSERT_THAT(ret, Eq(outpost::hal::ReturnCode::failure));
}

// ---------------------------------------------------------------------------
TEST(SimpleAtomicGpoViewTest, shouldTogglePins)
{
    unittest::hal::GpoStub<uint16_t> stub;
    SimpleOutputPin<uint16_t> pin1(stub, 0);
    SimpleOutputPin<uint16_t> pin2(stub, 1);
    SimpleOutputPin<uint16_t> pin3(stub, 3);
    outpost::hal::SimpleAtomicGpoView<uint16_t, 3> view(stub, pin1, pin2, pin3);
    stub.set(4U + 1U);  // 0101
    view.toggle();

    // 1110
    ASSERT_THAT(stub.get(), 8U + 4U + 2U /* + 1U */);
}

TEST(SimpleAtomicGpoViewTest, shouldClearPins)
{
    unittest::hal::GpoStub<uint16_t> stub;
    SimpleOutputPin<uint16_t> pin1(stub, 0);
    SimpleOutputPin<uint16_t> pin2(stub, 1);
    SimpleOutputPin<uint16_t> pin3(stub, 3);
    outpost::hal::SimpleAtomicGpoView<uint16_t, 3> view(stub, pin1, pin2, pin3);
    stub.set(8U + 1U);  // 1001

    view.clear();

    // 0000
    ASSERT_THAT(stub.get(), 0U);
}

TEST(SimpleAtomicGpoViewTest, shouldMapPinsOnWrite1)
{
    unittest::hal::GpoStub<uint16_t> stub;
    SimpleOutputPin<uint16_t> pin1(stub, 0);
    SimpleOutputPin<uint16_t> pin2(stub, 1);
    SimpleOutputPin<uint16_t> pin3(stub, 3);

    outpost::hal::SimpleAtomicGpoView<uint16_t, 3> view(stub, pin1, pin2, pin3);

    view.write(4 + 2 + 1);

    ASSERT_THAT(stub.get(), 8 + 2 + 1);
}

TEST(SimpleAtomicGpoViewTest, shouldMapPinsOnWrite2)
{
    unittest::hal::GpoStub<uint16_t> stub;
    SimpleOutputPin<uint16_t> pin1(stub, 0);
    SimpleOutputPin<uint16_t> pin2(stub, 1);
    SimpleOutputPin<uint16_t> pin3(stub, 3);

    outpost::hal::SimpleAtomicGpoView<uint16_t, 3> view(stub, pin1, pin2, pin3);

    view.write(/* 4 */ 2 + 1);

    ASSERT_THAT(stub.get(), /* 8 + */ 2 + 1);
}

TEST(SimpleAtomicGpoViewTest, shouldMapPinsOnWrite3)
{
    unittest::hal::GpoStub<uint16_t> stub;
    SimpleOutputPin<uint16_t> pin1(stub, 0);
    SimpleOutputPin<uint16_t> pin2(stub, 1);
    SimpleOutputPin<uint16_t> pin3(stub, 3);

    outpost::hal::SimpleAtomicGpoView<uint16_t, 3> view(stub, pin1, pin2, pin3);
    view.set();

    view.write(/* 4 */ 2 + 1);

    ASSERT_THAT(stub.get(), /* 8 + */ 2 + 1);
}

TEST(SimpleAtomicGpoViewTest, shouldMapPinsOnWrite4)
{
    unittest::hal::GpoStub<uint16_t> stub;
    SimpleOutputPin<uint16_t> pin1(stub, 0);
    SimpleOutputPin<uint16_t> pin2(stub, 1);
    SimpleOutputPin<uint16_t> pin3(stub, 3);

    outpost::hal::SimpleAtomicGpoView<uint16_t, 3> view(stub, pin1, pin2, pin3);
    stub.set(16U);  // should not be overwritten since it not selected in the mask

    view.write(/* 4 */ 2 + 1);

    ASSERT_THAT(stub.get(), 16 + /* 8 + */ 2 + 1);
}

TEST(SimpleAtomicGpoViewTest, shouldMapPinsOnWrite5)
{
    unittest::hal::GpoStub<uint16_t> stub;
    SimpleOutputPin<uint16_t> pin1(stub, 0);
    SimpleOutputPin<uint16_t> pin2(stub, 1);
    SimpleOutputPin<uint16_t> pin3(stub, 3);

    outpost::hal::SimpleAtomicGpoView<uint16_t, 3> view(stub, pin1, pin2, pin3);
    stub.set(8U);  // should be set to zero later

    view.write(/* 4 */ 2 + 1);

    ASSERT_THAT(stub.get(), /* 8 + */ 2 + 1);
}

TEST(SimpleAtomicGpoViewTest, shouldMapPinsOnWrite6)
{
    unittest::hal::GpoStub<uint16_t> stub;
    SimpleOutputPin<uint16_t> pin1(stub, 0);
    SimpleOutputPin<uint16_t> pin2(stub, 1);
    SimpleOutputPin<uint16_t> pin3(stub, 3);

    outpost::hal::SimpleAtomicGpoView<uint16_t, 3> view(stub, pin1, pin2, pin3);

    stub.set(8U);  // should be set to zero later

    view.write(/* 4 */ 2 + 1);

    ASSERT_THAT(stub.get(), /* 8 + */ 2 + 1);
}

// ---------------------------------------------------------------------------
TEST(ViewTest, typeCheck)
{
    static_assert(std::is_same<outpost::hal::BankTypeOf<1U>, uint8_t>::value, "");
    static_assert(std::is_same<outpost::hal::BankTypeOf<8U>, uint8_t>::value, "");
    static_assert(std::is_same<outpost::hal::BankTypeOf<9U>, uint16_t>::value, "");
    static_assert(std::is_same<outpost::hal::BankTypeOf<16U>, uint16_t>::value, "");
    static_assert(std::is_same<outpost::hal::BankTypeOf<17U>, uint32_t>::value, "");
    static_assert(std::is_same<outpost::hal::BankTypeOf<32U>, uint32_t>::value, "");
    static_assert(std::is_same<outpost::hal::BankTypeOf<33U>, uint64_t>::value, "");
    static_assert(std::is_same<outpost::hal::BankTypeOf<64U>, uint64_t>::value, "");
}
