/*
 * Copyright (c) 2014-2017, 2019, Fabian Greif
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

#include "periodic_task_manager.h"

#include "internal/time.h"

#include <outpost/rtos/failure_handler.h>
#include <outpost/rtos/mutex_guard.h>

#include <errno.h>

using namespace outpost::posix;

PeriodicTaskManager::PeriodicTaskManager() : mMutex(), mTimerRunning(false), mNextWakeTime()
{
}

PeriodicTaskManager::Status::Type
PeriodicTaskManager::nextPeriod(time::Duration period)
{
    rtos::MutexGuard lock(mMutex);
    Status::Type currentStatus = Status::running;

    if (mTimerRunning)
    {
        timespec currentTime = rtos::getTime(CLOCK_MONOTONIC);

        // Check if the time is in the current period
        if (rtos::isBigger(currentTime, mNextWakeTime))
        {
            currentStatus = Status::timeout;
        }
        else
        {
            rtos::sleepUntilAbsoluteTime(CLOCK_MONOTONIC, mNextWakeTime);
        }
    }
    else
    {
        // period is started now, no need to wait
        mNextWakeTime = rtos::getTime(CLOCK_MONOTONIC);
        mTimerRunning = true;
    }

    // calculate the next wake-up time
    timespec relative = rtos::toRelativeTime(period);
    rtos::addTime(mNextWakeTime, relative);

    return currentStatus;
}

PeriodicTaskManager::Status::Type
PeriodicTaskManager::status()
{
    rtos::MutexGuard lock(mMutex);
    if (!mTimerRunning)
    {
        return Status::idle;
    }
    else
    {
        timespec currentTime = rtos::getTime(CLOCK_MONOTONIC);

        // Check if the time is in the current period
        if (rtos::isBigger(currentTime, mNextWakeTime))
        {
            return Status::timeout;
        }
        else
        {
            return Status::running;
        }
    }
}

void
PeriodicTaskManager::cancel()
{
    rtos::MutexGuard lock(mMutex);
    mTimerRunning = false;
}
