/*
 * Copyright (c) 2020, Jan Malburg
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "barrier.h"

#include <rtems.h>

#include <outpost/rtos/failure_handler.h>

// ----------------------------------------------------------------------------
outpost::rtos::Barrier::Barrier(uint32_t numberOfThreads)
{
    rtems_name name = rtems_build_name('B', 'A', 'R', '-');

    rtems_status_code status =
            rtems_barrier_create(name, RTEMS_BARRIER_AUTOMATIC_RELEASE, numberOfThreads, &mId);
    if (status != RTEMS_SUCCESSFUL)
    {
        FailureHandler::fatal(FailureCode::resourceAllocationFailed(Resource::barrier));
    }
}

outpost::rtos::Barrier::~Barrier()
{
    rtems_barrier_delete(mId);
}

void
outpost::rtos::Barrier::wait()
{
    rtems_barrier_wait(mId, RTEMS_NO_TIMEOUT);
}
