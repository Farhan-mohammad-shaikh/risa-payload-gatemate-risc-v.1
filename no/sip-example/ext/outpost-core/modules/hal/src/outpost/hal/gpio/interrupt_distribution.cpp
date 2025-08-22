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

#include "outpost/hal/gpio/interrupt_distribution.h"

using namespace outpost::hal;

// ---------------------------------------------------------------------------
InterruptDistribution::InterruptDistribution() : mInitialized(false)
{
}

void
InterruptDistribution::initialize()
{
    mInitialized = true;
    auto iter = mHandlers.begin();
    while (iter != mHandlers.end())
    {
        iter->initialized();
        ++iter;
    }
}

void
InterruptDistribution::registerPin(InputPin& pin)
{
    OUTPOST_ASSERT(!mInitialized, "Can not add an interrupt handler during operation!");
    if (!mInitialized)
    {
        mHandlers.append(&pin);
    }
}

void
InterruptDistribution::callHandlers(InterruptType type, uint8_t index)
{
    auto iter = mHandlers.begin();
    while (iter != mHandlers.end())
    {
        if (index == iter->getCustomData())
        {
            iter->callHandlers(type);
        }
        ++iter;
    }
}
