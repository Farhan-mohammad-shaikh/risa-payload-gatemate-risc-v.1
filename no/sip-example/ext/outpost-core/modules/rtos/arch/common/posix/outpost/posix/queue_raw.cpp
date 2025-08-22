/*
 * Copyright (c) 2014-2017, 2019, Fabian Greif
 * Copyright (c) 2021, Felix Passenberg
 * Copyright (c) 2021, pfef_to
 * Copyright (c) 2022-2023, Jan-Gerd Mess
 * Copyright (c) 2023, Peter Ohr
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "queue_raw.h"

#include "internal/time.h"

#include <outpost/rtos/failure_handler.h>

#include <string.h>

using namespace outpost::posix;

// ---------------------------------------------------------------------------
QueueRaw::QueueRaw(size_t numberOfItems, size_t itemSize) :
    mBuffer(new uint8_t[numberOfItems * itemSize]),
    mMaximumSize(numberOfItems),
    mItemSize(itemSize),
    mItemsInBuffer(0),
    mHead(0),
    mTail(0)
{
    pthread_mutex_init(&mMutex, nullptr);
    pthread_cond_init(&mSignal, nullptr);
}

QueueRaw::~QueueRaw()
{
    pthread_mutex_lock(&mMutex);

    mItemsInBuffer = 0;
    mHead = 0;
    mTail = 0;
    delete[] mBuffer;

    pthread_mutex_unlock(&mMutex);

    pthread_mutex_destroy(&mMutex);
    pthread_cond_destroy(&mSignal);
}

// ---------------------------------------------------------------------------
bool
QueueRaw::send(const void* data)
{
    // might be clobbered by setjmp/longjmp -> volatile
    volatile bool itemStored = false;

    pthread_mutex_lock(&mMutex);
    pthread_cleanup_push(unlockMutex, &mMutex);
    if (mItemsInBuffer < mMaximumSize)
    {
        mHead = increment(mHead);

        uint8_t* pointer = &mBuffer[mHead * mItemSize];
        ::memcpy(pointer, data, mItemSize);
        mItemsInBuffer++;
        itemStored = true;

        pthread_cond_signal(&mSignal);
    }

    pthread_cleanup_pop(1);
    return itemStored;
}

bool
QueueRaw::receive(void* data, outpost::time::Duration timeout)
{
    // might be clobbered by setjmp/longjmp -> volatile
    volatile bool itemRetrieved = false;
    volatile bool timeoutOrErrorOccurred = false;

    pthread_mutex_lock(&mMutex);
    pthread_cleanup_push(unlockMutex, &mMutex);
    while ((mItemsInBuffer == 0) && !timeoutOrErrorOccurred)
    {
        if (timeout >= outpost::time::Duration::myriad())
        {
            if (pthread_cond_wait(&mSignal, &mMutex) != 0)
            {
                // Error has occurred
                timeoutOrErrorOccurred = true;
            }
        }
        else
        {
            timespec time = rtos::toAbsoluteTime(CLOCK_REALTIME, timeout);
            if (pthread_cond_timedwait(&mSignal, &mMutex, &time) != 0)
            {
                // Timeout or other error has occurred
                timeoutOrErrorOccurred = true;
            }
        }
    }

    if (!timeoutOrErrorOccurred)
    {
        mTail = increment(mTail);

        const uint8_t* pointer = &mBuffer[mTail * mItemSize];
        ::memcpy(data, pointer, mItemSize);
        mItemsInBuffer--;
        itemRetrieved = true;
    }

    pthread_cleanup_pop(1);
    return itemRetrieved;
}

void
QueueRaw::clear()
{
    pthread_mutex_lock(&mMutex);
    pthread_cleanup_push(unlockMutex, &mMutex);

    mItemsInBuffer = 0;
    mHead = 0;
    mTail = 0;

    pthread_cleanup_pop(1);
}

size_t
QueueRaw::getNumberOfPendingMessages()
{
    // might be clobbered by setjmp/longjmp -> volatile
    volatile size_t itemsInBuffer = 0;

    pthread_mutex_lock(&mMutex);
    pthread_cleanup_push(unlockMutex, &mMutex);

    itemsInBuffer = mItemsInBuffer;

    pthread_cleanup_pop(1);

    return itemsInBuffer;
}

size_t
QueueRaw::increment(size_t index) const
{
    if (index >= (mMaximumSize - 1))
    {
        index = 0;
    }
    else
    {
        index++;
    }

    return index;
}

void
QueueRaw::unlockMutex(void* mutex)
{
    pthread_mutex_unlock(reinterpret_cast<pthread_mutex_t*>(mutex));
}
