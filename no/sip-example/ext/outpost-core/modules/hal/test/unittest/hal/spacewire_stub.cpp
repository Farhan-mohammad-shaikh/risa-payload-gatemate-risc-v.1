/*
 * Copyright (c) 2016-2017, Fabian Greif
 * Copyright (c) 2017, Muhammad Bassam
 * Copyright (c) 2019, 2021, Jan Malburg
 * Copyright (c) 2020, Felix Passenberg
 * Copyright (c) 2021, Thomas Firchau
 * Copyright (c) 2022-2023, Jan-Gerd Mess
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "spacewire_stub.h"

using unittest::hal::SpaceWireStub;

SpaceWireStub::SpaceWireStub(size_t maximumLength) :
    mMaximumLength(maximumLength), mOpen(false), mUp(false)
{
}

SpaceWireStub::~SpaceWireStub()
{
}

size_t
SpaceWireStub::getMaximumPacketLength() const
{
    return mMaximumLength;
}

bool
SpaceWireStub::open()
{
    mOpen = true;
    return mOpen;
}

void
SpaceWireStub::close()
{
    mOpen = false;
    mUp = false;
}

bool
SpaceWireStub::up(outpost::time::Duration /*timeout*/)
{
    if (mOpen)
    {
        mUp = true;
    }
    return mUp;
}

void
SpaceWireStub::down(outpost::time::Duration /*timeout*/)
{
    mUp = false;
}

bool
SpaceWireStub::isUp()
{
    return mUp;
}

outpost::Expected<outpost::hal::SpaceWire::TransmitBuffer*,
                  outpost::hal::SpaceWire::OperationResult>
SpaceWireStub::requestBuffer(outpost::time::Duration /*timeout*/)
{
    outpost::hal::SpaceWire::TransmitBuffer* buffer = nullptr;
    {
        outpost::rtos::MutexGuard lock(mOperationLock);
        std::unique_ptr<TransmitBufferEntry> entry(new TransmitBufferEntry(mMaximumLength));
        buffer = &entry->header;
        mTransmitBuffers.emplace(make_pair(&(entry->header), std::move(entry)));
    }

    return buffer;
}

SpaceWireStub::OperationResult
SpaceWireStub::send(TransmitBuffer* buffer, outpost::time::Duration /*timeout*/)
{
    OperationResult result = OperationResult::success;
    if (mUp)
    {
        try
        {
            outpost::rtos::MutexGuard lock(mOperationLock);
            std::unique_ptr<TransmitBufferEntry>& entry = mTransmitBuffers.at(buffer);
            mSentPackets.emplace_back(
                    Packet{std::vector<uint8_t>(&entry->buffer.front(),
                                                &entry->buffer.front() + entry->header.getLength()),
                           entry->header.getEndMarker()});
            mTransmitBuffers.erase(buffer);
        }
        catch (std::out_of_range&)
        {
            result = OperationResult::failure;
        }
    }
    else
    {
        result = OperationResult::failure;
    }

    return result;
}

outpost::Expected<outpost::hal::SpaceWire::ReceiveBuffer, outpost::hal::SpaceWire::OperationResult>
SpaceWireStub::receive(outpost::time::Duration /*timeout*/)
{
    OperationResult result = OperationResult::success;
    ReceiveBuffer buffer;
    if (mUp)
    {
        if (!mPacketsToReceive.empty())
        {
            std::unique_ptr<ReceiveBufferEntry> entry(new ReceiveBufferEntry(
                    std::move(mPacketsToReceive.front().data), mPacketsToReceive.front().end));
            buffer = entry->header;
            mReceiveBuffers.emplace(make_pair(entry->header.getData().begin(), std::move(entry)));
            mPacketsToReceive.pop_front();
        }
        else
        {
            result = OperationResult::timeout;
        }
    }
    else
    {
        result = OperationResult::failure;
    }
    if (result != OperationResult::success)
    {
        return outpost::unexpected(result);
    }
    return buffer;
}

void
SpaceWireStub::releaseBuffer(const ReceiveBuffer& buffer)
{
    mReceiveBuffers.erase(buffer.getData().begin());
}

void
SpaceWireStub::flushReceiveBuffer()
{
    // Do nothing
}

void
SpaceWireStub::triggerSpWInterrupt(void)
{
    outpost::hal::TimeCode tc{0, 0};
    mTCD.dispatchTimeCode(tc);
}
