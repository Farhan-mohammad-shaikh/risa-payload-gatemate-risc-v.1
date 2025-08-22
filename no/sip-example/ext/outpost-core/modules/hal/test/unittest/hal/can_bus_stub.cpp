/*
 * Copyright (c) 2022, Jan-Gerd Mess
 * Copyright (c) 2022, Tobias Pfeffer
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "can_bus_stub.h"

#include <math.h>
#include <string.h>

using unittest::hal::CanBusStubBase;

CanBusStubBase::ReturnCode
CanBusStubBase::open()
{
    mOpen = true;
    return ReturnCode::success;
}

CanBusStubBase::ReturnCode
CanBusStubBase::close()
{
    mDataToReceive.clear();
    mDataToTransmit.clear();
    mOpen = false;
    return ReturnCode::success;
}

CanBusStubBase::ReturnCode
CanBusStubBase::read(CanFrame& frame, outpost::time::Duration timeout)
{
    (void) timeout;

    if (!mOpen)
    {
        return ReturnCode::notInitialized;
    }
    else if (mDataToReceive.isEmpty())
    {
        return ReturnCode::bufferEmpty;
    }

    frame = mDataToReceive.getFront();
    mDataToReceive.removeFront();
    return ReturnCode::success;
}

CanBusStubBase::ReturnCode
CanBusStubBase::write(const CanFrame& frame)
{
    if (!mOpen)
    {
        return ReturnCode::notInitialized;
    }

    mDataToTransmit.append(frame);
    return ReturnCode::success;
}

CanBusStubBase::ReturnCode
CanBusStubBase::clearReceiveBuffer()
{
    mDataToReceive.clear();
    return ReturnCode::success;
}

CanBusStubBase::ReturnCode
CanBusStubBase::transferToOtherStub(CanBusStubBase& rxStub, bool clearTxBuffer)
{
    if (!mOpen || !rxStub.mOpen)
    {
        return ReturnCode::notInitialized;
    }

    rxStub.mDataToReceive.append(mTransmitBuffer.skipFirst(1).first(mDataToTransmit.getSize()));

    if (clearTxBuffer)
    {
        mDataToTransmit.clear();
    }

    return ReturnCode::success;
}

bool
CanBusStubBase::isDataAvailable()
{
    return !mDataToReceive.isEmpty();
}
