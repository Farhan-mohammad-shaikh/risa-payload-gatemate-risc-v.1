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

#include "frame_decoder_hdlc.h"

#include <outpost/coding/hdlc.h>

outpost::transport::FrameDecoderHdlc::FrameDecoderHdlc()
{
}

outpost::transport::FrameDecoderHdlc::~FrameDecoderHdlc()
{
}

outpost::Expected<outpost::Slice<uint8_t>, outpost::transport::OperationResult>
outpost::transport::FrameDecoderHdlc::decode(outpost::Slice<const uint8_t> const& input,
                                             outpost::Slice<uint8_t> const& writeBuffer)
{
    // we need a copy, because
    // decodeOutput is modified in the next function call
    auto decodeOutput = writeBuffer;
    const size_t consumedBytes = outpost::coding::HdlcStuffing::decode(input, decodeOutput);
    OUTPOST_ASSERT(consumedBytes >= decodeOutput.getNumberOfElements(), "unplausible");
    (void) consumedBytes;  // Frame Transport serial checks for boundaries itself

    if (decodeOutput.getNumberOfElements() > 0)
    {
        return decodeOutput;
    }
    return outpost::unexpected(OperationResult::decodingError);
}
