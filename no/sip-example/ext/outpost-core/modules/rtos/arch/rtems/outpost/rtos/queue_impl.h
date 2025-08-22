/*
 * Copyright (c) 2014-2017, 2019, Fabian Greif
 * Copyright (c) 2016, 2020, 2023, Jan-Gerd Mess
 * Copyright (c) 2020, Jan Malburg
 * Copyright (c) 2023, Peter Ohr
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_RTOS_RTEMS_QUEUE_IMPL_H
#define OUTPOST_RTOS_RTEMS_QUEUE_IMPL_H

#include "queue.h"
#include "rtems/interval.h"

#include <outpost/rtos/failure_handler.h>

template <typename T>
outpost::rtos::Queue<T>::Queue(size_t numberOfItems) : mId()
{
    rtems_attribute attributes = RTEMS_FIFO | RTEMS_LOCAL;

// RTEMS is C, thus, old-style-casts need to be allowed here.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"

    rtems_status_code result = rtems_message_queue_create(
            rtems_build_name('R', 'T', 'Q', '0'), numberOfItems, sizeof(T), attributes, &mId);

#pragma GCC diagnostic pop

    if (result != RTEMS_SUCCESSFUL)
    {
        FailureHandler::fatal(FailureCode::resourceAllocationFailed(Resource::messageQueue));
    }
}

template <typename T>
outpost::rtos::Queue<T>::~Queue()
{
    rtems_message_queue_delete(mId);
}

template <typename T>
bool
outpost::rtos::Queue<T>::send(const T& data)
{
    rtems_status_code result = rtems_message_queue_send(mId, &data, sizeof(T));
    bool success = (result == RTEMS_SUCCESSFUL);

    return success;
}

template <typename T>
bool
outpost::rtos::Queue<T>::receive(T& data, outpost::time::Duration timeout)
{
    size_t size;
    rtems_option options = RTEMS_WAIT;
    rtems_interval interval = rtems::getInterval(timeout);
    rtems_status_code result = rtems_message_queue_receive(mId, &data, &size, options, interval);
    bool success = (result == RTEMS_SUCCESSFUL);

    return success;
}

template <typename T>
void
outpost::rtos::Queue<T>::clear()
{
    uint32_t clearedItems = 0;
    rtems_message_queue_flush(mId, &clearedItems);
}

template <typename T>
size_t
outpost::rtos::Queue<T>::getNumberOfPendingMessages()
{
    uint32_t count = 0;
    rtems_message_queue_get_number_pending(mId, &count);
    return count;
}

#endif
