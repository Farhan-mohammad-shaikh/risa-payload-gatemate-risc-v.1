/*
 * Copyright (c) 2020-2021, 2023, Jan-Gerd Mess
 * Copyright (c) 2021, Jan Malburg
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_SWB_SOFTWARE_BUS_IMPL_H
#define OUTPOST_SWB_SOFTWARE_BUS_IMPL_H

#include "software_bus.h"

#include <outpost/container/reference_queue.h>
#include <outpost/container/shared_object_pool.h>

namespace outpost
{
namespace swb
{
template <typename IDType>
SoftwareBus<IDType>::SoftwareBus(container::SharedBufferPoolBase& pool,
                                 container::ReferenceQueueBase<Message<IDType>>& queue,
                                 uint8_t priority,
                                 support::parameter::HeartbeatSource heartbeatSource) :
    BusDistributor<IDType>(queue, priority, heartbeatSource),
    mPool(pool),
    mQueue(queue),
    mNumberOfInvalidMessages(0U),
    mNumberOfFailedCopyOperations(0U),
    mNumberOfFailedSendOperations(0U),
    mNumberOfAcceptedMessages(0U)
{
}

template <typename IDType>
OperationResult
SoftwareBus<IDType>::sendMessage(const IDType id, const Slice<const uint8_t>& data)
{
    rtos::MutexGuard lock(mMutex);
    if (!valid(id, data))
    {
        mNumberOfInvalidMessages++;
        return OperationResult::invalidMessage;
    }
    container::ConstSharedBufferPointer buffer;
    OperationResult res = copySliceToBuffer(data, buffer);
    if (OperationResult::success == res)
    {
        return sendMessage(id, buffer, CopyMode::zero_copy);
    }
    else
    {
        mNumberOfFailedCopyOperations++;
    }
    return res;
}

template <typename IDType>
OperationResult
SoftwareBus<IDType>::sendMessage(const IDType id,
                                 const container::ConstSharedBufferPointer& buffer,
                                 CopyMode mode)
{
    rtos::MutexGuard lock(mMutex);
    if (!valid(id, buffer.asSlice()))
    {
        mNumberOfInvalidMessages++;
        return OperationResult::invalidMessage;
    }

    container::ConstSharedBufferPointer tmpBuffer;
    if (CopyMode::copy_once == mode)
    {
        OperationResult res = copySliceToBuffer(buffer.asSlice(), tmpBuffer);
        if (OperationResult::success != res)
        {
            mNumberOfFailedCopyOperations++;
            return res;
        }
    }
    else
    {
        tmpBuffer = buffer;
    }

    Message<IDType> msg = {id, tmpBuffer};
    return sendMessage(msg);
}

template <typename IDType>
OperationResult
SoftwareBus<IDType>::sendMessage(Message<IDType>& msg)
{
    rtos::MutexGuard lock(mMutex);
    if (!valid(msg.id, msg.buffer.asSlice()))
    {
        mNumberOfInvalidMessages++;
        return OperationResult::invalidMessage;
    }

    if (utils::OperationResult::success == mQueue.send(msg))
    {
        mNumberOfAcceptedMessages++;
        return OperationResult::success;
    }
    else
    {
        mNumberOfFailedSendOperations++;
        return OperationResult::sendFailed;
    }
}

template <typename IDType>
OperationResult
SoftwareBus<IDType>::copySliceToBuffer(const Slice<const uint8_t>& slice,
                                       container::ConstSharedBufferPointer& buffer)
{
    if (slice.getNumberOfElements() == 0)
    {
        container::SharedBufferPointer tmp;
        buffer = tmp;
        return OperationResult::success;
    }
    else if (slice.getNumberOfElements() <= mPool.getElementSize())
    {
        container::SharedBufferPointer tmp;
        if (mPool.allocate(tmp))
        {
            tmp.asSlice().copyFrom(slice);
            container::SharedChildPointer c;
            tmp.getChild(c, 0, 0, slice.getNumberOfElements());
            buffer = c;
            return OperationResult::success;
        }
        return OperationResult::noBufferAvailable;
    }
    return OperationResult::messageTooLong;
}

}  // namespace swb
}  // namespace outpost

#endif
