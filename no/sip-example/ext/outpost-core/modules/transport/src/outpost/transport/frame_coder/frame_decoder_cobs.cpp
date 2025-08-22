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

#include "frame_decoder_cobs.h"

#include <outpost/coding/cobs.h>

namespace outpost
{
namespace transport
{

FrameDecoderCobs::FrameDecoderCobs()
{
}

FrameDecoderCobs::~FrameDecoderCobs()
{
}

outpost::Expected<outpost::Slice<uint8_t>, OperationResult>
FrameDecoderCobs::decode(outpost::Slice<const uint8_t> const& input,
                         outpost::Slice<uint8_t> const& writeBuffer)
{
    // non const copies
    outpost::Slice<const uint8_t> remainingInput = input;
    outpost::Slice<uint8_t> decodeOutput = writeBuffer;

    const size_t consumedBytes = outpost::coding::CobsFrame::decode(remainingInput, decodeOutput);
    OUTPOST_ASSERT(consumedBytes >= decodeOutput.getNumberOfElements(), "unplausible");
    (void) consumedBytes;

    if (decodeOutput.getNumberOfElements() > 0)
    {
        return decodeOutput;
    }
    return outpost::unexpected(OperationResult::decodingError);
}

}  // namespace transport
}  // namespace outpost
