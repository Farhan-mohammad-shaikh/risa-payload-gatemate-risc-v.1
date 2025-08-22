/*
 * Copyright (c) 2014-2017, 2019, Fabian Greif
 * Copyright (c) 2023, Peter Ohr
 * Copyright (c) 2023, Jan-Gerd Mess
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_RTOS_NONE_QUEUE_IMPL_H
#define OUTPOST_RTOS_NONE_QUEUE_IMPL_H

#include "queue.h"

#include <outpost/rtos/failure_handler.h>

template <typename T>
outpost::rtos::Queue<T>::Queue(size_t numberOfItems) :
    mBuffer(new T[numberOfItems]),
    mMaximumSize(numberOfItems),
    mItemsInBuffer(0),
    mHead(0),
    mTail(0)
{
}

template <typename T>
outpost::rtos::Queue<T>::~Queue()
{
    delete[] mBuffer;
}

template <typename T>
bool
outpost::rtos::Queue<T>::send(const T& data)
{
    bool itemStored = false;
    if (mItemsInBuffer < mMaximumSize)
    {
        mHead = increment(mHead);

        mBuffer[mHead] = data;
        mItemsInBuffer++;
        itemStored = true;
    }

    return itemStored;
}

template <typename T>
bool
outpost::rtos::Queue<T>::receive(T& data, outpost::time::Duration)
{
    bool itemRetrieved = false;
    if (mItemsInBuffer > 0)
    {
        mTail = increment(mTail);

        data = mBuffer[mTail];
        mItemsInBuffer--;
        itemRetrieved = true;
    }

    return itemRetrieved;
}

template <typename T>
void
outpost::rtos::Queue<T>::clear()
{
    mItemsInBuffer = 0;
    mHead = 0;
    mTail = 0;
}

template <typename T>
size_t
outpost::rtos::Queue<T>::getNumberOfPendingMessages()
{
    return mItemsInBuffer;
}

template <typename T>
size_t
outpost::rtos::Queue<T>::increment(size_t index) const
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

#endif
