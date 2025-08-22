/*
 * Copyright (c) 2020, Jan Malburg
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "barrier.h"

#include <outpost/rtos/failure_handler.h>

using namespace outpost::posix;

// ----------------------------------------------------------------------------
Barrier::Barrier(uint32_t numberOfThreads)
{
    pthread_barrier_init(&mBarrier, nullptr, numberOfThreads);
}

Barrier::~Barrier()
{
    pthread_barrier_destroy(&mBarrier);
}

void
Barrier::wait()
{
    pthread_barrier_wait(&mBarrier);
}
