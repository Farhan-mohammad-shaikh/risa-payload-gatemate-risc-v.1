/*
 * Copyright (c) 2014-2017, 2019, Fabian Greif
 * Copyright (c) 2015, Norbert Toth
 * Copyright (c) 2020-2021, Jan Malburg
 * Copyright (c) 2021, pfef_to
 * Copyright (c) 2022-2023, Jan-Gerd Mess
 * Copyright (c) 2023, Peter Ohr
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

#ifndef OUTPOST_RTOS_QUEUE_H
#define OUTPOST_RTOS_QUEUE_H

#include <pthread.h>

#include "outpost/posix/queue_raw.h"
#include <outpost/time/duration.h>

#include <stddef.h>
#include <stdint.h>

#include <type_traits>

namespace outpost
{
namespace rtos
{
/**
 * Atomic Queue.
 *
 * Can be used to exchange data between different threads.
 *
 * \warning
 *      Limited to POD types (see http://en.cppreference.com/w/cpp/concept/PODType)
 *      for compatibility with the FreeRTOS and RTEMS implementations.
 *
 * \author  Fabian Greif
 * \ingroup rtos
 */
template <typename T>
class Queue
{
    static_assert(std::is_trivially_copyable<T>::value && std::is_standard_layout<T>::value,
                  "T must be trivially copyable");

public:
    /**
     * Create a Queue.
     *
     * \param numberOfItems
     *      The maximum number of items that the queue can contain.
     */
    explicit Queue(size_t numberOfItems) : mQueueRaw(numberOfItems, sizeof(T))
    {
    }

    Queue(const Queue& other) = delete;

    Queue&
    operator=(const Queue& other) = delete;

    ~Queue() = default;

    /**
     * Send data to the queue.
     *
     * May trigger a thread rescheduling. The calling thread will be preempted
     * if a higher priority thread is unblocked as the result of this operation.
     *
     * \param data
     *      Reference to the item that is to be placed on the queue.
     *
     * \retval true     Value was successfully stored in the queue.
     * \retval false    Timeout occurred. Queue is full and data could not be
     *                  appended in the specified time.
     */
    inline bool
    send(const T& data)
    {
        return mQueueRaw.send(&data);
    }

    /**
     * Send data to the queue. Not needed in POSIX.
     *
     * \param data
     *      Reference to the item that is to be placed on the queue.
     * \param taskWoken
     *      Is set to 0 if the send operation wakes a higher priority task.
     *      In that case, a yield should be executed before exiting the ISR.
     * \retval true     Value was successfully stored in the queue.
     * \retval false    Queue is full, data could not be appended.
     */
    inline bool
    sendFromISR(const T& data, bool& hasWokenTask)
    {
        return mQueueRaw.sendFromISR(&data, hasWokenTask);
    }

    /**
     * Receive data from the queue.
     *
     * \param data
     *      Reference to the buffer into which the received item will be copied.
     * \param timeout
     *      Timeout in milliseconds resolution.
     *
     * \retval true     Value was received correctly and put in \p data.
     * \retval false    Timeout occurred, \p data was not changed.
     */
    inline bool
    receive(T& data, outpost::time::Duration timeout)
    {
        return mQueueRaw.receive(&data, timeout);
    }

    /**
     * Receive data from the queue. Not needed in POSIX.
     *
     * \param data
     *      Reference to the buffer into which the received item will be copied.
     * \param taskWoken
     *      Is set to 0 if the receive operation wakes a higher priority task.
     *      In that case, a yield should be executed before exiting the ISR.
     *
     * \retval true     Value was received correctly and put in \p data.
     * \retval false    Timeout occurred, \p data was not changed.
     */
    inline bool
    receiveFromISR(T& data, bool& hasWokenTask)
    {
        return mQueueRaw.receiveFromISR(&data, hasWokenTask);
    }

    /**
     * Clear queue.
     */
    inline void
    clear()
    {
        return mQueueRaw.clear();
    }

    /**
     * Get number of elements in the queue
     */
    inline size_t
    getNumberOfPendingMessages()
    {
        return mQueueRaw.getNumberOfPendingMessages();
    }

private:
    outpost::posix::QueueRaw mQueueRaw;
};

}  // namespace rtos
}  // namespace outpost

#endif
