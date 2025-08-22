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

#include "unittest/hal/gpio/input_pin_stub.h"

using namespace unittest::hal;

// ---------------------------------------------------------------------------
outpost::Expected<bool, outpost::hal::ReturnCode>
InputPinStub::read() const
{
    if (mCode != outpost::hal::ReturnCode::success)
    {
        return outpost::unexpected(mCode);
    }
    else
    {
        return mValue;
    }
}

void
InputPinStub::set(bool value)
{
    mValue = value;
    mCode = outpost::hal::ReturnCode::success;
}

void
InputPinStub::set(outpost::hal::ReturnCode code)
{
    mCode = code;
}
