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

#include "buffered_frame_decoder_cobs.h"

#include <outpost/coding/cobs.h>

namespace outpost
{
namespace transport
{
BufferedFrameDecoderCobs::BufferedFrameDecoderCobs(outpost::Slice<uint8_t> receiveBuffer) :
    mReceiveBuffer(receiveBuffer), mReceivedData(mReceiveBuffer)
{
}

BufferedFrameDecoderCobs::~BufferedFrameDecoderCobs()
{
}

outpost::Expected<outpost::Slice<uint8_t>, outpost::transport::OperationResult>
BufferedFrameDecoderCobs::bufferedDecode(const uint8_t input,
                                         outpost::Slice<uint8_t> const& writeBuffer)
{
    if (mReceivedData.asSlice().getNumberOfElements() == mReceiveBuffer.getNumberOfElements())
    {
        return outpost::unexpected(OperationResult::bufferError);
    }

    mReceivedData.store(input);

    if (input == flagByte)
    {
        outpost::Slice<const uint8_t> inputBytes(mReceivedData.asSlice());
        outpost::Slice<uint8_t> decodeOutput = writeBuffer;
        const size_t consumedBytes = outpost::coding::CobsFrame::decode(inputBytes, decodeOutput);
        OUTPOST_ASSERT(consumedBytes >= decodeOutput.getNumberOfElements(), "unplausible");
        (void) consumedBytes;

        reset();

        if (decodeOutput.getNumberOfElements() > 0)
        {
            return decodeOutput;
        }
        return outpost::unexpected(OperationResult::decodingError);
    }
    return outpost::unexpected(OperationResult::notComplete);
}

void
BufferedFrameDecoderCobs::reset()
{
    mReceivedData.reset();
}

}  // namespace transport
}  // namespace outpost
