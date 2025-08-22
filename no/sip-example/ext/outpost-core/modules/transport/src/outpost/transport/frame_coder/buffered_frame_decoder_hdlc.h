/*
 * Copyright (c) 2024, Cold, Erin Cynthia
 * Copyright (c) 2024, cold_ei
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_TRANSPORT_BUFFERED_FRAME_DECODER_HDLC_H
#define OUTPOST_TRANSPORT_BUFFERED_FRAME_DECODER_HDLC_H

#include <outpost/coding/hdlc.h>
#include <outpost/storage/serialize.h>
#include <outpost/transport/frame_coder/buffered_frame_decoder.h>

namespace outpost
{
namespace transport
{
/**
 * Buffered frame decoder using HDLC byte stuffing
 *
 */
class BufferedFrameDecoderHdlc : public BufferedFrameDecoder
{
public:
    BufferedFrameDecoderHdlc(outpost::Slice<uint8_t> receiveBuffer);

    BufferedFrameDecoderHdlc(const BufferedFrameDecoderHdlc&) = delete;

    BufferedFrameDecoderHdlc&
    operator=(const BufferedFrameDecoderHdlc&) = delete;

    virtual ~BufferedFrameDecoderHdlc();

    /**
     * Decode a HDLC encoded frame into the output buffer.
     *
     * see the parent class \c BufferedFrameDecoder.
     *
     * Applies Decoding specific validity checks.
     */
    Expected<Slice<uint8_t>, OperationResult>
    bufferedDecode(const uint8_t input, const outpost::Slice<uint8_t>& writeBuffer) override;

    /**
     * Resets state and clears buffer
     *
     * see the parent class \c BufferedFrameDecoder.
     */
    void
    reset() override;

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
    static constexpr uint8_t flagByte = outpost::coding::HdlcStuffing::boundary_byte;

    outpost::Slice<uint8_t> mReceiveBuffer;
    outpost::Serialize mReceivedData;
    bool mBackToBackFrame;
    bool mInFrame;
};

}  // namespace transport
}  // namespace outpost
#endif
