/*
 * Copyright (c) 2021, Jan Malburg
 * Copyright (c) 2022, Tobias Pfeffer
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_SWB_BUS_DISTRIBUTOR_IMPL_H_
#define OUTPOST_SWB_BUS_DISTRIBUTOR_IMPL_H_

#include "bus_distributor.h"

#include <outpost/rtos/mutex_guard.h>
#include <outpost/utils/communicator.h>

namespace outpost
{
namespace swb
{
template <typename IDType>
BusDistributor<IDType>::BusDistributor(
        outpost::utils::Receiver<Message<IDType>>& receiver,
        uint8_t priority,
        outpost::support::parameter::HeartbeatSource heartbeatSource) :
    mHandlerThread(*this, receiver, heartbeatSource, priority),
    mReceiver(receiver),
    mDefaultChannel(nullptr)
{
}

template <typename IDType>
void
BusDistributor<IDType>::start()
{
    mHandlerThread.start();
}

template <typename IDType>
OperationResult
BusDistributor<IDType>::registerChannel(BusChannel<IDType>& channel)
{
    outpost::rtos::MutexGuard lock(mChannelMutex);
    mChannels.append(&channel);
    return OperationResult::success;
}

template <typename IDType>
OperationResult
BusDistributor<IDType>::unregisterChannel(BusChannel<IDType>& channel)
{
    outpost::rtos::MutexGuard lock(mChannelMutex);
    if (mChannels.removeNode(&channel))
    {
        return OperationResult::success;
    }
    else
    {
        return OperationResult::notFound;
    }
}

template <typename IDType>
OperationResult
BusDistributor<IDType>::setDefaultChannel(BusChannel<IDType>& channel)
{
    outpost::rtos::MutexGuard lock(mChannelMutex);
    if (nullptr == mDefaultChannel)
    {
        mDefaultChannel = &channel;
        return OperationResult::success;
    }
    else
    {
        return OperationResult::maxChannelsReached;
    }
}

template <typename IDType>
void
BusDistributor<IDType>::handleAllMessages()
{
    while (mHandlerThread.step(outpost::time::Duration::zero()))
    {
        // all done in the condition
    }
}

template <typename IDType>
bool
BusDistributor<IDType>::handleSingleMessage()
{
    return mHandlerThread.step(outpost::time::Duration::zero());
}

}  // namespace swb
}  // namespace outpost

#endif
