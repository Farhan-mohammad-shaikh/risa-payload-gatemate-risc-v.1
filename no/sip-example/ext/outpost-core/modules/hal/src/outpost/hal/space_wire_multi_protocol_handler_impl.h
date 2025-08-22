/*
 * Copyright (c) 2019, 2021, Jan Malburg
 * Copyright (c) 2021, Felix Passenberg
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2023, Jan-Gerd Mess
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_HAL_SPACEWIRE_MULTI_PROTOCOL_HANDLER_IMPL_H_
#define OUTPOST_HAL_SPACEWIRE_MULTI_PROTOCOL_HANDLER_IMPL_H_

#include "space_wire_multi_protocol_handler.h"

#include <outpost/utils/minmax.h>

namespace outpost
{
namespace hal
{
template <uint32_t maxPackages, uint32_t maxPacketSize>
bool
SpaceWireMultiProtocolHandler<maxPackages, maxPacketSize>::send(const Slice<const uint8_t>& buffer,
                                                                time::Duration timeout)
{
    time::SpacecraftElapsedTime startTime = mClock.now();

    auto requestResult = mSpWHandle.getSpaceWire().requestBuffer(timeout);
    if (!requestResult)
    {
        return false;
    }
    SpaceWire::TransmitBuffer* transmitBuffer = *requestResult;
    // better reject to long packages than cutting them
    if (transmitBuffer->getLength() < buffer.getNumberOfElements())
    {
        return false;
    }
    else
    {
        if (!transmitBuffer->getData().copyFrom(buffer))
        {
            // buffer invalid?
            return false;
        }
        transmitBuffer->setLength(buffer.getNumberOfElements());
        transmitBuffer->setEndMarker(hal::SpaceWire::EndMarker::eop);
    }

    if (timeout != time::Duration::zero() && timeout < time::Duration::myriad())
    {
        // update remaining timeout unless we are in non-blocking anyways
        time::SpacecraftElapsedTime now = mClock.now();
        if (now > startTime + timeout)
        {
            return false;
        }
        else
        {
            timeout = timeout - (now - startTime);
        }
    }

    SpaceWire::OperationResult result = mSpWHandle.getSpaceWire().send(transmitBuffer, timeout);
    if (result == SpaceWire::OperationResult::success)
    {
        return true;
    }
    else
    {
        return false;
    }
}

template <uint32_t maxPackages, uint32_t maxPacketSize>
Expected<SpWMessage, utils::OperationResult>
SpaceWireMultiProtocolHandler<maxPackages, maxPacketSize>::SpaceWireHandle::receive(
        time::Duration timeout)
{
    if (!mSpw.isUp())
    {
        mSpw.up(timeout);
        // will be called again anyways, so we don't handle the remaining timeout stuff
        return unexpected(utils::OperationResult::genericError);
    }

    auto result = mSpw.receive(timeout);
    if (!result)
    {
        switch (result.error())
        {
            case hal::SpaceWire::OperationResult::timeout:
                return unexpected(utils::OperationResult::timeout);
            case hal::SpaceWire::OperationResult::failure:
            default: return unexpected(utils::OperationResult::genericError);
        }
    }
    else
    {
        SpaceWire::ReceiveBuffer receiveBuffer = *result;
        bool success = false;
        uint32_t receivedSize = receiveBuffer.getLength();
        SpWMessage data;
        if (receivedSize >= protocolByteOffset + 1)
        {
            container::SharedBufferPointer pointer;
            if (mPool.allocate(pointer))
            {
                uint32_t copySize = utils::min<uint32_t>(receivedSize, maxPacketSize);
                if (!pointer.asSlice().copyFrom(receiveBuffer.getData().getDataPointer(), copySize))
                {
                    mSpw.releaseBuffer(receiveBuffer);
                    return unexpected(utils::OperationResult::genericError);
                }
                container::SharedChildPointer child;
                pointer.getChild(child, 0, 0, copySize);
                data.buffer = child;
                data.id.protocol = pointer[protocolByteOffset];
                if (receivedSize > maxPacketSize)
                {
                    // we cut the package
                    data.id.end = hal::SpaceWire::EndMarker::partial;
                }
                else
                {
                    data.id.end = receiveBuffer.getEndMarker();
                }
                success = true;
            }
        }

        mSpw.releaseBuffer(receiveBuffer);
        if (success)
        {
            return data;
        }
        else
        {
            return unexpected(utils::OperationResult::genericError);
        }
    }
}

template <uint32_t maxPackages, uint32_t maxPacketSize>
inline SpaceWire&
SpaceWireMultiProtocolHandler<maxPackages, maxPacketSize>::SpaceWireHandle::getSpaceWire()
{
    return mSpw;
}

template <uint32_t maxPackages, uint32_t maxPacketSize>
void
SpaceWireMultiProtocolHandler<maxPackages, maxPacketSize>::start()
{
    swb::BusDistributor<MessageID>::start();
}

/**
 * Add a listener for timecode
 * @param queue the queue to add
 * @return false if queue == nullptr or all places for Listener are filled
 */
template <uint32_t maxPackages, uint32_t maxPacketSize>
bool
SpaceWireMultiProtocolHandler<maxPackages, maxPacketSize>::addTimeCodeListener(
        rtos::Queue<TimeCode>* queue)
{
    return mSpWHandle.getSpaceWire().addTimeCodeListener(queue);
}

}  // namespace hal
}  // namespace outpost

#endif
