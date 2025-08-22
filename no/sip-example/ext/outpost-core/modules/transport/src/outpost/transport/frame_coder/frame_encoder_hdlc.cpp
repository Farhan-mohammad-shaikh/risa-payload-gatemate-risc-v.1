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

#include "frame_encoder_hdlc.h"

#include <outpost/coding/hdlc.h>

outpost::transport::FrameEncoderHdlc::FrameEncoderHdlc()
{
}

outpost::transport::FrameEncoderHdlc::~FrameEncoderHdlc()
{
}

outpost::Expected<outpost::Slice<uint8_t>, outpost::transport::OperationResult>
outpost::transport::FrameEncoderHdlc::encode(outpost::Slice<const uint8_t> const& input,
                                             outpost::Slice<uint8_t> const& writeBuffer)
{
    outpost::Slice<uint8_t> returnSlice = writeBuffer;
    const size_t encodedLength = outpost::coding::HdlcStuffing::encode(input, returnSlice);

    // encodedLength should be bigger as input
    // at least 2 bytes (0x7E, boundary)
    if (encodedLength <= input.getNumberOfElements())
    {
        return outpost::unexpected(OperationResult::encodingError);
    }

    OUTPOST_ASSERT(encodedLength == returnSlice.getNumberOfElements(), "Inconsistent return");
    return returnSlice;
}
