/*
 * Copyright (c) 2023-2024, Janosch Reinking
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

#include <outpost/base/testing_assert.h>

using namespace outpost::hal;

// ---------------------------------------------------------------------------
InputPin::InputPin() : mCustomData(0U), mInitialized(false)
{
}

InputPin::InputPin(CustomData customData) : mCustomData(customData), mInitialized(false)
{
}

void
InputPin::initialized()
{
    mInitialized = true;
}

void
InputPin::registerInterruptHandler(InterruptHandler& handler)
{
    OUTPOST_ASSERT(!mInitialized, "Can not add an interrupt handler during operation!");
    if (!mInitialized)
    {
        mHandlers.append(&handler);
    }
    else
    {
        /// just ignore it since the \c InterruptDistribution is already active
        /// the list cannot be adjusted anymore since it is read-only now.
    }
}

outpost::Expected<bool, ReturnCode>
InputPin::read() const
{
    return outpost::unexpected<ReturnCode>(ReturnCode::failure);
}

InputPin::CustomData
InputPin::getCustomData() const
{
    return mCustomData;
}
