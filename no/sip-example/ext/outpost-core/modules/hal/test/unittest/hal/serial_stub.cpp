/*
 * Copyright (c) 2015-2017, Fabian Greif
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2023, Pascal Pieper
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "serial_stub.h"

#include <string.h>

using unittest::hal::SerialStub;

SerialStub::SerialStub()
{
}

SerialStub::~SerialStub()
{
}

void
SerialStub::close()
{
    mDataToReceive.clear();
    mDataToTransmit.clear();
}

bool
SerialStub::isAvailable()
{
    return !mDataToReceive.empty();
}

size_t
SerialStub::getNumberOfBytesAvailable()
{
    return mDataToReceive.size();
}

SerialStub::MaybeSize
SerialStub::read(const outpost::Slice<uint8_t>& data, const outpost::time::Duration& /*timeout*/)
{
    size_t length = data.getNumberOfElements();
    size_t available = mDataToReceive.size();

    if (length > available)
    {
        length = available;
    }

    if (length > 0)
    {
        data.copyFrom(&mDataToReceive[0], length);
        mDataToReceive.erase(mDataToReceive.begin(), mDataToReceive.begin() + length);
    }
    return length;
}

SerialStub::MaybeSize
SerialStub::write(const outpost::Slice<const uint8_t>& data,
                  const outpost::time::Duration& /*timeout*/)
{
    mDataToTransmit.insert(mDataToTransmit.end(), data.begin(), data.end());
    return data.getNumberOfElements();
}

void
SerialStub::flushReceiver()
{
    // Do nothing
}

void
SerialStub::flushTransmitter()
{
    // Do nothing
}
