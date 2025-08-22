/*
 * Copyright (c) 2021, Andre Nahrwold
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

#include "spi_stub.h"

#include <string.h>  // for memcpy

using namespace unittest::hal;
using namespace outpost::hal;

SpiStub::SpiStub()
{
}

SpiStub::~SpiStub()
{
}

void
SpiStub::close()
{
    mDataToRead.clear();
    mDataToWrite.clear();
}

Spi::ReturnCode
SpiStub::read(outpost::Slice<uint8_t> data, uint8_t /*cs*/)
{
    if (data.begin() == nullptr || data.getNumberOfElements() == 0
        || data.getNumberOfElements() > mDataToRead.size())
    {
        return Spi::ReturnCode::dataInvalid;
    }
    data.copyFrom(&mDataToRead[0], data.getNumberOfElements());
    mDataToRead.erase(mDataToRead.begin(), mDataToRead.begin() + data.getNumberOfElements());

    return Spi::ReturnCode::success;
}

Spi::ReturnCode
SpiStub::write(outpost::Slice<const uint8_t> data, uint8_t /*cs*/)
{
    if (data.begin() == nullptr || data.getNumberOfElements() == 0)
    {
        return Spi::ReturnCode::dataInvalid;
    }

    mDataToWrite.insert(mDataToWrite.end(), data.begin(), data.end());

    return Spi::ReturnCode::success;
}

Spi::ReturnCode
SpiStub::transfer(outpost::Slice<const uint8_t> outData,
                  outpost::Slice<uint8_t> inData,
                  uint8_t /*cs*/)
{
    if (outData.begin() == nullptr || inData.begin() == nullptr
        || outData.getNumberOfElements() == 0 || inData.getNumberOfElements() == 0
        || inData.getNumberOfElements() > mDataToRead.size())
    {
        return Spi::ReturnCode::dataInvalid;
    }

    if (outData.getNumberOfElements() != inData.getNumberOfElements())
    {
        return Spi::ReturnCode::inAndOutDataHaveDifferentSize;
    }

    mDataToWrite.insert(mDataToWrite.end(), outData.begin(), outData.end());

    inData.copyFrom(&mDataToRead[0], inData.getNumberOfElements());
    mDataToRead.erase(mDataToRead.begin(), mDataToRead.begin() + inData.getNumberOfElements());

    return Spi::ReturnCode::success;
}
