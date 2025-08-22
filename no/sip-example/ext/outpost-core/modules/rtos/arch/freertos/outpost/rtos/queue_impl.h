/*
 * Copyright (c) 2014-2017, Fabian Greif
 * Copyright (c) 2021, Felix Passenberg
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2022-2023, Jan-Gerd Mess
 * Copyright (c) 2023, Peter Ohr
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_RTOS_FREERTOS_QUEUE_IMPL_H
#define OUTPOST_RTOS_FREERTOS_QUEUE_IMPL_H

#include "queue.h"

#include <outpost/rtos/failure_handler.h>

template <typename T>
outpost::rtos::Queue<T>::Queue(size_t numberOfItems)
{
    mHandle = xQueueCreate(numberOfItems, sizeof(T));

    if (mHandle == 0)
    {
        FailureHandler::fatal(FailureCode::resourceAllocationFailed(Resource::messageQueue));
    }
}

template <typename T>
outpost::rtos::Queue<T>::~Queue()
{
    vQueueDelete(static_cast<QueueHandle_t>(mHandle));
}

template <typename T>
bool
outpost::rtos::Queue<T>::send(const T& data)
{
    const TickType_t ticks = 0;
    return xQueueSend(mHandle, &data, ticks);
}

template <typename T>
bool
outpost::rtos::Queue<T>::sendFromISR(const T& data, bool& hasWokenTask)
{
    BaseType_t baseType = 0;
    bool res = xQueueSendFromISR(static_cast<QueueHandle_t>(mHandle), &data, &baseType);
    hasWokenTask = (baseType == 0);
    return res;
}

template <typename T>
bool
outpost::rtos::Queue<T>::receive(T& data, outpost::time::Duration timeout)
{
    const TickType_t ticks = timeout.milliseconds() > 1000 * 1000
                                     ? (timeout.milliseconds() * configTICK_RATE_HZ) / 1000
                                     : (timeout.milliseconds() / 1000) * configTICK_RATE_HZ;
    return xQueueReceive(static_cast<QueueHandle_t>(mHandle), &data, ticks);
}

template <typename T>
bool
outpost::rtos::Queue<T>::receiveFromISR(T& data, bool& hasWokenTask)
{
    BaseType_t baseType = 0;
    bool res = xQueueReceiveFromISR(static_cast<QueueHandle_t>(mHandle), &data, &baseType);
    hasWokenTask = (baseType != 0);
    return res;
}

template <typename T>
void
outpost::rtos::Queue<T>::clear()
{
    xQueueReset(static_cast<QueueHandle_t>(mHandle));
}

template <typename T>
size_t
outpost::rtos::Queue<T>::getNumberOfPendingMessages()
{
    return uxQueueMessagesWaiting(static_cast<QueueHandle_t>(mHandle));
}

#endif
