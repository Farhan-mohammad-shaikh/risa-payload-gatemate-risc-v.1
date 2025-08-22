/*
 * Copyright (c) 2021, toth_no
 * Copyright (c) 2023-2024, Felix Passenberg
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Cold, Erin Cynthia
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_TRANSPORT_FRAME_DECODER_HDLC_H
#define OUTPOST_TRANSPORT_FRAME_DECODER_HDLC_H

#include <outpost/coding/hdlc.h>
#include <outpost/transport/frame_coder/frame_decoder.h>

namespace outpost
{
namespace transport
{
/**
 * Frame decoder/decoder using HDLC byte stuffing
 *
 * The generated output byte stream can be passed directly to the UART
 * interface.
 *
 */
class FrameDecoderHdlc : public FrameDecoder
{
public:
    FrameDecoderHdlc();

    virtual ~FrameDecoderHdlc();

    /**
     * Decode a HDLC encoded frame into the output buffer.
     *
     * see the parent class \c FrameDecoder.
     *
     * Applies Decoding specific validity checks.
     */
    Expected<Slice<uint8_t>, OperationResult>
    decode(Slice<const uint8_t> const& input, outpost::Slice<uint8_t> const& writeBuffer) override;

    /**
     * \brief Alias template for the worst-case sized buffer given the maximum size of the unencoded
     * frame contents
     *
     * \tparam N maximum size of frame contents
     */
    template <size_t N>
    using WorseCaseSizedBuffer =
            std::array<uint8_t, 2 * N + outpost::coding::HdlcStuffing::boundary_overhead>;

private:
    static const uint8_t flagByte = coding::HdlcStuffing::boundary_byte;
};

}  // namespace transport
}  // namespace outpost
#endif
