/*
 * Copyright (c) 2013-2017, Fabian Greif
 * Copyright (c) 2021, Felix Passenberg
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2022, Jan-Gerd Mess
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "periodic_task_manager.h"

#include "traits.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <outpost/rtos/mutex.h>
#include <outpost/rtos/mutex_guard.h>

using namespace outpost::rtos;

PeriodicTaskManager::PeriodicTaskManager() :
    mMutex(), mTimerRunning(false), mLastWakeTime(), mCurrentPeriod()
{
}

PeriodicTaskManager::~PeriodicTaskManager()
{
}

PeriodicTaskManager::Status::Type
PeriodicTaskManager::nextPeriod(time::Duration period)
{
    MutexGuard lock(mMutex);
    Status::Type currentStatus = Status::running;

    const TickType_t nextPeriodTicks =
            period.milliseconds() < 1000 * 1000
                    ?  // prevent overflows by ordering dependent of amount.
                    (period.milliseconds() * configTICK_RATE_HZ) / 1000
                    : (period.milliseconds() / 1000) * configTICK_RATE_HZ;
    if (mTimerRunning)
    {
        if (static_cast<Traits<TickType_t>::SignedType>(xTaskGetTickCount() - mLastWakeTime)
            > static_cast<Traits<TickType_t>::SignedType>(mCurrentPeriod))
        {
            currentStatus = Status::timeout;
        }

        vTaskDelayUntil(&mLastWakeTime, mCurrentPeriod);
    }
    else
    {
        // period is started now, no need to wait
        mLastWakeTime = xTaskGetTickCount();
        mTimerRunning = true;
    }

    mCurrentPeriod = nextPeriodTicks;
    return currentStatus;
}

PeriodicTaskManager::Status::Type
PeriodicTaskManager::status()
{
    MutexGuard lock(mMutex);

    if (!mTimerRunning)
    {
        return Status::idle;
    }
    else if (static_cast<Traits<TickType_t>::SignedType>(xTaskGetTickCount() - mLastWakeTime)
             > static_cast<Traits<TickType_t>::SignedType>(mCurrentPeriod))
    {
        return Status::timeout;
    }
    else
    {
        return Status::running;
    }
}

void
PeriodicTaskManager::cancel()
{
    MutexGuard lock(mMutex);
    mTimerRunning = false;
}
