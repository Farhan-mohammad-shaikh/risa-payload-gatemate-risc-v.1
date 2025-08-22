/*
 * Copyright (c) 2024, Cold, Erin Cynthia
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "buffered_frame_decoder_hdlc.h"

#include <outpost/coding/hdlc.h>

namespace outpost
{
namespace transport
{
BufferedFrameDecoderHdlc::BufferedFrameDecoderHdlc(outpost::Slice<uint8_t> receiveBuffer) :
    mReceiveBuffer(receiveBuffer),
    mReceivedData(outpost::asSlice(mReceiveBuffer)),
    mBackToBackFrame(false),
    mInFrame(false)
{
}

BufferedFrameDecoderHdlc::~BufferedFrameDecoderHdlc()
{
}

outpost::Expected<outpost::Slice<uint8_t>, outpost::transport::OperationResult>
BufferedFrameDecoderHdlc::bufferedDecode(const uint8_t input,
                                         outpost::Slice<uint8_t> const& writeBuffer)
{
    if (mReceivedData.asSlice().getNumberOfElements() == mReceiveBuffer.getNumberOfElements())
    {
        return outpost::unexpected(OperationResult::bufferError);
    }

    if (mBackToBackFrame)
    {
        if (input != flagByte)
        {
            mReceivedData.store(input);
        }
        mInFrame = true;
        mBackToBackFrame = false;
    }
    else if (mInFrame)
    {
        mReceivedData.store(input);
        if (input == flagByte)
        {
            // non const version
            auto decodeOutput = writeBuffer;
            const size_t consumedBytes =
                    outpost::coding::HdlcStuffing::decode(mReceivedData.asSlice(), decodeOutput);
            OUTPOST_ASSERT(consumedBytes >= decodeOutput.getNumberOfElements(), "unplausible");
            (void) consumedBytes;

            reset();

            if (decodeOutput.getNumberOfElements() > 0)
            {
                // In back to back frames, the ending flag byte of the previous frame can count as
                // the starting flag of this frame, so we will insert the starting flag
                mReceivedData.store(flagByte);
                mBackToBackFrame = true;
                mInFrame = true;
                return decodeOutput;
            }
            return outpost::unexpected(OperationResult::decodingError);
        }
    }
    else if (input == flagByte)
    {
        mReceivedData.store(input);
        mInFrame = true;
    }
    else
    {
        // non flag bytes that arrive outside a frame are considered garbage and are not stored.
    }
    return outpost::unexpected(OperationResult::notComplete);
}

void
BufferedFrameDecoderHdlc::reset()
{
    mReceivedData.reset();
    mBackToBackFrame = false;
    mInFrame = false;
}

}  // namespace transport
}  // namespace outpost
