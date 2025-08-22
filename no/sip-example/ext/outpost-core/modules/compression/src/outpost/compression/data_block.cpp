/*
 * Copyright (c) 2013-2017, Fabian Greif
 * Copyright (c) 2020, 2023, Jan-Gerd Mess
 * Copyright (c) 2020, Jan Malburg
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "data_block.h"

#include "legall_wavelet.h"
#include "nls_encoder.h"

#include <outpost/base/fixedpoint.h>
#include <outpost/base/slice.h>
#include <outpost/storage/bitfield.h>
#include <outpost/storage/bitstream.h>

namespace outpost
{
namespace compression
{
constexpr size_t DataBlock::headerPadding;
constexpr size_t DataBlock::headerSize;

uint16_t
toUInt(Blocksize bs)
{
    switch (bs)
    {
        case Blocksize::bs16: return 16U;
        case Blocksize::bs128: return 128U;
        case Blocksize::bs256: return 256U;
        case Blocksize::bs512: return 512U;
        case Blocksize::bs1024: return 1024U;
        case Blocksize::bs2048: return 2048U;
        case Blocksize::bs4096: return 4096U;
        case Blocksize::disabled:
        default: return 0U;
    }
}

DataBlock::DataBlock() :
    mSampleCount(0),
    mParameterId(0),
    mStartTime(),
    mSamplingRate(SamplingRate::disabled),
    mBlocksize(Blocksize::disabled),
    mScheme(CompressionScheme::raw),
    mPointer(),
    mSampleBuffer(nullptr),
    mCoefficientBuffer(nullptr),
    mIsTransformed(false),
    mIsEncoded(false)
{
}

// Ignore -Wcast-align here
// because ShareBufferPointers from SharedBufferPools will be 4-byte aligned and headerSize is
// padded.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"

DataBlock::DataBlock(const outpost::container::SharedBufferPointer& p,
                     uint16_t parameterId,
                     outpost::time::GpsTime startTime,
                     SamplingRate rate,
                     Blocksize bs) :
    mSampleCount(0),
    mParameterId(parameterId),
    mStartTime(startTime),
    mSamplingRate(rate),
    mBlocksize(bs),
    mScheme(CompressionScheme::waveletNLS),
    mPointer(p),
    mSampleBuffer(reinterpret_cast<Fixedpoint*>(&p[headerSize])),
    mCoefficientBuffer(reinterpret_cast<int16_t*>(&p[headerSize])),
    mIsTransformed(false),
    mIsEncoded(false)
{
}

#pragma GCC diagnostic pop

size_t
DataBlock::getMaximumSize() const
{
    return mPointer.getLength();
}

outpost::Slice<Fixedpoint>
DataBlock::getSamples() const
{
    if (isTransformed() || isEncoded())
    {
        return outpost::Slice<Fixedpoint>::empty();
    }
    else
    {
        return outpost::Slice<Fixedpoint>::unsafe(mSampleBuffer, mSampleCount);
    }
}

outpost::Slice<int16_t>
DataBlock::getCoefficients() const
{
    if (isTransformed() && !isEncoded())
    {
        return outpost::Slice<int16_t>::unsafe(mCoefficientBuffer, mSampleCount);
    }
    else
    {
        return outpost::Slice<int16_t>::empty();
    }
}

outpost::Slice<uint8_t>
DataBlock::getEncodedData() const
{
    if (isEncoded())
    {
        return outpost::Slice<uint8_t>::unsafe(&mPointer[0U],
                                               headerSize - headerPadding + mSampleCount);
    }
    else
    {
        return outpost::Slice<uint8_t>::empty();
    }
}

bool
DataBlock::applyWaveletTransform()
{
    if (!isTransformed() && !isEncoded() && mSampleCount > 0)
    {
        outpost::Slice<Fixedpoint> samples = this->getSamples();
        LeGall53Wavelet::forwardTransformInPlace(samples);
        LeGall53Wavelet::reorder(samples);
        mIsTransformed = true;
        return true;
    }
    return false;
}

bool
DataBlock::isComplete() const
{
    return mSampleCount > 0 && mSampleCount == toUInt(mBlocksize);
}

bool
DataBlock::isValid() const
{
    return (mPointer.isValid()
            && mPointer.getLength() >= (toUInt(mBlocksize) * sizeof(Fixedpoint) + headerSize));
}

bool
DataBlock::push(Fixedpoint f)
{
    if (!isComplete() && isValid())
    {
        mSampleBuffer[mSampleCount++] = f;
        return true;
    }
    return false;
}

bool
DataBlock::encode(DataBlock& b, NLSEncoder& encoder) const
{
    if (isTransformed() && b.getMaximumSize() >= mSampleCount * sizeof(int16_t))
    {
        outpost::Slice<uint8_t> slice = b.mPointer.asSlice().skipFirst(headerSize - headerPadding);
        outpost::Bitstream bitstream(slice);
        encoder.encode(getCoefficients(), bitstream);
        outpost::Serialize dataStream(slice);
        bitstream.serialize(dataStream);
        b.mSampleCount = bitstream.getSerializedSize();
        b.mIsEncoded = true;
        b.mScheme = CompressionScheme::waveletNLS;

        outpost::Serialize headerStream(&b.mPointer[0U]);
        headerStream.store<uint8_t>(static_cast<uint8_t>(b.mScheme));
        headerStream.store<uint16_t>(mParameterId);
        headerStream.store<uint32_t>(mStartTime.timeSinceEpoch().seconds());
        headerStream.store<uint16_t>(mStartTime.timeSinceEpoch().milliseconds() % 1000U);

        uint8_t* pos = headerStream.getPointerToCurrentPosition();
        outpost::Bitfield::write<0, 3>(pos, static_cast<uint8_t>(mSamplingRate));
        outpost::Bitfield::write<4, 7>(pos, static_cast<uint8_t>(mBlocksize));
        headerStream.skip(1U);

        return true;
    }
    return false;
}

}  // namespace compression
}  // namespace outpost
