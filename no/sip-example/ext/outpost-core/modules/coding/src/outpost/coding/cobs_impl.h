/*
 * Copyright (c) 2014-2017, Fabian Greif
 * Copyright (c) 2020, Christian Hartlage
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2023, Jan-Gerd Mess
 * Copyright (c) 2023, Pascal Pieper
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Kirstein, Hannah
 * Copyright (c) 2024, Passenberg, Felix Constantin
 * Copyright (c) 2024, Cold, Erin Cynthia
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_UTILS_COBS_IMPL_H
#define OUTPOST_UTILS_COBS_IMPL_H

#include "cobs.h"

#include <limits>
#include <string.h>  // for memcpy

#include <cstddef>

namespace outpost
{
namespace coding
{
// ----------------------------------------------------------------------------
template <uint8_t blockLength>
constexpr CobsEncodingGeneratorBase<blockLength>::CobsEncodingGeneratorBase(
        outpost::Slice<const uint8_t> input) :
    mData(input), mCurrentPosition(0), mNextBlock(0), mZeroElementSkip(false)
{
}

template <uint8_t blockLength>
CobsEncodingGeneratorBase<blockLength>::~CobsEncodingGeneratorBase()
{
}

template <uint8_t blockLength>
constexpr CobsEncodingGeneratorBase<blockLength>::CobsEncodingGeneratorBase(
        const CobsEncodingGeneratorBase& other) :
    mData(other.mData),
    mCurrentPosition(other.mCurrentPosition),
    mNextBlock(other.mNextBlock),
    mZeroElementSkip(other.mZeroElementSkip)
{
}

template <uint8_t blockLength>
constexpr CobsEncodingGeneratorBase<blockLength>&
CobsEncodingGeneratorBase<blockLength>::operator=(const CobsEncodingGeneratorBase& other)
{
    // this handles self assignment gracefully
    mData = other.mData;
    mCurrentPosition = other.mCurrentPosition;
    mNextBlock = other.mNextBlock;
    mZeroElementSkip = other.mZeroElementSkip;

    return *this;
}

template <uint8_t blockLength>
constexpr uint8_t
CobsEncodingGeneratorBase<blockLength>::getNextByte()
{
    uint8_t value = 0;
    if (mNextBlock == cobsFrameDelimiter)
    {
        if (mZeroElementSkip)
        {
            // Zero values are replaced with length of the following block
            // and are therefore skipped here. This has to be done before
            // calling findNextBlock() because it starts on the current
            // character.
            mCurrentPosition++;
        }
        else
        {
            mZeroElementSkip = true;
        }
        mNextBlock = findNextBlock();

        if (mNextBlock == blockLength)
        {
            mZeroElementSkip = false;
        }

        value = mNextBlock + 1;
    }
    else
    {
        value = mData[mCurrentPosition];
        mCurrentPosition++;
        mNextBlock--;
    }

    return value;
}

template <uint8_t blockLength>
constexpr uint8_t
CobsEncodingGeneratorBase<blockLength>::findNextBlock() const
{
    uint8_t blockSize = 0;
    // Loop until either:
    // - A zero is found which determines the block length
    // - No zero is found for 254 consecutive bytes
    // - The end of the input array is reached.
    if (mData.getNumberOfElements() > 0)
    {
        size_t position = mCurrentPosition;
        while ((position < mData.getNumberOfElements()) && (mData[position] != cobsFrameDelimiter)
               && (blockSize < blockLength))
        {
            position++;
            blockSize++;
        }
    }

    return blockSize;
}

// ----------------------------------------------------------------------------
template <uint8_t blockLength>
constexpr size_t
CobsBase<blockLength>::encode(outpost::Slice<const uint8_t> input, outpost::Slice<uint8_t>& output)
{
    if (output.getNumberOfElements() < getMaximumSizeOfEncodedData(input.getNumberOfElements()))
    {
        output = output.first(0);
        return 0;
    }

    const uint8_t* inputPtr = input.begin();
    const uint8_t* inputEnd = input.end();
    uint8_t* outputPtr = output.begin();

    // Pointer to the position where later the block length is inserted
    uint8_t* blockLengthPtr = outputPtr++;
    size_t length = 1;
    uint8_t currentBlockLength = 0;

    while ((inputPtr < inputEnd) && (outputPtr < output.end()))
    {
        if (*inputPtr == cobsFrameDelimiter)
        {
            *blockLengthPtr = currentBlockLength + 1;
            blockLengthPtr = outputPtr++;
            length++;
            currentBlockLength = 0;
        }
        else
        {
            *outputPtr++ = *inputPtr;
            length++;
            currentBlockLength++;
            if ((currentBlockLength == blockLength) && (outputPtr < output.end()))
            {
                *blockLengthPtr = currentBlockLength + 1;
                blockLengthPtr = outputPtr++;
                length++;
                currentBlockLength = 0;
            }
        }
        inputPtr++;
    }
    *blockLengthPtr = currentBlockLength + 1;

    output = output.first(length);
    return length;
}

template <uint8_t blockLength>
constexpr size_t
CobsBase<blockLength>::getMaximumSizeOfEncodedData(size_t inputLength)
{
    size_t length = inputLength;
    if (inputLength == 0)
    {
        length = 1;
    }
    else
    {
        length += (inputLength - 1) / blockLength + 1;
    }

    return length;
}

template <uint8_t blockLength>
constexpr size_t
CobsBase<blockLength>::decode(outpost::Slice<const uint8_t> input, uint8_t* output)
{
    Slice out = Slice<uint8_t>::unsafe(output, std::numeric_limits<size_t>::max());
    return decode(input, out);
}

template <uint8_t blockLength>
constexpr size_t
CobsBase<blockLength>::decode(outpost::Slice<const uint8_t> input, Slice<uint8_t>& output)
{
    size_t outputPosition = 0;
    const uint8_t* inputPtr = input.begin();
    const uint8_t* inputEnd = input.end();

    if (getMaximumSizeOfEncodedData(output.getNumberOfElements()) < input.getNumberOfElements())
    {
        // there is most likely not enough space to store the output
        output = output.first(0);
        return 0;
    }

#ifdef OUTPOST_USE_ASSERT
    if (std::find(input.begin(), input.end(), cobsFrameDelimiter) != input.end())
    {
        OUTPOST_ASSERT(false, "cobs data stream contained a zero");
        output = output.first(0);
        return 0;
    }
#endif  // USE_OUTPOST_ASSERT

    while (inputPtr < inputEnd)
    {
        uint8_t data = *inputPtr++;
        if (data == cobsFrameDelimiter)
        {
            outputPosition = 0;
            inputPtr = inputEnd;
        }
        else
        {
            uint8_t currentBlockLength = data - 1;

            if (inputPtr + currentBlockLength > inputEnd)
            {
                // Prevent moving from outside the input range.
                // Output size was already checked before the while loop.
                outputPosition = 0;
                break;
            }

            // memmove instead of memcpy is needed here because the input and output
            // array may overlap.
            memmove(&output[outputPosition], inputPtr, currentBlockLength);
            outputPosition += currentBlockLength;
            inputPtr += currentBlockLength;

            if (currentBlockLength < blockLength)
            {
                // The last (implicit) zero is suppressed and not output.
                if (inputPtr < inputEnd)
                {
                    output[outputPosition] = cobsFrameDelimiter;
                    outputPosition++;
                }
            }
        }
    }

    output = output.first(outputPosition);
    return outputPosition;
}

constexpr size_t
CobsFrame::getMaximumSizeOfEncodedFrame(size_t inputLength)
{
    // Maximum size + one for framing byte
    return Cobs::getMaximumSizeOfEncodedData(inputLength) + 1;
}

constexpr size_t
CobsFrame::decode(Slice<const uint8_t>& input, Slice<uint8_t>& output)
{
    auto frameEnd = input.begin();
    while (frameEnd != input.end())
    {
        if (*frameEnd == cobsFrameDelimiter)
        {
            break;
        }
        frameEnd++;
    }

    if (frameEnd == input.end())
    {
        // No end marker found. Not a full frame. Abort.
        output = output.first(0);
        return 0;
    }

    if (frameEnd == input.begin())
    {
        // The first byte is the terminator.
        // So its probably the end.
        // Report to discard this.
        output = output.first(0);
        input = input.skipFirst(1);
        return 1;
    }

    // determine the length to decode
    size_t const terminatorIndex = std::distance(input.begin(), frameEnd);
    size_t const length = terminatorIndex + 1;

    // validity check
    bool valid = false;
    size_t testPos = 0;
    while (testPos < terminatorIndex && testPos < input.getNumberOfElements())
    {
        if (input[testPos] == cobsFrameDelimiter)
        {
            OUTPOST_ASSERT(false, "std::find did not found the first delimiter");
            break;
        }
        testPos += input[testPos];
    }
    // Only if the sums of all redirect to zero bytes ends at the terminating zero the frame may be
    // valid. If the end is shorter, we have found a zero in the data stream. If the end is
    // larger, std::find did not find the first delimiter (most unlikely),
    if (testPos == terminatorIndex)
    {
        valid = true;
    }

    if (!valid)
    {
        // no valid frame
        output = output.first(0);
        // but skip until the first delimiter
        input = input.skipFirst(length);
        return length;
    }

    // decode the frame (without the terminator byte)
    size_t outputPos = Cobs::decode(input.first(terminatorIndex), output);

    // Truncate the first data, so the remaining can be used for the next call.
    input = input.skipFirst(length);
    // Output is already truncated by the decode call.
    OUTPOST_ASSERT(output.getNumberOfElements() == outputPos, "COBS decode lengths unexpected");
    static_cast<void>(outputPos);  // prevent warning when not checked;

    return length;
}

constexpr size_t
CobsFrame::encode(Slice<const uint8_t> const& input, Slice<uint8_t>& output)
{
    // check encoding and framing lengths requirements
    if (output.getNumberOfElements() < getMaximumSizeOfEncodedFrame(input.getNumberOfElements()))
    {
        output = output.first(0);
        return 0;
    }

    // As encode truncates the output to fit without boundary Bytes.
    const Slice<uint8_t> originalOutputSlice = output;
    Slice<uint8_t> fittedOutput = output;

    const size_t encodedLength = Cobs::encode(input, fittedOutput);

    // Add the end frame marker.
    originalOutputSlice[encodedLength] = cobsFrameDelimiter;

    // Includes added end frame marker
    output = originalOutputSlice.first(encodedLength + 1);

    return output.getNumberOfElements();
}

}  // namespace coding
}  // namespace outpost

#endif
