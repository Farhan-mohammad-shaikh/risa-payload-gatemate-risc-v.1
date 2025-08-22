/*
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "unittest/hal/gpio/output_pin_stub.h"

using namespace unittest::hal;

// ---------------------------------------------------------------------------
OutputPinStub::OutputPinStub() : mValue(false), mCode(ReturnCode::success)
{
}

bool
OutputPinStub::getValue() const
{
    return mValue;
}

OutputPinStub::ReturnCode
OutputPinStub::toggle()
{
    if (mCode == ReturnCode::success)
    {
        mValue = !mValue;
        return ReturnCode::success;
    }
    else
    {
        return mCode;
    }
}

OutputPinStub::ReturnCode
OutputPinStub::set()
{
    if (mCode == ReturnCode::success)
    {
        mValue = true;
        return ReturnCode::success;
    }
    else
    {
        return mCode;
    }
}

OutputPinStub::ReturnCode
OutputPinStub::clear()
{
    if (mCode == ReturnCode::success)
    {
        mValue = false;
        return ReturnCode::success;
    }
    else
    {
        return mCode;
    }
}

void
OutputPinStub::set(OutputPinStub::ReturnCode code)
{
    mCode = code;
}
