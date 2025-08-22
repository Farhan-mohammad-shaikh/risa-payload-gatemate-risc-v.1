/*
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_POSIX_QUEUE_RAW_H
#define OUTPOST_POSIX_QUEUE_RAW_H

#include <pthread.h>

#include <outpost/time/duration.h>

#include <stddef.h>
#include <stdint.h>

namespace outpost
{
namespace posix
{
/**
 * Atomic Queue.
 *
 * Can be used to exchange data between different threads.
 *
 * \author  Fabian Greif
 * \ingroup rtos
 */
class QueueRaw
{
public:
    explicit QueueRaw(size_t numberOfItems, size_t itemSize);

    QueueRaw(const QueueRaw& other) = delete;

    QueueRaw&
    operator=(const QueueRaw& other) = delete;

    ~QueueRaw();

    bool
    send(const void* data);

    inline bool
    sendFromISR(const void* data, bool& hasWokenTask)
    {
        hasWokenTask = false;
        return send(data);
    }

    bool
    receive(void* data, outpost::time::Duration timeout);

    inline bool
    receiveFromISR(void* data, bool& hasWokenTask)
    {
        hasWokenTask = false;
        return receive(data, outpost::time::Duration::zero());
    }

    void
    clear();

    /**
     * Get number of elements in the queue
     */
    size_t
    getNumberOfPendingMessages();

private:
    size_t
    increment(size_t index) const;

    static void
    unlockMutex(void* mutex);

    // POSIX handles
    pthread_mutex_t mMutex;
    pthread_cond_t mSignal;

    uint8_t* mBuffer;
    const size_t mMaximumSize;
    const size_t mItemSize;
    size_t mItemsInBuffer;
    size_t mHead;
    size_t mTail;
};

}  // namespace posix
}  // namespace outpost

#endif
