/*
 * Copyright (c) 2013-2017, Fabian Greif
 * Copyright (c) 2013, Norbert Toth
 * Copyright (c) 2020, Jan Malburg
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

#include "semaphore.h"

#include <outpost/rtos/failure_handler.h>

// ----------------------------------------------------------------------------
outpost::rtos::Semaphore::Semaphore(uint32_t count)
{
    mHandle = xSemaphoreCreateCounting(static_cast<unsigned portBASE_TYPE>(-1), count);
    if (mHandle == 0)
    {
        FailureHandler::fatal(FailureCode::resourceAllocationFailed(Resource::semaphore));
    }
}

outpost::rtos::Semaphore::~Semaphore()
{
    // As semaphores are based on queues we use the queue functions to delete
    // the semaphore
    vQueueDelete(mHandle);
}

bool
outpost::rtos::Semaphore::acquire()
{
    // wait forever
    return (xSemaphoreTake(mHandle, portMAX_DELAY) == pdTRUE);
}

bool
outpost::rtos::Semaphore::acquire(time::Duration timeout)
{
    if (timeout >= outpost::time::Duration::myriad())
    {
        return acquire();
    }
    const TickType_t ticks = (timeout.milliseconds() * configTICK_RATE_HZ) / 1000;
    return (xSemaphoreTake(mHandle, ticks) == pdTRUE);
}

bool
outpost::rtos::Semaphore::acquireFromISR(bool& hasWokenTask)
{
    // wait forever
    BaseType_t baseType = 0;
    bool res = (xSemaphoreTakeFromISR(this->mHandle, &baseType) == pdTRUE);

    hasWokenTask = (baseType != 0);
    return res;
}

void
outpost::rtos::Semaphore::release()
{
    xSemaphoreGive(mHandle);
}

void
outpost::rtos::Semaphore::releaseFromISR(bool& hasWokenTask)
{
    BaseType_t baseType = 0;
    xSemaphoreGiveFromISR(mHandle, &baseType);
    hasWokenTask = (baseType != 0);
}

// ----------------------------------------------------------------------------
outpost::rtos::BinarySemaphore::BinarySemaphore()
{
    vSemaphoreCreateBinary(mHandle);

    if (mHandle == 0)
    {
        FailureHandler::fatal(FailureCode::resourceAllocationFailed(Resource::semaphore));
    }
}

outpost::rtos::BinarySemaphore::BinarySemaphore(State::Type initial)
{
    vSemaphoreCreateBinary(mHandle);

    if (mHandle == 0)
    {
        FailureHandler::fatal(FailureCode::resourceAllocationFailed(Resource::semaphore));
    }

    if (initial == State::acquired)
    {
        acquire();
    }
}

outpost::rtos::BinarySemaphore::~BinarySemaphore()
{
    // As semaphores are based on queues we use the queue functions to delete
    // the semaphore
    vQueueDelete(mHandle);
}

bool
outpost::rtos::BinarySemaphore::acquire()
{
    // wait forever
    return (xSemaphoreTake(this->mHandle, portMAX_DELAY) == pdTRUE);
}

bool
outpost::rtos::BinarySemaphore::acquire(time::Duration timeout)
{
    const TickType_t ticks = (timeout.milliseconds() * configTICK_RATE_HZ) / 1000;
    return (xSemaphoreTake(this->mHandle, ticks) == pdTRUE);
}

bool
outpost::rtos::BinarySemaphore::acquireFromISR(bool& hasWokenTask)
{
    // wait forever
    BaseType_t baseType = 0;
    bool res = (xSemaphoreTakeFromISR(this->mHandle, &baseType) == pdTRUE);
    hasWokenTask = (baseType != 0);
    return res;
}

void
outpost::rtos::BinarySemaphore::release()
{
    xSemaphoreGive(mHandle);
}

void
outpost::rtos::BinarySemaphore::releaseFromISR(bool& hasWokenTask)
{
    BaseType_t baseType = 0;
    xSemaphoreGiveFromISR(mHandle, &baseType);
    hasWokenTask = (baseType != 0);
}
