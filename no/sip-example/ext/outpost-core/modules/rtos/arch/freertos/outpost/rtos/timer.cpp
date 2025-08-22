/*
 * Copyright (c) 2013-2017, Fabian Greif
 * Copyright (c) 2013, Norbert Toth
 * Copyright (c) 2021, Felix Passenberg
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2023, Hannah Kirstein
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "timer.h"

#include <outpost/rtos/failure_handler.h>

// ----------------------------------------------------------------------------
outpost::rtos::Timer::~Timer()
{
    if (xTimerDelete(mHandle, portMAX_DELAY) != pdPASS)
    {
        FailureHandler::fatal(FailureCode::resourceAllocationFailed(Resource::timer));
    }
}

// ----------------------------------------------------------------------------
void
outpost::rtos::Timer::start(time::Duration duration)
{
    if ((xTimerChangePeriod(mHandle,
                            duration.milliseconds() > 1000 * 1000
                                    ?  // prevent overflows for large delays, and underflows for
                                       // short delays
                                    (duration.milliseconds() * configTICK_RATE_HZ) / 1000
                                    : (duration.milliseconds() / 1000) * configTICK_RATE_HZ,
                            portMAX_DELAY)
         != pdPASS)
        || (xTimerStart(mHandle, portMAX_DELAY) != pdPASS))
    {
        FailureHandler::fatal(FailureCode::genericRuntimeError(Resource::timer));
    }
}

void
outpost::rtos::Timer::reset()
{
    if (xTimerReset(mHandle, portMAX_DELAY) != pdPASS)
    {
        FailureHandler::fatal(FailureCode::genericRuntimeError(Resource::timer));
    }
}

void
outpost::rtos::Timer::cancel()
{
    if (xTimerStop(mHandle, portMAX_DELAY) != pdPASS)
    {
        FailureHandler::fatal(FailureCode::genericRuntimeError(Resource::timer));
    }
}

bool
outpost::rtos::Timer::isRunning()
{
    bool active = false;
    if (xTimerIsTimerActive(mHandle) != pdFALSE)
    {
        active = true;
    }

    return active;
}

// ----------------------------------------------------------------------------
void
outpost::rtos::Timer::startTimerDaemonThread(uint8_t priority, size_t stack, CpuMask /*affinity*/)
{
    (void) priority;
    (void) stack;
}

// ----------------------------------------------------------------------------
void
outpost::rtos::Timer::createTimer(const char* name)
{
    mHandle = xTimerCreate(
            name,
            1,        // dummy value (must be >= 0 but will be changed when starting the timer)
            pdFALSE,  // no auto-reload
            (void*) this,
            &Timer::invokeTimer);

    if (mHandle == 0)
    {
        FailureHandler::fatal(FailureCode::resourceAllocationFailed(Resource::timer));
    }
}

// ----------------------------------------------------------------------------
void
outpost::rtos::Timer::invokeTimer(TimerHandle_t handle)
{
    Timer* timer = reinterpret_cast<Timer*>(pvTimerGetTimerID(handle));
    (timer->mObject->*(timer->mFunction))(timer);
}
