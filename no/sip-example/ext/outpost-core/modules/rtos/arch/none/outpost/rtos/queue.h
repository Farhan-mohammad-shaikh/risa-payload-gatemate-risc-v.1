/*
 * Copyright (c) 2014-2017, 2019, Fabian Greif
 * Copyright (c) 2020-2021, Jan Malburg
 * Copyright (c) 2022-2023, Jan-Gerd Mess
 * Copyright (c) 2023, Peter Ohr
 * Copyright (c) 2024, Reinking, Janosch
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_RTOS_RTEMS_QUEUE_H
#define OUTPOST_RTOS_RTEMS_QUEUE_H

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
     * Uses \p new to allocate the storage space for the queue.
     *
     * \param numberOfItems
     *      The maximum number of items that the queue can contain.
     */
    explicit Queue(size_t numberOfItems);

    // disable copy constructor
    Queue(const Queue& other) = delete;

    // disable assignment operator
    Queue&
    operator=(const Queue& other) = delete;

    /**
     * Destroy the queue.
     */
    ~Queue();

    /**
     * Send data to the queue.
     *
     * \param data
     *      Reference to the item that is to be placed on the queue.
     *
     * \retval true     Value was successfully stored in the queue.
     * \retval false    Queue is full and data could not be appended.
     */
    bool
    send(const T& data);

    /**
     * Send data to the queue. Not needed in bare-metal.
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
        hasWokenTask = false;
        return send(data);
    }

    /**
     * Receive data from the queue.
     *
     * \param data
     *      Reference to the buffer into which the received item will be copied.
     * \param timeout
     *      Ignored. Function always returns immediately.
     *
     * \retval true     Value was received correctly and put in \p data.
     * \retval false    Queue empty, no data was retrieved.
     */
    bool
    receive(T& data, outpost::time::Duration timeout);

    /**
     * Receive data from the queue. Not needed in bare-metal.
     *
     * \param data
     *      Reference to the buffer into which the received item will be copied.
     * \param taskWoken
     *      Is set to true if the receive operation wakes a higher priority task.
     *      In that case, a yield should be executed before exiting the ISR.
     *
     * \retval true     Value was received correctly and put in \p data.
     * \retval false    No data available or some other error occured, \p data was not changed.
     */
    inline bool
    receiveFromISR(T& data, bool& hasWokenTask)
    {
        hasWokenTask = false;
        return receive(data, outpost::time::Duration::zero());
    }

    /**
     * Clear queue.
     *
     */
    void
    clear();

    /**
     * Get number of elements in the queue
     *
     */
    size_t
    getNumberOfPendingMessages();

private:
    size_t
    increment(size_t index) const;

    T* mBuffer;
    const size_t mMaximumSize;
    size_t mItemsInBuffer;
    size_t mHead;
    size_t mTail;
};

}  // namespace rtos
}  // namespace outpost

#include "queue_impl.h"

#endif
