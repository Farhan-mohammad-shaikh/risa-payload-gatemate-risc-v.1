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

#ifndef OUTPOST_TRANSPORT_FRAME_DECODER_COBS_H
#define OUTPOST_TRANSPORT_FRAME_DECODER_COBS_H

#include <outpost/coding/cobs.h>
#include <outpost/transport/frame_coder/frame_decoder.h>

namespace outpost
{
namespace transport
{
/**
 * Frame decoder/decoder using COBS (Consistent Overhead Byte Stuffing)
 *
 * The generated output byte stream can be passed directly to the UART
 * interface.
 *
 */
class FrameDecoderCobs : public FrameDecoder
{
public:
    FrameDecoderCobs();

    virtual ~FrameDecoderCobs();

    /**
     * Decode a buffer to a TRANSPORT Frame.
     *
     * see the parent class \c FrameDecoder.
     *
     * Applies Decoding specific validity checks.
     */
    outpost::Expected<outpost::Slice<uint8_t>, OperationResult>
    decode(outpost::Slice<const uint8_t> const& input,
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

private:
    static const uint8_t flagByte = outpost::coding::cobsFrameDelimiter;
};

}  // namespace transport
}  // namespace outpost
#endif
