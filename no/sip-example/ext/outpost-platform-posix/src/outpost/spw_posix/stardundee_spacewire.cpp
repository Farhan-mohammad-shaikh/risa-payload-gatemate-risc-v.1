/*
 * Copyright (c) 2021, Jan-Gerd Mess
 * Copyright (c) 2021, Jan Malburg
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "stardundee_spacewire.h"

#include "packet.hpp"
#include "stardundee_spacewire_factory.h"

namespace outpost
{
namespace spw_posix
{
StardundeeSpacewire::StardundeeSpacewire(const char* serialNumber, DeviceType type) :
    mTransmitEvent(outpost::rtos::BinarySemaphore::State::acquired),
    mReceiveBufferReleaseEvent(outpost::rtos::BinarySemaphore::State::released),
    mLinkSpeed(LinkSpeed::MBPS_10),
    mPort(1U),
    mChannelIndex(1U),
    mConfiguration(nullptr),
    mChannel(),
    mDevice(nullptr),
    mSerialNumber(serialNumber),
    mType(type)
{
    mDevice = outpost::spw_posix::StarDundeeDeviceFactory::getDevice(type, serialNumber);

    if (mDevice != nullptr)
    {
        mConfiguration = mDevice->GetDeviceConfiguration();
    }

    initializeBuffers();
}

void
StardundeeSpacewire::initializeBuffers()
{
    for (size_t i = 0; i < numberOfTransmitBuffers; ++i)
    {
        auto data = outpost::Slice<uint8_t>::unsafe(
                reinterpret_cast<uint8_t*>(mTransmitBufferData[i]), sizeof(mTransmitBufferData[i]));
        mTransmitBuffer[i] = TransmitBuffer(data);
        mTransmitBufferUsed[i] = false;
    }
    for (size_t i = 0; i < numberOfReceiveBuffers; ++i)
    {
        mReceiveBufferUsed[i] = false;
    }
}

bool
StardundeeSpacewire::isValid()
{
    bool result = (mDevice != nullptr) && (mConfiguration != nullptr);
    return result;
}

bool
StardundeeSpacewire::getTransmitBuffer(TransmitBuffer*& buffer)
{
    outpost::rtos::MutexGuard lock(mTransmissionMutex);

    bool found = false;
    for (size_t i = 0; (i < numberOfTransmitBuffers) && (found == false); ++i)
    {
        if (!mTransmitBufferUsed[i])
        {
            mTransmitBufferUsed[i] = true;
            buffer = &mTransmitBuffer[i];
            found = true;
        }
    }
    return found;
}

hal::SpaceWire::Result::Type
StardundeeSpacewire::requestBuffer(TransmitBuffer*& buffer, outpost::time::Duration timeout)
{
    Result::Type result = Result::failure;
    if (timeout == outpost::time::Duration::zero())
    {
        if (getTransmitBuffer(buffer))
        {
            result = Result::success;
        }
    }
    else
    {
        if (mTransmitEvent.acquire(timeout))
        {
            if (getTransmitBuffer(buffer))
            {
                result = Result::success;
            }
        }
        else
        {
            result = Result::timeout;
        }
    }
    return result;
}

hal::SpaceWire::Result::Type
StardundeeSpacewire::send(TransmitBuffer* buffer, outpost::time::Duration timeout)
{
    outpost::rtos::MutexGuard lock(mTransmissionMutex);
    SpaceWire::Result::Type result = SpaceWire::Result::failure;

    if (buffer != nullptr)
    {
        stardundee::com::starsystem::streamitems::Packet::EopType eop;
        eop = static_cast<stardundee::com::starsystem::streamitems::Packet::EopType>(
                buffer->getEndMarker());

        using stardundee::com::starsystem::transferoperations::TransferOperation;
        if (mChannel.TransmitPacket(
                    buffer->getData().begin(), buffer->getLength(), eop, timeout.milliseconds())
            == TransferOperation::TransferStatus::TRANSFER_STATUS_COMPLETE)
        {
            result = SpaceWire::Result::success;
        }
        releaseTransmitBuffer(buffer->getData().begin());
        mTransmitEvent.release();
    }

    return result;
}

hal::SpaceWire::Result::Type
StardundeeSpacewire::receive(ReceiveBuffer& buffer, outpost::time::Duration timeout)
{
    uint8_t* tmpBuffer;
    if (getReceiveBuffer(tmpBuffer))
    {
    }
    else if (mReceiveBufferReleaseEvent.acquire(timeout))
    {
        if (!getReceiveBuffer(tmpBuffer))
        {
            return hal::SpaceWire::Result::failure;
        }
    }
    else
    {
        return hal::SpaceWire::Result::timeout;
    }
    stardundee::com::starsystem::streamitems::Packet::EopType eop;
    uint32_t packetLength = maximumPacketLength;
    using stardundee::com::starsystem::transferoperations::TransferOperation;
    if (mChannel.ReceivePacket(tmpBuffer, &packetLength, &eop, timeout.milliseconds())
        == TransferOperation::TRANSFER_STATUS_COMPLETE)
    {
        hal::SpaceWire::EndMarker em = hal::SpaceWire::EndMarker::eop;
        buffer = ReceiveBuffer(outpost::Slice<uint8_t>::unsafe(tmpBuffer, packetLength), em);
        return hal::SpaceWire::Result::success;
    }
    else
    {
        ReceiveBuffer rcv(outpost::Slice<uint8_t>::unsafe(tmpBuffer, packetLength),
                          hal::SpaceWire::EndMarker::eop);
        releaseBuffer(rcv);
    }
    return hal::SpaceWire::Result::timeout;
}

void
StardundeeSpacewire::releaseBuffer(const ReceiveBuffer& buffer)
{
    bool found = false;
    for (size_t i = 0; i < numberOfReceiveBuffers && !found; ++i)
    {
        if (buffer.getData().begin() == mReceiveBufferData[i])
        {
            outpost::rtos::MutexGuard lock(mReceptionMutex);
            mReceiveBufferUsed[i] = false;
            mReceiveBufferReleaseEvent.release();
            found = true;
        }
    }
    if (!found)
    {
        printf("Buffer could not be found!\n");
    }
}

void
StardundeeSpacewire::releaseTransmitBuffer(uint8_t* dataBodyPointer)
{
    // Calculate the buffer index from the pointer address. The pointer
    // points two bytes into the transmit buffer data field.
    ptrdiff_t diff = (dataBodyPointer - dataBodyOffset)
                     - reinterpret_cast<uint8_t*>(&mTransmitBufferData[0]);
    diff /= sizeof(mTransmitBufferData[0]);

    if ((diff < 0) || (diff >= static_cast<ptrdiff_t>(numberOfTransmitBuffers)))
    {
        printf("SPW: Release failed\r\n");
    }
    else
    {
        outpost::rtos::MutexGuard lock(mTransmissionMutex);

        size_t index = diff;
        auto data = outpost::Slice<uint8_t>::unsafe(
                reinterpret_cast<uint8_t*>(mTransmitBufferData[index]),
                sizeof(mTransmitBufferData[index]));
        mTransmitBuffer[index] = TransmitBuffer(data);
        mTransmitBufferUsed[index] = false;
    }
}

bool
StardundeeSpacewire::getReceiveBuffer(uint8_t*& buffer)
{
    bool result = false;
    for (size_t i = 0; i < numberOfReceiveBuffers && !result; ++i)
    {
        outpost::rtos::MutexGuard lock(mReceptionMutex);
        if (false == mReceiveBufferUsed[i])
        {
            mReceiveBufferUsed[i] = true;
            buffer = mReceiveBufferData[i];
            result = true;
        }
    }
    return result;
}

bool
StardundeeSpacewire::addTimeCodeListener(outpost::rtos::Queue<outpost::hal::TimeCode>*)
{
    return false;
}

bool
StardundeeSpacewire::open()
{
    bool result = false;
    if (isValid())
    {
        stardundee::com::starsystem::deviceconfig::Port* port = mConfiguration->GetPort(mPort);

        if (port != nullptr
            && port->GetType() == stardundee::com::starsystem::deviceconfig::Port::PORT_TYPE_LINK)
        {
            mChannel = mDevice->OpenChannel(
                    stardundee::com::starsystem::channels::Channel::DIRECTION_INOUT,
                    mChannelIndex,
                    1);

            delete port;
        }

        result = mChannel.IsOpen();
    }
    return result;
}

outpost::spw_posix::StardundeeSpacewire::~StardundeeSpacewire()
{
}

}  // namespace spw_posix
}  // namespace outpost
