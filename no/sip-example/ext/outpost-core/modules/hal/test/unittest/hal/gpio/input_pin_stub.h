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

#ifndef UNITTEST_HAL_GPIO_INPUT_PIN_STUB_H
#define UNITTEST_HAL_GPIO_INPUT_PIN_STUB_H

#include <outpost/hal/gpio/input_pin.h>

namespace unittest
{
namespace hal
{

/**
 * \class InputPinStub
 */
class InputPinStub : public outpost::hal::InputPin
{
public:
    explicit InputPinStub() : mValue(false), mCode(outpost::hal::ReturnCode::success)
    {
    }

    InputPinStub(const InputPinStub&) = delete;

    InputPinStub&
    operator=(const InputPinStub&) = delete;

    ~InputPinStub() = default;

    outpost::Expected<bool, outpost::hal::ReturnCode>
    read() const override;

    void
    set(bool value);

    void
    set(outpost::hal::ReturnCode code);

private:
    bool mValue;
    outpost::hal::ReturnCode mCode;
};

}  // namespace hal
}  // namespace unittest

#endif  // UNITTEST_HAL_GPIO_INPUT_PIN_STUB_H
