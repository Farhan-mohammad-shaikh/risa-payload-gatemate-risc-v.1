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

#include "frame_encoder_cobs.h"

#include <outpost/coding/cobs.h>

namespace outpost
{
namespace transport
{

FrameEncoderCobs::FrameEncoderCobs()
{
}

FrameEncoderCobs::~FrameEncoderCobs()
{
}

outpost::Expected<outpost::Slice<uint8_t>, OperationResult>
FrameEncoderCobs::encode(outpost::Slice<const uint8_t> const& input,
                         outpost::Slice<uint8_t> const& writeBuffer)
{
    outpost::Slice<uint8_t> returnSlice = writeBuffer;
    const size_t encodedLength = outpost::coding::CobsFrame::encode(input, returnSlice);

    // encodedLength should be bigger than input by at least 2 bytes (overhead, delimiter)
    if (encodedLength <= input.getNumberOfElements())
    {
        return outpost::unexpected(OperationResult::encodingError);
    }

    OUTPOST_ASSERT(encodedLength == returnSlice.getNumberOfElements(), "Inconsistent return");
    return returnSlice;
}

}  // namespace transport
}  // namespace outpost
