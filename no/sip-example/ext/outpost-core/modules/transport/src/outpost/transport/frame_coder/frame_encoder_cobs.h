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

#ifndef OUTPOST_TRANSPORT_FRAME_ENCODER_COBS_H
#define OUTPOST_TRANSPORT_FRAME_ENCODER_COBS_H

#include <outpost/coding/cobs.h>
#include <outpost/transport/frame_coder/frame_encoder.h>

namespace outpost
{
namespace transport
{
/**
 * Frame encoderusing COBS (Consistent Overhead Byte Stuffing)
 *
 * The generated output byte stream can be passed directly to the UART
 * interface.
 *
 */
class FrameEncoderCobs : public FrameEncoder
{
public:
    FrameEncoderCobs();

    virtual ~FrameEncoderCobs();

    /**
     * Encode an input buffer into the output Buffer.
     *
     * see the parent class \c FrameEncoder.
     *
     * Applies Encoding specific validity checks.
     */
    outpost::Expected<outpost::Slice<uint8_t>, OperationResult>
    encode(outpost::Slice<const uint8_t> const& input,
           outpost::Slice<uint8_t> const& writeBuffer) override;

    /**
     * \brief Alias template for the worst-case sized buffer given the maximum size of the unencoded
     * frame contents
     *
     * \tparam N maximum size of frame contents
     */
    template <size_t N>
    using WorseCaseSizedBuffer =
            std::array<uint8_t, outpost::coding::CobsFrame::getMaximumSizeOfEncodedFrame(N)>;
};

}  // namespace transport
}  // namespace outpost
#endif
