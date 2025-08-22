/*
 * Copyright (c) 2020-2022, Jan-Gerd Mess
 * Copyright (c) 2021, pfef_to
 * Copyright (c) 2021, Jan Malburg
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_SWB_BUS_CHANNEL_IMPL_H_
#define OUTPOST_SWB_BUS_CHANNEL_IMPL_H_

#include "bus_channel.h"

#include <outpost/container/shared_buffer.h>

namespace outpost
{
namespace swb
{
template <typename IDType>
BusChannel<IDType>::BusChannel() :
    mNumIncomingMessages(0),
    mNumAppendedMessages(0),
    mNumFailedReceptions(0),
    mNumRetrievedMessages(0)
{
}

template <typename IDType>
BufferedBusChannel<IDType>::BufferedBusChannel(outpost::Slice<Message<IDType>> buffer) :
    mBuffer(buffer), mMessageAvailable(0U)
{
}

template <typename IDType>
OperationResult
BufferedBusChannel<IDType>::sendMessage(const Message<IDType>& m)
{
    OperationResult res = OperationResult::sendFailed;
    BusChannel<IDType>::mNumIncomingMessages++;

    if (matches(m))
    {
        outpost::rtos::MutexGuard lock(mMutex);
        if (mBuffer.append(m))
        {
            BusChannel<IDType>::mNumAppendedMessages++;
            mMessageAvailable.release();
            res = OperationResult::success;
        }
        else
        {
            BusChannel<IDType>::mNumFailedReceptions++;
            res = OperationResult::noBufferAvailable;
        }
    }
    return res;
}

template <typename IDType>
OperationResult
BufferedBusChannel<IDType>::receiveMessage(Message<IDType>& m, outpost::time::Duration timeout)
{
    OperationResult res = OperationResult::success;
    if (mMessageAvailable.acquire(timeout))
    {
        outpost::rtos::MutexGuard lock(mMutex);
        if (!mBuffer.isEmpty())
        {
            m = mBuffer.getFront();
            // required to release the underlying buffer because outpost::Deque<T> will not do so
            // automatically for the Message type.
            mBuffer.getFront() = Message<IDType>();
            mBuffer.removeFront();
            BusChannel<IDType>::mNumRetrievedMessages++;
        }
        else
        {
            res = OperationResult::invalidState;
        }
    }
    else
    {
        res = OperationResult::noMessageAvailable;
    }
    return res;
}

template <typename IDType>
size_t
BufferedBusChannel<IDType>::getCurrentNumberOfMessages()
{
    outpost::rtos::MutexGuard lock(mMutex);
    return mBuffer.getSize();
}

}  // namespace swb
}  // namespace outpost

#endif
