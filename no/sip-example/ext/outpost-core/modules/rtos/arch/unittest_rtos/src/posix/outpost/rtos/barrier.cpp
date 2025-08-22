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

#include "outpost/rtos/barrier.h"

#include "outpost/posix/barrier.h"

// ----------------------------------------------------------------------------
outpost::rtos::Barrier::Barrier(uint32_t numberOfThreads) : mImplementation(nullptr)
{
    auto* barrier = new outpost::posix::Barrier(numberOfThreads);
    mImplementation = barrier;
}

outpost::rtos::Barrier::~Barrier()
{
    auto* barrier = reinterpret_cast<outpost::posix::Barrier*>(mImplementation);
    delete barrier;
    mImplementation = nullptr;
}

void
outpost::rtos::Barrier::wait()
{
    auto* barrier = reinterpret_cast<outpost::posix::Barrier*>(mImplementation);
    return barrier->wait();
}
