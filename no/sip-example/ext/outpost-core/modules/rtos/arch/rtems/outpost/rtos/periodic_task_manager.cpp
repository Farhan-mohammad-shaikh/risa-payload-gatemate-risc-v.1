/*
 * Copyright (c) 2013-2017, Fabian Greif
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "periodic_task_manager.h"

using namespace outpost::rtos;

PeriodicTaskManager::PeriodicTaskManager() : mId()
{
    rtems_name name = rtems_build_name('P', 'E', 'R', 'D');
    rtems_status_code result = rtems_rate_monotonic_create(name, &mId);

    if (result != RTEMS_SUCCESSFUL)
    {
        FailureHandler::fatal(FailureCode::resourceAllocationFailed(Resource::periodicTask));
    }
}

PeriodicTaskManager::~PeriodicTaskManager()
{
    rtems_status_code result = rtems_rate_monotonic_delete(mId);
    if (result != RTEMS_SUCCESSFUL)
    {
        // TODO create different error code?
        FailureHandler::fatal(FailureCode::genericRuntimeError(Resource::periodicTask));
    }
}
