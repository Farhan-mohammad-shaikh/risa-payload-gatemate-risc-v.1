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

#include "unittest/rtos/barrier_mock.h"

using namespace outpost::posix;
using namespace unittest::rtos;

// ---------------------------------------------------------------------------
BarrierImplementation::BarrierImplementation(outpost::rtos::Barrier* barrier,
                                             uint32_t numberOfThreads) :
    Mockable<BarrierMock, outpost::posix::Barrier>(
            this, reinterpret_cast<intptr_t>(barrier), &mBarrier),
    mBarrier(numberOfThreads)
{
}

BarrierImplementation::~BarrierImplementation()
{
}

// ---------------------------------------------------------------------------
void
BarrierImplementation::wait()
{
    std::function<void()> mockFunction = std::bind(&BarrierMock::wait, mMock);
    std::function<void()> realFunction = std::bind(&Barrier::wait, &mBarrier);
    return handleMethodCall(mMock, mockFunction, realFunction);
}
