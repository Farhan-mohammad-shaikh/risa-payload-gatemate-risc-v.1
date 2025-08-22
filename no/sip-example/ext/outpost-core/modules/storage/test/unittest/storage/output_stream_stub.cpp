/*
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

#include "output_stream_stub.h"

#include <stdio.h>

size_t
unittest::storage::OutputStreamStub::append(const outpost::Slice<const uint8_t>& data)
{
    if (mData.size() >= mLimit)
    {
        return 0;
    }
    else if (mData.size() + data.getNumberOfElements() <= mLimit)
    {
        mData.insert(mData.end(), data.begin(), data.end());
        return data.getNumberOfElements();
    }
    else
    {
        auto tmp = data.first(mLimit - mData.size());
        mData.insert(mData.end(), tmp.begin(), tmp.end());
        return tmp.getNumberOfElements();
    }
}
